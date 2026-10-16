//go:build windows

package hostadmission

import "golang.org/x/sys/windows"

const nativeWriteOperation="win32.write"

func writeOwnedNativeOnce(handle windows.Handle,memory *nativeMemory,offset,length uint32,deadline *nativeDeadline,owner *originalFrameLease)(count int,err error){
 if memory==nil||deadline==nil||length==0||length>FrameBytes{return 0,ErrInvalid}
 ordinary,e:=borrowFrameOrOrdinary(memory,owner);if e!=nil{return 0,e};slot,e:=originalWindowsIO(handle);if e!=nil{if ordinary{e=qualifyNativeFailure(e,memory.returnOriginal())};return 0,e}
 slot.mu.Lock();defer slot.mu.Unlock()
 if slot.active||uint64(offset)+uint64(length)>uint64(len(memory.bytes)){failure:=error(ErrInvalid);if ordinary{failure=qualifyNativeFailure(failure,memory.returnOriginal())};return 0,failure}
 slot.memory=memory;slot.view=memory.bytes[offset:offset+length]
 defer func(){if slot.active{memory.accounting.holdFailure();return};if ordinary{if e:=memory.returnOriginal();e!=nil{err=qualifyNativeFailure(err,e)}};slot.memory=nil;slot.view=nil}()
 if _,e=deadline.remaining();e!=nil{slot.failed=true;return 0,e}
 if windows.ResetEvent(slot.event)!=nil{slot.failed=true;return 0,ErrUnavailable}
 slot.overlap=windows.Overlapped{HEvent:slot.event};slot.active=true
 e=windows.WriteFile(handle,slot.view,nil,&slot.overlap);n,e:=completeOwnedIO(slot,e,deadline,false)
 if uint64(n)>uint64(length){slot.failed=true;return 0,ErrInvalid};return int(n),e
}
