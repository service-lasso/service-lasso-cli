//go:build linux || windows

package hostadmission

// An acquisition owner retains these observations beside each original native
// source lease. This view cannot authenticate its own name, source association,
// provider or ROOT; it only compares the exact independently associated native
// read results against the calibration's closed evidence references.
type calibrationEvidenceObservation struct {
 name string
 source [32]byte
 observation sourceObjectObservation
}
func compareCalibrationOriginalEvidence(contract *allocationContract,observations []calibrationEvidenceObservation)error{
 if contract==nil||len(observations)==0||len(observations)>MaximumSourceInputs{return ErrInvalid}
 value,e:=decodeStrict(contract.calibrationRaw);if e!=nil{return e}
 row,e:=object(value,"schema","profileDigest","sourceDigest","domain","maximumBytes","nativeObjectBytes","runtimeBaseBytes","stackReservationBytes","ioReservationBytes","physicalPageBytes","maximumObjects","rawEvidence","sourceOwnedPeakBytes");if e!=nil{return e}
 entries,ok:=row["rawEvidence"].([]any);if !ok||len(entries)!=len(observations){return ErrInvalid}
 for i,entry:=range entries{
  reference,e:=object(entry,"name","sha256","size","sourceDigest");if e!=nil{return e}
  name,e:=text(reference["name"],240);if e!=nil||!evidenceName(name){return ErrInvalid}
  for j:=0;j<i;j++{prior,_:=entries[j].(map[string]any);if prior["name"]==name{return ErrInvalid}}
  matched:=-1
  for j:=range observations{if observations[j].name==name{if matched!=-1{return ErrInvalid};matched=j}}
  if matched<0{return ErrUnavailable};original:=&observations[matched]
  if original.source!=contract.source||original.observation.identity==[32]byte{}||original.observation.owner==""||original.observation.bytes==0||original.observation.rawSHA256==[32]byte{}||original.observation.gitBlob==[20]byte{}||!original.observation.eofObserved||!sourceObjectEOF(original.observation.eofCount,original.observation.eofError){return ErrInvalid}
  size,e:=integer(reference["size"],SessionBytes);if e!=nil||size!=original.observation.bytes{return ErrInvalid}
  digest,e:=digestText(reference["sha256"],64);if e!=nil||digest!=digestString(original.observation.rawSHA256){return ErrInvalid}
  source,e:=digestText(reference["sourceDigest"],64);if e!=nil||source!=digestString(original.source){return ErrInvalid}
  before,after:=original.observation.before,original.observation.after
  if before.identity!=original.observation.identity||after.identity!=before.identity||before.owner!=original.observation.owner||after.owner!=before.owner||before.size!=size||after.size!=size||before.rawBytes==0||before.rawBytes!=after.rawBytes||before.raw!=after.raw||before.nativeBasic!=after.nativeBasic{return ErrInvalid}
 }
 return nil
}