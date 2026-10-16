//go:build linux

package hostadmission

import (
 "crypto/sha256"
 "encoding/hex"
 "io"
 "sync"
 "sync/atomic"
 "strings"
 "golang.org/x/sys/unix"
)

const nativeReadOperation = "linux.read"

type nativeRetentionRoot struct { mu sync.Mutex; handle int; owner uint32; identity,profile [32]byte; files [4096]*nativeEvidenceFile; count uint32; failed atomic.Bool }
type nativeEvidenceFile struct { mu sync.Mutex; root *nativeRetentionRoot; handle int; identity,run,source [32]byte; maximum,count uint64; failed bool }

func openRetentionRoot(p *Profile)(*nativeRetentionRoot,error){
 var profileError error;p,profileError=originalProfile(p);if profileError!=nil{return nil,profileError}
 if p==nil||p.Platform!="linux"{return nil,ErrUnavailable};owner,e:=strconvUint32(p.Owner);if e!=nil{return nil,e};var grant *nativeGrant
 for i:=range nativeGrants{if nativeGrants[i].profileDigest==p.Digest{if grant!=nil{return nil,ErrUnavailable};grant=&nativeGrants[i]}};if grant==nil{return nil,ErrUnavailable}
 fd,e:=heldAbsoluteDirectory(grant.retentionPath,owner,true);if e!=nil{return nil,e};var stat unix.Stat_t;if unix.Fstat(fd,&stat)!=nil{unix.Close(fd);return nil,ErrUnavailable};identity:=nativeRootDigest(&stat)
 if hex.EncodeToString(identity[:])!=p.RetentionRootIdentity{unix.Close(fd);return nil,ErrUnavailable}
 return &nativeRetentionRoot{handle:fd,owner:owner,identity:identity,profile:p.Digest},nil
}
func(r *nativeRetentionRoot)recheckIdentity()error{if r==nil||r.failed.Load(){return ErrUnavailable};var stat unix.Stat_t;if unix.Fstat(r.handle,&stat)!=nil||stat.Mode&unix.S_IFMT!=unix.S_IFDIR||stat.Mode&07777!=0700||stat.Uid!=r.owner||nativeRootDigest(&stat)!=r.identity{r.failed.Store(true);return ErrUnavailable};return nil}
func(r *nativeRetentionRoot)verifyOriginal(p *Profile)error{if p==nil||r==nil||p.Digest!=r.profile||digestString(r.identity)!=p.RetentionRootIdentity{return ErrUnavailable};return r.recheckIdentity()}
func(r *nativeRetentionRoot)createOriginal(name string,maximum uint64,run,source [32]byte)(*nativeEvidenceFile,error){
 leaf,e:=retentionLeaf(run,name);if e!=nil{return nil,e};r.mu.Lock();defer r.mu.Unlock();if maximum==0||source==([32]byte{})||r.count>=uint32(len(r.files)){return nil,ErrCapacity};if e:=r.recheckIdentity();e!=nil{return nil,e}
 // Reserve the custody-table cell before creating an original kernel object.
 file:=&nativeEvidenceFile{root:r,handle:-1,maximum:maximum,run:run,source:source};r.files[r.count]=file;r.count++
 fd,e:=unix.Openat(r.handle,leaf,unix.O_RDWR|unix.O_CREAT|unix.O_EXCL|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0600);if e!=nil{file.failed=true;r.failed.Store(true);return nil,ErrUnavailable};file.handle=fd
 var stat unix.Stat_t;if unix.Fstat(fd,&stat)!=nil||stat.Mode&unix.S_IFMT!=unix.S_IFREG||stat.Mode&07777!=0600||stat.Uid!=r.owner||stat.Size!=0{file.failed=true;r.failed.Store(true);return nil,ErrUnavailable};file.identity=nativeRootDigest(&stat)
 if unix.Fsync(r.handle)!=nil{file.failed=true;r.failed.Store(true);return nil,ErrUnavailable};return file,nil
}
func(r *nativeRetentionRoot)totalOriginalBytes(maximum uint64)(uint64,error){
 r.mu.Lock();defer r.mu.Unlock();if e:=r.recheckIdentity();e!=nil{return 0,e};fd,e:=unix.Openat(r.handle,".",unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_CLOEXEC,0);if e!=nil{return 0,ErrUnavailable};var total uint64;var buffer [FrameBytes]byte;count:=0
 for{n,e:=unix.ReadDirent(fd,buffer[:]);if e==unix.EINTR{continue};if e!=nil{unix.Close(fd);r.failed.Store(true);return 0,ErrUnavailable};if n==0{break};_,_,names:=unix.ParseDirent(buffer[:n],-1,nil);for _,name:=range names{count++;if count>65536||!evidenceName(name)||strings.IndexByte(name,'.')!=64{unix.Close(fd);r.failed.Store(true);return 0,ErrUnavailable};var info unix.Stat_t;if unix.Fstatat(r.handle,name,&info,unix.AT_SYMLINK_NOFOLLOW)!=nil||info.Mode&unix.S_IFMT!=unix.S_IFREG||info.Mode&07777!=0600||info.Uid!=r.owner||info.Size<0||uint64(info.Size)>maximum-total{unix.Close(fd);r.failed.Store(true);return 0,ErrUnavailable};total+=uint64(info.Size)}}
 if unix.Close(fd)!=nil{r.failed.Store(true);return 0,ErrUnavailable};return total,r.recheckIdentity()
}
func(f *nativeEvidenceFile)recheckOriginal()error{if f==nil{return ErrUnavailable};f.mu.Lock();defer f.mu.Unlock();return f.recheckLocked()}
func(f *nativeEvidenceFile)recheckLocked()error{if f.failed||f.handle<0{return ErrUnavailable};if e:=f.root.recheckIdentity();e!=nil{return e};var stat unix.Stat_t;if unix.Fstat(f.handle,&stat)!=nil||stat.Mode&unix.S_IFMT!=unix.S_IFREG||stat.Mode&07777!=0600||stat.Uid!=f.root.owner||nativeRootDigest(&stat)!=f.identity||stat.Size<0||uint64(stat.Size)!=f.count{f.failed=true;return ErrUnavailable};return nil}
func(f *nativeEvidenceFile)writeOriginal(raw []byte)error{
 f.mu.Lock();defer f.mu.Unlock();if e:=f.recheckLocked();e!=nil{return e};if uint64(len(raw))>f.maximum-f.count{f.failed=true;return ErrCapacity}
 for offset:=0;offset<len(raw);{n,e:=unix.Write(f.handle,raw[offset:]);if e==unix.EINTR{continue};if e!=nil||n<=0{f.failed=true;return ErrUnavailable};offset+=n;f.count+=uint64(n)};return f.recheckLocked()
}
func(f *nativeEvidenceFile)flushAndReadback(count uint64,digest []byte)error{
 f.mu.Lock();defer f.mu.Unlock();if len(digest)!=32||count!=f.count{return ErrInvalid};if e:=f.recheckLocked();e!=nil{return e};if unix.Fsync(f.handle)!=nil{f.failed=true;return ErrUnavailable}
 var before unix.Stat_t;if unix.Fstat(f.handle,&before)!=nil{f.failed=true;return ErrUnavailable};h:=sha256.New();var buffer [FrameBytes]byte;var offset int64
 for uint64(offset)<count{want:=int(count-uint64(offset));if want>len(buffer){want=len(buffer)};n,e:=unix.Pread(f.handle,buffer[:want],offset);if e==unix.EINTR{continue};if e!=nil||n<=0{f.failed=true;return ErrUnavailable};h.Write(buffer[:n]);offset+=int64(n)}
 var extra [1]byte;n,e:=unix.Pread(f.handle,extra[:],offset);if e!=nil&&e!=io.EOF||n!=0{f.failed=true;return ErrUnavailable};var after unix.Stat_t
 if unix.Fstat(f.handle,&after)!=nil||!sameNativeFile(&before,&after)||!bytesEqual(h.Sum(nil),digest)||unix.Fsync(f.root.handle)!=nil{f.failed=true;return ErrUnavailable};return nil
}
