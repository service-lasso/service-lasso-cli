//go:build linux || windows

package hostadmission

// Borrowing pins the original native mapping during bounded IO/observation.
// Kernel close is denied while a borrower is active, rather than waiting under
// an admission lock or dereferencing a mapping after concurrent unmapping.
// These private leases never authorize a replacement object or release charge.
func(m *nativeMemory)borrowOriginal()error{
 if m==nil{return ErrInvalid};m.mu.Lock();defer m.mu.Unlock()
 if m.sourceBorrow||m.frameOwner!=nil{return ErrPhase};if m.borrowers>=MaximumReferences{return ErrCapacity};if e:=m.recheckLocked();e!=nil{return e};m.borrowers++;return nil
}
func(m *nativeMemory)returnOriginal()error{
 if m==nil{return ErrInvalid};m.mu.Lock();defer m.mu.Unlock()
 if m.sourceBorrow||m.frameOwner!=nil{return ErrPhase};if m.borrowers==0{m.accounting.holdFailure();return ErrPhase};m.borrowers--;return nil
}

// Original admission bodies remain exclusive native views for their full
// provider/caller/source/parser/observer/readback lifetime. Ordinary IO borrows
// cannot overlap or overwrite them. A failed read permanently retains this
// original view and its charge; no retry or generic return ends its custody.
func(m *nativeMemory)borrowSourceOriginal()error{
 if m==nil{return ErrInvalid};m.mu.Lock();defer m.mu.Unlock()
 if m.accounting==nil||m.accounting.domain!=1||m.sourceBorrow||m.frameOwner!=nil||m.borrowers!=0{return ErrPhase}
 if e:=m.recheckLocked();e!=nil{return e};m.sourceBorrow=true;m.borrowers=1;return nil
}

// Frame exclusion is a private mapping lifetime lease, not source admission.
// It blocks other ordinary and source borrows as well as unmapping. Lower IO
// receives this SAME owner rather than opening another ordinary alias lease.
type originalFrameLease struct {
 memory *nativeMemory
 memoryIdentity [32]byte
 memoryAccounting *nativeAccounting
 memoryParent *nativeProcess
 memoryIndex uint32
 memoryCharge uint64
 memoryLength,memoryCapacity int
 memoryFirst *byte
 memoryNativeOriginal memoryFrameOriginal
 borrowed,complete,retained,returnAttempted,returned bool
 borrowError,returnError error
 primaryError error
 rawFlushError,nativeFlushError error
 rawFlushReturned,nativeFlushReturned bool
 nativeFlushSkipped bool
 nativeFlushEligibilityError error
 rawOriginal,nativeOriginal *nativeEvidenceFile
 declaredLength uint32
}
func(m *nativeMemory)borrowFrameOriginal(owner *originalFrameLease)(result error){
 if m==nil||owner==nil||owner.borrowed{return ErrInvalid}
 owner.memory=m
 defer func(){owner.borrowError=result}()
 m.mu.Lock();defer m.mu.Unlock()
 if m.sourceBorrow||m.frameOwner!=nil||m.borrowers!=0{return ErrPhase}
 if m.accounting==nil||m.identity==([32]byte{}){return ErrUnavailable}
 if e:=m.recheckLocked();e!=nil{return e}
 owner.memoryIdentity=m.identity;owner.memoryAccounting=m.accounting;owner.memoryParent=m.originalOwner
 owner.memoryIndex=m.index;owner.memoryCharge=m.charge;owner.memoryLength=len(m.bytes)
 owner.memoryCapacity=cap(m.bytes);owner.memoryFirst=&m.bytes[0];owner.memoryNativeOriginal=m.frameNativeOriginal()
 m.frameOwner=owner;m.borrowers=1;owner.borrowed=true;return nil
}
func(owner *originalFrameLease)memoryUnchangedLocked()bool{
 m:=owner.memory
 return m!=nil&&m.identity==owner.memoryIdentity&&m.accounting==owner.memoryAccounting&&
  m.originalOwner==owner.memoryParent&&m.index==owner.memoryIndex&&m.charge==owner.memoryCharge&&
  len(m.bytes)==owner.memoryLength&&cap(m.bytes)==owner.memoryCapacity&&len(m.bytes)>0&&
  &m.bytes[0]==owner.memoryFirst&&m.frameNativeOriginal()==owner.memoryNativeOriginal&&
  !m.ended&&!m.closeAttempted
}
func(m *nativeMemory)checkFrameOriginal(owner *originalFrameLease)error{
 if m==nil||owner==nil{return ErrInvalid};m.mu.Lock();defer m.mu.Unlock()
 if m.frameOwner!=owner||owner.memory!=m||!owner.borrowed||owner.retained||
    owner.returnAttempted||m.borrowers!=1||m.sourceBorrow||!owner.memoryUnchangedLocked(){return ErrPhase}
 return nil
}
func(owner *originalFrameLease)finish(primary error)(result error){
 if owner==nil||owner.memory==nil||!owner.borrowed||owner.returnAttempted{return ErrPhase}
 owner.primaryError=primary
 m:=owner.memory;m.mu.Lock()
 if primary!=nil||!owner.complete{
  owner.retained=true;m.mu.Unlock();if owner.memoryAccounting!=nil{owner.memoryAccounting.holdFailure()};return nil
 }
 owner.returnAttempted=true
 if m.frameOwner!=owner||m.borrowers!=1||m.sourceBorrow||!owner.memoryUnchangedLocked(){
  owner.returnError=ErrPhase;owner.retained=true;m.mu.Unlock()
  if owner.memoryAccounting!=nil{owner.memoryAccounting.holdFailure()};return ErrPhase
 }
 m.borrowers=0;m.frameOwner=nil;owner.returned=true;m.mu.Unlock();return nil
}
func(owner *originalFrameLease)flushCaptures(raw,native *nativeEvidenceFile,
 rawCount,nativeCount uint64,rawDigest,nativeDigest []byte)error{
 if owner==nil||!owner.borrowed||owner.returnAttempted||owner.rawFlushReturned||
    owner.nativeFlushReturned||raw==nil||native==nil||raw==native{return ErrPhase}
 owner.rawOriginal=raw;owner.nativeOriginal=native
 rawError:=raw.flushAndReadback(rawCount,rawDigest)
 owner.rawFlushError=rawError;owner.rawFlushReturned=true
 // A failure of one original file does not automatically lose the other.
 // Root/other-file loss is a genuine independent eligibility guard; do not
 // create another effect through a failed owner just to collect a diagnosis.
 native.mu.Lock()
 eligible:=!native.failed&&native.root!=nil&&!native.root.failed.Load()&&
  native.handle!=raw.handle&&native.identity!=raw.identity
 native.mu.Unlock()
 if eligible{
  nativeError:=native.flushAndReadback(nativeCount,nativeDigest)
  owner.nativeFlushError=nativeError;owner.nativeFlushReturned=true
 }else{
  owner.nativeFlushSkipped=true;owner.nativeFlushEligibilityError=ErrUnavailable
 }
 if rawError!=nil{return rawError}
 if owner.nativeFlushSkipped{return owner.nativeFlushEligibilityError}
 return owner.nativeFlushError
}
func borrowFrameOrOrdinary(memory *nativeMemory,owner *originalFrameLease)(ordinary bool,result error){
 if owner!=nil{return false,memory.checkFrameOriginal(owner)}
 return true,memory.borrowOriginal()
}
func(m *nativeMemory)retainSourceReadFailure(){
 if m==nil{return};m.mu.Lock();m.sourceBorrowFailed=true;accounting:=m.accounting;m.mu.Unlock()
 if accounting!=nil{accounting.holdFailure()}
}
