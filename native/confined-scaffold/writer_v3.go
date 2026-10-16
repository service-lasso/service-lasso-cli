package main

import (
 "encoding/binary"
 "fmt"
 "os"
 "unicode/utf8"
)

// The owning route must charge and retain this complete original input arena.
// The decoder borrows it; strings and fixed metadata also need actual source
// allocation instrumentation before the production route may use this decoder.
// The writer receives no input handles or launch capability.
// This structural decoder does not repeat the SEA canonical semantic owner.
type writerAttempt struct{
 nonce,readReceipt,planDigest [32]byte
 destination string
 entries [128]entry
 count uint32
 original []byte
}
type writerCursor struct{raw []byte;at int}
func(c *writerCursor)take(n int)([]byte,error){if c==nil||n<0||c.at<0||c.at>len(c.raw)||n>len(c.raw)-c.at{return nil,fmt.Errorf("truncated writer input")};out:=c.raw[c.at:c.at+n];c.at+=n;return out,nil}
func(c *writerCursor)u32()(uint32,error){raw,e:=c.take(4);if e!=nil{return 0,e};return binary.BigEndian.Uint32(raw),nil}
func(c *writerCursor)text(maximum uint32)(string,error){n,e:=c.u32();if e!=nil||n==0||n>maximum{return "",fmt.Errorf("invalid writer string")};raw,e:=c.take(int(n));if e!=nil||!utf8.Valid(raw){return "",fmt.Errorf("invalid writer UTF8")};for _,b:=range raw{if b==0{return "",fmt.Errorf("invalid writer string")}};return string(raw),nil}
func parseWriterAttempt(original []byte)(out writerAttempt,result error){
 // u32 protocol=3, B32 nonce/receipt/plan, S destination, u32 count,
 // ordered(S path,u8 mode,u32 byteCount,raw bytes). No optional/trailing data.
 if len(original)<109||len(original)>768000{return out,fmt.Errorf("writer input quota exceeded")};c:=writerCursor{raw:original};version,e:=c.u32();if e!=nil||version!=3{return out,fmt.Errorf("writer version mismatch")}
 for _,target:=range []*[32]byte{&out.nonce,&out.readReceipt,&out.planDigest}{raw,e:=c.take(32);if e!=nil{return out,e};copy(target[:],raw)}
 if out.nonce==[32]byte{}||out.readReceipt==[32]byte{}||out.planDigest==[32]byte{}{return out,fmt.Errorf("invalid writer context")}
 out.destination,e=c.text(4096);if e!=nil{return out,e};out.count,e=c.u32();if e!=nil||out.count<1||out.count>128{return out,fmt.Errorf("invalid writer count")}
 for i:=uint32(0);i<out.count;i++{
  path,e:=c.text(240);if e!=nil{return out,e};mode,e:=c.take(1);if e!=nil||mode[0]>1{return out,fmt.Errorf("invalid writer mode")};n,e:=c.u32();if e!=nil||n>768000{return out,fmt.Errorf("invalid writer byte count")};bytes,e:=c.take(int(n));if e!=nil{return out,e};m:=os.FileMode(0644);if mode[0]==1{m=0755};out.entries[i]=entry{path:path,mode:m,bytes:bytes}
 }
 if c.at!=len(original){return out,fmt.Errorf("trailing writer input")};if e=validateWriterEntries(out.destination,out.entries[:out.count]);e!=nil{return out,e};out.original=original;return out,nil
}
