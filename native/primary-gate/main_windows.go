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
	"sync"
	"syscall"
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

// CreateFile's directory API creates a pathname and only then returns an open
// handle.  That is not sufficient here: a same-owner process could acquire a
// delete or write handle in that interval.  Use NtCreateFile with the final
// stage name (or a held parent) so creation and the non-sharing handle are one
// kernel operation.
const (
	gateObjCaseInsensitive        = 0x40
	gateObjDontReparse            = 0x1000
	gateFileListDirectory         = 0x0001
	gateFileReadData              = 0x0001
	gateFileWriteData             = 0x0002
	gateDeleteAccess              = 0x10000
	gateReadControl               = 0x20000
	gateSynchronize               = 0x100000
	gateShareRead                 = 0x0001
	gateFileOpen                  = 1
	gateFileCreate                = 2
	gateFileDirectoryFile         = 0x0001
	gateFileSynchronousIoNonalert = 0x20
	gateFileOpenReparsePoint      = 0x200000
)

type gateUnicodeString struct {
	length, maximumLength uint16
	buffer                *uint16
}
type gateObjectAttributes struct {
	length                   uint32
	rootDirectory            syscall.Handle
	objectName               *gateUnicodeString
	attributes               uint32
	securityDescriptor       uintptr
	securityQualityOfService uintptr
}
type gateIOStatusBlock struct{ status, information uintptr }

var gateNtDll = syscall.NewLazyDLL("ntdll.dll")
var gateNtCreateFile = gateNtDll.NewProc("NtCreateFile")

func gateOpen(name string, parent windows.Handle, access, disposition, options uint32, descriptor *windows.SECURITY_DESCRIPTOR) (windows.Handle, error) {
	if parent == 0 && filepath.IsAbs(name) {
		// NtCreateFile receives an NT object-manager path when no RootDirectory
		// is supplied; Win32's C:\\ spelling is not an NT object name.
		name = `\??\` + name
	}
	pointer, err := syscall.UTF16PtrFromString(name)
	if err != nil {
		return 0, err
	}
	length := uint16(len(syscall.StringToUTF16(name))-1) * 2
	u := gateUnicodeString{length, length + 2, pointer}
	var securityDescriptor uintptr
	if descriptor != nil {
		securityDescriptor = uintptr(unsafe.Pointer(descriptor))
	}
	attributes := gateObjectAttributes{uint32(unsafe.Sizeof(gateObjectAttributes{})), syscall.Handle(parent), &u, gateObjCaseInsensitive | gateObjDontReparse, securityDescriptor, 0}
	var handle windows.Handle
	var iosb gateIOStatusBlock
	if disposition == gateFileOpen {
		options |= gateFileOpenReparsePoint
	}
	status, _, callErr := gateNtCreateFile.Call(uintptr(unsafe.Pointer(&handle)), uintptr(access), uintptr(unsafe.Pointer(&attributes)), uintptr(unsafe.Pointer(&iosb)), 0, 0, gateShareRead, uintptr(disposition), uintptr(options), 0, 0)
	if int32(status) < 0 {
		if callErr != syscall.Errno(0) {
			return 0, callErr
		}
		return 0, fmt.Errorf("NtCreateFile status 0x%x", status)
	}
	return handle, nil
}

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
	handle, err := gateOpen(path, 0, gateFileListDirectory|gateDeleteAccess|gateReadControl|gateSynchronize, gateFileCreate, gateFileDirectoryFile|gateFileSynchronousIoNonalert, attributes.SecurityDescriptor)
	if err != nil {
		return "", 0, err
	}
	if err = verifyPrivate(handle); err != nil {
		windows.CloseHandle(handle)
		return "", 0, err
	}
	return path, handle, nil
}

func stageImage(directory windows.Handle, name string, value []byte) (windows.Handle, error) {
	attributes, err := privateAttributes()
	if err != nil {
		return 0, err
	}
	// FILE_CREATE returns the new leaf handle relative to the held directory;
	// it never re-resolves a stage-directory pathname after that handle exists.
	// The retained read handle then rejects any pre-existing writable/delete
	// client during the intentional writer-close/reopen transition.
	handle, err := gateOpen(name, directory, gateFileWriteData|gateReadControl|gateSynchronize, gateFileCreate, gateFileSynchronousIoNonalert, attributes.SecurityDescriptor)
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
	handle, err = gateOpen(name, directory, gateFileReadData|gateReadControl|gateSynchronize, gateFileOpen, gateFileSynchronousIoNonalert, nil)
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
	if !newGateCapability() { fail() }
	directory, directoryHandle, err := stageDirectory()
	if err != nil {
		failAt("private directory")
	}
	defer windows.CloseHandle(directoryHandle)
	defer os.RemoveAll(directory)
	seaName := "service-lassoctl.sea.exe"
	helperName := "service-lasso-confined-scaffold.exe"
	seaHandle, err := stageImage(directoryHandle, seaName, seaBytes)
	if err != nil {
		failAt("SEA image")
	}
	defer windows.CloseHandle(seaHandle)
	helperHandle, err := stageImage(directoryHandle, helperName, confinedWriterBytes)
	if err != nil {
		failAt("writer image")
	}
	defer windows.CloseHandle(helperHandle)
	// This is the exact protected image staged relative to directoryHandle.
	// Never pass helperName to exec.Command: that would reintroduce PATH lookup.
	heldExecutionPath = filepath.Join(directory, helperName)
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
	// CreateProcess still consumes a name.  The file itself was created and
	// reopened relative to the held directory; its retained no-write/no-delete
	// handle prevents an incompatible client handle from surviving the writer
	// close or appearing before this launch.
	child := exec.Command(filepath.Join(directory, seaName), os.Args[1:]...)
	child.Stdin, child.Stdout, child.Stderr = os.Stdin, os.Stdout, os.Stderr
	child.Env = gateEnvironment(os.Environ(), "windows-v2", pipe, 0)
	if err := child.Start(); err != nil {
		fail()
	}
	// Keep a separate real process-object handle.  Its identity survives PID
	// lookup and makes PID reuse impossible while admission is active.
	process, err := windows.OpenProcess(windows.SYNCHRONIZE|windows.PROCESS_QUERY_LIMITED_INFORMATION, false, uint32(child.Process.Pid))
	if err != nil {
		failAt("child process identity")
	}
	defer windows.CloseHandle(process)
	lifecycle := &windowsLifecycle{process: process, pid: uint32(child.Process.Pid)}
	exit := make(chan error, 1)
	go func() { err := child.Wait(); lifecycle.terminate(); exit <- err }()
	go serveWindows(listener, lifecycle)
	if err := <-exit; err != nil {
		if exit, ok := err.(*exec.ExitError); ok {
			os.Exit(exit.ExitCode())
		}
		fail()
	}
}

func fail()           { fmt.Fprintln(os.Stderr, "The native primary gate could not start."); os.Exit(1) }
func failAt(_ string) { fail() }

var getNamedPipeClientProcessID = windows.NewLazySystemDLL("kernel32.dll").NewProc("GetNamedPipeClientProcessId")

func windowsPeerIs(connection net.Conn, expectedPID uint32) bool {
	fdConnection, ok := connection.(interface{ Fd() uintptr })
	if !ok {
		return false
	}
	var actual uint32
	result, _, callErr := getNamedPipeClientProcessID.Call(fdConnection.Fd(), uintptr(unsafe.Pointer(&actual)))
	return result != 0 && callErr == syscall.Errno(0) && actual == expectedPID
}

type windowsLifecycle struct {
	mu         sync.Mutex
	process    windows.Handle
	pid        uint32
	terminated bool
}

func (l *windowsLifecycle) terminate() { l.mu.Lock(); l.terminated = true; l.mu.Unlock() }
func (l *windowsLifecycle) admit(connection net.Conn) bool {
	l.mu.Lock()
	defer l.mu.Unlock()
	if l.terminated || !windowsPeerIs(connection, l.pid) {
		return false
	}
	state, err := windows.WaitForSingleObject(l.process, 0)
	if err != nil || state != uint32(windows.WAIT_TIMEOUT) {
		return false
	}
	materialize(connection)
	return true
}
func serveWindows(listener net.Listener, lifecycle *windowsLifecycle) {
	for {
		connection, err := listener.Accept()
		if err != nil {
			return
		}
		if !lifecycle.admit(connection) {
			_ = connection.Close()
			continue
		}
		return
	}
}
