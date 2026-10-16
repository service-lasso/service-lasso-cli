//go:build windows

package main

import (
	"crypto/rand"
	"encoding/binary"
	"encoding/hex"
	"fmt"
	"path/filepath"
	"strings"
	"syscall"
	"unsafe"

	"golang.org/x/sys/windows"
)

// NtCreateFile is used instead of CreateFileW.  RootDirectory makes every
// child lookup relative to a held handle; OBJ_DONT_REPARSE and
// FILE_OPEN_REPARSE_POINT stop junctions and other reparse traversal before a
// descendant can be created. Handles stay open through the original commit or
// failed-candidate retention path and are closed once with checked outcomes.
const (
	objCaseInsensitive         = 0x40
	objDontReparse             = 0x1000
	fileListDirectory          = 0x0001
	fileWriteData              = 0x0002
	deleteAccess               = 0x10000
	readControl                = 0x20000
	synchronize                = 0x100000
	shareRead                  = 0x0001
	fileOpen                   = 1
	fileCreate                 = 2
	fileDirectoryFile          = 0x0001
	fileSynchronousIoNonalert  = 0x20
	fileOpenReparsePoint       = 0x200000
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

var ntdll = syscall.NewLazyDLL("ntdll.dll")
var ntCreateFile = ntdll.NewProc("NtCreateFile")
var ntSetInformationFile = ntdll.NewProc("NtSetInformationFile")

const privateOwnerDacl = "D:P(A;;FA;;;OW)"

func privateDescriptor() (*windows.SECURITY_DESCRIPTOR, error) {
	return windows.SecurityDescriptorFromString(privateOwnerDacl)
}
func verifyPrivateDescriptor(handle syscall.Handle) error {
	descriptor, err := windows.GetSecurityInfo(windows.Handle(handle), windows.SE_FILE_OBJECT, windows.DACL_SECURITY_INFORMATION)
	if err != nil || !descriptor.IsValid() || descriptor.String() != privateOwnerDacl {
		return fmt.Errorf("private DACL verification failed")
	}
	control, _, err := descriptor.Control()
	if err != nil || control&windows.SE_DACL_PROTECTED == 0 {
		return fmt.Errorf("private DACL is inheritable")
	}
	ownerDescriptor,err:=windows.GetSecurityInfo(windows.Handle(handle),windows.SE_FILE_OBJECT,windows.OWNER_SECURITY_INFORMATION);if err!=nil||ownerDescriptor==nil{return fmt.Errorf("private native owner unavailable")};owner,_,err:=ownerDescriptor.Owner();if err!=nil||owner==nil{return fmt.Errorf("private native owner unavailable")}
	originalOutputOwner.mu.Lock();defer originalOutputOwner.mu.Unlock();if !originalOutputOwner.ready||!owner.Equals(originalOutputOwner.owner){return fmt.Errorf("private native owner differs")}
	return nil
}
func nativeOpen(name string, parent syscall.Handle, access uint32, disposition uint32, options uint32, descriptor *windows.SECURITY_DESCRIPTOR) (syscall.Handle, error) {
	return nativeOpenShared(name,parent,access,disposition,options,descriptor,shareRead)
}
func nativeOpenShared(name string, parent syscall.Handle, access uint32, disposition uint32, options uint32, descriptor *windows.SECURITY_DESCRIPTOR,sharing uint32) (syscall.Handle, error) {
	access|=windows.FILE_READ_ATTRIBUTES
	pointer, err := syscall.UTF16PtrFromString(name)
	if err != nil {
		return 0, err
	}
	length := uint16(len(syscall.StringToUTF16(name))-1) * 2
	u := unicodeString{length, length + 2, pointer}
	var securityDescriptor uintptr
	if descriptor != nil { securityDescriptor = uintptr(unsafe.Pointer(descriptor)) }
	attributes := objectAttributes{uint32(unsafe.Sizeof(objectAttributes{})), parent, &u, objCaseInsensitive | objDontReparse, securityDescriptor, 0}
	var handle syscall.Handle
	var iosb ioStatusBlock
	status, _, callErr := ntCreateFile.Call(uintptr(unsafe.Pointer(&handle)), uintptr(access), uintptr(unsafe.Pointer(&attributes)), uintptr(unsafe.Pointer(&iosb)), 0, 0, uintptr(sharing), uintptr(disposition), uintptr(options|fileOpenReparsePoint), 0, 0)
	if int32(status) < 0 {
		// STATUS_OBJECT_NAME_COLLISION is the only expected collision from a
		// FILE_CREATE operation. Preserve it as a closed protocol code.
		if status == 0xc0000035 {
			return 0, errDestinationExists
		}
		if callErr != syscall.Errno(0) {
			return 0, callErr
		}
		return 0, fmt.Errorf("NtCreateFile status 0x%x", status)
	}
	return handle, nil
}
func createPrivate(name string, parent syscall.Handle, access uint32, options uint32) (syscall.Handle, error) {
	descriptor, err := privateDescriptor()
	if err != nil { return 0, err }
	handle, err := nativeOpen(name, parent, access|readControl, fileCreate, options, descriptor)
	if err != nil { return 0, err }
	// The caller records this original handle before descriptor verification.
	// A failed verification therefore cannot hide an unchecked native close.
	return handle, nil
}
func commitCandidateNt(candidate,parent syscall.Handle,leaf string)error{
	name,err:=syscall.UTF16FromString(leaf);if err!=nil||len(name)<2||len(name)>256{return fmt.Errorf("invalid commit leaf")};name=name[:len(name)-1]
	// FILE_RENAME_INFORMATION uses BOOLEAN ReplaceIfExists=false, the original
	// held parent HANDLE, and the final UTF16 leaf. No pathname re-resolution or
	// replace-existing flag is available. The selected product supports x64.
	if unsafe.Sizeof(parent)!=8{return fmt.Errorf("unsupported native rename layout")}
	var raw [20+510]byte;binary.LittleEndian.PutUint64(raw[8:16],uint64(parent));binary.LittleEndian.PutUint32(raw[16:20],uint32(2*len(name)));for i,c:=range name{binary.LittleEndian.PutUint16(raw[20+2*i:22+2*i],c)}
	var iosb ioStatusBlock;status,_,_:=ntSetInformationFile.Call(uintptr(candidate),uintptr(unsafe.Pointer(&iosb)),uintptr(unsafe.Pointer(&raw[0])),uintptr(20+2*len(name)),19)
	if int32(status)<0{if status==0xc0000035{return errDestinationExists};return fmt.Errorf("native candidate commit rejected")};return nil
}
func materializeNt(destination string,entries []entry)error{return materializeNtObserved(destination,entries,nil)}
func materializeNtObserved(destination string, entries []entry,digest *writerReadbackDigest) (result error) {
	defer func(){if result!=nil&&digest!=nil{digest.failed=true}}()
	if err:=validateWriterEntries(destination,entries);err!=nil{return err}
	if filepath.Clean(destination)!=destination{return fmt.Errorf("destination is not canonical")}
	if err := initializeOutputModeOwner(); err != nil { return err }
	defer func(){if err:=closeOutputModeOwner();err!=nil&&result==nil{result=err}}()
	volume := filepath.VolumeName(destination)
	rest := strings.Trim(strings.TrimPrefix(destination, volume), `\\/`)
	if volume == "" || rest == "" {
		return fmt.Errorf("invalid absolute destination")
	}
	pieces := strings.FieldsFunc(rest, func(r rune) bool { return r == '\\' || r == '/' });if len(pieces)<1||len(pieces)>128{return fmt.Errorf("native writer reference capacity exhausted")}
	// A volume designator is not a directory object.  Keep the trailing
	// separator so this opens the volume root as the held directory handle.
	var originalHandles [128]syscall.Handle
	rootAccess:=uint32(fileListDirectory|synchronize);if len(pieces)==1{rootAccess|=windows.FILE_GENERIC_WRITE}
	root, err := nativeOpen(`\??\`+volume+`\`, 0, rootAccess, fileOpen, fileDirectoryFile|fileSynchronousIoNonalert, nil)
	if err != nil {
		return err
	}
	originalHandles[0]=root;handles:=originalHandles[:1]
	closeAll := func() error {
		var first error
		for index := len(handles) - 1; index >= 0; index-- {
			if err:=syscall.CloseHandle(handles[index]);err!=nil&&first==nil{first=err}
		}
		return first
	}
	defer func(){if err:=closeAll();err!=nil&&result==nil{result=err}}()
	// Failed attempts retain the original private candidate. No failure path
	// disposes original files or repairs uncertainty by recreating a directory.
	current := root
	for index, piece := range pieces[:len(pieces)-1] {
		if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
		access:=uint32(fileListDirectory|synchronize);if index==len(pieces)-2{access|=windows.FILE_GENERIC_WRITE}
		child, e := nativeOpen(piece, current, access, fileOpen, fileDirectoryFile|fileSynchronousIoNonalert, nil)
		if e != nil {
			return e
		}
		handles = append(handles, child)
		current = child
	}
	if err = testGate("before-project-create"); err != nil {
		return err
	}
	finalLeaf:=pieces[len(pieces)-1];commitParent:=current
	var candidateNonce [16]byte;if _,err:=rand.Read(candidateNonce[:]);err!=nil{return err};candidateLeaf:=".service-lasso-txn-"+hex.EncodeToString(candidateNonce[:])
	if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
	project, e := createPrivate(candidateLeaf, current, fileListDirectory|windows.FILE_GENERIC_WRITE|deleteAccess|synchronize, fileDirectoryFile|fileSynchronousIoNonalert)
	if e != nil {
		return e
	}
	handles = append(handles, project)
	if err=verifyPrivateDescriptor(project);err!=nil{return err}
	current = project
	var directories [128]writerWindowsDirectory;directories[0]=writerWindowsDirectory{"",project};directoryCount:=1;var files [128]writerWindowsFile
	for index, item := range entries {
		current = project
		pieces = strings.Split(item.path, "/")
		prefixEnd:=0
		for _, piece := range pieces[:len(pieces)-1] {
			prefixEnd+=len(piece)+1;prefix:=item.path[:prefixEnd-1];found:=false
			for i:=0;i<directoryCount;i++{if directories[i].path==prefix{current=directories[i].handle;found=true;break}};if found{continue}
			if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
			if directoryCount==len(directories){return fmt.Errorf("native writer directory capacity exhausted")}
			child, e := createPrivate(piece, current, fileListDirectory|windows.FILE_GENERIC_WRITE|deleteAccess|synchronize, fileDirectoryFile|fileSynchronousIoNonalert)
			if e!=nil{return e}
			handles = append(handles, child)
			if e=verifyPrivateDescriptor(child);e!=nil{return e}
			directories[directoryCount]=writerWindowsDirectory{prefix,child};directoryCount++
			current = child
		}
		if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
		file, e := createOutputFile(pieces[len(pieces)-1], current, windows.FILE_READ_DATA|fileWriteData|deleteAccess|synchronize, fileSynchronousIoNonalert, item.mode)
		if e != nil {
			return e
		}
		handles = append(handles, file)
		observedMode, e := observeOutputMode(file)
		if e != nil || observedMode != item.mode {
			return fmt.Errorf("output native mode does not match plan")
		}
		written := uint32(0)
		if e = syscall.WriteFile(file, item.bytes, &written, nil); e != nil || int(written) != len(item.bytes) {
			if e != nil {
				return e
			}
			return fmt.Errorf("short native write")
		}
		if e = syscall.FlushFileBuffers(file); e != nil {
			return e
		}
		actual,e:=readbackOriginalFile(file,item);if e!=nil{return e};if digest!=nil{if e=digest.add(item.path,actual);e!=nil{return e}}
		files[index]=writerWindowsFile{handle:file,identity:actual.nativeIdentity}
		observedMode, e = observeOutputMode(file)
		if e != nil || observedMode != item.mode {
			return fmt.Errorf("output native mode changed")
		}
		if err = testGate("after-file-write"); err != nil {
			return err
		}
	}
	if err=testGate("before-project-commit");err!=nil{return err}
	if err=verifyWindowsDiscovery(entries,files[:len(entries)],directories[:directoryCount]);err!=nil{return err}
	for i:=directoryCount-1;i>=0;i--{if err=syscall.FlushFileBuffers(directories[i].handle);err!=nil{return err}}
	if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
	originalIdentity,_,err:=writerWindowsIdentity(project);if err!=nil{return err}
	if err=commitCandidateNt(project,commitParent,finalLeaf);err!=nil{return err}
	// Read-only final-name observation permits the existing original writer's
	// write/delete access in its share mask; it grants neither access itself.
	reopened,err:=nativeOpenShared(finalLeaf,commitParent,fileListDirectory|readControl|synchronize,fileOpen,fileDirectoryFile|fileSynchronousIoNonalert,nil,0x7);if err!=nil{return err};handles=append(handles,reopened)
	heldIdentity,_,err:=writerWindowsIdentity(project);if err!=nil{return err};reopenedIdentity,_,err:=writerWindowsIdentity(reopened);if err!=nil||originalIdentity!=heldIdentity||heldIdentity!=reopenedIdentity{return fmt.Errorf("renamed original identity differs")}
	if err=verifyPrivateDescriptor(reopened);err!=nil{return err}
	if err=verifyWindowsDiscovery(entries,files[:len(entries)],directories[:directoryCount]);err!=nil{return err}
	return syscall.FlushFileBuffers(commitParent)
}
