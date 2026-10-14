//go:build windows
package main

import (
 "crypto/sha256"
 "encoding/binary"
 "errors"
 "io"
 "path/filepath"
 "strings"
 "golang.org/x/sys/windows"
)
type nativeInputRoot struct { handles []windows.Handle; handle windows.Handle }
type nativeInputLeaf struct { handle windows.Handle; before windows.ByHandleFileInformation; size uint32 }
func nativeReadInfo(handle windows.Handle,directory bool)(windows.ByHandleFileInformation,error){
 var info windows.ByHandleFileInformation;kind,err:=windows.GetFileType(handle);if err!=nil||kind!=windows.FILE_TYPE_DISK{return info,errNativeProtocol}
 if err=windows.GetFileInformationByHandle(handle,&info);err!=nil{return info,err};if info.FileAttributes&windows.FILE_ATTRIBUTE_REPARSE_POINT!=0||(info.FileAttributes&windows.FILE_ATTRIBUTE_DIRECTORY!=0)!=directory{return info,errNativeProtocol};return info,nil
}
func openNativeInputRoot(root string)(*nativeInputRoot,error){
 volume:=filepath.VolumeName(root);if len(volume)!=2||volume[1]!=':'||!filepath.IsAbs(root)||filepath.Clean(root)!=root||strings.Contains(root,"\x00"){return nil,errNativeProtocol}
 path:=strings.TrimPrefix(root,volume+`\`);parts:=strings.Split(path,`\`);for _,part:=range parts{if part==""||part=="."||part==".."||strings.ContainsAny(part,"/:"){return nil,errNativeProtocol}}
 first,err:=gateOpen(volume+`\`,0,gateFileListDirectory|gateReadControl|gateSynchronize,gateFileOpen,gateFileDirectoryFile|gateFileSynchronousIoNonalert,nil);if err!=nil{return nil,err}
 held:=&nativeInputRoot{handles:[]windows.Handle{first},handle:first};if _,err=nativeReadInfo(first,true);err!=nil{held.close();return nil,err}
 for _,part:=range parts{next,e:=gateOpen(part,held.handle,gateFileListDirectory|gateReadControl|gateSynchronize,gateFileOpen,gateFileDirectoryFile|gateFileSynchronousIoNonalert,nil);if e!=nil{held.close();return nil,e};held.handles=append(held.handles,next);held.handle=next;if _,e=nativeReadInfo(next,true);e!=nil{held.close();return nil,e}};return held,nil
}
func(r *nativeInputRoot)close()error{var failure error;for i:=len(r.handles)-1;i>=0;i--{if e:=windows.CloseHandle(r.handles[i]);e!=nil{failure=errors.Join(failure,e)}};r.handles=nil;return failure}
func(r *nativeInputRoot)open(name string,size uint32)(*nativeInputLeaf,error){
 allowed:=false;for _,fixed:=range nativeAssetNames{if name==fixed{allowed=true}};if !allowed{return nil,errNativeProtocol}
 handle,err:=gateOpen(name,r.handle,gateFileReadData|gateReadControl|gateSynchronize,gateFileOpen,gateFileSynchronousIoNonalert,nil);if err!=nil{return nil,err};before,err:=nativeReadInfo(handle,false)
 if err!=nil||uint64(before.FileSizeHigh)<<32|uint64(before.FileSizeLow)!=uint64(size){windows.CloseHandle(handle);return nil,errNativeProtocol};return &nativeInputLeaf{handle,before,size},nil
}
func(l *nativeInputLeaf)readExact(bytes []byte)([32]byte,error){
 var identity [32]byte;if len(bytes)!=int(l.size){return identity,errNativeProtocol};for offset:=0;offset<len(bytes);{var n uint32;err:=windows.ReadFile(l.handle,bytes[offset:],&n,nil);if err!=nil{return identity,err};if n==0{return identity,io.ErrUnexpectedEOF};offset+=int(n)}
 var probe [1]byte;var n uint32;err:=windows.ReadFile(l.handle,probe[:],&n,nil);if err!=nil&&err!=windows.ERROR_HANDLE_EOF{return identity,err};if n!=0{return identity,errNativeProtocol};after,err:=nativeReadInfo(l.handle,false);if err!=nil{return identity,err}
 if after.VolumeSerialNumber!=l.before.VolumeSerialNumber||after.FileIndexHigh!=l.before.FileIndexHigh||after.FileIndexLow!=l.before.FileIndexLow||after.FileSizeHigh!=l.before.FileSizeHigh||after.FileSizeLow!=l.before.FileSizeLow||after.LastWriteTime!=l.before.LastWriteTime||after.CreationTime!=l.before.CreationTime{return identity,errNativeProtocol}
 var raw [36]byte;binary.BigEndian.PutUint32(raw[0:4],after.VolumeSerialNumber);binary.BigEndian.PutUint32(raw[4:8],after.FileIndexHigh);binary.BigEndian.PutUint32(raw[8:12],after.FileIndexLow);binary.BigEndian.PutUint32(raw[12:16],after.FileSizeHigh);binary.BigEndian.PutUint32(raw[16:20],after.FileSizeLow);binary.BigEndian.PutUint32(raw[20:24],after.CreationTime.HighDateTime);binary.BigEndian.PutUint32(raw[24:28],after.CreationTime.LowDateTime);binary.BigEndian.PutUint32(raw[28:32],after.LastWriteTime.HighDateTime);binary.BigEndian.PutUint32(raw[32:36],after.LastWriteTime.LowDateTime);return sha256.Sum256(raw[:]),nil
}
func(l *nativeInputLeaf)close()error{return windows.CloseHandle(l.handle)}
