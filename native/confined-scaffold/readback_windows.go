//go:build windows

package main

import (
 "crypto/sha256"
 "fmt"
 "syscall"
 "golang.org/x/sys/windows"
)

// Original synchronous handle only: no reopened pathname or planned-byte echo.
// Full native event retention and observer binding are separate obligations.
func readbackOriginalFile(fd syscall.Handle, expected entry)(heldReadback,error){
 var out heldReadback;var before,after windows.ByHandleFileInformation
 h:=windows.Handle(fd)
 originalIdentity,originalBasic,e:=writerWindowsIdentity(fd);if e!=nil{return out,e}
 var identityBefore,identityAfter [24]byte
 // FILE_ID_INFO: original volume serial plus full 128-bit native file ID.
 if windows.GetFileInformationByHandleEx(h,18,&identityBefore[0],uint32(len(identityBefore)))!=nil{return out,fmt.Errorf("original output identity unavailable")}
 if len(expected.bytes)>768000||windows.GetFileInformationByHandle(h,&before)!=nil||before.FileAttributes&(windows.FILE_ATTRIBUTE_DIRECTORY|windows.FILE_ATTRIBUTE_REPARSE_POINT)!=0||before.NumberOfLinks!=1||before.FileSizeHigh!=0||before.FileSizeLow!=uint32(len(expected.bytes)){return out,fmt.Errorf("original output identity invalid")}
 mode,e:=observeOutputMode(fd);if e!=nil||mode!=expected.mode{return out,fmt.Errorf("original output mode differs")};out.mode=mode
 var high int32;position,e:=windows.SetFilePointer(h,0,&high,windows.FILE_BEGIN);if e!=nil||position!=0||high!=0{return out,fmt.Errorf("original output seek failed")}
 var buffer [16384]byte;hash:=sha256.New();var total uint32
 for total<before.FileSizeLow{
  take:=uint32(len(buffer));if before.FileSizeLow-total<take{take=before.FileSizeLow-total};var n uint32
  if e:=windows.ReadFile(h,buffer[:take],&n,nil);e!=nil||n==0||n>take{return out,fmt.Errorf("original output readback incomplete")};hash.Write(buffer[:n]);total+=n
 }
 var eof uint32;e=windows.ReadFile(h,buffer[:1],&eof,nil);if (e!=nil&&e!=windows.ERROR_HANDLE_EOF)||eof!=0{return out,fmt.Errorf("original output EOF absent")}
 if windows.GetFileInformationByHandleEx(h,18,&identityAfter[0],uint32(len(identityAfter)))!=nil||identityBefore!=identityAfter||windows.GetFileInformationByHandle(h,&after)!=nil||before.VolumeSerialNumber!=after.VolumeSerialNumber||before.FileIndexHigh!=after.FileIndexHigh||before.FileIndexLow!=after.FileIndexLow||before.CreationTime!=after.CreationTime||before.LastWriteTime!=after.LastWriteTime||before.FileSizeHigh!=after.FileSizeHigh||before.FileSizeLow!=after.FileSizeLow||before.FileAttributes!=after.FileAttributes||before.NumberOfLinks!=after.NumberOfLinks{return out,fmt.Errorf("original output changed")}
 mode,e=observeOutputMode(fd);if e!=nil||mode!=out.mode{return out,fmt.Errorf("original output mode changed")}
 afterIdentity,afterBasic,e:=writerWindowsIdentity(fd);if e!=nil||originalIdentity!=afterIdentity||originalBasic!=afterBasic{return out,fmt.Errorf("original output metadata changed")}
 copy(out.sha256[:],hash.Sum(nil));if out.sha256!=sha256.Sum256(expected.bytes){return out,fmt.Errorf("original output bytes differ")};out.bytes=total;out.nativeIdentity=writerWindowsFingerprint(afterIdentity,afterBasic);return out,nil
}
