//go:build linux || windows

package hostadmission

import "crypto/subtle"

// A retained key body is confidential native read custody, not an HA2 provider
// capability. The actual independently admitted provider/key-version lease
// must be authenticated separately. Neither keyPath, these bytes, an HMAC nor
// this private struct can authenticate P/G/C or enroll a ledger key.
type originalKeyBody struct{
 handle objectHandle
 memory *nativeMemory
 before,after sourceObjectMetadata
 eofCount int
 eofError error
 eofObserved bool
}
func readOriginalKeyBody(handle objectHandle,memory *nativeMemory)(body originalKeyBody,err error){
 body.handle=handle;body.memory=memory
 if memory==nil||memory.accounting==nil||memory.accounting.domain!=1||len(memory.bytes)<32{return body,ErrInvalid}
 if e:=memory.borrowSourceOriginal();e!=nil{return body,e}
 defer func(){if err!=nil{memory.retainSourceReadFailure()}}()
 before,e:=observeSourceObject(handle);if e!=nil{return body,e};body.before=before
 if before.size!=32{return body,ErrInvalid}
 if e=verifyOriginalKeyPermissions(handle,before);e!=nil{return body,e}
 offset:=0
 for offset<32{
  n,e:=readSourceObjectAt(handle,memory.bytes[offset:32],uint64(offset))
  if n<0||n>32-offset{return body,ErrInvalid};if e!=nil{return body,e};if n==0{return body,ErrInvalid};offset+=n
 }
 var eof [1]byte;body.eofCount,body.eofError=readSourceObjectAt(handle,eof[:],32)
 if !sourceObjectEOF(body.eofCount,body.eofError){return body,ErrInvalid};body.eofObserved=true
 after,e:=observeSourceObject(handle);if e!=nil{return body,e};body.after=after
 if !sameSourceObjectObservation(before,after){return body,ErrInvalid}
 if e=verifyOriginalKeyPermissions(handle,after);e!=nil{return body,e}
 // The confidential key remains solely in the original exclusive native
 // control mapping. Do not hash it into a Git source association, log it,
 // return a public copy, close its original object or release its charge.
 return body,nil
}
func recheckOriginalKeyBody(body *originalKeyBody)(err error){
 if body==nil||body.memory==nil{return ErrInvalid};memory:=body.memory
 memory.mu.Lock();defer memory.mu.Unlock()
 defer func(){if err!=nil{memory.sourceBorrowFailed=true;if memory.accounting!=nil{memory.accounting.holdFailure()}}}()
 if memory.accounting==nil||memory.accounting.domain!=1||!memory.sourceBorrow||memory.sourceBorrowFailed||memory.borrowers!=1||len(memory.bytes)<32||!body.eofObserved||!sourceObjectEOF(body.eofCount,body.eofError)||body.before.size!=32||!sameSourceObjectObservation(body.before,body.after){return ErrInvalid}
 if e:=memory.recheckLocked();e!=nil{return e}
 current,e:=observeSourceObject(body.handle);if e!=nil{return e}
 if !sameSourceObjectObservation(body.after,current){return ErrInvalid}
 if e=verifyOriginalKeyPermissions(body.handle,current);e!=nil{return e}
 // Compare the confidential original body by actual same-object readback;
 // a matching file identity alone cannot detect mutation of the native view.
 // This bounded temporary stack copy belongs to the control runtime domain.
 // Native runtime/stack instrumentation must cover it before positive use.
 var observed [32]byte;defer func(){for i:=range observed{observed[i]=0}}()
 offset:=0
 for offset<len(observed){
  n,e:=readSourceObjectAt(body.handle,observed[offset:],uint64(offset))
  if n<0||n>len(observed)-offset{return ErrInvalid};if e!=nil{return e};if n==0{return ErrInvalid};offset+=n
 }
 var eof [1]byte;count,e:=readSourceObjectAt(body.handle,eof[:],32)
 if !sourceObjectEOF(count,e)||subtle.ConstantTimeCompare(observed[:],memory.bytes[:32])!=1{return ErrInvalid}
 after,e:=observeSourceObject(body.handle);if e!=nil{return e}
 if !sameSourceObjectObservation(current,after){return ErrInvalid}
 return verifyOriginalKeyPermissions(body.handle,after)
}
