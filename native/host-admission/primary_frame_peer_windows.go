//go:build windows

package hostadmission

import (
 "unsafe"
 "golang.org/x/sys/windows"
)

var originalPipeInformation=objectKernel.NewProc("GetNamedPipeInfo")
func correlateOriginalFramePeer(channel windows.Handle,original *nativeProcess)error{
 if original==nil{return ErrInvalid};var flags uint32
 ok,_,_:=originalPipeInformation.Call(uintptr(channel),uintptr(unsafe.Pointer(&flags)),0,0,0)
 if ok==0||flags!=1{return ErrInvalid} // Original byte-stream server endpoint.
 var actual uint32;ok,_,_=pipeClientPID.Call(uintptr(channel),uintptr(unsafe.Pointer(&actual)))
 if ok==0||actual==0||actual!=original.pid{return ErrInvalid}
 return original.recheckOriginal()
}
