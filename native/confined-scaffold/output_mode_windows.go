//go:build windows

package main

import (
 "fmt"
 "os"
 "syscall"
 "sync"
 "unsafe"
 "golang.org/x/sys/windows"
)

const ownerNonExecuteDACL="D:P(A;;0x001f01df;;;OW)"
var outputAccessCheck=syscall.NewLazyDLL("advapi32.dll").NewProc("AccessCheck")
type outputGenericMapping struct{read,write,execute,all uint32}
var originalOutputOwner struct{mu sync.Mutex; attempted,ready,closeAttempted bool; token,effective windows.Token; owner *windows.SID; tokenState,effectiveState [56]byte; tokenClose,effectiveClose error}

func outputTokenState(token windows.Token)([56]byte,error){var aligned [7]uint64;var state [56]byte;var returned uint32;e:=windows.GetTokenInformation(token,windows.TokenStatistics,(*byte)(unsafe.Pointer(&aligned[0])),uint32(len(state)),&returned);if e!=nil||returned!=uint32(len(state)){return state,fmt.Errorf("output original token state unavailable")};copy(state[:],unsafe.Slice((*byte)(unsafe.Pointer(&aligned[0])),len(state)));return state,nil}

// The single writer attempt retains these two original tokens before candidate
// creation. Native observer/accounting integration must classify the originals;
// failure keeps both actual handles, rather than retrying/replacing the owner.
func initializeOutputModeOwner()error{
 originalOutputOwner.mu.Lock();defer originalOutputOwner.mu.Unlock();if originalOutputOwner.attempted{return fmt.Errorf("output owner attempt already consumed")};originalOutputOwner.attempted=true
 if windows.OpenProcessToken(windows.CurrentProcess(),windows.TOKEN_QUERY|windows.TOKEN_DUPLICATE,&originalOutputOwner.token)!=nil{return fmt.Errorf("output owner token unavailable")}
 state,e:=outputTokenState(originalOutputOwner.token);if e!=nil{return e};originalOutputOwner.tokenState=state
 user,e:=originalOutputOwner.token.GetTokenUser();if e!=nil{return e};originalOutputOwner.owner=user.User.Sid
 if windows.DuplicateTokenEx(originalOutputOwner.token,windows.TOKEN_QUERY,nil,windows.SecurityImpersonation,windows.TokenImpersonation,&originalOutputOwner.effective)!=nil{return fmt.Errorf("output effective token unavailable")}
 state,e=outputTokenState(originalOutputOwner.effective);if e!=nil{return e};originalOutputOwner.effectiveState=state
 effectiveUser,e:=originalOutputOwner.effective.GetTokenUser();if e!=nil||!effectiveUser.User.Sid.Equals(originalOutputOwner.owner){return fmt.Errorf("output effective owner differs")};originalOutputOwner.ready=true;return nil
}

func closeOutputModeOwner()error{
 originalOutputOwner.mu.Lock();defer originalOutputOwner.mu.Unlock()
 if !originalOutputOwner.ready||originalOutputOwner.closeAttempted{return fmt.Errorf("output owner close phase invalid")};originalOutputOwner.closeAttempted=true;originalOutputOwner.ready=false
 // Exactly one close per original. Keep both raw original identities and close
 // outcomes in this private context; uncertainty never retries a handle number.
 // The complete native observer must retain the corresponding close receipts.
 originalOutputOwner.effectiveClose=originalOutputOwner.effective.Close()
 originalOutputOwner.tokenClose=originalOutputOwner.token.Close()
 if originalOutputOwner.effectiveClose!=nil||originalOutputOwner.tokenClose!=nil{return fmt.Errorf("output owner native closure unresolved")};return nil
}

func createOutputFile(name string,parent syscall.Handle,access,options uint32,mode os.FileMode)(syscall.Handle,error){
 dacl:=privateOwnerDacl;if mode==0644{dacl=ownerNonExecuteDACL}else if mode!=0755{return 0,fmt.Errorf("invalid output mode")}
 descriptor,e:=windows.SecurityDescriptorFromString(dacl);if e!=nil{return 0,e}
 return nativeOpen(name,parent,access|readControl,fileCreate,options,descriptor)
}

// The returned enum is derived from the original held file's actual DACL and
// native effective owner-token execute check. It is never a requested mode echo
// or a claim that Windows implements POSIX permissions. Qualification must
// retain this original descriptor/token observation with native file identity.
func observeOutputMode(handle syscall.Handle)(os.FileMode,error){
 originalOutputOwner.mu.Lock();defer originalOutputOwner.mu.Unlock();if !originalOutputOwner.ready{return 0,fmt.Errorf("output owner not initialized")}
 state,e:=outputTokenState(originalOutputOwner.token);if e!=nil||state!=originalOutputOwner.tokenState{return 0,fmt.Errorf("output original token state changed")};state,e=outputTokenState(originalOutputOwner.effective);if e!=nil||state!=originalOutputOwner.effectiveState{return 0,fmt.Errorf("output effective token state changed")}
 kind,e:=windows.GetFileType(windows.Handle(handle));if e!=nil||kind!=windows.FILE_TYPE_DISK{return 0,fmt.Errorf("output is not a disk file")}
 var info windows.ByHandleFileInformation;if windows.GetFileInformationByHandle(windows.Handle(handle),&info)!=nil||info.FileAttributes&(windows.FILE_ATTRIBUTE_DIRECTORY|windows.FILE_ATTRIBUTE_REPARSE_POINT|windows.FILE_ATTRIBUTE_READONLY)!=0{return 0,fmt.Errorf("output file attributes are unsafe")}
 descriptor,e:=windows.GetSecurityInfo(windows.Handle(handle),windows.SE_FILE_OBJECT,windows.OWNER_SECURITY_INFORMATION|windows.GROUP_SECURITY_INFORMATION|windows.DACL_SECURITY_INFORMATION)
 if e!=nil||descriptor==nil||!descriptor.IsValid(){return 0,fmt.Errorf("output security observation failed")}
 control,_,e:=descriptor.Control();if e!=nil||control&windows.SE_DACL_PROTECTED==0{return 0,fmt.Errorf("output DACL is not protected")}
 acl,defaulted,e:=descriptor.DACL();if e!=nil||acl==nil||defaulted||acl.AceCount!=1{return 0,fmt.Errorf("output DACL is not exact")}
 var ace *windows.ACCESS_ALLOWED_ACE;if windows.GetAce(acl,0,&ace)!=nil||ace==nil||ace.Header.AceType!=windows.ACCESS_ALLOWED_ACE_TYPE||ace.Header.AceFlags!=0{return 0,fmt.Errorf("output ACE is not exact")}
 ownerRights,e:=windows.StringToSid("S-1-3-4");if e!=nil{return 0,e};sid:=(*windows.SID)(unsafe.Pointer(&ace.SidStart))
 if !sid.Equals(ownerRights)||int(ace.Header.AceSize)!=8+ownerRights.Len(){return 0,fmt.Errorf("output ACE owner is not exact")}
 var mode os.FileMode;switch uint32(ace.Mask){case 0x001f01df:mode=0644;case 0x001f01ff:mode=0755;default:return 0,fmt.Errorf("output ACE access is not exact")}
 user,e:=originalOutputOwner.token.GetTokenUser();if e!=nil||!user.User.Sid.Equals(originalOutputOwner.owner){return 0,fmt.Errorf("output original token changed")};owner,_,e:=descriptor.Owner();if e!=nil||owner==nil||!owner.Equals(originalOutputOwner.owner){return 0,fmt.Errorf("output owner changed")}
 effectiveUser,e:=originalOutputOwner.effective.GetTokenUser();if e!=nil||!effectiveUser.User.Sid.Equals(originalOutputOwner.owner){return 0,fmt.Errorf("output effective owner changed")}
 // AccessCheck requires an impersonation token and a fully populated security
 // descriptor. Desired FILE_EXECUTE has no generic access bits to reinterpret.
 // The fixed, aligned privilege buffer is reserved before the single native
 // call; insufficient size denies and is not retried with unbounded growth.
 mapping:=outputGenericMapping{windows.FILE_GENERIC_READ,windows.FILE_GENERIC_WRITE,windows.FILE_GENERIC_EXECUTE,0x001f01ff}
 var privileges [64]uint64;privilegeBytes:=uint32(unsafe.Sizeof(privileges));var granted,status uint32
 ok,_,_:=outputAccessCheck.Call(uintptr(unsafe.Pointer(descriptor)),uintptr(originalOutputOwner.effective),uintptr(windows.FILE_EXECUTE),uintptr(unsafe.Pointer(&mapping)),uintptr(unsafe.Pointer(&privileges[0])),uintptr(unsafe.Pointer(&privilegeBytes)),uintptr(unsafe.Pointer(&granted)),uintptr(unsafe.Pointer(&status)))
 if ok==0||privilegeBytes>uint32(unsafe.Sizeof(privileges))||*(*uint32)(unsafe.Pointer(&privileges[0]))!=0||mode==0644&&(status!=0||granted!=0)||mode==0755&&(status==0||granted&windows.FILE_EXECUTE==0){return 0,fmt.Errorf("output execute observation disagrees")}
 state,e=outputTokenState(originalOutputOwner.token);if e!=nil||state!=originalOutputOwner.tokenState{return 0,fmt.Errorf("output original token state changed")};state,e=outputTokenState(originalOutputOwner.effective);if e!=nil||state!=originalOutputOwner.effectiveState{return 0,fmt.Errorf("output effective token state changed")}
 return mode,nil
}
