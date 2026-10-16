//go:build linux || windows || darwin

package main

import (
 "encoding/binary"
 "io"
)

// Encode directly into the original pre-reserved command region. The writer
// borrows final PLAN bytes only; source handles, capability and inspection
// control never cross this boundary.
func encodeWriterCommand(destination []byte,nonce [32]byte,plan nativePlan)(int,error){
 if len(destination)<768000||nonce==[32]byte{}||plan.receipt==[32]byte{}||plan.digest==[32]byte{}||len(plan.entries)<1||len(plan.entries)>128{return 0,errNativeProtocol}
 needed:=uint64(108+len(plan.destination))
 for _,entry:=range plan.entries{
  if !validNativePath(entry.path)||entry.mode!=0644&&entry.mode!=0755{return 0,errNativeProtocol}
  needed+=uint64(9+len(entry.path)+len(entry.bytes));if needed>768000{return 0,errNativeCapacity}
 }
 if needed>768000{return 0,errNativeCapacity}
 at:=0
 put32:=func(n uint32){binary.BigEndian.PutUint32(destination[at:at+4],n);at+=4}
 put:=func(bytes []byte){copy(destination[at:],bytes);at+=len(bytes)}
 text:=func(value string){put32(uint32(len(value)));copy(destination[at:],value);at+=len(value)}
 put32(3);put(nonce[:]);put(plan.receipt[:]);put(plan.digest[:]);text(plan.destination);put32(uint32(len(plan.entries)))
 for _,entry:=range plan.entries{text(entry.path);mode:=byte(0);if entry.mode==0755{mode=1};destination[at]=mode;at++;put32(uint32(len(entry.bytes)));put(entry.bytes)}
 if uint64(at)!=needed{return 0,errNativeProtocol};return at,nil
}

// Retain every actual byte up to the fixed limit and deny overflow. This sink
// cannot grow in response to a child stream. Its owning original stream capture
// and native EOF evidence remain separate from this result consumer.
type writerResultBuffer struct{raw [64]byte;used int;limit int;failed bool}
func(b *writerResultBuffer)Write(bytes []byte)(int,error){
 if b==nil||b.failed||b.limit<0||b.limit>len(b.raw)||len(bytes)>b.limit-b.used{if b!=nil{b.failed=true};return 0,io.ErrShortBuffer}
 copy(b.raw[b.used:],bytes);b.used+=len(bytes);return len(bytes),nil
}
