//go:build linux

package hostadmission

import (
 "crypto/rand"
 "crypto/sha256"
 "encoding/binary"
 "encoding/hex"
 "io"
 "os"
 "strings"
 "sync"

 "golang.org/x/sys/unix"
)

type nativeGrant struct { profileDigest [32]byte; storePath,keyPath,retentionPath string; clientUsers []string }
type storeOwnerIdentity = uint32
// Installer-owned grants are separate immutable admitted inputs. No runtime
// flag, endpoint request or caller filesystem root fills this list.
var nativeGrants=[]nativeGrant{}
type nativeStore struct { root int; keyHandle int; key [32]byte; rootIdentity [32]byte; owner uint32; outstanding map[int]struct{}; uncertain bool; initializationAuthorized bool; readOriginal nativeStoreRead; readOrdinal uint64; initialization originalStoreInitialization; operationMu sync.Mutex; operationOrdinal uint64; operation originalStoreOperation; disposed bool }
type nativeStoreRead struct {
 ordinal uint64
 started,openReturned,beforeReturned,readReturned,afterReturned,closeReturned,complete bool
 name string
 maximum uint32
 handle int
 rootOriginal int
 rootIdentityOriginal [32]byte
 ownerOriginal uint32
 before,after unix.Stat_t
 raw []byte
 openError,beforeError,readError,afterError,closeError,resultError error
}
func nativeRootDigest(stat *unix.Stat_t)[32]byte{var raw [28]byte;binary.BigEndian.PutUint64(raw[0:8],uint64(stat.Dev));binary.BigEndian.PutUint64(raw[8:16],stat.Ino);binary.BigEndian.PutUint32(raw[16:20],stat.Uid);binary.BigEndian.PutUint32(raw[20:24],stat.Gid);binary.BigEndian.PutUint32(raw[24:28],stat.Mode);return DomainHash("SLCLI-LINUX-ROOT-1\x00",raw[:])}
func heldAbsoluteDirectory(path string,owner uint32,private bool)(int,error){
 if !strings.HasPrefix(path,"/")||path=="/"||strings.Contains(path,"//")||strings.ContainsAny(path,"\\\x00"){return -1,ErrUnavailable}
 root,e:=unix.Open("/",unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_CLOEXEC,0);if e!=nil{return -1,ErrUnavailable}
 for _,part:=range strings.Split(path,"/")[1:]{if part==""||part=="."||part==".."{unix.Close(root);return -1,ErrUnavailable};child,e:=unix.Openat(root,part,unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0);if e!=nil{unix.Close(root);return -1,ErrUnavailable};if e=unix.Close(root);e!=nil{unix.Close(child);return -1,ErrUnavailable};root=child}
 var stat unix.Stat_t;if unix.Fstat(root,&stat)!=nil||stat.Mode&unix.S_IFMT!=unix.S_IFDIR||private&&(stat.Uid!=owner||stat.Mode&07777!=0700){unix.Close(root);return -1,ErrUnavailable};return root,nil
}
func OpenStore(p *Profile)(*nativeStore,error){
 if !beginStoreAttempt(){return nil,ErrUnavailable}
 var admissionError error;p,admissionError=originalProfile(p);if admissionError!=nil{return nil,admissionError}
 if p==nil||p.Platform!="linux"||p.StoreProvider!="linux-openat-atomic-v1"||uint32(os.Geteuid())==0&&p.Owner!="0"{return nil,ErrUnavailable}
 owner,e:=strconvUint32(p.Owner);if e!=nil||owner!=uint32(os.Geteuid()){return nil,ErrUnavailable};var selected *nativeGrant
 for i:=range nativeGrants{if nativeGrants[i].profileDigest==p.Digest{if selected!=nil{return nil,ErrUnavailable};selected=&nativeGrants[i]}};if selected==nil{return nil,ErrUnavailable}
 root,e:=heldAbsoluteDirectory(selected.storePath,owner,true);if e!=nil{return nil,e};var stat unix.Stat_t;if unix.Fstat(root,&stat)!=nil{unix.Close(root);return nil,ErrUnavailable};identity:=nativeRootDigest(&stat);if digestString(identity)!=p.StoreRootIdentity{unix.Close(root);return nil,ErrUnavailable};if unix.Flock(root,unix.LOCK_EX|unix.LOCK_NB)!=nil{unix.Close(root);return nil,ErrUnavailable}
 keyRootPath,keyName,e:=splitPrivateLeaf(selected.keyPath);if e!=nil{unix.Close(root);return nil,e};keyRoot,e:=heldAbsoluteDirectory(keyRootPath,owner,true);if e!=nil{unix.Close(root);return nil,e};keyFD,e:=unix.Openat(keyRoot,keyName,unix.O_RDONLY|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0);closeErr:=unix.Close(keyRoot);if e!=nil||closeErr!=nil{if keyFD>=0{unix.Close(keyFD)};unix.Close(root);return nil,ErrUnavailable}
 var keyStat unix.Stat_t;if unix.Fstat(keyFD,&keyStat)!=nil||keyStat.Mode&unix.S_IFMT!=unix.S_IFREG||keyStat.Mode&07777!=0600||keyStat.Uid!=owner||keyStat.Size!=32{unix.Close(keyFD);unix.Close(root);return nil,ErrUnavailable}
 out:=&nativeStore{root:root,keyHandle:keyFD,rootIdentity:identity,owner:owner,outstanding:map[int]struct{}{root:{},keyFD:{}}};if e=readSameFD(keyFD,out.key[:]);e!=nil{out.uncertain=true;retainStore(out);return nil,ErrUnavailable};var after unix.Stat_t;if unix.Fstat(keyFD,&after)!=nil||!sameNativeFile(&keyStat,&after){out.uncertain=true;retainStore(out);return nil,ErrUnavailable};return out,nil
}
func strconvUint32(s string)(uint32,error){var out uint64;if s==""{return 0,ErrInvalid};for _,c:=range s{if c<'0'||c>'9'{return 0,ErrInvalid};out=out*10+uint64(c-'0');if out>0xffffffff{return 0,ErrInvalid}};return uint32(out),nil}
func splitPrivateLeaf(path string)(string,string,error){at:=strings.LastIndex(path,"/");if at<=0||at==len(path)-1{return "","",ErrUnavailable};name:=path[at+1:];if name=="."||name==".."||strings.ContainsAny(name,"\\\x00"){return "","",ErrUnavailable};return path[:at],name,nil}
func sameNativeFile(a,b *unix.Stat_t)bool{return a.Dev==b.Dev&&a.Ino==b.Ino&&a.Mode==b.Mode&&a.Uid==b.Uid&&a.Gid==b.Gid&&a.Size==b.Size&&a.Mtim==b.Mtim&&a.Ctim==b.Ctim}
func readSameFD(fd int,out []byte)error{for offset:=0;offset<len(out);{n,e:=unix.Read(fd,out[offset:]);if e==unix.EINTR{continue};if e!=nil{return e};if n==0{return io.ErrUnexpectedEOF};offset+=n};var extra [1]byte;for{n,e:=unix.Read(fd,extra[:]);if e==unix.EINTR{continue};if e!=nil{return e};if n!=0{return ErrInvalid};return nil}}
func storeName(name string)bool{return name=="generation-a"||name=="generation-b"||name=="selector"||strings.HasPrefix(name,"observation-")||strings.HasPrefix(name,"closure-")||strings.HasPrefix(name,"receipt-")}
func(s *nativeStore) Read(name string,cap uint32)(raw []byte,result error){
 token,e:=s.beginOriginalOperation(1);if e!=nil{return nil,e}
 defer s.finishOriginalOperation(token,&result)
 return s.readOriginalOwned(token,name,cap)
}
func(s *nativeStore)readOriginalOwned(token originalStoreToken,name string,cap uint32)(raw []byte,result error){
 if e:=s.checkOriginalOperation(token);e!=nil{return nil,e}
 if s==nil||s.uncertain||!storeName(name)||strings.ContainsAny(name,"/\\\x00")||cap==0||cap>LedgerPayloadCap+36{return nil,ErrUnavailable}
 if s.readOrdinal==^uint64(0){return nil,ErrUnavailable};s.readOrdinal++
 s.readOriginal=nativeStoreRead{ordinal:s.readOrdinal,started:true,name:name,maximum:cap,handle:-1,
  rootOriginal:s.root,rootIdentityOriginal:s.rootIdentity,ownerOriginal:s.owner}
 owner:=&s.readOriginal
 defer func(){owner.resultError=result;if result!=nil{s.uncertain=true}}()
 fd,e:=unix.Openat(owner.rootOriginal,name,unix.O_RDONLY|unix.O_NOFOLLOW|unix.O_NONBLOCK|unix.O_CLOEXEC,0)
 owner.handle=fd;owner.openError=e;owner.openReturned=true
 if e!=nil{return nil,ErrUnavailable};s.outstanding[fd]=struct{}{}
 e=unix.Fstat(fd,&owner.before);owner.beforeError=e;owner.beforeReturned=true
 if e!=nil||owner.before.Mode&unix.S_IFMT!=unix.S_IFREG||owner.before.Mode&07777!=0600||owner.before.Uid!=owner.ownerOriginal||owner.before.Size<1||owner.before.Size>int64(cap){return nil,ErrUnavailable}
 raw=make([]byte,int(owner.before.Size));owner.raw=raw
 e=readSameFD(fd,raw);owner.readError=e;owner.readReturned=true
 if e!=nil{return raw,ErrUnavailable}
 e=unix.Fstat(fd,&owner.after);owner.afterError=e;owner.afterReturned=true
 if e!=nil||!sameNativeFile(&owner.before,&owner.after){return raw,ErrUnavailable}
 if s.root!=owner.rootOriginal||s.rootIdentity!=owner.rootIdentityOriginal||s.owner!=owner.ownerOriginal{return raw,ErrClosure}
 e=s.closeOwned(fd);owner.closeError=e;owner.closeReturned=true
 if e!=nil{return raw,ErrUnavailable};owner.complete=true;return raw,nil
}
func(s *nativeStore) closeOwned(fd int)error{if _,ok:=s.outstanding[fd];!ok{return ErrInvalid};if e:=unix.Close(fd);e!=nil{s.uncertain=true;return e};delete(s.outstanding,fd);return nil}
// A transaction creates one exclusive private original temporary object,
// flushes and reads its exact held bytes, then atomically replaces the admitted
// control-store leaf and flushes the original containing directory. It never
// retries or removes a pathname on failure; unresolved objects remain retained.
func(s *nativeStore) Replace(name string,raw []byte)(result error){
 token,claimError:=s.beginOriginalOperation(2);if claimError!=nil{return claimError}
 defer s.finishOriginalOperation(token,&result)
 return s.replaceOriginalOwned(token,name,raw)
}
func(s *nativeStore)replaceOriginalOwned(token originalStoreToken,name string,raw []byte)error{
 if e:=s.checkOriginalOperation(token);e!=nil{return e}
 if s.uncertain||!storeName(name)||strings.ContainsAny(name,"/\\\x00")||len(raw)==0||len(raw)>LedgerPayloadCap+36{return ErrUnavailable}
 var nonce [16]byte;if _,e:=rand.Read(nonce[:]);e!=nil{return ErrUnavailable};temporary:=".transaction-"+hex.EncodeToString(nonce[:]);fd,e:=unix.Openat(s.root,temporary,unix.O_RDWR|unix.O_CREAT|unix.O_EXCL|unix.O_NOFOLLOW|unix.O_CLOEXEC,0600);if e!=nil{return ErrUnavailable};s.outstanding[fd]=struct{}{}
 for offset:=0;offset<len(raw);{n,e:=unix.Write(fd,raw[offset:]);if e==unix.EINTR{continue};if e!=nil||n==0{s.uncertain=true;return ErrUnavailable};offset+=n};if unix.Fsync(fd)!=nil{s.uncertain=true;return ErrUnavailable}
 var before unix.Stat_t;if unix.Fstat(fd,&before)!=nil||before.Mode&unix.S_IFMT!=unix.S_IFREG||before.Mode&07777!=0600||before.Uid!=s.owner||before.Size!=int64(len(raw)){s.uncertain=true;return ErrUnavailable};if _,e=unix.Seek(fd,0,io.SeekStart);e!=nil{s.uncertain=true;return ErrUnavailable};held:=make([]byte,len(raw));if readSameFD(fd,held)!=nil||!bytesEqual(raw,held){s.uncertain=true;return ErrUnavailable};var after unix.Stat_t;if unix.Fstat(fd,&after)!=nil||!sameNativeFile(&before,&after){s.uncertain=true;return ErrUnavailable}
 if unix.Fsync(s.root)!=nil||unix.Renameat(s.root,temporary,s.root,name)!=nil||unix.Fsync(s.root)!=nil{s.uncertain=true;return ErrUnavailable};if e=s.closeOwned(fd);e!=nil{return ErrUnavailable};readback,e:=s.readOriginalOwned(token,name,uint32(len(raw)));if e!=nil||!bytesEqual(readback,raw){s.uncertain=true;return ErrUnavailable};return nil
}


func(s *nativeStore) Close()(result error){token,e:=s.beginOriginalOperation(4);if e!=nil{return e};defer s.finishOriginalOperation(token,&result);return s.closeOriginalStoreOwned(token)}
func(s *nativeStore) ReadbackDigest(name string,cap uint32)([32]byte,error){var out [32]byte;raw,e:=s.Read(name,cap);if e!=nil{return out,e};return sha256.Sum256(raw),nil}

// The complete physical store inventory is obtained from the original held
// directory. Unknown leaves, symlinks, special objects and unfinished scratch
// are counted or denied; they can never disappear from capacity accounting.
func(s *nativeStore) TotalBytes()(size uint64,result error){
 token,claimError:=s.beginOriginalOperation(3);if claimError!=nil{return 0,claimError}
 defer s.finishOriginalOperation(token,&result)
 return s.totalBytesOriginalOwned(token)
}
func(s *nativeStore)totalBytesOriginalOwned(token originalStoreToken)(uint64,error){
 if e:=s.checkOriginalOperation(token);e!=nil{return 0,e}
 if s.uncertain{return 0,ErrUnavailable}
 fd,e:=unix.Openat(s.root,".",unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_CLOEXEC,0);if e!=nil{return 0,ErrUnavailable};s.outstanding[fd]=struct{}{}
 var total uint64;var buffer [32768]byte;entries:=0
 for{n,e:=unix.ReadDirent(fd,buffer[:]);if e==unix.EINTR{continue};if e!=nil{s.uncertain=true;return 0,ErrUnavailable};if n==0{break};_,_,names:=unix.ParseDirent(buffer[:n],-1,nil)
  for _,name:=range names{entries++;if entries>256{s.uncertain=true;return 0,ErrUnavailable};if !storeName(name)&&!strings.HasPrefix(name,".transaction-"){s.uncertain=true;return 0,ErrUnavailable}
   var info unix.Stat_t;if unix.Fstatat(s.root,name,&info,unix.AT_SYMLINK_NOFOLLOW)!=nil||info.Mode&unix.S_IFMT!=unix.S_IFREG||info.Mode&07777!=0600||info.Uid!=s.owner||info.Size<1||uint64(info.Size)>StoreBytes-total{s.uncertain=true;return 0,ErrUnavailable};total+=uint64(info.Size)
  }
 };if e=s.closeOwned(fd);e!=nil{return 0,e};return total,nil
}
