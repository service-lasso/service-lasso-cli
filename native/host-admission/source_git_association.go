//go:build linux || windows

package hostadmission

import (
 "bytes"
 "crypto/sha1"
 "crypto/sha256"
 "encoding/hex"
 "strconv"
)

// Git membership comparison is not source authority. The private acquisition
// owner must already authenticate the expected commit/blob/raw association and
// retain all original native packet leases. These borrowed raw Git bodies are
// never obtained by invoking Git, parsing caller JSON or reopening a pathname.
func originalGitObjectID(kind string,raw []byte)([20]byte,error){
 var result [20]byte;if len(raw)==0||len(raw)>FrameBytes||kind!="commit"&&kind!="tree"&&kind!="blob"{return result,ErrInvalid}
 h:=sha1.New();h.Write([]byte(kind));h.Write([]byte{' '});var length [20]byte;h.Write(strconv.AppendUint(length[:0],uint64(len(raw)),10));h.Write([]byte{0});h.Write(raw)
 var digest [20]byte;sum:=h.Sum(digest[:0]);if len(sum)!=20{return result,ErrInvalid};copy(result[:],sum);return result,nil
}
func originalGitHexID(raw []byte)([20]byte,error){
 var result [20]byte;if len(raw)!=40{return result,ErrInvalid};for _,b:=range raw{if !(b>='0'&&b<='9'||b>='a'&&b<='f'){return result,ErrInvalid}}
 if _,e:=hex.Decode(result[:],raw);e!=nil{return result,ErrInvalid};return result,nil
}
// Each tree is the next originally held raw tree object on this exact path.
// No extra trees, symbolic leaf, gitlink or nonregular blob may satisfy it.
func compareOriginalGitSourcePath(commitRaw []byte,treeBodies [][]byte,path []byte,blobRaw []byte,commitID,blobID [20]byte,rawSHA256 [32]byte)error{
 if commitID==[20]byte{}||blobID==[20]byte{}||rawSHA256==[32]byte{}||len(path)==0||len(path)>FrameBytes||len(treeBodies)==0||len(treeBodies)>MaximumSourceInputs||len(blobRaw)==0||len(blobRaw)>FrameBytes{return ErrInvalid}
 actual,e:=originalGitObjectID("commit",commitRaw);if e!=nil||actual!=commitID{return ErrInvalid}
 if len(commitRaw)<46||!bytes.Equal(commitRaw[:5],[]byte("tree "))||commitRaw[45]!='\n'{return ErrInvalid}
 expectedTree,e:=originalGitHexID(commitRaw[5:45]);if e!=nil{return e}
 start:=0;treeIndex:=0
 for i:=0;i<=len(path);i++{
  if i<len(path)&&path[i]!='/'{if path[i]==0{return ErrInvalid};continue}
  component:=path[start:i];if len(component)==0||bytes.Equal(component,[]byte("."))||bytes.Equal(component,[]byte(".."))||treeIndex>=len(treeBodies){return ErrInvalid}
  body:=treeBodies[treeIndex];treeIndex++;actual,e=originalGitObjectID("tree",body);if e!=nil||actual!=expectedTree{return ErrInvalid}
  selectedID,selectedMode,e:=originalGitTreeEntry(body,component);if e!=nil{return e}
  if i==len(path){
   if treeIndex!=len(treeBodies)||selectedMode!="100644"&&selectedMode!="100755"||selectedID!=blobID{return ErrInvalid}
   actual,e=originalGitObjectID("blob",blobRaw);if e!=nil||actual!=blobID||sha256.Sum256(blobRaw)!=rawSHA256{return ErrInvalid};return nil
  }
  if selectedMode!="40000"{return ErrInvalid};expectedTree=selectedID;start=i+1
 }
 return ErrInvalid
}
func originalGitTreeEntry(raw,name []byte)([20]byte,string,error){
 var selected [20]byte;modeSelected:="";matched:=false
 for at:=0;at<len(raw);{
  space:=bytes.IndexByte(raw[at:],' ');if space<=0{return selected,"",ErrInvalid};space+=at;mode:=string(raw[at:space])
  if mode!="40000"&&mode!="100644"&&mode!="100755"&&mode!="120000"&&mode!="160000"{return selected,"",ErrInvalid}
  zero:=bytes.IndexByte(raw[space+1:],0);if zero<=0{return selected,"",ErrInvalid};zero+=space+1
  entryName:=raw[space+1:zero];if bytes.IndexByte(entryName,'/')>=0||len(raw)-zero-1<20{return selected,"",ErrInvalid}
  // Detect duplicate original entry names without a separate heap name table.
  for previous:=0;previous<at;{
   priorSpace:=bytes.IndexByte(raw[previous:at],' ');if priorSpace<=0{return selected,"",ErrInvalid};priorSpace+=previous
   priorZero:=bytes.IndexByte(raw[priorSpace+1:at],0);if priorZero<=0{return selected,"",ErrInvalid};priorZero+=priorSpace+1
   if bytes.Equal(raw[priorSpace+1:priorZero],entryName){return selected,"",ErrInvalid};previous=priorZero+21
  }
  if bytes.Equal(entryName,name){if matched{return selected,"",ErrInvalid};matched=true;copy(selected[:],raw[zero+1:zero+21]);modeSelected=mode}
  at=zero+21
 }
 if !matched||selected==[20]byte{}{return selected,"",ErrUnavailable};return selected,modeSelected,nil
}