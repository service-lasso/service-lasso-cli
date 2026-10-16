//go:build linux || windows

package hostadmission

import (
 "crypto/hmac"
 "crypto/sha256"
 "encoding/binary"
 "hash"
 "io"
 "strings"
 "unicode/utf8"
)

const (
 HostDomain = "SLCLI-HOST-ADMISSION-2\x00"
 SnapshotFragmentDomain = "SLCLI-HOST-SNAPSHOT-FRAGMENT-3\x00"
 LaunchDomain = "SLCLI-HOST-LAUNCH-3\x00"
 InspectionSEADomain = "SLCLI-INSPECTION-SEA-1\x00"
 InspectionPrimaryDomain = "SLCLI-INSPECTION-PRIMARY-1\x00"
 InspectionLibraryDomain = "SLCLI-INSPECTION-LIBRARY-1\x00"
)
type Cursor struct { Raw []byte; Offset int }
func(c *Cursor) Take(n int)([]byte,error){if c==nil||c.Offset<0||c.Offset>len(c.Raw)||n<0||n>len(c.Raw)-c.Offset{return nil,ErrInvalid};out:=c.Raw[c.Offset:c.Offset+n];c.Offset+=n;return out,nil}
func(c *Cursor) U8()(byte,error){raw,e:=c.Take(1);if e!=nil{return 0,e};return raw[0],nil}
func(c *Cursor) U32()(uint32,error){raw,e:=c.Take(4);if e!=nil{return 0,e};return binary.BigEndian.Uint32(raw),nil}
func(c *Cursor) U64()(uint64,error){raw,e:=c.Take(8);if e!=nil{return 0,e};return binary.BigEndian.Uint64(raw),nil}
func(c *Cursor) B32()([32]byte,error){var out [32]byte;raw,e:=c.Take(32);if e!=nil{return out,e};copy(out[:],raw);return out,nil}
func(c *Cursor) S(maximum uint32,empty bool)(string,error){n,e:=c.U32();if e!=nil||n>maximum||(!empty&&n==0){return "",ErrInvalid};raw,e:=c.Take(int(n));if e!=nil||!utf8.Valid(raw)||strings.IndexByte(string(raw),0)>=0{return "",ErrInvalid};return string(raw),nil}
func(c *Cursor) End()error{if c.Offset!=len(c.Raw){return ErrInvalid};return nil}
func U32(value uint32)[4]byte{var raw [4]byte;binary.BigEndian.PutUint32(raw[:],value);return raw}
func U64(value uint64)[8]byte{var raw [8]byte;binary.BigEndian.PutUint64(raw[:],value);return raw}
func DomainHash(domain string,parts ...[]byte)[32]byte{hash:=sha256.New();hash.Write([]byte(domain));for _,part:=range parts{hash.Write(part)};var out [32]byte;copy(out[:],hash.Sum(nil));return out}

type Frame struct { Body [FrameBytes]byte; Length uint32 }
func(f *Frame) Bytes()[]byte{return f.Body[:f.Length]}
func(f *Frame) Operation()byte{return f.Body[0]}
func(f *Frame) Sequence()uint32{return binary.BigEndian.Uint32(f.Body[1:5])}
func MAC(domain string,direction byte,nonce,capability [32]byte,sequence uint32,body []byte)[32]byte{
 hash:=hmac.New(sha256.New,capability[:]);hash.Write([]byte(domain));hash.Write([]byte{direction});hash.Write(nonce[:]);seq:=U32(sequence);length:=U32(uint32(len(body)));hash.Write(seq[:]);hash.Write(length[:]);hash.Write(body);var out [32]byte;copy(out[:],hash.Sum(nil));return out
}
// Frame storage is owned/pre-reserved by the binding. No attacker length is used
// to allocate a new slice and no read is performed while holding the ledger lock.
func ReadFrame(r io.Reader,frame *Frame,domain string,direction byte,nonce,capability [32]byte,sequence uint32,caps map[byte]uint32)error{
 if r==nil||frame==nil||direction>1||nonce==([32]byte{})||capability==([32]byte{}){return ErrInvalid};frame.Length=0
 var length [4]byte;if _,e:=io.ReadFull(r,length[:]);e!=nil{return e};n:=binary.BigEndian.Uint32(length[:]);if n<5||n>FrameBytes{return ErrInvalid}
 if _,e:=io.ReadFull(r,frame.Body[:5]);e!=nil{return e};cap,ok:=caps[frame.Body[0]];if !ok||n>cap||binary.BigEndian.Uint32(frame.Body[1:5])!=sequence{return ErrInvalid}
 if _,e:=io.ReadFull(r,frame.Body[5:n]);e!=nil{return e};var actual [32]byte;if _,e:=io.ReadFull(r,actual[:]);e!=nil{return e};expected:=MAC(domain,direction,nonce,capability,sequence,frame.Body[:n]);if !hmac.Equal(actual[:],expected[:]){return ErrInvalid};frame.Length=n;return nil
}
func WriteFrame(w io.Writer,frame *Frame,domain string,direction byte,nonce,capability [32]byte,sequence uint32,cap uint32)error{
 if w==nil||frame==nil||direction>1||nonce==([32]byte{})||capability==([32]byte{}){return ErrInvalid}
 if frame.Length<5||frame.Length>FrameBytes||frame.Length>cap||frame.Sequence()!=sequence{return ErrInvalid};length:=U32(frame.Length);mac:=MAC(domain,direction,nonce,capability,sequence,frame.Bytes());for _,part:=range [][]byte{length[:],frame.Bytes(),mac[:]}{for len(part)>0{n,e:=w.Write(part);if e!=nil{return e};if n<=0||n>len(part){return io.ErrShortWrite};part=part[n:]}};return nil
}

type LaunchRequest struct { Mode byte; InvocationNonce [32]byte; InputRoot,CatalogSelector,Destination string; ID,Name *string }
func ParseLaunch(frame *Frame)(LaunchRequest,error){var out LaunchRequest;if frame==nil||frame.Length<5||frame.Length>FrameBytes{return out,ErrInvalid};c:=Cursor{Raw:frame.Bytes()};op,e:=c.U8();seq,se:=c.U32();if e!=nil||se!=nil||op!=1||seq!=0{return out,ErrInvalid};out.Mode,e=c.U8();if e!=nil||out.Mode>1{return out,ErrInvalid};out.InvocationNonce,e=c.B32();if e!=nil||out.InvocationNonce==([32]byte{}){return out,ErrInvalid};out.InputRoot,e=c.S(4096,false);if e!=nil{return out,e};out.CatalogSelector,e=c.S(256,false);if e!=nil{return out,e};out.Destination,e=c.S(4096,out.Mode==1);if e!=nil||out.Mode==1&&out.Destination!=""{return out,ErrInvalid};out.ID,e=parseOptional(&c,63);if e!=nil{return out,e};out.Name,e=parseOptional(&c,120);if e!=nil{return out,e};if out.Mode==1&&(out.ID!=nil||out.Name!=nil){return out,ErrInvalid};return out,c.End()}
func parseOptional(c *Cursor,maximum uint32)(*string,error){flag,e:=c.U8();if e!=nil||flag>1{return nil,ErrInvalid};if flag==0{return nil,nil};value,e:=c.S(maximum,false);if e!=nil{return nil,e};return &value,nil}
type LibraryAction struct { RunIdentity,Token [32]byte; Action byte; Destination string; ID,Name *string }
func ParseAction(frame *Frame)(LibraryAction,error){var out LibraryAction;if frame==nil||frame.Length<5||frame.Length>FrameBytes{return out,ErrInvalid};c:=Cursor{Raw:frame.Bytes()};op,e:=c.U8();seq,se:=c.U32();if e!=nil||se!=nil||op!=3||seq!=1{return out,ErrInvalid};out.RunIdentity,e=c.B32();if e!=nil||out.RunIdentity==([32]byte{}){return out,ErrInvalid};out.Token,e=c.B32();if e!=nil||out.Token==([32]byte{}){return out,ErrInvalid};out.Action,e=c.U8();if e!=nil||out.Action>2{return out,ErrInvalid};out.Destination,e=c.S(4096,out.Action!=0);if e!=nil{return out,e};out.ID,e=parseOptional(&c,63);if e!=nil{return out,e};out.Name,e=parseOptional(&c,120);if e!=nil{return out,e};if out.Action!=0&&out.Destination!=""||out.Action==2&&(out.ID!=nil||out.Name!=nil){return out,ErrInvalid};return out,c.End()}

type InspectionForward struct { Run,LaunchNonce,Receipt,Catalog,Digest [32]byte; Length,Count,Total,Offset,Sequence uint32; begun,ended,eofObserved,failed bool; recordHash hash.Hash }
// Streaming hops retain the exact original record bytes and source frames in
// their reserved capture sinks. They do not assemble or parse owner semantics.
func(s *InspectionForward) Validate(frame *Frame)(result error){
 if s==nil{return ErrInvalid};defer func(){if result!=nil{s.failed=true}}();if s.failed||s.Run==([32]byte{})||s.LaunchNonce==([32]byte{})||s.Receipt==([32]byte{})||s.Catalog==([32]byte{})||frame==nil||frame.Length<5||frame.Length>FrameBytes{return ErrInvalid}
 c:=Cursor{Raw:frame.Bytes()};op,e:=c.U8();seq,se:=c.U32();if e!=nil||se!=nil||s.ended||seq!=s.Sequence{return ErrInvalid}
 switch op{
 case 1:if s.begun||seq!=0{return ErrInvalid};run,e:=c.B32();if e!=nil||run!=s.Run{return ErrInvalid};nonce,e:=c.B32();if e!=nil||nonce!=s.LaunchNonce{return ErrInvalid};receipt,e:=c.B32();if e!=nil||receipt!=s.Receipt{return ErrInvalid};catalog,e:=c.B32();if e!=nil||catalog!=s.Catalog{return ErrInvalid};s.Digest,e=c.B32();if e!=nil||s.Digest==([32]byte{}){return ErrInvalid};s.Length,e=c.U32();if e!=nil||s.Length<133||s.Length>768000{return ErrInvalid};s.Count,e=c.U32();if e!=nil||s.Count<1||s.Count>128{return ErrInvalid};s.Total,e=c.U32();if e!=nil||s.Total>768000{return ErrInvalid};s.recordHash=sha256.New();s.recordHash.Write([]byte("SLCLI-INSPECTION-RECORD-1\x00"));s.begun=true
 case 2:if !s.begun||seq>1024{return ErrInvalid};run,e:=c.B32();if e!=nil||run!=s.Run{return ErrInvalid};digest,e:=c.B32();if e!=nil||digest!=s.Digest{return ErrInvalid};offset,e:=c.U32();if e!=nil||offset!=s.Offset{return ErrInvalid};length,e:=c.U32();if e!=nil||length==0||length>16307||length>s.Length-s.Offset{return ErrInvalid};chunk,e:=c.Take(int(length));if e!=nil{return e};s.recordHash.Write(chunk);s.Offset+=length
 case 3:if !s.begun||s.Offset!=s.Length{return ErrInvalid};run,e:=c.B32();if e!=nil||run!=s.Run{return ErrInvalid};digest,e:=c.B32();if e!=nil||digest!=s.Digest{return ErrInvalid};length,e:=c.U32();if e!=nil||length!=s.Length{return ErrInvalid};count,e:=c.U32();if e!=nil||count!=s.Count{return ErrInvalid};total,e:=c.U32();if e!=nil||total!=s.Total{return ErrInvalid};if !hmac.Equal(s.recordHash.Sum(nil),s.Digest[:]){return ErrInvalid};s.ended=true
 default:return ErrInvalid
 };if e=c.End();e!=nil{return e};s.Sequence++;return nil
}
// EOF is delivered only by the retained original native reader after actual
// zero-byte completion. END does not stand in for physical channel closure.
func(s *InspectionForward) EOF()error{if s==nil{return ErrInvalid};if s.failed||!s.ended||s.eofObserved{s.failed=true;return ErrInvalid};s.eofObserved=true;return nil}
