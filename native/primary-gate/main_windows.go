//go:build windows

package main

import (
	"crypto/rand"
	"crypto/sha256"
	"encoding/hex"
	"fmt"
	"net"
	"os"
	"os/exec"
	"path/filepath"
	"unsafe"

	"github.com/Microsoft/go-winio"
	"golang.org/x/sys/windows"
)

// Windows cannot execute an anonymous image. The target build therefore makes
// its staging directory and both images with an explicit protected OWNER
// RIGHTS DACL, verifies that descriptor from the held handles, and retains
// read handles which share neither write nor delete for the complete child
// lifetime. CreateProcess still receives a filename, but the held parent and
// leaf capabilities prevent a replacement or rename between verification and
// process creation.
const privateStageDacl = "D:P(A;;FA;;;OW)"

func privateAttributes() (*windows.SecurityAttributes, error) {
	descriptor, err := windows.SecurityDescriptorFromString(privateStageDacl)
	if err != nil {
		return nil, err
	}
	return &windows.SecurityAttributes{Length: uint32(unsafe.Sizeof(windows.SecurityAttributes{})), SecurityDescriptor: descriptor}, nil
}

func verifyPrivate(handle windows.Handle) error {
	descriptor, err := windows.GetSecurityInfo(handle, windows.SE_FILE_OBJECT, windows.DACL_SECURITY_INFORMATION)
	if err != nil || !descriptor.IsValid() || descriptor.String() != privateStageDacl {
		return fmt.Errorf("private stage DACL verification failed")
	}
	control, _, err := descriptor.Control()
	if err != nil || control&windows.SE_DACL_PROTECTED == 0 {
		return fmt.Errorf("private stage DACL is not protected")
	}
	return nil
}

func stageDirectory() (string, windows.Handle, error) {
	seed := make([]byte, 24)
	if _, err := rand.Read(seed); err != nil {
		return "", 0, err
	}
	path := filepath.Join(os.TempDir(), "service-lasso-primary-"+hex.EncodeToString(seed))
	attributes, err := privateAttributes()
	if err != nil {
		return "", 0, err
	}
	if err = windows.CreateDirectory(windows.StringToUTF16Ptr(path), attributes); err != nil {
		return "", 0, err
	}
	handle, err := windows.CreateFile(windows.StringToUTF16Ptr(path), windows.GENERIC_READ, windows.FILE_SHARE_READ, nil, windows.OPEN_EXISTING, windows.FILE_FLAG_BACKUP_SEMANTICS|windows.FILE_FLAG_OPEN_REPARSE_POINT, 0)
	if err != nil {
		return "", 0, err
	}
	if err = verifyPrivate(handle); err != nil {
		windows.CloseHandle(handle)
		return "", 0, err
	}
	return path, handle, nil
}

func stageImage(path string, value []byte) (windows.Handle, error) {
	attributes, err := privateAttributes()
	if err != nil {
		return 0, err
	}
	handle, err := windows.CreateFile(windows.StringToUTF16Ptr(path), windows.GENERIC_READ|windows.GENERIC_WRITE, windows.FILE_SHARE_READ, attributes, windows.CREATE_NEW, windows.FILE_ATTRIBUTE_NORMAL, 0)
	if err != nil {
		return 0, err
	}
	if err = verifyPrivate(handle); err != nil {
		windows.CloseHandle(handle)
		return 0, err
	}
	var written uint32
	if err = windows.WriteFile(handle, value, &written, nil); err != nil || int(written) != len(value) {
		windows.CloseHandle(handle)
		return 0, fmt.Errorf("private stage write failed")
	}
	if err = windows.FlushFileBuffers(handle); err != nil {
		windows.CloseHandle(handle)
		return 0, err
	}
	// The writer handle necessarily has write access, which the Windows image
	// loader will not share. Close it only after the protected descriptor is
	// installed and durable, then reopen the same leaf for the remainder of
	// the launch with read-only access and no write/delete sharing.
	if err = windows.CloseHandle(handle); err != nil {
		return 0, err
	}
	handle, err = windows.CreateFile(windows.StringToUTF16Ptr(path), windows.GENERIC_READ, windows.FILE_SHARE_READ, nil, windows.OPEN_EXISTING, windows.FILE_ATTRIBUTE_NORMAL|windows.FILE_FLAG_OPEN_REPARSE_POINT, 0)
	if err != nil {
		return 0, err
	}
	if err = verifyPrivate(handle); err != nil {
		windows.CloseHandle(handle)
		return 0, err
	}
	if _, err = windows.SetFilePointer(handle, 0, nil, 0); err != nil {
		windows.CloseHandle(handle)
		return 0, err
	}
	read := make([]byte, len(value))
	var count uint32
	if err = windows.ReadFile(handle, read, &count, nil); err != nil || int(count) != len(value) || sha256.Sum256(read) != sha256.Sum256(value) {
		windows.CloseHandle(handle)
		return 0, fmt.Errorf("private stage readback failed")
	}
	if _, err = windows.SetFilePointer(handle, 0, nil, 0); err != nil {
		windows.CloseHandle(handle)
		return 0, err
	}
	return handle, nil
}
func main() {
	directory, directoryHandle, err := stageDirectory()
	if err != nil {
		failAt("private directory")
	}
	defer windows.CloseHandle(directoryHandle)
	defer os.RemoveAll(directory)
	path := filepath.Join(directory, "service-lassoctl.sea.exe")
	helper := filepath.Join(directory, "service-lasso-confined-scaffold.exe")
	seaHandle, err := stageImage(path, seaBytes)
	if err != nil {
		failAt("SEA image")
	}
	defer windows.CloseHandle(seaHandle)
	helperHandle, err := stageImage(helper, confinedWriterBytes)
	if err != nil {
		failAt("writer image")
	}
	defer windows.CloseHandle(helperHandle)
	if err = verifyPrivate(seaHandle); err != nil {
		failAt("SEA DACL")
	}
	pipeSeed := sha256.Sum256([]byte(directory))
	pipe := `\\.\pipe\service-lasso-primary-` + hex.EncodeToString(pipeSeed[:])
	listener, err := winio.ListenPipe(pipe, &winio.PipeConfig{SecurityDescriptor: privateStageDacl})
	if err != nil {
		failAt("private pipe")
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

func fail()           { fmt.Fprintln(os.Stderr, "The native primary gate could not start."); os.Exit(1) }
func failAt(_ string) { fail() }

func serveWindows(listener net.Listener, helper string) {
	for {
		connection, err := listener.Accept()
		if err != nil {
			return
		}
		go materialize(connection, helper)
	}
}
