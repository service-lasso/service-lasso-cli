//go:build linux

package hostadmission

import "golang.org/x/sys/unix"

func verifyOriginalKeyPermissions(handle int,metadata sourceObjectMetadata)error{
 var stat unix.Stat_t
 if unix.Fstat(handle,&stat)!=nil||stat.Mode&unix.S_IFMT!=unix.S_IFREG||stat.Mode&07777!=0600||stat.Size!=32||metadata.size!=32{return ErrInvalid}
 owner:=generationString(uint64(stat.Uid));if owner!=metadata.owner{return ErrInvalid}
 return nil
}
