//go:build windows

package hostadmission

import (
 "crypto/rand"
 "crypto/sha256"
 "encoding/binary"
 "encoding/hex"
 "io"
 "path/filepath"
 "strings"
 "syscall"
 "sync"
 "unicode/utf16"
 "unsafe"
 "golang.org/x/sys/windows"
)

type nativeGrant struct { profileDigest [32]byte; storePath,keyPath,retentionPath string; clientUsers []string }
type storeOwnerIdentity = string
var nativeGrants=[]nativeGrant{}
type nativeStore struct { root,keyHandle windows.Handle; key [32]byte; rootIdentity [32]byte; owner string; outstanding map[windows.Handle]struct{}; uncertain,initializationAuthorized bool; readOriginal nativeStoreRead; readOrdinal uint64; initialization originalStoreInitialization; operationMu sync.Mutex; operationOrdinal uint64; operation originalStoreOperation; disposed bool }
type nativeStoreRead struct {
 ordinal uint64
 started,openReturned,beforeReturned,identityReturned,readReturned,afterReturned,afterIdentityReturned,closeReturned,complete bool
 name string
 maximum uint32
 handle,rootOriginal windows.Handle
 rootIdentityOriginal [32]byte
 ownerOriginal string
 before,after windows.ByHandleFileInformation
 identity,afterIdentity [24]byte
 raw []byte
 openError,beforeError,identityError,readError,afterError,afterIdentityError,closeError,resultError error
}
type nativeUnicode struct { length,maximum uint16; buffer *uint16 }
type nativeAttributes struct { length uint32; root windows.Handle; name *nativeUnicode; attributes uint32; descriptor,quality uintptr }
type nativeStatus struct { status,information uintptr }
var storeNT=syscall.NewLazyDLL("ntdll.dll")
var storeCreate=storeNT.NewProc("NtCreateFile")
var storeSet=storeNT.NewProc("NtSetInformationFile")
const ownerDACL="D:P(A;;FA;;;OW)"

func ntStoreOpen(name string,parent windows.Handle,access,disposition,options uint32,descriptor *windows.SECURITY_DESCRIPTOR)(windows.Handle,error){
 if parent==0{if !filepath.IsAbs(name){return 0,ErrUnavailable};name=`\??\`+name}
 units,e:=windows.UTF16FromString(name);if e!=nil||len(units)>32767{return 0,ErrUnavailable};u:=nativeUnicode{uint16(2*(len(units)-1)),uint16(2*len(units)),&units[0]}
 attributes:=nativeAttributes{length:uint32(unsafe.Sizeof(nativeAttributes{})),root:parent,name:&u,attributes:0x40|0x1000};if descriptor!=nil{attributes.descriptor=uintptr(unsafe.Pointer(descriptor))}
 var handle windows.Handle;var status nativeStatus
 result,_,_:=storeCreate.Call(uintptr(unsafe.Pointer(&handle)),uintptr(access),uintptr(unsafe.Pointer(&attributes)),uintptr(unsafe.Pointer(&status)),0,0,1,uintptr(disposition),uintptr(options|0x200000),0,0)
 if int32(result)<0{return 0,ErrUnavailable};return handle,nil
}
func winInfo(handle windows.Handle,directory bool,owner string)(windows.ByHandleFileInformation,error){
 var info windows.ByHandleFileInformation;kind,e:=windows.GetFileType(handle);if e!=nil||kind!=windows.FILE_TYPE_DISK{return info,ErrUnavailable}
 if windows.GetFileInformationByHandle(handle,&info)!=nil||info.FileAttributes&windows.FILE_ATTRIBUTE_REPARSE_POINT!=0||(info.FileAttributes&windows.FILE_ATTRIBUTE_DIRECTORY!=0)!=directory||info.FileAttributes&windows.FILE_ATTRIBUTE_READONLY!=0{return info,ErrUnavailable}
 if owner!=""{sd,e:=windows.GetSecurityInfo(handle,windows.SE_FILE_OBJECT,windows.OWNER_SECURITY_INFORMATION|windows.DACL_SECURITY_INFORMATION);if e!=nil||!sd.IsValid(){return info,ErrUnavailable};sid,_,e:=sd.Owner();if e!=nil||sid.String()!=owner{return info,ErrUnavailable};dacl,e:=windows.GetSecurityInfo(handle,windows.SE_FILE_OBJECT,windows.DACL_SECURITY_INFORMATION);if e!=nil||dacl.String()!=ownerDACL{return info,ErrUnavailable};control,_,e:=dacl.Control();if e!=nil||control&windows.SE_DACL_PROTECTED==0{return info,ErrUnavailable}}
 return info,nil
}
func winRootDigest(handle windows.Handle,info windows.ByHandleFileInformation,owner string)([32]byte,error){id,e:=nativeDiskIdentity(handle);if e!=nil{return [32]byte{},e};var raw [28]byte;copy(raw[:24],id[:]);binary.BigEndian.PutUint32(raw[24:],info.FileAttributes);return DomainHash("SLCLI-WINDOWS-ROOT-1\x00",raw[:],[]byte(owner)),nil}
func heldWindowsDirectory(path,owner string,writable bool)(windows.Handle,error){
 volume:=filepath.VolumeName(path);if len(volume)!=2||volume[1]!=':'||!filepath.IsAbs(path)||filepath.Clean(path)!=path||strings.Contains(path,"\x00"){return 0,ErrUnavailable}
 components:=strings.Split(strings.TrimPrefix(path,volume+`\`),`\`);for _,part:=range components{if part==""||part=="."||part==".."||strings.ContainsAny(part,"/:"){return 0,ErrUnavailable}}
 current,e:=ntStoreOpen(volume+`\`,0,0x0001|0x20000|0x100000,1,1|0x20,nil);if e!=nil{return 0,e}
 for i,part:=range components{access:=uint32(0x0001|0x20000|0x100000);if i==len(components)-1&&writable{access|=0x0002|0x10000};next,e:=ntStoreOpen(part,current,access,1,1|0x20,nil);closeErr:=windows.CloseHandle(current);if e!=nil||closeErr!=nil{if next!=0{windows.CloseHandle(next)};return 0,ErrUnavailable};current=next;if _,e=winInfo(current,true,"");e!=nil{windows.CloseHandle(current);return 0,e}}
 if _,e=winInfo(current,true,owner);e!=nil{windows.CloseHandle(current);return 0,e};return current,nil
}
func OpenStore(p *Profile)(*nativeStore,error){
 var admissionError error;p,admissionError=originalProfile(p);if admissionError!=nil{return nil,admissionError}
 if !beginStoreAttempt(){return nil,ErrUnavailable}
 if p==nil||p.Platform!="win32"||p.StoreProvider!="windows-nt-atomic-v1"{return nil,ErrUnavailable}
 token,e:=windows.OpenCurrentProcessToken();if e!=nil{return nil,ErrUnavailable};user,e:=token.GetTokenUser();closeErr:=token.Close();if e!=nil||closeErr!=nil||user.User.Sid.String()!=p.Owner{return nil,ErrUnavailable}
 var selected *nativeGrant;for i:=range nativeGrants{if nativeGrants[i].profileDigest==p.Digest{if selected!=nil{return nil,ErrUnavailable};selected=&nativeGrants[i]}};if selected==nil{return nil,ErrUnavailable}
 root,e:=heldWindowsDirectory(selected.storePath,p.Owner,true);if e!=nil{return nil,e};info,e:=winInfo(root,true,p.Owner);rootIdentity,idError:=winRootDigest(root,info,p.Owner);if e!=nil||idError!=nil||digestString(rootIdentity)!=p.StoreRootIdentity{windows.CloseHandle(root);return nil,ErrUnavailable}
 keyDirectory,keyName:=filepath.Split(selected.keyPath);keyRoot,e:=heldWindowsDirectory(strings.TrimSuffix(keyDirectory,`\`),p.Owner,false);if e!=nil{windows.CloseHandle(root);return nil,e};key,e:=ntStoreOpen(keyName,keyRoot,0x0001|0x20000|0x100000,1,0x20,nil);closeErr=windows.CloseHandle(keyRoot);if e!=nil||closeErr!=nil{if key!=0{windows.CloseHandle(key)};windows.CloseHandle(root);return nil,ErrUnavailable}
 before,e:=winInfo(key,false,p.Owner);if e!=nil||winSize(before)!=32{windows.CloseHandle(key);windows.CloseHandle(root);return nil,ErrUnavailable}
 store:=&nativeStore{root:root,keyHandle:key,rootIdentity:rootIdentity,owner:p.Owner,outstanding:map[windows.Handle]struct{}{root:{},key:{}}}
 keyIdentity,keyIdentityError:=nativeDiskIdentity(key);if keyIdentityError!=nil{store.uncertain=true;retainStore(store);return nil,keyIdentityError};if readWindowsExact(key,store.key[:])!=nil{store.uncertain=true;retainStore(store);return nil,ErrUnavailable};after,e:=winInfo(key,false,p.Owner);afterIdentity,idError:=nativeDiskIdentity(key);if e!=nil||idError!=nil||afterIdentity!=keyIdentity||!sameWindowsFile(before,after){store.uncertain=true;retainStore(store);return nil,ErrUnavailable};return store,nil
}
func winSize(info windows.ByHandleFileInformation)uint64{return uint64(info.FileSizeHigh)<<32|uint64(info.FileSizeLow)}
func sameWindowsFile(a,b windows.ByHandleFileInformation)bool{return a.VolumeSerialNumber==b.VolumeSerialNumber&&a.FileIndexHigh==b.FileIndexHigh&&a.FileIndexLow==b.FileIndexLow&&a.FileSizeHigh==b.FileSizeHigh&&a.FileSizeLow==b.FileSizeLow&&a.FileAttributes==b.FileAttributes&&a.LastWriteTime==b.LastWriteTime&&a.CreationTime==b.CreationTime}
func readWindowsExact(handle windows.Handle,out []byte)error{for offset:=0;offset<len(out);{var n uint32;e:=windows.ReadFile(handle,out[offset:],&n,nil);if e!=nil||n==0{return io.ErrUnexpectedEOF};offset+=int(n)};var byte [1]byte;var n uint32;e:=windows.ReadFile(handle,byte[:],&n,nil);if e!=nil&&e!=windows.ERROR_HANDLE_EOF{return e};if n!=0{return ErrInvalid};return nil}
func storeName(name string)bool{return name=="generation-a"||name=="generation-b"||name=="selector"||strings.HasPrefix(name,"observation-")||strings.HasPrefix(name,"closure-")||strings.HasPrefix(name,"receipt-")}
func(s *nativeStore)closeOwned(handle windows.Handle)error{if _,exists:=s.outstanding[handle];!exists{return ErrInvalid};if e:=windows.CloseHandle(handle);e!=nil{s.uncertain=true;return e};delete(s.outstanding,handle);return nil}
func(s *nativeStore)Read(name string,cap uint32)(raw []byte,result error){
 token,e:=s.beginOriginalOperation(1);if e!=nil{return nil,e}
 defer s.finishOriginalOperation(token,&result)
 return s.readOriginalOwned(token,name,cap)
}
func(s *nativeStore)readOriginalOwned(token originalStoreToken,name string,cap uint32)(raw []byte,result error){
 if e:=s.checkOriginalOperation(token);e!=nil{return nil,e}
 if s==nil||s.uncertain||!storeName(name)||strings.ContainsAny(name,"/\\:\x00")||cap==0||cap>LedgerPayloadCap+36{return nil,ErrUnavailable}
 if s.readOrdinal==^uint64(0){return nil,ErrUnavailable};s.readOrdinal++
 s.readOriginal=nativeStoreRead{ordinal:s.readOrdinal,started:true,name:name,maximum:cap,rootOriginal:s.root,
  rootIdentityOriginal:s.rootIdentity,ownerOriginal:s.owner}
 owner:=&s.readOriginal
 defer func(){owner.resultError=result;if result!=nil{s.uncertain=true}}()
 handle,e:=ntStoreOpen(name,owner.rootOriginal,0x0001|0x20000|0x100000,1,0x20,nil)
 owner.handle=handle;owner.openError=e;owner.openReturned=true
 if e!=nil{return nil,e};s.outstanding[handle]=struct{}{}
 before,e:=winInfo(handle,false,owner.ownerOriginal);owner.before=before;owner.beforeError=e;owner.beforeReturned=true
 if e!=nil||winSize(before)==0||winSize(before)>uint64(cap){return nil,ErrUnavailable}
 originalID,idError:=nativeDiskIdentity(handle);owner.identity=originalID;owner.identityError=idError;owner.identityReturned=true
 if idError!=nil{return nil,idError}
 raw=make([]byte,int(winSize(before)));owner.raw=raw
 e=readWindowsExact(handle,raw);owner.readError=e;owner.readReturned=true
 if e!=nil{return raw,ErrUnavailable}
 after,e:=winInfo(handle,false,owner.ownerOriginal);owner.after=after;owner.afterError=e;owner.afterReturned=true
 afterID,idError:=nativeDiskIdentity(handle);owner.afterIdentity=afterID;owner.afterIdentityError=idError;owner.afterIdentityReturned=true
 if e!=nil||idError!=nil||afterID!=originalID||!sameWindowsFile(before,after){return raw,ErrUnavailable}
 if s.root!=owner.rootOriginal||s.rootIdentity!=owner.rootIdentityOriginal||s.owner!=owner.ownerOriginal{return raw,ErrClosure}
 e=s.closeOwned(handle);owner.closeError=e;owner.closeReturned=true
 if e!=nil{return raw,e};owner.complete=true;return raw,nil
}
func(s *nativeStore)Replace(name string,raw []byte)(resultError error){
 token,claimError:=s.beginOriginalOperation(2);if claimError!=nil{return claimError}
 defer s.finishOriginalOperation(token,&resultError)
 return s.replaceOriginalOwned(token,name,raw)
}
func(s *nativeStore)replaceOriginalOwned(token originalStoreToken,name string,raw []byte)error{
 if e:=s.checkOriginalOperation(token);e!=nil{return e}
 if s.uncertain||!storeName(name)||strings.ContainsAny(name,"/\\:\x00")||len(raw)==0||len(raw)>LedgerPayloadCap+36{return ErrUnavailable}
 descriptor,e:=windows.SecurityDescriptorFromString(ownerDACL);if e!=nil{return ErrUnavailable};var nonce [16]byte;if _,e=rand.Read(nonce[:]);e!=nil{return e};temporary:=".transaction-"+hex.EncodeToString(nonce[:]);handle,e:=ntStoreOpen(temporary,s.root,0x0001|0x0002|0x10000|0x20000|0x100000,2,0x20,descriptor);if e!=nil{return e};s.outstanding[handle]=struct{}{}
 for offset:=0;offset<len(raw);{var n uint32;e=windows.WriteFile(handle,raw[offset:],&n,nil);if e!=nil||n==0{s.uncertain=true;return ErrUnavailable};offset+=int(n)}
 if windows.FlushFileBuffers(handle)!=nil{s.uncertain=true;return ErrUnavailable};before,e:=winInfo(handle,false,s.owner);if e!=nil||winSize(before)!=uint64(len(raw)){s.uncertain=true;return ErrUnavailable};originalID,idError:=nativeDiskIdentity(handle);if idError!=nil{s.uncertain=true;return idError};if _,e=windows.Seek(handle,0,io.SeekStart);e!=nil{s.uncertain=true;return ErrUnavailable};held:=make([]byte,len(raw));if readWindowsExact(handle,held)!=nil||!bytesEqual(raw,held){s.uncertain=true;return ErrUnavailable};after,e:=winInfo(handle,false,s.owner);afterID,idError:=nativeDiskIdentity(handle);if e!=nil||idError!=nil||afterID!=originalID||!sameWindowsFile(before,after){s.uncertain=true;return ErrUnavailable}
 // FileRenameInformation uses the retained directory as RootDirectory. The
 // atomic replacement is confined to this owner-provisioned control store.
 type rename struct { replace byte; root windows.Handle; length uint32; name [1]uint16 }
 units,e:=windows.UTF16FromString(name);if e!=nil{return e};var value [512]byte;prefix:=int(unsafe.Offsetof(rename{}.name));length:=prefix+2*(len(units)-1);if length>len(value){return ErrInvalid};row:=(*rename)(unsafe.Pointer(&value[0]));row.replace=1;row.root=s.root;row.length=uint32(2*(len(units)-1));for i,u:=range units[:len(units)-1]{binary.LittleEndian.PutUint16(value[prefix+2*i:],u)};var status nativeStatus
 result,_,_:=storeSet.Call(uintptr(handle),uintptr(unsafe.Pointer(&status)),uintptr(unsafe.Pointer(&value[0])),uintptr(length),10);if int32(result)<0||windows.FlushFileBuffers(s.root)!=nil{s.uncertain=true;return ErrUnavailable}
 if e=s.closeOwned(handle);e!=nil{return e};actual,e:=s.readOriginalOwned(token,name,uint32(len(raw)));if e!=nil||!bytesEqual(actual,raw){s.uncertain=true;return ErrUnavailable};return nil
}
func(s *nativeStore)TotalBytes()(size uint64,resultError error){
 token,claimError:=s.beginOriginalOperation(3);if claimError!=nil{return 0,claimError}
 defer s.finishOriginalOperation(token,&resultError)
 return s.totalBytesOriginalOwned(token)
}
func(s *nativeStore)totalBytesOriginalOwned(token originalStoreToken)(uint64,error){
 if e:=s.checkOriginalOperation(token);e!=nil{return 0,e}
 if s.uncertain{return 0,ErrUnavailable};var raw [32768]byte;var total uint64;entries:=0;class:=uint32(11)
 for{e:=windows.GetFileInformationByHandleEx(s.root,class,&raw[0],uint32(len(raw)));class=10;if e==windows.ERROR_NO_MORE_FILES{break};if e!=nil{s.uncertain=true;return 0,ErrUnavailable};offset:=0
  for{if offset+104>len(raw){s.uncertain=true;return 0,ErrInvalid};next:=binary.LittleEndian.Uint32(raw[offset:]);size:=binary.LittleEndian.Uint64(raw[offset+40:]);attributes:=binary.LittleEndian.Uint32(raw[offset+56:]);length:=binary.LittleEndian.Uint32(raw[offset+60:]);if length%2!=0||length==0||length>480||offset+104+int(length)>len(raw){s.uncertain=true;return 0,ErrInvalid};var units [240]uint16;for i:=0;i<int(length)/2;i++{units[i]=binary.LittleEndian.Uint16(raw[offset+104+2*i:])};name:=string(utf16.Decode(units[:length/2]));if name!="."&&name!=".."{entries++;if entries>256||attributes&(windows.FILE_ATTRIBUTE_DIRECTORY|windows.FILE_ATTRIBUTE_REPARSE_POINT)!=0||size==0||size>StoreBytes-total||!storeName(name)&&!strings.HasPrefix(name,".transaction-"){s.uncertain=true;return 0,ErrUnavailable};total+=size};if next==0{break};if next<104||int(next)>len(raw)-offset{s.uncertain=true;return 0,ErrInvalid};offset+=int(next)
  }
 };return total,nil
}


func(s *nativeStore)Close()(result error){token,e:=s.beginOriginalOperation(4);if e!=nil{return e};defer s.finishOriginalOperation(token,&result);return s.closeOriginalStoreOwned(token)}
func(s *nativeStore)ReadbackDigest(name string,cap uint32)([32]byte,error){var out [32]byte;raw,e:=s.Read(name,cap);if e!=nil{return out,e};return sha256.Sum256(raw),nil}
