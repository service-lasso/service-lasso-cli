//go:build linux || windows || darwin

package main

import "strings"

// Bounded transport namespace checks; owning canonical derivation remains SEA.
func nativePathsCompatible(a,b string)bool{
 ai,bi:=0,0
 for {
  ae,be:=ai,bi;for ae<len(a)&&a[ae]!='/'{ae++};for be<len(b)&&b[be]!='/'{be++}
  ac,bc:=a[ai:ae],b[bi:be];if !strings.EqualFold(ac,bc){return true};if ac!=bc{return false}
  if ae==len(a)||be==len(b){return false};ai=ae+1;bi=be+1
 }
}
func validNativePath(path string)bool{
 if len(path)<1||len(path)>240{return false};depth,start:=0,0
 for i:=0;i<=len(path);i++{
  if i<len(path)&&path[i]!='/'{b:=path[i];if !(b>='a'&&b<='z'||b>='A'&&b<='Z'||b>='0'&&b<='9'||b=='_'||b=='.'||b=='@'||b=='-'){return false};continue}
  part:=path[start:i];depth++;if part==""||part=="."||part==".."||strings.HasSuffix(part,".")||depth>12{return false};base:=part;if dot:=strings.IndexByte(base,'.');dot>=0{base=base[:dot]}
  if strings.EqualFold(base,"con")||strings.EqualFold(base,"prn")||strings.EqualFold(base,"aux")||strings.EqualFold(base,"nul")||len(base)==4&&(strings.EqualFold(base[:3],"com")||strings.EqualFold(base[:3],"lpt"))&&base[3]>='1'&&base[3]<='9'{return false};start=i+1
 }
 return true
}
