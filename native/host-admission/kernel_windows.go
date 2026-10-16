//go:build windows

package hostadmission

import (
 "crypto/sha256"
 "encoding/binary"
 "sync"
 "syscall"
 "unsafe"
 "golang.org/x/sys/windows"
)

const nativePlatform="win32"
const nativeCloseOperation="win32.close"
type objectHandle = windows.Handle
type processExit struct { ended,naturalZero bool; raw,nativeWait []byte; digest [32]byte }
type nativeProcess struct {
 mu sync.Mutex
 handle,imageHandle windows.Handle
 ownerToken windows.Token
 ownerTokenState [56]byte
 pid uint32
 birth windows.Filetime
 owner string
 identity,source,image [32]byte
 imageBefore windows.ByHandleFileInformation
 imageIdentity [24]byte
 exit processExit
 // Original owner storage, reserved before the once-only native exit query.
 // Native outputs are retained before any subsequent diagnostic can fail.
 exitAttempted bool
 exitBusy bool
 exitUnresolved bool
 ownershipOwner,ownershipRetained *nativeOwnershipCapture
 exitOriginal processExitOriginal
 exitDeadline exitDeadlineCapture
 exitWaitMilliseconds uint32
 exitFailure error
 exitPublicationError error
 exitWaitState uint32
 exitCode uint32
 exitClockTick uint64
 exitCreation,exitTime,exitKernel,exitUser windows.Filetime
 exitWaitError,exitCodeError,exitTimesError,exitClockError,exitReceiptError error
 exitWaitCalled,exitWaitReturned,exitCodeCalled,exitCodeReturned,exitTimesCalled,exitTimesReturned bool
 exitNative [28]byte
}
type processExitOriginal struct {
 handle,imageHandle windows.Handle
 ownerToken windows.Token
 ownerTokenState [56]byte
 pid uint32
 birth windows.Filetime
 owner string
 identity,source,image [32]byte
 imageIdentity [24]byte
}
func(p *nativeProcess)exitOriginalUnchanged()bool{
 o:=&p.exitOriginal
 return p.handle==o.handle&&p.imageHandle==o.imageHandle&&p.ownerToken==o.ownerToken&&
  p.ownerTokenState==o.ownerTokenState&&p.pid==o.pid&&p.birth==o.birth&&p.owner==o.owner&&
  p.identity==o.identity&&p.source==o.source&&p.image==o.image&&p.imageIdentity==o.imageIdentity
}
var objectKernel=windows.NewLazySystemDLL("kernel32.dll")
var counterCall=objectKernel.NewProc("QueryPerformanceCounter")
var frequencyCall=objectKernel.NewProc("QueryPerformanceFrequency")
var compareObjectCall=objectKernel.NewProc("CompareObjectHandles")
type nativeClockCapture struct {
 started,countReturned,frequencyReturned bool
 count,frequency int64
 countResult,countAux,frequencyResult,frequencyAux uintptr
 countError,frequencyError error
 tick uint64
}
func nativeTickOriginal(c *nativeClockCapture)(uint64,error){
 if c==nil||c.started{return 0,ErrUnavailable};c.started=true
 result,aux,e:=counterCall.Call(uintptr(unsafe.Pointer(&c.count)))
 c.countResult=result;c.countAux=aux;c.countError=e;c.countReturned=true
 result,aux,e=frequencyCall.Call(uintptr(unsafe.Pointer(&c.frequency)))
 c.frequencyResult=result;c.frequencyAux=aux;c.frequencyError=e;c.frequencyReturned=true
 if c.countResult==0||c.frequencyResult==0||c.count<=0||c.frequency<=0{return 0,ErrUnavailable}
 seconds:=uint64(c.count/c.frequency);remaining:=uint64(c.count%c.frequency)
 if seconds>^uint64(0)/1000000000||remaining>^uint64(0)/1000000000{return 0,ErrUnavailable}
 whole:=seconds*1000000000;fraction:=remaining*1000000000/uint64(c.frequency)
 if fraction>^uint64(0)-whole{return 0,ErrUnavailable};c.tick=whole+fraction
 if c.tick==0{return 0,ErrUnavailable};return c.tick,nil
}
func nativeTick()(uint64,error){var original nativeClockCapture;return nativeTickOriginal(&original)}
func nativeClose(handle windows.Handle)error{return windows.CloseHandle(handle)}

// Pipes/channels have no portable disk file ID. Their private identity is
// creation-bound and each later observation checks the actual original kernel
// object against an independently retained duplicate using CompareObjectHandles.
// A numeric handle or a supplied pipe name can never enroll an identity.
type pipeWitness struct { original,duplicate windows.Handle; identity [32]byte; occupied bool }
var pipeWitnesses [MaximumBindings*MaximumReferences]pipeWitness
var pipeWitnessMu sync.Mutex
func retainPipeWitness(original windows.Handle,run [32]byte,ordinal uint32)([32]byte,error){
 var identity [32]byte;if original==0||run==([32]byte{}){return identity,ErrInvalid}
 // Reserve original custody before creating the duplicate. A later failed
 // observation retains this cell and its native handle; no unchecked close
 // loses an uncertain object and no replacement may reuse the original cell.
 pipeWitnessMu.Lock();for i:=range pipeWitnesses{if pipeWitnesses[i].occupied&&pipeWitnesses[i].original==original{pipeWitnessMu.Unlock();return identity,ErrPhase}};var witness *pipeWitness;for i:=range pipeWitnesses{if !pipeWitnesses[i].occupied{witness=&pipeWitnesses[i];witness.original=original;witness.occupied=true;break}};pipeWitnessMu.Unlock();if witness==nil{return identity,ErrCapacity}
 kind,e:=windows.GetFileType(original);if e!=nil||kind!=windows.FILE_TYPE_PIPE{return identity,ErrInvalid}
 process:=windows.CurrentProcess();var duplicate windows.Handle;e=windows.DuplicateHandle(process,original,process,&duplicate,0,false,windows.DUPLICATE_SAME_ACCESS)
 pipeWitnessMu.Lock();witness.duplicate=duplicate;pipeWitnessMu.Unlock();if e!=nil||duplicate==0{return identity,ErrUnavailable}
 tick,e:=nativeTick();if e!=nil{return identity,e};var context [12]byte;binary.BigEndian.PutUint32(context[:4],ordinal);binary.BigEndian.PutUint64(context[4:],tick);identity=DomainHash("SLCLI-WINDOWS-PIPE-1\x00",run[:],context[:])
 pipeWitnessMu.Lock();witness.identity=identity;pipeWitnessMu.Unlock();return identity,nil
}
func nativeHandleIdentity(handle windows.Handle,kind byte)([32]byte,error){
 var identity [32]byte;fileType,e:=windows.GetFileType(handle);if e!=nil{return identity,ErrUnavailable}
 if fileType==windows.FILE_TYPE_PIPE{pipeWitnessMu.Lock();defer pipeWitnessMu.Unlock();for i:=range pipeWitnesses{witness:=&pipeWitnesses[i];if witness.occupied&&witness.original==handle&&witness.duplicate!=0&&witness.identity!=([32]byte{}){same,_,_:=compareObjectCall.Call(uintptr(handle),uintptr(witness.duplicate));if same==0{return identity,ErrInvalid};return witness.identity,nil}};return identity,ErrInvalid}
 if fileType!=windows.FILE_TYPE_DISK{return identity,ErrInvalid};var info windows.ByHandleFileInformation;if windows.GetFileInformationByHandle(handle,&info)!=nil||info.FileAttributes&windows.FILE_ATTRIBUTE_REPARSE_POINT!=0{return identity,ErrInvalid}
 fileID,e:=nativeDiskIdentity(handle);if e!=nil{return identity,e};var raw [29]byte;copy(raw[:24],fileID[:]);binary.BigEndian.PutUint32(raw[24:28],info.FileAttributes&windows.FILE_ATTRIBUTE_DIRECTORY);raw[28]=kind;return DomainHash("SLCLI-WINDOWS-OBJECT-1\x00",raw[:]),nil
}
func nativeDiskIdentity(handle windows.Handle)([24]byte,error){
 var result [24]byte;var aligned [3]uint64;raw:=(*[24]byte)(unsafe.Pointer(&aligned[0]))
 if e:=windows.GetFileInformationByHandleEx(handle,windows.FileIdInfo,&raw[0],24);e!=nil{return result,ErrUnavailable};copy(result[:],raw[:])
 zero:=true;for _,b:=range result[8:]{if b!=0{zero=false;break}};if zero{return [24]byte{},ErrUnavailable};return result,nil
}
func originalOwnerTokenState(token windows.Token)([56]byte,error){
 var state [56]byte;var aligned [7]uint64;var returned uint32
 if token==0{return state,ErrInvalid};e:=windows.GetTokenInformation(token,windows.TokenStatistics,(*byte)(unsafe.Pointer(&aligned[0])),56,&returned);if e!=nil||returned!=56{return state,ErrUnavailable};copy(state[:],unsafe.Slice((*byte)(unsafe.Pointer(&aligned[0])),56));return state,nil
}
func(p *nativeProcess)recheckOriginal()error{
 if p==nil||p.handle==0||p.imageHandle==0||p.identity==([32]byte{})||p.source==([32]byte{}){return ErrInvalid};p.mu.Lock();defer p.mu.Unlock();if p.exitBusy||p.exitUnresolved||p.ownershipOwner!=nil||p.ownershipRetained!=nil{return ErrPhase};if p.exitAttempted&&p.exitFailure!=nil{return p.exitFailure};if p.exit.ended{return nil}
 var creation,exit,kernel,user windows.Filetime;if windows.GetProcessTimes(p.handle,&creation,&exit,&kernel,&user)!=nil||creation!=p.birth{return ErrInvalid}
 // Reobserve the original retained token; a newly opened token cannot replace
 // this custody. Current process-to-token correlation remains a separate native
 // observer obligation, not a consequence of TokenStatistics equality.
 state,e:=originalOwnerTokenState(p.ownerToken);if e!=nil||state!=p.ownerTokenState{return ErrInvalid};actual,e:=p.ownerToken.GetTokenUser();if e!=nil||actual.User.Sid.String()!=p.owner{return ErrInvalid};after,e:=originalOwnerTokenState(p.ownerToken);if e!=nil||after!=state{return ErrInvalid}
 var image windows.ByHandleFileInformation;if windows.GetFileInformationByHandle(p.imageHandle,&image)!=nil||!sameWindowsFile(p.imageBefore,image){return ErrInvalid}
 fileID,e:=nativeDiskIdentity(p.imageHandle);if e!=nil||fileID!=p.imageIdentity{return ErrInvalid}
 waitState,e:=windows.WaitForSingleObject(p.handle,0);if e!=nil{return ErrUnavailable};if waitState!=windows.WAIT_TIMEOUT{return ErrClosure};return nil
}
func(p *nativeProcess)observeOriginalExit(deadline *nativeDeadline)(result processExit,resultError error){
 if p==nil||p.handle==0||deadline==nil{return processExit{},ErrInvalid}
 p.mu.Lock();if p.exitBusy||p.exitUnresolved||p.ownershipOwner!=nil||p.ownershipRetained!=nil{p.mu.Unlock();return processExit{},ErrPhase};if p.exitAttempted{out,e:=p.exit,p.exitFailure;if p.exitDeadline.original!=deadline||!p.exitOriginalUnchanged(){e=ErrPhase};p.mu.Unlock();return out,e}
 p.exitOriginal=processExitOriginal{handle:p.handle,imageHandle:p.imageHandle,ownerToken:p.ownerToken,
  ownerTokenState:p.ownerTokenState,pid:p.pid,birth:p.birth,owner:p.owner,
  identity:p.identity,source:p.source,image:p.image,imageIdentity:p.imageIdentity}
 p.exitBusy=true;p.exitAttempted=true;p.exitFailure=ErrClosure;p.exit.nativeWait=p.exitNative[:];p.mu.Unlock()
 defer func(){p.mu.Lock();if !p.exitBusy||!p.exitOriginalUnchanged(){p.exitUnresolved=true;p.exitPublicationError=ErrClosure;if resultError==nil{resultError=ErrClosure}}else{p.exitBusy=false};p.mu.Unlock()}()
 remaining,e:=p.exitDeadline.begin(deadline);if e!=nil{p.exitFailure=e;return p.exit,e}
 milliseconds:=remaining/1000000
 if milliseconds>=uint64(windows.INFINITE){p.exitFailure=ErrInvalid;return p.exit,p.exitFailure}
 p.exitWaitMilliseconds=uint32(milliseconds)
 p.exitWaitCalled=true
 state,e:=windows.WaitForSingleObject(p.exitOriginal.handle,p.exitWaitMilliseconds)
 p.exitWaitState=state;p.exitWaitError=e;p.exitWaitReturned=true
 binary.BigEndian.PutUint32(p.exitNative[:4],state);binary.BigEndian.PutUint32(p.exitNative[8:12],p.exitOriginal.pid)
 if e!=nil||state!=windows.WAIT_OBJECT_0{_ = p.finishExitDeadline();return p.exit,p.exitFailure}
 p.exitCodeCalled=true
 e=windows.GetExitCodeProcess(p.exitOriginal.handle,&p.exitCode)
 p.exitCodeError=e;p.exitCodeReturned=true;binary.BigEndian.PutUint32(p.exitNative[4:8],p.exitCode)
 if e!=nil{_ = p.finishExitDeadline();return p.exit,p.exitFailure}
 p.exitTimesCalled=true
 e=windows.GetProcessTimes(p.exitOriginal.handle,&p.exitCreation,&p.exitTime,&p.exitKernel,&p.exitUser)
 p.exitTimesError=e;p.exitTimesReturned=true
 binary.BigEndian.PutUint32(p.exitNative[12:16],p.exitCreation.HighDateTime);binary.BigEndian.PutUint32(p.exitNative[16:20],p.exitCreation.LowDateTime);binary.BigEndian.PutUint32(p.exitNative[20:24],p.exitTime.HighDateTime);binary.BigEndian.PutUint32(p.exitNative[24:28],p.exitTime.LowDateTime)
 clockError:=p.finishExitDeadline()
 if e!=nil||p.exitCreation!=p.exitOriginal.birth||p.exitTime==(windows.Filetime{}){p.exitFailure=ErrInvalid;return p.exit,p.exitFailure}
 p.exit.ended=true;p.exit.naturalZero=p.exitCode==0
 if clockError!=nil{p.exitFailure=clockError;return p.exit,clockError};tick:=p.exitDeadline.after;outcome:="failure";if p.exitCode==0{outcome="success"};receipt,e:=primitiveBytes("exit","win32.process_wait",outcome,p.exitOriginal.identity,p.exitOriginal.identity,p.exitOriginal.source,tick,nil,nil);p.exitReceiptError=e;if e!=nil{p.exitFailure=e;return p.exit,e}
 // The original native wait status/exit code/birth/exit times are retained,
 // not replaced with an aggregate successful-exit boolean.
 p.exit.ended=true;p.exit.naturalZero=p.exitCode==0;p.exit.raw=receipt;p.exit.digest=sha256.Sum256(receipt);p.exitFailure=nil;return p.exit,nil
}
type nativeOwnershipCapture struct {
 started,timesReturned,waitReturned bool
 original *nativeProcess
 handle windows.Handle
 birth,creation,exit,kernel,user windows.Filetime
 identity,source [32]byte
 state uint32
 timesError,waitError,resultError error
 tuple processExitOriginal
 publicationError error
}
func(p *nativeProcess)ownershipEnded()(bool,error){var capture nativeOwnershipCapture;return p.ownershipEndedOriginal(&capture)}
func(p *nativeProcess)ownershipEndedOriginal(c *nativeOwnershipCapture)(ended bool,result error){
 if p==nil||c==nil||c.started{return false,ErrInvalid}
 p.mu.Lock()
 if p.ownershipOwner!=nil||p.ownershipRetained!=nil{p.mu.Unlock();return false,ErrPhase}
 c.started=true;c.original=p;c.handle=p.handle;c.birth=p.birth;c.identity=p.identity;c.source=p.source
 c.tuple=processExitOriginal{handle:p.handle,imageHandle:p.imageHandle,ownerToken:p.ownerToken,ownerTokenState:p.ownerTokenState,pid:p.pid,birth:p.birth,owner:p.owner,identity:p.identity,source:p.source,image:p.image,imageIdentity:p.imageIdentity}
 if c.handle==0||c.identity==([32]byte{})||c.source==([32]byte{}){c.resultError=ErrInvalid;p.mu.Unlock();return false,ErrInvalid}
 if p.exitBusy||p.exitUnresolved||p.ownershipOwner!=nil||p.ownershipRetained!=nil{c.resultError=ErrPhase;p.mu.Unlock();return false,ErrPhase}
 p.ownershipOwner=c;p.mu.Unlock()
 defer func(){
  c.resultError=result
  p.mu.Lock();defer p.mu.Unlock()
  current:=processExitOriginal{handle:p.handle,imageHandle:p.imageHandle,ownerToken:p.ownerToken,ownerTokenState:p.ownerTokenState,pid:p.pid,birth:p.birth,owner:p.owner,identity:p.identity,source:p.source,image:p.image,imageIdentity:p.imageIdentity}
  if p.ownershipOwner!=c||current!=c.tuple{c.publicationError=ErrClosure;p.ownershipRetained=c;if result==nil{result=ErrClosure;ended=false}}else{p.ownershipOwner=nil}
  if result!=nil{p.ownershipRetained=c}
 }()
 c.timesError=windows.GetProcessTimes(c.handle,&c.creation,&c.exit,&c.kernel,&c.user);c.timesReturned=true
 if c.timesError!=nil||c.creation!=c.birth{return false,ErrInvalid}
 c.state,c.waitError=windows.WaitForSingleObject(c.handle,0);c.waitReturned=true
 if c.waitError!=nil{return false,ErrUnavailable}
 return c.state==windows.WAIT_OBJECT_0&&c.exit!=(windows.Filetime{}),nil
}
func nativeRead(handle windows.Handle,bytes []byte)(int,error){var n uint32;e:=windows.ReadFile(handle,bytes,&n,nil);if e==windows.ERROR_BROKEN_PIPE||e==windows.ERROR_HANDLE_EOF{return int(n),nil};return int(n),e}
func nativeWrite(handle windows.Handle,bytes []byte)(int,error){var n uint32;e:=windows.WriteFile(handle,bytes,&n,nil);return int(n),e}
func nativeFlush(handle windows.Handle)error{return windows.FlushFileBuffers(handle)}
func nativeHandleDuplicate(handle windows.Handle)(windows.Handle,error){var copy windows.Handle;process:=windows.CurrentProcess();e:=windows.DuplicateHandle(process,handle,process,&copy,0,false,windows.DUPLICATE_SAME_ACCESS);return copy,e}
var _ = syscall.Handle(0)
