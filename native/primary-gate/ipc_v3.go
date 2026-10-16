//go:build linux || windows || darwin

package main

import (
 "crypto/hmac"
 "crypto/sha256"
 "encoding/binary"
 "encoding/hex"
 "net"
 "os"
 "os/exec"
 "strings"
 "time"
)

// Fixed by the source packager, not by argv/environment or a request peer.
var nativeCandidateVersion string
var nativeCandidateSourceSHA string
var originalPrimaryLaunch = time.Now()
const authoringDomain = "SLCLI-AUTHORING-3\x00"

func candidateGreetingIdentity()([20]byte,[32]byte,error){
 var source [20]byte;helper:=sha256.Sum256(confinedWriterBytes)
 if !validNativeCandidateVersion(nativeCandidateVersion)||len(nativeCandidateSourceSHA)!=40{return source,helper,errNativeProtocol}
 raw,e:=hex.DecodeString(nativeCandidateSourceSHA);if e!=nil||len(raw)!=20{return source,helper,errNativeProtocol};copy(source[:],raw)
 return source,helper,nil
}
func validNativeCandidateVersion(value string)bool{
 if len(value)<1||len(value)>64{return false};offset:=0
 for component:=0;component<3;component++{start:=offset;for offset<len(value)&&value[offset]>='0'&&value[offset]<='9'{offset++};if offset==start{return false};if component<2{if offset>=len(value)||value[offset]!='.'{return false};offset++}}
 if len(value)-offset!=12||!strings.EqualFold(value[offset:offset+5],"-dev."){return false};for _,b:=range value[offset+5:]{if !(b>='0'&&b<='9'||b>='a'&&b<='f'||b>='A'&&b<='F'){return false}};return true
}
func primaryPhase(connection net.Conn,maximum time.Duration)error{
 absolute:=originalPrimaryLaunch.Add(40*time.Second);now:=time.Now();if !now.Before(absolute){return errNativeProtocol};end:=now.Add(maximum);if absolute.Before(end){end=absolute};return connection.SetDeadline(end)
}
func primaryFailure(connection net.Conn,nonce [32]byte,operation byte,sequence uint32,code byte)error{
 body:=[7]byte{operation,0,0,0,0,1,code};binary.BigEndian.PutUint32(body[1:5],sequence)
 return writeNativeFrame(connection,authoringDomain,1,nonce[:],gateCapability[:],sequence,body[:],256)
}
func materialize(connection net.Conn){
 // Closure proof and native runtime/allocation accounting are owned by the
 // complete production service integration, not inferred from this consumer.
 defer connection.Close()
 source,helper,e:=candidateGreetingIdentity();if e!=nil{return}
 if e=primaryPhase(connection,10*time.Second);e!=nil{return};nonce,e:=nativeNonce();if e!=nil{return}
 if e=nativeGreeting(connection,gateCapability[:],nonce,nativeCandidateVersion,source,helper);e!=nil{return}
 budget:=&nativeBudget{}
 if e=primaryPhase(connection,10*time.Second);e!=nil{return}
 request,e:=readNativeFrame(connection,authoringDomain,0,nonce[:],gateCapability[:],0,map[byte]uint32{2:8192},budget);if e!=nil{return}
 cursor:=wireCursor{bytes:request};operation,e:=cursor.u8();sequence,se:=cursor.u32();if e!=nil||se!=nil||operation!=2||sequence!=0{return};root,e:=cursor.string(4096,false);if e!=nil||cursor.end()!=nil{return}
 // No request data can enroll or select a different source catalogue grant.
 if len(nativeAdmissions)!=1{_ = primaryFailure(connection,nonce,3,0,1);return};admission:=nativeAdmissions[0]
 assets,e:=readOriginalAssets(root,admission,budget);if e!=nil{budget.holdFailure();_ = primaryFailure(connection,nonce,3,0,nativeInputFailureCode(e));return}
 receipt:=readReceipt(nonce[:],assets)
 resultLength:=uint32(38);for _,asset:=range assets{resultLength+=5+uint32(len(asset.bytes))};if resultLength>300000{budget.holdFailure();return}
 readResult,e:=budget.allocate(resultLength);if e!=nil{return};readResult[0]=3;readResult[5]=0;copy(readResult[6:38],receipt[:]);at:=38
 for _,asset:=range assets{readResult[at]=asset.name;at++;copy(readResult[at:at+4],be32(uint32(len(asset.bytes))));at+=4;copy(readResult[at:],asset.bytes);at+=len(asset.bytes)}
 if e=writeNativeFrame(connection,authoringDomain,1,nonce[:],gateCapability[:],0,readResult,300000);e!=nil{budget.holdFailure();return}
 if e=primaryPhase(connection,10*time.Second);e!=nil{return}
 action,e:=readNativeFrame(connection,authoringDomain,0,nonce[:],gateCapability[:],1,map[byte]uint32{4:768000,6:256},budget);if e!=nil{return}
 if action[0]==6{
  if len(action)!=5{return}
  // The selected local CLOSE acknowledges completed reader/asset/private-buffer
  // ownership, not exit of this still-live primary/SEA or later host FREE.
  // Original native buffer ownership/closure is not yet connected here.
  _ = primaryFailure(connection,nonce,7,1,14);return
 }
 plan,e:=parseNativePlan(action,nonce[:],receipt,admission);if e!=nil{_ = primaryFailure(connection,nonce,5,1,7);return}
 if e=primaryPhase(connection,20*time.Second);e!=nil{_ = primaryFailure(connection,nonce,5,1,13);return}
 command,e:=budget.allocate(768000);if e!=nil{_ = primaryFailure(connection,nonce,5,1,17);return};size,e:=encodeWriterCommand(command,nonce,plan);if e!=nil{_ = primaryFailure(connection,nonce,5,1,7);return}
 if heldExecutionPath==""{_ = primaryFailure(connection,nonce,5,1,10);return}
 child:=exec.Command(heldExecutionPath);if heldHelper!=nil{child.ExtraFiles=[]*os.File{heldHelper}};child.Stdin=bytesReader(command[:size])
 stdout:=writerResultBuffer{limit:32};stderr:=writerResultBuffer{limit:64};child.Stdout=&stdout;child.Stderr=&stderr
 if e=child.Run();e!=nil||stdout.failed||stderr.failed||stdout.used!=32||stderr.used!=0{budget.holdFailure();_ = primaryFailure(connection,nonce,5,1,11);return}
 if !hmac.Equal(stdout.raw[:32],plan.expectedReadback[:]){budget.holdFailure();_ = primaryFailure(connection,nonce,5,1,12);return}
 // Local PLAN success requires original writer natural-zero, actual binary
 // stream EOF and complete held readback. The still-live primary/SEA exit and
 // host archive/ACK are later obligations, never a prerequisite for sending
 // this local terminal response. Native writer/stream/buffer ownership is
 // unfinished here; Go Wait and consumer equality alone cannot supply it.
 budget.holdFailure();_ = primaryFailure(connection,nonce,5,1,14)
}

