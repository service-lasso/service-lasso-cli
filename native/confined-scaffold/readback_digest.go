package main

import (
 "crypto/sha256"
 "encoding/binary"
 "fmt"
 "hash"
)

// One original writer attempt. Context is non-capability data supplied by the
// original primary; the primary must independently verify it against custody.
// Entries here are actual held-handle observations, never input hash echoes.
type writerReadbackDigest struct{
 hash hash.Hash
 expected uint32
 observed uint32
 previous string
 failed bool
 finalized bool
}

func newWriterReadbackDigest(nonce,receipt,plan [32]byte,count uint32)(*writerReadbackDigest,error){
 if nonce==[32]byte{}||receipt==[32]byte{}||plan==[32]byte{}||count<1||count>128{return nil,fmt.Errorf("invalid original readback context")}
 h:=sha256.New();h.Write([]byte("SLCLI-READBACK-3\x00"));h.Write(nonce[:]);h.Write(receipt[:]);h.Write(plan[:]);var raw [4]byte;binary.BigEndian.PutUint32(raw[:],count);h.Write(raw[:]);return &writerReadbackDigest{hash:h,expected:count},nil
}
func(d *writerReadbackDigest)add(path string,actual heldReadback)(result error){
 if d==nil{return fmt.Errorf("missing original readback digest")};defer func(){if result!=nil{d.failed=true}}()
 if d.failed||d.finalized||d.observed>=d.expected||!writerPortablePath(path)||path<=d.previous||actual.mode!=0644&&actual.mode!=0755||actual.bytes>768000{return fmt.Errorf("invalid actual readback record")}
 var raw [4]byte;binary.BigEndian.PutUint32(raw[:],uint32(len(path)));d.hash.Write(raw[:]);d.hash.Write([]byte(path));mode:=byte(0);if actual.mode==0755{mode=1};d.hash.Write([]byte{mode});binary.BigEndian.PutUint32(raw[:],actual.bytes);d.hash.Write(raw[:]);d.hash.Write(actual.sha256[:]);d.previous=path;d.observed++;return nil
}
func(d *writerReadbackDigest)finish()([32]byte,error){
 var out [32]byte;if d==nil{return out,fmt.Errorf("missing original readback digest")};if d.failed||d.finalized||d.observed!=d.expected{d.failed=true;return out,fmt.Errorf("incomplete original readback digest")};d.finalized=true;copy(out[:],d.hash.Sum(nil));return out,nil
}
