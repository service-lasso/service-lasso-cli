//go:build darwin

package main

import (
	"net"
	"os/exec"
	"testing"
	"time"
)

func TestDarwinLifecycleRejectsAdmissionAfterObservedChildExit(t *testing.T) {
	child := exec.Command("/bin/sh", "-c", "exit 0")
	if err := child.Start(); err != nil {
		t.Fatal(err)
	}
	lifecycle, err := newDarwinLifecycle(child.Process.Pid)
	if err != nil {
		t.Fatal(err)
	}
	defer close(lifecycle.kqueue)
	if err = child.Wait(); err != nil {
		t.Fatal(err)
	}
	server, client := net.Pipe()
	defer client.Close()
	done := make(chan struct{})
	go func() { lifecycle.admit(server); close(done) }()
	_ = client.SetReadDeadline(time.Now().Add(time.Second))
	if _, err = client.Read(make([]byte, 1)); err == nil {
		t.Fatal("post-exit connection was not rejected")
	}
	select {
	case <-done:
	case <-time.After(time.Second):
		t.Fatal("post-exit admission did not finish")
	}
}
