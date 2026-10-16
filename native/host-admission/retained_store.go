//go:build linux || windows

package hostadmission

import "sync"

// A failed original storage operation is not a pathname cleanup candidate.
// Preserve its handle table and key/root custody until the actual service
// process exits or an explicitly authorized original-object recovery owns it.
// There is exactly one store open attempt per service lifetime.
var storeAttemptMu sync.Mutex
var storeAttempted bool
var failedOriginalStore *nativeStore
func beginStoreAttempt()bool{storeAttemptMu.Lock();defer storeAttemptMu.Unlock();if storeAttempted{return false};storeAttempted=true;return true}
func retainStore(store *nativeStore){storeAttemptMu.Lock();defer storeAttemptMu.Unlock();if failedOriginalStore==nil{failedOriginalStore=store}}
