//go:build darwin

package main

import (
 "encoding/binary"
 "fmt"
 "golang.org/x/sys/unix"
)

// Legacy Darwin confinement follows its native dirent layout. This does not
// establish Darwin F4 qualification or change the Windows/Linux release gate.
func discoverOriginalDirectory(fd int,expected int,correlate func(string)error)error{
 if expected<0||expected>128||correlate==nil{return fmt.Errorf("invalid discovery context")};var before,after unix.Stat_t;if unix.Fstat(fd,&before)!=nil||before.Mode&unix.S_IFMT!=unix.S_IFDIR{return fmt.Errorf("original directory unavailable")};position,e:=unix.Seek(fd,0,0);if e!=nil||position!=0{return fmt.Errorf("original directory cursor unavailable")}
 var raw [16384]byte;var names [128]string;count,total,records:=0,0,0
 for {n,e:=unix.ReadDirent(fd,raw[:]);if e!=nil||n<0||n>len(raw){return fmt.Errorf("original discovery failed")};if n==0{break};total+=n;if total>65536{return fmt.Errorf("original discovery quota exceeded")}
  for at:=0;at<n;{if n-at<22{return fmt.Errorf("truncated original directory record")};length:=int(binary.LittleEndian.Uint16(raw[at+16:at+18]));nameLength:=int(binary.LittleEndian.Uint16(raw[at+18:at+20]));if length<22||length>n-at||nameLength<1||nameLength>1023||21+nameLength>=length||raw[at+21+nameLength]!=0{return fmt.Errorf("invalid original directory record")};records++;if records>130{return fmt.Errorf("original directory record quota exceeded")};name:=string(raw[at+21:at+21+nameLength]);at+=length;if name=="."||name==".."{continue};if count>=expected||count>=len(names){return fmt.Errorf("unexpected original child")};for i:=0;i<count;i++{if names[i]==name{return fmt.Errorf("duplicate original child")}};if e:=correlate(name);e!=nil{return e};names[count]=name;count++}
 }
 if count!=expected||unix.Fstat(fd,&after)!=nil||!writerStatStable(before,after){return fmt.Errorf("original directory changed")};return nil
}
