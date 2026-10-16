//go:build linux || windows

package hostadmission

import "crypto/sha256"

// Actual-method source vectors. The separate native harness supplies genuine
// originals and establishes the real post-readback failure/deadline timing.
// No callback, status flag or fabricated pair grants a terminal disposition.
func originalTerminalPairLateFailureVector(l *Ledger,r reservation,intent closeIntent,closure *verifiedClosure)error{
 if l==nil||r.binding==nil||r.binding.originals==nil||r.slot>=MaximumBindings{return ErrInvalid}
 returned,e:=l.finalize(r,intent,closure)
 if e==nil{return ErrInvalid}
 l.mu.Lock();defer l.mu.Unlock()
 owner:=l.terminalOriginals[r.slot]
 if owner==nil||owner!=&r.binding.originals.terminalCapture||!owner.returned||
    owner.resultError==nil||!l.lostOriginalObjects||owner.closure!=closure{return ErrInvalid}
 pair:=&owner.closurePair
 if owner.readyPair.started{pair=&owner.readyPair}
 if !pair.started||!pair.complete||pair.generation!=l.generation||pair.slot!=l.slot||
    !sameOriginalSlice(pair.raw,l.raw)||sha256.Sum256(pair.raw)!=sha256.Sum256(l.raw)||
    !bytesEqual(pair.selection,pair.selectorReadback)||len(pair.generationReadback)==0||
    owner.transaction.reservation.id!=returned.reservation.id||
    owner.transaction.generation!=returned.generation{return ErrInvalid}
 // Terminal failure retains the actual completed successor; it grants no new
 // slot operation, even when its committed disposition was already FREE.
 if _,slotError:=l.slotLocked(r);slotError==nil{return ErrInvalid}
 return nil
}

func originalACKLateReuseFailureVector(l *Ledger,t terminalTransaction,ack []byte,archive *verifiedArchive)error{
 if l==nil||t.reservation.binding==nil||t.reservation.slot>=MaximumBindings{return ErrInvalid}
 e:=l.archiveACK(t,ack,archive)
 if e==nil{return ErrInvalid}
 l.mu.Lock();defer l.mu.Unlock()
 owner:=l.terminalOriginals[t.reservation.slot]
 if owner==nil||!owner.ackReturned||owner.ackError==nil||owner.reuseClockError==nil||
    !owner.ackPair.complete||!owner.reusePair.complete||!l.lostOriginalObjects||
    owner.reuseGeneration!=l.generation||owner.reusePair.generation!=l.generation||
    !sameOriginalSlice(owner.reuseRaw,l.raw)||!sameOriginalSlice(owner.reusePair.raw,l.raw)||
    !sameOriginalSlice(owner.ackOriginal,ack)||owner.ackHash!=sha256.Sum256(ack)||
    owner.archiveOriginal!=archive||!owner.reuseDeadline.finished{return ErrInvalid}
 if l.payload.TerminalCells[t.reservation.slot].ReservationID!=nil{return ErrInvalid}
 if _,slotError:=l.slotLocked(t.reservation);slotError==nil{return ErrInvalid}
 return nil
}
