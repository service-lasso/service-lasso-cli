//go:build linux || windows

package hostadmission

import "sync"

type nativeDeadline struct { mu sync.Mutex; birth,absolute,phaseStart,phaseEnd,previous uint64; phase byte; failed bool }
// Embedded in the actual native effect owner before the effect begins. This
// retains the SAME original geometry and each independent clock outcome;
// metadata equality is not a native-clock provenance grant.
type exitDeadlineCapture struct {
 original *nativeDeadline
 birth,absolute,phaseStart,phaseEnd,predecessor,before,after uint64
 phase byte
 beforeError,afterError,beginQualificationError,qualificationError error
 beforeNative,afterNative nativeClockCapture
 started,finished bool
}
func(c *exitDeadlineCapture)begin(d *nativeDeadline)(remaining uint64,result error){
 if c==nil||d==nil||c.started{return 0,ErrUnavailable}
 defer func(){c.beginQualificationError=result}()
 d.mu.Lock()
 if d.failed{d.mu.Unlock();return 0,ErrClosure}
 c.started=true;c.original=d;c.birth=d.birth;c.absolute=d.absolute
 c.phase=d.phase;c.phaseStart=d.phaseStart;c.phaseEnd=d.phaseEnd;c.predecessor=d.previous
 d.mu.Unlock()
 tick,e:=nativeTickOriginal(&c.beforeNative);c.before=tick;c.beforeError=e
 d.mu.Lock();defer d.mu.Unlock()
 if e!=nil||!c.matchesLocked(c.predecessor)||tick<c.predecessor||tick<c.birth||
    tick<c.phaseStart||tick>=c.phaseEnd||tick>=c.absolute{d.failed=true;return 0,ErrClosure}
 d.previous=tick
 end:=c.phaseEnd;if end>c.absolute{end=c.absolute};return end-tick,nil
}
func(c *exitDeadlineCapture)matchesLocked(previous uint64)bool{
 d:=c.original
 return d!=nil&&!d.failed&&d.birth==c.birth&&d.absolute==c.absolute&&
  d.phase==c.phase&&d.phaseStart==c.phaseStart&&d.phaseEnd==c.phaseEnd&&
  d.previous==previous&&c.birth!=0&&c.phaseStart>=c.birth&&
  c.phaseEnd>c.phaseStart&&c.phaseEnd<=c.absolute
}
func(c *exitDeadlineCapture)finish()(result error){
 if c==nil||!c.started||c.finished||c.original==nil{return ErrUnavailable}
 defer func(){c.qualificationError=result}()
 c.finished=true
 tick,e:=nativeTickOriginal(&c.afterNative);c.after=tick;c.afterError=e
 d:=c.original;d.mu.Lock();defer d.mu.Unlock()
 if e!=nil||!c.matchesLocked(c.before)||tick<c.before||tick>=c.phaseEnd||tick>=c.absolute{
  d.failed=true;return ErrClosure
 }
 d.previous=tick;return nil
}
const secondTicks uint64=1000000000
// All phases share the original birth and absolute bound. Neither a received
// BEGIN/READ/action nor a client reconnect can restart this object.
func startDeadline()(*nativeDeadline,error){tick,e:=nativeTick();if e!=nil||tick>^uint64(0)-40*secondTicks{return nil,ErrUnavailable};return &nativeDeadline{birth:tick,absolute:tick+40*secondTicks,phaseStart:tick,phaseEnd:tick+10*secondTicks,previous:tick},nil}
// This qualification factory keeps its existing phase geometry. Production
// callers still require their genuine separately retained original deadline.
func(d *nativeDeadline)observeLocked()(uint64,error){
 tick,e:=nativeTick()
 if e!=nil||d.failed||tick<d.birth||tick<d.previous||tick>=d.absolute||tick>=d.phaseEnd{d.failed=true;return tick,ErrClosure}
 d.previous=tick;return tick,nil
}
func(d *nativeDeadline)remaining()(uint64,error){if d==nil{return 0,ErrUnavailable};d.mu.Lock();defer d.mu.Unlock();tick,e:=d.observeLocked();if e!=nil{return 0,e};end:=d.phaseEnd;if end>d.absolute{end=d.absolute};return end-tick,nil}
func(d *nativeDeadline)advance(next byte)error{if d==nil{return ErrInvalid};d.mu.Lock();defer d.mu.Unlock();tick,e:=d.observeLocked();if e!=nil||next!=d.phase+1||next>3||tick<d.phaseStart{d.failed=true;return ErrClosure};maximum:=10*secondTicks;if next==3{maximum=20*secondTicks};if tick>^uint64(0)-maximum{d.failed=true;return ErrClosure};d.phase=next;d.phaseStart=tick;d.phaseEnd=tick+maximum;if d.phaseEnd>d.absolute{d.phaseEnd=d.absolute};return nil}
func(d *nativeDeadline)terminalTick()(uint64,error){if d==nil{return 0,ErrUnavailable};d.mu.Lock();defer d.mu.Unlock();return d.observeLocked()}
