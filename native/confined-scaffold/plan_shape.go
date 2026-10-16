package main

import (
 "fmt"
 "strings"
 "unicode/utf8"
)

// Transport/confinement shape only. Canonical policy, inventory membership and
// derivation remain the original SEA owner's job, checked by its native primary.
func validateWriterEntries(destination string,entries []entry)error{
 if len(destination)<1||len(destination)>4096||!utf8.ValidString(destination)||strings.IndexByte(destination,0)>=0||len(entries)<1||len(entries)>128{return fmt.Errorf("invalid writer plan shape")}
 size:=uint64(4+96+4+len(destination)+4);previous:=""
 for index,item:=range entries{
  if !writerPortablePath(item.path)||item.path<=previous||item.mode!=0644&&item.mode!=0755{return fmt.Errorf("invalid writer entry shape")};previous=item.path
  for prior:=0;prior<index;prior++{if !writerPathsCompatible(entries[prior].path,item.path){return fmt.Errorf("writer namespace collision")}}
  growth:=uint64(9+len(item.path))+uint64(len(item.bytes));if size>768000||growth>768000-size{return fmt.Errorf("writer plan quota exceeded")};size+=growth
 }
 return nil
}
// Compare bounded components without allocating a second namespace map. A
// directory cannot acquire two spellings, and a file cannot be a parent.
func writerPathsCompatible(a,b string)bool{
 ai,bi:=0,0
 for {
  ae,be:=ai,bi;for ae<len(a)&&a[ae]!='/'{ae++};for be<len(b)&&b[be]!='/'{be++}
  ac,bc:=a[ai:ae],b[bi:be];if !strings.EqualFold(ac,bc){return true};if ac!=bc{return false}
  if ae==len(a)||be==len(b){return false};ai=ae+1;bi=be+1
 }
}
func writerPortablePath(path string)bool{
 if len(path)<1||len(path)>240{return false};depth,start:=0,0
 for i:=0;i<=len(path);i++{
  if i<len(path)&&path[i]!='/'{b:=path[i];if !(b>='a'&&b<='z'||b>='A'&&b<='Z'||b>='0'&&b<='9'||b=='_'||b=='.'||b=='@'||b=='-'){return false};continue}
  part:=path[start:i];depth++;if part==""||part=="."||part==".."||strings.HasSuffix(part,".")||depth>12{return false};base:=part;if dot:=strings.IndexByte(base,'.');dot>=0{base=base[:dot]}
  if strings.EqualFold(base,"con")||strings.EqualFold(base,"prn")||strings.EqualFold(base,"aux")||strings.EqualFold(base,"nul")||len(base)==4&&(strings.EqualFold(base[:3],"com")||strings.EqualFold(base[:3],"lpt"))&&base[3]>='1'&&base[3]<='9'{return false};start=i+1
 }
 return true
}
