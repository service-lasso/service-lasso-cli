//go:build !windows

package main

import (
 "crypto/sha256"
 "fmt"
 "os"
 "golang.org/x/sys/unix"
)

// Reads the actual original read/write descriptor after flush. A zero-length
// file still performs the original one-byte EOF read. Native receipt/observer
// integration must retain the raw read/EOF/stat events and actual file identity;
// this local equality check alone is not the full private readback proof.
func readbackOriginalFile(fd int,expected entry)(heldReadback,error){
 var out heldReadback;var before,after unix.Stat_t
	if len(expected.bytes)>768000{return out,fmt.Errorf("original output readback quota exceeded")}
 if unix.Fstat(fd,&before)!=nil||before.Mode&unix.S_IFMT!=unix.S_IFREG||before.Nlink!=1||before.Uid!=uint32(os.Geteuid())||before.Size<0||before.Size!=int64(len(expected.bytes)){return out,fmt.Errorf("original output identity invalid")}
 out.mode=os.FileMode(before.Mode&07777);if out.mode!=0644&&out.mode!=0755||out.mode!=expected.mode{return out,fmt.Errorf("original output mode differs")}
 var buffer [16384]byte;hash:=sha256.New();offset:=int64(0)
 for offset<before.Size{
  remaining:=before.Size-offset;take:=len(buffer);if remaining<int64(take){take=int(remaining)}
  n,e:=unix.Pread(fd,buffer[:take],offset);if e!=nil||n<=0||n>take{return out,fmt.Errorf("original output readback incomplete")};hash.Write(buffer[:n]);offset+=int64(n)
 }
 n,e:=unix.Pread(fd,buffer[:1],offset);if e!=nil||n!=0{return out,fmt.Errorf("original output EOF absent")}
 if unix.Fstat(fd,&after)!=nil||!writerStatStable(before,after){return out,fmt.Errorf("original output changed")}
 copy(out.sha256[:],hash.Sum(nil));if out.sha256!=sha256.Sum256(expected.bytes){return out,fmt.Errorf("original output bytes differ")};out.bytes=uint32(offset);out.nativeIdentity=writerStatDigest(after);return out,nil
}
