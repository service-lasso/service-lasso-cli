//go:build !windows

package main

import (
 "fmt"
 "strings"
 "golang.org/x/sys/unix"
)

type writerDirectory struct{path string;fd int}
type writerOriginalFile struct{fd int;stat unix.Stat_t}
func writerParentLeaf(path string)(string,string){at:=strings.LastIndexByte(path,'/');if at<0{return "",path};return path[:at],path[at+1:]}

func verifyWriterDiscovery(entries []entry,files []writerOriginalFile,directories []writerDirectory)error{
 if len(entries)!=len(files)||len(entries)<1||len(entries)>128||len(directories)<1||len(directories)>128{return fmt.Errorf("invalid original discovery custody")}
 for _,directory:=range directories{
  expected:=0;for _,item:=range entries{parent,_:=writerParentLeaf(item.path);if parent==directory.path{expected++}};for _,child:=range directories{if child.path==""{continue};parent,_:=writerParentLeaf(child.path);if parent==directory.path{expected++}}
  if err:=discoverOriginalDirectory(directory.fd,expected,func(name string)error{
   var namespace unix.Stat_t;if unix.Fstatat(directory.fd,name,&namespace,unix.AT_SYMLINK_NOFOLLOW)!=nil{return fmt.Errorf("discovered original unavailable")}
   for i,item:=range entries{
    parent,leaf:=writerParentLeaf(item.path);if parent!=directory.path||leaf!=name{continue};var held unix.Stat_t
    if unix.Fstat(files[i].fd,&held)!=nil||!writerStatStable(files[i].stat,held)||!writerStatStable(held,namespace)||held.Mode&unix.S_IFMT!=unix.S_IFREG||held.Nlink!=1{return fmt.Errorf("discovered file differs from held original")};return nil
   }
   for _,child:=range directories{
    if child.path==""{continue};parent,leaf:=writerParentLeaf(child.path);if parent!=directory.path||leaf!=name{continue};var held unix.Stat_t
    if unix.Fstat(child.fd,&held)!=nil||!writerStatStable(held,namespace)||held.Mode&unix.S_IFMT!=unix.S_IFDIR{return fmt.Errorf("discovered directory differs from held original")};return nil
   }
   return fmt.Errorf("unselected discovered original")
  });err!=nil{return err}
 }
 return nil
}
