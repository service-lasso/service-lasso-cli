//go:build linux || windows

package hostadmission

import "crypto/sha256"

// This recheck is a custody comparison, never an admission constructor. The
// private bootstrap must separately authenticate the original caller/provider,
// source ROOT and genuine Git association before interpreting the returned
// original bytes. No profile seal or accounting contract is installed here.
// A streamed measurement cannot be mistaken for a complete retained body.
func recheckOriginalSourceBody(handle objectHandle,memory *nativeMemory,original *sourceObjectObservation)(raw []byte,err error){
 if memory==nil||original==nil{return nil,ErrInvalid}
 memory.mu.Lock();defer memory.mu.Unlock()
 defer func(){if err!=nil{memory.sourceBorrowFailed=true;if memory.accounting!=nil{memory.accounting.holdFailure()}}}()
 if memory.accounting==nil||memory.accounting.domain!=1||!memory.sourceBorrow||memory.sourceBorrowFailed||memory.borrowers!=1||original.streamed||!original.eofObserved||!sourceObjectEOF(original.eofCount,original.eofError){return nil,ErrInvalid}
 if e:=memory.recheckLocked();e!=nil{return nil,e}
 if original.bytes==0||original.bytes>FrameBytes||original.bytes>uint64(len(memory.bytes))||original.identity==[32]byte{}||original.rawSHA256==[32]byte{}||original.gitBlob==[20]byte{}{return nil,ErrInvalid}
 if !sameSourceObjectObservation(original.before,original.after)||original.before.identity!=original.identity||original.before.owner!=original.owner||original.before.size!=original.bytes{return nil,ErrInvalid}
 before,e:=observeSourceObject(handle);if e!=nil{return nil,e}
 if !sameSourceObjectObservation(original.after,before){return nil,ErrInvalid}
 raw=memory.bytes[:original.bytes]
 if sha256.Sum256(raw)!=original.rawSHA256{return nil,ErrInvalid}
 blob,e:=originalGitObjectID("blob",raw);if e!=nil||blob!=original.gitBlob{return nil,ErrInvalid}
 // Saved memory and equal file metadata do not prove current held bytes.
 // Re-read that same original without overwriting its retained native view
 // or allocating another frame-sized source body. The bounded one-byte
 // scratch is reused only after each synchronous original read completes.
 var held [1]byte
 for offset:=uint64(0);offset<original.bytes;offset++{
  n,e:=readSourceObjectAt(handle,held[:],offset)
  if n<0||n>len(held){return nil,ErrInvalid};if e!=nil{return nil,e}
  if n!=1||held[0]!=raw[offset]{return nil,ErrInvalid}
 }
 n,e:=readSourceObjectAt(handle,held[:],original.bytes)
 if !sourceObjectEOF(n,e){return nil,ErrInvalid}
 after,e:=observeSourceObject(handle);if e!=nil{return nil,e}
 if !sameSourceObjectObservation(before,after){return nil,ErrInvalid}
 // The exclusive source borrow stays held. A returned native view cannot
 // retire its original mapping, file, parser, provider or archive obligation.
 return raw,nil
}
func sameSourceObjectObservation(left,right sourceObjectMetadata)bool{
 return left.identity!=[32]byte{}&&left.rawBytes!=0&&left.rawBytes<=uint32(len(left.raw))&&left.identity==right.identity&&left.owner==right.owner&&left.size==right.size&&left.rawBytes==right.rawBytes&&left.raw==right.raw&&left.nativeBasic==right.nativeBasic
}
