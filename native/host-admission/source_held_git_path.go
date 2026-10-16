//go:build linux || windows

package hostadmission

// This is a custody/membership comparison inside the original provider lease,
// never an authority issuer. The acquisition owner must authenticate the
// repository, original input ROOT and source association independently before
// it calls this reader. No pathname is opened and no request installs a source.
// All borrowed original bodies remain held on success and on failure through
// their original archive/readback and physical-accounting obligations.
type originalGitSourceLease struct{
 commit *originalSourceBody
 trees [MaximumSourceInputs]*originalSourceBody
 treeCount uint32
 blob *originalSourceBody
 path []byte
 commitID,blobID [20]byte
 rawSHA256 [32]byte
}
func(lease *originalGitSourceLease)compareHeldSourcePath()(err error){
 if lease==nil{return ErrUnavailable}
 defer func(){if err!=nil{lease.retainFailure()}}()
 if lease.commit==nil||lease.blob==nil||lease.treeCount==0||lease.treeCount>MaximumSourceInputs||len(lease.path)==0||len(lease.path)>FrameBytes||lease.commitID==[20]byte{}||lease.blobID==[20]byte{}||lease.rawSHA256==[32]byte{}{return ErrInvalid}
 if sameSourceBodyStorage(lease.commit,lease.blob){return ErrInvalid}
 for i:=uint32(0);i<lease.treeCount;i++{
  body:=lease.trees[i];if body==nil||sameSourceBodyStorage(body,lease.commit)||sameSourceBodyStorage(body,lease.blob){return ErrInvalid}
  for j:=uint32(0);j<i;j++{if sameSourceBodyStorage(body,lease.trees[j]){return ErrInvalid}}
 }
 // No hidden extra object can be substituted for an exact path chain.
 for i:=lease.treeCount;i<MaximumSourceInputs;i++{if lease.trees[i]!=nil{return ErrInvalid}}
 // The source-read observation's gitBlob is the framed blob checksum of
 // the retained file body, including for files containing commit/tree bytes.
 // It is not the commit/tree identity. The comparison below independently
 // computes those identities with their genuine original Git object types.
 commit,e:=recheckOriginalSourceBody(lease.commit.handle,lease.commit.memory,&lease.commit.observation);if e!=nil{return e}
 blob,e:=recheckOriginalSourceBody(lease.blob.handle,lease.blob.memory,&lease.blob.observation);if e!=nil{return e}
 var trees [MaximumSourceInputs][]byte
 for i:=uint32(0);i<lease.treeCount;i++{body:=lease.trees[i];trees[i],e=recheckOriginalSourceBody(body.handle,body.memory,&body.observation);if e!=nil{return e}}
 if e=compareOriginalGitSourcePath(commit,trees[:lease.treeCount],lease.path,blob,lease.commitID,lease.blobID,lease.rawSHA256);e!=nil{return e}
 // Original-object observations after the complete chain comparison are
 // required; a matching hash from an earlier read cannot finish custody.
 if _,e=recheckOriginalSourceBody(lease.commit.handle,lease.commit.memory,&lease.commit.observation);e!=nil{return e}
 for i:=uint32(0);i<lease.treeCount;i++{body:=lease.trees[i];if _,e=recheckOriginalSourceBody(body.handle,body.memory,&body.observation);e!=nil{return e}}
 if _,e=recheckOriginalSourceBody(lease.blob.handle,lease.blob.memory,&lease.blob.observation);e!=nil{return e}
 return nil
}
func(lease *originalGitSourceLease)retainFailure(){
 if lease==nil{return};retainFailedSourceBody(lease.commit);retainFailedSourceBody(lease.blob)
 // Includes every supplied original, even when an invalid count or extra
 // object caused failure before the reader visited its body.
 for i:=uint32(0);i<MaximumSourceInputs;i++{retainFailedSourceBody(lease.trees[i])}
}
