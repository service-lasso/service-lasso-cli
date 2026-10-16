//go:build linux || windows

package hostadmission

// The installer-owned local access class is immutable admitted grant data.
// Its native endpoint ACL is independently checked at endpoint creation; being
// named here alone neither installs access nor proves a source image.
func admittedNativeClient(profile [32]byte,owner string)bool{matches:=0;grants:=0;for _,grant:=range nativeGrants{if grant.profileDigest!=profile{continue};grants++;for _,allowed:=range grant.clientUsers{if allowed==owner{matches++}}};return grants==1&&matches==1}
