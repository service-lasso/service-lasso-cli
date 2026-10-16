//go:build linux || windows

package hostadmission

import "crypto/sha256"

// The independent native fixture establishes a genuine failure after the
// response's actual writes (capture flush/readback or later validation). This
// invokes the ordinary source method, then tests the original alias exclusion;
// it does not manufacture the native failure or a source admission.
func originalFrameLateFailureRetentionVector(output *primaryFrameOutput,frame *Frame)error{
 if output==nil||output.input==nil||output.input.memory==nil||frame==nil||
    output.failed.Load()||output.frameLease.borrowed{return ErrInvalid}
 e:=output.write(frame)
 lease:=&output.frameLease;memory:=output.input.memory;result:=&output.writeResult
 if e==nil||!output.failed.Load()||!output.input.failed.Load()||!result.started||!result.returned||
    !result.complete||!result.retainedBorrow||!lease.borrowed||!lease.retained||
    lease.complete||lease.returnAttempted||lease.returned||lease.primaryError==nil||
    !lease.rawFlushReturned||(!lease.nativeFlushReturned&&!lease.nativeFlushSkipped){return ErrInvalid}
 memory.mu.Lock();exclusive:=memory.frameOwner==lease&&memory.borrowers==1&&!memory.sourceBorrow;memory.mu.Unlock()
 if !exclusive||result.memory!=memory||len(result.observed)==0{return ErrInvalid}
 bytes:=result.observed;digest:=sha256.Sum256(bytes)
 // A failed exclusive frame cannot be overwritten by either ordinary IO or
 // a second source-body borrower. These deny before another native effect.
 if memory.borrowOriginal()==nil||memory.borrowSourceOriginal()==nil{return ErrInvalid}
 if output.write(frame)==nil||!sameOriginalSlice(bytes,result.observed)||
    sha256.Sum256(bytes)!=digest||lease.returnAttempted||!lease.retained{return ErrInvalid}
 memory.mu.Lock();exclusive=memory.frameOwner==lease&&memory.borrowers==1;memory.mu.Unlock()
 if !exclusive{return ErrInvalid};return nil
}
