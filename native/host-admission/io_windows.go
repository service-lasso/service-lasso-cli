//go:build windows

package hostadmission

import (
 "sync"
 "golang.org/x/sys/windows"
)

type ownedWindowsIO struct { mu sync.Mutex; handle,event,witness windows.Handle; overlap windows.Overlapped; occupied,failed,active bool; memory *nativeMemory; view []byte; initiatorError,completionError,cancelError,waitError error; actualCount,reportedCount,waitState uint32; completionObserved,completionQueried bool }
var ownedWindowsIOMu sync.Mutex
var ownedWindowsIOs [MaximumBindings*MaximumReferences]ownedWindowsIO
func prepareOwnedIO(handle windows.Handle)error{
 ownedWindowsIOMu.Lock();defer ownedWindowsIOMu.Unlock();kind,e:=windows.GetFileType(handle);if e!=nil||kind!=windows.FILE_TYPE_PIPE{return ErrInvalid};var slot *ownedWindowsIO
 for i:=range ownedWindowsIOs{candidate:=&ownedWindowsIOs[i];if candidate.occupied&&candidate.handle==handle{return ErrInvalid};if !candidate.occupied&&slot==nil{slot=candidate}};if slot==nil{return ErrCapacity};slot.occupied=true;slot.handle=handle
 witness,e:=nativeHandleDuplicate(handle);if e!=nil{slot.failed=true;return e};slot.witness=witness;event,e:=windows.CreateEvent(nil,1,0,nil);if e!=nil{slot.failed=true;return ErrUnavailable};slot.event=event;slot.overlap.HEvent=event;return nil
}
func originalWindowsIO(handle windows.Handle)(*ownedWindowsIO,error){ownedWindowsIOMu.Lock();defer ownedWindowsIOMu.Unlock();for i:=range ownedWindowsIOs{slot:=&ownedWindowsIOs[i];if slot.occupied&&slot.handle==handle{same,_,_:=compareObjectCall.Call(uintptr(handle),uintptr(slot.witness));if slot.failed||same==0{return nil,ErrUnavailable};return slot,nil}};return nil,ErrInvalid}
// Overlapped ReadFile/WriteFile use a null synchronous-count pointer. Actual
// transfer counts come from this same original OVERLAPPED completion, including
// immediate success; a zero synchronous placeholder is never an EOF witness.
// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
// https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-getoverlappedresult
func completeOwnedIO(slot *ownedWindowsIO,pending error,deadline *nativeDeadline,reading bool)(uint32,error){
 slot.initiatorError=pending;slot.completionError=nil;slot.cancelError=nil;slot.waitError=nil;slot.actualCount=0;slot.reportedCount=0;slot.waitState=0;slot.completionObserved=false;slot.completionQueried=false
 if pending==windows.ERROR_BROKEN_PIPE||pending==windows.ERROR_HANDLE_EOF{slot.active=false;if !reading{slot.failed=true;return 0,qualifyNativeFailure(ErrUnavailable,pending)};slot.completionObserved=true;if _,e:=deadline.remaining();e!=nil{slot.failed=true;return 0,e};return 0,nil}
 if pending!=nil&&pending!=windows.ERROR_IO_PENDING&&pending!=windows.ERROR_MORE_DATA{slot.active=false;slot.failed=true;return 0,qualifyNativeFailure(ErrUnavailable,pending)}
 if pending==windows.ERROR_IO_PENDING{
  remaining,e:=deadline.remaining();if e!=nil{slot.cancelError=windows.CancelIoEx(slot.handle,&slot.overlap);slot.failed=true;return 0,e};timeout:=uint32((remaining+999999)/1000000);state,e:=windows.WaitForSingleObject(slot.event,timeout);slot.waitState=state;slot.waitError=e
  if e!=nil||state!=windows.WAIT_OBJECT_0{slot.cancelError=windows.CancelIoEx(slot.handle,&slot.overlap);slot.failed=true;return 0,qualifyNativeFailure(ErrClosure,e)}
 }
 var count uint32;e:=windows.GetOverlappedResult(slot.handle,&slot.overlap,&count,false);slot.completionError=e;slot.completionQueried=true;slot.reportedCount=count
 if e!=nil&&e!=windows.ERROR_BROKEN_PIPE&&e!=windows.ERROR_HANDLE_EOF&&e!=windows.ERROR_OPERATION_ABORTED&&e!=windows.ERROR_MORE_DATA{
  // An unknown/incomplete completion leaves the original IO lease alive. Its
  // output count is not observed data and cannot authorize reading this view.
  slot.failed=true;return 0,qualifyNativeFailure(ErrUnavailable,e)
 }
 slot.active=false;slot.completionObserved=true
 if !reading&&(e==windows.ERROR_BROKEN_PIPE||e==windows.ERROR_HANDLE_EOF){slot.completionObserved=false;slot.failed=true;return 0,qualifyNativeFailure(ErrUnavailable,e)}
 if e==windows.ERROR_OPERATION_ABORTED{slot.failed=true;return 0,qualifyNativeFailure(ErrClosure,e)}
 slot.actualCount=count;if e==windows.ERROR_MORE_DATA||pending==windows.ERROR_MORE_DATA{slot.failed=true;cause:=e;if cause==nil{cause=pending};return count,qualifyNativeFailure(ErrUnavailable,cause)}
 if _,e=deadline.remaining();e!=nil{slot.failed=true;return count,e};return count,nil
}
func readOwnedNativeIO(handle windows.Handle,memory *nativeMemory,offset,length uint32,deadline *nativeDeadline,owner *originalFrameLease)(result int,resultError error){
 if memory==nil||deadline==nil||length==0||length>FrameBytes{return 0,ErrInvalid};ordinary,e:=borrowFrameOrOrdinary(memory,owner);if e!=nil{return 0,e};slot,e:=originalWindowsIO(handle);if e!=nil{if ordinary{e=qualifyNativeFailure(e,memory.returnOriginal())};return 0,e};slot.mu.Lock();defer slot.mu.Unlock()
 if slot.active||uint64(offset)+uint64(length)>uint64(len(memory.bytes)){failure:=error(ErrInvalid);if ordinary{failure=qualifyNativeFailure(failure,memory.returnOriginal())};return 0,failure};slot.memory=memory;slot.view=memory.bytes[offset:offset+length]
 defer func(){if slot.active{memory.accounting.holdFailure();return};if ordinary{if e:=memory.returnOriginal();e!=nil{resultError=qualifyNativeFailure(resultError,e)}};slot.memory=nil;slot.view=nil}()
 if _,e:=deadline.remaining();e!=nil{slot.failed=true;return 0,e}
 if windows.ResetEvent(slot.event)!=nil{slot.failed=true;return 0,ErrUnavailable};slot.overlap=windows.Overlapped{HEvent:slot.event};slot.active=true;e=windows.ReadFile(handle,slot.view,nil,&slot.overlap);n,e:=completeOwnedIO(slot,e,deadline,true);if uint64(n)>uint64(length){slot.failed=true;return 0,ErrInvalid};return int(n),e
}
func writeOwnedNativeIO(handle windows.Handle,memory *nativeMemory,offset,length uint32,deadline *nativeDeadline)(resultError error){
 if memory==nil||deadline==nil||length==0||length>FrameBytes{return ErrInvalid};if e:=memory.borrowOriginal();e!=nil{return e};slot,e:=originalWindowsIO(handle);if e!=nil{return qualifyNativeFailure(e,memory.returnOriginal())};slot.mu.Lock();defer slot.mu.Unlock()
 if slot.active||uint64(offset)+uint64(length)>uint64(len(memory.bytes)){return qualifyNativeFailure(ErrInvalid,memory.returnOriginal())};slot.memory=memory;slot.view=memory.bytes[offset:offset+length]
 defer func(){if slot.active{memory.accounting.holdFailure();return};if e:=memory.returnOriginal();e!=nil{resultError=qualifyNativeFailure(resultError,e)};slot.memory=nil;slot.view=nil}()
 for written:=0;written<len(slot.view);{if _,e:=deadline.remaining();e!=nil{slot.failed=true;return e};if windows.ResetEvent(slot.event)!=nil{slot.failed=true;return ErrUnavailable};slot.overlap=windows.Overlapped{HEvent:slot.event};slot.active=true;e=windows.WriteFile(handle,slot.view[written:],nil,&slot.overlap);n,e:=completeOwnedIO(slot,e,deadline,false);if e!=nil{slot.failed=true;return qualifyNativeFailure(ErrUnavailable,e)};if n==0||uint64(n)>uint64(len(slot.view)-written){slot.failed=true;return ErrUnavailable};written+=int(n)};return nil
}
