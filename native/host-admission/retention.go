//go:build linux || windows

package hostadmission

import (
 "crypto/sha256"
 "hash"
 "sync"
)

type retentionCapacity struct { mu sync.Mutex; maximum,reserved uint64; uncertain bool }
type nativeRetention struct {
 mu sync.Mutex
 root *nativeRetentionRoot
 capacity *retentionCapacity
 initialMaximum,finalMaximum uint64
 initialUsed,finalUsed uint64
 initialRecords,finalRecords uint32
 run,source [32]byte
 archiveRaw []byte
 archiveDigest [32]byte
 archiveFile *nativeEvidenceFile
 finalFile *nativeEvidenceFile
 originalReceipt []byte
 reservation reservation
 sealed,failed bool
}
type nativeCapture struct {
 mu sync.Mutex
 original *nativeObject
 sink *nativeEvidenceFile
 nativeReads *nativeEvidenceFile
 retention *nativeRetention
 buffer *nativeMemory
 maximum,count uint64
 readCount uint32
 rawHash hash.Hash
 readsHash hash.Hash
 readsBytes uint64
 digest [32]byte
 readsDigest,eofDigest [32]byte
 eofRaw []byte
 eof,closed,failed bool
 deadline *nativeDeadline
 collecting bool
 eofWitnessBorrowed bool
}
func retentionLeaf(run [32]byte,name string)(string,error){if run==([32]byte{})||!evidenceName(name){return "",ErrInvalid};leaf:=digestString(run)+"."+name;if !evidenceName(leaf){return "",ErrInvalid};return leaf,nil}
func openRetentionCapacity(profile *Profile,root *nativeRetentionRoot)(*retentionCapacity,error){if profile==nil||root==nil{return nil,ErrUnavailable};if e:=root.verifyOriginal(profile);e!=nil{return nil,e};used,e:=root.totalOriginalBytes(profile.RetentionMaximum);if e!=nil{return nil,e};return &retentionCapacity{maximum:profile.RetentionMaximum,reserved:used},nil}

func reserveRetention(profile *Profile,capacity *retentionCapacity,root *nativeRetentionRoot,run,source [32]byte)(*nativeRetention,error){
 if profile==nil||capacity==nil||root==nil||run==([32]byte{})||source==([32]byte{})||profile.RetentionRecordMaximum==0||profile.RetentionMaximum==0{return nil,ErrUnavailable}
 if e:=root.verifyOriginal(profile);e!=nil{return nil,e}
 maximum:=profile.RetentionRecordMaximum;if maximum>profile.RetentionMaximum/2{return nil,ErrCapacity}
 capacity.mu.Lock();defer capacity.mu.Unlock();if capacity.uncertain||capacity.maximum!=profile.RetentionMaximum||capacity.reserved>capacity.maximum-2*maximum{return nil,ErrCapacity};capacity.reserved+=2*maximum
 return &nativeRetention{root:root,capacity:capacity,initialMaximum:maximum,finalMaximum:maximum,run:run,source:source},nil
}
func(r *nativeRetention)verifyReserved()error{
 if r==nil{return ErrUnavailable};r.mu.Lock();defer r.mu.Unlock()
 if r.root==nil||r.capacity==nil||r.initialMaximum==0||r.finalMaximum==0||r.failed||r.run==[32]byte{}||r.source==[32]byte{}||r.initialUsed>r.initialMaximum||r.finalUsed>r.finalMaximum{return ErrUnavailable}
 // This checks the original aggregate reservation owner, not a caller's
 // asserted free-space value. It cannot establish provider/90-day authority.
 r.capacity.mu.Lock();invalid:=r.capacity.uncertain||r.capacity.reserved>r.capacity.maximum||r.initialMaximum>r.capacity.reserved||r.finalMaximum>r.capacity.reserved-r.initialMaximum;r.capacity.mu.Unlock()
 if invalid{r.failed=true;return ErrUnavailable};return r.root.recheckIdentity()
}
func(r *nativeRetention)reserveBytes(bytes uint64,final bool)error{
 if r==nil{return ErrUnavailable};r.mu.Lock();defer r.mu.Unlock();if r.failed||!final&&r.sealed{return ErrPhase};if r.initialUsed>r.initialMaximum||r.finalUsed>r.finalMaximum{r.failed=true;return ErrInvalid};if final{if bytes>r.finalMaximum-r.finalUsed{r.failed=true;return ErrCapacity};r.finalUsed+=bytes}else{if bytes>r.initialMaximum-r.initialUsed{r.failed=true;return ErrCapacity};r.initialUsed+=bytes};return nil
}
func(r *nativeRetention)createRecord(name string,maximum uint64,final bool)(*nativeEvidenceFile,error){
 if !evidenceName(name)||maximum==0{return nil,ErrInvalid};if e:=r.reserveBytes(maximum,final);e!=nil{return nil,e};r.mu.Lock();if final{if r.finalRecords>=4096{r.failed=true;r.mu.Unlock();return nil,ErrCapacity};r.finalRecords++}else{if r.initialRecords>=4096{r.failed=true;r.mu.Unlock();return nil,ErrCapacity};r.initialRecords++};r.mu.Unlock()
 file,e:=r.root.createOriginal(name,maximum,r.run,r.source);if e!=nil{r.mu.Lock();r.failed=true;r.mu.Unlock();return nil,e};return file,nil
}
func evidenceName(name string)bool{if len(name)<1||len(name)>240||name=="."||name==".."{return false};for _,c:=range name{if !(c>='A'&&c<='Z'||c>='a'&&c<='z'||c>='0'&&c<='9'||c=='.'||c=='_'||c=='-'){return false}};return true}

// Captures have one actual collector and one original native read end. The
// observer receives identity/readback custody, never a competing reader.
func newCapture(original *nativeObject,retention *nativeRetention,name string,maximum uint64,control *nativeAccounting,deadline *nativeDeadline)(*nativeCapture,error){
 // Selected v2 nativeClass3 is the actual stream; class4 is an input-file.
 // This structural discriminant remains subordinate to the genuine retained
 // original native stream/source constructor and its pre-effect ownership.
 if original==nil||original.kind!=3||retention==nil||maximum==0||deadline==nil||!evidenceName(name)||!evidenceName(name+".native"){return nil,ErrInvalid};if e:=original.recheckIdentity(original.identity,original.source);e!=nil{return nil,e};if e:=prepareOwnedIO(original.handle);e!=nil{return nil,e}
 file,e:=retention.createRecord(name,maximum,false);if e!=nil{return nil,e};reads,e:=retention.createRecord(name+".native",1025*2052,false);if e!=nil{return nil,e};buffer,e:=allocateNativeMemory(control,FrameBytes+2048,nil);if e!=nil{return nil,e}
 return &nativeCapture{original:original,sink:file,nativeReads:reads,retention:retention,buffer:buffer,maximum:maximum,rawHash:sha256.New(),readsHash:sha256.New(),deadline:deadline},nil
}
func(c *nativeCapture)verifyOriginal()error{if c==nil||c.original==nil||c.sink==nil||c.buffer==nil{return ErrUnavailable};c.mu.Lock();failed:=c.failed;c.mu.Unlock();if failed{return ErrUnavailable};if e:=c.original.recheckIdentity(c.original.identity,c.original.source);e!=nil{return e};return c.sink.recheckOriginal()}
func(c *nativeCapture)collect(forward func([]byte)error)(result error){
 if c==nil{return ErrInvalid};c.mu.Lock();if c.collecting||c.failed||c.eof{c.mu.Unlock();return ErrPhase};c.collecting=true;c.mu.Unlock()
 if e:=c.verifyOriginal();e!=nil{c.markFailure();return e};if e:=c.buffer.borrowOriginal();e!=nil{c.markFailure();return e};defer func(){c.mu.Lock();retainedEOF:=c.eofWitnessBorrowed;c.mu.Unlock();if !retainedEOF{if e:=c.buffer.returnOriginal();e!=nil{c.markFailure();if result==nil{result=e}}}}()
 for{
  if c.readCount>=1025{c.markFailure();return ErrCapacity}
  n,e:=readOwnedNativeIO(c.original.handle,c.buffer,0,FrameBytes,c.deadline,nil);c.readCount++
  if e!=nil||n<0||n>FrameBytes{var observed []byte;if n>0&&n<=FrameBytes{observed=c.buffer.bytes[:n]};tick,clockErr:=nativeTick();if clockErr==nil{size,primitiveErr:=primitiveInto(c.buffer.bytes[FrameBytes:FrameBytes+2048],"read",nativeReadOperation,"unresolved",c.original.identity,c.original.identity,c.original.source,tick,observed,nil);if primitiveErr==nil{raw:=c.buffer.bytes[FrameBytes:FrameBytes+size];length:=U32(uint32(size));if c.nativeReads.writeOriginal(length[:])==nil{c.nativeReads.writeOriginal(raw)}}};c.markFailure();return ErrClosure}
  tick,e:=nativeTick();if e!=nil{c.markFailure();return e};kind:="read";if n==0{kind="eof"};size,e:=primitiveInto(c.buffer.bytes[FrameBytes:FrameBytes+2048],kind,nativeReadOperation,"success",c.original.identity,c.original.identity,c.original.source,tick,c.buffer.bytes[:n],nil);if e!=nil{c.markFailure();return e};primitive:=c.buffer.bytes[FrameBytes:FrameBytes+size];length:=U32(uint32(size));if e=c.nativeReads.writeOriginal(length[:]);e!=nil{c.markFailure();return e};if e=c.nativeReads.writeOriginal(primitive);e!=nil{c.markFailure();return e}
  c.readsHash.Write(length[:]);c.readsHash.Write(primitive);c.readsBytes+=uint64(4+len(primitive))
  if n==0{
   if e=c.sink.flushAndReadback(c.count,c.rawHash.Sum(nil));e!=nil{c.markFailure();return e}
   if e=c.nativeReads.flushAndReadback(c.readsBytes,c.readsHash.Sum(nil));e!=nil{c.markFailure();return e}
   // The EOF primitive borrows this original native scratch. Keep its actual
   // lease through archival; returning the collector lease here would permit
   // unmapping while proof still refers to the original primitive bytes.
   c.mu.Lock();copy(c.digest[:],c.rawHash.Sum(nil));copy(c.readsDigest[:],c.readsHash.Sum(nil));c.eofRaw=primitive;c.eofDigest=sha256.Sum256(primitive);c.eof=true;c.eofWitnessBorrowed=true;c.mu.Unlock();return nil
  }
  c.mu.Lock();if c.eof||c.failed||uint64(n)>c.maximum-c.count{c.failed=true;c.mu.Unlock();return ErrCapacity};c.count+=uint64(n);c.mu.Unlock()
  bytes:=c.buffer.bytes[:n];if e=c.sink.writeOriginal(bytes);e!=nil{c.markFailure();return e};c.rawHash.Write(bytes)
  // Forwarding operates on this bounded original frame before buffer reuse.
  // It cannot enqueue, truncate or replace a failed original write as success.
  if forward!=nil{if e=forward(bytes);e!=nil{c.markFailure();return e}}
 }
}
func(c *nativeCapture)markFailure(){c.mu.Lock();c.failed=true;c.mu.Unlock();if c.buffer!=nil{c.buffer.accounting.holdFailure()}}
func(c *nativeCapture)verifyEOF()(err error){
 if c==nil{return ErrClosure};c.mu.Lock()
 defer func(){if err!=nil{c.failed=true};c.mu.Unlock();if err!=nil&&c.buffer!=nil&&c.buffer.accounting!=nil{c.buffer.accounting.holdFailure()}}()
 if c.failed||!c.eof||!c.eofWitnessBorrowed||len(c.eofRaw)==0||c.buffer==nil||c.sink==nil||c.nativeReads==nil||c.readsBytes==0||c.readsDigest==[32]byte{}||c.eofDigest==[32]byte{}||sha256.Sum256(c.eofRaw)!=c.eofDigest{return ErrClosure}
 // The original EOF primitive still borrows the original native scratch;
 // capture bytes and their native read/EOF transcript are independently
 // rechecked at closure and archive use, including a genuine empty stream.
 if e:=c.buffer.recheckOriginal();e!=nil{return e}
 c.buffer.mu.Lock();borrowed:=c.buffer.borrowers==1;c.buffer.mu.Unlock();if !borrowed{return ErrClosure}
 if e:=c.nativeReads.flushAndReadback(c.readsBytes,c.readsDigest[:]);e!=nil{return e}
 if e:=c.sink.flushAndReadback(c.count,c.digest[:]);e!=nil{return e};return nil
}
func(r *nativeRetention)verifyArchiveOriginal(archive *verifiedArchive,terminal terminalTransaction)error{
 if r==nil||archive==nil{return ErrClosure};r.mu.Lock();defer r.mu.Unlock()
 if r.failed||r.root==nil||r.archiveFile==nil||!r.sealed||r.archiveDigest!=archive.digest||sha256.Sum256(archive.raw)!=archive.digest||r.reservation.id!=terminal.reservation.id{return ErrClosure}
 if e:=r.archiveFile.flushAndReadback(uint64(len(archive.raw)),archive.digest[:]);e!=nil{return e}
 if len(r.originalReceipt)!=len(terminal.body)||!bytesEqual(r.originalReceipt,terminal.body){return ErrInvalid};return r.root.recheckIdentity()
}
