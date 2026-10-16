//go:build linux || windows

package hostadmission

import "crypto/sha256"

// This reader decodes an original held calibration body; it does not grant
// accounting authority. The acquisition owner must authenticate every original
// rawEvidence object and the independent P/C source associations before use.
// raw remains borrowed from the retained original admission mapping, never a
// caller-owned copy or an embedded table keyed by this image's future digest.
func parseCalibrationSourceBytes(raw []byte,profile *Profile)(allocationContract,error){
 var out allocationContract
 if profile==nil||profile.Platform!=nativePlatform||profile.Digest==[32]byte{}||len(raw)==0||len(raw)>FrameBytes{return out,ErrInvalid}
 value,e:=decodeStrict(raw);if e!=nil{return out,e}
 row,e:=object(value,"schema","profileDigest","sourceDigest","domain","maximumBytes","nativeObjectBytes","runtimeBaseBytes","stackReservationBytes","ioReservationBytes","physicalPageBytes","maximumObjects","rawEvidence","sourceOwnedPeakBytes");if e!=nil||row["schema"]!="service-lasso-native-allocation-calibration.v1"{return out,ErrInvalid}
 profileText,e:=digestText(row["profileDigest"],64);if e!=nil||profileText!=digestString(profile.Digest){return out,ErrInvalid};out.profile=profile.Digest
 sourceText,e:=digestText(row["sourceDigest"],64);if e!=nil{return out,e};out.source,e=ParseDigest(sourceText);if e!=nil||out.source==[32]byte{}{return allocationContract{},ErrInvalid}
 domain,e:=zeroInteger(row["domain"],4);if e!=nil||domain==3{return out,ErrInvalid};out.domain=byte(domain)
 out.maximumBytes,e=integer(row["maximumBytes"],9007199254740991);if e!=nil{return out,e}
 // The original measured B0/B2 maxima include the full runtimes. The copy-set ceiling is only a subset of B2; no maximum is invented or enrolled here.
 switch out.domain{case 0,2:case 1:if out.maximumBytes!=ControlBytes{return out,ErrInvalid};case 4:default:return out,ErrInvalid}
 for _,binding:=range []struct{name string;target *uint64}{{"nativeObjectBytes",&out.nativeObjectBytes},{"runtimeBaseBytes",&out.runtimeBaseBytes},{"stackReservationBytes",&out.stackReservationBytes},{"ioReservationBytes",&out.ioReservationBytes},{"physicalPageBytes",&out.physicalPageBytes}}{
  *binding.target,e=integer(row[binding.name],out.maximumBytes);if e!=nil{return out,e}
 }
 objects,e:=integer(row["maximumObjects"],256);if e!=nil{return out,e};out.maximumObjects=uint32(objects)
 if out.physicalPageBytes&(out.physicalPageBytes-1)!=0{return out,ErrInvalid}
 out.calibrationRaw=raw;out.calibrationSHA256=sha256.Sum256(raw)
 if e=validateAllocationCalibration(&out);e!=nil{return allocationContract{},e}
 return out,nil
}