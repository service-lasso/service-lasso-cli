//go:build linux

package hostadmission

import (
 "bytes"
 "crypto/sha256"
 "encoding/binary"
 "encoding/hex"
 "strconv"
 "golang.org/x/sys/unix"
)

type peerCustody struct { role byte; process *nativeProcess; credentials [12]byte; packageSource [32]byte; birthRaw,nativeBirth,peerRaw []byte }
// The channel is an already-held original accepted AF_UNIX object. Its actual
// SO_PEERCRED selects the task. No request PID, pathname, token, or claimed hash
// can replace the admitted original Node parent.
func authenticateNodePeer(channel int,p *Profile)(*peerCustody,error){
 return authenticateOriginalPeer(channel,p,originalNodePeer)
}
func authenticateObserverPeer(channel int,p *Profile)(*peerCustody,error){return authenticateOriginalPeer(channel,p,originalObserverPeer)}
// Both ordinary routes use public facade82/image79. This private native peer
// observation supplies no role15/source/ROOT enrollment or copy-parent alias.
func authenticateFacadePeer(channel int,p *Profile)(*peerCustody,error){return authenticateOriginalPeer(channel,p,originalFacadePeer)}
func authenticateOriginalPeer(channel int,p *Profile,role byte)(*peerCustody,error){
 var profileError error;p,profileError=originalProfile(p);if profileError!=nil{return nil,profileError}
 expectedImage,profileError:=originalPeerImage(p,role);if profileError!=nil{return nil,profileError};identityDomain,profileError:=originalPeerIdentityDomain(role);if profileError!=nil{return nil,profileError}
 if p==nil||p.Platform!="linux"{return nil,ErrUnavailable};cred,e:=unix.GetsockoptUcred(channel,unix.SOL_SOCKET,unix.SO_PEERCRED);if e!=nil||cred.Pid<=0||!originalPeerOwnerAllowed(p,role,strconv.FormatUint(uint64(cred.Uid),10)){return nil,ErrUnavailable}
 slot,e:=reserveProcessOriginal();if e!=nil{return nil,e};process:=&nativeProcess{pidfd:-1,procDirectory:-1,imageFD:-1,birthFD:-1,pid:uint32(cred.Pid),uid:cred.Uid};if e=retainProcessOriginal(slot,process);e!=nil{return nil,e}
 pidfd,e:=unix.PidfdOpen(int(cred.Pid),0);if e!=nil{return nil,ErrUnavailable};process.pidfd=pidfd
 directory,e:=unix.Open("/proc/"+strconv.FormatInt(int64(cred.Pid),10),unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_CLOEXEC,0);if e!=nil{return nil,ErrUnavailable};process.procDirectory=directory
 statFD,e:=unix.Openat(directory,"stat",unix.O_RDONLY|unix.O_NOFOLLOW|unix.O_CLOEXEC,0);if e!=nil{return nil,ErrUnavailable};process.birthFD=statFD
 birth,raw,e:=readHeldOriginalBirth(statFD,process.pid);if e!=nil{return nil,e};process.birth=birth
 executable,e:=unix.Openat(directory,"exe",unix.O_RDONLY|unix.O_CLOEXEC,0);if e!=nil{return nil,ErrUnavailable};process.imageFD=executable
 var stat unix.Stat_t;if unix.Fstat(executable,&stat)!=nil||stat.Mode&unix.S_IFMT!=unix.S_IFREG||stat.Size<=0{return nil,ErrUnavailable};process.imageBefore=stat
 image,e:=hashOriginalImage(executable,&stat);if e!=nil||digestString(image)!=expectedImage{return nil,ErrUnavailable};process.image=image
 decoded,e:=hex.DecodeString(p.Service.ImageSHA256);if e!=nil||len(decoded)!=32{return nil,ErrInvalid};var source [32]byte;copy(source[:],decoded);process.source=source
 var packageSource [32]byte;if role==originalNodePeer{packageBytes,e:=hex.DecodeString(p.Launch.Library.PackageSourceSHA256);if e!=nil||len(packageBytes)!=32{return nil,ErrInvalid};copy(packageSource[:],packageBytes)}
 var native [32]byte;binary.BigEndian.PutUint32(native[0:4],process.pid);binary.BigEndian.PutUint32(native[4:8],process.uid);binary.BigEndian.PutUint64(native[8:16],birth);binary.BigEndian.PutUint64(native[16:24],uint64(stat.Dev));binary.BigEndian.PutUint64(native[24:32],stat.Ino);process.identity=DomainHash(identityDomain,native[:],image[:])
 tick,e:=nativeTick();if e!=nil{return nil,e};receipt,e:=primitiveBytes("birth","linux.pidfd_open","success",process.identity,process.identity,source,tick,nil,nil);if e!=nil{return nil,e}
 second,e:=unix.GetsockoptUcred(channel,unix.SOL_SOCKET,unix.SO_PEERCRED);if e!=nil||*second!=*cred{return nil,ErrUnavailable};after,_,e:=readHeldOriginalBirth(statFD,process.pid);if e!=nil||after!=birth{return nil,ErrUnavailable};if e=process.recheckOriginal();e!=nil{return nil,e}
 peerRaw,e:=primitiveBytes("identity","linux.so_peercred","success",process.identity,process.identity,source,tick,nil,nil);if e!=nil{return nil,e};out:=&peerCustody{role:role,process:process,packageSource:packageSource,birthRaw:receipt,nativeBirth:raw,peerRaw:peerRaw};binary.BigEndian.PutUint32(out.credentials[0:4],uint32(cred.Pid));binary.BigEndian.PutUint32(out.credentials[4:8],cred.Uid);binary.BigEndian.PutUint32(out.credentials[8:12],cred.Gid);return out,nil
}
func readHeldOriginalBirth(fd int,pid uint32)(uint64,[]byte,error){
 if fd<0||pid==0{return 0,nil,ErrInvalid};var raw [4096]byte;n,e:=unix.Pread(fd,raw[:],0);if e!=nil||n<=0||n>=len(raw){return 0,nil,ErrUnavailable}
 var eof [1]byte;extra,e:=unix.Pread(fd,eof[:],int64(n));if e!=nil||extra!=0{return 0,nil,ErrUnavailable}
 at:=bytes.LastIndexByte(raw[:n],')');first:=bytes.IndexByte(raw[:n],' ');if at<0||first<0||first>at{return 0,nil,ErrInvalid};actual,e:=procDecimal(raw[:first],uint64(^uint32(0)));if e!=nil||uint32(actual)!=pid{return 0,nil,ErrInvalid}
 offset:=at+1;var birth uint64
 // Field 3 (state) is index zero after comm; field 22 is index nineteen.
 // Borrow original byte tokens instead of allocating strings/Fields tables.
 for field:=0;field<=19;field++{
  for offset<n&&(raw[offset]==' '||raw[offset]=='\t'||raw[offset]=='\n'){offset++};start:=offset
  for offset<n&&raw[offset]!=' '&&raw[offset]!='\t'&&raw[offset]!='\n'{offset++};if offset==start{return 0,nil,ErrInvalid}
  if field==19{birth,e=procDecimal(raw[start:offset],^uint64(0));if e!=nil||birth==0{return 0,nil,ErrInvalid}}
 }
 out:=make([]byte,n);copy(out,raw[:n]);return birth,out,nil
}
func procDecimal(raw []byte,maximum uint64)(uint64,error){
 if len(raw)==0||len(raw)>20||len(raw)>1&&raw[0]=='0'{return 0,ErrInvalid};var out uint64
 for _,b:=range raw{if b<'0'||b>'9'{return 0,ErrInvalid};digit:=uint64(b-'0');if digit>maximum||out>(maximum-digit)/10{return 0,ErrInvalid};out=out*10+digit};return out,nil
}
func hashOriginalImage(fd int,before *unix.Stat_t)([32]byte,error){
 var result [32]byte;if before==nil||before.Size<=0{return result,ErrInvalid};h:=sha256.New();var buffer [FrameBytes]byte;var offset int64
 for offset<before.Size{want:=before.Size-offset;if want>int64(len(buffer)){want=int64(len(buffer))};n,e:=unix.Pread(fd,buffer[:int(want)],offset);if e==unix.EINTR{continue};if e!=nil||n<=0{return result,ErrUnavailable};h.Write(buffer[:n]);offset+=int64(n)};var extra [1]byte;n,e:=unix.Pread(fd,extra[:],offset);if e!=nil||n!=0{return result,ErrUnavailable};var after unix.Stat_t;if unix.Fstat(fd,&after)!=nil||!sameNativeFile(before,&after){return result,ErrUnavailable};var rawDigest [32]byte;sum:=h.Sum(rawDigest[:0]);if len(sum)!=32{return result,ErrInvalid};copy(result[:],sum);return result,nil
}
