//go:build linux

package hostadmission

func originalProcessWaitTimedOut(p *nativeProcess)bool{
 return p!=nil&&p.exitPollCalled&&p.exitPollReturned&&p.exitPollCount==0&&
  p.exitPollError==nil&&!p.exitWaitCalled&&!p.exitWaitReturned&&
  p.exitWaitMilliseconds>=0&&
  uint64(p.exitWaitMilliseconds)==(p.exitDeadline.phaseEnd-p.exitDeadline.before)/1000000
}
func originalOwnershipReturned(c *nativeOwnershipCapture)bool{
 return c!=nil&&c.started&&c.returned&&c.nativeError==nil&&
  c.resultError==nil&&c.publicationError==nil
}
