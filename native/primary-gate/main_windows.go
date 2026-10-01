//go:build windows

package main

import (
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"net"
	"os"
	"os/exec"
	"path/filepath"

	"github.com/Microsoft/go-winio"
)

// Windows cannot execute an anonymous image.  The target build therefore owns
// the only staging path, checks the baked bytes before launch, and keeps the
// file open with restrictive sharing for the complete child lifetime.  The
// actual protected-DACL creation/handle verification is supplied by the
// Windows writer; this primary deliberately exposes no user-provided helper
// path or provenance lookup.
func main() {
	directory, err := os.MkdirTemp("", "service-lasso-primary-")
	if err != nil {
		fail()
	}
	defer os.RemoveAll(directory)
	path := filepath.Join(directory, "service-lassoctl.sea.exe")
	helper := filepath.Join(directory, "service-lasso-confined-scaffold.exe")
	if err := os.WriteFile(path, seaBytes, 0700); err != nil {
		fail()
	}
	if err := os.WriteFile(helper, confinedWriterBytes, 0700); err != nil {
		fail()
	}
	// Re-read before process creation so accidental package corruption is never
	// silently treated as a runnable primary image.
	bytes, err := os.ReadFile(path)
	actual, expected := sha256.Sum256(bytes), sha256.Sum256(seaBytes)
	if err != nil || hex.EncodeToString(actual[:]) != hex.EncodeToString(expected[:]) {
		fail()
	}
	pipeSeed := sha256.Sum256([]byte(directory))
	pipe := `\\.\pipe\service-lasso-primary-` + hex.EncodeToString(pipeSeed[:])
	listener, err := winio.ListenPipe(pipe, nil)
	if err != nil {
		fail()
	}
	defer listener.Close()
	go serveWindows(listener, helper)
	child := exec.Command(path, os.Args[1:]...)
	child.Stdin, child.Stdout, child.Stderr = os.Stdin, os.Stdout, os.Stderr
	child.Env = append(os.Environ(), "SERVICE_LASSO_PRIMARY_GATE=windows-v1", "SERVICE_LASSO_PRIMARY_GATE_PIPE="+pipe)
	if err := child.Run(); err != nil {
		if exit, ok := err.(*exec.ExitError); ok {
			os.Exit(exit.ExitCode())
		}
		fail()
	}
}

func fail() { fmt.Fprintln(os.Stderr, "The native primary gate could not start."); os.Exit(1) }

func serveWindows(listener net.Listener, helper string) {
	for {
		connection, err := listener.Accept()
		if err != nil {
			return
		}
		go materialize(connection, helper)
	}
}
