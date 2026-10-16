//go:build linux || windows

package hostadmission

import (
 "crypto/sha256"
 "sync"
)

// This contract is admitted with the complete allocator/runtime source and its
// original native calibration evidence. None of these numbers is accepted from
// a request, environment, digest assertion, finalizer or Go memory target.
type allocationContract struct {
 domain byte
 maximumBytes uint64
 source,profile [32]byte
 nativeObjectBytes,runtimeBaseBytes,stackReservationBytes,ioReservationBytes uint64
 physicalPageBytes uint64
 maximumObjects uint32
 calibrationRaw []byte
 calibrationSHA256 [32]byte
}
var admittedAllocationContracts=[]allocationContract{}
var serviceControlAccountingAttempt struct{mu sync.Mutex;attempted bool}
type allocationEvent struct {
 sequence uint32; requested,mapped,overhead,cumulative uint64; object [32]byte; ended bool
 endBusy,endAttempted,endUnresolved,parentReturned,parentEnded bool
 parentOriginal *nativeProcess
 parentNative nativeOwnershipCapture
 parentError,publicationError,resultError error
}
type nativeAccounting struct {
 mu sync.Mutex
 domain byte // 0 combined primary/SEA/writer/full facade B0, 1 service control, 2 external Node parent/copies B2, 4 admitted observer
 limit,charged,peak uint64
 contract allocationContract
 events [256]allocationEvent
 count uint32
 retained bool
 originalParent *nativeProcess
}
func newAccounting(domain byte,profile,source [32]byte,parent *nativeProcess)(*nativeAccounting,error){
 // The 4MiB store ceiling is combined durable store/transaction/scratch
 // custody, never an additional service-memory allowance. Store buffers must
 // use the original aggregate control account and its separate store reserve.
 if domain==3{return nil,ErrInvalid}
 // One aggregate control owner per original service process, never one 1MiB
 // allowance per request/binding/store operation. A failed admission does not
 // reset this source-owned attempt or install a replacement accounting owner.
 if domain==1{serviceControlAccountingAttempt.mu.Lock();if serviceControlAccountingAttempt.attempted{serviceControlAccountingAttempt.mu.Unlock();return nil,ErrUnavailable};serviceControlAccountingAttempt.attempted=true;serviceControlAccountingAttempt.mu.Unlock()}
 var chosen *allocationContract;for i:=range admittedAllocationContracts{candidate:=&admittedAllocationContracts[i];if candidate.domain==domain&&candidate.profile==profile&&candidate.source==source{if chosen!=nil{return nil,ErrUnavailable};chosen=candidate}}
 if chosen==nil||len(chosen.calibrationRaw)==0||len(chosen.calibrationRaw)>FrameBytes||sha256.Sum256(chosen.calibrationRaw)!=chosen.calibrationSHA256||chosen.maximumObjects==0||chosen.maximumObjects>256||chosen.nativeObjectBytes==0||chosen.runtimeBaseBytes==0||chosen.stackReservationBytes==0||chosen.ioReservationBytes==0||chosen.physicalPageBytes==0||chosen.physicalPageBytes&(chosen.physicalPageBytes-1)!=0{return nil,ErrUnavailable}
 granularity,e:=nativeAllocationGranularity();if e!=nil||chosen.physicalPageBytes<granularity||chosen.physicalPageBytes%granularity!=0{return nil,ErrUnavailable}
 if e=validateAllocationCalibration(chosen);e!=nil{return nil,ErrUnavailable}
 // B0 and B2 are independently source-qualified finite whole-runtime maxima. SessionBytes remains the per-binding copy subset of B2, not its runtime maximum.
 limit:=chosen.maximumBytes;if limit==0||limit>9007199254740991{return nil,ErrUnavailable};switch domain{case 0:case 1:if limit!=ControlBytes{return nil,ErrUnavailable};case 2:if parent==nil||limit<SessionBytes{return nil,ErrUnavailable};case 4:default:return nil,ErrInvalid}
 base:=chosen.runtimeBaseBytes;if base>limit||chosen.stackReservationBytes>limit-base{return nil,ErrCapacity};base+=chosen.stackReservationBytes;if chosen.ioReservationBytes>limit-base{return nil,ErrCapacity};base+=chosen.ioReservationBytes
 if base>limit{return nil,ErrCapacity};return &nativeAccounting{domain:domain,limit:limit,charged:base,peak:base,contract:*chosen,originalParent:parent},nil
}
func(a *nativeAccounting) reserveMapping(requested uint64)(uint32,uint64,error){
 if a==nil{return 0,0,ErrUnavailable};a.mu.Lock();defer a.mu.Unlock();if a.retained||a.count>=a.contract.maximumObjects||requested==0{return 0,0,ErrCapacity}
 page:=a.contract.physicalPageBytes;if requested>^uint64(0)-(page-1){return 0,0,ErrCapacity};mapped:=(requested+page-1)&^(page-1);overhead:=a.contract.nativeObjectBytes
 if mapped>a.limit-a.charged||overhead>a.limit-a.charged-mapped{return 0,0,ErrCapacity};a.charged+=mapped+overhead;if a.charged>a.peak{a.peak=a.charged}
 index:=a.count;a.count++;a.events[index]=allocationEvent{sequence:index+1,requested:requested,mapped:mapped,overhead:overhead,cumulative:a.charged};return index,mapped,nil
}
func(a *nativeAccounting) bindMapping(index uint32,identity [32]byte)error{a.mu.Lock();defer a.mu.Unlock();if index>=a.count||identity==([32]byte{})||a.events[index].object!=([32]byte{}){a.retained=true;return ErrInvalid};a.events[index].object=identity;return nil}
func(a *nativeAccounting) endMapping(index uint32,identity [32]byte)(result error){
 if a==nil{return ErrInvalid}
 a.mu.Lock()
 if a.retained||index>=a.count||a.events[index].object!=identity||a.events[index].ended||
    a.events[index].endAttempted{a.retained=true;a.mu.Unlock();return ErrInvalid}
 event:=&a.events[index]
 event.endAttempted=true;event.endBusy=true;event.parentOriginal=a.originalParent
 sequence,requested,mapped,overhead,cumulative:=event.sequence,event.requested,event.mapped,event.overhead,event.cumulative
 domain,parent:=a.domain,event.parentOriginal
 a.mu.Unlock()
 // This event owns its once-only result cell while native lifetime observation
 // runs outside accounting metadata. Retain the actual result before reentry.
 if domain==2{
  if parent==nil{event.parentError=ErrClosure}else{
   event.parentEnded,event.parentError=parent.ownershipEndedOriginal(&event.parentNative);event.parentReturned=true
  }
 }
 a.mu.Lock();defer a.mu.Unlock()
 defer func(){event.resultError=result}()
 if index>=a.count||event!=&a.events[index]||!event.endBusy||event.ended||
    event.sequence!=sequence||event.requested!=requested||event.mapped!=mapped||
    event.overhead!=overhead||event.cumulative!=cumulative||event.object!=identity||
    a.domain!=domain||a.originalParent!=parent{
  event.publicationError=ErrClosure;event.endUnresolved=true;a.retained=true;return ErrClosure
 }
 event.endBusy=false
 // Returned Node Buffer data and metadata remain conservatively charged until
 // the original authenticated parent has actually exited. GC is irrelevant.
 if domain==2&&(!event.parentReturned||event.parentError!=nil||!event.parentEnded){event.endUnresolved=true;a.retained=true;return ErrClosure}
 if a.retained||event.mapped>a.charged||event.overhead>a.charged-event.mapped{event.endUnresolved=true;a.retained=true;return ErrInvalid}
 // Actual unmap retires mapped storage only. The original nativeMemory record
 // remains reachable in retainedMemories, and its measured metadata cannot be
 // silently released with the kernel mapping. Whole registry/arena ownership
 // must separately end that source-owned metadata before its charge can retire.
 a.charged-=event.mapped;event.ended=true;return nil
}
func(a *nativeAccounting) holdFailure(){a.mu.Lock();a.retained=true;a.mu.Unlock()}

// Runtime/stack/IO overhead is a separately admitted source-domain reservation,
// not a claim that a declared ceiling or native mmap proves whole-process RSS.
// Positive qualification requires original peak/copy/native closure evidence.

// Calibration is immutable source admission data, never a runtime request.
// Every declared overhead is bound to its original retained measurement record;
// an equal hash or this parser alone is not a native qualification result.
func validateAllocationCalibration(c *allocationContract)error{
 if c==nil||c.maximumBytes==0{return ErrInvalid};value,e:=decodeStrict(c.calibrationRaw);if e!=nil{return e};row,e:=object(value,"schema","profileDigest","sourceDigest","domain","maximumBytes","nativeObjectBytes","runtimeBaseBytes","stackReservationBytes","ioReservationBytes","physicalPageBytes","maximumObjects","rawEvidence","sourceOwnedPeakBytes");if e!=nil{return e}
 schema,e:=text(row["schema"],80);if e!=nil||schema!="service-lasso-native-allocation-calibration.v1"{return ErrInvalid}
 profile,e:=digestText(row["profileDigest"],64);if e!=nil||profile!=digestString(c.profile){return ErrInvalid};source,e:=digestText(row["sourceDigest"],64);if e!=nil||source!=digestString(c.source){return ErrInvalid}
 domain,e:=zeroInteger(row["domain"],4);if e!=nil||byte(domain)!=c.domain||equalInteger(row["maximumBytes"],c.maximumBytes)!=nil{return ErrInvalid}
 for _,binding:=range []struct{key string;value uint64}{{"nativeObjectBytes",c.nativeObjectBytes},{"runtimeBaseBytes",c.runtimeBaseBytes},{"stackReservationBytes",c.stackReservationBytes},{"ioReservationBytes",c.ioReservationBytes},{"physicalPageBytes",c.physicalPageBytes},{"maximumObjects",uint64(c.maximumObjects)}}{if equalInteger(row[binding.key],binding.value)!=nil{return ErrInvalid}}
 evidence,ok:=row["rawEvidence"].([]any);if !ok||len(evidence)<1||len(evidence)>128{return ErrInvalid};names:=map[string]struct{}{}
 for _,entry:=range evidence{body,e:=object(entry,"name","sha256","size","sourceDigest");if e!=nil{return e};name,e:=text(body["name"],240);if e!=nil||!evidenceName(name){return ErrInvalid};if _,exists:=names[name];exists{return ErrInvalid};names[name]=struct{}{};if _,e=digestText(body["sha256"],64);e!=nil{return e};if _,e=integer(body["size"],SessionBytes);e!=nil{return e};bound,e:=digestText(body["sourceDigest"],64);if e!=nil||bound!=source{return ErrInvalid}}
 peak,e:=integer(row["sourceOwnedPeakBytes"],c.maximumBytes);if e!=nil{return e};if c.runtimeBaseBytes>c.maximumBytes||c.stackReservationBytes>c.maximumBytes-c.runtimeBaseBytes||c.ioReservationBytes>c.maximumBytes-c.runtimeBaseBytes-c.stackReservationBytes{return ErrCapacity};if peak>c.runtimeBaseBytes+c.stackReservationBytes+c.ioReservationBytes{return ErrCapacity};return nil
}
