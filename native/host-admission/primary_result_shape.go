//go:build linux || windows

package hostadmission

func validatePrimaryResult(frame *Frame)error{
 if frame==nil||frame.Length<7||frame.Length>256{return ErrInvalid}
 cursor:=Cursor{Raw:frame.Bytes()};operation,e:=cursor.U8();sequence,se:=cursor.U32();status,ste:=cursor.U8()
 if e!=nil||se!=nil||ste!=nil||status>1{return ErrInvalid}
 switch operation{case 2:if sequence!=0{return ErrInvalid};case 4:if sequence==0||sequence>1024{return ErrInvalid};case 6:if sequence==0||sequence>1025{return ErrInvalid};default:return ErrInvalid}
 if status==1{code,e:=cursor.U8();if e!=nil||code<1||code>8{return ErrInvalid};return cursor.End()}
 switch operation{
 case 2:
  id,e:=cursor.B32();slot,sle:=cursor.U8();generation,ge:=cursor.U64();observer,oe:=cursor.B32();deadline,de:=cursor.U64()
  if e!=nil||sle!=nil||ge!=nil||oe!=nil||de!=nil||id==[32]byte{}||slot>=MaximumBindings||generation==0||observer==[32]byte{}||deadline==0{return ErrInvalid}
 case 4:
  generation,e:=cursor.U64();digest,de:=cursor.B32();if e!=nil||de!=nil||generation==0||digest==[32]byte{}{return ErrInvalid}
 case 6:
  generation,e:=cursor.U64();intent,ie:=cursor.B32();tick,te:=cursor.U64();if e!=nil||ie!=nil||te!=nil||generation==0||intent==[32]byte{}||tick==0{return ErrInvalid}
 }
 return cursor.End()
}
