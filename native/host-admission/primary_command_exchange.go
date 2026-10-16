//go:build linux || windows

package hostadmission

// The ordinary owning service route uses one original authenticated channel,
// input/output capture set and command state. It returns a durable intent only;
// the independent original process/IO/observer/archive owner finalizes later.
func exchangeOriginalPrimary(input *primaryFrameInput,output *primaryFrameOutput,commands *primaryCommands)(intent closeIntent,err error){
 if input==nil||output==nil||commands==nil||commands.ledger==nil||input.binding==nil||output.input!=input||commands.binding!=input.binding{return intent,ErrUnavailable}
 // One original service driver owns this command session through terminal
 // intent or failure. A concurrent/restarted driver cannot read another frame
 // or overwrite the same embedded result/retention owners.
 commands.exchangeMu.Lock();if commands.exchangeClaimed{commands.exchangeMu.Unlock();return intent,ErrPhase};commands.exchangeClaimed=true;commands.exchangeMu.Unlock()
 sourceReady:=false
 defer func(){if err!=nil{
  if sourceReady{err=commands.retainFailure(err)}else{err=commands.retainEarlyFailure(err)}
  input.failed.Store(true);output.failed.Store(true)
  if input.memory!=nil&&input.memory.accounting!=nil{input.memory.accounting.holdFailure()}
 }}()
 profile,e:=originalProfile(input.profile);commands.inputProfileOriginal=profile;commands.inputProfileError=e;if e!=nil{return intent,e}
 ledgerProfile,e:=originalProfile(commands.ledger.profile);commands.ledgerProfileOriginal=ledgerProfile;commands.ledgerProfileError=e;if e!=nil||ledgerProfile!=profile{return intent,ErrUnavailable}
 sourceReady=true
 for{
  var request Frame
  if e=input.read(&request);e!=nil{return intent,e}
  response,commandError:=commands.dispatch(&request)
  if commandError!=nil{
   failure,shapeError:=originalPrimaryFailureResponse(&request,commandError)
   if shapeError!=nil{return intent,commandError}
   if e=output.write(&failure);e!=nil{commands.failureTransportCause=e;return intent,e}
   return intent,commandError
  }
  if e=output.write(&response);e!=nil{commands.failureTransportCause=e;return intent,e}
  if commands.closing{
   // A completed write plus original readback proves only delivery to the
   // source channel. It does not mark intentACKObserved or synthesize primary
   // receipt/physical close. No second request/response follows this intent.
   return commands.intent,nil
  }
 }
}
