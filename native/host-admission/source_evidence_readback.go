//go:build linux || windows

package hostadmission

import (
 "crypto/sha1"
 "crypto/sha256"
 "strconv"
)

// A streamed original measurement is not a retained full-body view. Re-read
// that same held object through its original exclusive bounded scratch before
// using its closed calibration references. Matching saved observations alone
// cannot establish current original byte custody or measurement authority.
func recheckOriginalStreamedEvidence(body *originalSourceBody)(err error){
 if body==nil||body.memory==nil{return ErrInvalid};memory:=body.memory
 memory.mu.Lock();defer memory.mu.Unlock()
 defer func(){if err!=nil{memory.sourceBorrowFailed=true;if memory.accounting!=nil{memory.accounting.holdFailure()}}}()
 original:=&body.observation
 if memory.accounting==nil||memory.accounting.domain!=1||!memory.sourceBorrow||memory.sourceBorrowFailed||memory.borrowers!=1||!original.streamed||!original.eofObserved||!sourceObjectEOF(original.eofCount,original.eofError)||original.bytes==0||original.bytes>SessionBytes||len(memory.bytes)==0||len(memory.bytes)>FrameBytes||original.rawSHA256==[32]byte{}||original.gitBlob==[20]byte{}{return ErrInvalid}
 if e:=memory.recheckLocked();e!=nil{return e}
 if !sameSourceObjectObservation(original.before,original.after)||original.before.identity!=original.identity||original.before.owner!=original.owner||original.before.size!=original.bytes{return ErrInvalid}
 before,e:=observeSourceObject(body.handle);if e!=nil{return e};if !sameSourceObjectObservation(original.after,before){return ErrInvalid}
 rawHash,blobHash:=sha256.New(),sha1.New();blobHash.Write([]byte("blob "));var length [20]byte;blobHash.Write(strconv.AppendUint(length[:0],original.bytes,10));blobHash.Write([]byte{0})
 var offset uint64
 for offset<original.bytes{
  count:=original.bytes-offset;if count>uint64(len(memory.bytes)){count=uint64(len(memory.bytes))}
  n,e:=readSourceEvidenceAt(body.handle,memory.bytes[:count],offset);if n<0||uint64(n)>count{return ErrInvalid};if e!=nil{return e};if n==0{return ErrInvalid}
  rawHash.Write(memory.bytes[:n]);blobHash.Write(memory.bytes[:n]);offset+=uint64(n)
 }
 var eof [1]byte;n,e:=readSourceEvidenceAt(body.handle,eof[:],offset);if !sourceObjectEOF(n,e){return ErrInvalid}
 var rawDigest [32]byte;sum:=rawHash.Sum(rawDigest[:0]);if len(sum)!=32{return ErrInvalid};copy(rawDigest[:],sum)
 var blobDigest [20]byte;sum=blobHash.Sum(blobDigest[:0]);if len(sum)!=20{return ErrInvalid};copy(blobDigest[:],sum)
 if rawDigest!=original.rawSHA256||blobDigest!=original.gitBlob{return ErrInvalid}
 after,e:=observeSourceObject(body.handle);if e!=nil{return e};if !sameSourceObjectObservation(before,after){return ErrInvalid}
 // No mapping, file or original scratch lease is returned or released here.
 return nil
}
