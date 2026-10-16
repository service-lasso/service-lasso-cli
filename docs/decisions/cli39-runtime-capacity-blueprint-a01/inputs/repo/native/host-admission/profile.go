//go:build linux || windows

package hostadmission

import (
 "bytes"
 "crypto/sha256"
 "encoding/hex"
 "encoding/json"
 "errors"
 "io"
 "runtime"
 "strconv"
 "strings"
 "unicode/utf8"
)

var ErrUnavailable = errors.New("admission_unavailable")
var ErrInvalid = errors.New("invalid_resource")

const (
 SessionBytes uint64 = 8388608
 ControlBytes uint64 = 1048576
 StoreBytes uint64 = 4194304
 MaximumBindings = 16
 MaximumReferences = 128
 MaximumCopySets = 128
 FrameBytes = 16384
)

type Endpoint struct { Kind, Name string }
type Source struct { Repository, Commit, ImageSHA256, SourceSHA256 string }
type ImageBinding struct { Repository, Commit, Blob, RawSHA256, ImageSHA256 string }
type LibraryBinding struct { Repository, Commit, Blob, RawSHA256, NodeImageSHA256, PackageSourceSHA256, InheritedChannelKind string }
type LaunchProfile struct {
 Endpoint Endpoint
 Primary, Observer, Launcher, Facade ImageBinding
 Library LibraryBinding
 MaximumRequest, MaximumResult, MaximumStdout, MaximumStderr, MaximumCapture uint64
}
type Profile struct {
 Platform string
 LibraryFacadeSourceSHA256 string
 Service Source
 Endpoint Endpoint
 Owner string
 StoreProvider, StoreRootIdentity, KeyVersion string
 RetentionProvider, RetentionRootIdentity string
 RetentionMaximum, RetentionRecordMaximum uint64
 Launch LaunchProfile
 Raw []byte
 Digest [32]byte
 seal *profileSeal
}
type profileSeal struct{public,original *Profile}
type sourceProfile struct { raw []byte; digest [32]byte }
// This is populated only by a separately reviewed immutable source admission.
// Neither a caller profile, environment value nor store initialization enrolls
// an entry. Absence grants no production capability and does not establish
// completion of the production source or native qualification.
var admittedProfiles = []sourceProfile{}

func LoadProfile() (*Profile,error) {
 platform := runtime.GOOS; if platform == "windows" { platform = "win32" }
 var selected *Profile
 for _, admitted := range admittedProfiles {
  if len(admitted.raw)==0 || len(admitted.raw)>FrameBytes || sha256.Sum256(admitted.raw)!=admitted.digest { return nil,ErrUnavailable }
  // Never expose or borrow the mutable source catalogue's raw slice as a
  // returned public view. The private original and public bytes are distinct.
  originalRaw:=append([]byte(nil),admitted.raw...)
  p,err:=parseProfile(originalRaw);if err!=nil{return nil,ErrUnavailable}
  if p.Platform!=platform { continue }
  if selected!=nil{return nil,ErrUnavailable};public:=*p;public.Raw=append([]byte(nil),originalRaw...);seal:=&profileSeal{original:p,public:&public};p.seal=seal;public.seal=seal;selected=&public
 }
 if selected==nil{return nil,ErrUnavailable};return selected,nil
}
// Exact original association only. Clones/forgeries and changed projections do
// not select authority. Owning routes use the retained private original after
// this check, so later projection writes cannot alter an active native grant.
func originalProfile(p *Profile)(*Profile,error){
 if p==nil||p.seal==nil||p!=p.seal.public&&p!=p.seal.original{return nil,ErrUnavailable};original:=p.seal.original
 if original==nil||sha256.Sum256(original.Raw)!=original.Digest||p.Digest!=original.Digest||!bytes.Equal(p.Raw,original.Raw)||p.Platform!=original.Platform||p.LibraryFacadeSourceSHA256!=original.LibraryFacadeSourceSHA256||p.Service!=original.Service||p.Endpoint!=original.Endpoint||p.Owner!=original.Owner||p.StoreProvider!=original.StoreProvider||p.StoreRootIdentity!=original.StoreRootIdentity||p.KeyVersion!=original.KeyVersion||p.RetentionProvider!=original.RetentionProvider||p.RetentionRootIdentity!=original.RetentionRootIdentity||p.RetentionMaximum!=original.RetentionMaximum||p.RetentionRecordMaximum!=original.RetentionRecordMaximum||p.Launch!=original.Launch{return nil,ErrUnavailable};return original,nil
}
func object(v any, keys ...string)(map[string]any,error){
 row,ok:=v.(map[string]any);if !ok||len(row)!=len(keys){return nil,ErrInvalid}
 for _,key:=range keys{if _,ok=row[key];!ok{return nil,ErrInvalid}};return row,nil
}
func text(v any, cap int)(string,error){s,ok:=v.(string);if !ok||len(s)==0||len(s)>cap||!utf8.ValidString(s){return "",ErrInvalid};for _,c:=range s{if c<32||c==127{return "",ErrInvalid}};return s,nil}
func digestText(v any, count int)(string,error){s,e:=text(v,count);if e!=nil||len(s)!=count{return "",ErrInvalid};for _,c:=range s{if !(c>='0'&&c<='9'||c>='a'&&c<='f'){return "",ErrInvalid}};return s,nil}
func integer(v any, maximum uint64)(uint64,error){n,ok:=v.(json.Number);if !ok{return 0,ErrInvalid};s:=string(n);if strings.ContainsAny(s,".eE+-"){return 0,ErrInvalid};value,e:=strconv.ParseUint(s,10,64);if e!=nil||value==0||value>maximum{return 0,ErrInvalid};return value,nil}
func equalInteger(v any, expected uint64)error{value,e:=integer(v,expected);if e!=nil||value!=expected{return ErrInvalid};return nil}
func decodeStrict(raw []byte)(any,error){return decodeStrictLimit(raw,FrameBytes)}
func decodeLedgerStrict(raw []byte)(any,error){return decodeStrictLimit(raw,LedgerPayloadCap)}
func decodeStrictLimit(raw []byte,maximum int)(any,error){
 if len(raw)==0||len(raw)>maximum||!utf8.Valid(raw)||!validUnicodeEscapes(raw){return nil,ErrInvalid}
 d:=json.NewDecoder(bytes.NewReader(raw));d.UseNumber();members:=0;tokens:=0
 var read func(int)(any,error)
 read=func(depth int)(any,error){
  tokens++;if depth>16||tokens>65536{return nil,ErrInvalid};token,e:=d.Token();if e!=nil{return nil,ErrInvalid}
  delim,container:=token.(json.Delim);if !container{return token,nil}
  switch delim{
  case '{':row:=map[string]any{};for d.More(){members++;if members>4096{return nil,ErrInvalid};key,e:=d.Token();s,ok:=key.(string);if e!=nil||!ok{return nil,ErrInvalid};if _,exists:=row[s];exists{return nil,ErrInvalid};value,e:=read(depth+1);if e!=nil{return nil,e};row[s]=value};end,e:=d.Token();if e!=nil||end!=json.Delim('}'){return nil,ErrInvalid};return row,nil
  case '[':rows:=[]any{};for d.More(){if len(rows)>=4096{return nil,ErrInvalid};value,e:=read(depth+1);if e!=nil{return nil,e};rows=append(rows,value)};end,e:=d.Token();if e!=nil||end!=json.Delim(']'){return nil,ErrInvalid};return rows,nil
  default:return nil,ErrInvalid
  }
 }
 value,e:=read(0);if e!=nil{return nil,e};if _,e=d.Token();e!=io.EOF{return nil,ErrInvalid};return value,nil
}
func validUnicodeEscapes(raw []byte)bool{
 quoted:=false
 hex4:=func(at int)(uint16,bool){if at+4>len(raw){return 0,false};var out uint16;for _,b:=range raw[at:at+4]{out<<=4;switch{case b>='0'&&b<='9':out|=uint16(b-'0');case b>='a'&&b<='f':out|=uint16(b-'a'+10);case b>='A'&&b<='F':out|=uint16(b-'A'+10);default:return 0,false}};return out,true}
 for i:=0;i<len(raw);i++{if raw[i]=='"'{quoted=!quoted;continue};if !quoted||raw[i]!='\\'{continue};i++;if i>=len(raw){return false};if raw[i]!='u'{continue};value,ok:=hex4(i+1);if !ok{return false};i+=4
  if value>=0xdc00&&value<=0xdfff{return false};if value>=0xd800&&value<=0xdbff{if i+6>=len(raw)||raw[i+1]!='\\'||raw[i+2]!='u'{return false};low,ok:=hex4(i+3);if !ok||low<0xdc00||low>0xdfff{return false};i+=6}
 };return !quoted
}
func endpoint(v any, platform string)(Endpoint,error){
 var out Endpoint;row,e:=object(v,"kind","name");if e!=nil{return out,e};out.Kind,e=text(row["kind"],32);if e!=nil{return out,e};out.Name,e=text(row["name"],4096);if e!=nil{return out,e}
 if platform=="linux" {if out.Kind!="unix-socket"||!strings.HasPrefix(out.Name,"/")||strings.Contains(out.Name,"//")||strings.ContainsAny(out.Name,"\\\x00"){return out,ErrInvalid};for _,part:=range strings.Split(out.Name,"/")[1:]{if part==""||part=="."||part==".."{return out,ErrInvalid}}} else if platform=="win32" {if out.Kind!="named-pipe"||!strings.HasPrefix(out.Name,`\\.\pipe\`)||strings.Contains(out.Name[9:],`\`)||len(out.Name)<=9{return out,ErrInvalid}}else{return out,ErrInvalid}
 return out,nil
}
func image(v any)(ImageBinding,error){var out ImageBinding;row,e:=object(v,"repository","commit","blob","rawSha256","imageSha256");if e!=nil{return out,e};out.Repository,e=text(row["repository"],256);if e!=nil{return out,e};out.Commit,e=digestText(row["commit"],40);if e!=nil{return out,e};out.Blob,e=digestText(row["blob"],40);if e!=nil{return out,e};out.RawSHA256,e=digestText(row["rawSha256"],64);if e!=nil{return out,e};out.ImageSHA256,e=digestText(row["imageSha256"],64);return out,e}
func parseProfile(raw []byte)(*Profile,error){
 value,e:=decodeStrict(raw);if e!=nil{return nil,e};row,e:=object(value,"schema","platform","serviceSource","endpoint","owner","store","clientImages","control","retention","launch");if e!=nil{return nil,e}
 if row["schema"]!="service-lasso.cli-host-profile.v4"{return nil,ErrInvalid};p:=&Profile{Raw:raw,Digest:sha256.Sum256(raw)};p.Platform,e=text(row["platform"],8);if e!=nil{return nil,e};p.Endpoint,e=endpoint(row["endpoint"],p.Platform);if e!=nil{return nil,e}
 service,e:=object(row["serviceSource"],"repository","commit","imageSha256","sourceSha256");if e!=nil{return nil,e};p.Service.Repository,e=text(service["repository"],256);if e!=nil{return nil,e};p.Service.Commit,e=digestText(service["commit"],40);if e!=nil{return nil,e};p.Service.ImageSHA256,e=digestText(service["imageSha256"],64);if e!=nil{return nil,e};p.Service.SourceSHA256,e=digestText(service["sourceSha256"],64);if e!=nil{return nil,e}
 owner,e:=object(row["owner"],"nativeId");if e!=nil{return nil,e};p.Owner,e=text(owner["nativeId"],184);if e!=nil{return nil,e};if p.Platform=="linux"{if _,e=strconv.ParseUint(p.Owner,10,32);e!=nil{return nil,ErrInvalid}}else if !strings.HasPrefix(p.Owner,"S-1-"){return nil,ErrInvalid}
 store,e:=object(row["store"],"provider","rootIdentity","keyVersion","maximumBytes");if e!=nil{return nil,e};p.StoreProvider,e=text(store["provider"],64);if e!=nil{return nil,e};p.StoreRootIdentity,e=digestText(store["rootIdentity"],64);if e!=nil{return nil,e};p.KeyVersion,e=text(store["keyVersion"],64);if e!=nil||equalInteger(store["maximumBytes"],StoreBytes)!=nil{return nil,ErrInvalid}
 control,e:=object(row["control"],"maximumBindings","maximumReferencesPerBinding","maximumServiceBytes");if e!=nil||equalInteger(control["maximumBindings"],MaximumBindings)!=nil||equalInteger(control["maximumReferencesPerBinding"],MaximumReferences)!=nil||equalInteger(control["maximumServiceBytes"],ControlBytes)!=nil{return nil,ErrInvalid}
 retention,e:=object(row["retention"],"provider","rootIdentity","maximumBytes","maximumRecordBytes","minimumDays");if e!=nil||equalInteger(retention["minimumDays"],90)!=nil{return nil,ErrInvalid};p.RetentionProvider,e=text(retention["provider"],64);if e!=nil{return nil,e};p.RetentionRootIdentity,e=digestText(retention["rootIdentity"],64);if e!=nil{return nil,e};p.RetentionMaximum,e=integer(retention["maximumBytes"],9007199254740991);if e!=nil{return nil,e};p.RetentionRecordMaximum,e=integer(retention["maximumRecordBytes"],p.RetentionMaximum);if e!=nil{return nil,e}
 launch,e:=object(row["launch"],"endpoint","primaryImageBinding","observerImageBinding","launcherImageBinding","libraryTransportBinding","maximumRequestBytes","maximumResultBytes","maximumStdoutBytes","maximumStderrBytes","maximumAggregateCaptureBytes","clientCopies","facadeImageBinding");if e!=nil{return nil,e};p.Launch.Endpoint,e=endpoint(launch["endpoint"],p.Platform);if e!=nil||p.Launch.Endpoint.Name==p.Endpoint.Name{return nil,ErrInvalid};p.Launch.Primary,e=image(launch["primaryImageBinding"]);if e!=nil{return nil,e};p.Launch.Observer,e=image(launch["observerImageBinding"]);if e!=nil{return nil,e};p.Launch.Launcher,e=image(launch["launcherImageBinding"]);if e!=nil{return nil,e};p.Launch.Facade,e=image(launch["facadeImageBinding"]);if e!=nil||p.Launch.Facade.RawSHA256==strings.Repeat("0",64)||p.Launch.Facade.ImageSHA256==strings.Repeat("0",64){return nil,ErrInvalid}
 library,e:=object(launch["libraryTransportBinding"],"repository","commit","blob","rawSha256","nodeImageSha256","packageSourceSha256","inheritedChannelKind");if e!=nil{return nil,e};p.Launch.Library.Repository,e=text(library["repository"],256);if e!=nil{return nil,e};p.Launch.Library.Commit,e=digestText(library["commit"],40);if e!=nil{return nil,e};p.Launch.Library.Blob,e=digestText(library["blob"],40);if e!=nil{return nil,e};p.Launch.Library.RawSHA256,e=digestText(library["rawSha256"],64);if e!=nil{return nil,e};p.Launch.Library.NodeImageSHA256,e=digestText(library["nodeImageSha256"],64);if e!=nil{return nil,e};p.Launch.Library.PackageSourceSHA256,e=digestText(library["packageSourceSha256"],64);if e!=nil{return nil,e};p.Launch.Library.InheritedChannelKind,e=text(library["inheritedChannelKind"],64);if e!=nil{return nil,e}
 copies,e:=object(launch["clientCopies"],"maximumBytes","maximumCopySets","release");if e!=nil||equalInteger(copies["maximumBytes"],SessionBytes)!=nil||equalInteger(copies["maximumCopySets"],MaximumCopySets)!=nil||copies["release"]!="original-parent-exit"{return nil,ErrInvalid}
 p.Launch.MaximumRequest,e=integer(launch["maximumRequestBytes"],FrameBytes);if e!=nil{return nil,e};p.Launch.MaximumResult,e=integer(launch["maximumResultBytes"],FrameBytes);if e!=nil{return nil,e};p.Launch.MaximumCapture,e=integer(launch["maximumAggregateCaptureBytes"],p.RetentionRecordMaximum);if e!=nil{return nil,e};p.Launch.MaximumStdout,e=integer(launch["maximumStdoutBytes"],p.Launch.MaximumCapture);if e!=nil{return nil,e};p.Launch.MaximumStderr,e=integer(launch["maximumStderrBytes"],p.Launch.MaximumCapture);if e!=nil{return nil,e}
 images,e:=object(row["clientImages"],"primarySha256","observerSha256","launcherSha256","libraryFacadeSourceSha256","facadeSha256");if e!=nil{return nil,e};for key,expected:=range map[string]string{"primarySha256":p.Launch.Primary.ImageSHA256,"observerSha256":p.Launch.Observer.ImageSHA256,"launcherSha256":p.Launch.Launcher.ImageSHA256,"facadeSha256":p.Launch.Facade.ImageSHA256}{value,e:=digestText(images[key],64);if e!=nil||value!=expected{return nil,ErrInvalid}};p.LibraryFacadeSourceSHA256,e=digestText(images["libraryFacadeSourceSha256"],64);if e!=nil{return nil,e}
 return p,nil
}
func ParseDigest(s string)([32]byte,error){var out [32]byte;raw,e:=hex.DecodeString(s);if e!=nil||len(raw)!=32||strings.ToLower(s)!=s{return out,ErrInvalid};copy(out[:],raw);return out,nil}
