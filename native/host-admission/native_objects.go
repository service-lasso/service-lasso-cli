//go:build linux || windows

package hostadmission

import (
 "crypto/sha256"
 "encoding/json"
 "sync"
)

type nativePrimitive struct {
 Kind string `json:"kind"`
 Platform string `json:"platform"`
 ObjectIdentityDigest string `json:"objectIdentityDigest"`
 MonotonicTick string `json:"monotonicTick"`
 Operation string `json:"operation"`
 Outcome string `json:"outcome"`
 BytesSHA256 *string `json:"bytesSha256"`
 ByteCount *uint64 `json:"byteCount"`
 Mode *string `json:"mode"`
 RelatedIdentityDigest string `json:"relatedIdentityDigest"`
 SourceImageSHA256 string `json:"sourceImageSha256"`
}
type nativeTerminal struct { closed,naturalZero bool; digest [32]byte }
func(p *nativeProcess)finishExitDeadline()error{
 e:=p.exitDeadline.finish()
 p.exitClockTick=p.exitDeadline.after;p.exitClockError=e
 return e
}
type nativeObject struct {
 mu sync.Mutex
 handle objectHandle
 identity,source [32]byte
 kind byte
 // Set only by the genuine private original constructor from the selected
 // primary terminal edge. Parsed kind/membership claims cannot set this fact.
 primaryTerminalEdge bool
 terminalNaturalZero bool
 ended bool
 closeAttempted bool
 closeRaw []byte
 closeDigest [32]byte
 process *nativeProcess
 buffer *nativeMemory
 latestRaw []byte
 latestDigest [32]byte
 sourceDomain *nativeAccounting
 // Exclusive original terminal-effect owner. These fields are embedded before
 // the effect; occupancy blocks recheck/close readers while native results are
 // retained outside the short object metadata lock.
 terminalAttempted,terminalBusy,terminalUnresolved,terminalResultReturned bool
 terminalProcessOriginal *nativeProcess
 terminalDeadlineOriginal *nativeDeadline
 terminalIdentityOriginal,terminalSourceOriginal [32]byte
 terminalHandleOriginal objectHandle
 terminalKindOriginal byte
 terminalBufferOriginal *nativeMemory
 terminalAccountingOriginal *nativeAccounting
 terminalResult processExit
 terminalResultError error
 terminalPublicationError error
}
func(n *nativeObject)terminalOriginalUnchangedLocked(deadline *nativeDeadline)bool{
 return n!=nil&&n.process==n.terminalProcessOriginal&&deadline==n.terminalDeadlineOriginal&&
  n.identity==n.terminalIdentityOriginal&&n.source==n.terminalSourceOriginal&&
  n.handle==n.terminalHandleOriginal&&n.kind==n.terminalKindOriginal&&
  n.buffer==n.terminalBufferOriginal&&n.sourceDomain==n.terminalAccountingOriginal&&
  !n.ended&&!n.closeAttempted
}
func primitiveBytes(kind,operation,outcome string,identity,related,source [32]byte,tick uint64,data []byte,mode *string)([]byte,error){
 if identity==([32]byte{})||related==([32]byte{})||source==([32]byte{})||tick==0{return nil,ErrInvalid}
 row:=nativePrimitive{Kind:kind,Platform:nativePlatform,Operation:operation,Outcome:outcome,ObjectIdentityDigest:digestString(identity),RelatedIdentityDigest:digestString(related),SourceImageSHA256:digestString(source),MonotonicTick:generationString(tick),Mode:mode}
 switch kind{
 case "read","write","eof","discovery","ledger":if data==nil&&outcome=="success"{return nil,ErrInvalid};if data!=nil{count:=uint64(len(data));digest:=sha256.Sum256(data);row.ByteCount=&count;row.BytesSHA256=stringRef(digestString(digest))};if mode!=nil{return nil,ErrInvalid};if kind=="eof"&&(len(data)!=0||outcome!="success"){return nil,ErrInvalid}
 case "birth","identity","flush","exit","close":if data!=nil{return nil,ErrInvalid};if mode!=nil&&(kind!="identity"||(*mode!="0644"&&*mode!="0755")||(operation!="linux.fstat"&&operation!="win32.security_info")){return nil,ErrInvalid}
 default:return nil,ErrInvalid
 }
 if !validNativeOperation(kind,operation){return nil,ErrInvalid};if outcome!="success"&&outcome!="failure"&&outcome!="unresolved"{return nil,ErrInvalid}
 raw,e:=json.Marshal(row);if e!=nil||len(raw)>2048{return nil,ErrInvalid};return append(raw,'\n'),nil
}
func validNativeOperation(kind,operation string)bool{
 if !(len(operation)>7&&operation[:7]=="ledger."||nativePlatform=="linux"&&len(operation)>6&&operation[:6]=="linux."||nativePlatform=="win32"&&len(operation)>6&&operation[:6]=="win32."){return false}
 switch kind{
 case "birth":return operation=="linux.execveat"||operation=="linux.pidfd_open"||operation=="win32.create_process"||operation=="win32.process_birth"
 case "identity":return operation=="linux.so_peercred"||operation=="linux.fstat"||operation=="linux.rename_noreplace"||operation=="win32.token_user"||operation=="win32.pipe_peer"||operation=="win32.file_identity"||operation=="win32.security_info"||operation=="win32.rename_noreplace"
 case "read":return operation=="linux.openat_read"||operation=="linux.read"||operation=="win32.nt_open_read"||operation=="win32.read"
 case "eof":return operation=="linux.read"||operation=="win32.read"
 case "write":return operation=="linux.write"||operation=="win32.write"
 case "flush":return operation=="linux.fsync"||operation=="win32.flush"
 case "discovery":return operation=="linux.getdents"||operation=="win32.directory_query"
 case "exit":return operation=="linux.waitid"||operation=="win32.process_wait"||operation=="linux.pidfd_poll"
 case "close":return operation=="linux.close"||operation=="win32.close"
 case "ledger":return operation=="ledger.generation_write"||operation=="ledger.generation_readback"||operation=="ledger.selector_commit"||operation=="ledger.selector_readback"
 };return false
}
func(n *nativeObject)recheckIdentity(identity,source [32]byte)error{
 if n==nil||identity!=n.identity||source!=n.source||source==([32]byte{}){return ErrInvalid};n.mu.Lock();defer n.mu.Unlock()
 if n.terminalBusy||n.terminalUnresolved{return ErrPhase}
 if n.ended{if n.closeDigest==([32]byte{})||len(n.closeRaw)==0||sha256.Sum256(n.closeRaw)!=n.closeDigest{return ErrClosure};return nil}
 if n.closeAttempted{return ErrClosure}
 if n.process!=nil{return n.process.recheckOriginal()};if n.buffer!=nil{return n.buffer.recheckOriginal()}
 actual,e:=nativeHandleIdentity(n.handle,n.kind);if e!=nil||actual!=n.identity{return ErrInvalid};return nil
}
func(n *nativeObject)recheckSnapshot(row resourceRow)error{
 if n==nil||row.identity!=n.identity||row.kind!=n.kind{return ErrInvalid}
 if e:=n.recheckIdentity(n.identity,n.source);e!=nil{return e};n.mu.Lock();defer n.mu.Unlock()
 if n.terminalBusy||n.terminalUnresolved{return ErrPhase}
 if row.state==1{if !n.ended||row.charged!=0||row.observation!=n.closeDigest{return ErrClosure};if n.buffer!=nil&&n.buffer.chargedBytes()!=0{return ErrClosure};return nil}
 if n.ended{if n.buffer!=nil&&row.state==2&&row.charged==n.buffer.chargedBytes()&&row.observation==n.closeDigest{return nil};return ErrInvalid};if row.observation!=n.latestDigest{return ErrInvalid}
 if n.buffer!=nil{if row.charged!=n.buffer.chargedBytes(){return ErrInvalid}}else if row.charged!=0&&n.sourceDomain==nil{return ErrInvalid};return nil
}
func(n *nativeObject)observeTerminal(deadline *nativeDeadline)(nativeTerminal,error){
 var out nativeTerminal;if n==nil{return out,ErrInvalid};n.mu.Lock()
 if n.terminalBusy||n.terminalUnresolved{n.mu.Unlock();return out,ErrPhase}
 if n.ended{defer n.mu.Unlock();out.digest=n.closeDigest;if len(n.closeRaw)==0||n.closeDigest==([32]byte{})||sha256.Sum256(n.closeRaw)!=n.closeDigest{return out,ErrClosure};if n.buffer!=nil&&n.buffer.chargedBytes()!=0{return out,ErrClosure};out.closed=true;out.naturalZero=n.terminalNaturalZero;return out,nil}
 if n.process!=nil{
  if deadline==nil||n.closeAttempted||n.identity==([32]byte{})||n.source==([32]byte{}){n.mu.Unlock();return out,ErrUnavailable}
  if n.terminalAttempted{
   defer n.mu.Unlock()
   if !n.terminalOriginalUnchangedLocked(deadline)||
      !n.terminalResultReturned||n.terminalResultError!=nil{return out,ErrPhase}
   out.naturalZero=n.terminalResult.naturalZero;out.digest=n.terminalResult.digest;return out,nil
  }
  n.terminalAttempted=true;n.terminalBusy=true;n.terminalProcessOriginal=n.process;n.terminalDeadlineOriginal=deadline
  n.terminalIdentityOriginal=n.identity;n.terminalSourceOriginal=n.source;n.terminalHandleOriginal=n.handle
  n.terminalKindOriginal=n.kind;n.terminalBufferOriginal=n.buffer;n.terminalAccountingOriginal=n.sourceDomain
  process:=n.process;n.mu.Unlock()
  wait,e:=process.observeOriginalExit(deadline)
  // Retain the actual output and error in this exclusively occupied original
  // cell before reacquiring metadata or considering publication. Late conflict
  // leaves both this cell and the process owner's native outputs intact.
  n.terminalResult=wait;n.terminalResultError=e;n.terminalResultReturned=true
  n.mu.Lock();defer n.mu.Unlock()
  if !n.terminalBusy||!n.terminalOriginalUnchangedLocked(deadline){
   n.terminalUnresolved=true;n.terminalPublicationError=ErrClosure;if e!=nil{return out,e};return out,ErrClosure
  }
  n.terminalBusy=false
  if e!=nil{n.terminalUnresolved=true;out.digest=n.latestDigest;return out,e};out.naturalZero=wait.naturalZero;out.digest=wait.digest
  // Native wait retains pidfd/process/token/image ownership (and WNOWAIT on
  // Linux). Exit is a real fact, but it is not disposal of those originals.
  // Their genuine pending disposition/capture producer must supply closure.
  if wait.ended{n.terminalNaturalZero=wait.naturalZero;n.latestRaw=wait.raw;n.latestDigest=wait.digest};return out,nil
 }
 defer n.mu.Unlock()
 if n.buffer!=nil{ended,e:=n.buffer.observeEndedOwnership();if e!=nil{out.digest=n.latestDigest;return out,e};if !ended{out.digest=n.latestDigest;return out,ErrClosure}}
 // A peer EOF is not a local close observation. Original close must have run
 // through closeOriginal, retaining its complete native receipt.
 out.digest=n.latestDigest;return out,ErrClosure
}
func(n *nativeObject)closeOriginal()error{
 if n==nil{return ErrInvalid};n.mu.Lock();defer n.mu.Unlock();if n.ended{return ErrPhase}
 if n.terminalBusy||n.terminalUnresolved{return ErrPhase}
 if n.process!=nil||n.closeAttempted{return ErrPhase};tick,e:=nativeTick();if e!=nil{return e};n.closeAttempted=true
 // Reserve the once-only attempt before invoking the kernel. An uncertain
 // close (including a successful close followed by evidence failure) cannot
 // be repeated against a descriptor/handle number that may have been reused.
 if n.buffer!=nil{e=n.buffer.closeOwned()}else{e=nativeClose(n.handle)}
 if e!=nil{if n.sourceDomain!=nil{n.sourceDomain.holdFailure()};raw,receiptErr:=primitiveBytes("close",nativeCloseOperation,"failure",n.identity,n.identity,n.source,tick,nil,nil);if receiptErr==nil{n.latestRaw=raw;n.latestDigest=sha256.Sum256(raw)};return e}
 raw,e:=primitiveBytes("close",nativeCloseOperation,"success",n.identity,n.identity,n.source,tick,nil,nil);if e!=nil{if n.sourceDomain!=nil{n.sourceDomain.holdFailure()};return e}
 n.ended=true;n.closeRaw=raw;n.closeDigest=sha256.Sum256(raw);return nil
}
