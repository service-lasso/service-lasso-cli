//go:build !windows

package main

import (
	"fmt"
	"net"
	"os"
	"os/exec"
	"path/filepath"
)

// Native descriptor-preserving launch implementations are deliberately split
// by target.  This source refuses an unsupported target instead of degrading
// to a verified-then-re-resolved staging pathname.
func main() {
	directory, err := os.MkdirTemp("", "service-lasso-primary-")
	if err != nil {
		fail()
	}
	defer os.RemoveAll(directory)
	path := filepath.Join(directory, "service-lassoctl.sea")
	if err := os.WriteFile(path, seaBytes, 0700); err != nil {
		fail()
	}
	helper := filepath.Join(directory, "service-lasso-confined-scaffold")
	if err := os.WriteFile(helper, confinedWriterBytes, 0700); err != nil {
		fail()
	}
	endpoint := filepath.Join(directory, "primary.sock")
	listener, err := net.Listen("unix", endpoint)
	if err != nil {
		fail()
	}
	defer listener.Close()
	go serve(listener, helper)
	child := exec.Command(path, os.Args[1:]...)
	child.Stdin, child.Stdout, child.Stderr = os.Stdin, os.Stdout, os.Stderr
	child.Env = append(os.Environ(), "SERVICE_LASSO_PRIMARY_GATE=unix-v1", "SERVICE_LASSO_PRIMARY_GATE_PIPE="+endpoint)
	if err := child.Run(); err != nil {
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
