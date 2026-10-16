//go:build windows

package main

import (
 "fmt"
 "unsafe"
 "golang.org/x/sys/windows"
)

type writerInputArena struct{bytes []byte;address uintptr;closeAttempted bool}
func openWriterInputArena(size int)(*writerInputArena,error){
 if size!=writerCommandCapacity+1{return nil,fmt.Errorf("invalid writer arena capacity")}
 address,e:=windows.VirtualAlloc(0,uintptr(size),windows.MEM_RESERVE|windows.MEM_COMMIT,windows.PAGE_READWRITE);if e!=nil{return nil,e};if address==0{return nil,fmt.Errorf("writer original arena unavailable")}
 return &writerInputArena{bytes:unsafe.Slice((*byte)(unsafe.Pointer(address)),size),address:address},nil
}
func(a *writerInputArena)closeOriginal()error{
 if a==nil||a.closeAttempted||a.address==0||len(a.bytes)!=writerCommandCapacity+1{return fmt.Errorf("writer arena custody unavailable")};a.closeAttempted=true
 if e:=windows.VirtualFree(a.address,0,windows.MEM_RELEASE);e!=nil{return e};a.bytes=nil;a.address=0;return nil
}
