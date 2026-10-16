//go:build linux

package hostadmission

import "golang.org/x/sys/unix"

func prepareOwnedIO(handle int)error{flags,e:=unix.FcntlInt(uintptr(handle),unix.F_GETFL,0);if e!=nil{return qualifyNativeFailure(ErrUnavailable,e)};if _,e=unix.FcntlInt(uintptr(handle),unix.F_SETFL,flags|unix.O_NONBLOCK);e!=nil{return qualifyNativeFailure(ErrUnavailable,e)};return nil}
func waitOwnedIO(handle int,event int16,deadline *nativeDeadline)error{
 for{remaining,e:=deadline.remaining();if e!=nil{return e};milliseconds:=(remaining+999999)/1000000;if milliseconds>40000{return ErrInvalid};poll:=[]unix.PollFd{{Fd:int32(handle),Events:event}};n,e:=unix.Poll(poll,int(milliseconds));if e==unix.EINTR{continue};if e!=nil{return qualifyNativeFailure(ErrUnavailable,e)};if poll[0].Revents&(unix.POLLERR|unix.POLLNVAL)!=0{return ErrUnavailable};if _,e=deadline.remaining();e!=nil{return e};if n>0&&poll[0].Revents&(event|unix.POLLHUP)!=0{return nil}}
}
func readOwnedIO(handle int,out []byte,deadline *nativeDeadline)(int,error){if len(out)==0||len(out)>FrameBytes{return 0,ErrInvalid};for{if e:=waitOwnedIO(handle,unix.POLLIN,deadline);e!=nil{return 0,e};n,e:=unix.Read(handle,out);if e==unix.EINTR||e==unix.EAGAIN{continue};if e!=nil{return n,qualifyNativeFailure(ErrUnavailable,e)};if _,e=deadline.remaining();e!=nil{return n,e};return n,nil}}
func writeOwnedIO(handle int,raw []byte,deadline *nativeDeadline)error{if len(raw)==0||len(raw)>FrameBytes{return ErrInvalid};for offset:=0;offset<len(raw);{if e:=waitOwnedIO(handle,unix.POLLOUT,deadline);e!=nil{return e};n,e:=unix.Write(handle,raw[offset:]);if e==unix.EINTR||e==unix.EAGAIN{continue};if e!=nil{return qualifyNativeFailure(ErrUnavailable,e)};if n<=0{return ErrUnavailable};offset+=n;if _,e=deadline.remaining();e!=nil{return e}};return nil}
func readOwnedNativeIO(handle int,memory *nativeMemory,offset,length uint32,deadline *nativeDeadline,owner *originalFrameLease)(result int,resultError error){
 if memory==nil||deadline==nil||length==0||length>FrameBytes{return 0,ErrInvalid};ordinary,e:=borrowFrameOrOrdinary(memory,owner);if e!=nil{return 0,e};defer func(){if ordinary{if e:=memory.returnOriginal();e!=nil{resultError=qualifyNativeFailure(resultError,e)}}}();if uint64(offset)+uint64(length)>uint64(len(memory.bytes)){return 0,ErrInvalid};return readOwnedIO(handle,memory.bytes[offset:offset+length],deadline)
}
func writeOwnedNativeIO(handle int,memory *nativeMemory,offset,length uint32,deadline *nativeDeadline)(resultError error){
 if memory==nil||deadline==nil||length==0||length>FrameBytes{return ErrInvalid};if e:=memory.borrowOriginal();e!=nil{return e};defer func(){if e:=memory.returnOriginal();e!=nil{resultError=qualifyNativeFailure(resultError,e)}}();if uint64(offset)+uint64(length)>uint64(len(memory.bytes)){return ErrInvalid};return writeOwnedIO(handle,memory.bytes[offset:offset+length],deadline)
}
