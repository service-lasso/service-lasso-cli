//go:build windows

package hostadmission

import (
 "encoding/binary"
 "syscall"
 "unsafe"
 "golang.org/x/sys/windows"
)

var sourceQueryObject=syscall.NewLazyDLL("ntdll.dll").NewProc("NtQueryObject")
var sourceHandleInformation=syscall.NewLazyDLL("kernel32.dll").NewProc("GetHandleInformation")
var sourceQueryFile=syscall.NewLazyDLL("ntdll.dll").NewProc("NtQueryInformationFile")
func observeSourceObject(handle windows.Handle)(sourceObjectMetadata,error){
 var out sourceObjectMetadata;if handle==0||handle==windows.InvalidHandle{return out,ErrInvalid}
 kind,e:=windows.GetFileType(handle);if e!=nil||kind!=windows.FILE_TYPE_DISK{return out,ErrInvalid}
 // Actual native granted rights, never requested access or a caller assertion.
 var basic [7]uint64;var returned uint32
 status,_,_:=sourceQueryObject.Call(uintptr(handle),0,uintptr(unsafe.Pointer(&basic[0])),unsafe.Sizeof(basic),uintptr(unsafe.Pointer(&returned)))
 if int32(status)<0||returned!=uint32(unsafe.Sizeof(basic)){return out,ErrInvalid}
 basicRaw:=unsafe.Slice((*byte)(unsafe.Pointer(&basic[0])),int(unsafe.Sizeof(basic)))
 access:=binary.LittleEndian.Uint32(basicRaw[4:8])
 const forbidden uint32=0x00000002|0x00000004|0x00000010|0x00000100|0x00010000|0x00040000|0x00080000|0x10000000|0x40000000
 if access&0x00120081!=0x00120081||access&forbidden!=0{return out,ErrInvalid}
 var handleFlags uint32;ok,_,_:=sourceHandleInformation.Call(uintptr(handle),uintptr(unsafe.Pointer(&handleFlags)));if ok==0||handleFlags&1!=0{return out,ErrInvalid}
 // Source leases use original synchronous read objects. An asynchronous handle
 // cannot make lpNumberOfBytesRead into an authoritative completion count.
 var mode uint32;var io nativeStatus
 status,_,_=sourceQueryFile.Call(uintptr(handle),uintptr(unsafe.Pointer(&io)),uintptr(unsafe.Pointer(&mode)),unsafe.Sizeof(mode),16)
 if int32(status)<0||int32(io.status)<0||io.information!=unsafe.Sizeof(mode)||mode&(0x10|0x20)==0{return out,ErrInvalid}
 var originalBasic [5]uint64;var basicIO nativeStatus
 status,_,_=sourceQueryFile.Call(uintptr(handle),uintptr(unsafe.Pointer(&basicIO)),uintptr(unsafe.Pointer(&originalBasic[0])),unsafe.Sizeof(originalBasic),4)
 if int32(status)<0||int32(basicIO.status)<0||basicIO.information!=unsafe.Sizeof(originalBasic){return out,ErrInvalid}
 copy(out.nativeBasic[:],unsafe.Slice((*byte)(unsafe.Pointer(&originalBasic[0])),int(unsafe.Sizeof(originalBasic))))
 var info windows.ByHandleFileInformation
 if windows.GetFileInformationByHandle(handle,&info)!=nil||info.FileAttributes&(windows.FILE_ATTRIBUTE_DIRECTORY|windows.FILE_ATTRIBUTE_REPARSE_POINT)!=0{return out,ErrInvalid}
 identity,e:=nativeDiskIdentity(handle);if e!=nil{return out,e}
 owner,e:=getFileOwner(handle);if e!=nil||len(owner)==0||len(owner)>184{return out,ErrInvalid}
 out.owner=owner;out.size=uint64(info.FileSizeHigh)<<32|uint64(info.FileSizeLow)
 raw:=out.raw[:];copy(raw[:24],identity[:]);binary.BigEndian.PutUint32(raw[24:28],info.FileAttributes)
 binary.BigEndian.PutUint64(raw[28:36],out.size);binary.BigEndian.PutUint32(raw[36:40],info.CreationTime.LowDateTime);binary.BigEndian.PutUint32(raw[40:44],info.CreationTime.HighDateTime)
 binary.BigEndian.PutUint32(raw[44:48],info.LastWriteTime.LowDateTime);binary.BigEndian.PutUint32(raw[48:52],info.LastWriteTime.HighDateTime)
 binary.BigEndian.PutUint32(raw[52:56],info.NumberOfLinks);binary.BigEndian.PutUint32(raw[56:60],access);binary.BigEndian.PutUint32(raw[60:64],mode);binary.BigEndian.PutUint64(raw[64:72],originalBasic[3]);binary.BigEndian.PutUint32(raw[72:76],handleFlags);out.rawBytes=76
 out.identity=DomainHash("SLCLI-ORIGINAL-SOURCE-OBJECT-1\x00",identity[:],[]byte(owner));return out,nil
}
func readSourceObjectAt(handle windows.Handle,destination []byte,offset uint64)(int,error){
 if handle==0||handle==windows.InvalidHandle||len(destination)==0||offset>FrameBytes{return 0,ErrInvalid}
 return readSourceEvidenceAt(handle,destination,offset)
}
func readSourceEvidenceAt(handle windows.Handle,destination []byte,offset uint64)(int,error){
 if handle==0||handle==windows.InvalidHandle||len(destination)==0||len(destination)>FrameBytes||offset>SessionBytes{return 0,ErrInvalid}
 actual,e:=windows.SetFilePointer(handle,int32(offset),nil,windows.FILE_BEGIN);if e!=nil||uint64(actual)!=offset{return 0,ErrInvalid}
 var transferred uint32;e=windows.ReadFile(handle,destination,&transferred,nil)
 if transferred>uint32(len(destination)){return 0,ErrInvalid};return int(transferred),e
}
// Preserve the original ReadFile result; only a genuine zero-byte completion
// or native ERROR_HANDLE_EOF with zero bytes establishes the final extra read.
func sourceObjectEOF(count int,err error)bool{return count==0&&(err==nil||err==windows.ERROR_HANDLE_EOF)}
