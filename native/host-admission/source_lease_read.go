//go:build linux || windows

package hostadmission

import (
 "crypto/sha1"
 "crypto/sha256"
 "strconv"
)

// These observations are untrusted bytes until the original PA1 caller/provider
// independently authenticates the source association. A hash comparison, native
// handle, owner or this reader cannot issue an admission or profile seal.
type sourceObjectObservation struct {
 identity [32]byte
 owner string
 bytes uint64
 rawSHA256 [32]byte
 gitBlob [20]byte
 before,after sourceObjectMetadata
 eofCount int
 eofError error
 eofObserved,streamed bool
}
type sourceObjectMetadata struct {
 identity [32]byte
 owner string
 size uint64
 raw [128]byte
 rawBytes uint32
 nativeBasic [40]byte
}

// The original admission owner supplies its already-held read-only object and
// original precharged control scratch. No pathname is opened or substituted.
// The caller must retain the same original object/observer/capture leases after
// this read; the exclusive source borrow intentionally remains held on return.
// Returning an observation is not permission to close, release or reuse it.
func readOriginalSourceObject(handle objectHandle,scratch *nativeMemory,maximum uint64)(observation sourceObjectObservation,err error){
 if scratch==nil||scratch.accounting==nil||scratch.accounting.domain!=1||maximum==0||maximum>FrameBytes{return observation,ErrInvalid}
 if e:=scratch.borrowSourceOriginal();e!=nil{return observation,e}
 defer func(){if err!=nil{scratch.retainSourceReadFailure()}}()
 before,e:=observeSourceObject(handle);if e!=nil{return observation,e}
 if before.size==0||before.size>maximum||before.size>uint64(len(scratch.bytes)){return observation,ErrCapacity}
 observation.before=before
 var offset uint64
 for offset<before.size{
  n,e:=readSourceObjectAt(handle,scratch.bytes[offset:before.size],offset)
  if n<0||uint64(n)>before.size-offset{return observation,ErrInvalid}
  if e!=nil{return observation,e};if n==0{return observation,ErrInvalid}
  offset+=uint64(n)
 }
 // A real extra read distinguishes the complete held file from a prefix.
 var eof [1]byte
 n,e:=readSourceObjectAt(handle,eof[:],offset);observation.eofCount=n;observation.eofError=e;if !sourceObjectEOF(n,e){return observation,ErrInvalid};observation.eofObserved=true
 after,e:=observeSourceObject(handle);if e!=nil{return observation,e}
 if before.identity!=after.identity||before.owner!=after.owner||before.size!=after.size||before.rawBytes!=after.rawBytes||before.raw!=after.raw||before.nativeBasic!=after.nativeBasic{return observation,ErrInvalid}
 raw:=scratch.bytes[:before.size]
 observation.identity=before.identity;observation.owner=before.owner;observation.bytes=before.size
 observation.after=after;observation.rawSHA256=sha256.Sum256(raw)
 // Hash the genuine Git blob preimage without allocating a second raw body.
 h:=sha1.New();h.Write([]byte("blob "));var length [20]byte
 digits:=strconv.AppendUint(length[:0],before.size,10);h.Write(digits);h.Write([]byte{0});h.Write(raw)
 var blobDigest [20]byte;sum:=h.Sum(blobDigest[:0]);if len(sum)!=len(observation.gitBlob){return observation,ErrInvalid};copy(observation.gitBlob[:],sum);return observation,nil
}
// Measurement evidence may exceed a profile frame. Hash it from the same held
// original using the existing bounded control scratch, retaining the original
// file lease throughout. This never allocates a SessionBytes-sized control
// buffer or interprets a larger evidence object as a profile/grant/C body.
func observeOriginalCalibrationEvidence(handle objectHandle,scratch *nativeMemory,maximum uint64)(observation sourceObjectObservation,err error){
 if scratch==nil||scratch.accounting==nil||scratch.accounting.domain!=1||maximum==0||maximum>SessionBytes||len(scratch.bytes)==0||len(scratch.bytes)>FrameBytes{return observation,ErrInvalid}
 if e:=scratch.borrowSourceOriginal();e!=nil{return observation,e}
 defer func(){if err!=nil{scratch.retainSourceReadFailure()}}()
 before,e:=observeSourceObject(handle);if e!=nil{return observation,e};if before.size==0||before.size>maximum{return observation,ErrCapacity}
 observation.before=before;rawHash:=sha256.New();blobHash:=sha1.New();blobHash.Write([]byte("blob "));var length [20]byte
 blobHash.Write(strconv.AppendUint(length[:0],before.size,10));blobHash.Write([]byte{0})
 var offset uint64
 for offset<before.size{
  count:=before.size-offset;if count>uint64(len(scratch.bytes)){count=uint64(len(scratch.bytes))}
  n,e:=readSourceEvidenceAt(handle,scratch.bytes[:count],offset)
  if n<0||uint64(n)>count{return observation,ErrInvalid};if e!=nil{return observation,e};if n==0{return observation,ErrInvalid}
  rawHash.Write(scratch.bytes[:n]);blobHash.Write(scratch.bytes[:n]);offset+=uint64(n)
 }
 var eof [1]byte;n,e:=readSourceEvidenceAt(handle,eof[:],offset);observation.eofCount=n;observation.eofError=e;if !sourceObjectEOF(n,e){return observation,ErrInvalid};observation.eofObserved=true;observation.streamed=true
 after,e:=observeSourceObject(handle);if e!=nil{return observation,e}
 if before.identity!=after.identity||before.owner!=after.owner||before.size!=after.size||before.rawBytes!=after.rawBytes||before.raw!=after.raw||before.nativeBasic!=after.nativeBasic{return observation,ErrInvalid}
 observation.identity=before.identity;observation.owner=before.owner;observation.bytes=before.size;observation.after=after
 var rawDigest [32]byte;sum:=rawHash.Sum(rawDigest[:0]);if len(sum)!=32{return observation,ErrInvalid};copy(observation.rawSHA256[:],sum)
 var blobDigest [20]byte;sum=blobHash.Sum(blobDigest[:0]);if len(sum)!=20{return observation,ErrInvalid};copy(observation.gitBlob[:],sum)
 return observation,nil
}