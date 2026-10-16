//go:build windows

package hostadmission

import "path/filepath"

func sourceOwnerIdentity(owner string)bool{
 if len(owner)<5||owner[:4]!="S-1-"{return false};start:=4;component:=0
 for i:=4;i<=len(owner);i++{
  if i<len(owner)&&owner[i]!='-'{continue};part:=owner[start:i];if len(part)==0||len(part)>1&&part[0]=='0'{return false}
  maximum:=uint64(^uint32(0));if component==0{maximum=(uint64(1)<<48)-1}
  var value uint64;for j:=0;j<len(part);j++{b:=part[j];if b<'0'||b>'9'{return false};digit:=uint64(b-'0');if value>(maximum-digit)/10{return false};value=value*10+digit}
  component++;if component>16{return false};start=i+1
 };return component>=1
}
func sourceGrantRoute(route string)bool{
 if !filepath.IsAbs(route){return false};volume:=filepath.VolumeName(route);if volume==""{return false}
 rest:=route[len(volume):];if len(rest)==0||rest[0]!='\\'&&rest[0]!='/'{return false};if len(rest)==1{return true}
 start:=1
 for i:=1;i<=len(rest);i++{if i<len(rest)&&rest[i]!='\\'&&rest[i]!='/'{continue};part:=rest[start:i];if part==""||part=="."||part==".."{return false};start=i+1};return true
}