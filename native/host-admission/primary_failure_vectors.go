//go:build linux || windows

package hostadmission

// The independently admitted native fixture supplies the actual original
// transport, accepted reservation and malformed/out-of-phase request. It must
// independently cause the response transport to fail after dispatch failure.
// No mock store, fabricated native return or callback can establish that cause.
// This vector exercises the ordinary complete exchange and its nested unwind.
func originalDispatchTransportFailureVector(input *primaryFrameInput,output *primaryFrameOutput,commands *primaryCommands)error{
 if input==nil||output==nil||commands==nil||commands.ledger==nil||
    !commands.reserved||commands.failed||commands.failureRetentionAttempted||commands.failureRetentionSkipped||
    commands.failureOriginalCause!=nil||commands.failureRetentionCause!=nil||
    commands.failureTransportCause!=nil{return ErrInvalid}
 ledger:=commands.ledger
 ledger.mu.Lock();before:=ledger.generation;lost:=ledger.lostOriginalObjects;ledger.mu.Unlock()
 if lost||before==^uint64(0){return ErrUnavailable}
 _,e:=exchangeOriginalPrimary(input,output,commands)
 if e==nil||!commands.failed||!input.failed.Load()||!output.failed.Load()||
    !commands.failureRetentionAttempted||commands.failureOriginalCause==nil||
    commands.failureTransportCause==nil||commands.failureRetentionCause!=nil{return ErrInvalid}
 ledger.mu.Lock()
 if ledger.lostOriginalObjects||ledger.generation!=before+1||!ledger.pair.complete||
    ledger.pair.generation!=ledger.generation||commands.reservation.slot>=MaximumBindings||
    ledger.payload.Slots[commands.reservation.slot].State!="RETAINED"{
  ledger.mu.Unlock();return ErrInvalid
 }
 generation:=ledger.generation;raw:=ledger.raw
 pairRaw:=ledger.pair.raw;selectorReadback:=ledger.pair.selectorReadback
 generationReadback:=ledger.pair.generationReadback
 ledger.mu.Unlock()
 // A second unwind from the same actual transport cause must not create a
 // second retention transaction or overwrite either original returned cause.
 originalCause:=commands.failureOriginalCause;transportCause:=commands.failureTransportCause
 _ = commands.retainFailure(transportCause)
 ledger.mu.Lock();defer ledger.mu.Unlock()
 if ledger.generation!=generation||ledger.lostOriginalObjects||
    !sameOriginalSlice(raw,ledger.raw)||!sameOriginalSlice(pairRaw,ledger.pair.raw)||
    !sameOriginalSlice(selectorReadback,ledger.pair.selectorReadback)||
    !sameOriginalSlice(generationReadback,ledger.pair.generationReadback)||
    commands.failureOriginalCause!=originalCause||commands.failureTransportCause!=transportCause||
    commands.failureRetentionCause!=nil{return ErrInvalid}
 return nil
}

// The independently supplied original profile check must genuinely fail or
// return a different original profile; this vector installs no profile seal.
// Consuming the driver must record the failure without turning unavailable
// source authority into a store write or another retry opportunity.
func originalExchangeProfileFailureVector(input *primaryFrameInput,output *primaryFrameOutput,commands *primaryCommands)error{
 if input==nil||output==nil||commands==nil||commands.ledger==nil||
    !commands.reserved||commands.exchangeClaimed||commands.failed||
    commands.failureRetentionAttempted||commands.failureRetentionSkipped{return ErrInvalid}
 ledger:=commands.ledger
 ledger.mu.Lock();generation:=ledger.generation;raw:=ledger.raw;pairRaw:=ledger.pair.raw;ledger.mu.Unlock()
 _,e:=exchangeOriginalPrimary(input,output,commands)
 if e==nil||!commands.exchangeClaimed||!commands.failed||!input.failed.Load()||!output.failed.Load()||
    commands.failureOriginalCause==nil||commands.failureRetentionAttempted||
    !commands.failureRetentionSkipped||commands.failureRetentionEligibilityCause==nil||
    commands.failureRetentionCause!=nil||commands.failureTransportCause!=nil{return ErrInvalid}
 if commands.inputProfileError==nil&&commands.ledgerProfileError==nil&&
    commands.inputProfileOriginal==commands.ledgerProfileOriginal{return ErrInvalid}
 cause:=commands.failureOriginalCause;eligibility:=commands.failureRetentionEligibilityCause
 _ = commands.retainFailure(e)
 if _,e=exchangeOriginalPrimary(input,output,commands);e==nil{return ErrInvalid}
 ledger.mu.Lock();defer ledger.mu.Unlock()
 if ledger.generation!=generation||!sameOriginalSlice(raw,ledger.raw)||
    !sameOriginalSlice(pairRaw,ledger.pair.raw)||commands.failureRetentionAttempted||
    commands.failureOriginalCause!=cause||commands.failureRetentionEligibilityCause!=eligibility{return ErrInvalid}
 return nil
}
