//go:build windows

package main

import (
 "crypto/sha256"
 "encoding/binary"
 "fmt"
 "strings"
 "syscall"
 "unsafe"
 "golang.org/x/sys/windows"
)

type writerWindowsDirectory struct{path string;handle syscall.Handle}
type writerWindowsFile struct{handle syscall.Handle;identity [32]byte}
type writerWindowsChild struct{name string;fileID [16]byte;attributes uint32;size uint64;created,written,changed uint64}
var ntWriterQueryDirectory=ntdll.NewProc("NtQueryDirectoryFile")

func writerWindowsIdentity(handle syscall.Handle)(identity [24]byte,basic [40]byte,result error){
 h:=windows.Handle(handle);if windows.GetFileInformationByHandleEx(h,18,&identity[0],uint32(len(identity)))!=nil||windows.GetFileInformationByHandleEx(h,0,&basic[0],uint32(len(basic)))!=nil{return identity,basic,fmt.Errorf("original native identity unavailable")}
 // LastAccessTime may change as a consequence of our original read. Preserve
 // the other original metadata for change correlation, including ChangeTime.
 for i:=8;i<16;i++{basic[i]=0};return identity,basic,nil
}
func writerWindowsFingerprint(identity [24]byte,basic [40]byte)[32]byte{var raw [64]byte;copy(raw[:24],identity[:]);copy(raw[24:],basic[:]);return sha256.Sum256(raw[:])}
func writerWindowsParentLeaf(path string)(string,string){at:=strings.LastIndexByte(path,'/');if at<0{return "",path};return path[:at],path[at+1:]}

// Native information class60 has full128-bit child IDs. IO_STATUS_BLOCK gives
// the actual returned raw length; no zero-padded buffer capacity is called an
// observed byte count. Raw chunks/terminal status still require retained native
// event and observer receipts in the owning production path.
func discoverWindowsDirectory(handle syscall.Handle,expected int,correlate func(writerWindowsChild)error)error{
 if expected<0||expected>128||correlate==nil{return fmt.Errorf("invalid original discovery context")};identity,before,e:=writerWindowsIdentity(handle);if e!=nil{return e}
 var raw [16384]byte;var names [128]string;count,total,records:=0,0,0;restart:=uintptr(1)
 for {
  var status ioStatusBlock;result,_,_:=ntWriterQueryDirectory.Call(uintptr(handle),0,0,0,uintptr(unsafe.Pointer(&status)),uintptr(unsafe.Pointer(&raw[0])),uintptr(len(raw)),60,0,0,restart);restart=0
  if uint32(result)==0x80000006{if status.information!=0{return fmt.Errorf("invalid discovery EOF")};break};if result!=0||status.status!=0||status.information==0||status.information>uintptr(len(raw)){return fmt.Errorf("original discovery unavailable")};n:=int(status.information);total+=n;if total>65536{return fmt.Errorf("original discovery quota exceeded")}
  for at:=0;at<n;{
   if n-at<88{return fmt.Errorf("truncated original directory record")};next:=int(binary.LittleEndian.Uint32(raw[at:at+4]));length:=int(binary.LittleEndian.Uint32(raw[at+60:at+64]));if length<2||length>480||length%2!=0||88+length>n-at{return fmt.Errorf("invalid original directory name")};recordEnd:=at+88+length;if next!=0&&(next<88+length||next%8!=0||next>n-at){return fmt.Errorf("invalid original directory framing")}
   var ascii [240]byte;for i:=0;i<length/2;i++{unit:=binary.LittleEndian.Uint16(raw[at+88+2*i:at+90+2*i]);if unit==0||unit>127{return fmt.Errorf("unselected original directory name")};ascii[i]=byte(unit)}
   child:=writerWindowsChild{name:string(ascii[:length/2]),attributes:binary.LittleEndian.Uint32(raw[at+56:at+60]),size:binary.LittleEndian.Uint64(raw[at+40:at+48]),created:binary.LittleEndian.Uint64(raw[at+8:at+16]),written:binary.LittleEndian.Uint64(raw[at+24:at+32]),changed:binary.LittleEndian.Uint64(raw[at+32:at+40])};copy(child.fileID[:],raw[at+72:at+88]);records++;if records>130{return fmt.Errorf("original directory record quota exceeded")}
   if child.name!="."&&child.name!=".."{if count>=expected||count>=len(names)||child.attributes&windows.FILE_ATTRIBUTE_REPARSE_POINT!=0{return fmt.Errorf("unexpected original child")};for i:=0;i<count;i++{if names[i]==child.name{return fmt.Errorf("duplicate original child")}};if e:=correlate(child);e!=nil{return e};names[count]=child.name;count++}
   if next==0{if recordEnd>n||n-recordEnd>7{return fmt.Errorf("invalid terminal original directory record")};break};at+=next
  }
 }
 afterIdentity,after,e:=writerWindowsIdentity(handle);if e!=nil||identity!=afterIdentity||before!=after||count!=expected{return fmt.Errorf("original directory changed")};return nil
}

func verifyWindowsDiscovery(entries []entry,files []writerWindowsFile,directories []writerWindowsDirectory)error{
 if len(entries)!=len(files)||len(entries)<1||len(entries)>128||len(directories)<1||len(directories)>128{return fmt.Errorf("invalid original discovery custody")}
 for _,directory:=range directories{
  parentIdentity,_,e:=writerWindowsIdentity(directory.handle);if e!=nil{return e};expected:=0;for _,item:=range entries{parent,_:=writerWindowsParentLeaf(item.path);if parent==directory.path{expected++}};for _,child:=range directories{if child.path==""{continue};parent,_:=writerWindowsParentLeaf(child.path);if parent==directory.path{expected++}}
  if e:=discoverWindowsDirectory(directory.handle,expected,func(child writerWindowsChild)error{
   correlate:=func(handle syscall.Handle,directory bool,expectedFingerprint *[32]byte)error{
    identity,basic,e:=writerWindowsIdentity(handle);if e!=nil{return e};for i:=0;i<8;i++{if identity[i]!=parentIdentity[i]{return fmt.Errorf("discovered original volume differs")}};for i:=0;i<16;i++{if identity[8+i]!=child.fileID[i]{return fmt.Errorf("discovered original identity differs")}}
    attributes:=binary.LittleEndian.Uint32(basic[32:36]);if attributes!=child.attributes||(attributes&windows.FILE_ATTRIBUTE_DIRECTORY!=0)!=directory||binary.LittleEndian.Uint64(basic[:8])!=child.created||binary.LittleEndian.Uint64(basic[16:24])!=child.written||binary.LittleEndian.Uint64(basic[24:32])!=child.changed{return fmt.Errorf("discovered original metadata differs")};if expectedFingerprint!=nil&&writerWindowsFingerprint(identity,basic)!=*expectedFingerprint{return fmt.Errorf("original file changed after readback")};return nil
   }
   for i,item:=range entries{parent,leaf:=writerWindowsParentLeaf(item.path);if parent==directory.path&&leaf==child.name{if child.size!=uint64(len(item.bytes)){return fmt.Errorf("discovered original size differs")};return correlate(files[i].handle,false,&files[i].identity)}}
   for _,item:=range directories{if item.path==""{continue};parent,leaf:=writerWindowsParentLeaf(item.path);if parent==directory.path&&leaf==child.name{return correlate(item.handle,true,nil)}};return fmt.Errorf("unselected discovered original")
  });e!=nil{return e}
 }
 return nil
}
