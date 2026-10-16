//go:build linux

package hostadmission

import (
 "encoding/binary"
 "strconv"
 "golang.org/x/sys/unix"
)

func observeSourceObject(handle int)(sourceObjectMetadata,error){
 var out sourceObjectMetadata;if handle<0{return out,ErrInvalid}
 flags,e:=unix.FcntlInt(uintptr(handle),unix.F_GETFL,0);if e!=nil||flags&unix.O_ACCMODE!=unix.O_RDONLY||flags&unix.O_PATH!=0{return out,ErrInvalid}
 descriptorFlags,e:=unix.FcntlInt(uintptr(handle),unix.F_GETFD,0);if e!=nil||descriptorFlags&unix.FD_CLOEXEC==0{return out,ErrInvalid}
 var stat unix.Stat_t;if unix.Fstat(handle,&stat)!=nil||stat.Mode&unix.S_IFMT!=unix.S_IFREG||stat.Size<0{return out,ErrInvalid}
 out.owner=strconv.FormatUint(uint64(stat.Uid),10);out.size=uint64(stat.Size)
 raw:=out.raw[:];binary.BigEndian.PutUint64(raw[:8],uint64(stat.Dev));binary.BigEndian.PutUint64(raw[8:16],stat.Ino)
 binary.BigEndian.PutUint32(raw[16:20],stat.Mode);binary.BigEndian.PutUint32(raw[20:24],stat.Uid);binary.BigEndian.PutUint32(raw[24:28],stat.Gid)
 binary.BigEndian.PutUint64(raw[28:36],uint64(stat.Nlink));binary.BigEndian.PutUint64(raw[36:44],out.size)
 binary.BigEndian.PutUint64(raw[44:52],uint64(stat.Mtim.Sec));binary.BigEndian.PutUint64(raw[52:60],uint64(stat.Mtim.Nsec))
 binary.BigEndian.PutUint64(raw[60:68],uint64(stat.Ctim.Sec));binary.BigEndian.PutUint64(raw[68:76],uint64(stat.Ctim.Nsec))
 binary.BigEndian.PutUint32(raw[76:80],uint32(flags));binary.BigEndian.PutUint32(raw[80:84],uint32(descriptorFlags));out.rawBytes=84
 // Stable identity excludes mutable size/timestamps; before/after above retain
 // and compare those separately. Access class comes from the original handle.
 out.identity=DomainHash("SLCLI-ORIGINAL-SOURCE-OBJECT-1\x00",raw[:28]);return out,nil
}
func readSourceObjectAt(handle int,destination []byte,offset uint64)(int,error){
 if handle<0||len(destination)==0||offset>FrameBytes{return 0,ErrInvalid}
 return readSourceEvidenceAt(handle,destination,offset)
}
func readSourceEvidenceAt(handle int,destination []byte,offset uint64)(int,error){
 if handle<0||len(destination)==0||len(destination)>FrameBytes||offset>SessionBytes{return 0,ErrInvalid}
 for{n,e:=unix.Pread(handle,destination,int64(offset));if e==unix.EINTR{continue};return n,e}
}
func sourceObjectEOF(count int,err error)bool{return count==0&&err==nil}
