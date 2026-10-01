//go:build windows

package main

import (
	"fmt"
	"path/filepath"
	"strings"
	"syscall"
	"unsafe"
)

// NtCreateFile is used instead of CreateFileW.  RootDirectory makes every
// child lookup relative to a held handle; OBJ_DONT_REPARSE and
// FILE_OPEN_REPARSE_POINT stop junctions and other reparse traversal before a
// descendant can be created.  Handles stay open until commit or rollback.
const (
	objCaseInsensitive         = 0x40
	objDontReparse             = 0x1000
	fileListDirectory          = 0x0001
	fileWriteData              = 0x0002
	deleteAccess               = 0x10000
	synchronize                = 0x100000
	shareRead                  = 0x0001
	fileOpen                   = 1
	fileCreate                 = 2
	fileDirectoryFile          = 0x0001
	fileSynchronousIoNonalert  = 0x20
	fileOpenReparsePoint       = 0x200000
	fileDispositionInformation = 13
)

type unicodeString struct {
	length        uint16
	maximumLength uint16
	buffer        *uint16
}
type objectAttributes struct {
	length                   uint32
	rootDirectory            syscall.Handle
	objectName               *unicodeString
	attributes               uint32
	securityDescriptor       uintptr
	securityQualityOfService uintptr
}
type ioStatusBlock struct {
	status      uintptr
	information uintptr
}
type fileDispositionInformationValue struct{ deleteFile byte }

var ntdll = syscall.NewLazyDLL("ntdll.dll")
var ntCreateFile = ntdll.NewProc("NtCreateFile")
var ntSetInformationFile = ntdll.NewProc("NtSetInformationFile")

func nativeOpen(name string, parent syscall.Handle, access uint32, disposition uint32, options uint32) (syscall.Handle, error) {
	pointer, err := syscall.UTF16PtrFromString(name)
	if err != nil {
		return 0, err
	}
	length := uint16(len(syscall.StringToUTF16(name))-1) * 2
	u := unicodeString{length, length + 2, pointer}
	attributes := objectAttributes{uint32(unsafe.Sizeof(objectAttributes{})), parent, &u, objCaseInsensitive | objDontReparse, 0, 0}
	var handle syscall.Handle
	var iosb ioStatusBlock
	status, _, callErr := ntCreateFile.Call(uintptr(unsafe.Pointer(&handle)), uintptr(access), uintptr(unsafe.Pointer(&attributes)), uintptr(unsafe.Pointer(&iosb)), 0, 0, shareRead, uintptr(disposition), uintptr(options|fileOpenReparsePoint), 0, 0)
	if int32(status) < 0 {
		if callErr != syscall.Errno(0) {
			return 0, callErr
		}
		return 0, fmt.Errorf("NtCreateFile status 0x%x", status)
	}
	return handle, nil
}
func dispose(handle syscall.Handle) {
	value := fileDispositionInformationValue{1}
	var iosb ioStatusBlock
	_, _, _ = ntSetInformationFile.Call(uintptr(handle), uintptr(unsafe.Pointer(&iosb)), uintptr(unsafe.Pointer(&value)), unsafe.Sizeof(value), fileDispositionInformation)
}
func materializeNt(destination string, entries []entry) error {
	volume := filepath.VolumeName(destination)
	rest := strings.Trim(strings.TrimPrefix(destination, volume), `\\/`)
	if volume == "" || rest == "" {
		return fmt.Errorf("invalid absolute destination")
	}
	// A volume designator is not a directory object.  Keep the trailing
	// separator so this opens the volume root as the held directory handle.
	root, err := nativeOpen(`\??\`+volume+`\`, 0, fileListDirectory|synchronize, fileOpen, fileDirectoryFile|fileSynchronousIoNonalert)
	if err != nil {
		return err
	}
	handles := []syscall.Handle{root}
	created := []syscall.Handle{}
	closeAll := func() {
		for index := len(handles) - 1; index >= 0; index-- {
			_ = syscall.CloseHandle(handles[index])
		}
	}
	defer closeAll()
	rollback := func() {
		for index := len(created) - 1; index >= 0; index-- {
			dispose(created[index])
		}
	}
	current := root
	pieces := strings.FieldsFunc(rest, func(r rune) bool { return r == '\\' || r == '/' })
	for _, piece := range pieces[:len(pieces)-1] {
		child, e := nativeOpen(piece, current, fileListDirectory|synchronize, fileOpen, fileDirectoryFile|fileSynchronousIoNonalert)
		if e != nil {
			return e
		}
		handles = append(handles, child)
		current = child
	}
	if err = testGate("before-project-create"); err != nil {
		return err
	}
	project, e := nativeOpen(pieces[len(pieces)-1], current, fileListDirectory|deleteAccess|synchronize, fileCreate, fileDirectoryFile|fileSynchronousIoNonalert)
	if e != nil {
		return e
	}
	handles = append(handles, project)
	created = append(created, project)
	current = project
	for _, item := range entries {
		current = project
		pieces = strings.Split(item.path, "/")
		for _, piece := range pieces[:len(pieces)-1] {
			child, e := nativeOpen(piece, current, fileListDirectory|synchronize, fileOpen, fileDirectoryFile|fileSynchronousIoNonalert)
			if e != nil {
				child, e = nativeOpen(piece, current, fileListDirectory|deleteAccess|synchronize, fileCreate, fileDirectoryFile|fileSynchronousIoNonalert)
				if e != nil {
					rollback()
					return e
				}
				created = append(created, child)
			}
			handles = append(handles, child)
			current = child
		}
		file, e := nativeOpen(pieces[len(pieces)-1], current, fileWriteData|deleteAccess|synchronize, fileCreate, fileSynchronousIoNonalert)
		if e != nil {
			rollback()
			return e
		}
		handles = append(handles, file)
		created = append(created, file)
		written := uint32(0)
		if e = syscall.WriteFile(file, item.bytes, &written, nil); e != nil || int(written) != len(item.bytes) {
			rollback()
			if e != nil {
				return e
			}
			return fmt.Errorf("short native write")
		}
		if e = syscall.FlushFileBuffers(file); e != nil {
			rollback()
			return e
		}
		if err = testGate("after-file-write"); err != nil {
			rollback()
			return err
		}
	}
	return nil
}
