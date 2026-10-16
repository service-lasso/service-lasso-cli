//go:build linux || windows

package hostadmission

import (
 "crypto/rand"
 "crypto/sha256"
 "encoding/json"
 "errors"
 "math"
 "strconv"
)

var ErrCapacity = errors.New("capacity_exhausted")
var ErrPhase = errors.New("invalid_phase")
var ErrClosure = errors.New("closure_unresolved")

// A binding is created only by the authenticated production launcher while it
// retains the original primary, observer and stream objects. Requests cannot
// construct one from a PID, a digest or an endpoint spelling.
type originalBinding struct {
 owner string
 primary, observer, run, epoch [32]byte
 deadline uint64
 authenticated bool
 provisional bool
 originals *originalObjects
}
type reservation struct { id [32]byte; slot uint8; generation uint64; binding *originalBinding }
type closeIntent struct { generation, accepted, deadline uint64; digest [32]byte }
type terminalTransaction struct {
 reservation reservation
 generation, completed uint64
 intent, closure, ledger, receipt [32]byte
 body []byte
 closureLedgerRaw []byte
 disposition byte
}
const closureStoreReservation uint64 = 3*(runtimeSnapshotBytes)+512

func clonePayload(payload LedgerPayload)LedgerPayload{out:=payload;out.Observations=append([]LedgerObservation{},payload.Observations...);return out}
func encodePayload(payload LedgerPayload)([]byte,error){
 raw,e:=json.Marshal(payload);if e!=nil||len(raw)+1>LedgerPayloadCap{return nil,ErrInvalid}
 return append(raw,'\n'),nil
}
func (l *Ledger) reserveStorage(n uint64)error{
 if l.lostOriginalObjects||n>StoreBytes-l.usedStoreBytes||l.reservedStoreBytes>StoreBytes-l.usedStoreBytes-n{return ErrCapacity}
 l.reservedStoreBytes+=n;return nil
}

// The selector is the sole generation authority. Neither file mtime nor a
// larger unselected generation can repair an invalid selected generation.
func OpenLedger(profile *Profile,host,epoch [32]byte)(resultLedger *Ledger,resultError error){
 var profileError error;profile,profileError=originalProfile(profile);if profileError!=nil{return nil,profileError}
 if profile==nil||host==([32]byte{})||epoch==([32]byte{}){return nil,ErrUnavailable}
 store,e:=OpenStore(profile);if e!=nil{return nil,e}
 complete:=false;defer func(){if !complete{store.uncertain=true;retainStore(store)}}()
 original,e:=store.snapshotOriginal();if e!=nil{return nil,e}
 owner,e:=store.claimInitialization(profile,host,epoch,false,original);if e!=nil{return nil,e}
 defer func(){owner.resultError=resultError;owner.returned=true}()
 l:=&Ledger{profile:profile,store:store,key:original.key}
 owner.ledger=l
 selected,selectorNative,selectorOperation,e:=store.readCapturedOriginal("selector",74);owner.selected=selected;owner.selectorNative=selectorNative;owner.selectorOperation=selectorOperation;owner.selectorReadError=e;if e!=nil{return nil,ErrUnavailable}
 slot,generation,digest,e:=readSelector(selected,l.key);owner.selectorError=e;if e!=nil{return nil,e}
 name:="generation-a";if slot==1{name="generation-b"}
 frame,generationNative,generationOperation,e:=store.readCapturedOriginal(name,LedgerPayloadCap+36);owner.frame=frame;owner.generationNative=generationNative;owner.generationOperation=generationOperation;owner.generationReadError=e;if e!=nil{return nil,e}
 raw,e:=readLedgerFrame(frame,l.key);owner.raw=raw;owner.frameError=e;if e!=nil||sha256.Sum256(raw)!=digest{return nil,ErrInvalid}
 payload,e:=validateLedgerPayload(raw,profile,digestString(host),digestString(epoch));owner.payloadError=e;if e!=nil{return nil,e}
 parsed,e:=strconv.ParseUint(payload.Generation,10,64);owner.generationError=e;if e!=nil||parsed!=generation{return nil,ErrInvalid}
 l.payload=payload;l.raw=raw;l.slot=slot;l.generation=generation
 size,e:=store.TotalBytes();owner.inventory=size;owner.inventoryError=e;if e!=nil||size>StoreBytes{return nil,ErrUnavailable};l.usedStoreBytes=size
 // Original kernel objects are never reconstructed at service restart. Every
 // used record is persisted quarantined before a launch can be considered.
 changed:=false
 if l.payload.HostEpoch!=digestString(epoch){l.payload.HostEpoch=digestString(epoch);changed=true}
 for i:=range l.payload.Slots{if l.payload.Slots[i].State!="FREE"{l.payload.Slots[i].State="QUARANTINED";changed=true}}
 if changed{e=l.commitRetainedLocked(l.payload,&owner.commitPair);owner.commitError=e;if e!=nil{return nil,e}}
 complete=true;return l,nil
}

// Owner initialization is invoked only from the separately source-admitted
// install/recovery command. It requires an exclusively held, verified empty
// native store; ordinary launch/admission requests have no initialization path.
func initializeLedger(profile *Profile,store *nativeStore,host,epoch [32]byte)(resultLedger *Ledger,resultError error){
 var profileError error;profile,profileError=originalProfile(profile);if profileError!=nil{return nil,profileError}
 if profile==nil||store==nil||host==([32]byte{})||epoch==([32]byte{}){return nil,ErrUnavailable}
 original,e:=store.snapshotOriginal();if e!=nil{return nil,e}
 if !original.initializationAuthorized||original.identity==([32]byte{}){return nil,ErrUnavailable}
 owner,e:=store.claimInitialization(profile,host,epoch,true,original);if e!=nil{return nil,e}
 defer func(){owner.resultError=resultError;owner.returned=true}()
 size,e:=store.TotalBytes();owner.inventory=size;owner.inventoryError=e;if e!=nil||size!=0{return nil,ErrInvalid}
 payload:=LedgerPayload{Schema:LedgerSchema,HostIdentity:digestString(host),HostEpoch:digestString(epoch),ProfileSHA256:digestString(profile.Digest),KeyVersion:profile.KeyVersion,Generation:"1",Observations:[]LedgerObservation{}}
 for i:=0;i<MaximumBindings;i++{payload.Slots[i]=nullSlot(uint8(i));payload.TerminalCells[i]=nullCell(uint8(i))}
 l:=&Ledger{profile:profile,store:store,key:original.key};owner.ledger=l
 e=l.commitRetainedLocked(payload,&owner.commitPair);owner.commitError=e;if e!=nil{return nil,e};return l,nil
}

func (l *Ledger) commitLocked(next LedgerPayload)(result error){
 if l.lostOriginalObjects||l.generation==math.MaxUint64||l.pairOrdinal==math.MaxUint64{return ErrUnavailable}
 generation:=l.generation+1;next.Generation=generationString(generation)
 if l.raw!=nil{next.PreviousPayloadSHA256=stringRef(digestString(sha256.Sum256(l.raw)))}
 raw,e:=encodePayload(next);if e!=nil{return e}
 host,epoch:=l.payload.HostIdentity,l.payload.HostEpoch;if l.generation==0{host,epoch=next.HostIdentity,next.HostEpoch};if _,e=validateLedgerPayload(raw,l.profile,host,epoch);e!=nil{return e}
 frame,e:=ledgerFrame(raw,l.key);if e!=nil{return e}
 // Both original generation files, current/new selector and each physical
 // transaction copy count against the finite native-store limit.
 if e=l.reserveStorage(uint64(2*len(frame)+148));e!=nil{return e}
 inactive:=byte(1-l.slot);name:="generation-a";if inactive==1{name="generation-b"}
 digest:=sha256.Sum256(raw);selection,e:=selector(inactive,generation,digest,l.key);if e!=nil{return e}
 // Reserve original candidate and output owners before the first store
 // effect. No later diagnostic failure can drop the returned readback slice
 // or substitute the previous ledger for a completed durable successor.
 l.pairOrdinal++
 l.pair=originalLedgerPair{started:true,baseGeneration:l.generation,
  generation:generation,baseSlot:l.slot,slot:inactive,baseRaw:l.raw,
  raw:raw,frame:frame,selection:selection[:],payload:next}
 pair:=&l.pair
 pair.storeOriginal=l.store
 store:=pair.storeOriginal
 token,e:=store.beginOriginalOperation(5);pair.storeClaimError=e
 if e!=nil{l.lostOriginalObjects=true;return e}
 defer func(){
  pair.storeOperation=store.finishOriginalOperation(token,&result)
  pair.storePublicationError=pair.storeOperation.publicationError
  if result!=nil{l.lostOriginalObjects=true}
 }()
 readOrdinal:=store.readOrdinal
 e=store.replaceOriginalOwned(token,name,frame);pair.generationWriteError=e;pair.generationReplaceRead=store.readOriginalSince(readOrdinal)
 if e!=nil{l.lostOriginalObjects=true;return e}
 readOrdinal=store.readOrdinal
 e=store.replaceOriginalOwned(token,"selector",pair.selection);pair.selectorWriteError=e;pair.selectorReplaceRead=store.readOriginalSince(readOrdinal)
 if e!=nil{l.lostOriginalObjects=true;return e}
 readOrdinal=store.readOrdinal
 actual,e:=store.readOriginalOwned(token,"selector",74);pair.selectorReadback=actual;pair.selectorReadError=e;pair.selectorReadOriginal=store.readOriginalSince(readOrdinal)
 if e!=nil||!bytesEqual(actual,pair.selection){l.lostOriginalObjects=true;return ErrUnavailable}
 readOrdinal=store.readOrdinal
 selected,e:=store.readOriginalOwned(token,name,LedgerPayloadCap+36);pair.generationReadback=selected;pair.generationReadError=e;pair.generationReadOriginal=store.readOriginalSince(readOrdinal)
 if e!=nil||!bytesEqual(selected,frame){l.lostOriginalObjects=true;return ErrUnavailable}
 // Both actual readbacks are complete. Inventory failure below denies the
 // operation but retains this exact committed frontier and all pair owners.
 pair.complete=true
 l.payload=next;l.raw=raw;l.slot=inactive;l.generation=generation
 total,e:=store.totalBytesOriginalOwned(token);pair.inventory=total;pair.inventoryError=e
 if e!=nil||total>StoreBytes{l.lostOriginalObjects=true;return ErrUnavailable}
 l.usedStoreBytes=total
 l.reservedStoreBytes-=uint64(2*len(frame)+148);return nil
}
// Retain only a candidate actually started by THIS commit invocation. An early
// validation failure cannot relabel a previous successful/partial pair.
func(l *Ledger)commitRetainedLocked(next LedgerPayload,original *originalLedgerPair)error{
 ordinal:=l.pairOrdinal
 e:=l.commitLocked(next)
 if original!=nil&&l.pairOrdinal!=ordinal{*original=l.pair}
 return e
}

func (l *Ledger) reserve(binding *originalBinding,tick uint64)(reservation,error){
 var out reservation
 if binding==nil||!binding.authenticated||!binding.provisional||binding.originals==nil||tick>=binding.deadline{return out,ErrUnavailable}
 if e:=binding.originals.verifyPrebound(binding);e!=nil{return out,e}
 l.mu.Lock();defer l.mu.Unlock()
 if l.lostOriginalObjects||l.payload.HostEpoch!=digestString(binding.epoch){return out,ErrUnavailable}
 used:=0;user:=0;free:=-1
 for i,slot:=range l.payload.Slots{
  if slot.State!="FREE"{used++;if slot.OwnerNativeID!=nil&&*slot.OwnerNativeID==binding.owner{user++}}
  if slot.State=="FREE"&&l.payload.TerminalCells[i].ReservationID==nil&&free<0{free=i}
 }
 if used>=MaximumBindings||user>=4||free<0{return out,ErrCapacity}
 // This capacity remains reserved for the entire accepted reservation. A
 // closure transaction/receipt cannot depend on later discretionary space.
 // Generation/selector transaction space is reserved once per serialized
 // commit below. Per-binding space covers its original observation, closure,
 // replacement scratch and exact finite terminal receipt without multiplying
 // the complete global generation reserve sixteen times.
 if e:=l.reserveStorage(closureStoreReservation);e!=nil{return out,e}
 if _,e:=rand.Read(out.id[:]);e!=nil{return out,e};out.slot=uint8(free);out.binding=binding
 next:=clonePayload(l.payload);generation:=generationString(l.generation+1)
 slot:=Slot{Schema:SlotSchema,SlotID:out.slot,State:"RESERVED",ReservationID:stringRef(digestString(out.id)),Generation:stringRef(generation),HostEpoch:stringRef(digestString(binding.epoch)),ServiceProfileSHA256:stringRef(digestString(l.profile.Digest)),OwnerNativeID:stringRef(binding.owner),PrimaryIdentityDigest:stringRef(digestString(binding.primary)),CreatedSequence:stringRef(generation),ResourceDigest:stringRef(digestString(binding.originals.snapshotDigest(out.id))),MaximumBytes:uintRef(SessionBytes),KeyVersion:stringRef(l.profile.KeyVersion),ObserverBindingDigest:stringRef(digestString(binding.observer)),AbsoluteDeadlineTick:stringRef(generationString(binding.deadline))}
 next.Slots[free]=slot
 if e:=l.commitLocked(next);e!=nil{return reservation{},e};out.generation=l.generation;binding.provisional=false;return out,nil
}

func (l *Ledger) slotLocked(r reservation)(Slot,error){
 // Native custody loss is sticky before any new observation/closure write.
 // commitLocked alone is too late to protect their original store objects.
 if l==nil||l.lostOriginalObjects{return Slot{},ErrUnavailable}
 if r.binding==nil||r.slot>=MaximumBindings{return Slot{},ErrInvalid};slot:=l.payload.Slots[r.slot]
 if slot.ReservationID==nil||*slot.ReservationID!=digestString(r.id)||slot.PrimaryIdentityDigest==nil||*slot.PrimaryIdentityDigest!=digestString(r.binding.primary){return Slot{},ErrInvalid};return slot,nil
}

func (l *Ledger) observe(r reservation,snapshot *verifiedSnapshot)(uint64,[32]byte,error){
 var digest [32]byte
 // Potentially blocking native identity/IO work precedes the ledger mutex.
 if r.binding==nil||snapshot==nil||snapshot.originals!=r.binding.originals||!snapshot.verified{return 0,digest,ErrInvalid}
 if e:=r.binding.originals.recheckSnapshot(snapshot);e!=nil{return 0,digest,e}
 digest=DomainHash("SLCLI-HOST-SNAPSHOT-1\x00",r.id[:],snapshot.raw)
 l.mu.Lock();defer l.mu.Unlock();slot,e:=l.slotLocked(r);if e!=nil{return 0,digest,e}
 if slot.State!="RESERVED"&&slot.State!="LIVE"{return 0,digest,ErrPhase}
 if snapshot.tick>=r.binding.deadline{return 0,digest,ErrClosure}
 name:="observation-"+strconv.Itoa(int(r.slot))
 if e=l.store.Replace(name,snapshot.raw);e!=nil{l.lostOriginalObjects=true;return 0,digest,e}
 slot.State="LIVE";slot.RetainedBytes=snapshot.charged;slot.ResourceDigest=stringRef(digestString(digest));slot.Generation=stringRef(generationString(l.generation+1))
 next:=clonePayload(l.payload);next.Slots[r.slot]=slot
 found:=false;for i:=range next.Observations{if next.Observations[i].SlotID==r.slot{next.Observations[i]=LedgerObservation{r.slot,digestString(sha256.Sum256(snapshot.raw)),uint64(len(snapshot.raw))};found=true}}
 if !found{next.Observations=append(next.Observations,LedgerObservation{r.slot,digestString(sha256.Sum256(snapshot.raw)),uint64(len(snapshot.raw))});sortObservations(next.Observations)}
 if e=l.commitLocked(next);e!=nil{return 0,digest,e};return l.generation,digest,nil
}
func sortObservations(rows []LedgerObservation){for i:=1;i<len(rows);i++{for j:=i;j>0&&rows[j].SlotID<rows[j-1].SlotID;j--{rows[j],rows[j-1]=rows[j-1],rows[j]}}}

func (l *Ledger) acceptIntent(r reservation,body []byte,snapshot *verifiedSnapshot,tick uint64)(closeIntent,error){
 var out closeIntent
 if r.binding==nil||snapshot==nil||!snapshot.verified||snapshot.originals!=r.binding.originals||tick>=r.binding.deadline{return out,ErrClosure}
 if e:=r.binding.originals.verifyIntentReady(snapshot);e!=nil{return out,e}
 c:=Cursor{Raw:body};op,e:=c.U8();seq,se:=c.U32();id,ie:=c.B32();final,fe:=c.B32();observer,oe:=c.B32();expected,ee:=c.U8();deadline,de:=c.U64()
 if e!=nil||se!=nil||ie!=nil||fe!=nil||oe!=nil||ee!=nil||de!=nil||op!=5||seq<1||seq>1025||id!=r.id||observer!=r.binding.observer||expected!=0||deadline!=r.binding.deadline||c.End()!=nil{return out,ErrInvalid}
 observed:=DomainHash("SLCLI-HOST-SNAPSHOT-1\x00",r.id[:],snapshot.raw);if final!=observed{return out,ErrInvalid}
 l.mu.Lock();defer l.mu.Unlock();slot,e:=l.slotLocked(r);if e!=nil{return out,e};if slot.State!="LIVE"||slot.ResourceDigest==nil||*slot.ResourceDigest!=digestString(final){return out,ErrPhase}
 out.digest=DomainHash("SLCLI-HOST-CLOSE-INTENT-2\x00",r.id[:],r.binding.primary[:],r.binding.observer[:],body);out.accepted=tick;out.deadline=deadline;out.generation=l.generation+1
 slot.State="CLOSE_PENDING";slot.CloseIntentDigest=stringRef(digestString(out.digest));slot.CloseIntentGeneration=stringRef(generationString(out.generation));slot.CloseIntentTick=stringRef(generationString(tick));slot.Generation=stringRef(generationString(out.generation))
 next:=clonePayload(l.payload);next.Slots[r.slot]=slot;if e=l.commitLocked(next);e!=nil{return closeIntent{},e};return out,nil
}

func (l *Ledger) finalize(r reservation,intent closeIntent,closure *verifiedClosure)(out terminalTransaction,result error){
 if l==nil||r.binding==nil||r.binding.originals==nil||closure==nil||closure.originals!=r.binding.originals||!closure.verified{return out,ErrClosure}
 // Original process wait/EOF/IO checks run outside the admission lock.
 if e:=r.binding.originals.recheckClosure(closure);e!=nil{return out,e}
 l.mu.Lock();defer l.mu.Unlock();slot,e:=l.slotLocked(r);if e!=nil{return out,e}
 owner:=&r.binding.originals.terminalCapture
 r.binding.originals.mu.Lock()
 if owner.started{r.binding.originals.mu.Unlock();return out,ErrPhase}
 *owner=originalTerminalCapture{started:true,closure:closure,ledgerOriginal:l}
 r.binding.originals.mu.Unlock()
 l.terminalOriginals[r.slot]=owner
 defer func(){owner.transaction=out;owner.resultError=result;owner.returned=true}()
 // A retained terminal cell owns its original receipt/archive obligation.
 // Repeated finalization cannot overwrite that custody before exact ACK.
 if l.payload.TerminalCells[r.slot].ReservationID!=nil{return out,ErrPhase}
 success:=slot.State=="CLOSE_PENDING"&&closure.naturalZero&&closure.sessionClosed&&closure.tick<=r.binding.deadline&&intent.digest!=([32]byte{})&&slot.CloseIntentDigest!=nil&&*slot.CloseIntentDigest==digestString(intent.digest)
 disposition:="RETAINED";out.disposition=1;if success{disposition="FREE";out.disposition=0}
 if slot.State=="RETAINED"||slot.State=="QUARANTINED"{success=false;disposition=slot.State;if disposition=="QUARANTINED"{out.disposition=2}}
 out.reservation=r;out.intent=intent.digest;out.completed=closure.tick;out.closure=sha256.Sum256(closure.raw);out.generation=l.generation+1
 owner.transaction=out
 closureName:="closure-"+strconv.Itoa(int(r.slot));e=l.store.Replace(closureName,closure.raw);owner.closureWriteError=e;if e!=nil{l.lostOriginalObjects=true;return out,e}
 next:=clonePayload(l.payload)
 if success{next.Slots[r.slot]=nullSlot(r.slot);remaining:=make([]LedgerObservation,0,len(next.Observations));for _,observation:=range next.Observations{if observation.SlotID!=r.slot{remaining=append(remaining,observation)}};next.Observations=remaining}else{slot.State=disposition;slot.ClosureEvidenceDigest=stringRef(digestString(out.closure));slot.Generation=stringRef(generationString(out.generation));next.Slots[r.slot]=slot}
 var intentRef *string;if intent.digest!=([32]byte{}){intentRef=stringRef(digestString(intent.digest))}
 next.TerminalCells[r.slot]=TerminalCell{SlotID:r.slot,ReservationID:stringRef(digestString(r.id)),IntentDigest:intentRef,ClosureEvidenceDigest:stringRef(digestString(out.closure)),TerminalGeneration:stringRef(generationString(out.generation)),Disposition:stringRef(disposition)}
 e=l.commitRetainedLocked(next,&owner.closurePair);owner.closureCommitError=e;if e!=nil{return out,e}
 // Hash the committed closure generation BEFORE receipt construction. The
 // subsequent receipt-ready generation cannot contain/hash its own future.
 out.ledger=sha256.Sum256(l.raw)
 out.closureLedgerRaw=append([]byte{},l.raw...)
 out.body=terminalBody(out)
 owner.transaction=out
 e=validateObserverReceiptBody(out.body);owner.bodyError=e;if e!=nil{l.lostOriginalObjects=true;return out,e}
 out.receipt=DomainHash("SLCLI-HOST-TERMINAL-2\x00",out.body)
 owner.transaction=out
 receiptName:="receipt-"+strconv.Itoa(int(r.slot));e=l.store.Replace(receiptName,out.body);owner.receiptWriteError=e;if e!=nil{l.lostOriginalObjects=true;return out,e}
 ready:=clonePayload(l.payload);cell:=ready.TerminalCells[r.slot];cell.ClosureLedgerRef=stringRef(digestString(out.ledger));cell.TerminalReceiptDigest=stringRef(digestString(out.receipt));cell.TerminalReceiptRef=stringRef(digestString(sha256.Sum256(out.body)));ready.TerminalCells[r.slot]=cell
 e=l.commitRetainedLocked(ready,&owner.readyPair);owner.readyCommitError=e;if e!=nil{return out,e};return out,nil
}
func terminalBody(t terminalTransaction)[]byte{
 out:=make([]byte,0,223);out=append(out,9,0,0,0,1);out=append(out,t.reservation.id[:]...);out=append(out,t.reservation.slot);out=append(out,t.reservation.binding.observer[:]...);out=append(out,t.intent[:]...);gen:=U64(t.generation);out=append(out,gen[:]...);out=append(out,t.closure[:]...);out=append(out,t.ledger[:]...);tick:=U64(t.completed);out=append(out,tick[:]...);out=append(out,t.disposition);return out
}

func (l *Ledger) archiveACK(t terminalTransaction,ack []byte,archive *verifiedArchive)(result error){
 if l==nil||t.reservation.binding==nil||t.reservation.binding.originals==nil||t.reservation.slot>=MaximumBindings||archive==nil||!archive.verified||archive.originals!=t.reservation.binding.originals{return ErrClosure}
 originals:=t.reservation.binding.originals
 l.mu.Lock()
 owner:=l.terminalOriginals[t.reservation.slot]
 if owner==nil||owner!=&originals.terminalCapture||owner.ledgerOriginal!=l||!owner.returned||owner.resultError!=nil||owner.ackStarted||
    !sameTerminalOriginal(t,owner.transaction){l.mu.Unlock();return ErrPhase}
 owner.ackStarted=true;owner.ackOriginal=ack;owner.ackHash=sha256.Sum256(ack);owner.archiveOriginal=archive
 l.mu.Unlock()
 defer func(){
  if owner.ackDeadline.started&&!owner.ackDeadline.finished{owner.ackClockError=owner.ackDeadline.finish()}
  if owner.reuseDeadline.started&&!owner.reuseDeadline.finished{owner.reuseClockError=owner.reuseDeadline.finish()}
  owner.ackError=result;owner.ackReturned=true
 }()
 _,e:=owner.ackDeadline.begin(originals.deadlineOriginal);owner.ackClockError=e;if e!=nil{return e}
 if owner.ackDeadline.absolute!=t.reservation.binding.deadline{return ErrClosure}
 e=archive.originals.recheckArchive(archive,t);owner.archiveError=e;if e!=nil{owner.ackClockError=owner.ackDeadline.finish();return e}
 // Both terminal delivery and original archive ACK belong to the original
 // positive deadline. Late archival preserves custody but never grants reuse.
 decoded,e:=parseObserverTerminalACK(ack)
 owner.decodeError=e
 if e!=nil||decoded.reservation!=t.reservation.id||decoded.generation!=t.generation||decoded.receipt!=t.receipt||decoded.archive!=archive.digest{return ErrInvalid}
 id,receipt:=decoded.reservation,decoded.receipt
 l.mu.Lock();defer l.mu.Unlock();cell:=l.payload.TerminalCells[t.reservation.slot]
 if l.terminalOriginals[t.reservation.slot]!=owner||!sameTerminalOriginal(t,owner.transaction)||
    sha256.Sum256(owner.ackOriginal)!=owner.ackHash||!sameOriginalSlice(ack,owner.ackOriginal){return ErrClosure}
 if cell.ReservationID==nil||*cell.ReservationID!=digestString(id)||cell.TerminalReceiptDigest==nil||*cell.TerminalReceiptDigest!=digestString(receipt)||cell.TerminalGeneration==nil||*cell.TerminalGeneration!=generationString(t.generation)||cell.ClosureEvidenceDigest==nil||*cell.ClosureEvidenceDigest!=digestString(t.closure)||cell.ClosureLedgerRef==nil||*cell.ClosureLedgerRef!=digestString(t.ledger)||cell.TerminalReceiptRef==nil||*cell.TerminalReceiptRef!=digestString(sha256.Sum256(t.body))||cell.ACKDigest!=nil{return ErrPhase}
 // The original committed disposition owns both reuse and store retirement.
 expectedDisposition:="FREE";switch t.disposition{case 0:case 1:expectedDisposition="RETAINED";case 2:expectedDisposition="QUARANTINED";default:return ErrInvalid}
 if cell.Disposition==nil||*cell.Disposition!=expectedDisposition{return ErrPhase}
 if t.intent==[32]byte{}{if cell.IntentDigest!=nil{return ErrPhase}}else if cell.IntentDigest==nil||*cell.IntentDigest!=digestString(t.intent){return ErrPhase}
 next:=clonePayload(l.payload);cell.ACKDigest=stringRef(digestString(sha256.Sum256(ack)));next.TerminalCells[t.reservation.slot]=cell
 e=l.commitRetainedLocked(next,&owner.ackPair);owner.ackCommitError=e;owner.ackGeneration=l.generation;owner.ackRaw=l.raw
 owner.ackClockError=owner.ackDeadline.finish()
 if e!=nil{return e};if owner.ackClockError!=nil{l.lostOriginalObjects=true;return owner.ackClockError}
 if sha256.Sum256(owner.ackOriginal)!=owner.ackHash||!sameTerminalOriginal(t,owner.transaction){l.lostOriginalObjects=true;return ErrClosure}
 // Reuse follows a second committed generation preserving the actual ACK.
 // The original receipt/closure generation is already archived independently.
 _,e=owner.reuseDeadline.begin(originals.deadlineOriginal);owner.reuseClockError=e;if e!=nil{return e}
 next=clonePayload(l.payload);next.TerminalCells[t.reservation.slot]=nullCell(t.reservation.slot)
 e=l.commitRetainedLocked(next,&owner.reusePair);owner.reuseCommitError=e;owner.reuseGeneration=l.generation;owner.reuseRaw=l.raw
 owner.reuseClockError=owner.reuseDeadline.finish()
 if e!=nil{return e};if owner.reuseClockError!=nil{l.lostOriginalObjects=true;return owner.reuseClockError}
 if sha256.Sum256(owner.ackOriginal)!=owner.ackHash||!sameTerminalOriginal(t,owner.transaction){l.lostOriginalObjects=true;return ErrClosure}
 if t.disposition==0{if l.reservedStoreBytes<closureStoreReservation{l.lostOriginalObjects=true;return ErrInvalid};l.reservedStoreBytes-=closureStoreReservation};return nil
}
func sameTerminalOriginal(a,b terminalTransaction)bool{
 return a.reservation.id==b.reservation.id&&a.reservation.slot==b.reservation.slot&&
  a.reservation.generation==b.reservation.generation&&a.reservation.binding==b.reservation.binding&&
  a.generation==b.generation&&a.completed==b.completed&&a.intent==b.intent&&a.closure==b.closure&&
  a.ledger==b.ledger&&a.receipt==b.receipt&&a.disposition==b.disposition&&
  sameOriginalSlice(a.body,b.body)&&sameOriginalSlice(a.closureLedgerRaw,b.closureLedgerRaw)&&
  sha256.Sum256(a.closureLedgerRaw)==a.ledger&&DomainHash("SLCLI-HOST-TERMINAL-2\x00",a.body)==a.receipt
}
