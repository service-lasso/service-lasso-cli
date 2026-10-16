//go:build linux || windows

package hostadmission

import (
 "strings"
 "unicode/utf8"
)

// Parsing is not enrollment. The acquisition owner must independently bind
// these original bytes and P's original source association before retaining a
// native grant; no runtime request can install the returned source value.
func parseGrantSourceBytes(raw []byte,profile *Profile)(nativeGrant,error){
 var out nativeGrant
 if profile==nil||profile.Platform!=nativePlatform||profile.Digest==[32]byte{}||len(raw)==0||len(raw)>FrameBytes{return out,ErrInvalid}
 value,e:=decodeStrict(raw);if e!=nil{return out,e}
 row,e:=object(value,"schema","profileDigest","storePath","keyPath","retentionPath","clientUsers");if e!=nil||row["schema"]!="service-lasso-native-input-grant.v1"{return out,ErrInvalid}
 digest,e:=digestText(row["profileDigest"],64);if e!=nil||digest!=digestString(profile.Digest){return out,ErrInvalid};out.profileDigest=profile.Digest
 for _,field:=range []struct{name string;target *string}{{"storePath",&out.storePath},{"keyPath",&out.keyPath},{"retentionPath",&out.retentionPath}}{
  route,ok:=row[field.name].(string);if !ok||len(route)==0||len(route)>4096||!utf8.ValidString(route)||strings.IndexByte(route,0)>=0||!sourceGrantRoute(route){return out,ErrInvalid};*field.target=route
 }
 owners,ok:=row["clientUsers"].([]any);if !ok||len(owners)==0||len(owners)>128{return out,ErrInvalid}
 out.clientUsers=make([]string,len(owners))
 for i,rawOwner:=range owners{
  owner,ok:=rawOwner.(string);if !ok||len(owner)==0||len(owner)>184||!sourceOwnerIdentity(owner){return nativeGrant{},ErrInvalid}
  for j:=0;j<i;j++{if owner==out.clientUsers[j]{return nativeGrant{},ErrInvalid}};out.clientUsers[i]=owner
 }
 return out,nil
}