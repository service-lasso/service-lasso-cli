//go:build darwin

// The helper is installed by a controlled qualification job into a root-owned
// directory. It deliberately accepts only an inherited descriptor, never a
// path: sudo therefore cannot be used as an arbitrary chflags trampoline.
package main

import (
	"crypto/subtle"
	"encoding/base64"
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"path/filepath"

	"golang.org/x/sys/unix"
)

// The qualification authority is provisioned outside a repository job by the
// host owner. A passwordless sudo caller cannot create this root-owned policy,
// and this helper intentionally has no enrolment/configuration operation.
const grantPath = "/var/db/service-lasso/darwin-qualification-grant.json"

type grant struct { Capability string `json:"capability"`; Device uint64 `json:"device"`; Inode uint64 `json:"inode"` }

func readGrant() (grant, error) {
	var value grant
	info, err := os.Lstat(grantPath)
	if err != nil || !info.Mode().IsRegular() || info.Mode().Perm()&022 != 0 || filepath.Dir(grantPath) != "/var/db/service-lasso" { return value, fmt.Errorf("missing protected grant") }
	bytes, err := os.ReadFile(grantPath); if err != nil || json.Unmarshal(bytes, &value) != nil { return value, fmt.Errorf("invalid protected grant") }
	if _, err := base64.StdEncoding.DecodeString(value.Capability); err != nil { return value, fmt.Errorf("invalid grant capability") }
	return value, nil
}

func main() {
	fd := flag.Int("fd", -1, "inherited object descriptor")
	capabilityFD := flag.Int("capability-fd", -1, "inherited per-job capability descriptor")
	device := flag.Uint64("device", 0, "expected device")
	inode := flag.Uint64("inode", 0, "expected inode")
	mode := flag.String("mode", "", "set or clear")
	flag.Parse()
	if flag.NArg() != 0 || *fd < 3 || *capabilityFD < 3 || (*mode != "set" && *mode != "clear") {
		os.Exit(2)
	}
	policy, err := readGrant(); if err != nil { os.Exit(2) }
	expected, err := base64.StdEncoding.DecodeString(policy.Capability); if err != nil || len(expected) != 32 { os.Exit(2) }
	presented := make([]byte, 32); if count, readErr := unix.Read(*capabilityFD, presented); readErr != nil || count != len(presented) || subtle.ConstantTimeCompare(expected, presented) != 1 { os.Exit(2) }
	var stat unix.Stat_t
	if err = unix.Fstat(*fd, &stat); err != nil || stat.Dev != int32(*device) || stat.Ino != *inode || uint64(stat.Dev) != policy.Device || stat.Ino != policy.Inode {
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
