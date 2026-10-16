//go:build linux || windows

package hostadmission

import (
 "crypto/sha256"
 "encoding/binary"
 "hash"
 "strings"
 "sync"
 "sync/atomic"
 "unsafe"
)

type clientExpectedFile struct { path string; mode uint32; bytes []byte; digest [32]byte }
type clientCopyLayout struct { publication,chargeStart,eventStart,eventBytes,recordStart,recordBytes,dataStart,dataBytes uint64 }
type clientCopyLedger struct { mu sync.Mutex; memory,observerSnapshot *nativeMemory; parent *nativeProcess; run,packageSource [32]byte; layout clientCopyLayout; files []clientExpectedFile; sets uint32; prospective uint64; cumulative,recordOffset,dataOffset,evidenceBytes uint64; sink *nativeEvidenceFile; recordHash hash.Hash; failed bool }

// Construction belongs exclusively to the authenticated original native
// inspection receiver. Files are the held complete source-SEA record, not an
// endpoint parameter or caller-supplied snapshot.
func newClientCopyLedger(memory,observerSnapshot *nativeMemory,peer *peerCustody,run [32]byte,layout clientCopyLayout,files []clientExpectedFile,sink *nativeEvidenceFile,base uint64)(result *clientCopyLedger,err error){
 if memory==nil||observerSnapshot==nil||observerSnapshot.accounting==nil||observerSnapshot.accounting.domain!=4||peer==nil||peer.role!=originalNodePeer||peer.packageSource==([32]byte{})||peer.process==nil||memory.originalOwner!=peer.process||memory.accounting==nil||memory.accounting.domain!=2||run==([32]byte{})||sink==nil||len(files)<1||len(files)>MaximumAuthoredFiles||base==0||base>SessionBytes{return nil,ErrInvalid};if e:=memory.recheckOriginal();e!=nil{return nil,e};if e:=observerSnapshot.recheckOriginal();e!=nil{return nil,e};if e:=peer.process.recheckOriginal();e!=nil{return nil,e}
 if e:=memory.borrowOriginal();e!=nil{return nil,e};defer func(){if e:=memory.returnOriginal();e!=nil{if err==nil{err=e;result=nil}}}();if e:=observerSnapshot.borrowOriginal();e!=nil{return nil,e};defer func(){if e:=observerSnapshot.returnOriginal();e!=nil{if err==nil{err=e;result=nil}}}()
 size:=uint64(len(memory.bytes));regions:=[]struct{start,length uint64}{{layout.publication,4},{layout.chargeStart,16},{layout.eventStart,MaximumCopySets*56},{layout.recordStart,layout.recordBytes},{layout.dataStart,layout.dataBytes}}
 if layout.eventBytes!=MaximumCopySets*56||layout.publication%4!=0||layout.chargeStart%8!=0||layout.eventStart%8!=0||layout.recordBytes==0||layout.dataBytes==0{return nil,ErrInvalid}
 for i,region:=range regions{if region.start>size||region.length>size-region.start{return nil,ErrInvalid};for j:=0;j<i;j++{other:=regions[j];if region.start<other.start+other.length&&other.start<region.start+region.length{return nil,ErrInvalid}}}
 snapshotStart:=uintptr(unsafe.Pointer(&observerSnapshot.bytes[0]));snapshotEnd:=snapshotStart+uintptr(len(observerSnapshot.bytes));for _,file:=range files{if !evidencePath(file.path)||file.mode!=0644&&file.mode!=0755||sha256.Sum256(file.bytes)!=file.digest{return nil,ErrInvalid};if len(file.bytes)>0{start:=uintptr(unsafe.Pointer(&file.bytes[0]));if start<snapshotStart||start>snapshotEnd||uintptr(len(file.bytes))>snapshotEnd-start{return nil,ErrInvalid}}}
 publication:=(*uint32)(unsafe.Pointer(&memory.bytes[layout.publication]));if atomic.LoadUint32(publication)!=0{return nil,ErrInvalid}
 high:=atomic.LoadUint64((*uint64)(unsafe.Pointer(&memory.bytes[layout.chargeStart])));failed:=atomic.LoadUint32((*uint32)(unsafe.Pointer(&memory.bytes[layout.chargeStart+8])));if high<base||high>SessionBytes||failed!=0{return nil,ErrInvalid}
 return &clientCopyLedger{memory:memory,observerSnapshot:observerSnapshot,parent:peer.process,run:run,packageSource:peer.packageSource,layout:layout,files:files,sink:sink,prospective:high,cumulative:base,recordHash:sha256.New()},nil
}
func evidencePath(path string)bool{
 if len(path)==0||len(path)>240||path[0]=='/'||path[len(path)-1]=='/'{return false};start,depth:=0,0
 for i:=0;i<=len(path);i++{
  if i<len(path)&&path[i]!='/'{b:=path[i];if !(b>='a'&&b<='z'||b>='A'&&b<='Z'||b>='0'&&b<='9'||b=='_'||b=='.'||b=='@'||b=='-'){return false};continue}
  part:=path[start:i];depth++;if part==""||part=="."||part==".."||strings.HasSuffix(part,".")||depth>12{return false}
  base:=part;if dot:=strings.IndexByte(base,'.');dot>=0{base=base[:dot]};if strings.EqualFold(base,"con")||strings.EqualFold(base,"prn")||strings.EqualFold(base,"aux")||strings.EqualFold(base,"nul"){return false};if len(base)==4&&(strings.EqualFold(base[:3],"com")||strings.EqualFold(base[:3],"lpt"))&&base[3]>='1'&&base[3]<='9'{return false};start=i+1
 }
 return true
}

// The local allocator publishes immutable private records before returning
// mutable public Buffers. At the final action the original service/observer
// captures those records through its retained mapping. Copying itself introduces
// no request or IPC into acceptedTemplateFiles. A failed observation retains
// every mapping, event, prospective charge and original parent binding.
func(l *clientCopyLedger)observePublished(sequence uint32)(result error){
 if l==nil{return ErrInvalid};l.mu.Lock();defer l.mu.Unlock();fail:=func(e error)error{l.failed=true;l.memory.accounting.holdFailure();return e}
 if l.failed||sequence!=l.sets+1||sequence>MaximumCopySets{return fail(ErrPhase)};if e:=l.parent.recheckOriginal();e!=nil{return fail(e)}
 // Native leases prevent local unmapping throughout the read/capture, without
 // blocking IO under a host admission/ledger lock. Node's actual publication
 // supplies the cross-process barrier for the immutable private records.
 if e:=l.memory.borrowOriginal();e!=nil{return fail(e)};defer func(){if e:=l.memory.returnOriginal();e!=nil{fail(e);if result==nil{result=e}}}()
 if e:=l.observerSnapshot.borrowOriginal();e!=nil{return fail(e)};defer func(){if e:=l.observerSnapshot.returnOriginal();e!=nil{fail(e);if result==nil{result=e}}}()
 publication:=(*uint32)(unsafe.Pointer(&l.memory.bytes[l.layout.publication]));published:=atomic.LoadUint32(publication);if published<sequence||published>MaximumCopySets{return fail(ErrInvalid)}
 high:=atomic.LoadUint64((*uint64)(unsafe.Pointer(&l.memory.bytes[l.layout.chargeStart])));failed:=atomic.LoadUint32((*uint32)(unsafe.Pointer(&l.memory.bytes[l.layout.chargeStart+8])));if high<l.prospective||high>SessionBytes{return fail(ErrCapacity)};l.prospective=high;if failed!=0{return fail(ErrClosure)}
 start:=l.layout.eventStart+uint64(sequence-1)*56;event:=l.memory.bytes[start:start+56];if binary.LittleEndian.Uint32(event[:4])!=sequence||binary.LittleEndian.Uint32(event[4:8])!=uint32(len(l.files)){return fail(ErrInvalid)}
 reserved:=binary.LittleEndian.Uint64(event[8:16]);cumulative:=binary.LittleEndian.Uint64(event[16:24]);firstData:=binary.LittleEndian.Uint64(event[24:32]);lastData:=binary.LittleEndian.Uint64(event[32:40]);firstRecord:=binary.LittleEndian.Uint64(event[40:48]);lastRecord:=binary.LittleEndian.Uint64(event[48:56])
 if reserved==0||reserved>SessionBytes-l.cumulative||cumulative!=l.cumulative+reserved||cumulative>l.prospective||firstData!=l.dataOffset||firstRecord!=l.recordOffset||lastData<firstData||lastData>l.layout.dataBytes||lastRecord<firstRecord||lastRecord>l.layout.recordBytes{return fail(ErrCapacity)}
 raw:=l.memory.bytes[l.layout.recordStart+firstRecord:l.layout.recordStart+lastRecord];cursor:=Cursor{Raw:raw};offset:=firstData
 for _,file:=range l.files{path,e:=cursor.S(240,false);if e!=nil||path!=file.path{return fail(ErrInvalid)};mode,e:=cursor.U32();if e!=nil||mode!=file.mode{return fail(ErrInvalid)};count,e:=cursor.U32();if e!=nil||uint64(count)>lastData-offset||int(count)!=len(file.bytes){return fail(ErrInvalid)};digest,e:=cursor.B32();if e!=nil||digest!=file.digest{return fail(ErrInvalid)};stored,e:=cursor.Take(8);if e!=nil||binary.BigEndian.Uint64(stored)!=offset{return fail(ErrInvalid)};offset+=uint64(count)}
 if cursor.End()!=nil||offset!=lastData||atomic.LoadUint32(publication)!=published{return fail(ErrInvalid)}
 // Original allocation-time equality belongs to the admitted instrumented
 // native copier; public mutable data is never reinterpreted as the snapshot.
 // Retain its exact immutable event and private raw record before final action.
 if e:=l.sink.writeOriginal(event);e!=nil{return fail(e)};if e:=l.sink.writeOriginal(raw);e!=nil{return fail(e)}
 l.recordHash.Write(event);l.recordHash.Write(raw);l.evidenceBytes+=uint64(len(event)+len(raw));if e:=l.sink.flushAndReadback(l.evidenceBytes,l.recordHash.Sum(nil));e!=nil{return fail(e)}
 l.sets=sequence;l.cumulative=cumulative;l.recordOffset=lastRecord;l.dataOffset=lastData;return nil
}
