//go:build linux

package hostadmission

import (
 "sync"
 "golang.org/x/sys/unix"
)

type nativeMemory struct { mu sync.Mutex; fd int; bytes []byte; identity [32]byte; accounting *nativeAccounting; index uint32; borrowers uint32; sourceBorrow,sourceBorrowFailed bool; frameOwner *originalFrameLease; charge uint64; ended,closeAttempted,mappingChargeReleased bool; originalOwner *nativeProcess }
type memoryFrameOriginal struct { fd int }
func(m *nativeMemory)frameNativeOriginal()memoryFrameOriginal{return memoryFrameOriginal{fd:m.fd}}
func nativeAllocationGranularity()(uint64,error){size:=unix.Getpagesize();if size<=0{return 0,ErrUnavailable};return uint64(size),nil}
func allocateNativeMemory(accounting *nativeAccounting,requested uint64,owner *nativeProcess)(*nativeMemory,error){
 if accounting==nil||accounting.domain==2&&(owner==nil||owner!=accounting.originalParent){return nil,ErrUnavailable}
 index,mapped,e:=accounting.reserveMapping(requested);if e!=nil{return nil,e};if mapped>uint64(^uint(0)>>1){accounting.holdFailure();return nil,ErrCapacity}
 memory:=&nativeMemory{fd:-1,accounting:accounting,index:index,charge:mapped+accounting.contract.nativeObjectBytes,originalOwner:owner};if e=reserveNativeMemoryCustody(memory);e!=nil{accounting.holdFailure();return nil,e}
 fd,e:=unix.MemfdCreate("service-lasso-owned-copy",unix.MFD_CLOEXEC|unix.MFD_ALLOW_SEALING);if e!=nil{accounting.holdFailure();return nil,ErrUnavailable};memory.fd=fd
 if unix.Ftruncate(fd,int64(mapped))!=nil{accounting.holdFailure();retainNativeMemory(memory);return nil,ErrUnavailable}
 memory.bytes,e=unix.Mmap(fd,0,int(mapped),unix.PROT_READ|unix.PROT_WRITE,unix.MAP_SHARED);if e!=nil{accounting.holdFailure();retainNativeMemory(memory);return nil,ErrUnavailable}
 if _,e=unix.FcntlInt(uintptr(fd),unix.F_ADD_SEALS,unix.F_SEAL_GROW|unix.F_SEAL_SHRINK);e!=nil{accounting.holdFailure();retainNativeMemory(memory);return nil,ErrUnavailable}
 memory.identity,e=nativeHandleIdentity(fd,7);if e!=nil||accounting.bindMapping(index,memory.identity)!=nil{accounting.holdFailure();retainNativeMemory(memory);return nil,ErrUnavailable};return memory,nil
}
func(m *nativeMemory)recheckOriginal()error{if m==nil{return ErrInvalid};m.mu.Lock();defer m.mu.Unlock();return m.recheckLocked()}
func(m *nativeMemory)recheckLocked()error{if m.ended||m.closeAttempted||m.fd<0||len(m.bytes)==0{return ErrInvalid};actual,e:=nativeHandleIdentity(m.fd,7);if e!=nil||actual!=m.identity{return ErrInvalid};var stat unix.Stat_t;if unix.Fstat(m.fd,&stat)!=nil||stat.Size!=int64(len(m.bytes)){return ErrInvalid};seals,e:=unix.FcntlInt(uintptr(m.fd),unix.F_GET_SEALS,0);if e!=nil||seals&(unix.F_SEAL_GROW|unix.F_SEAL_SHRINK)!=(unix.F_SEAL_GROW|unix.F_SEAL_SHRINK){return ErrInvalid};return nil}
func(m *nativeMemory)chargedBytes()uint64{if m==nil{return 0};m.mu.Lock();defer m.mu.Unlock();return m.charge}
func(m *nativeMemory)observeEndedOwnership()(bool,error){if m==nil{return false,ErrInvalid};m.mu.Lock();defer m.mu.Unlock();if m.ended{return true,nil};if m.originalOwner!=nil{ended,e:=m.originalOwner.ownershipEnded();if e!=nil||!ended{return false,ErrClosure}};return false,nil}
func(m *nativeMemory)closeOwned()error{if m==nil{return ErrPhase};m.mu.Lock();defer m.mu.Unlock();if m.ended||m.closeAttempted{return ErrPhase};if m.borrowers!=0{return ErrClosure};if m.accounting.domain==2{if m.originalOwner==nil{return ErrClosure};ended,e:=m.originalOwner.ownershipEnded();if e!=nil||!ended{return ErrClosure}};if e:=m.recheckLocked();e!=nil{return e};m.closeAttempted=true;if unix.Munmap(m.bytes)!=nil{m.accounting.holdFailure();return ErrClosure};if unix.Close(m.fd)!=nil{m.accounting.holdFailure();return ErrClosure};m.ended=true;m.bytes=nil;if e:=m.accounting.endMapping(m.index,m.identity);e!=nil{m.accounting.holdFailure();return e};m.charge=m.accounting.contract.nativeObjectBytes;m.mappingChargeReleased=true;return nil}
