//go:build linux || windows || darwin

package main

import (
 "bytes"
 "encoding/binary"
 "errors"
 "io"
 "testing"
)

// These are unexecuted decoder regression sources, not TC/CA native acceptance.
// Full original owner/bootstrap/observer fixture paths remain separately required.
func TestV3FrameRejectsUntrustedLengthBeforePayloadAllocation(t *testing.T) {
 for _,size:=range []uint32{8193,0xffffffff} {
  var wire [9]byte
  binary.BigEndian.PutUint32(wire[:4],size);wire[4]=2
  input:=bytes.NewReader(wire[:]);budget:=&nativeBudget{}
  _,err:=readNativeFrame(input,authoringDomain,0,make([]byte,32),make([]byte,32),0,map[byte]uint32{2:8192},budget)
  if !errors.Is(err,errNativeProtocol)||budget.bytes!=0||budget.peak!=0||input.Len()!=0 { t.Fatalf("length %d accepted or allocated before cap: %v %d %d",size,err,budget.bytes,budget.peak) }
 }
}

func TestV3FrameBindsDirectionSequenceAndCompleteMac(t *testing.T) {
 nonce:=bytes.Repeat([]byte{0x51},32);key:=bytes.Repeat([]byte{0xa7},32)
 body:=append([]byte{2,0,0,0,0},wireString("/held/original")...)
 var encoded bytes.Buffer
 if err:=writeNativeFrame(&encoded,authoringDomain,0,nonce,key,0,body,8192);err!=nil {t.Fatal(err)}
 baseline:=append([]byte(nil),encoded.Bytes()...)
 for _,kind:=range []string{"body","mac","direction","sequence","truncated-mac"} {
  wire:=append([]byte(nil),baseline...);direction:=byte(0);sequence:=uint32(0)
  switch kind {case "body":wire[10]^=1;case "mac":wire[len(wire)-1]^=1;case "direction":direction=1;case "sequence":sequence=1;case "truncated-mac":wire=wire[:len(wire)-1]}
  budget:=&nativeBudget{}
  if _,err:=readNativeFrame(bytes.NewReader(wire),authoringDomain,direction,nonce,key,sequence,map[byte]uint32{2:8192},budget);err==nil {t.Fatalf("accepted %s",kind)}
  if kind=="sequence" {if budget.bytes!=0 {t.Fatal("sequence allocated payload")}} else if !budget.retained {t.Fatalf("%s lost conservative failed-buffer charge",kind)}
 }
 budget:=&nativeBudget{}
 actual,err:=readNativeFrame(bytes.NewReader(baseline),authoringDomain,0,nonce,key,0,map[byte]uint32{2:8192},budget)
 if err!=nil||!bytes.Equal(actual,body)||budget.retained||budget.bytes!=uint64(len(body)){t.Fatalf("valid frame mismatch: %v",err)}
}

func regressionPlan(receipt [32]byte,paths []string) []byte {
 body:=append([]byte{4,0,0,0,1},receipt[:]...)
 body=append(body,wireString("/original/destination")...);body=append(body,wireString("valid-service")...)
 body=append(body,0);body=append(body,be32(uint32(len(paths)))...)
 for _,path:=range paths {body=append(body,wireString(path)...);body=append(body,0);body=append(body,be32(1)...);body=append(body,'x')}
 return body
}

func TestV3PlanRejectsNamespaceAliasAndTrailingBytes(t *testing.T) {
 var receipt [32]byte;receipt[0]=0x31
 admission:=nativeAdmission{maximumFiles:128,maximumTotal:4096}
 for _,paths:=range [][]string{{"a","a/b"},{"A/x","a/y"},{"../a"},{"CON.txt"},{"a.","b"},{"b","a"},{"a","a"}} {
  if _,err:=parseNativePlan(regressionPlan(receipt,paths),make([]byte,32),receipt,admission);err==nil {t.Fatalf("accepted namespace %q",paths)}
 }
 valid:=regressionPlan(receipt,[]string{"A/x","b/y"})
 if _,err:=parseNativePlan(append(valid,0),make([]byte,32),receipt,admission);err==nil {t.Fatal("accepted trailing plan byte")}
 wrong:=receipt;wrong[1]=1
 if _,err:=parseNativePlan(valid,make([]byte,32),wrong,admission);err==nil {t.Fatal("accepted replacement READ receipt")}
 if _,err:=parseNativePlan(valid,make([]byte,32),receipt,admission);err!=nil {t.Fatalf("rejected separate namespace: %v",err)}
}

func TestV3TruncatedFrameKeepsAllocatedOriginalPrefix(t *testing.T) {
 body:=append([]byte{2,0,0,0,0},wireString("/original")...)
 var wire bytes.Buffer
 if err:=writeNativeFrame(&wire,authoringDomain,0,make([]byte,32),make([]byte,32),0,body,8192);err!=nil{t.Fatal(err)}
 budget:=&nativeBudget{}
 _,err:=readNativeFrame(bytes.NewReader(wire.Bytes()[:11]),authoringDomain,0,make([]byte,32),make([]byte,32),0,map[byte]uint32{2:8192},budget)
 if !errors.Is(err,io.ErrUnexpectedEOF)||!budget.retained||budget.bytes!=uint64(len(body)){t.Fatalf("lost incomplete capture charge: %v %d",err,budget.bytes)}
}