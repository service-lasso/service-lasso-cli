//go:build windows

package hostadmission

import (
 "crypto/sha256"
 "encoding/binary"
 "encoding/hex"
 "unsafe"
 "golang.org/x/sys/windows"
)

type peerCustody struct { role byte; process *nativeProcess; credentials [12]byte; packageSource [32]byte; birthRaw,nativeBirth,peerRaw []byte }
var pipeClientPID=objectKernel.NewProc("GetNamedPipeClientProcessId")
func authenticateNodePeer(channel windows.Handle,p *Profile)(*peerCustody,error){
 return authenticateOriginalPeer(channel,p,originalNodePeer)
}
func authenticateObserverPeer(channel windows.Handle,p *Profile)(*peerCustody,error){return authenticateOriginalPeer(channel,p,originalObserverPeer)}
func authenticateFacadePeer(channel windows.Handle,p *Profile)(*peerCustody,error){return authenticateOriginalPeer(channel,p,originalFacadePeer)}
func authenticateOriginalPeer(channel windows.Handle,p *Profile,role byte)(*peerCustody,error){
 var profileError error;p,profileError=originalProfile(p);if profileError!=nil{return nil,profileError}
 expectedImage,profileError:=originalPeerImage(p,role);if profileError!=nil{return nil,profileError};identityDomain,profileError:=originalPeerIdentityDomain(role);if profileError!=nil{return nil,profileError}
 if p==nil||p.Platform!="win32"{return nil,ErrUnavailable};var pid uint32;ok,_,_:=pipeClientPID.Call(uintptr(channel),uintptr(unsafe.Pointer(&pid)));if ok==0||pid==0{return nil,ErrUnavailable}
 slot,e:=reserveProcessOriginal();if e!=nil{return nil,e};process:=&nativeProcess{pid:pid};if e=retainProcessOriginal(slot,process);e!=nil{return nil,e};handle,e:=windows.OpenProcess(windows.PROCESS_QUERY_LIMITED_INFORMATION|windows.PROCESS_DUP_HANDLE|windows.SYNCHRONIZE,false,pid);if e!=nil{return nil,ErrUnavailable};process.handle=handle
 var creation,exit,kernel,user windows.Filetime;if windows.GetProcessTimes(handle,&creation,&exit,&kernel,&user)!=nil||creation==(windows.Filetime{})||exit!=(windows.Filetime{}){return nil,ErrUnavailable};process.birth=creation
 if windows.OpenProcessToken(handle,windows.TOKEN_QUERY,&process.ownerToken)!=nil{return nil,ErrUnavailable};tokenState,e:=originalOwnerTokenState(process.ownerToken);if e!=nil{return nil,e};process.ownerTokenState=tokenState;actual,e:=process.ownerToken.GetTokenUser();if e!=nil{return nil,ErrUnavailable};afterToken,e:=originalOwnerTokenState(process.ownerToken);if e!=nil||afterToken!=tokenState{return nil,ErrInvalid};process.owner=actual.User.Sid.String();if !originalPeerOwnerAllowed(p,role,process.owner){return nil,ErrUnavailable}
 var imagePath [32768]uint16;size:=uint32(len(imagePath));if windows.QueryFullProcessImageName(handle,0,&imagePath[0],&size)!=nil||size==0||size>=uint32(len(imagePath)){return nil,ErrUnavailable}
 imageHandle,e:=ntStoreOpen(windows.UTF16ToString(imagePath[:size]),0,0x0001|0x0080|0x20000|0x100000,1,0x20,nil);if e!=nil{return nil,e};process.imageHandle=imageHandle;info,e:=winInfo(imageHandle,false,"");if e!=nil||winSize(info)==0{return nil,ErrUnavailable};process.imageBefore=info
 fileID,e:=nativeDiskIdentity(imageHandle);if e!=nil{return nil,e};process.imageIdentity=fileID
 image,e:=hashOriginalImage(imageHandle,info);if e!=nil||digestString(image)!=expectedImage{return nil,ErrUnavailable};process.image=image
 sourceBytes,e:=hex.DecodeString(p.Service.ImageSHA256);if e!=nil||len(sourceBytes)!=32{return nil,ErrInvalid};copy(process.source[:],sourceBytes);var packageSource [32]byte;if role==originalNodePeer{packageBytes,e:=hex.DecodeString(p.Launch.Library.PackageSourceSHA256);if e!=nil||len(packageBytes)!=32{return nil,ErrInvalid};copy(packageSource[:],packageBytes)}
 var native [40]byte;binary.BigEndian.PutUint32(native[:4],pid);binary.BigEndian.PutUint32(native[4:8],creation.HighDateTime);binary.BigEndian.PutUint32(native[8:12],creation.LowDateTime);copy(native[12:36],fileID[:]);binary.BigEndian.PutUint32(native[36:40],uint32(size));process.identity=DomainHash(identityDomain,native[:],image[:],[]byte(process.owner))
 tick,e:=nativeTick();if e!=nil{return nil,e};birthRaw,e:=primitiveBytes("birth","win32.process_birth","success",process.identity,process.identity,process.source,tick,nil,nil);if e!=nil{return nil,e};peerRaw,e:=primitiveBytes("identity","win32.pipe_peer","success",process.identity,process.identity,process.source,tick,nil,nil);if e!=nil{return nil,e}
 var second uint32;ok,_,_=pipeClientPID.Call(uintptr(channel),uintptr(unsafe.Pointer(&second)));if ok==0||second!=pid{return nil,ErrUnavailable};if e=process.recheckOriginal();e!=nil{return nil,e};raw:=make([]byte,len(native));copy(raw,native[:]);out:=&peerCustody{role:role,process:process,packageSource:packageSource,birthRaw:birthRaw,nativeBirth:raw,peerRaw:peerRaw};binary.BigEndian.PutUint32(out.credentials[:4],pid);return out,nil
}
func hashOriginalImage(handle windows.Handle,before windows.ByHandleFileInformation)([32]byte,error){
 var result [32]byte;size:=winSize(before);if size==0||size>uint64(^uint64(0)>>1){return result,ErrInvalid};originalID,e:=nativeDiskIdentity(handle);if e!=nil{return result,e};if _,e:=windows.Seek(handle,0,0);e!=nil{return result,ErrUnavailable};h:=sha256.New();var buffer [FrameBytes]byte;var offset uint64
 for offset<size{want:=int(size-offset);if want>len(buffer){want=len(buffer)};var n uint32;e:=windows.ReadFile(handle,buffer[:want],&n,nil);if e!=nil||n==0{return result,ErrUnavailable};h.Write(buffer[:n]);offset+=uint64(n)}
 var extra [1]byte;var n uint32;e=windows.ReadFile(handle,extra[:],&n,nil);if e!=nil&&e!=windows.ERROR_HANDLE_EOF||n!=0{return result,ErrUnavailable};after,e:=winInfo(handle,false,"");if e!=nil||!sameWindowsFile(before,after){return result,ErrUnavailable};afterID,e:=nativeDiskIdentity(handle);if e!=nil||afterID!=originalID{return result,ErrUnavailable};var rawDigest [32]byte;sum:=h.Sum(rawDigest[:0]);if len(sum)!=32{return result,ErrInvalid};copy(result[:],sum);return result,nil
}
