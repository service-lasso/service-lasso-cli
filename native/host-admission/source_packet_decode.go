//go:build linux || windows

package hostadmission

// These are original retained read objects, not source-authority capabilities.
// The independently admitted caller/provider still owns their source packet,
// raw Git membership and ROOT authentication. Ordinary requests cannot acquire
// or enroll this private decoder result, and it carries no profile seal.
type originalSourceBody struct{
 handle objectHandle
 memory *nativeMemory
 observation sourceObjectObservation
}
type decodedOriginalSourcePacket struct{
 profile *Profile
 grant nativeGrant
 calibrations [MaximumSourceInputs-2]allocationContract
 calibrationCount uint32
}
func decodeOriginalSourcePacket(profileBody,grantBody *originalSourceBody,calibrationBodies []*originalSourceBody)(decoded decodedOriginalSourcePacket,err error){
 // Every failed decode keeps every participating original, including bodies
 // not yet parsed. No retry resets an acquired input packet or its charges.
 defer func(){if err!=nil{retainFailedSourceBody(profileBody);retainFailedSourceBody(grantBody);for _,body:=range calibrationBodies{retainFailedSourceBody(body)};decoded=decodedOriginalSourcePacket{}}}()
 // Invalid counts and missing members still arrived with original held
 // resources. Install failure retention before checking the packet shape.
 if profileBody==nil||grantBody==nil||len(calibrationBodies)==0||len(calibrationBodies)>MaximumSourceInputs-2{return decoded,ErrInvalid}
 if sameSourceBodyStorage(profileBody,grantBody){return decoded,ErrInvalid}
 for i,body:=range calibrationBodies{
  if body==nil||sameSourceBodyStorage(body,profileBody)||sameSourceBodyStorage(body,grantBody){return decoded,ErrInvalid}
  for j:=0;j<i;j++{if sameSourceBodyStorage(body,calibrationBodies[j]){return decoded,ErrInvalid}}
 }
 raw,e:=recheckOriginalSourceBody(profileBody.handle,profileBody.memory,&profileBody.observation);if e!=nil{return decoded,e}
 profile,e:=parseProfile(raw);if e!=nil||profile.Platform!=nativePlatform||!sourceOwnerIdentity(profile.Owner){return decoded,ErrInvalid}
 raw,e=recheckOriginalSourceBody(grantBody.handle,grantBody.memory,&grantBody.observation);if e!=nil{return decoded,e}
 grant,e:=parseGrantSourceBytes(raw,profile);if e!=nil{return decoded,e}
 decoded.profile=profile;decoded.grant=grant
 for i,body:=range calibrationBodies{
  raw,e=recheckOriginalSourceBody(body.handle,body.memory,&body.observation);if e!=nil{return decoded,e}
  calibration,e:=parseCalibrationSourceBytes(raw,profile);if e!=nil{return decoded,e}
  for j:=0;j<i;j++{prior:=decoded.calibrations[j];if prior.domain==calibration.domain&&prior.source==calibration.source{return decoded,ErrInvalid}}
  decoded.calibrations[i]=calibration;decoded.calibrationCount++
 }
 // Parser acceptance does not end custody. Reobserve all original bodies
 // after the entire decode, without reopening any input path or copying the
 // raw source bytes into a mutable compiled association table.
 if _,e=recheckOriginalSourceBody(profileBody.handle,profileBody.memory,&profileBody.observation);e!=nil{return decoded,e}
 if _,e=recheckOriginalSourceBody(grantBody.handle,grantBody.memory,&grantBody.observation);e!=nil{return decoded,e}
 for _,body:=range calibrationBodies{if _,e=recheckOriginalSourceBody(body.handle,body.memory,&body.observation);e!=nil{return decoded,e}}
 return decoded,nil
}
func sameSourceBodyStorage(left,right *originalSourceBody)bool{
 if left==nil||right==nil{return true}
 return left==right||left.memory==right.memory||left.observation.identity==right.observation.identity||left.handle==right.handle
}
func retainFailedSourceBody(body *originalSourceBody){if body!=nil&&body.memory!=nil{body.memory.retainSourceReadFailure()}}
