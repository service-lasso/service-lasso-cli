//go:build linux || windows

package hostadmission

import "encoding/binary"

const noHandleIndex uint32 = 0xffffffff
const noRuntimeAssociation uint16 = 0xffff
const runtimeRowBytes = 124
const runtimeSnapshotBytes = 6 + MaximumReferences*runtimeRowBytes
// All fields are claims until independently joined to their original native
// descriptors, source ranges, immutable membership and pinned owner frontier.
type resourceRow struct {
 id uint32
 kind,state byte
 charged uint64
 identity,observation [32]byte
 handleIndex uint32
 ownerDomain,processRole,producerSlot,consumerSlot byte
 sourceMember uint32
 nativeRights uint64
 aliasOf,physicalExtent uint16
 captureFirst,captureCount,startCut,endCut uint32
 effectFlags uint16
 inputSlot,applicability byte
}
type resourceSnapshot struct { phase byte; rows [MaximumReferences]resourceRow; count uint32; charged uint64; raw [runtimeSnapshotBytes]byte; length uint32 }
func(row resourceRow)membership()byte{return (row.applicability>>4)&7}
func(row resourceRow)terminalDependency()bool{return row.applicability&128!=0}
func validateResourceRow(row resourceRow,ordinal,count uint32)error{
 if row.kind>21||row.state>3||row.ownerDomain!=0&&row.ownerDomain!=1&&row.ownerDomain!=2&&row.ownerDomain!=4||row.processRole>7||row.producerSlot==0||row.producerSlot>128||row.sourceMember==0{return ErrInvalid}
 if row.membership()>4||row.terminalDependency()&&row.membership()!=0{return ErrInvalid}
 if (row.consumerSlot==0)!=(row.applicability&1!=0)||row.consumerSlot>128||(row.inputSlot==0)!=(row.applicability&2!=0)||row.inputSlot>128{return ErrInvalid}
 if (row.physicalExtent==noRuntimeAssociation)!=(row.applicability&4!=0)||row.physicalExtent!=noRuntimeAssociation&&row.physicalExtent>=256{return ErrInvalid}
 if row.kind>=5&&row.kind<=13&&row.physicalExtent==noRuntimeAssociation{return ErrInvalid}
 if (row.handleIndex==noHandleIndex)!=(row.applicability&8!=0)||row.handleIndex!=noHandleIndex&&row.handleIndex>=count{return ErrInvalid}
 if row.aliasOf!=noRuntimeAssociation&&uint32(row.aliasOf)>=ordinal{return ErrInvalid}
 if row.observation==([32]byte{})||row.captureFirst==0||row.captureCount==0||row.captureCount-1>^uint32(0)-row.captureFirst||row.startCut==0||row.startCut>13||row.endCut>13{return ErrInvalid}
 flags:=row.effectFlags
 if flags&^uint16(255)!=0||flags&1==0||flags&4!=0&&flags&2==0||flags&8!=0&&flags&6!=6||flags&64!=0&&flags&32==0||flags&32!=0&&flags&8==0||flags&128!=0&&(flags&8==0||row.kind>1){return ErrInvalid}
 born:=flags&8!=0
 if (row.identity==([32]byte{}))==born{return ErrInvalid}
 if !born&&(row.state<2||row.handleIndex!=noHandleIndex||row.nativeRights!=0||row.endCut!=0||flags&224!=0){return ErrInvalid}
 if !born&&flags&2==0&&flags!=1{return ErrInvalid}
 switch row.state{
 case 0:
  if flags&15!=15||flags&112!=0||row.endCut!=0{return ErrInvalid}
 case 1:
  if flags&15!=15||flags&16!=0||row.endCut==0||row.handleIndex!=noHandleIndex||flags&96!=0&&flags&96!=96||row.kind<=1&&flags&128==0{return ErrInvalid}
 case 2,3:
  if row.endCut!=0||flags&2!=0&&flags&16==0{return ErrInvalid}
 }
 handlePolicy:=byte(2) // Source variant; the private native descriptor fixes H/N.
 switch row.kind{case 0,2,3,4,19,21:handlePolicy=1;case 5,6,7,9,10,11,12,13,15,16:handlePolicy=0}
 if handlePolicy==0&&row.handleIndex!=noHandleIndex||row.state==0&&handlePolicy==1&&row.handleIndex==noHandleIndex{return ErrInvalid}
 return nil
}
func parseSnapshot(raw []byte)(resourceSnapshot,error){
 var out resourceSnapshot
 if len(raw)<6||len(raw)>len(out.raw){return out,ErrInvalid}
 if raw[0]!=2||raw[1]>7{return out,ErrInvalid};count:=binary.BigEndian.Uint32(raw[2:6]);if count==0||count>MaximumReferences||len(raw)!=6+runtimeRowBytes*int(count){return out,ErrInvalid}
 out.phase=raw[1];out.count=count
 for i:=uint32(0);i<count;i++{
  b:=raw[6+runtimeRowBytes*int(i):6+runtimeRowBytes*int(i+1)];row:=&out.rows[i]
  row.id=binary.BigEndian.Uint32(b[0:4]);row.kind=b[4];row.state=b[5];row.charged=binary.BigEndian.Uint64(b[6:14]);copy(row.identity[:],b[14:46]);row.handleIndex=binary.BigEndian.Uint32(b[46:50]);copy(row.observation[:],b[50:82]);row.ownerDomain=b[82];row.processRole=b[83];row.producerSlot=b[84];row.consumerSlot=b[85];row.sourceMember=binary.BigEndian.Uint32(b[86:90]);row.nativeRights=binary.BigEndian.Uint64(b[90:98]);row.aliasOf=binary.BigEndian.Uint16(b[98:100]);row.physicalExtent=binary.BigEndian.Uint16(b[100:102]);row.captureFirst=binary.BigEndian.Uint32(b[102:106]);row.captureCount=binary.BigEndian.Uint32(b[106:110]);row.startCut=binary.BigEndian.Uint32(b[110:114]);row.endCut=binary.BigEndian.Uint32(b[114:118]);row.effectFlags=binary.BigEndian.Uint16(b[118:120]);row.inputSlot=b[120];row.applicability=b[121]
  if b[122]!=0||b[123]!=0||i>0&&row.id<=out.rows[i-1].id||row.charged>^uint64(0)-out.charged{return resourceSnapshot{},ErrInvalid}
  if e:=validateResourceRow(*row,i,count);e!=nil{return resourceSnapshot{},e};out.charged+=row.charged
 }
 // No SessionBytes substitution for the simultaneous full Node domain budgets.
 // Independent native owner admission must check every actual domain subtotal.
 copy(out.raw[:],raw);out.length=uint32(len(raw));return out,nil
}
func encodeSnapshot(snapshot resourceSnapshot)([]byte,error){
 if snapshot.phase>7||snapshot.count==0||snapshot.count>MaximumReferences{return nil,ErrInvalid}
 raw:=make([]byte,6+runtimeRowBytes*int(snapshot.count));raw[0]=2;raw[1]=snapshot.phase;binary.BigEndian.PutUint32(raw[2:6],snapshot.count)
 for i:=uint32(0);i<snapshot.count;i++{row:=snapshot.rows[i];b:=raw[6+runtimeRowBytes*int(i):6+runtimeRowBytes*int(i+1)];binary.BigEndian.PutUint32(b[0:4],row.id);b[4]=row.kind;b[5]=row.state;binary.BigEndian.PutUint64(b[6:14],row.charged);copy(b[14:46],row.identity[:]);binary.BigEndian.PutUint32(b[46:50],row.handleIndex);copy(b[50:82],row.observation[:]);b[82]=row.ownerDomain;b[83]=row.processRole;b[84]=row.producerSlot;b[85]=row.consumerSlot;binary.BigEndian.PutUint32(b[86:90],row.sourceMember);binary.BigEndian.PutUint64(b[90:98],row.nativeRights);binary.BigEndian.PutUint16(b[98:100],row.aliasOf);binary.BigEndian.PutUint16(b[100:102],row.physicalExtent);binary.BigEndian.PutUint32(b[102:106],row.captureFirst);binary.BigEndian.PutUint32(b[106:110],row.captureCount);binary.BigEndian.PutUint32(b[110:114],row.startCut);binary.BigEndian.PutUint32(b[114:118],row.endCut);binary.BigEndian.PutUint16(b[118:120],row.effectFlags);b[120]=row.inputSlot;b[121]=row.applicability}
 if _,e:=parseSnapshot(raw);e!=nil{return nil,e};return raw,nil
}
// These seals are held only inside the source-owned native service. A parsed
// request snapshot is routing data until every resource is independently
// correlated with the original duplicate and its actual retained observation.
type verifiedSnapshot struct { originals *originalObjects; raw []byte; charged,tick uint64; verified bool; parsed resourceSnapshot }
type verifiedClosure struct { originals *originalObjects; raw []byte; tick uint64; naturalZero,sessionClosed,verified bool }
type verifiedArchive struct { originals *originalObjects; raw []byte; digest [32]byte; verified bool }
