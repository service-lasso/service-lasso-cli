//go:build linux || windows

package hostadmission

import "crypto/sha256"

// These are actual-method source vectors, not constructors or source grants.
// The independently admitted native harness supplies an authentic original
// process/deadline and establishes real running-to-exit or still-alive timing.
func originalProcessWaitPositiveVector(p *nativeProcess,d *nativeDeadline)error{
 if p==nil||d==nil||p.exitAttempted{return ErrInvalid}
 result,e:=p.observeOriginalExit(d)
 if e!=nil||!result.ended||!result.naturalZero||len(result.raw)==0||
    result.digest!=sha256.Sum256(result.raw)||len(result.nativeWait)==0||
    p.exitBusy||p.exitDeadline.original!=d||!p.exitDeadline.started||
    !p.exitDeadline.finished||p.exitDeadline.beforeError!=nil||
    p.exitDeadline.afterError!=nil||p.exitDeadline.qualificationError!=nil||
    p.exitDeadline.after<p.exitDeadline.before||p.exitDeadline.after>=p.exitDeadline.phaseEnd||
    p.exitDeadline.after>=p.exitDeadline.absolute{return ErrInvalid}
 return nil
}
func originalProcessWaitTimeoutVector(p *nativeProcess,d *nativeDeadline)error{
 if p==nil||d==nil||p.exitAttempted{return ErrInvalid}
 result,e:=p.observeOriginalExit(d)
 if e==nil||result.ended||!originalProcessWaitTimedOut(p)||p.exitBusy||
    p.exitDeadline.original!=d||!p.exitDeadline.finished{return ErrInvalid}
 before,after:=p.exitDeadline.before,p.exitDeadline.after
 nativeWait:=result.nativeWait;raw:=result.raw;digest:=result.digest
 repeated,repeatError:=p.observeOriginalExit(d)
 if repeatError==nil||repeated.ended||!originalProcessWaitTimedOut(p)||
    p.exitBusy||p.exitDeadline.before!=before||p.exitDeadline.after!=after||
    !sameOriginalSlice(nativeWait,repeated.nativeWait)||
    !sameOriginalSlice(raw,repeated.raw)||repeated.digest!=digest{return ErrInvalid}
 return nil
}

// The native harness invokes these two steps around actual natural process
// completion. No callback or supplied readiness boolean establishes exit.
// Both original observations remain in distinct fixture-owned result cells.
type originalOwnershipReadinessVector struct {
 original *nativeProcess
 captures [2]nativeOwnershipCapture
 firstComplete,complete bool
}
func(v *originalOwnershipReadinessVector)beforeExit(p *nativeProcess)error{
 if v==nil||p==nil||v.original!=nil{return ErrInvalid}
 v.original=p
 ended,e:=p.ownershipEndedOriginal(&v.captures[0])
 if e!=nil||ended||!originalOwnershipReturned(&v.captures[0])||
    p.ownershipOwner!=nil||p.ownershipRetained!=nil{return ErrInvalid}
 v.firstComplete=true;return nil
}
func(v *originalOwnershipReadinessVector)afterExit()error{
 if v==nil||v.original==nil||!v.firstComplete||v.complete{return ErrInvalid}
 first:=v.captures[0]
 ended,e:=v.original.ownershipEndedOriginal(&v.captures[1])
 if e!=nil||!ended||!originalOwnershipReturned(&v.captures[1])||
    v.captures[0]!=first||v.captures[0].tuple!=v.captures[1].tuple||
    v.original.ownershipOwner!=nil||v.original.ownershipRetained!=nil{return ErrInvalid}
 v.complete=true;return nil
}
