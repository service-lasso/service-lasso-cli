//go:build linux || windows

package hostadmission

// One embedded original store-operation owner, shared by the actual nested
// generation/selector readback body. This is lifetime/exclusion metadata, not
// native provenance, durability evidence, a constructor or a renewed deadline.
type originalStoreOperation struct {
 store *nativeStore
 ordinal uint64
 kind byte // 1 read, 2 replace, 3 inventory, 4 close, 5 same durable pair
 active,complete,unresolved bool
 rootOriginal,keyOriginal objectHandle
 rootIdentityOriginal [32]byte
 ownerOriginal storeOwnerIdentity
 resultError,publicationError error
 close originalStoreClose
}
type originalStoreClose struct {
 keyReturned,rootReturned bool
 keyError,rootError,rootSkippedError,keyDisposalError error
}
type originalStoreToken struct { original *originalStoreOperation; ordinal uint64; snapshot originalStoreOperation }
func(s *nativeStore)readOriginalSince(ordinal uint64)nativeStoreRead{
 if s.readOrdinal==ordinal{return nativeStoreRead{}}
 return s.readOriginal
}
func(s *nativeStore)readCapturedOriginal(name string,maximum uint32)(raw []byte,capture nativeStoreRead,operation originalStoreOperation,result error){
 token,e:=s.beginOriginalOperation(1);if e!=nil{return nil,capture,operation,e}
 defer func(){operation=s.finishOriginalOperation(token,&result)}()
 ordinal:=s.readOrdinal
 raw,result=s.readOriginalOwned(token,name,maximum)
 capture=s.readOriginalSince(ordinal)
 return
}
type originalStoreSnapshot struct {
 identity,key [32]byte
 initializationAuthorized bool
}
type originalStoreInitialization struct {
 started,ownerMode,returned bool
 profile *Profile
 host,epoch [32]byte
 snapshot originalStoreSnapshot
 ledger *Ledger
 selected,frame,raw []byte
 selectorNative,generationNative nativeStoreRead
 selectorOperation,generationOperation originalStoreOperation
 inventory uint64
 selectorReadError,selectorError,generationReadError,frameError,payloadError,generationError,inventoryError,commitError,resultError error
 commitPair originalLedgerPair
}
func(s *nativeStore)claimInitialization(profile *Profile,host,epoch [32]byte,ownerMode bool,snapshot originalStoreSnapshot)(*originalStoreInitialization,error){
 if s==nil{return nil,ErrInvalid};s.operationMu.Lock();defer s.operationMu.Unlock()
 if s.initialization.started||s.operation.active||s.operation.unresolved||s.uncertain||s.disposed{return nil,ErrUnavailable}
 s.initialization=originalStoreInitialization{started:true,ownerMode:ownerMode,profile:profile,host:host,epoch:epoch,snapshot:snapshot}
 return &s.initialization,nil
}
// Snapshot only immutable admitted values while no native effect owns the
// store. Subsequent native operations must still acquire their own token.
func(s *nativeStore)snapshotOriginal()(originalStoreSnapshot,error){
 if s==nil{return originalStoreSnapshot{},ErrInvalid}
 s.operationMu.Lock();defer s.operationMu.Unlock()
 if s.operation.active||s.operation.unresolved||s.uncertain||s.disposed{return originalStoreSnapshot{},ErrUnavailable}
 return originalStoreSnapshot{identity:s.rootIdentity,key:s.key,initializationAuthorized:s.initializationAuthorized},nil
}
func(s *nativeStore)disposeOriginalKey(token originalStoreToken)error{
 s.operationMu.Lock();defer s.operationMu.Unlock()
 if !s.operationUnchangedLocked(token){return ErrClosure}
 s.disposed=true;for i:=range s.key{s.key[i]=0};return nil
}
func(s *nativeStore)closeOriginalStoreOwned(token originalStoreToken)error{
 if e:=s.checkOriginalOperation(token);e!=nil{return e}
 p:=token.original;c:=&p.close
 // Preserve each actual close result before the next effect or metadata
 // transition. A failed/uncertain key close does not license root disposal.
 e:=s.closeOwned(p.keyOriginal);c.keyError=e;c.keyReturned=true
 if e!=nil{c.rootSkippedError=ErrClosure;return e}
 e=s.closeOwned(p.rootOriginal);c.rootError=e;c.rootReturned=true
 if e!=nil{return e}
 e=s.disposeOriginalKey(token);c.keyDisposalError=e;return e
}
func(s *nativeStore)beginOriginalOperation(kind byte)(originalStoreToken,error){
 if s==nil||kind<1||kind>5{return originalStoreToken{},ErrInvalid}
 s.operationMu.Lock();defer s.operationMu.Unlock()
 if s.operation.active||s.operation.unresolved||s.uncertain||s.disposed{return originalStoreToken{},ErrUnavailable}
 if s.operationOrdinal==^uint64(0){s.uncertain=true;return originalStoreToken{},ErrUnavailable}
 s.operationOrdinal++
 s.operation=originalStoreOperation{store:s,ordinal:s.operationOrdinal,kind:kind,active:true,
  rootOriginal:s.root,keyOriginal:s.keyHandle,rootIdentityOriginal:s.rootIdentity,ownerOriginal:s.owner}
 return originalStoreToken{original:&s.operation,ordinal:s.operationOrdinal,snapshot:s.operation},nil
}
func(s *nativeStore)operationUnchangedLocked(token originalStoreToken)bool{
 p:=token.original
 return p!=nil&&p==&s.operation&&p.store==s&&p.active&&!p.unresolved&&
  token.ordinal!=0&&p.ordinal==token.ordinal&&s.operationOrdinal==token.ordinal&&
  s.root==p.rootOriginal&&s.keyHandle==p.keyOriginal&&
  s.rootIdentity==p.rootIdentityOriginal&&s.owner==p.ownerOriginal
}
func(s *nativeStore)checkOriginalOperation(token originalStoreToken)error{
 if s==nil{return ErrInvalid};s.operationMu.Lock();defer s.operationMu.Unlock()
 if !s.operationUnchangedLocked(token){return ErrClosure};return nil
}
func(s *nativeStore)finishOriginalOperation(token originalStoreToken,result *error)(original originalStoreOperation){
 if s==nil||result==nil{return};s.operationMu.Lock();defer s.operationMu.Unlock()
 p:=token.original
 if p==nil||p!=&s.operation||token.ordinal==0||p.ordinal!=token.ordinal||s.operationOrdinal!=token.ordinal||!p.active||p.unresolved{
  // The address is reusable after successful transfer. An old token cannot
  // publish its return into a newer operation occupying that same address.
  original=token.snapshot;original.resultError=*result;original.publicationError=ErrClosure;original.unresolved=true
  if *result==nil{*result=ErrClosure};return
 }
 defer func(){original=*p}()
 p.resultError=*result
 if !s.operationUnchangedLocked(token){
  p.publicationError=ErrClosure;p.unresolved=true;s.uncertain=true
  if *result==nil{*result=ErrClosure};return
 }
 if *result!=nil{p.unresolved=true;s.uncertain=true}else{p.complete=true}
 p.active=false
 return
}
