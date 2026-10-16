//go:build linux

package main

import (
 "encoding/binary"
 "fmt"
 "golang.org/x/sys/unix"
)

// Enumerate the original held directory once with a fixed raw buffer. The
// owning observer route must retain every actual raw chunk and native EOF;
// callbacks correlate each name with an original retained child descriptor.
func discoverOriginalDirectory(fd int,expected int,correlate func(string)error)error{
 if expected<0||expected>128||correlate==nil{return fmt.Errorf("invalid discovery context")}
 var before,after unix.Stat_t;if unix.Fstat(fd,&before)!=nil||before.Mode&unix.S_IFMT!=unix.S_IFDIR{return fmt.Errorf("original directory unavailable")}
 position,e:=unix.Seek(fd,0,0);if e!=nil||position!=0{return fmt.Errorf("original directory cursor unavailable")}
 var raw [16384]byte;var names [128]string;count,total,records:=0,0,0
 for {
  n,e:=unix.ReadDirent(fd,raw[:]);if e!=nil||n<0||n>len(raw){return fmt.Errorf("original discovery failed")};if n==0{break};total+=n;if total>65536{return fmt.Errorf("original discovery quota exceeded")}
  for at:=0;at<n;{
   if n-at<20{return fmt.Errorf("truncated original directory record")};length:=int(binary.LittleEndian.Uint16(raw[at+16:at+18]));if length<20||length>n-at||length%8!=0{return fmt.Errorf("invalid original directory record")}
   records++;if records>130{return fmt.Errorf("original directory record quota exceeded")};end:=at+19;for end<at+length&&raw[end]!=0{end++};if end==at+length||end==at+19{return fmt.Errorf("invalid original directory name")};name:=string(raw[at+19:end]);at+=length
   if name=="."||name==".."{continue};if count>=expected||count>=len(names){return fmt.Errorf("unexpected original child")};for i:=0;i<count;i++{if names[i]==name{return fmt.Errorf("duplicate original child")}};if e:=correlate(name);e!=nil{return e};names[count]=name;count++
  }
 }
 if count!=expected||unix.Fstat(fd,&after)!=nil||!writerStatStable(before,after)||before.Nlink!=after.Nlink{return fmt.Errorf("original directory changed")};return nil
}
