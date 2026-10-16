//go:build windows

package hostadmission

import (
 "encoding/binary"
 "sync"
 "unsafe"
 "golang.org/x/sys/windows"
)

var virtualAllocate=objectKernel.NewProc("VirtualAlloc")
var virtualRelease=objectKernel.NewProc("VirtualFree")
var virtualQuery=objectKernel.NewProc("VirtualQuery")
var systemInfoCall=objectKernel.NewProc("GetSystemInfo")
type nativeMemory struct { mu sync.Mutex; address uintptr; bytes []byte; identity [32]byte; accounting *nativeAccounting; index uint32; borrowers uint32; sourceBorrow,sourceBorrowFailed bool; frameOwner *originalFrameLease; charge uint64; ended,closeAttempted,mappingChargeReleased bool; originalOwner *nativeProcess; originalProcess windows.Handle; section,nodeSection windows.Handle }
type memoryFrameOriginal struct { address uintptr; originalProcess,section,nodeSection windows.Handle }
func(m *nativeMemory)frameNativeOriginal()memoryFrameOriginal{return memoryFrameOriginal{address:m.address,originalProcess:m.originalProcess,section:m.section,nodeSection:m.nodeSection}}
type memoryBasic struct { base,allocationBase uintptr; allocationProtect uint32; partition uint16; padding uint16; regionSize uintptr; state,protect,kind uint32 }
func nativeAllocationGranularity()(uint64,error){type systemInfo struct{architecture,reserved uint16;page uint32;minimum,maximum,mask uintptr;processors,processorType,granularity uint32;level,revision uint16};var info systemInfo;systemInfoCall.Call(uintptr(unsafe.Pointer(&info)));if info.page==0||info.granularity==0||info.granularity<info.page{return 0,ErrUnavailable};return uint64(info.granularity),nil}
func allocateNativeMemory(accounting *nativeAccounting,requested uint64,owner *nativeProcess)(*nativeMemory,error){
 if accounting==nil||accounting.domain==2{return nil,ErrUnavailable}
 index,mapped,e:=accounting.reserveMapping(requested);if e!=nil{return nil,e};if mapped>uint64(^uint(0)>>1){accounting.holdFailure();return nil,ErrCapacity}
 memory:=&nativeMemory{accounting:accounting,index:index,charge:mapped+accounting.contract.nativeObjectBytes,originalOwner:owner,originalProcess:windows.CurrentProcess()};if e=reserveNativeMemoryCustody(memory);e!=nil{accounting.holdFailure();return nil,e}
 address,_,_:=virtualAllocate.Call(0,uintptr(mapped),0x1000|0x2000,0x04);if address==0{accounting.holdFailure();return nil,ErrUnavailable};memory.address=address;memory.bytes=unsafe.Slice((*byte)(unsafe.Pointer(address)),int(mapped))
 tick,e:=nativeTick();if e!=nil{accounting.holdFailure();retainNativeMemory(memory);return nil,e};var record [24]byte;binary.BigEndian.PutUint64(record[:8],uint64(address));binary.BigEndian.PutUint64(record[8:16],mapped);binary.BigEndian.PutUint64(record[16:24],tick);memory.identity=DomainHash("SLCLI-WINDOWS-ALLOCATION-1\x00",accounting.contract.source[:],record[:])
 if e=memory.recheckOriginal();e!=nil||accounting.bindMapping(index,memory.identity)!=nil{accounting.holdFailure();retainNativeMemory(memory);return nil,ErrUnavailable};return memory,nil
}
func(m *nativeMemory)recheckOriginal()error{if m==nil{return ErrInvalid};m.mu.Lock();defer m.mu.Unlock();return m.recheckLocked()}
func(m *nativeMemory)recheckLocked()error{
 if m.ended||m.closeAttempted||m.address==0||len(m.bytes)==0||m.identity==([32]byte{}){return ErrInvalid};var info memoryBasic;size,_,_:=virtualQuery.Call(m.address,uintptr(unsafe.Pointer(&info)),unsafe.Sizeof(info));kind:=uint32(0x20000);if m.section!=0{kind=0x40000};if size!=unsafe.Sizeof(info)||info.base!=m.address||info.allocationBase!=m.address||info.regionSize!=uintptr(len(m.bytes))||info.state!=0x1000||info.protect!=0x04||info.kind!=kind{return ErrInvalid};return nil
}
func(m *nativeMemory)chargedBytes()uint64{if m==nil{return 0};m.mu.Lock();defer m.mu.Unlock();return m.charge}
func(m *nativeMemory)observeEndedOwnership()(bool,error){if m==nil{return false,ErrInvalid};m.mu.Lock();defer m.mu.Unlock();if m.ended{return true,nil};if m.originalOwner!=nil{ended,e:=m.originalOwner.ownershipEnded();if e!=nil||!ended{return false,ErrClosure}};return false,nil}
func(m *nativeMemory)closeOwned()error{
 if m==nil{return ErrPhase};m.mu.Lock();defer m.mu.Unlock();if m.ended||m.closeAttempted{return ErrPhase};if m.borrowers!=0{return ErrClosure};if m.accounting.domain==2{if m.originalOwner==nil{return ErrClosure};ended,e:=m.originalOwner.ownershipEnded();if e!=nil||!ended{return ErrClosure}}
 if e:=m.recheckLocked();e!=nil{return e};m.closeAttempted=true;if m.section!=0{if windows.UnmapViewOfFile(m.address)!=nil||windows.CloseHandle(m.section)!=nil{m.accounting.holdFailure();return ErrClosure}}else{ok,_,_:=virtualRelease.Call(m.address,0,0x8000);if ok==0{m.accounting.holdFailure();return ErrClosure}};m.ended=true;m.bytes=nil;if e:=m.accounting.endMapping(m.index,m.identity);e!=nil{m.accounting.holdFailure();return e};m.charge=m.accounting.contract.nativeObjectBytes;m.mappingChargeReleased=true;return nil
}

// Client memory is one original service-created section, duplicated into the
// authenticated held Node process. VirtualAlloc in the service cannot serve as
// an external Node Buffer allocation or cross-process custody substitute.
func allocateClientSection(accounting *nativeAccounting,requested uint64,peer *peerCustody)(*nativeMemory,error){
 if accounting==nil||accounting.domain!=2||peer==nil||peer.role!=originalNodePeer||peer.packageSource==([32]byte{})||peer.process==nil||accounting.originalParent!=peer.process{return nil,ErrUnavailable};if e:=peer.process.recheckOriginal();e!=nil{return nil,e}
 index,mapped,e:=accounting.reserveMapping(requested);if e!=nil{return nil,e};if mapped>uint64(^uint(0)>>1){accounting.holdFailure();return nil,ErrCapacity};memory:=&nativeMemory{accounting:accounting,index:index,charge:mapped+accounting.contract.nativeObjectBytes,originalOwner:peer.process,originalProcess:windows.CurrentProcess()};if e=reserveNativeMemoryCustody(memory);e!=nil{accounting.holdFailure();return nil,e}
 section,e:=windows.CreateFileMapping(windows.InvalidHandle,nil,windows.PAGE_READWRITE,uint32(mapped>>32),uint32(mapped),nil);if e!=nil{accounting.holdFailure();return nil,ErrUnavailable};memory.section=section
 address,e:=windows.MapViewOfFile(section,windows.FILE_MAP_READ|windows.FILE_MAP_WRITE,0,0,uintptr(mapped));if e!=nil{accounting.holdFailure();return nil,ErrUnavailable};memory.address=address;memory.bytes=unsafe.Slice((*byte)(unsafe.Pointer(address)),int(mapped))
 tick,e:=nativeTick();if e!=nil{accounting.holdFailure();return nil,e};var record [16]byte;binary.BigEndian.PutUint64(record[:8],mapped);binary.BigEndian.PutUint64(record[8:],tick);memory.identity=DomainHash("SLCLI-WINDOWS-CLIENT-SECTION-1\x00",peer.process.identity[:],accounting.contract.source[:],record[:])
 if e=memory.recheckOriginal();e!=nil||accounting.bindMapping(index,memory.identity)!=nil{accounting.holdFailure();return nil,ErrUnavailable}
 if windows.DuplicateHandle(windows.CurrentProcess(),section,peer.process.handle,&memory.nodeSection,0,false,windows.DUPLICATE_SAME_ACCESS)!=nil{accounting.holdFailure();return nil,ErrUnavailable};if e=peer.process.recheckOriginal();e!=nil{accounting.holdFailure();return nil,e};return memory,nil
}
