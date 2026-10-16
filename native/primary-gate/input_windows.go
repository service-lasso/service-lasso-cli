//go:build windows
package main

import (
 "crypto/sha256"
 "encoding/binary"
 "errors"
 "io"
 "path/filepath"
 "strings"
 "unsafe"
 "golang.org/x/sys/windows"
)

type nativeInputRoot struct { handles [124]windows.Handle; attempted [124]bool; count int; handle windows.Handle; leaves [4]*nativeInputLeaf; leafCount int; closeErr error }
type nativeInputLeaf struct { handle windows.Handle; before windows.ByHandleFileInformation; originalID [24]byte; size uint32; closeAttempted bool; closeErr error }
var originalInputRoot *nativeInputRoot
func nativeReadFileID(handle windows.Handle)([24]byte,error){
 var raw [24]byte;var aligned [3]uint64;if e:=windows.GetFileInformationByHandleEx(handle,windows.FileIdInfo,(*byte)(unsafe.Pointer(&aligned[0])),24);e!=nil{return raw,e};copy(raw[:],unsafe.Slice((*byte)(unsafe.Pointer(&aligned[0])),24));zero:=true;for _,b:=range raw[8:]{if b!=0{zero=false;break}};if zero{return [24]byte{},errNativeProtocol};return raw,nil
}
func nativeReadInfo(handle windows.Handle,directory bool)(windows.ByHandleFileInformation,error){
 var info windows.ByHandleFileInformation;kind,err:=windows.GetFileType(handle);if err!=nil||kind!=windows.FILE_TYPE_DISK{return info,errNativeProtocol}
 if err=windows.GetFileInformationByHandle(handle,&info);err!=nil{return info,err};if info.FileAttributes&windows.FILE_ATTRIBUTE_REPARSE_POINT!=0||(info.FileAttributes&windows.FILE_ATTRIBUTE_DIRECTORY!=0)!=directory{return info,errNativeProtocol};return info,nil
}
func openNativeInputRoot(root string)(*nativeInputRoot,error){
 if originalInputRoot!=nil{return nil,errNativeProtocol}
 volume:=filepath.VolumeName(root);if len(volume)!=2||volume[1]!=':'||!filepath.IsAbs(root)||filepath.Clean(root)!=root||strings.Contains(root,"\x00"){return nil,errNativeProtocol}
 path:=strings.TrimPrefix(root,volume+`\`);parts:=strings.Split(path,`\`);if len(parts)+1>124{return nil,errNativeCapacity};for _,part:=range parts{if part==""||part=="."||part==".."||strings.ContainsAny(part,"/:"){return nil,errNativeProtocol}}
 held:=&nativeInputRoot{};originalInputRoot=held;held.count=1
 first,err:=gateOpen(volume+`\`,0,gateFileListDirectory|gateFileReadAttributes|gateReadControl|gateSynchronize,gateFileOpen,gateFileDirectoryFile|gateFileSynchronousIoNonalert,nil);held.handles[0]=first;if err!=nil{return nil,err};held.handle=first;if _,err=nativeReadInfo(first,true);err!=nil{held.close();return nil,err}
 for _,part:=range parts{index:=held.count;held.count++;next,e:=gateOpen(part,held.handle,gateFileListDirectory|gateFileReadAttributes|gateReadControl|gateSynchronize,gateFileOpen,gateFileDirectoryFile|gateFileSynchronousIoNonalert,nil);held.handles[index]=next;if e!=nil{held.close();return nil,e};held.handle=next;if _,e=nativeReadInfo(next,true);e!=nil{held.close();return nil,e}};return held,nil
}
func(r *nativeInputRoot)close()error{
 if r==nil{return errNativeProtocol};for i:=r.leafCount-1;i>=0;i--{if e:=r.leaves[i].close();e!=nil&&r.closeErr==nil{r.closeErr=e}}
 for i:=r.count-1;i>=0;i--{if r.attempted[i]||r.handles[i]==0{continue};r.attempted[i]=true;if e:=windows.CloseHandle(r.handles[i]);e!=nil&&r.closeErr==nil{r.closeErr=e}};return r.closeErr
}
func(r *nativeInputRoot)open(name string,size uint32)(*nativeInputLeaf,error){
 if r==nil||r!=originalInputRoot||r.handle==0||r.closeErr!=nil||r.leafCount>=4{return nil,errNativeProtocol};allowed:=false;for _,fixed:=range nativeAssetNames{if name==fixed{allowed=true}};if !allowed{return nil,errNativeProtocol};for i:=0;i<r.count;i++{if r.attempted[i]{return nil,errNativeProtocol}}
 leaf:=&nativeInputLeaf{size:size};r.leaves[r.leafCount]=leaf;r.leafCount++;handle,err:=gateOpen(name,r.handle,gateFileReadData|gateFileReadAttributes|gateReadControl|gateSynchronize,gateFileOpen,gateFileSynchronousIoNonalert,nil);leaf.handle=handle;if err!=nil{return nil,err};leaf.before,err=nativeReadInfo(handle,false)
 if err!=nil{return nil,err};if uint64(leaf.before.FileSizeHigh)<<32|uint64(leaf.before.FileSizeLow)!=uint64(size){return nil,errNativeInputChanged};leaf.originalID,err=nativeReadFileID(handle);if err!=nil{return nil,err};return leaf,nil
}
func(l *nativeInputLeaf)readExact(bytes []byte)([32]byte,error){
 var identity [32]byte;if l==nil||l.handle==0||l.closeAttempted||len(bytes)!=int(l.size){return identity,errNativeProtocol};for offset:=0;offset<len(bytes);{var n uint32;err:=windows.ReadFile(l.handle,bytes[offset:],&n,nil);if err!=nil{return identity,err};if n==0{return identity,io.ErrUnexpectedEOF};offset+=int(n)}
 var probe [1]byte;var n uint32;err:=windows.ReadFile(l.handle,probe[:],&n,nil);if err!=nil&&err!=windows.ERROR_HANDLE_EOF{return identity,err};if n!=0{return identity,errNativeInputChanged};after,err:=nativeReadInfo(l.handle,false);if err!=nil{return identity,err};afterID,err:=nativeReadFileID(l.handle);if err!=nil{return identity,err};if afterID!=l.originalID{return identity,errNativeInputChanged}
 if after.FileAttributes!=l.before.FileAttributes||after.FileSizeHigh!=l.before.FileSizeHigh||after.FileSizeLow!=l.before.FileSizeLow||after.LastWriteTime!=l.before.LastWriteTime||after.CreationTime!=l.before.CreationTime{return identity,errNativeInputChanged}
 var raw [48]byte;copy(raw[:24],afterID[:]);binary.BigEndian.PutUint32(raw[24:28],after.FileSizeHigh);binary.BigEndian.PutUint32(raw[28:32],after.FileSizeLow);binary.BigEndian.PutUint32(raw[32:36],after.CreationTime.HighDateTime);binary.BigEndian.PutUint32(raw[36:40],after.CreationTime.LowDateTime);binary.BigEndian.PutUint32(raw[40:44],after.LastWriteTime.HighDateTime);binary.BigEndian.PutUint32(raw[44:48],after.LastWriteTime.LowDateTime);return sha256.Sum256(raw[:]),nil
}
func(l *nativeInputLeaf)close()error{if l==nil{return errNativeProtocol};if l.closeAttempted{return l.closeErr};l.closeAttempted=true;if l.handle!=0{l.closeErr=windows.CloseHandle(l.handle)};return l.closeErr}
func nativeInputPlatformCode(failure error)byte{var status *gateNativeOpenError;if errors.As(failure,&status){switch status.status{case windows.STATUS_REPARSE_POINT_ENCOUNTERED,windows.STATUS_STOPPED_ON_SYMLINK,windows.STATUS_NOT_A_DIRECTORY,windows.STATUS_OBJECT_TYPE_MISMATCH:return 2}};return 1}
