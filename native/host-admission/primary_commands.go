//go:build linux || windows

package hostadmission

import (
 "encoding/binary"
 "errors"
 "sync"
)

var errOriginalPeerUnauthenticated=errors.New("unauthenticated")
var errOriginalProfileMismatch=errors.New("profile_mismatch")
var errOriginalLedgerUnavailable=errors.New("ledger_unavailable")

// The service creates this only beside its retained original authenticated
// primary channel, observer binding and ledger. This command decoder is not a
// peer constructor: the native reader must verify the original channel/MAC and
// retain exact source frames before dispatch. No request can install a binding,
// observer, profile, original object table or channel capability here.
type primaryCommands struct{
 exchangeMu sync.Mutex
 exchangeClaimed bool
 ledger *Ledger
 binding *originalBinding
 next uint32
 reservation reservation
 snapshot *verifiedSnapshot
 intent closeIntent
 reserved,closing,failed bool
 failureOriginalCause error
 // A failed custody-retention write is a separate actual effect. Keep the
 // triggering command cause even when the public response is ledger unavailable.
 failureRetentionCause error
 failureRetentionAttempted bool
 failureRetentionSkipped bool
 failureRetentionEligibilityCause error
 // Output failure is independently retained even when dispatch already failed.
 failureTransportCause error
 inputProfileOriginal,ledgerProfileOriginal *Profile
 inputProfileError,ledgerProfileError error
}
// Both the decoder and its transport converge here. The original failure and
// its once-only durable retention attempt belong to one command owner; unwinding
// through the transport cannot start a second transaction or replace that cause.
func(s *primaryCommands)retainFailure(cause error)error{
 if s==nil||cause==nil{return cause}
 if s.failureOriginalCause==nil{s.failureOriginalCause=cause}
 s.failed=true
 if s.failureRetentionSkipped{return errOriginalLedgerUnavailable}
 if s.reserved&&!s.failureRetentionAttempted{
  s.failureRetentionAttempted=true
  s.failureRetentionCause=s.ledger.retainOriginalReservation(s.reservation)
 }
 if s.failureRetentionCause!=nil{return errOriginalLedgerUnavailable}
 if s.ledger!=nil{s.ledger.mu.Lock();lost:=s.ledger.lostOriginalObjects;s.ledger.mu.Unlock();if lost{return errOriginalLedgerUnavailable}}
 return cause
}
// Failure before the complete original profile pair is source-admitted cannot
// authorize a store effect. Keep that genuine eligibility failure beside the
// first cause and consume the same driver without retrying a retained write.
func(s *primaryCommands)retainEarlyFailure(cause error)error{
 if s==nil||cause==nil{return cause}
 if s.failureOriginalCause==nil{s.failureOriginalCause=cause};s.failed=true
 if s.reserved&&!s.failureRetentionAttempted&&!s.failureRetentionSkipped{
  s.failureRetentionSkipped=true;s.failureRetentionEligibilityCause=cause
 }
 return cause
}
func(s *primaryCommands)dispatch(frame *Frame)(response Frame,err error){
 if s==nil||s.ledger==nil||s.binding==nil||s.binding.originals==nil||frame==nil{return response,ErrUnavailable}
 // An absorbing failed command owner has already retained its original
 // causes. Later requests cannot replace them or retry a durable effect.
 if s.failed{return response,ErrPhase}
 defer func(){if err!=nil{err=s.retainFailure(err);response=Frame{}}}()
 if s.closing||!s.binding.authenticated||frame.Length<5||frame.Length>FrameBytes||frame.Sequence()!=s.next||s.next>1025{return response,ErrPhase}
 tick,e:=nativeTick();if e!=nil||tick>=s.binding.deadline{return response,ErrClosure}
 cursor:=Cursor{Raw:frame.Bytes()};operation,e:=cursor.U8();sequence,se:=cursor.U32();if e!=nil||se!=nil||sequence!=s.next{return response,ErrInvalid}
 switch operation{
 case 1:
  if s.reserved||sequence!=0||frame.Length!=5||cursor.End()!=nil{return response,ErrPhase}
  reserved,e:=s.ledger.reserve(s.binding,tick);if e!=nil{return response,e}
  s.reservation=reserved;s.reserved=true
  beginPrimaryResponse(&response,2,sequence)
  response.Body[5]=0;copy(response.Body[6:38],reserved.id[:]);response.Body[38]=reserved.slot
  binary.BigEndian.PutUint64(response.Body[39:47],reserved.generation)
  copy(response.Body[47:79],s.binding.observer[:]);binary.BigEndian.PutUint64(response.Body[79:87],s.binding.deadline);response.Length=87
 case 3:
  if !s.reserved||sequence==0||sequence>1024{return response,ErrPhase}
  id,e:=cursor.B32();phase,pe:=cursor.U8();length,le:=cursor.U32()
  if e!=nil||pe!=nil||le!=nil||id!=s.reservation.id||phase>7||length<6||length>runtimeSnapshotBytes{return response,ErrInvalid}
  raw,e:=cursor.Take(int(length));if e!=nil||cursor.End()!=nil{return response,ErrInvalid}
  // The envelope phase must name the same exact native snapshot phase. Its
  // independently retained original duplicates determine every row's state.
  parsed,e:=parseSnapshot(raw);if e!=nil||parsed.phase!=phase{return response,ErrInvalid}
  snapshot,e:=s.binding.originals.validateSnapshot(raw,tick);if e!=nil{return response,e}
  generation,digest,e:=s.ledger.observe(s.reservation,snapshot);if e!=nil{return response,e}
  s.snapshot=snapshot;beginPrimaryResponse(&response,4,sequence);response.Body[5]=0
  binary.BigEndian.PutUint64(response.Body[6:14],generation);copy(response.Body[14:46],digest[:]);response.Length=46
 case 5:
  if !s.reserved||sequence==0||sequence>1025||frame.Length>256||s.snapshot==nil{return response,ErrPhase}
  intent,e:=s.ledger.acceptIntent(s.reservation,frame.Bytes(),s.snapshot,tick);if e!=nil{return response,e}
  s.intent=intent;s.closing=true;beginPrimaryResponse(&response,6,sequence);response.Body[5]=0
  binary.BigEndian.PutUint64(response.Body[6:14],intent.generation);copy(response.Body[14:46],intent.digest[:]);binary.BigEndian.PutUint64(response.Body[46:54],intent.accepted);response.Length=54
  // Neither this durable intent nor sending its ACK proves primary receipt,
  // natural exit, stream EOF, closed IO or FREE. The independent source owner
  // must correlate the actual ACK/read/exit and terminal/archive obligations.
 default:return response,ErrInvalid
 }
 s.next++;return response,nil
}
func beginPrimaryResponse(frame *Frame,operation byte,sequence uint32){frame.Body[0]=operation;binary.BigEndian.PutUint32(frame.Body[1:5],sequence)}

// Only the authenticated original transport may call this after a valid
// operation/sequence header. Public failures contain the closed enum alone.
// Unknown operations or malformed headers have no synthesized response.
func originalPrimaryFailureResponse(request *Frame,failure error)(response Frame,err error){
 if request==nil||failure==nil||request.Length<5||request.Length>FrameBytes{return response,ErrInvalid}
 operation:=byte(0);switch request.Operation(){case 1:operation=2;case 3:operation=4;case 5:operation=6;default:return response,ErrInvalid}
 sequence:=request.Sequence();if operation==2&&sequence!=0||operation==4&&(sequence==0||sequence>1024)||operation==6&&(sequence==0||sequence>1025){return response,ErrInvalid}
 code:=byte(6)
 switch{case errors.Is(failure,ErrUnavailable):code=1;case errors.Is(failure,errOriginalPeerUnauthenticated):code=2;case errors.Is(failure,errOriginalProfileMismatch):code=3;case errors.Is(failure,ErrCapacity):code=4;case errors.Is(failure,ErrPhase):code=5;case errors.Is(failure,errOriginalLedgerUnavailable):code=7;case errors.Is(failure,ErrClosure):code=8}
 beginPrimaryResponse(&response,operation,request.Sequence());response.Body[5]=1;response.Body[6]=code;response.Length=7;return response,nil
}

// Failures preserve every original resource and its last durable charge. A
// failed request cannot reuse a slot or turn CLOSE_PENDING into successful FREE.
// This transaction has no pathname cleanup, process kill or closure assertion.
func(l *Ledger)retainOriginalReservation(reserved reservation)error{
 if l==nil{return ErrUnavailable};l.mu.Lock();defer l.mu.Unlock()
 if l.lostOriginalObjects{return ErrUnavailable};slot,e:=l.slotLocked(reserved);if e!=nil{return e}
 if slot.State=="RETAINED"||slot.State=="QUARANTINED"{return nil}
 if slot.State!="RESERVED"&&slot.State!="LIVE"&&slot.State!="CLOSE_PENDING"{return ErrPhase}
 slot.State="RETAINED";slot.Generation=stringRef(generationString(l.generation+1))
 next:=clonePayload(l.payload);next.Slots[reserved.slot]=slot
 if e=l.commitLocked(next);e!=nil{l.lostOriginalObjects=true;return e};return nil
}
