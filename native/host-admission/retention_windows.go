//go:build windows

package hostadmission

import (
 "crypto/sha256"
 "encoding/binary"
 "strings"
 "sync"
 "sync/atomic"
 "unicode/utf16"
 "golang.org/x/sys/windows"
)

const nativeReadOperation = "win32.read"
type nativeRetentionRoot struct { mu sync.Mutex; handle windows.Handle; owner string; identity,profile [32]byte; files [4096]*nativeEvidenceFile; count uint32; failed atomic.Bool }
type nativeEvidenceFile struct { mu sync.Mutex; root *nativeRetentionRoot; handle windows.Handle; identity,run,source [32]byte; maximum,count uint64; failed bool }
func openRetentionRoot(p *Profile)(*nativeRetentionRoot,error){
 var profileError error;p,profileError=originalProfile(p);if profileError!=nil{return nil,profileError}
 if p==nil||p.Platform!="win32"{return nil,ErrUnavailable};var grant *nativeGrant;for i:=range nativeGrants{if nativeGrants[i].profileDigest==p.Digest{if grant!=nil{return nil,ErrUnavailable};grant=&nativeGrants[i]}};if grant==nil{return nil,ErrUnavailable}
 handle,e:=heldWindowsDirectory(grant.retentionPath,p.Owner,true);if e!=nil{return nil,e};info,e:=winInfo(handle,true,p.Owner);identity,idError:=winRootDigest(handle,info,p.Owner);if e!=nil||idError!=nil||digestString(identity)!=p.RetentionRootIdentity{windows.CloseHandle(handle);return nil,ErrUnavailable}
 return &nativeRetentionRoot{handle:handle,owner:p.Owner,identity:identity,profile:p.Digest},nil
}
func(r *nativeRetentionRoot)recheckIdentity()error{if r==nil||r.failed.Load(){return ErrUnavailable};info,e:=winInfo(r.handle,true,r.owner);identity,idError:=winRootDigest(r.handle,info,r.owner);if e!=nil||idError!=nil||identity!=r.identity{r.failed.Store(true);return ErrUnavailable};return nil}
func(r *nativeRetentionRoot)verifyOriginal(p *Profile)error{if p==nil||r==nil||p.Digest!=r.profile||digestString(r.identity)!=p.RetentionRootIdentity{return ErrUnavailable};return r.recheckIdentity()}
func(r *nativeRetentionRoot)totalOriginalBytes(maximum uint64)(uint64,error){
 r.mu.Lock();defer r.mu.Unlock();if e:=r.recheckIdentity();e!=nil{return 0,e};var raw [FrameBytes]byte;var total uint64;count:=0;class:=uint32(11)
 for{e:=windows.GetFileInformationByHandleEx(r.handle,class,&raw[0],uint32(len(raw)));class=10;if e==windows.ERROR_NO_MORE_FILES{break};if e!=nil{r.failed.Store(true);return 0,ErrUnavailable};offset:=0
  for{if offset+104>len(raw){r.failed.Store(true);return 0,ErrInvalid};next:=binary.LittleEndian.Uint32(raw[offset:]);size:=binary.LittleEndian.Uint64(raw[offset+40:]);attributes:=binary.LittleEndian.Uint32(raw[offset+56:]);length:=binary.LittleEndian.Uint32(raw[offset+60:]);if length%2!=0||length==0||length>480||offset+104+int(length)>len(raw){r.failed.Store(true);return 0,ErrInvalid};var units [240]uint16;for i:=0;i<int(length)/2;i++{units[i]=binary.LittleEndian.Uint16(raw[offset+104+2*i:])};name:=string(utf16.Decode(units[:length/2]))
   if name!="."&&name!=".."{count++;if count>65536||!evidenceName(name)||strings.IndexByte(name,'.')!=64||attributes&(windows.FILE_ATTRIBUTE_DIRECTORY|windows.FILE_ATTRIBUTE_REPARSE_POINT)!=0||size>maximum-total{r.failed.Store(true);return 0,ErrUnavailable};handle,e:=ntStoreOpen(name,r.handle,0x0001|0x20000|0x100000,1,0x20,nil);if e!=nil{r.failed.Store(true);return 0,ErrUnavailable};info,infoErr:=winInfo(handle,false,r.owner);closeErr:=windows.CloseHandle(handle);if infoErr!=nil||closeErr!=nil||winSize(info)!=size{r.failed.Store(true);return 0,ErrUnavailable};total+=size}
   if next==0{break};if next<104||int(next)>len(raw)-offset{r.failed.Store(true);return 0,ErrInvalid};offset+=int(next)
  }
 };return total,r.recheckIdentity()
}
func(r *nativeRetentionRoot)createOriginal(name string,maximum uint64,run,source [32]byte)(*nativeEvidenceFile,error){
 leaf,e:=retentionLeaf(run,name);if e!=nil{return nil,e};r.mu.Lock();defer r.mu.Unlock();if maximum==0||source==([32]byte{})||r.count>=uint32(len(r.files)){return nil,ErrCapacity};if e:=r.recheckIdentity();e!=nil{return nil,e}
 file:=&nativeEvidenceFile{root:r,maximum:maximum,run:run,source:source};r.files[r.count]=file;r.count++;descriptor,e:=windows.SecurityDescriptorFromString(ownerDACL);if e!=nil{file.failed=true;r.failed.Store(true);return nil,ErrUnavailable}
 handle,e:=ntStoreOpen(leaf,r.handle,0x0001|0x0002|0x20000|0x100000,2,0x20,descriptor);if e!=nil{file.failed=true;r.failed.Store(true);return nil,ErrUnavailable};file.handle=handle;info,e:=winInfo(handle,false,r.owner)
 if e!=nil||winSize(info)!=0{file.failed=true;r.failed.Store(true);return nil,ErrUnavailable};file.identity,e=winRootDigest(handle,info,r.owner);if e!=nil{file.failed=true;r.failed.Store(true);return nil,e};if windows.FlushFileBuffers(r.handle)!=nil{file.failed=true;r.failed.Store(true);return nil,ErrUnavailable};return file,nil
}
func(f *nativeEvidenceFile)recheckOriginal()error{if f==nil{return ErrUnavailable};f.mu.Lock();defer f.mu.Unlock();return f.recheckLocked()}
func(f *nativeEvidenceFile)recheckLocked()error{if f.failed||f.handle==0{return ErrUnavailable};if e:=f.root.recheckIdentity();e!=nil{return e};info,e:=winInfo(f.handle,false,f.root.owner);identity,idError:=winRootDigest(f.handle,info,f.root.owner);if e!=nil||idError!=nil||identity!=f.identity||winSize(info)!=f.count{f.failed=true;return ErrUnavailable};return nil}
func(f *nativeEvidenceFile)writeOriginal(raw []byte)error{
 f.mu.Lock();defer f.mu.Unlock();if e:=f.recheckLocked();e!=nil{return e};if uint64(len(raw))>f.maximum-f.count{f.failed=true;return ErrCapacity}
 for offset:=0;offset<len(raw);{var n uint32;e:=windows.WriteFile(f.handle,raw[offset:],&n,nil);if e!=nil||n==0{f.failed=true;return ErrUnavailable};offset+=int(n);f.count+=uint64(n)};return f.recheckLocked()
}
func(f *nativeEvidenceFile)flushAndReadback(count uint64,digest []byte)error{
 f.mu.Lock();defer f.mu.Unlock();if len(digest)!=32||count!=f.count{return ErrInvalid};if e:=f.recheckLocked();e!=nil{return e};if windows.FlushFileBuffers(f.handle)!=nil{f.failed=true;return ErrUnavailable};before,e:=winInfo(f.handle,false,f.root.owner);if e!=nil{f.failed=true;return ErrUnavailable}
 if _,e=windows.Seek(f.handle,0,0);e!=nil{f.failed=true;return ErrUnavailable};h:=sha256.New();var buffer [FrameBytes]byte;var offset uint64
 for offset<count{want:=int(count-offset);if want>len(buffer){want=len(buffer)};var n uint32;e=windows.ReadFile(f.handle,buffer[:want],&n,nil);if e!=nil||n==0{f.failed=true;return ErrUnavailable};h.Write(buffer[:n]);offset+=uint64(n)}
 var extra [1]byte;var n uint32;e=windows.ReadFile(f.handle,extra[:],&n,nil);if e!=nil&&e!=windows.ERROR_HANDLE_EOF||n!=0{f.failed=true;return ErrUnavailable};after,e:=winInfo(f.handle,false,f.root.owner)
 if e!=nil||!sameWindowsFile(before,after)||!bytesEqual(h.Sum(nil),digest){f.failed=true;return ErrUnavailable};if _,e=windows.Seek(f.handle,int64(count),0);e!=nil||windows.FlushFileBuffers(f.root.handle)!=nil{f.failed=true;return ErrUnavailable};return nil
}
