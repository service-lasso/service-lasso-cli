//go:build linux || windows

package hostadmission

import "crypto/sha256"

func ledgerRawHash(raw []byte)[32]byte{if raw==nil{return [32]byte{}};return sha256.Sum256(raw)}
func sameOriginalSlice(a,b []byte)bool{
 if len(a)!=len(b)||cap(a)!=cap(b){return false}
 if len(a)==0{return a==nil&&b==nil}
 return &a[0]==&b[0]
}

// Working metadata for the selected single original binding transition. This
// is not a constructor, another resource table, a native observation or an
// admission grant. Its complete backing must be paid by the original owner.
// The native/storage callers must retain this SAME cell across their effects.
type originalPendingState byte
const (
 pendingIdle originalPendingState = iota
 pendingPinned
 pendingEffectsRetained
 pendingPublished
 pendingUnresolved
)
type originalRowPin struct {
 original *nativeObject
 source [32]byte
 descriptor resourceRow
 version uint64
}
type originalPending struct {
 state originalPendingState
 ordinal,revision uint64
 binding *originalBinding
 caller *nativeObject
 deadline *nativeDeadline
 originTick,expires,predecessor uint64
 // The original phase geometry is part of the pin, not a renewable allowance.
 deadlinePhase byte
 phaseStart,phaseEnd uint64
 count uint32
 rows [MaximumReferences]originalRowPin
 terminalBitmap [16]byte
 phase byte
 frontier [32]byte
 beforeRaw,candidateRaw,resultRaw []byte
 ledger *Ledger
 ledgerGeneration uint64
 ledgerSelector byte
 ledgerHash [32]byte
 // Progress is closed: zero unchanged, one THIS exact durable successor.
 // Only the original pair body after both actual readbacks may set one.
 pairProgress byte
 pairGeneration uint64
 pairSelector byte
 pairHash [32]byte
 pairStore *nativeStore
 pairRaw,pairSelectorRaw,pairFrameReadback,pairSelectorReadback []byte
 // These are retained references to already reserved source capture extents;
 // no append allocation or caller supplied equality establishes disposition.
 captureOwner *nativeRetention
 captureFirst,captureCount uint32
 capturesComplete,resultRetained bool
}

// Called only under the SAME original binding's short transition lock. It
// copies current committed descriptors, never request claims. Actual native
// lifetime pin acquisition and precharged capture admission remain required
// before this metadata can be used by a real outside-lock operation.
func(p *originalPending)pinRowsLocked(o *originalObjects)error{
 if p==nil||o==nil||p!=&o.pending||p.state!=pendingPinned||p.binding==nil||p.binding.originals!=o||
    o.transitionUnresolved.Load()||p.ordinal==0||p.ordinal!=o.nextOrdinal||p.revision!=o.revision||
    !o.authenticated||o.terminal||o.count==0||o.count>MaximumReferences{return ErrUnavailable}
 p.count=o.count;p.phase=o.lastPhase;p.frontier=o.latestDigest
 p.beforeRaw=o.latestRaw;p.terminalBitmap=o.primaryTerminalBitmap
 for i:=uint32(0);i<o.count;i++{
  original:=&o.rows[i];row:=original.currentRow()
  if original.native==nil||original.version==0||original.source==([32]byte{})||
     validateResourceRow(row,i,o.count)!=nil{return ErrInvalid}
  p.rows[i]=originalRowPin{original:original.native,source:original.source,
    descriptor:row,version:original.version}
 }
 return nil
}

// No native clock/IO/identity callback runs in either predicate. The caller
// must first retain its fresh SAME-deadline observation in the precharged
// native capture range, then acquire the finite original transition once.
func(p *originalPending)rowsUnchangedLocked(o *originalObjects)bool{
 if p==nil||o==nil||p!=&o.pending||p.binding==nil||p.binding.originals!=o||
    o.transitionUnresolved.Load()||p.ordinal==0||p.ordinal!=o.nextOrdinal||p.revision!=o.revision||
    (p.state!=pendingPinned&&p.state!=pendingEffectsRetained)||
    !o.authenticated||o.terminal||o.count!=p.count||o.lastPhase!=p.phase||
    o.latestDigest!=p.frontier||!sameOriginalSlice(o.latestRaw,p.beforeRaw)||
    o.primaryTerminalBitmap!=p.terminalBitmap{return false}
 for i:=uint32(0);i<p.count;i++{
  original:=&o.rows[i];pin:=&p.rows[i]
  if original.native!=pin.original||original.source!=pin.source||
     original.version!=pin.version||original.currentRow()!=pin.descriptor{return false}
 }
 return true
}
// Deadline metadata is inspected under its own short metadata lock by the
// caller. This predicate never samples a new clock or renews the original.
// A pointer/tuple match here remains subordinate to actual caller admission,
// native lifetime pins and independently retained complete clock captures.
func(p *originalPending)contextUnchangedLocked(o *originalObjects)bool{
 if p==nil||o==nil||p!=&o.pending||o.transitionUnresolved.Load()||
    p.binding==nil||p.binding!=o.bindingOriginal||!p.binding.authenticated||
    p.binding.originals!=o||p.caller==nil||p.caller!=o.callerOriginal||
    p.deadline==nil||p.deadline!=o.deadlineOriginal||p.deadline.failed||
    p.deadline.birth!=p.originTick||p.deadline.absolute!=p.expires||
    p.deadline.phase!=p.deadlinePhase||p.deadline.phaseStart!=p.phaseStart||
    p.deadline.phaseEnd!=p.phaseEnd||p.deadline.previous!=p.predecessor||
    p.expires!=p.binding.deadline||p.originTick==0||p.predecessor<p.originTick||
    p.phaseStart<p.originTick||p.phaseStart>p.predecessor||
    p.phaseEnd>p.expires||p.phaseEnd<=p.phaseStart||
    p.predecessor>=p.phaseEnd||p.predecessor>=p.expires{return false}
 return (p.state==pendingPinned||p.state==pendingEffectsRetained)&&
   p.ordinal!=0&&p.ordinal==o.nextOrdinal&&p.revision==o.revision
}
func(p *originalPending)ledgerUnchangedLocked(l *Ledger)bool{
 if p==nil||l==nil||p.ledger!=l||l.lostOriginalObjects||p.pairProgress>1{return false}
 if p.pairProgress==0{
  return l.generation==p.ledgerGeneration&&l.slot==p.ledgerSelector&&
    ledgerRawHash(l.raw)==p.ledgerHash
 }
 if p.ledgerGeneration==^uint64(0)||p.pairGeneration!=p.ledgerGeneration+1||
    p.pairSelector!=p.ledgerSelector^1||p.pairStore!=l.store||
    len(p.pairRaw)==0||len(p.pairFrameReadback)==0||len(p.pairSelectorRaw)!=74||
    !bytesEqual(p.pairSelectorRaw,p.pairSelectorReadback){return false}
 raw,e:=readLedgerFrame(p.pairFrameReadback,l.key)
 if e!=nil||!bytesEqual(raw,p.pairRaw)||ledgerRawHash(raw)!=p.pairHash{return false}
 slot,generation,hash,e:=readSelector(p.pairSelectorReadback,l.key)
 return e==nil&&slot==p.pairSelector&&generation==p.pairGeneration&&hash==p.pairHash&&
   l.generation==p.pairGeneration&&l.slot==p.pairSelector&&ledgerRawHash(l.raw)==p.pairHash
}
