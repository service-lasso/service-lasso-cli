//go:build linux || windows

package hostadmission

import("crypto/sha256";"encoding/hex";"strconv";"unsafe")

// Encode into an original precharged native scratch view. No JSON heap, map,
// reallocating append, or inferred empty failure capture. The owning route may
// reuse this view only after its actual original write/flush/readback has ended.
func primitiveInto(destination []byte,kind,operation,outcome string,identity,related,source [32]byte,tick uint64,data []byte,mode *string)(int,error){
 if len(destination)<2048||identity==[32]byte{}||related==[32]byte{}||source==[32]byte{}||tick==0||!validNativeOperation(kind,operation)||outcome!="success"&&outcome!="failure"&&outcome!="unresolved"{return 0,ErrInvalid}
 if len(data)>0{d,s:=uintptr(unsafe.Pointer(unsafe.SliceData(destination))),uintptr(unsafe.Pointer(unsafe.SliceData(data)));if d>^uintptr(0)-2048||s>^uintptr(0)-uintptr(len(data))||d<s+uintptr(len(data))&&s<d+2048{return 0,ErrInvalid}}
 hasBytes:=false
 switch kind{
 case "read","write","eof","discovery","ledger":if data==nil&&outcome=="success"{return 0,ErrInvalid};hasBytes=data!=nil;if mode!=nil||kind=="eof"&&(len(data)!=0||outcome!="success"){return 0,ErrInvalid}
 case "birth","identity","flush","exit","close":if data!=nil||mode!=nil&&(kind!="identity"||(*mode!="0644"&&*mode!="0755")||(operation!="linux.fstat"&&operation!="win32.security_info")){return 0,ErrInvalid}
 default:return 0,ErrInvalid
 }
 var observedDigest [32]byte;if hasBytes{observedDigest=sha256.Sum256(data)}
 at:=0;failed:=false
 put:=func(value string){if failed||len(value)>len(destination)-at{failed=true;return};copy(destination[at:],value);at+=len(value)}
 digest:=func(value [32]byte){if failed||64>len(destination)-at{failed=true;return};hex.Encode(destination[at:at+64],value[:]);at+=64}
 number:=func(value uint64){if failed||20>len(destination)-at{failed=true;return};encoded:=strconv.AppendUint(destination[at:at:at+20],value,10);at+=len(encoded)}
 put("{\"kind\":\"");put(kind);put("\",\"platform\":\"");put(nativePlatform);put("\",\"objectIdentityDigest\":\"");digest(identity);put("\",\"monotonicTick\":\"");number(tick);put("\",\"operation\":\"");put(operation);put("\",\"outcome\":\"");put(outcome);put("\",\"bytesSha256\":")
 if hasBytes{put("\"");digest(observedDigest);put("\"")}else{put("null")};put(",\"byteCount\":");if hasBytes{number(uint64(len(data)))}else{put("null")};put(",\"mode\":");if mode==nil{put("null")}else{put("\"");put(*mode);put("\"")};put(",\"relatedIdentityDigest\":\"");digest(related);put("\",\"sourceImageSha256\":\"");digest(source);put("\"}\n")
 if failed||at>2048{return 0,ErrCapacity};return at,nil
}
