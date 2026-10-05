//go:build linux
package main

import (
 "crypto/sha256"
 "encoding/binary"
 "errors"
 "io"
 "strings"
 "golang.org/x/sys/unix"
)
type nativeInputRoot struct { handles []int; fd int }
type nativeInputLeaf struct { fd int; before unix.Stat_t; size uint32 }
func openNativeInputRoot(root string)(*nativeInputRoot,error){
 if !strings.HasPrefix(root,"/")||root=="/"||strings.Contains(root,"\x00"){return nil,errNativeProtocol}
 parts:=strings.Split(strings.TrimPrefix(root,"/"),"/");for _,part:=range parts{if part==""||part=="."||part==".."{return nil,errNativeProtocol}}
 fd,err:=unix.Open("/",unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_CLOEXEC,0);if err!=nil{return nil,err};held:=&nativeInputRoot{handles:[]int{fd},fd:fd}
 for _,part:=range parts{next,e:=unix.Openat(held.fd,part,unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0);if e!=nil{held.close();return nil,e};held.handles=append(held.handles,next);held.fd=next};return held,nil
}
func(r *nativeInputRoot)close()error{var failure error;for i:=len(r.handles)-1;i>=0;i--{if e:=unix.Close(r.handles[i]);e!=nil{failure=errors.Join(failure,e)}};r.handles=nil;return failure}
func(r *nativeInputRoot)open(name string,size uint32)(*nativeInputLeaf,error){
 allowed:=false;for _,fixed:=range nativeAssetNames{if name==fixed{allowed=true}};if !allowed{return nil,errNativeProtocol}
 fd,err:=unix.Openat(r.fd,name,unix.O_RDONLY|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0);if err!=nil{return nil,err};var before unix.Stat_t
 if err=unix.Fstat(fd,&before);err!=nil||before.Mode&unix.S_IFMT!=unix.S_IFREG||before.Size!=int64(size){unix.Close(fd);return nil,errNativeProtocol};return &nativeInputLeaf{fd,before,size},nil
}
func(l *nativeInputLeaf)readExact(bytes []byte)([32]byte,error){
 var identity [32]byte;if len(bytes)!=int(l.size){return identity,errNativeProtocol};for offset:=0;offset<len(bytes);{n,err:=unix.Read(l.fd,bytes[offset:]);if err==unix.EINTR{continue};if err!=nil{return identity,err};if n==0{return identity,io.ErrUnexpectedEOF};offset+=n}
 var probe [1]byte;n,err:=unix.Read(l.fd,probe[:]);if err!=nil||n!=0{return identity,errNativeProtocol};var after unix.Stat_t;if err=unix.Fstat(l.fd,&after);err!=nil{return identity,err}
 if after.Mode&unix.S_IFMT!=unix.S_IFREG||after.Dev!=l.before.Dev||after.Ino!=l.before.Ino||after.Size!=l.before.Size||after.Mtim!=l.before.Mtim||after.Ctim!=l.before.Ctim{return identity,errNativeProtocol}
 var raw [48]byte;binary.BigEndian.PutUint64(raw[0:8],uint64(after.Dev));binary.BigEndian.PutUint64(raw[8:16],after.Ino);binary.BigEndian.PutUint64(raw[16:24],uint64(after.Size));binary.BigEndian.PutUint64(raw[24:32],uint64(after.Mode));binary.BigEndian.PutUint64(raw[32:40],uint64(after.Mtim.Sec));binary.BigEndian.PutUint64(raw[40:48],uint64(after.Mtim.Nsec));return sha256.Sum256(raw[:]),nil
}
func(l *nativeInputLeaf)close()error{return unix.Close(l.fd)}
