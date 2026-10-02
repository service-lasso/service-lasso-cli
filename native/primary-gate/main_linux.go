//go:build linux

package main

import (
	"fmt"
	"net"
	"os"
	"os/exec"
	"sync"

	"golang.org/x/sys/unix"
)

// Linux keeps both executable identities in sealed anonymous memfds.  The
// descriptor survives the child start and /proc/self/fd resolves the already
// opened kernel object, never an archive neighbour or temporary filename.
func sealedImage(name string, value []byte) (int, error) {
	fd, err := unix.MemfdCreate(name, unix.MFD_CLOEXEC|unix.MFD_ALLOW_SEALING)
	if err != nil {
		return 0, err
	}
	for written := 0; written < len(value); {
		count, writeErr := unix.Write(fd, value[written:])
		if writeErr != nil || count == 0 {
			unix.Close(fd)
			return 0, fmt.Errorf("image write failed")
		}
		written += count
	}
	if _, err = unix.FcntlInt(uintptr(fd), unix.F_ADD_SEALS, unix.F_SEAL_SEAL|unix.F_SEAL_SHRINK|unix.F_SEAL_GROW|unix.F_SEAL_WRITE); err != nil {
		unix.Close(fd)
		return 0, err
	}
	return fd, nil
}

func main() {
	if !newGateCapability() { fail() }
	sea, err := sealedImage("service-lassoctl-sea", seaBytes)
	if err != nil {
		fail()
	}
	defer unix.Close(sea)
	helper, err := sealedImage("service-lasso-confined-writer", confinedWriterBytes)
	if err != nil {
		fail()
	}
	defer unix.Close(helper)
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
	// Pass the held descriptor path, rather than an extracted writer path. The
	// IPC process resolves this process's sealed descriptor on every request.
	heldHelper = os.NewFile(uintptr(helper), "service-lasso-confined-writer")
	heldExecutionPath = "/proc/self/fd/3"
	// The SEA receives one end of a connected socketpair as fd 3.  There is no
	// filesystem or namespace endpoint to discover, replace, or pre-bind.
	// The retained end is the capability boundary; a PID is never authority.
	child := exec.Command(fmt.Sprintf("/proc/self/fd/%d", sea), os.Args[1:]...)
	child.ExtraFiles = []*os.File{clientFile}
	child.Stdin, child.Stdout, child.Stderr = os.Stdin, os.Stdout, os.Stderr
	child.Env = gateEnvironment(os.Environ(), "linux-v3", "", 3)
	if err = child.Start(); err != nil {
		fail()
	}
	// A pidfd is a held kernel identity, not an integer that may be recycled.
	pidfd, err := unix.PidfdOpen(child.Process.Pid, 0)
	if err != nil {
		fail()
	}
	defer unix.Close(pidfd)
	_ = clientFile.Close()
	lifecycle := &linuxLifecycle{pidfd: pidfd}
	exit := make(chan error, 1)
	go func() { err := child.Wait(); lifecycle.terminate(); exit <- err }()
	go serveLinux(server, lifecycle)
	if err = <-exit; err != nil {
		if exit, ok := err.(*exec.ExitError); ok {
			os.Exit(exit.ExitCode())
		}
		fail()
	}
}

func fail() { fmt.Fprintln(os.Stderr, "The native primary gate could not start."); os.Exit(1) }

type linuxLifecycle struct {
	mu         sync.Mutex
	pidfd      int
	terminated bool
}

func (l *linuxLifecycle) terminate() { l.mu.Lock(); l.terminated = true; l.mu.Unlock() }
func (l *linuxLifecycle) admit(connection net.Conn) {
	l.mu.Lock()
	defer l.mu.Unlock()
	if l.terminated {
		_ = connection.Close()
		return
	}
	ready, err := unix.Poll([]unix.PollFd{{Fd: int32(l.pidfd), Events: unix.POLLIN}}, 0)
	if err != nil || ready != 0 {
		_ = connection.Close()
		return
	}
	materialize(connection)
}
func serveLinux(connection net.Conn, lifecycle *linuxLifecycle) {
	lifecycle.admit(connection)
}
