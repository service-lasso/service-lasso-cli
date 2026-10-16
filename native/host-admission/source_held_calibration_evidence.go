//go:build linux || windows

package hostadmission

type originalCalibrationEvidenceLease struct{
 name string
 source [32]byte
 body *originalSourceBody
}
// The provider's original admission owns names/source associations separately.
// This routine compares those actual held raw objects to the closed C body;
// it cannot supply the measurement, provider, ROOT or source authority itself.
func compareHeldCalibrationEvidence(contract *allocationContract,leases []originalCalibrationEvidenceLease)(err error){
 defer func(){if err!=nil{for i:=range leases{retainFailedSourceBody(leases[i].body)}}}()
 if contract==nil||len(leases)==0||len(leases)>MaximumSourceInputs{return ErrInvalid}
 var observations [MaximumSourceInputs]calibrationEvidenceObservation
 for i:=range leases{
  lease:=&leases[i];if lease.body==nil||!evidenceName(lease.name)||lease.source!=contract.source{return ErrInvalid}
  for j:=0;j<i;j++{if lease.name==leases[j].name||sameSourceBodyStorage(lease.body,leases[j].body){return ErrInvalid}}
  if err=recheckHeldCalibrationEvidenceBody(lease.body);err!=nil{return err}
  observations[i]=calibrationEvidenceObservation{name:lease.name,source:lease.source,observation:lease.body.observation}
 }
 if err=compareCalibrationOriginalEvidence(contract,observations[:len(leases)]);err!=nil{return err}
 for i:=range leases{if err=recheckHeldCalibrationEvidenceBody(leases[i].body);err!=nil{return err}}
 return nil
}
func recheckHeldCalibrationEvidenceBody(body *originalSourceBody)error{
 if body==nil{return ErrInvalid}
 if body.observation.streamed{return recheckOriginalStreamedEvidence(body)}
 _,e:=recheckOriginalSourceBody(body.handle,body.memory,&body.observation);return e
}
