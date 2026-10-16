//go:build linux || windows

package hostadmission

import (
 "crypto/hmac"
 "crypto/sha256"
 "encoding/binary"
 "hash"
 "sync"
 "sync/atomic"
)

// This retained transport state is not an admission constructor. Production
// bootstrap must independently authenticate its original profile, primary,
// channel, capability, reservation and private capture objects before use.
// All kernel reads use the original stable native control mapping, including
// on Windows where an unresolved OVERLAPPED may outlive this call.
type primaryFrameInput struct{
 mu sync.Mutex
 active bool
 profile *Profile
 channel *nativeObject
 primary *nativeProcess
 binding *originalBinding
 memory *nativeMemory
 deadline *nativeDeadline
 rawCapture,nativeCapture *nativeEvidenceFile
 rawHash,nativeHash hash.Hash
 rawBytes,nativeBytes uint64
 nonce,capability [32]byte
 sequence uint32
 failed atomic.Bool
 readResult originalFrameIOResult
 frameLease originalFrameLease
}
func(input *primaryFrameInput)read(frame *Frame)(err error){
 if input==nil||frame==nil{return ErrInvalid}
 input.mu.Lock();if input.active||input.frameLease.borrowed&&!input.frameLease.returned{input.mu.Unlock();return ErrPhase};input.active=true;input.mu.Unlock()
 defer func(){input.mu.Lock();input.active=false;input.mu.Unlock()}()
 frame.Length=0
 defer func(){if err!=nil{input.failed.Store(true);if input.memory!=nil&&input.memory.accounting!=nil{input.memory.accounting.holdFailure()}}}()
 profile,e:=originalProfile(input.profile);if e!=nil{return e}
 // Selected v2 class2 is the authenticated channel; class3 remains the
 // independently owned stdout/stderr stream collector. The discriminant does
 // not construct either original or establish its native/source authority.
 if input.failed.Load()||input.channel==nil||input.channel.kind!=2||input.primary==nil||input.binding==nil||input.binding.originals==nil||input.binding.primary!=input.primary.identity||input.memory==nil||input.memory.accounting==nil||input.memory.accounting.domain!=1||input.memory.accounting.limit!=ControlBytes||input.memory.accounting.contract.profile!=profile.Digest||input.deadline==nil||input.rawCapture==nil||input.nativeCapture==nil||input.nonce==[32]byte{}||input.capability==[32]byte{}||input.sequence>1025||len(input.memory.bytes)<FrameBytes+36+2048{return ErrUnavailable}
 serviceImage,e:=ParseDigest(profile.Service.ImageSHA256);if e!=nil||input.channel.source!=serviceImage||digestString(input.primary.image)!=profile.Launch.Primary.ImageSHA256{return ErrUnavailable}
 if input.rawCapture.root==nil||input.nativeCapture.root==nil||input.rawCapture.root.profile!=profile.Digest||input.nativeCapture.root.profile!=profile.Digest||input.rawCapture.run!=input.binding.run||input.nativeCapture.run!=input.binding.run||input.rawCapture.source!=serviceImage||input.nativeCapture.source!=serviceImage{return ErrUnavailable}
 if input.rawCapture==input.nativeCapture||input.rawCapture.identity==input.nativeCapture.identity||input.rawCapture.handle==input.nativeCapture.handle{return ErrInvalid}
 if e=input.binding.originals.verifyPrebound(input.binding);e!=nil{return e}
 if e=input.channel.recheckIdentity(input.channel.identity,serviceImage);e!=nil{return e}
 if e=input.primary.recheckOriginal();e!=nil{return e}
 if e=correlateOriginalFramePeer(input.channel.handle,input.primary);e!=nil{return e}
 if e=input.rawCapture.recheckOriginal();e!=nil{return e};if e=input.nativeCapture.recheckOriginal();e!=nil{return e}
 if input.frameLease.borrowed&&!input.frameLease.returned{return ErrPhase}
 input.frameLease=originalFrameLease{}
 if e=input.memory.borrowFrameOriginal(&input.frameLease);e!=nil{return e}
 defer func(){
  input.frameLease.primaryError=err
  if err!=nil&&!input.frameLease.rawFlushReturned&&input.rawHash!=nil&&input.nativeHash!=nil{
   // Preserve the first failure while independently finalizing eligible
   // already-held capture files; failure never authorizes mapping reuse.
   _ = input.flushOriginalCaptures()
  }
  returnError:=input.frameLease.finish(err)
  input.readResult.retainedBorrow=input.frameLease.retained
  if returnError!=nil&&err==nil{err=returnError}
 }()
 if input.rawHash==nil{input.rawHash=sha256.New()};if input.nativeHash==nil{input.nativeHash=sha256.New()}
 if e=input.readExact(0,4);e!=nil{return e}
 length:=binary.BigEndian.Uint32(input.memory.bytes[:4]);input.frameLease.declaredLength=length;if length<5||length>FrameBytes{return ErrInvalid}
 // Operation and sequence are checked before reading any variable body.
 if e=input.readExact(4,5);e!=nil{return e}
 operation:=input.memory.bytes[4];sequence:=binary.BigEndian.Uint32(input.memory.bytes[5:9])
 maximum:=uint32(256);switch operation{case 1:if sequence!=0||length!=5{return ErrInvalid};case 3:maximum=FrameBytes;if sequence==0||sequence>1024{return ErrInvalid};case 5:if sequence==0||sequence>1025{return ErrInvalid};default:return ErrInvalid}
 if sequence!=input.sequence||length>maximum{return ErrInvalid}
 if length>5{if e=input.readExact(9,length-5);e!=nil{return e}}
 if e=input.readExact(4+length,32);e!=nil{return e}
 body:=input.memory.bytes[4:4+length];actual:=input.memory.bytes[4+length:4+length+32]
 expected:=MAC(HostDomain,0,input.nonce,input.capability,sequence,body)
 if !hmac.Equal(actual,expected[:]){return ErrInvalid}
 if e=input.flushOriginalCaptures();e!=nil{return e}
 if _,e=input.deadline.remaining();e!=nil{return e}
 if e=input.primary.recheckOriginal();e!=nil{return e}
 if e=input.channel.recheckIdentity(input.channel.identity,serviceImage);e!=nil{return e}
 if e=correlateOriginalFramePeer(input.channel.handle,input.primary);e!=nil{return e}
 if e=input.binding.originals.verifyPrebound(input.binding);e!=nil{return e}
 // Frame's fixed source-owned storage must be covered by complete runtime /
 // stack instrumentation. This copy is not physical accounting evidence.
 if input.failed.Load(){return ErrClosure}
 copy(frame.Body[:length],body);frame.Length=length;input.sequence++;input.frameLease.complete=true;return nil
}
func(input *primaryFrameInput)readExact(offset,length uint32)error{
 if length==0||length>FrameBytes||uint64(offset)+uint64(length)>FrameBytes+36{return ErrInvalid}
 completed:=uint32(0)
 for completed<length{
  if input.readResult.started&&!input.readResult.complete{return ErrPhase}
  input.readResult=originalFrameIOResult{started:true,memory:input.memory,offset:offset+completed,length:length-completed}
  result:=&input.readResult
  n,e:=readOwnedNativeIO(input.channel.handle,input.memory,offset+completed,length-completed,input.deadline,&input.frameLease)
  result.count=n;result.nativeError=e;result.returned=true
  var observed []byte
  if n>0&&uint32(n)<=length-completed{observed=input.memory.bytes[offset+completed:offset+completed+uint32(n)]}
  outcome:="success";kind:="read";if e!=nil||n<0||uint32(n)>length-completed{outcome="unresolved"}else if n==0{kind="eof";observed=input.memory.bytes[offset+completed:offset+completed]}
  result.observed=observed
  tick,clockError:=nativeTickOriginal(&result.clock);result.clockError=clockError;if clockError!=nil{return clockError}
  scratch:=input.memory.bytes[FrameBytes+36:FrameBytes+36+2048]
  size,captureError:=primitiveInto(scratch,kind,nativeReadOperation,outcome,input.channel.identity,input.channel.identity,input.channel.source,tick,observed,nil)
  result.receiptBytes=size;result.receiptError=captureError;if captureError!=nil{return captureError};prefix:=U32(uint32(size))
  captureError=input.nativeCapture.writeOriginal(prefix[:]);result.nativePrefixError=captureError;result.nativePrefixReturned=true;if captureError!=nil{return captureError}
  captureError=input.nativeCapture.writeOriginal(scratch[:size]);result.nativeCaptureError=captureError;result.nativeCaptureReturned=true;if captureError!=nil{return captureError}
  input.nativeHash.Write(prefix[:]);input.nativeHash.Write(scratch[:size]);input.nativeBytes+=uint64(4+size)
  if len(observed)>0{captureError=input.rawCapture.writeOriginal(observed);result.rawCaptureError=captureError;result.rawCaptureReturned=true;if captureError!=nil{return captureError};input.rawHash.Write(observed);input.rawBytes+=uint64(len(observed))}
  if e!=nil||n<=0||uint32(n)>length-completed{return ErrClosure}
  completed+=uint32(n)
  result.complete=true
 }
 return nil
}
func(input *primaryFrameInput)flushOriginalCaptures()error{
 var rawDigest,nativeDigest [32]byte
 return input.frameLease.flushCaptures(input.rawCapture,input.nativeCapture,
  input.rawBytes,input.nativeBytes,input.rawHash.Sum(rawDigest[:0]),input.nativeHash.Sum(nativeDigest[:0]))
}
