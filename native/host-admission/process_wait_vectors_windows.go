//go:build windows

package hostadmission

import "golang.org/x/sys/windows"

func originalProcessWaitTimedOut(p *nativeProcess)bool{
 return p!=nil&&p.exitWaitCalled&&p.exitWaitReturned&&p.exitWaitState==windows.WAIT_TIMEOUT&&
  p.exitWaitError==nil&&!p.exitCodeCalled&&!p.exitTimesCalled&&
  p.exitWaitMilliseconds!=windows.INFINITE&&
  uint64(p.exitWaitMilliseconds)==(p.exitDeadline.phaseEnd-p.exitDeadline.before)/1000000
}
func originalOwnershipReturned(c *nativeOwnershipCapture)bool{
 return c!=nil&&c.started&&c.timesReturned&&c.waitReturned&&
  c.timesError==nil&&c.waitError==nil&&c.resultError==nil&&c.publicationError==nil
}
