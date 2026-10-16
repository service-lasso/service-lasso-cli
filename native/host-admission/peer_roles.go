//go:build linux || windows

package hostadmission

const originalNodePeer byte=0
const originalObserverPeer byte=1
const originalFacadePeer byte=2

// Role selection is private source control, never a requested actor/image or
// PID. The profile must already be the independently authenticated original.
func originalPeerImage(profile *Profile,role byte)(string,error){
 var e error;profile,e=originalProfile(profile);if e!=nil{return "",e}
 switch role{case originalNodePeer:return profile.Launch.Library.NodeImageSHA256,nil;case originalObserverPeer:return profile.Launch.Observer.ImageSHA256,nil;case originalFacadePeer:return profile.Launch.Facade.ImageSHA256,nil;default:return "",ErrInvalid}
}
func originalPeerOwnerAllowed(profile *Profile,role byte,owner string)bool{
 var e error;profile,e=originalProfile(profile);if e!=nil||!sourceOwnerIdentity(owner){return false}
 switch role{case originalNodePeer,originalFacadePeer:return admittedNativeClient(profile.Digest,owner);case originalObserverPeer:return owner==profile.Owner;default:return false}
}
func originalPeerIdentityDomain(role byte)(string,error){
 if nativePlatform=="linux"{switch role{case originalNodePeer:return "SLCLI-LINUX-PARENT-1\x00",nil;case originalObserverPeer:return "SLCLI-LINUX-OBSERVER-1\x00",nil;case originalFacadePeer:return "SLCLI-LINUX-FACADE-1\x00",nil}}
 if nativePlatform=="win32"{switch role{case originalNodePeer:return "SLCLI-WINDOWS-PARENT-1\x00",nil;case originalObserverPeer:return "SLCLI-WINDOWS-OBSERVER-1\x00",nil;case originalFacadePeer:return "SLCLI-WINDOWS-FACADE-1\x00",nil}}
 return "",ErrInvalid
}
