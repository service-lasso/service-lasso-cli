//go:build linux

package main

import (
	"fmt"
	"net"
	"os"
	"os/exec"
	"path/filepath"

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
	directory, err := os.MkdirTemp("", "service-lasso-primary-")
	if err != nil {
		fail()
	}
	defer os.RemoveAll(directory)
	endpoint := filepath.Join(directory, "primary.sock")
	listener, err := net.Listen("unix", endpoint)
	if err != nil {
		fail()
	}
	defer listener.Close()
	// Pass the held descriptor path, rather than an extracted writer path. The
	// IPC process resolves this process's sealed descriptor on every request.
	heldHelper = os.NewFile(uintptr(helper), "service-lasso-confined-writer")
	go serve(listener, "@held")
	child := exec.Command(fmt.Sprintf("/proc/self/fd/%d", sea), os.Args[1:]...)
	child.Stdin, child.Stdout, child.Stderr = os.Stdin, os.Stdout, os.Stderr
	child.Env = append(os.Environ(), "SERVICE_LASSO_PRIMARY_GATE=linux-v1", "SERVICE_LASSO_PRIMARY_GATE_PIPE="+endpoint)
	if err = child.Run(); err != nil {
		if exit, ok := err.(*exec.ExitError); ok {
			os.Exit(exit.ExitCode())
		}
		fail()
	}
}

func fail() { fmt.Fprintln(os.Stderr, "The native primary gate could not start."); os.Exit(1) }
func serve(listener net.Listener, helper string) {
	for {
		connection, err := listener.Accept()
		if err != nil {
			return
		}
		go materialize(connection, helper)
	}
}
