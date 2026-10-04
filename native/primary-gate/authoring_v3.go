//go:build linux || windows

package main

import (
 "crypto/hmac"
 "crypto/rand"
 "crypto/sha256"
 "encoding/binary"
 "errors"
 "io"
 "os"
 "sort"
 "strings"
 "sync"
 "time"
 "unicode/utf8"
)

const nativeBudgetLimit = 8388608
var errNativeProtocol = errors.New("native protocol rejected")
var errNativeCapacity = errors.New("native capacity exhausted")
type nativeBudget struct { mu sync.Mutex; bytes uint64; peak uint64; retained bool }
func (b *nativeBudget) reserve(n uint64) error { b.mu.Lock(); defer b.mu.Unlock(); if n > nativeBudgetLimit-b.bytes { return errNativeCapacity }; b.bytes += n; if b.bytes>b.peak { b.peak=b.bytes }; return nil }
func (b *nativeBudget) release(n uint64) { b.mu.Lock(); defer b.mu.Unlock(); if n>b.bytes { b.retained=true; return }; b.bytes-=n }
func (b *nativeBudget) allocate(n uint32) ([]byte,error) { if err:=b.reserve(uint64(n)); err!=nil{return nil,err}; return make([]byte,n),nil }
type wireCursor struct { bytes []byte; offset int }
func (c *wireCursor) take(n int) ([]byte,error) { if n<0||n>len(c.bytes)-c.offset{return nil,errNativeProtocol}; out:=c.bytes[c.offset:c.offset+n]; c.offset+=n; return out,nil }
func (c *wireCursor) u8()(byte,error){b,e:=c.take(1);if e!=nil{return 0,e};return b[0],nil}
func (c *wireCursor) u32()(uint32,error){b,e:=c.take(4);if e!=nil{return 0,e};return binary.BigEndian.Uint32(b),nil}
func (c *wireCursor) string(max uint32,empty bool)(string,error){n,e:=c.u32();if e!=nil||n>max||(!empty&&n==0){return "",errNativeProtocol};b,e:=c.take(int(n));if e!=nil||!utf8.Valid(b)||strings.IndexByte(string(b),0)>=0{return "",errNativeProtocol};return string(b),nil}
func (c *wireCursor) end() error { if c.offset!=len(c.bytes){return errNativeProtocol};return nil }
func be32(n uint32) []byte {var b [4]byte;binary.BigEndian.PutUint32(b[:],n);return b[:]}
func wireString(s string) []byte {out:=make([]byte,4+len(s));binary.BigEndian.PutUint32(out,uint32(len(s)));copy(out[4:],s);return out}
func domainHash(domain string,parts ...[]byte) [32]byte {h:=sha256.New();h.Write([]byte(domain));for _,p:=range parts{h.Write(p)};var out [32]byte;copy(out[:],h.Sum(nil));return out}
func wireMAC(domain string,direction byte,nonce,capability []byte,sequence uint32,body []byte) []byte {h:=hmac.New(sha256.New,capability);h.Write([]byte(domain));h.Write([]byte{direction});h.Write(nonce);h.Write(be32(sequence));h.Write(be32(uint32(len(body))));h.Write(body);return h.Sum(nil)}
// Read five fixed header bytes first. The source-owned expected operation and
// phase cap decide the allocation, never a payload status or remote claim.
func readNativeFrame(r io.Reader,domain string,direction byte,nonce,capability []byte,sequence uint32,caps map[byte]uint32,budget *nativeBudget)([]byte,error){
 var length [4]byte;if _,err:=io.ReadFull(r,length[:]);err!=nil{return nil,err};n:=binary.BigEndian.Uint32(length[:]);if n<5{return nil,errNativeProtocol}
 var header [5]byte;if _,err:=io.ReadFull(r,header[:]);err!=nil{return nil,err};cap,ok:=caps[header[0]];if !ok||n>cap||binary.BigEndian.Uint32(header[1:])!=sequence{return nil,errNativeProtocol}
 body,err:=budget.allocate(n);if err!=nil{return nil,err};copy(body,header[:]);if _,err=io.ReadFull(r,body[5:]);err!=nil{budget.retained=true;return nil,err};var mac [32]byte;if _,err=io.ReadFull(r,mac[:]);err!=nil{budget.retained=true;return nil,err};if !hmac.Equal(mac[:],wireMAC(domain,direction,nonce,capability,sequence,body)){budget.release(uint64(n));return nil,errNativeProtocol};return body,nil
}
func writeNativeFrame(w io.Writer,domain string,direction byte,nonce,capability []byte,sequence uint32,body []byte,cap uint32)error{
 if len(body)<5||uint32(len(body))>cap||binary.BigEndian.Uint32(body[1:5])!=sequence{return errNativeProtocol}
 for _,p:=range [][]byte{be32(uint32(len(body))),body,wireMAC(domain,direction,nonce,capability,sequence,body)}{for len(p)>0{n,e:=w.Write(p);if e!=nil{return e};if n==0{return io.ErrShortWrite};p=p[n:]}};return nil
}
var nativeAssetNames=[]string{"service-template.tar.gz","template-candidate.json","template-contract.json","SHA256SUMS"}
var nativeAssetLimits=[]uint32{262144,1024,32768,512}
type nativeAsset struct { name byte; bytes []byte; identity [32]byte; digest [32]byte }
type nativeAdmission struct { catalogIdentity string; sizes [4]uint32; hashes [4][32]byte; maximumFiles uint32; maximumTotal uint32; source [20]byte }
// Actual catalog pins are embedded by a later separately reviewed source
// admission. Empty production catalog grants no tuple and causes no asset IO.
var nativeAdmissions=[]nativeAdmission{}
func readOriginalAssets(root string,admission nativeAdmission,budget *nativeBudget)([]nativeAsset,error){
 held,err:=openNativeInputRoot(root);if err!=nil{return nil,err};defer held.close()
 out:=make([]nativeAsset,0,4)
 for i,name:=range nativeAssetNames {
  if admission.sizes[i]==0||admission.sizes[i]>nativeAssetLimits[i]{return nil,errNativeProtocol}
  leaf,e:=held.open(name,admission.sizes[i]);if e!=nil{return nil,e}
  body,e:=budget.allocate(admission.sizes[i]);if e!=nil{leaf.close();return nil,e}
  identity,e:=leaf.readExact(body);closeErr:=leaf.close();if e!=nil||closeErr!=nil{budget.retained=true;if e!=nil{return nil,e};return nil,closeErr}
  digest:=sha256.Sum256(body);if digest!=admission.hashes[i]{return nil,errNativeProtocol};out=append(out,nativeAsset{byte(i),body,identity,digest})
 };return out,nil
}
func readReceipt(nonce []byte,assets []nativeAsset)[32]byte {h:=sha256.New();h.Write([]byte("SLCLI-READ-3\x00"));h.Write(nonce);for _,asset:=range assets{h.Write([]byte{asset.name});h.Write(be32(uint32(len(asset.bytes))));h.Write(asset.bytes)};var out [32]byte;copy(out[:],h.Sum(nil));return out}
type entry struct { path string; mode os.FileMode; bytes []byte }
type nativePlan struct { destination,id string; name *string; entries []entry; receipt, digest, expectedReadback [32]byte }
func parseNativePlan(body,nonce []byte,receipt [32]byte,admission nativeAdmission)(nativePlan,error){
 var p nativePlan;c:=wireCursor{bytes:body};op,e:=c.u8();seq,se:=c.u32();if e!=nil||se!=nil||op!=4||seq!=1{return p,errNativeProtocol};r,e:=c.take(32);if e!=nil||!hmac.Equal(r,receipt[:]){return p,errNativeProtocol};payloadStart:=c.offset
 p.destination,e=c.string(4096,false);if e!=nil{return p,e};p.id,e=c.string(63,false);if e!=nil||!validNativeID(p.id){return p,errNativeProtocol};hasName,e:=c.u8();if e!=nil||hasName>1{return p,errNativeProtocol};if hasName==1{name,e:=c.string(120,false);if e!=nil||!validNativeName(name){return p,errNativeProtocol};p.name=&name}
 count,e:=c.u32();if e!=nil||count<1||count>128||count>admission.maximumFiles{return p,errNativeProtocol};p.entries=make([]entry,0,count);previous:="";var total uint32
 for i:=uint32(0);i<count;i++{path,e:=c.string(240,false);if e!=nil||!validNativePath(path)||path<=previous{return p,errNativeProtocol};previous=path;mode,e:=c.u8();if e!=nil||mode>1{return p,errNativeProtocol};n,e:=c.u32();if e!=nil||n>admission.maximumTotal-total{return p,errNativeProtocol};total+=n;bytes,e:=c.take(int(n));if e!=nil{return p,e};m:=uint32(0644);if mode==1{m=0755};p.entries=append(p.entries,entry{path:path,mode:os.FileMode(m),bytes:bytes})}
 if c.end()!=nil{return p,errNativeProtocol};p.receipt=receipt;p.digest=domainHash("SLCLI-PLAN-3\x00",nonce,receipt[:],body[payloadStart:]);p.expectedReadback=planReadbackDigest(nonce,p);return p,nil
}
func validNativeID(s string)bool{if len(s)<1||len(s)>63||s[0]<'a'||s[0]>'z'{return false};for _,c:=range s{if !(c>='a'&&c<='z'||c>='0'&&c<='9'||c=='-'){return false}};return true}
func validNativeName(s string)bool{if len(s)<1||len(s)>120||!((s[0]>='A'&&s[0]<='Z')||(s[0]>='a'&&s[0]<='z')||(s[0]>='0'&&s[0]<='9')){return false};for _,c:=range s{if !(c>='A'&&c<='Z'||c>='a'&&c<='z'||c>='0'&&c<='9'||strings.ContainsRune(" .,'()/_-",c)){return false}};return true}
func validNativePath(s string)bool{if len(s)<1||len(s)>240||strings.HasPrefix(s,"/")||strings.Contains(s,"\\")||len(strings.Split(s,"/"))>12{return false};for _,p:=range strings.Split(s,"/"){if p==""||p=="."||p==".."{return false};for _,c:=range p{if !(c>='A'&&c<='Z'||c>='a'&&c<='z'||c>='0'&&c<='9'||strings.ContainsRune("_.@-",c)){return false}}};return true}
func planReadbackDigest(nonce []byte,p nativePlan)[32]byte{h:=sha256.New();h.Write([]byte("SLCLI-READBACK-3\x00"));h.Write(nonce);h.Write(p.receipt[:]);h.Write(p.digest[:]);h.Write(be32(uint32(len(p.entries))));for _,e:=range p.entries{h.Write(wireString(e.path));m:=byte(0);if e.mode==0755{m=1};h.Write([]byte{m});h.Write(be32(uint32(len(e.bytes))));digest:=sha256.Sum256(e.bytes);h.Write(digest[:])};var out [32]byte;copy(out[:],h.Sum(nil));return out}
func nativeGreeting(w io.Writer,capability []byte,nonce [32]byte,version string,source [20]byte,helper [32]byte)error{
 body:=append([]byte{1,0,0,0,0,0,0,0,3},nonce[:]...);body=append(body,wireString(version)...);body=append(body,source[:]...);body=append(body,helper[:]...);if len(body)>512{return errNativeProtocol};mac:=hmac.New(sha256.New,capability);mac.Write([]byte("SLCLI-GREETING-3\x00"));mac.Write(body);for _,part:=range [][]byte{be32(uint32(len(body))),body,mac.Sum(nil)}{if _,err:=w.Write(part);err!=nil{return err}};return nil
}
func nativeNonce()([32]byte,error){var out [32]byte;_,err:=rand.Read(out[:]);return out,err}
func nativeDeadline(start time.Time,phase time.Duration)time.Time{deadline:=start.Add(40*time.Second);if d:=time.Now().Add(phase);d.Before(deadline){return d};return deadline}
func orderedEntries(entries []entry)bool{return sort.SliceIsSorted(entries,func(i,j int)bool{return entries[i].path<entries[j].path})}
