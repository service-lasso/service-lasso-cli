//go:build linux || windows

package hostadmission

// The original provider owns every lease and independently authenticates its
// source ROOT/repository associations before this comparison. This reader
// installs no Profile seal, grant catalogue or accounting contract. Parser
// success and Git membership cannot authenticate the provider or its caller.
type originalSourcePacketLease struct{
 profile,grant *originalGitSourceLease
 calibrations [MaximumSourceInputs-2]*originalGitSourceLease
 calibrationCount uint32
}
func(packet *originalSourcePacketLease)decodeHeldPacket()(decoded decodedOriginalSourcePacket,err error){
 if packet==nil{return decoded,ErrUnavailable}
 defer func(){if err!=nil{packet.retainFailure();decoded=decodedOriginalSourcePacket{}}}()
 if packet.profile==nil||packet.grant==nil||packet.calibrationCount==0||packet.calibrationCount>MaximumSourceInputs-2{return decoded,ErrInvalid}
 for i:=packet.calibrationCount;i<MaximumSourceInputs-2;i++{if packet.calibrations[i]!=nil{return decoded,ErrInvalid}}
 if e:=packet.profile.compareHeldSourcePath();e!=nil{return decoded,e}
 if e:=packet.grant.compareHeldSourcePath();e!=nil{return decoded,e}
 var bodies [MaximumSourceInputs-2]*originalSourceBody
 for i:=uint32(0);i<packet.calibrationCount;i++{lease:=packet.calibrations[i];if lease==nil{return decoded,ErrInvalid};if e:=lease.compareHeldSourcePath();e!=nil{return decoded,e};bodies[i]=lease.blob}
 decoded,err=decodeOriginalSourcePacket(packet.profile.blob,packet.grant.blob,bodies[:packet.calibrationCount]);if err!=nil{return decoded,err}
 // The exact raw profile digest named by G/C is checked by the closed source
 // readers. Preserve the independently admitted genuine blob associations as
 // well; no equal reencoded JSON or replacement raw file can satisfy them.
 if decoded.profile.Digest!=packet.profile.rawSHA256{return decoded,ErrInvalid}
 for i:=uint32(0);i<packet.calibrationCount;i++{if decoded.calibrations[i].calibrationSHA256!=packet.calibrations[i].rawSHA256{return decoded,ErrInvalid}}
 if err=packet.profile.compareHeldSourcePath();err!=nil{return decoded,err}
 if err=packet.grant.compareHeldSourcePath();err!=nil{return decoded,err}
 for i:=uint32(0);i<packet.calibrationCount;i++{if err=packet.calibrations[i].compareHeldSourcePath();err!=nil{return decoded,err}}
 return decoded,nil
}
func(packet *originalSourcePacketLease)retainFailure(){
 if packet==nil{return};packet.profile.retainFailure();packet.grant.retainFailure()
 for i:=uint32(0);i<MaximumSourceInputs-2;i++{packet.calibrations[i].retainFailure()}
}
