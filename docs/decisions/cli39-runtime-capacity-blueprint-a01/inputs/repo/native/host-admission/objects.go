//go:build linux || windows

package hostadmission

import (
 "crypto/sha256"
 "sync"
 "sync/atomic"
)

// The platform constructors retain the actual original native object. No
// exported API accepts a PID/handle integer, a closure boolean or a replacement
// object as authority for one of these records.
type originalObject struct {
 id uint32
 kind byte
 native *nativeObject
 identity [32]byte
 source [32]byte
 latest [32]byte
 state byte
 charged uint64
 closed bool
 // Genuine private pre-effect constructor retains the complete canonical
 // source descriptor. A parsed request never supplies this original cell.
 descriptor resourceRow
 version uint64
}
type originalObjects struct {
 mu sync.Mutex
 rows [MaximumReferences]originalObject
 count uint32
 primary,observer,run [32]byte
 profile [32]byte
 lastPhase byte
 latestRaw []byte
 latestDigest [32]byte
 authenticated,terminal bool
 originalStdout,originalStderr *nativeCapture
 originalArchive *nativeRetention
 intentACKObserved bool
 primaryTerminalBitmap [16]byte
 // One embedded working cell; descriptor/pin storage remains charged to the
 // original owner. Complete constructor and native caller integration follow.
 pending originalPending
 revision,nextOrdinal uint64
 transitionUnresolved atomic.Bool
 // Bound by the genuine original pre-effect constructor; requests never set
 // these source caller/deadline identities. Constructor wiring is unfinished.
 bindingOriginal *originalBinding
 callerOriginal *nativeObject
 deadlineOriginal *nativeDeadline
 // Owned by THIS original binding through its independent native archive
 // lifetime, even after the ledger slot is reused by another original binding.
 terminalCapture originalTerminalCapture
}
func(original *originalObject)currentRow()resourceRow{
 expected:=original.descriptor
 // Current dynamic cells and the immutable source/owner/native geometry must
 // both match. Descriptor construction and native source joins remain required.
 expected.id=original.id;expected.kind=original.kind;expected.identity=original.identity
 expected.observation=original.latest;expected.state=original.state;expected.charged=original.charged
 return expected
}
func(original *originalObject)matchesClaim(row resourceRow)bool{
 return original!=nil&&row==original.currentRow()
}
func(o *originalObjects)verifyPrebound(binding *originalBinding)error{
 if o==nil||binding==nil{return ErrUnavailable}
 o.mu.Lock();defer o.mu.Unlock()
 if !o.authenticated||o.terminal||o.primary!=binding.primary||o.observer!=binding.observer||o.run!=binding.run||o.count==0||o.count>MaximumReferences||o.originalStdout==nil||o.originalStderr==nil||o.originalArchive==nil{return ErrUnavailable}
 // Source/image/owner and original process/channel checks occur without the
 // ledger lock. Collector custody is original; it does not compete for bytes.
 if e:=o.originalStdout.verifyOriginal();e!=nil{return e};if e:=o.originalStderr.verifyOriginal();e!=nil{return e};if e:=o.originalArchive.verifyReserved();e!=nil{return e}
 primary:=false
 for i:=uint32(0);i<o.count;i++{row:=&o.rows[i];if row.native==nil||row.identity==([32]byte{})||row.source==([32]byte{})||row.latest==([32]byte{}){return ErrInvalid};if e:=row.native.recheckIdentity(row.identity,row.source);e!=nil{return e};if row.kind==0&&row.descriptor.processRole==1{if primary||row.identity!=binding.primary||row.descriptor.membership()!=0||!row.native.primaryTerminalEdge{return ErrInvalid};primary=true}}
 if !primary{return ErrInvalid};return nil
}
func(o *originalObjects)snapshotDigest(reservation [32]byte)[32]byte{o.mu.Lock();defer o.mu.Unlock();return DomainHash("SLCLI-HOST-SNAPSHOT-1\x00",reservation[:],o.latestRaw)}
func(o *originalObjects)validateSnapshot(raw []byte,tick uint64)(*verifiedSnapshot,error){
 parsed,e:=parseSnapshot(raw);if e!=nil{return nil,e};o.mu.Lock();defer o.mu.Unlock()
 if !o.authenticated||o.terminal||parsed.count!=o.count||parsed.phase<o.lastPhase{return nil,ErrPhase}
 for i:=uint32(0);i<parsed.count;i++{requested:=parsed.rows[i];original:=&o.rows[i]
  if !original.matchesClaim(requested){return nil,ErrInvalid}
  if original.native==nil{return nil,ErrInvalid};if e:=original.native.recheckSnapshot(requested);e!=nil{return nil,e}
 }
 held:=make([]byte,len(raw));copy(held,raw);snapshot:=&verifiedSnapshot{originals:o,raw:held,charged:parsed.charged,tick:tick,verified:true,parsed:parsed}
 o.lastPhase=parsed.phase;o.latestRaw=held;o.latestDigest=sha256.Sum256(held);return snapshot,nil
}
func(o *originalObjects)recheckSnapshot(snapshot *verifiedSnapshot)error{
 if o==nil{return ErrInvalid};o.mu.Lock();defer o.mu.Unlock();return o.recheckSnapshotLocked(snapshot)
}
// The caller retains o.mu through every decision that consumes the snapshot.
// Another original observation must not replace rows after their recheck but
// before close-intent eligibility is computed from those same native objects.
func(o *originalObjects)recheckSnapshotLocked(snapshot *verifiedSnapshot)error{
 if snapshot==nil||snapshot.originals!=o||!snapshot.verified{return ErrInvalid}
 if !o.authenticated||o.terminal||sha256.Sum256(snapshot.raw)!=o.latestDigest||snapshot.parsed.count!=o.count{return ErrPhase}
 for i:=uint32(0);i<o.count;i++{row:=snapshot.parsed.rows[i];if !o.rows[i].matchesClaim(row)||o.rows[i].native==nil{return ErrInvalid};if e:=o.rows[i].native.recheckSnapshot(row);e!=nil{return e}};return nil
}
func(o *originalObjects)verifyIntentReady(snapshot *verifiedSnapshot)error{
 if o==nil{return ErrInvalid};o.mu.Lock();defer o.mu.Unlock()
 if e:=o.recheckSnapshotLocked(snapshot);e!=nil{return e};if snapshot.parsed.phase!=7{return ErrPhase}
 primary:=0;finalBytes:=uint64(0)
 for i:=uint32(0);i<snapshot.parsed.count;i++{row:=snapshot.parsed.rows[i]
  terminal:=o.primaryTerminalBitmap[i/8]&(1<<uint(i%8))!=0
  if terminal!=row.terminalDependency()||row.membership()>4{return ErrClosure}
  // Late control rows remain in the full snapshot, counted and independently
  // native-checked; their lifetimes are distinct from primary-session FREE.
  if row.membership()!=0{continue}
  if row.kind==0&&row.processRole==1{primary++;if row.state!=0||!terminal||row.identity!=o.primary{return ErrClosure}}
  if row.state!=1{
   native:=o.rows[i].native
   if native==nil||!terminal||row.state!=0||!native.primaryTerminalEdge{return ErrClosure}
   if row.kind==6||row.kind==7{if row.charged>512-finalBytes{return ErrClosure};finalBytes+=row.charged}
  }
 };if primary!=1{return ErrClosure};return nil
}
func(o *originalObjects)captureClosure(tick uint64)(*verifiedClosure,error){
 o.mu.Lock();defer o.mu.Unlock();if !o.authenticated||o.terminal{return nil,ErrPhase}
 natural:=false;sessionClosed:=true
 snapshot:=resourceSnapshot{phase:7,count:o.count}
 for i:=uint32(0);i<o.count;i++{original:=&o.rows[i];if original.native==nil{return nil,ErrInvalid};row:=original.currentRow()
  // Carry the complete original descriptor, including real cut/capture/flags
  // and retained accounting. A terminal boolean cannot manufacture those cells.
  snapshot.rows[i]=row
  if row.membership()==0{
   observation,e:=original.native.observeTerminal(o.deadlineOriginal)
   if row.kind==0&&row.processRole==1&&row.identity==o.primary{natural=e==nil&&observation.naturalZero}
   if e!=nil||!observation.closed||row.state!=1||row.observation!=observation.digest{sessionClosed=false}
  }else{
   // Observer/facade/parent/service remain genuine live late obligations. Their
   // current full source descriptors are rechecked rather than forced closed.
   if e:=original.native.recheckSnapshot(row);e!=nil{sessionClosed=false}
  }
  if row.charged>^uint64(0)-snapshot.charged{return nil,ErrCapacity};snapshot.charged+=row.charged
 }
 if o.originalStdout==nil||o.originalStderr==nil||o.originalStdout.verifyEOF()!=nil||o.originalStderr.verifyEOF()!=nil{sessionClosed=false}
 if !o.intentACKObserved{sessionClosed=false}
 raw,e:=encodeSnapshot(snapshot);if e!=nil{return nil,e};return &verifiedClosure{originals:o,raw:raw,tick:tick,naturalZero:natural,sessionClosed:sessionClosed,verified:true},nil
}
func(o *originalObjects)recheckClosure(closure *verifiedClosure)error{
 if closure==nil||closure.originals!=o||!closure.verified{return ErrInvalid};parsed,e:=parseSnapshot(closure.raw);if e!=nil{return e};o.mu.Lock();defer o.mu.Unlock()
 if !o.authenticated||parsed.count!=o.count{return ErrInvalid}
 for i:=uint32(0);i<o.count;i++{row:=parsed.rows[i];original:=&o.rows[i];if !original.matchesClaim(row){return ErrInvalid};if original.native==nil{return ErrInvalid};if e:=original.native.recheckSnapshot(row);e!=nil{return e}}
 if closure.sessionClosed&&(o.originalStdout.verifyEOF()!=nil||o.originalStderr.verifyEOF()!=nil){return ErrClosure};return nil
}
func(o *originalObjects)recheckArchive(archive *verifiedArchive,terminal terminalTransaction)error{
 if archive==nil||archive.originals!=o||!archive.verified||sha256.Sum256(archive.raw)!=archive.digest||o.originalArchive==nil{return ErrInvalid}
 return o.originalArchive.verifyArchiveOriginal(archive,terminal)
}
