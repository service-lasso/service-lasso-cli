//go:build darwin

// The helper is installed by a controlled qualification job into a root-owned
// directory. It deliberately accepts only an inherited descriptor, never a
// path: sudo therefore cannot be used as an arbitrary chflags trampoline.
package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"

	"golang.org/x/sys/unix"
)

func requiredOwner() (uint32, error) {
	value := os.Getenv("SUDO_UID")
	uid, err := strconv.ParseUint(value, 10, 32)
	if err != nil {
		return 0, fmt.Errorf("missing sudo caller identity")
	}
	return uint32(uid), nil
}

func main() {
	fd := flag.Int("fd", -1, "inherited object descriptor")
	device := flag.Uint64("device", 0, "expected device")
	inode := flag.Uint64("inode", 0, "expected inode")
	mode := flag.String("mode", "", "set or clear")
	flag.Parse()
	if flag.NArg() != 0 || *fd < 3 || (*mode != "set" && *mode != "clear") {
		os.Exit(2)
	}
	owner, err := requiredOwner()
	if err != nil {
		os.Exit(2)
	}
	var stat unix.Stat_t
	if err = unix.Fstat(*fd, &stat); err != nil || stat.Dev != int32(*device) || stat.Ino != *inode || stat.Uid != owner {
		os.Exit(2)
	}
	flags := unix.SF_IMMUTABLE
	if *mode == "clear" {
		flags = 0
	}
	if err = unix.Fchflags(*fd, flags); err != nil {
		os.Exit(1)
	}
}
