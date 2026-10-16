//go:build linux || windows

package hostadmission

import "errors"

// Keep the existing closed qualification category and the actual native cause.
// Public response codecs continue selecting their closed enum with errors.Is;
// this internal chain is not a native receipt or source-admission grant.
func qualifyNativeFailure(qualification,original error)error{
 if original==nil{return qualification}
 return errors.Join(qualification,original)
}
