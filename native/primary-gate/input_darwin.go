//go:build darwin
package main

import (
 "crypto/sha256"
 "encoding/binary"
 "errors"
 "io"
 "strings"
 "golang.org/x/sys/unix"
)

type nativeInputRoot struct { handles [124]int; attempted [124]bool; count int; fd int; leaves [4]*nativeInputLeaf; leafCount int; closeErr error }
type nativeInputLeaf struct { fd int; before unix.Stat_t; size uint32; closeAttempted bool; closeErr error }
var originalInputRoot *nativeInputRoot
func openNativeInputRoot(root string)(*nativeInputRoot,error){
 if originalInputRoot!=nil{return nil,errNativeProtocol}
 if !strings.HasPrefix(root,"/")||root=="/"||strings.Contains(root,"\x00"){return nil,errNativeProtocol}
 parts:=strings.Split(strings.TrimPrefix(root,"/"),"/");if len(parts)+1>124{return nil,errNativeCapacity};for _,part:=range parts{if part==""||part=="."||part==".."{return nil,errNativeProtocol}}
 held:=&nativeInputRoot{fd:-1};originalInputRoot=held
 held.handles[0]=-1;held.count=1;fd,err:=unix.Open("/",unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0);held.handles[0]=fd;if err!=nil{return nil,err};held.fd=fd
 var rootStat unix.Stat_t;if unix.Fstat(fd,&rootStat)!=nil||rootStat.Mode&unix.S_IFMT!=unix.S_IFDIR{held.close();return nil,errNativeProtocol}
 for _,part:=range parts{index:=held.count;held.handles[index]=-1;held.count++;next,e:=unix.Openat(held.fd,part,unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0);held.handles[index]=next;if e!=nil{held.close();return nil,e};held.fd=next;var stat unix.Stat_t;if unix.Fstat(next,&stat)!=nil||stat.Mode&unix.S_IFMT!=unix.S_IFDIR{held.close();return nil,errNativeProtocol}};return held,nil
}
func(r *nativeInputRoot)close()error{
 if r==nil{return errNativeProtocol};for i:=r.leafCount-1;i>=0;i--{if e:=r.leaves[i].close();e!=nil&&r.closeErr==nil{r.closeErr=e}}
 for i:=r.count-1;i>=0;i--{if r.attempted[i]||r.handles[i]<0{continue};r.attempted[i]=true;if e:=unix.Close(r.handles[i]);e!=nil&&r.closeErr==nil{r.closeErr=e}};return r.closeErr
}
func(r *nativeInputRoot)open(name string,size uint32)(*nativeInputLeaf,error){
 if r==nil||r!=originalInputRoot||r.fd<0||r.closeErr!=nil||r.leafCount>=4{return nil,errNativeProtocol};allowed:=false;for _,fixed:=range nativeAssetNames{if name==fixed{allowed=true}};if !allowed{return nil,errNativeProtocol}
 for i:=0;i<r.count;i++{if r.attempted[i]{return nil,errNativeProtocol}}
 leaf:=&nativeInputLeaf{fd:-1,size:size};r.leaves[r.leafCount]=leaf;r.leafCount++
 fd,err:=unix.Openat(r.fd,name,unix.O_RDONLY|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0);leaf.fd=fd;if err!=nil{return nil,err}
 if err=unix.Fstat(fd,&leaf.before);err!=nil{return nil,err};if leaf.before.Mode&unix.S_IFMT!=unix.S_IFREG{return nil,errNativeProtocol};if leaf.before.Size!=int64(size){return nil,errNativeInputChanged};return leaf,nil
}
func(l *nativeInputLeaf)readExact(bytes []byte)([32]byte,error){
 var identity [32]byte;if l==nil||l.fd<0||l.closeAttempted||len(bytes)!=int(l.size){return identity,errNativeProtocol};for offset:=0;offset<len(bytes);{n,err:=unix.Read(l.fd,bytes[offset:]);if err==unix.EINTR{continue};if err!=nil{return identity,err};if n==0{return identity,io.ErrUnexpectedEOF};offset+=n}
 var probe [1]byte;var n int;var err error;for{n,err=unix.Read(l.fd,probe[:]);if err==unix.EINTR{continue};break};if err!=nil{return identity,err};if n!=0{return identity,errNativeInputChanged};var after unix.Stat_t;if err=unix.Fstat(l.fd,&after);err!=nil{return identity,err}
 if after.Mode!=l.before.Mode||after.Uid!=l.before.Uid||after.Gid!=l.before.Gid||after.Dev!=l.before.Dev||after.Ino!=l.before.Ino||after.Size!=l.before.Size||after.Mtimespec!=l.before.Mtimespec||after.Ctimespec!=l.before.Ctimespec{return identity,errNativeInputChanged}
 var raw [48]byte;binary.BigEndian.PutUint64(raw[0:8],uint64(after.Dev));binary.BigEndian.PutUint64(raw[8:16],after.Ino);binary.BigEndian.PutUint64(raw[16:24],uint64(after.Size));binary.BigEndian.PutUint64(raw[24:32],uint64(after.Mode));binary.BigEndian.PutUint64(raw[32:40],uint64(after.Mtimespec.Sec));binary.BigEndian.PutUint64(raw[40:48],uint64(after.Mtimespec.Nsec));return sha256.Sum256(raw[:]),nil
}
func(l *nativeInputLeaf)close()error{if l==nil{return errNativeProtocol};if l.closeAttempted{return l.closeErr};l.closeAttempted=true;if l.fd>=0{l.closeErr=unix.Close(l.fd)};return l.closeErr}
func nativeInputPlatformCode(failure error)byte{if errors.Is(failure,unix.ELOOP)||errors.Is(failure,unix.ENOTDIR){return 2};return 1}
