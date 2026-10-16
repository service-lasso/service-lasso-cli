//go:build linux || windows

package hostadmission

// These decoders implement only the selected HA4 observer grammar. A frame,
// digest or index cannot enroll an observer, install a retention reservation,
// or replace an original native duplicate. The owning service must correlate
// every decoded value with its independently admitted retained originals.
type observerBindRequest struct{
 primary,run,retention [32]byte
 primaryIndex,stdoutIndex,stderrIndex uint32
}
func parseObserverBind(frame *Frame)(request observerBindRequest,err error){
 if frame==nil||frame.Length!=113{return request,ErrInvalid}
 c:=Cursor{Raw:frame.Bytes()};op,e:=c.U8();sequence,se:=c.U32()
 if e!=nil||se!=nil||op!=7||sequence!=0{return request,ErrInvalid}
 request.primary,e=c.B32();if e!=nil{return request,e}
 request.run,e=c.B32();if e!=nil{return request,e}
 request.primaryIndex,e=c.U32();if e!=nil{return request,e}
 request.stdoutIndex,e=c.U32();if e!=nil{return request,e}
 request.stderrIndex,e=c.U32();if e!=nil{return request,e}
 request.retention,e=c.B32();if e!=nil{return request,e}
 if request.primary==[32]byte{}||request.run==[32]byte{}||request.retention==[32]byte{}||request.primaryIndex>=MaximumReferences||request.stdoutIndex>=MaximumReferences||request.stderrIndex>=MaximumReferences||request.primaryIndex==request.stdoutIndex||request.primaryIndex==request.stderrIndex||request.stdoutIndex==request.stderrIndex{return request,ErrInvalid}
 return request,c.End()
}
func observerBindingDigest(observer [32]byte,frame *Frame)([32]byte,error){
 if observer==[32]byte{}{return [32]byte{},ErrInvalid}
 if _,e:=parseObserverBind(frame);e!=nil{return [32]byte{},e}
 return DomainHash("SLCLI-HOST-OBSERVER-BIND-2\x00",observer[:],frame.Bytes()),nil
}
type observerTerminalACK struct{
 reservation,receipt,archive [32]byte
 generation uint64
}
func parseObserverTerminalACK(raw []byte)(ack observerTerminalACK,err error){
 if len(raw)!=109{return ack,ErrInvalid}
 c:=Cursor{Raw:raw};op,e:=c.U8();sequence,se:=c.U32()
 if e!=nil||se!=nil||op!=10||sequence!=1{return ack,ErrInvalid}
 ack.reservation,e=c.B32();if e!=nil{return ack,e}
 ack.generation,e=c.U64();if e!=nil{return ack,e}
 ack.receipt,e=c.B32();if e!=nil{return ack,e}
 ack.archive,e=c.B32();if e!=nil{return ack,e}
 if ack.reservation==[32]byte{}||ack.generation==0||ack.receipt==[32]byte{}||ack.archive==[32]byte{}{return ack,ErrInvalid}
 return ack,c.End()
}
func validateObserverBindACK(frame *Frame)error{
 if frame==nil||frame.Length<7||frame.Length>38{return ErrInvalid}
 c:=Cursor{Raw:frame.Bytes()};op,e:=c.U8();sequence,se:=c.U32();status,ste:=c.U8()
 if e!=nil||se!=nil||ste!=nil||op!=8||sequence!=0||status>1{return ErrInvalid}
 if status==1{code,e:=c.U8();if e!=nil||code<1||code>8{return ErrInvalid};return c.End()}
 binding,e:=c.B32();if e!=nil||binding==[32]byte{}{return ErrInvalid};return c.End()
}
// Receipt shape does not prove FREE. The consumer must correlate the exact
// original closure, committed generation and independently read archive before
// it acknowledges this body. A zero intent is allowed only for retained or
// quarantined failures that never accepted CLOSE_INTENT.
func validateObserverReceiptBody(raw []byte)error{
 if len(raw)!=183{return ErrInvalid}
 c:=Cursor{Raw:raw};op,e:=c.U8();sequence,se:=c.U32();id,ie:=c.B32();slot,sle:=c.U8()
 observer,oe:=c.B32();intent,inte:=c.B32();generation,ge:=c.U64()
 closure,ce:=c.B32();ledger,le:=c.B32();tick,te:=c.U64();disposition,de:=c.U8()
 if e!=nil||se!=nil||ie!=nil||sle!=nil||oe!=nil||inte!=nil||ge!=nil||ce!=nil||le!=nil||te!=nil||de!=nil||op!=9||sequence!=1||id==[32]byte{}||slot>=MaximumBindings||observer==[32]byte{}||generation==0||closure==[32]byte{}||ledger==[32]byte{}||tick==0||disposition>2||disposition==0&&intent==[32]byte{}{return ErrInvalid}
 return c.End()
}
