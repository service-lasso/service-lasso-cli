package main

import (
 "fmt"
 "io"
 "os"
)

// The extra byte detects an oversized command without accepting a truncated
// prefix. This original region stays alive while every entry borrows its bytes.
// Production parent accounting must include this region, runtime, metadata and
// original stdin/stdout custody in the same primary/SEA/writer source domain.
const writerCommandCapacity = 768000
var originalWriterInput *writerInputArena

func runWriterV3()(result error){
 if originalWriterInput!=nil{return fmt.Errorf("writer attempt already born")}
 arena,e:=openWriterInputArena(writerCommandCapacity+1);if e!=nil{return e};originalWriterInput=arena
 used:=0
 for {
  if used>=len(arena.bytes){return fmt.Errorf("writer input quota exceeded")}
  end:=used+16384;if end>len(arena.bytes){end=len(arena.bytes)}
  n,readErr:=os.Stdin.Read(arena.bytes[used:end]);if n<0||n>end-used{return fmt.Errorf("invalid original input observation")};used+=n
  if used>writerCommandCapacity{return fmt.Errorf("writer input quota exceeded")}
  if readErr==io.EOF{break};if readErr!=nil{return fmt.Errorf("original writer input unavailable")};if n==0{return fmt.Errorf("writer input made no progress")}
 }
 attempt,e:=parseWriterAttempt(arena.bytes[:used]);if e!=nil{return e}
 digest,e:=newWriterReadbackDigest(attempt.nonce,attempt.readReceipt,attempt.planDigest,attempt.count);if e!=nil{return e}
 if e=materializeObserved(attempt.destination,attempt.entries[:attempt.count],digest);e!=nil{return e}
 actual,e:=digest.finish();if e!=nil{return e}
 // Materialization has completed all original file/directory closes before
 // this release. A failed release is terminal and cannot emit a success body.
 if e=arena.closeOriginal();e!=nil{return e}
 for remaining:=actual[:];len(remaining)>0;{
  n,e:=os.Stdout.Write(remaining);if e!=nil||n<=0||n>len(remaining){return fmt.Errorf("original writer result unavailable")};remaining=remaining[n:]
 }
 return nil
}
