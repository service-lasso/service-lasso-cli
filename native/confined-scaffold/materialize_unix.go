//go:build !windows

package main

import (
	"crypto/rand"
	"encoding/hex"
	"fmt"
	"golang.org/x/sys/unix"
	"strings"
)

// Every lookup below is relative to a descriptor already opened with
// O_NOFOLLOW.  No untrusted descendant is subsequently addressed from the
// process working directory. Failure handling keeps the same held parent and
// records every original created leaf before it can be written. POSIX has no
// unlink-by-handle operation: a pathname identity check followed by unlink is
// racy. The failure path therefore retains its original named partial leaf rather
// than risk deleting a concurrent replacement. The Windows source likewise
// retains its original private failed candidate for truthful custody.
func openDir(name string, parent int) (int, error) {
	return unix.Openat(parent, name, unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW|unix.O_CLOEXEC, 0)
}
func closeAll(handles []int) error {
	var first error
	for i := len(handles) - 1; i >= 0; i-- {
		if err:=unix.Close(handles[i]);err!=nil&&first==nil{first=err}
	}
	return first
}
func createCandidate(parent int) (string, error) {
	var bytes [16]byte
	if _, err := rand.Read(bytes[:]); err != nil { return "", err }
	name := ".service-lasso-txn-" + hex.EncodeToString(bytes[:])
	if err := unix.Mkdirat(parent, name, 0700); err != nil { return "", err }
	return name,nil
}
func materialize(destination string,entries []entry)error{return materializeObserved(destination,entries,nil)}
func materializeObserved(destination string, entries []entry,digest *writerReadbackDigest) (result error) {
	defer func(){if result!=nil&&digest!=nil{digest.failed=true}}()
	if err:=validateWriterEntries(destination,entries);err!=nil{return err}
	parts, err := destinationParts(destination)
	if err != nil {
		return err
	}
	var originalHandles [128]int
	root, err := openFilesystemRoot()
	if err != nil {
		return err
	}
	originalHandles[0]=root;handles:=originalHandles[:1]
	parent := root
	// Capture the completed original table at closure, rather than the initial
	// root-only slice. Close failures must never become writer success.
	defer func(){if err:=closeAll(handles);err!=nil&&result==nil{result=err}}()
	for _, part := range parts[:len(parts)-1] {
		if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
		next, e := openDir(part, parent)
		if e != nil {
			return e
		}
		handles = append(handles, next)
		parent = next
	}
	leaf := parts[len(parts)-1]
	if err = testGate("before-project-create"); err != nil {
		return err
	}
	// Build beneath a held parent under an unguessable, invocation-owned
	// candidate.  The final leaf is untouched until the exclusive commit.
	candidate, e := createCandidate(parent)
	if e != nil {
		return e
	}
	if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
	project, e := openDir(candidate, parent)
	if e != nil {
		return e
	}
	handles = append(handles, project)
	var directories [128]writerDirectory;directories[0]=writerDirectory{"",project};directoryCount:=1;var files [128]writerOriginalFile
	for index, item := range entries {
		current := project
		parts := strings.Split(item.path, "/")
		prefixEnd:=0
		for _, part := range parts[:len(parts)-1] {
			prefixEnd+=len(part)+1;prefix:=item.path[:prefixEnd-1];found:=false
			for i:=0;i<directoryCount;i++{if directories[i].path==prefix{current=directories[i].fd;found=true;break}};if found{continue}
			if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
			if directoryCount==len(directories){return fmt.Errorf("native writer directory capacity exhausted")}
			if e=unix.Mkdirat(current,part,0700);e!=nil{return e}
			child,e:=openDir(part,current);if e!=nil{return e}
			handles = append(handles, child)
			directories[directoryCount]=writerDirectory{prefix,child};directoryCount++
			current = child
		}
		name := parts[len(parts)-1]
		if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
		fd, e := unix.Openat(current, name, unix.O_RDWR|unix.O_CREAT|unix.O_EXCL|unix.O_NOFOLLOW|unix.O_CLOEXEC, uint32(item.mode))
		if e != nil {
			return e
		}
		handles = append(handles, fd)
		if e=unix.Fchmod(fd,uint32(item.mode));e!=nil{return e}
		written := 0
		for written < len(item.bytes) {
			count, we := unix.Write(fd, item.bytes[written:])
			if we != nil {
				return we
			}
			if count == 0 {
				return fmt.Errorf("short native write")
			}
			written += count
		}
		if e = unix.Fsync(fd); e != nil {
			return e
		}
		actual,e:=readbackOriginalFile(fd,item);if e!=nil{return e};if digest!=nil{if e=digest.add(item.path,actual);e!=nil{return e}}
		files[index].fd=fd;if e=unix.Fstat(fd,&files[index].stat);e!=nil{return e};if writerStatDigest(files[index].stat)!=actual.nativeIdentity{return fmt.Errorf("original output changed after readback")}
		if err = testGate("after-file-write"); err != nil {
			return err
		}
	}
	if err = testGate("before-project-commit"); err != nil {
		return err
	}
	if err=verifyWriterDiscovery(entries,files[:len(entries)],directories[:directoryCount]);err!=nil{return err}
	for i:=directoryCount-1;i>=0;i--{if err=unix.Fsync(directories[i].fd);err!=nil{return err}}
	// Each supported POSIX host supplies an atomic no-replace rename relative
	// to the same held parent.  A concurrent leaf wins intact; it is never
	// overwritten or removed.  A failed candidate is deliberately retained as
	// unknown state because POSIX has no unlink-by-held-object primitive.
	if len(handles)==len(originalHandles){return fmt.Errorf("native writer reference capacity exhausted")}
	var original,held,reopenedIdentity unix.Stat_t;if err=unix.Fstat(project,&original);err!=nil{return err}
	if err=exclusiveCommit(parent,candidate,leaf);err!=nil{return err}
	reopened,err:=openDir(leaf,parent);if err!=nil{return err};handles=append(handles,reopened)
	if unix.Fstat(project,&held)!=nil||unix.Fstat(reopened,&reopenedIdentity)!=nil||original.Dev!=held.Dev||original.Ino!=held.Ino||held.Dev!=reopenedIdentity.Dev||held.Ino!=reopenedIdentity.Ino||held.Mode&unix.S_IFMT!=unix.S_IFDIR||!writerStatStable(held,reopenedIdentity){return fmt.Errorf("renamed original identity differs")}
	if err=verifyWriterDiscovery(entries,files[:len(entries)],directories[:directoryCount]);err!=nil{return err}
	return unix.Fsync(parent)
}
