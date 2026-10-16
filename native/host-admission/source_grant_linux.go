//go:build linux

package hostadmission

func sourceOwnerIdentity(owner string)bool{_,e:=procDecimal([]byte(owner),uint64(^uint32(0)));return e==nil}
func sourceGrantRoute(route string)bool{
 if len(route)==0||route[0]!='/'{return false};if route=="/"{return true}
 start:=1
 for i:=1;i<=len(route);i++{if i<len(route)&&route[i]!='/'{continue};part:=route[start:i];if part==""||part=="."||part==".."{return false};start=i+1};return true
}