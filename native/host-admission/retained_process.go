//go:build linux || windows

package hostadmission

import "sync"

// An occupied cell remains custody even when a constructor fails. Reserve it
// before native process handles are created; saturation denies admission.
var originalProcesses struct { sync.Mutex; count uint32; rows [(2*MaximumBindings+2)*MaximumReferences]*nativeProcess }
// Reserve before even constructing the retained process body. Empty reserved
// cells survive a failed allocation/constructor and are never reused. The
// global occurrence capacity is not a proof of a binding-wide runtime bound.
func reserveProcessOriginal()(uint32,error){originalProcesses.Lock();defer originalProcesses.Unlock();if originalProcesses.count>=uint32(len(originalProcesses.rows)){return 0,ErrCapacity};slot:=originalProcesses.count;originalProcesses.count++;return slot,nil}
func retainProcessOriginal(slot uint32,process *nativeProcess)error{originalProcesses.Lock();defer originalProcesses.Unlock();if process==nil||slot>=originalProcesses.count||slot>=uint32(len(originalProcesses.rows))||originalProcesses.rows[slot]!=nil{return ErrInvalid};originalProcesses.rows[slot]=process;return nil}
