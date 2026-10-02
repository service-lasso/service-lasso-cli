//go:build windows

package main

import (
 "fmt"
 "os"
 "path/filepath"
)

// The Windows build uses the native object-manager implementation kept in the
// companion source.  It is intentionally separate: Win32 pathname APIs cannot
// provide the no-reparse, held-parent guarantee required by this boundary.
func materialize(destination string, entries []entry) error {
 if !filepath.IsAbs(destination) { return fmt.Errorf("destination is not absolute") }
 return materializeNt(destination, entries)
}
var _ = os.FileMode(0)
