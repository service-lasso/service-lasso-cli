//go:build linux

package hostadmission

import "golang.org/x/sys/unix"

func correlateOriginalFramePeer(channel int,original *nativeProcess)error{
 if original==nil{return ErrInvalid};kind,e:=unix.GetsockoptInt(channel,unix.SOL_SOCKET,unix.SO_TYPE);if e!=nil||kind!=unix.SOCK_STREAM{return ErrInvalid}
 address,e:=unix.Getsockname(channel);if e!=nil{return ErrInvalid};if _,ok:=address.(*unix.SockaddrUnix);!ok{return ErrInvalid}
 credential,e:=unix.GetsockoptUcred(channel,unix.SOL_SOCKET,unix.SO_PEERCRED)
 if e!=nil||credential.Pid<=0||uint32(credential.Pid)!=original.pid||credential.Uid!=original.uid{return ErrInvalid}
 return original.recheckOriginal()
}
