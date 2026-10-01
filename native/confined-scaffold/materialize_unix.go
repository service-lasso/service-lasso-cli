//go:build !windows

package main

import (
 "fmt"
 "path/filepath"
 "strings"
 "syscall"
 "unsafe"
)

// Every lookup below is relative to a descriptor already opened with
// O_NOFOLLOW.  No untrusted descendant is subsequently addressed from the
// process working directory.  Rollback uses the same held parent descriptors
// and removes only entries whose device/inode still match the entry created.
type owned struct { parent int; name string; dev, ino uint64; directory bool }
func openDir(name string, parent int) (int, error) { return syscall.Openat(parent, name, syscall.O_RDONLY|syscall.O_DIRECTORY|syscall.O_NOFOLLOW, 0) }
func id(fd int) (uint64, uint64, error) { var st syscall.Stat_t; if err := syscall.Fstat(fd, &st); err != nil { return 0,0,err }; return uint64(st.Dev),uint64(st.Ino),nil }
func closeAll(handles []int) { for i:=len(handles)-1;i>=0;i-- { _=syscall.Close(handles[i]) } }
func unlinkDirectory(parent int, name string) error { raw, err := syscall.BytePtrFromString(name); if err != nil { return err }; _, _, errno := syscall.Syscall6(syscall.SYS_UNLINKAT, uintptr(parent), uintptr(unsafe.Pointer(raw)), uintptr(0x200), 0, 0, 0); if errno != 0 { return errno }; return nil }
func materialize(destination string, entries []entry) error {
 clean := filepath.Clean(destination); if clean != destination { return fmt.Errorf("noncanonical destination") }
 parts := strings.Split(strings.TrimPrefix(clean, "/"), "/"); if len(parts)==0 { return fmt.Errorf("root destination") }
 root, err := syscall.Open("/", syscall.O_RDONLY|syscall.O_DIRECTORY|syscall.O_NOFOLLOW, 0); if err != nil { return err }; handles:=[]int{root}; parent:=root; ownedEntries:=[]owned{}
 defer closeAll(handles)
 rollback := func() { for i:=len(ownedEntries)-1;i>=0;i-- { item:=ownedEntries[i]; fd,e:=openDir(item.name,item.parent); if item.directory && e==nil { d,n,_:=id(fd); _=syscall.Close(fd); if d==item.dev && n==item.ino { _=unlinkDirectory(item.parent,item.name) } } } }
 for _, part := range parts[:len(parts)-1] { next,e:=openDir(part,parent); if e!=nil { return e }; handles=append(handles,next); parent=next }
 leaf:=parts[len(parts)-1]; if err=syscall.Mkdirat(parent,leaf,0700); err!=nil { return err }; project,e:=openDir(leaf,parent); if e!=nil { rollback(); return e }; d,n,e:=id(project); if e!=nil { rollback(); return e }; ownedEntries=append(ownedEntries,owned{parent,leaf,d,n,true}); handles=append(handles,project)
 for _, item := range entries { current:=project; parts:=strings.Split(item.path,"/"); for _, part:=range parts[:len(parts)-1] { child,e:=openDir(part,current); if e!=nil { if e!=syscall.ENOENT { rollback(); return e }; if e=syscall.Mkdirat(current,part,0700);e!=nil { rollback(); return e }; child,e=openDir(part,current); if e!=nil { rollback(); return e }; cd,cn,e:=id(child); if e!=nil { rollback(); return e }; ownedEntries=append(ownedEntries,owned{current,part,cd,cn,true}) }; handles=append(handles,child); current=child }
  name:=parts[len(parts)-1]; fd,e:=syscall.Openat(current,name,syscall.O_WRONLY|syscall.O_CREAT|syscall.O_EXCL|syscall.O_NOFOLLOW,uint32(item.mode)); if e!=nil { rollback(); return e }; written:=0; for written<len(item.bytes) { count,we:=syscall.Write(fd,item.bytes[written:]); if we!=nil { _=syscall.Close(fd); rollback(); return we }; written+=count }; if e=syscall.Fsync(fd);e!=nil { _=syscall.Close(fd); rollback(); return e }; fdDev,fdIno,e:=id(fd); _=syscall.Close(fd); if e!=nil { rollback(); return e }; ownedEntries=append(ownedEntries,owned{current,name,fdDev,fdIno,false})
 }
 return nil
}
