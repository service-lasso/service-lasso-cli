//go:build linux || windows

package hostadmission

import (
 "crypto/sha256"
 "encoding/binary"
 "hash"
 "sync"
 "sync/atomic"
)

type primaryFrameOutput struct{
 mu sync.Mutex
 active bool
 input *primaryFrameInput
 rawCapture,nativeCapture *nativeEvidenceFile
 rawHash,nativeHash hash.Hash
 rawBytes,nativeBytes uint64
 sequence uint32
 failed atomic.Bool
 writeResult originalFrameIOResult
 frameLease originalFrameLease
}
// One pre-effect owner cell. Successful rows transfer their complete bytes and
// native receipt to the already reserved original captures before reuse. A
// failed/uncertain row keeps this cell and its actual mapping borrow forever;
// no second effect overwrites raw outputs while diagnostics are failing.
type originalFrameIOResult struct {
 started,returned,complete,retainedBorrow bool
 nativePrefixReturned,nativeCaptureReturned,rawCaptureReturned bool
 memory *nativeMemory
 offset,length uint32
 count int
 receiptBytes int
 nativeError,clockError,receiptError,nativePrefixError,nativeCaptureError,rawCaptureError error
 clock nativeClockCapture
 observed []byte
}
func(output *primaryFrameOutput)write(frame *Frame)(err error){
 if output==nil||output.input==nil||frame==nil{return ErrInvalid};input:=output.input
 output.mu.Lock();if output.active||output.frameLease.borrowed&&!output.frameLease.returned{output.mu.Unlock();return ErrPhase};output.active=true;output.mu.Unlock()
 defer func(){output.mu.Lock();output.active=false;output.mu.Unlock()}()
 defer func(){if err!=nil{output.failed.Store(true);input.failed.Store(true);if input.memory!=nil&&input.memory.accounting!=nil{input.memory.accounting.holdFailure()}}}()
 if output.failed.Load()||input.failed.Load()||input.memory==nil||input.channel==nil||input.channel.kind!=2||input.primary==nil||input.binding==nil||output.rawCapture==nil||output.nativeCapture==nil||frame.Length<7||frame.Length>256||frame.Sequence()!=output.sequence{return ErrInvalid}
 if e:=validatePrimaryResult(frame);e!=nil{return e}
 switch frame.Operation(){case 2:if output.sequence!=0{return ErrPhase};case 4:if output.sequence==0||output.sequence>1024{return ErrPhase};case 6:if output.sequence==0||output.sequence>1025{return ErrPhase};default:return ErrInvalid}
 profile,e:=originalProfile(input.profile);if e!=nil{return e};serviceImage,e:=ParseDigest(profile.Service.ImageSHA256);if e!=nil{return e}
 if input.memory.accounting==nil||input.memory.accounting.domain!=1||input.memory.accounting.limit!=ControlBytes||input.memory.accounting.contract.profile!=profile.Digest||input.deadline==nil||input.nonce==[32]byte{}||input.capability==[32]byte{}||input.binding.originals==nil||input.binding.primary!=input.primary.identity||digestString(input.primary.image)!=profile.Launch.Primary.ImageSHA256{return ErrUnavailable}
 if e=input.binding.originals.verifyPrebound(input.binding);e!=nil{return e}
 if output.rawCapture.root==nil||output.nativeCapture.root==nil||output.rawCapture.root.profile!=profile.Digest||output.nativeCapture.root.profile!=profile.Digest||output.rawCapture.source!=serviceImage||output.nativeCapture.source!=serviceImage||output.rawCapture.run!=input.binding.run||output.nativeCapture.run!=input.binding.run||output.rawCapture==output.nativeCapture||output.rawCapture.identity==output.nativeCapture.identity||output.rawCapture.handle==output.nativeCapture.handle{return ErrInvalid}
 for _,out:=range []*nativeEvidenceFile{output.rawCapture,output.nativeCapture}{for _,in:=range []*nativeEvidenceFile{input.rawCapture,input.nativeCapture}{if in==nil||out==in||out.identity==in.identity||out.handle==in.handle{return ErrInvalid}}}
 if e=input.channel.recheckIdentity(input.channel.identity,serviceImage);e!=nil{return e};if e=correlateOriginalFramePeer(input.channel.handle,input.primary);e!=nil{return e}
 if e=output.rawCapture.recheckOriginal();e!=nil{return e};if e=output.nativeCapture.recheckOriginal();e!=nil{return e}
 if output.frameLease.borrowed&&!output.frameLease.returned{return ErrPhase}
 output.frameLease=originalFrameLease{declaredLength:frame.Length}
 if e=input.memory.borrowFrameOriginal(&output.frameLease);e!=nil{return e}
 defer func(){
  output.frameLease.primaryError=err
  if err!=nil&&!output.frameLease.rawFlushReturned&&output.rawHash!=nil&&output.nativeHash!=nil{
   var rawDigest,nativeDigest [32]byte
   _ = output.frameLease.flushCaptures(output.rawCapture,output.nativeCapture,
    output.rawBytes,output.nativeBytes,output.rawHash.Sum(rawDigest[:0]),output.nativeHash.Sum(nativeDigest[:0]))
  }
  returnError:=output.frameLease.finish(err)
  output.writeResult.retainedBorrow=output.frameLease.retained
  if returnError!=nil&&err==nil{err=returnError}
 }()
 if len(input.memory.bytes)<FrameBytes+36+2048{return ErrCapacity}
 binary.BigEndian.PutUint32(input.memory.bytes[:4],frame.Length);copy(input.memory.bytes[4:4+frame.Length],frame.Bytes())
 mac:=MAC(HostDomain,1,input.nonce,input.capability,output.sequence,frame.Bytes());copy(input.memory.bytes[4+frame.Length:4+frame.Length+32],mac[:])
 if output.rawHash==nil{output.rawHash=sha256.New()};if output.nativeHash==nil{output.nativeHash=sha256.New()}
 total:=frame.Length+36;completed:=uint32(0)
 for completed<total{
  if output.writeResult.started&&!output.writeResult.complete{return ErrPhase}
  output.writeResult=originalFrameIOResult{started:true,memory:input.memory,offset:completed,length:total-completed}
  result:=&output.writeResult
  n,e:=writeOwnedNativeOnce(input.channel.handle,input.memory,completed,total-completed,input.deadline,&output.frameLease)
  result.count=n;result.nativeError=e;result.returned=true
  var observed []byte;if n>0&&uint32(n)<=total-completed{observed=input.memory.bytes[completed:completed+uint32(n)]}else if n==0&&e==nil{observed=input.memory.bytes[completed:completed]}
  result.observed=observed
  outcome:="success";if e!=nil||n<=0||uint32(n)>total-completed{outcome="unresolved"}
  tick,clockError:=nativeTickOriginal(&result.clock);result.clockError=clockError;if clockError!=nil{return clockError}
  scratch:=input.memory.bytes[FrameBytes+36:FrameBytes+36+2048]
  size,captureError:=primitiveInto(scratch,"write",nativeWriteOperation,outcome,input.channel.identity,input.channel.identity,serviceImage,tick,observed,nil);result.receiptBytes=size;result.receiptError=captureError;if captureError!=nil{return captureError}
  prefix:=U32(uint32(size));captureError=output.nativeCapture.writeOriginal(prefix[:]);result.nativePrefixError=captureError;result.nativePrefixReturned=true;if captureError!=nil{return captureError};captureError=output.nativeCapture.writeOriginal(scratch[:size]);result.nativeCaptureError=captureError;result.nativeCaptureReturned=true;if captureError!=nil{return captureError}
  output.nativeHash.Write(prefix[:]);output.nativeHash.Write(scratch[:size]);output.nativeBytes+=uint64(4+size)
  if len(observed)>0{captureError=output.rawCapture.writeOriginal(observed);result.rawCaptureError=captureError;result.rawCaptureReturned=true;if captureError!=nil{return captureError};output.rawHash.Write(observed);output.rawBytes+=uint64(len(observed))}
  if e!=nil||n<=0||uint32(n)>total-completed{return ErrClosure};completed+=uint32(n)
  result.complete=true
 }
 var rawDigest,nativeDigest [32]byte
 e=output.frameLease.flushCaptures(output.rawCapture,output.nativeCapture,
  output.rawBytes,output.nativeBytes,output.rawHash.Sum(rawDigest[:0]),output.nativeHash.Sum(nativeDigest[:0]));if e!=nil{return e}
 if _,e=input.deadline.remaining();e!=nil{return e}
 // The same retained channel remains the write object through readback. After
 // CLOSE_INTENT_ACK the original primary may close and exit immediately; that
 // selected transition must not require a still-live peer after delivery.
 // Physical exit, ACK receipt, EOF and final closure remain independent proof
 // obligations. Earlier responses still require the live original binding.
 if e=input.channel.recheckIdentity(input.channel.identity,serviceImage);e!=nil{return e}
 if frame.Operation()!=6{
  if e=input.primary.recheckOriginal();e!=nil{return e}
  if e=correlateOriginalFramePeer(input.channel.handle,input.primary);e!=nil{return e}
  if e=input.binding.originals.verifyPrebound(input.binding);e!=nil{return e}
 }
 if output.failed.Load()||input.failed.Load(){return ErrClosure}
 output.sequence++;output.frameLease.complete=true;return nil
}
