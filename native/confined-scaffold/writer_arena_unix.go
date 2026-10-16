//go:build linux || darwin

package main

import (
 "fmt"
 "golang.org/x/sys/unix"
)

type writerInputArena struct{bytes []byte;closeAttempted bool}
func openWriterInputArena(size int)(*writerInputArena,error){
 if size!=writerCommandCapacity+1{return nil,fmt.Errorf("invalid writer arena capacity")}
 bytes,e:=unix.Mmap(-1,0,size,unix.PROT_READ|unix.PROT_WRITE,unix.MAP_PRIVATE|unix.MAP_ANON);if e!=nil{return nil,e}
 return &writerInputArena{bytes:bytes},nil
}
func(a *writerInputArena)closeOriginal()error{
 if a==nil||a.closeAttempted||len(a.bytes)!=writerCommandCapacity+1{return fmt.Errorf("writer arena custody unavailable")};a.closeAttempted=true
 if e:=unix.Munmap(a.bytes);e!=nil{return e};a.bytes=nil;return nil
}
