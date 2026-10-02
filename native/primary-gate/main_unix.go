//go:build darwin

package main

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"net"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"sync"

	"golang.org/x/sys/unix"
)

// Darwin does not offer Linux's sealed memfd execution. The provisioned helper
// accepts a held descriptor and its device/inode identity only; it never opens
// a caller path or exposes generic chflags authority. Its own bytes and parent
// must already be root-owned and non-writable by this job before sudo exec.
func darwinHelper() (string, error) {
	path := os.Getenv("SERVICE_LASSO_DARWIN_PRIVILEGED_HELPER")
	if path == "" || !filepath.IsAbs(path) || filepath.Clean(path) != path {
		return "", fmt.Errorf("Darwin privileged helper is not provisioned")
	}
	var file, parent unix.Stat_t
	if err := unix.Lstat(path, &file); err != nil || file.Mode&unix.S_IFMT != unix.S_IFREG || file.Uid != 0 || file.Mode&0022 != 0 {
		return "", fmt.Errorf("Darwin privileged helper is not protected")
	}
	if err := unix.Lstat(filepath.Dir(path), &parent); err != nil || parent.Mode&unix.S_IFMT != unix.S_IFDIR || parent.Uid != 0 || parent.Mode&0022 != 0 {
		return "", fmt.Errorf("Darwin privileged helper parent is not protected")
	}
	bytes, err := os.ReadFile(path)
	digest := sha256.Sum256(bytes)
	if err != nil || !strings.EqualFold(strings.TrimSpace(darwinHelperSHA256), hex.EncodeToString(digest[:])) {
		return "", fmt.Errorf("Darwin privileged helper binary does not match the embedded source-built digest")
	}
	return path, nil
}

func setImmutable(fd int, mode string) error {
	helper, err := darwinHelper()
	if err != nil { return err }
	capabilityFD, err := strconv.Atoi(os.Getenv("SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD"))
	if err != nil || capabilityFD < 3 { return fmt.Errorf("Darwin immutable capability is not inherited") }
	capability := os.NewFile(uintptr(capabilityFD), "service-lasso-darwin-immutability-capability")
	if capability == nil { return fmt.Errorf("Darwin immutable capability is unavailable") }
	var stat unix.Stat_t
	if err = unix.Fstat(fd, &stat); err != nil || stat.Uid != uint32(os.Getuid()) { return fmt.Errorf("Darwin image is not job-owned") }
	if qualificationOwner == nil { return fmt.Errorf("Darwin owner coordination is unavailable") }
	request, err := qualificationOwner.begin(fd, mode)
	if err != nil { return err }
	succeeded := false
	defer func() { if !succeeded { _ = qualificationOwner.complete(request, false) } }()
	// The policy-bound capability and image descriptor are the only preserved
	// descriptors. A runner without externally provisioned authority fails.
	command := exec.Command("/usr/bin/sudo", "-n", "-C", "5", "--", helper, "--fd", "3", "--capability-fd", "4", "--device", strconv.FormatUint(uint64(stat.Dev), 10), "--inode", strconv.FormatUint(stat.Ino, 10), "--mode", mode)
	command.ExtraFiles = []*os.File{os.NewFile(uintptr(fd), "service-lasso-owned-image"), capability}
	if err = command.Run(); err != nil { return err }
	if err = unix.Fstat(fd, &stat); err != nil { return err }
	if mode == "set" && stat.Flags&unix.SF_IMMUTABLE == 0 { return fmt.Errorf("system immutable readback failed") }
	if mode == "clear" && stat.Flags&unix.SF_IMMUTABLE != 0 { return fmt.Errorf("system immutable clear readback failed") }
	succeeded = true
	return qualificationOwner.complete(request, true)
}

func immutable(fd int) error {
	if err := setImmutable(fd, "set"); err != nil { return err }
	var stat unix.Stat_t
	if err := unix.Fstat(fd, &stat); err != nil || stat.Flags&unix.SF_IMMUTABLE == 0 {
		return fmt.Errorf("system immutable readback failed")
	}
	return nil
}

func releaseImmutable(fd int) error {
	return setImmutable(fd, "clear")
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

func verifyImmutableImage(writer, reader int, expected []byte) error {
	if err := immutable(reader); err != nil {
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
	if !newGateCapability() { fail() }
	if _, err := darwinHelper(); err != nil { fail("external-helper") }
	capabilityFD, err := strconv.Atoi(os.Getenv("SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD"))
	var capabilityStat unix.Stat_t
	if err != nil || capabilityFD < 3 || unix.Fstat(capabilityFD, &capabilityStat) != nil { fail("external-capability") }
	qualificationOwner, err = connectDarwinOwner()
	if err != nil { fail("external-owner") }
	defer qualificationOwner.connection.Close()
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
	if verifyImmutableImage(seaWriter, seaReader, seaBytes) != nil { fail("sea-immutability") }
	if verifyImmutableImage(helperWriter, helperReader, confinedWriterBytes) != nil { fail("writer-immutability") }
	if immutable(directoryFD) != nil { fail("parent-immutability") }
	defer unix.Close(directoryFD)
	defer func() {
		// A failed release retains the protected object for recovery instead of
		// recursively deleting a target whose ownership cannot be proved.
		if releaseImmutable(seaReader) == nil && releaseImmutable(helperReader) == nil && releaseImmutable(directoryFD) == nil {
			_ = os.RemoveAll(directory)
		}
	}()
	// Resolve only from the held immutable directory descriptor. The SEA and
	// writer leaf names cannot be substituted after the flag readback.
	// ipc.go maps its sole ExtraFiles entry to fd 3 for the writer.  Preserve
	// the held immutable directory, never a pathname or an unmapped fd 4.
	heldHelper = os.NewFile(uintptr(directoryFD), "service-lasso-immutable-directory")
	heldExecutionPath = "/dev/fd/3/service-lasso-confined-scaffold"
	directoryFile := os.NewFile(uintptr(directoryFD), "service-lasso-immutable-directory")
	defer directoryFile.Close()
	child := exec.Command("/dev/fd/4/service-lassoctl.sea", os.Args[1:]...)
	child.ExtraFiles = []*os.File{clientFile, directoryFile}
	child.Stdin, child.Stdout, child.Stderr = os.Stdin, os.Stdout, os.Stderr
	child.Env = gateEnvironment(os.Environ(), "darwin-v4", "", 3)
	if err := child.Start(); err != nil {
		fail()
	}
	_ = clientFile.Close()
	lifecycle, err := newDarwinLifecycle(child.Process.Pid)
	if err != nil { fail() }
	defer unix.Close(lifecycle.kqueue)
	exit := make(chan error, 1)
	go func() { err := child.Wait(); lifecycle.terminate(); exit <- err }()
	go serveDarwin(server, lifecycle)
	if err := <-exit; err != nil {
		if exit, ok := err.(*exec.ExitError); ok {
			os.Exit(exit.ExitCode())
		}
		fail()
	}
}

func fail(stages ...string) {
	// Fixed stage labels only: never print paths, capabilities or helper errors.
	stage := "startup"
	if len(stages) == 1 { stage = stages[0] }
	fmt.Fprintf(os.Stderr, "The native primary gate could not start (stage=%s).\n", stage)
	os.Exit(1)
}

type darwinLifecycle struct {
	mu         sync.Mutex
	kqueue     int
	terminated bool
}

func newDarwinLifecycle(pid int) (*darwinLifecycle, error) {
	queue, err := unix.Kqueue()
	if err != nil { return nil, err }
	change := unix.Kevent_t{Ident: uint64(pid), Filter: unix.EVFILT_PROC, Flags: unix.EV_ADD | unix.EV_ONESHOT, Fflags: unix.NOTE_EXIT}
	if _, err = unix.Kevent(queue, []unix.Kevent_t{change}, nil, nil); err != nil { unix.Close(queue); return nil, err }
	return &darwinLifecycle{kqueue: queue}, nil
}
func (l *darwinLifecycle) terminate() { l.mu.Lock(); l.terminated = true; l.mu.Unlock() }
func (l *darwinLifecycle) exited() bool {
	events := make([]unix.Kevent_t, 1)
	timeout := unix.Timespec{}
	count, err := unix.Kevent(l.kqueue, nil, events, &timeout)
	return err != nil || count != 0
}
func (l *darwinLifecycle) admit(connection net.Conn) {
	l.mu.Lock()
	defer l.mu.Unlock()
	// kqueue is registered for this child process before the endpoint is
	// offered. It observes the original process exit without consulting a
	// reusable PID or an asynchronous boolean alone.
	if l.terminated || l.exited() {
		l.terminated = true
		_ = connection.Close()
		return
	}
	materialize(connection)
}
func serveDarwin(connection net.Conn, lifecycle *darwinLifecycle) {
	lifecycle.admit(connection)
}
