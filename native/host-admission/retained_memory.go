//go:build linux || windows

package hostadmission

import "sync"

var retainedMemoryMu sync.Mutex
var retainedMemories [(2*MaximumBindings+2)*256]*nativeMemory
func reserveNativeMemoryCustody(memory *nativeMemory)error{retainedMemoryMu.Lock();defer retainedMemoryMu.Unlock();if memory==nil{return ErrInvalid};for i:=range retainedMemories{if retainedMemories[i]==nil{retainedMemories[i]=memory;return nil}};return ErrCapacity}
func retainNativeMemory(memory *nativeMemory){if memory!=nil&&memory.accounting!=nil{memory.accounting.holdFailure()}}
