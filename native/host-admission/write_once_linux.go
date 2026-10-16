//go:build linux

package hostadmission

import "golang.org/x/sys/unix"

const nativeWriteOperation="linux.write"

// One actual write completion lets the source collector retain its observed
// count before another native operation can overwrite that observation.
func writeOwnedNativeOnce(handle int,memory *nativeMemory,offset,length uint32,deadline *nativeDeadline,owner *originalFrameLease)(count int,err error){
 if memory==nil||deadline==nil||length==0||length>FrameBytes{return 0,ErrInvalid}
 ordinary,e:=borrowFrameOrOrdinary(memory,owner);if e!=nil{return 0,e};defer func(){if ordinary{if e:=memory.returnOriginal();e!=nil{err=qualifyNativeFailure(err,e)}}}()
 if uint64(offset)+uint64(length)>uint64(len(memory.bytes)){return 0,ErrInvalid}
 if e:=waitOwnedIO(handle,unix.POLLOUT,deadline);e!=nil{return 0,e}
 n,e:=unix.Write(handle,memory.bytes[offset:offset+length]);if e!=nil{return n,qualifyNativeFailure(ErrUnavailable,e)}
 if n<0||uint32(n)>length{return 0,ErrInvalid};if _,e=deadline.remaining();e!=nil{return n,e};return n,nil
}
