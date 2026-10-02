//go:build darwin

package main

import (
	"bytes"
	"encoding/hex"
	"fmt"
	"os"
	"os/exec"
	"runtime"
	"strconv"
	"testing"

	"golang.org/x/sys/unix"
)

func TestDarwinBorrowedDescriptorChild(t *testing.T) {
	if os.Getenv("SERVICE_LASSO_DESCRIPTOR_CHILD") != "1" { return }
	var stat unix.Stat_t
	if unix.Fstat(3, &stat) != nil || fmt.Sprintf("%d:%d", stat.Dev, stat.Ino) != os.Getenv("SERVICE_LASSO_DESCRIPTOR_OBJECT") { os.Exit(11) }
	value := make([]byte, 32)
	for offset := 0; offset < len(value); {
		n, err := unix.Read(4, value[offset:])
		if err != nil || n <= 0 { os.Exit(12) }
		offset += n
	}
	if hex.EncodeToString(value) != os.Getenv("SERVICE_LASSO_DESCRIPTOR_BYTES") { os.Exit(13) }
	// Exercise the descriptor's actual object after consuming the stream.
	if unix.Fstat(3, &stat) != nil { os.Exit(14) }
	os.Exit(0)
}

func TestDarwinRepeatedHelperBorrowingPreservesCapabilityObjectsAndReusedFDs(t *testing.T) {
	image, err := os.CreateTemp(t.TempDir(), "held-image")
	if err != nil { t.Fatal(err) }
	defer image.Close()
	if _, err = image.Write(seaBytes); err != nil { t.Fatal(err) }
	directory, err := os.Open(t.TempDir())
	if err != nil { t.Fatal(err) }
	defer directory.Close()
	read, write, err := os.Pipe()
	if err != nil { t.Fatal(err) }
	defer read.Close(); defer write.Close()
	executable, err := os.Executable()
	if err != nil { t.Fatal(err) }
	for operation := 0; operation < 24; operation++ {
		object, mode := image, "set"
		if operation % 2 != 0 { object, mode = directory, "clear" }
		request, err := ownerObjectRequest(int(object.Fd()), mode)
		if err != nil { t.Fatal(err) }
		value := bytes.Repeat([]byte{byte(operation + 1)}, 32)
		if _, err = write.Write(value); err != nil { t.Fatal(err) }
		command := exec.Command(executable, "-test.run=^TestDarwinBorrowedDescriptorChild$")
		command.Env = append(os.Environ(), "SERVICE_LASSO_DESCRIPTOR_CHILD=1", "SERVICE_LASSO_DESCRIPTOR_OBJECT=" + strconv.FormatUint(request.Device, 10) + ":" + strconv.FormatUint(request.Inode, 10), "SERVICE_LASSO_DESCRIPTOR_BYTES=" + hex.EncodeToString(value))
		if err = runDarwinHelperCommand(command, int(object.Fd()), int(read.Fd())); err != nil { t.Fatal(err) }
		for _, duplicate := range command.ExtraFiles {
			if _, err = duplicate.Stat(); err == nil { t.Fatal("per-operation duplicate retained after child closure") }
		}
		// A new object can reuse one of the now-closed duplicate descriptors.
		// Obsolete finalizers must never close it, or the original raw borrows.
		reused, err := os.Open(os.DevNull)
		if err != nil { t.Fatal(err) }
		for iteration := 0; iteration < 3; iteration++ { runtime.GC() }
		if _, err = reused.Stat(); err != nil { t.Fatal("reused descriptor closed by obsolete owner", err) }
		if err = reused.Close(); err != nil { t.Fatal(err) }
		after, err := ownerObjectRequest(int(object.Fd()), mode)
		if err != nil || after.Device != request.Device || after.Inode != request.Inode || after.Digest != request.Digest { t.Fatal("held object lost across operations", err) }
		if _, err = read.Stat(); err != nil { t.Fatal("capability stream lost across operations", err) }
	}
	for _, target := range []string{os.DevNull + "/missing", executable} {
		command := exec.Command(target, "-test.run=^TestDarwinBorrowedDescriptorChild$")
		if target == executable { command.Env = append(os.Environ(), "SERVICE_LASSO_DESCRIPTOR_CHILD=1", "SERVICE_LASSO_DESCRIPTOR_OBJECT=wrong") }
		if err = runDarwinHelperCommand(command, int(image.Fd()), int(read.Fd())); err == nil { t.Fatal("failed launch/child accepted") }
		for _, duplicate := range command.ExtraFiles { if _, err = duplicate.Stat(); err == nil { t.Fatal("failed operation leaked duplicate") } }
		runtime.GC()
		if _, err = image.Stat(); err != nil { t.Fatal(err) }
		if _, err = read.Stat(); err != nil { t.Fatal(err) }
	}
	// Second Dup failure also closes the already-created image duplicate.
	if err = runDarwinHelperCommand(exec.Command(executable), int(image.Fd()), -1); err == nil { t.Fatal("invalid capability accepted") }
	runtime.GC()
	if _, err = image.Stat(); err != nil { t.Fatal(err) }
}
