//go:build linux

package hostadmission

import (
 "crypto/sha256"
 "encoding/binary"
 "sync"
 "unsafe"
 "golang.org/x/sys/unix"
)

const nativePlatform="linux"
const nativeCloseOperation="linux.close"
type objectHandle = int
type processExit struct { ended,naturalZero bool; raw,nativeWait []byte; digest [32]byte }
type nativeProcess struct {
 mu sync.Mutex
 pidfd int
 procDirectory,imageFD,birthFD int
 pid,uid uint32
 birth uint64
 identity,source,image [32]byte
 imageBefore unix.Stat_t
 ownChild bool
 exit processExit
 // Embedded in the original process owner before its first native observation.
 // Calls and returned outputs survive later clock/receipt/identity failures;
 // a failed observation never reuses this one-use original result cell.
 exitAttempted bool
 exitBusy bool
 exitUnresolved bool
 ownershipOwner,ownershipRetained *nativeOwnershipCapture
 exitOriginal processExitOriginal
 exitDeadline exitDeadlineCapture
 exitWaitMilliseconds int
 exitFailure error
 exitPublicationError error
 exitPoll [1]unix.PollFd
 exitPollCount int
 exitClockTick uint64
 exitPollError,exitWaitError,exitClockError,exitReceiptError error
 exitPollCalled,exitPollReturned,exitWaitCalled,exitWaitReturned bool
 exitInfo unix.Siginfo
}
type processExitOriginal struct {
 pidfd,procDirectory,imageFD,birthFD int
 pid,uid uint32
 birth uint64
 identity,source,image [32]byte
 ownChild bool
}
func(p *nativeProcess)exitOriginalUnchanged()bool{
 o:=&p.exitOriginal
 return p.pidfd==o.pidfd&&p.procDirectory==o.procDirectory&&p.imageFD==o.imageFD&&
  p.birthFD==o.birthFD&&p.pid==o.pid&&p.uid==o.uid&&p.birth==o.birth&&
  p.identity==o.identity&&p.source==o.source&&p.image==o.image&&p.ownChild==o.ownChild
}
type nativeClockCapture struct { started,returned bool; now unix.Timespec; nativeError error; tick uint64 }
func nativeTickOriginal(c *nativeClockCapture)(uint64,error){
 if c==nil||c.started{return 0,ErrUnavailable};c.started=true
 e:=unix.ClockGettime(unix.CLOCK_MONOTONIC,&c.now);c.nativeError=e;c.returned=true
 if e!=nil{return 0,e}
 if c.now.Sec<0||c.now.Nsec<0||c.now.Nsec>=1000000000||uint64(c.now.Sec)>(^uint64(0)-uint64(c.now.Nsec))/1000000000{return 0,ErrUnavailable}
 c.tick=uint64(c.now.Sec)*1000000000+uint64(c.now.Nsec);if c.tick==0{return 0,ErrUnavailable};return c.tick,nil
}
func nativeTick()(uint64,error){var original nativeClockCapture;return nativeTickOriginal(&original)}
func nativeClose(handle int)error{return unix.Close(handle)}
func nativeHandleIdentity(handle int,kind byte)([32]byte,error){
 var identity [32]byte;if handle<0||kind>8{return identity,ErrInvalid};var stat unix.Stat_t;if unix.Fstat(handle,&stat)!=nil{return identity,ErrUnavailable}
 flags,e:=unix.FcntlInt(uintptr(handle),unix.F_GETFL,0);if e!=nil{return identity,ErrUnavailable};var raw [36]byte;binary.BigEndian.PutUint64(raw[0:8],uint64(stat.Dev));binary.BigEndian.PutUint64(raw[8:16],stat.Ino);binary.BigEndian.PutUint32(raw[16:20],stat.Mode&unix.S_IFMT);binary.BigEndian.PutUint32(raw[20:24],stat.Uid);binary.BigEndian.PutUint32(raw[24:28],stat.Gid);binary.BigEndian.PutUint32(raw[28:32],uint32(flags)&unix.O_ACCMODE);binary.BigEndian.PutUint32(raw[32:36],uint32(kind));return DomainHash("SLCLI-LINUX-OBJECT-1\x00",raw[:]),nil
}
func(p *nativeProcess)recheckOriginal()error{
 if p==nil||p.pidfd<0||p.procDirectory<0||p.imageFD<0||p.birthFD<0||p.identity==([32]byte{})||p.source==([32]byte{})||p.birth==0{return ErrInvalid}
 p.mu.Lock();defer p.mu.Unlock();if p.exitBusy||p.exitUnresolved||p.ownershipOwner!=nil||p.ownershipRetained!=nil{return ErrPhase};if p.exitAttempted&&p.exitFailure!=nil{return p.exitFailure};if p.exit.ended{return nil}
 var image unix.Stat_t;if unix.Fstat(p.imageFD,&image)!=nil||!sameNativeFile(&p.imageBefore,&image){return ErrInvalid}
 ready,e:=unix.Poll([]unix.PollFd{{Fd:int32(p.pidfd),Events:unix.POLLIN}},0);if e!=nil{return ErrUnavailable};if ready!=0{return ErrClosure}
 // A current executable is read through this already-held original proc task
 // directory while the original pidfd is alive. PID absence/reuse cannot grant
 // identity; another exec into a different inode denies the session.
 // This auxiliary correlation follows only the kernel's fixed exe entry in
 // the original held proc directory. It creates no replacement descriptor and
 // grants no new image authority; the actual original image FD above remains
 // the admitted identity/hash object. Native auxiliary stat evidence must be
 // retained by the complete observer path alongside the original fstat.
 var current unix.Stat_t;if unix.Fstatat(p.procDirectory,"exe",&current,0)!=nil||current.Dev!=p.imageBefore.Dev||current.Ino!=p.imageBefore.Ino{return ErrInvalid}
 birth,_,e:=readHeldOriginalBirth(p.birthFD,p.pid);if e!=nil||birth!=p.birth{return ErrInvalid}
 ready,e=unix.Poll([]unix.PollFd{{Fd:int32(p.pidfd),Events:unix.POLLIN}},0);if e!=nil||ready!=0{return ErrClosure};return nil
}
func(p *nativeProcess)observeOriginalExit(deadline *nativeDeadline)(result processExit,resultError error){
 if p==nil||p.pidfd<0||deadline==nil{return processExit{},ErrInvalid}
 p.mu.Lock();if p.exitBusy||p.exitUnresolved||p.ownershipOwner!=nil||p.ownershipRetained!=nil{p.mu.Unlock();return processExit{},ErrPhase};if p.exitAttempted{out,e:=p.exit,p.exitFailure;if p.exitDeadline.original!=deadline||!p.exitOriginalUnchanged(){e=ErrPhase};p.mu.Unlock();return out,e}
 // The actual original process owner remains exclusively pinned throughout
 // the outside-lock wait and receipt capture. Other native methods deny this
 // occupied owner, so none can inspect partial output or dispose its handles.
 p.exitOriginal=processExitOriginal{pidfd:p.pidfd,procDirectory:p.procDirectory,imageFD:p.imageFD,
  birthFD:p.birthFD,pid:p.pid,uid:p.uid,birth:p.birth,identity:p.identity,source:p.source,image:p.image,ownChild:p.ownChild}
 p.exitBusy=true;p.exitAttempted=true;p.exitFailure=ErrClosure;p.mu.Unlock()
 defer func(){p.mu.Lock();if !p.exitBusy||!p.exitOriginalUnchanged(){p.exitUnresolved=true;p.exitPublicationError=ErrClosure;if resultError==nil{resultError=ErrClosure}}else{p.exitBusy=false};p.mu.Unlock()}()
 remaining,e:=p.exitDeadline.begin(deadline);if e!=nil{p.exitFailure=e;return p.exit,e}
 // Floor the remaining original nanoseconds. Poll's scheduler/granularity
 // behavior is checked by the retained SAME-clock observation after return;
 // no negative/infinite timeout or retry is permitted.
 milliseconds:=remaining/1000000
 if milliseconds>0x7fffffff{p.exitFailure=ErrInvalid;return p.exit,p.exitFailure}
 p.exitWaitMilliseconds=int(milliseconds)
 p.exitPoll[0]=unix.PollFd{Fd:int32(p.exitOriginal.pidfd),Events:unix.POLLIN}
 p.exitPollCalled=true
 ready,e:=unix.Poll(p.exitPoll[:],p.exitWaitMilliseconds)
 p.exitPollCount=ready;p.exitPollError=e;p.exitPollReturned=true
 if e!=nil||ready!=1||p.exitPoll[0].Revents&unix.POLLIN==0||p.exitPoll[0].Revents&(unix.POLLERR|unix.POLLNVAL)!=0||!p.exitOriginal.ownChild{
  if e!=nil{p.exitFailure=ErrUnavailable};_ = p.finishExitDeadline();return p.exit,p.exitFailure
 }
 // Only the original native parent can consume this child's wait status.
 // Non-parent observers retain their original pidfd and correlate this exact
 // native wait receipt; pidfd readability alone is never natural-zero proof.
 p.exitWaitCalled=true
 e=unix.Waitid(unix.P_PIDFD,p.exitOriginal.pidfd,&p.exitInfo,unix.WEXITED|unix.WNOHANG|unix.WNOWAIT,nil)
 p.exitWaitError=e;p.exitWaitReturned=true
 rawInfo:=(*[128]byte)(unsafe.Pointer(&p.exitInfo));p.exit.nativeWait=rawInfo[:]
 clockError:=p.finishExitDeadline()
 if e!=nil{return p.exit,p.exitFailure}
 actualPID:=binary.NativeEndian.Uint32(rawInfo[16:20]);status:=binary.NativeEndian.Uint32(rawInfo[24:28]);if actualPID!=p.exitOriginal.pid||p.exitInfo.Signo!=unix.SIGCHLD||(p.exitInfo.Code!=1&&p.exitInfo.Code!=2&&p.exitInfo.Code!=3){p.exitFailure=ErrInvalid;return p.exit,p.exitFailure}
 p.exit.ended=true;p.exit.naturalZero=p.exitInfo.Code==1&&status==0
 if clockError!=nil{p.exitFailure=clockError;return p.exit,clockError};tick:=p.exitDeadline.after;natural:=p.exit.naturalZero;outcome:="failure";if natural{outcome="success"}
 receipt,e:=primitiveBytes("exit","linux.waitid",outcome,p.exitOriginal.identity,p.exitOriginal.identity,p.exitOriginal.source,tick,nil,nil);p.exitReceiptError=e;if e!=nil{p.exitFailure=e;return p.exit,e}
 // The original raw siginfo is retained beside the closed primitive receipt,
 // so the outcome cannot conceal nonzero/signal/native wait data.
 p.exit.ended=true;p.exit.naturalZero=natural;p.exit.raw=receipt;p.exit.digest=sha256.Sum256(receipt);p.exitFailure=nil;return p.exit,nil
}
// External Node-parent memory has a physical-exit boundary, not a fabricated
// child wait status. A non-parent pidfd observer can establish ended ownership
// while natural-zero qualification still needs the separate original parent
// wait receipt. These are deliberately different claims.
type nativeOwnershipCapture struct {
 started,returned bool
 original *nativeProcess
 handle int
 birth uint64
 identity,source [32]byte
 poll [1]unix.PollFd
 ready int
 nativeError,resultError error
 tuple processExitOriginal
 publicationError error
}
func(p *nativeProcess)ownershipEnded()(bool,error){var capture nativeOwnershipCapture;return p.ownershipEndedOriginal(&capture)}
func(p *nativeProcess)ownershipEndedOriginal(c *nativeOwnershipCapture)(ended bool,result error){
 if p==nil||c==nil||c.started{return false,ErrInvalid}
 p.mu.Lock()
 if p.ownershipOwner!=nil||p.ownershipRetained!=nil{p.mu.Unlock();return false,ErrPhase}
 c.started=true;c.original=p;c.handle=p.pidfd;c.birth=p.birth;c.identity=p.identity;c.source=p.source
 c.tuple=processExitOriginal{pidfd:p.pidfd,procDirectory:p.procDirectory,imageFD:p.imageFD,birthFD:p.birthFD,pid:p.pid,uid:p.uid,birth:p.birth,identity:p.identity,source:p.source,image:p.image,ownChild:p.ownChild}
 if c.handle<0||c.identity==([32]byte{})||c.source==([32]byte{})||c.birth==0{c.resultError=ErrInvalid;p.mu.Unlock();return false,ErrInvalid}
 if p.exitBusy||p.exitUnresolved||p.ownershipOwner!=nil||p.ownershipRetained!=nil{c.resultError=ErrPhase;p.mu.Unlock();return false,ErrPhase}
 p.ownershipOwner=c;p.mu.Unlock()
 defer func(){
  c.resultError=result
  p.mu.Lock();defer p.mu.Unlock()
  current:=processExitOriginal{pidfd:p.pidfd,procDirectory:p.procDirectory,imageFD:p.imageFD,birthFD:p.birthFD,pid:p.pid,uid:p.uid,birth:p.birth,identity:p.identity,source:p.source,image:p.image,ownChild:p.ownChild}
  if p.ownershipOwner!=c||current!=c.tuple{c.publicationError=ErrClosure;p.ownershipRetained=c;if result==nil{result=ErrClosure;ended=false}}else{p.ownershipOwner=nil}
  if result!=nil{p.ownershipRetained=c}
 }()
 c.poll[0]=unix.PollFd{Fd:int32(c.handle),Events:unix.POLLIN}
 c.ready,c.nativeError=unix.Poll(c.poll[:],0);c.returned=true
 if c.nativeError!=nil||c.poll[0].Revents&(unix.POLLNVAL|unix.POLLERR)!=0{return false,ErrUnavailable}
 return c.ready==1&&c.poll[0].Revents&unix.POLLIN!=0,nil
}
func nativeRead(handle int,bytes []byte)(int,error){for{n,e:=unix.Read(handle,bytes);if e==unix.EINTR{continue};return n,e}}
func nativeWrite(handle int,bytes []byte)(int,error){for{n,e:=unix.Write(handle,bytes);if e==unix.EINTR{continue};return n,e}}
func nativeFlush(handle int)error{return unix.Fsync(handle)}
func nativeHandleDuplicate(handle int)(int,error){return unix.FcntlInt(uintptr(handle),unix.F_DUPFD_CLOEXEC,0)}
