//go:build linux || windows

package hostadmission

import (
 "crypto/hmac"
 "crypto/sha256"
 "encoding/binary"
 "encoding/hex"
 "encoding/json"
 "strconv"
 "sync"
)

const LedgerPayloadCap = 262144
const LedgerSchema = "service-lasso.cli-host-ledger.v2"
const SlotSchema = "service-lasso.cli-host-slot.v2"

type Slot struct {
 Schema string `json:"schema"`
 SlotID uint8 `json:"slotId"`
 ReservationID *string `json:"reservationId"`
 Generation *string `json:"generation"`
 State string `json:"state"`
 HostEpoch *string `json:"hostEpoch"`
 ServiceProfileSHA256 *string `json:"serviceProfileSha256"`
 OwnerNativeID *string `json:"ownerNativeId"`
 PrimaryIdentityDigest *string `json:"primaryIdentityDigest"`
 CreatedSequence *string `json:"createdSequence"`
 ResourceDigest *string `json:"resourceDigest"`
 MaximumBytes *uint64 `json:"maximumBytes"`
 RetainedBytes uint64 `json:"retainedBytes"`
 ClosureEvidenceDigest *string `json:"closureEvidenceDigest"`
 KeyVersion *string `json:"keyVersion"`
 ObserverBindingDigest *string `json:"observerBindingDigest"`
 CloseIntentDigest *string `json:"closeIntentDigest"`
 CloseIntentGeneration *string `json:"closeIntentGeneration"`
 CloseIntentTick *string `json:"closeIntentTick"`
 AbsoluteDeadlineTick *string `json:"absoluteDeadlineTick"`
}
type LedgerObservation struct { SlotID uint8 `json:"slotId"`; SHA256 string `json:"sha256"`; Size uint64 `json:"size"` }
type TerminalCell struct {
 SlotID uint8 `json:"slotId"`
 ReservationID *string `json:"reservationId"`
 IntentDigest *string `json:"intentDigest"`
 ClosureEvidenceDigest *string `json:"closureEvidenceDigest"`
 TerminalGeneration *string `json:"terminalGeneration"`
 ClosureLedgerRef *string `json:"closureLedgerRef"`
 TerminalReceiptDigest *string `json:"terminalReceiptDigest"`
 TerminalReceiptRef *string `json:"terminalReceiptRef"`
 Disposition *string `json:"disposition"`
 ACKDigest *string `json:"ackDigest"`
}
type LedgerPayload struct {
 Schema string `json:"schema"`
 HostIdentity string `json:"hostIdentity"`
 HostEpoch string `json:"hostEpoch"`
 ProfileSHA256 string `json:"profileSha256"`
 KeyVersion string `json:"keyVersion"`
 Generation string `json:"generation"`
 Slots [MaximumBindings]Slot `json:"slots"`
 Observations []LedgerObservation `json:"observations"`
 TerminalCells [MaximumBindings]TerminalCell `json:"terminalCells"`
 PreviousPayloadSHA256 *string `json:"previousPayloadSha256"`
}
type Ledger struct {
 mu sync.Mutex
 profile *Profile
 store *nativeStore
 key [32]byte
 payload LedgerPayload
 raw []byte
 slot byte
 generation uint64
 usedStoreBytes uint64
 reservedStoreBytes uint64
 lostOriginalObjects bool
 // Embedded original transaction custody. A failed commit leaves this cell
 // occupied, including each actual readback and independent returned failure.
 // It is not native provenance or permission to repair a partial pair.
 pair originalLedgerPair
 pairOrdinal uint64
 // One capture owner per existing terminal cell, never additional binding
 // capacity. Its enlarged backing requires the original source calibration.
 terminalOriginals [MaximumBindings]*originalTerminalCapture
}
type originalTerminalCapture struct {
 ledgerOriginal *Ledger
 started,returned,ackStarted,ackReturned bool
 closure *verifiedClosure
 transaction terminalTransaction
 closureWriteError,closureCommitError,bodyError,receiptWriteError,readyCommitError,resultError error
 ackOriginal []byte
 ackHash [32]byte
 archiveOriginal *verifiedArchive
 ackDeadline,reuseDeadline exitDeadlineCapture
 archiveError,ackClockError,decodeError,ackCommitError,reuseClockError,reuseCommitError,ackError error
 ackGeneration,reuseGeneration uint64
 ackRaw,reuseRaw []byte
 closurePair,readyPair,ackPair,reusePair originalLedgerPair
}
type originalLedgerPair struct {
 started,complete bool
 baseGeneration,generation uint64
 baseSlot,slot byte
 baseRaw,raw,frame,selection,selectorReadback,generationReadback []byte
 payload LedgerPayload
 generationWriteError,selectorWriteError,selectorReadError,generationReadError,inventoryError error
 inventory uint64
 storeOriginal *nativeStore
 storeOperation originalStoreOperation
 storeClaimError,storePublicationError error
 generationReplaceRead,selectorReplaceRead,selectorReadOriginal,generationReadOriginal nativeStoreRead
}
func stringRef(s string)*string{return &s}
func uintRef(n uint64)*uint64{return &n}
func digestString(d [32]byte)string{return hex.EncodeToString(d[:])}
func nullSlot(id uint8)Slot{return Slot{Schema:SlotSchema,SlotID:id,State:"FREE"}}
func nullCell(id uint8)TerminalCell{return TerminalCell{SlotID:id}}
func generationString(n uint64)string{return strconv.FormatUint(n,10)}
func ledgerMAC(key [32]byte,domain string,parts ...[]byte)[32]byte{h:=hmac.New(sha256.New,key[:]);h.Write([]byte(domain));for _,part:=range parts{h.Write(part)};var out [32]byte;copy(out[:],h.Sum(nil));return out}
func ledgerFrame(raw []byte,key [32]byte)([]byte,error){if len(raw)==0||len(raw)>LedgerPayloadCap{return nil,ErrInvalid};length:=U32(uint32(len(raw)));mac:=ledgerMAC(key,"SLCLI-HOST-LEDGER-2\x00",length[:],raw);out:=make([]byte,len(raw)+36);copy(out,length[:]);copy(out[4:],raw);copy(out[4+len(raw):],mac[:]);return out,nil}
func readLedgerFrame(frame []byte,key [32]byte)([]byte,error){if len(frame)<37{return nil,ErrInvalid};length:=binary.BigEndian.Uint32(frame);if length==0||length>LedgerPayloadCap||int(length)+36!=len(frame){return nil,ErrInvalid};raw:=frame[4:4+length];mac:=ledgerMAC(key,"SLCLI-HOST-LEDGER-2\x00",frame[:4],raw);if !hmac.Equal(mac[:],frame[4+length:]){return nil,ErrInvalid};return raw,nil}
func selector(slot byte,generation uint64,digest [32]byte,key [32]byte)([74]byte,error){var out [74]byte;if slot>1||generation==0{return out,ErrInvalid};out[0]=2;out[1]=slot;binary.BigEndian.PutUint64(out[2:10],generation);copy(out[10:42],digest[:]);mac:=ledgerMAC(key,"SLCLI-HOST-SELECTOR-2\x00",out[:42]);copy(out[42:],mac[:]);return out,nil}
func readSelector(raw []byte,key [32]byte)(byte,uint64,[32]byte,error){var digest [32]byte;if len(raw)!=74||raw[0]!=2||raw[1]>1{return 0,0,digest,ErrInvalid};generation:=binary.BigEndian.Uint64(raw[2:10]);if generation==0{return 0,0,digest,ErrInvalid};mac:=ledgerMAC(key,"SLCLI-HOST-SELECTOR-2\x00",raw[:42]);if !hmac.Equal(mac[:],raw[42:]){return 0,0,digest,ErrInvalid};copy(digest[:],raw[10:42]);return raw[1],generation,digest,nil}
func validateLedgerPayload(raw []byte,p *Profile,host,epoch string)(LedgerPayload,error){
 if p==nil{return LedgerPayload{},ErrInvalid}
 var payload LedgerPayload;value,e:=decodeLedgerStrict(raw);if e!=nil{return payload,e};row,e:=object(value,"schema","hostIdentity","hostEpoch","profileSha256","keyVersion","generation","slots","observations","terminalCells","previousPayloadSha256");if e!=nil{return payload,e}
 if row["schema"]!=LedgerSchema||row["hostIdentity"]!=host||row["profileSha256"]!=digestString(p.Digest)||row["keyVersion"]!=p.KeyVersion{return payload,ErrInvalid}
 if _,e=digestText(row["hostEpoch"],64);e!=nil{return payload,e};gen,ok:=row["generation"].(string);if !ok{return payload,ErrInvalid};generation,e:=strconv.ParseUint(gen,10,64);if e!=nil||generation==0||generationString(generation)!=gen{return payload,ErrInvalid}
 slots,ok:=row["slots"].([]any);if !ok||len(slots)!=MaximumBindings{return payload,ErrInvalid};cells,ok:=row["terminalCells"].([]any);if !ok||len(cells)!=MaximumBindings{return payload,ErrInvalid}
 slotKeys:=[]string{"schema","slotId","reservationId","generation","state","hostEpoch","serviceProfileSha256","ownerNativeId","primaryIdentityDigest","createdSequence","resourceDigest","maximumBytes","retainedBytes","closureEvidenceDigest","keyVersion","observerBindingDigest","closeIntentDigest","closeIntentGeneration","closeIntentTick","absoluteDeadlineTick"}
 cellKeys:=[]string{"slotId","reservationId","intentDigest","closureEvidenceDigest","terminalGeneration","closureLedgerRef","terminalReceiptDigest","terminalReceiptRef","disposition","ackDigest"}
 for i:=0;i<MaximumBindings;i++ {
  slot,e:=object(slots[i],slotKeys...);if e!=nil{return payload,e};id,e:=zeroInteger(slot["slotId"],MaximumBindings-1);if e!=nil||id!=uint64(i)||slot["schema"]!=SlotSchema{return payload,ErrInvalid}
  state,ok:=slot["state"].(string);if !ok||!contains([]string{"FREE","RESERVED","LIVE","CLOSE_PENDING","RETAINED","QUARANTINED"},state){return payload,ErrInvalid}
  retained,e:=zeroInteger(slot["retainedBytes"],SessionBytes);if e!=nil{return payload,e}
  if state=="FREE"{if retained!=0{return payload,ErrInvalid};for _,key:=range slotKeys{if key!="schema"&&key!="slotId"&&key!="state"&&key!="retainedBytes"&&slot[key]!=nil{return payload,ErrInvalid}}}else{
   for _,key:=range []string{"reservationId","hostEpoch","serviceProfileSha256","primaryIdentityDigest","resourceDigest","observerBindingDigest"}{if _,e=digestText(slot[key],64);e!=nil{return payload,e}}
   if slot["serviceProfileSha256"]!=digestString(p.Digest)||slot["keyVersion"]!=p.KeyVersion||equalInteger(slot["maximumBytes"],SessionBytes)!=nil{return payload,ErrInvalid};if _,e=text(slot["ownerNativeId"],184);e!=nil{return payload,e}
   for _,key:=range []string{"generation","createdSequence","absoluteDeadlineTick"}{if e=decimalString(slot[key],true);e!=nil{return payload,e}}
   for _,key:=range []string{"closureEvidenceDigest","closeIntentDigest"}{if slot[key]!=nil{if _,e=digestText(slot[key],64);e!=nil{return payload,e}}}
   if slot["closeIntentDigest"]==nil{if slot["closeIntentGeneration"]!=nil||slot["closeIntentTick"]!=nil{return payload,ErrInvalid}}else{if decimalString(slot["closeIntentGeneration"],true)!=nil||decimalString(slot["closeIntentTick"],true)!=nil{return payload,ErrInvalid}}
   if state=="CLOSE_PENDING"&&slot["closeIntentDigest"]==nil{return payload,ErrInvalid}
  }
  cell,e:=object(cells[i],cellKeys...);if e!=nil{return payload,e};id,e=zeroInteger(cell["slotId"],MaximumBindings-1);if e!=nil||id!=uint64(i){return payload,ErrInvalid}
  if cell["reservationId"]==nil{for _,key:=range cellKeys{if key!="slotId"&&cell[key]!=nil{return payload,ErrInvalid}}}else{
   for _,key:=range []string{"reservationId","closureEvidenceDigest"}{if _,e=digestText(cell[key],64);e!=nil{return payload,e}}
   if decimalString(cell["terminalGeneration"],true)!=nil{return payload,ErrInvalid};if !contains([]string{"FREE","RETAINED","QUARANTINED"},cell["disposition"]){return payload,ErrInvalid}
   for _,key:=range []string{"intentDigest","closureLedgerRef","terminalReceiptDigest","terminalReceiptRef","ackDigest"}{if cell[key]!=nil{if _,e=digestText(cell[key],64);e!=nil{return payload,e}}}
   if (cell["terminalReceiptDigest"]==nil)!=(cell["terminalReceiptRef"]==nil)||cell["ackDigest"]!=nil&&cell["terminalReceiptDigest"]==nil||cell["disposition"]=="FREE"&&cell["intentDigest"]==nil{return payload,ErrInvalid}
  }
 }
 observations,ok:=row["observations"].([]any);if !ok||len(observations)>MaximumBindings{return payload,ErrInvalid};previous:=-1;for _,observation:=range observations{obs,e:=object(observation,"slotId","sha256","size");if e!=nil{return payload,e};id,e:=zeroInteger(obs["slotId"],MaximumBindings-1);if e!=nil||int(id)<=previous{return payload,ErrInvalid};previous=int(id);slot:=slots[id].(map[string]any);if slot["state"]=="FREE"{return payload,ErrInvalid};if _,e=digestText(obs["sha256"],64);e!=nil{return payload,e};if _,e=integer(obs["size"],runtimeSnapshotBytes);e!=nil{return payload,e}}
 if row["previousPayloadSha256"]!=nil{if _,e=digestText(row["previousPayloadSha256"],64);e!=nil{return payload,e}}
 if e=json.Unmarshal(raw,&payload);e!=nil{return payload,ErrInvalid};canonical,e:=json.Marshal(payload);if e!=nil||!bytesEqual(append(canonical,'\n'),raw){return payload,ErrInvalid};return payload,nil
}
func bytesEqual(a,b []byte)bool{return hmac.Equal(a,b)}
func contains(values []string,value any)bool{s,ok:=value.(string);if !ok{return false};for _,candidate:=range values{if s==candidate{return true}};return false}
func zeroInteger(v any,maximum uint64)(uint64,error){n,ok:=v.(json.Number);if !ok{return 0,ErrInvalid};s:=string(n);value,e:=strconv.ParseUint(s,10,64);if e!=nil||value>maximum||strconv.FormatUint(value,10)!=s{return 0,ErrInvalid};return value,nil}
func decimalString(v any,positive bool)error{s,ok:=v.(string);if !ok{return ErrInvalid};n,e:=strconv.ParseUint(s,10,64);if e!=nil||positive&&n==0||strconv.FormatUint(n,10)!=s{return ErrInvalid};return nil}
