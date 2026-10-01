//go:build darwin

package main

import (
	"crypto/sha256"
	"fmt"
	"net"
	"os"
	"os/exec"
	"path/filepath"
	"sync"

	"golang.org/x/sys/unix"
)

// Darwin does not offer Linux's sealed memfd execution. A read descriptor or
// unlink is insufficient because another same-identity process can retain a
// writable descriptor. The system immutable flag is required; unavailable
// non-interactive privilege is a closed launch failure, never a downgrade.
func immutable(fd int, path string) error {
	command := exec.Command("/usr/bin/sudo", "-n", "/usr/bin/chflags", "schg", path)
	if err := command.Run(); err != nil {
		return err
	}
	var stat unix.Stat_t
	if err := unix.Fstat(fd, &stat); err != nil || stat.Flags&unix.SF_IMMUTABLE == 0 {
		return fmt.Errorf("system immutable readback failed")
	}
	return nil
}

func releaseImmutable(path string) error {
	return exec.Command("/usr/bin/sudo", "-n", "/usr/bin/chflags", "noschg", path).Run()
}

func writeImage(path string, value []byte) (int, int, error) {
	writer, err := unix.Open(path, unix.O_WRONLY|unix.O_CREAT|unix.O_EXCL|unix.O_NOFOLLOW, 0700)
	if err != nil {
		return 0, 0, err
	}
	for written := 0; written < len(value); {
		count, writeErr := unix.Write(writer, value[written:])
		if writeErr != nil || count == 0 {
			_ = unix.Close(writer)
			return 0, 0, fmt.Errorf("image write failed")
		}
		written += count
	}
	if err = unix.Fsync(writer); err != nil {
		_ = unix.Close(writer)
		return 0, 0, err
	}
	reader, err := unix.Open(path, unix.O_RDONLY|unix.O_NOFOLLOW, 0)
	if err != nil {
		_ = unix.Close(writer)
		return 0, 0, err
	}
	return writer, reader, nil
}

func verifyImmutableImage(writer, reader int, path string, expected []byte) error {
	if err := immutable(reader, path); err != nil {
		return err
	}
	// A later writable open is insufficient proof: the pre-open writer must
	// also be rejected after activation.
	if _, err := unix.Pwrite(writer, []byte{0}, 0); err == nil {
		return fmt.Errorf("system immutable existing writer accepted write")
	}
	bytes := make([]byte, len(expected))
	for read := 0; read < len(bytes); {
		count, readErr := unix.Pread(reader, bytes[read:], int64(read))
		if readErr != nil || count == 0 {
			return fmt.Errorf("immutable image read failed")
		}
		read += count
	}
	if sha256.Sum256(bytes) != sha256.Sum256(expected) {
		return fmt.Errorf("immutable image digest mismatch")
	}
	return nil
}

func main() {
	directory, err := os.MkdirTemp("", "service-lasso-primary-")
	if err != nil {
		fail()
	}
	seaPath := filepath.Join(directory, "service-lassoctl.sea")
	helperPath := filepath.Join(directory, "service-lasso-confined-scaffold")
	seaWriter, seaReader, err := writeImage(seaPath, seaBytes)
	if err != nil {
		fail()
	}
	defer unix.Close(seaWriter)
	defer unix.Close(seaReader)
	helperWriter, helperReader, err := writeImage(helperPath, confinedWriterBytes)
	if err != nil {
		fail()
	}
	defer unix.Close(helperWriter)
	defer unix.Close(helperReader)
	directoryFD, err := unix.Open(directory, unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW, 0)
	if err != nil {
		fail()
	}
	pair, err := unix.Socketpair(unix.AF_UNIX, unix.SOCK_STREAM, 0)
	if err != nil {
		fail()
	}
	unix.CloseOnExec(pair[0])
	unix.CloseOnExec(pair[1])
	serverFile := os.NewFile(uintptr(pair[0]), "service-lasso-primary-server")
	clientFile := os.NewFile(uintptr(pair[1]), "service-lasso-primary-client")
	defer serverFile.Close()
	server, err := net.FileConn(serverFile)
	if err != nil {
		fail()
	}
	defer server.Close()
	// Freeze both images and their parent before publishing the held directory
	// descriptor to the child. The request endpoint is the separate inherited
	// socketpair and never appears in this mutable filesystem namespace.
	if verifyImmutableImage(seaWriter, seaReader, seaPath, seaBytes) != nil || verifyImmutableImage(helperWriter, helperReader, helperPath, confinedWriterBytes) != nil || immutable(directoryFD, directory) != nil {
		fail()
	}
	defer unix.Close(directoryFD)
	defer func() {
		// A failed release retains the protected object for recovery instead of
		// recursively deleting a target whose ownership cannot be proved.
		if releaseImmutable(seaPath) == nil && releaseImmutable(helperPath) == nil && releaseImmutable(directory) == nil {
			_ = os.RemoveAll(directory)
		}
	}()
	// Resolve only from the held immutable directory descriptor. The SEA and
	// writer leaf names cannot be substituted after the flag readback.
	heldHelper = os.NewFile(uintptr(directoryFD), "service-lasso-immutable-directory")
	heldExecutionPath = "/dev/fd/4/service-lasso-confined-scaffold"
	directoryFile := os.NewFile(uintptr(directoryFD), "service-lasso-immutable-directory")
	child := exec.Command("/dev/fd/4/service-lassoctl.sea", os.Args[1:]...)
	child.ExtraFiles = []*os.File{clientFile, directoryFile}
	child.Stdin, child.Stdout, child.Stderr = os.Stdin, os.Stdout, os.Stderr
	child.Env = append(os.Environ(), "SERVICE_LASSO_PRIMARY_GATE=darwin-v3", "SERVICE_LASSO_PRIMARY_GATE_FD=3")
	if err := child.Start(); err != nil {
		fail()
	}
	_ = clientFile.Close()
	lifecycle := &darwinLifecycle{}
	exit := make(chan error, 1)
	go func() { err := child.Wait(); lifecycle.terminate(); exit <- err }()
	go serveDarwin(server, "@held", lifecycle)
	if err := <-exit; err != nil {
		if exit, ok := err.(*exec.ExitError); ok {
			os.Exit(exit.ExitCode())
		}
		fail()
	}
}

func fail() { fmt.Fprintln(os.Stderr, "The native primary gate could not start."); os.Exit(1) }

type darwinLifecycle struct {
	mu         sync.Mutex
	terminated bool
}

func (l *darwinLifecycle) terminate() { l.mu.Lock(); l.terminated = true; l.mu.Unlock() }
func (l *darwinLifecycle) admit(connection net.Conn, helper string) {
	l.mu.Lock()
	defer l.mu.Unlock()
	if l.terminated {
		_ = connection.Close()
		return
	}
	materialize(connection, helper)
}
func serveDarwin(connection net.Conn, helper string, lifecycle *darwinLifecycle) {
	lifecycle.admit(connection, helper)
}
