//go:build linux || windows

package hostadmission

import "testing"

// Codec claims only. These vectors are not native objects, authentic source
// inputs, installed qualification, identity witnesses or execution admission.
func TestSectionSnapshotVersionAndHandlePolicy(t *testing.T) {
 section:=resourceRow{id:1,kind:22,charged:4096,identity:[32]byte{1},observation:[32]byte{2},
  handleIndex:0,ownerDomain:2,processRole:1,producerSlot:1,sourceMember:1,nativeRights:4,
  aliasOf:noRuntimeAssociation,physicalExtent:0,captureFirst:1,captureCount:1,startCut:1,
  effectFlags:15,applicability:3}
 view:=resourceRow{id:2,kind:9,charged:4096,identity:[32]byte{3},observation:[32]byte{4},
  handleIndex:noHandleIndex,ownerDomain:2,processRole:1,producerSlot:1,sourceMember:2,nativeRights:4,
  aliasOf:0,physicalExtent:0,captureFirst:2,captureCount:1,startCut:1,effectFlags:15,applicability:11}
 snapshot:=resourceSnapshot{count:2};snapshot.rows[0]=section;snapshot.rows[1]=view
 raw,e:=encodeSnapshot(snapshot);if e!=nil{t.Fatal(e)}
 parsed,e:=parseSnapshot(raw);if e!=nil||parsed.count!=2||parsed.rows[0].kind!=22||parsed.rows[1].aliasOf!=0{t.Fatal("v3 section/view claims rejected",e)}
 for _,version:=range []byte{1,2}{old:=append([]byte(nil),raw...);old[0]=version;if _,e:=parseSnapshot(old);e==nil{t.Fatalf("old version %d accepted",version)}}
 absent:=section;absent.handleIndex=noHandleIndex;absent.applicability|=8
 if validateResourceRow(absent,0,2)==nil{t.Fatal("live section without HANDLE accepted")}
 unjoined:=section;unjoined.physicalExtent=noRuntimeAssociation;unjoined.applicability|=4
 if validateResourceRow(unjoined,0,2)==nil{t.Fatal("section without original physical extent accepted")}
 extra:=section;extra.kind=23;if validateResourceRow(extra,0,2)==nil{t.Fatal("unselected class accepted")}
 wrongView:=view;wrongView.handleIndex=1;wrongView.applicability&^=8
 if validateResourceRow(wrongView,1,2)==nil{t.Fatal("private view with HANDLE accepted")}
 forward:=view;forward.aliasOf=1;if validateResourceRow(forward,1,2)==nil{t.Fatal("forward alias accepted")}
}
