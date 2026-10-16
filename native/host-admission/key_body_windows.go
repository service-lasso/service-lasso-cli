//go:build windows

package hostadmission

import "golang.org/x/sys/windows"

func verifyOriginalKeyPermissions(handle windows.Handle,metadata sourceObjectMetadata)error{
 if metadata.size!=32||metadata.owner==""{return ErrInvalid}
 info,e:=winInfo(handle,false,metadata.owner);if e!=nil||winSize(info)!=32{return ErrInvalid}
 return nil
}
