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
	"io"
	"os"
	"path/filepath"

	"golang.org/x/sys/unix"
)

// The qualification authority is provisioned outside a repository job by the
// host owner. A passwordless sudo caller cannot create this root-owned policy,
// and this helper intentionally has no enrolment/configuration operation.
const grantPath = "/var/db/service-lasso/darwin-qualification-grant.json"

type grant struct {
	Capability string `json:"capability"`
	Device     uint64 `json:"device"`
	Inode      uint64 `json:"inode"`
}

func readGrant() (grant, error) {
	var value grant
	parent := filepath.Dir(grantPath)
	var namedStat, openedStat unix.Stat_t
	if parent != "/var/db/service-lasso" {
		return value, fmt.Errorf("missing protected grant parent")
	}
	for _, ancestor := range []string{"/var", "/var/db", parent} {
		var stat unix.Stat_t
		if unix.Lstat(ancestor, &stat) != nil || stat.Uid != 0 || stat.Mode&unix.S_IFMT != unix.S_IFDIR || stat.Mode&0022 != 0 {
			return value, fmt.Errorf("missing protected grant parent")
		}
	}
	if unix.Lstat(grantPath, &namedStat) != nil || namedStat.Uid != 0 || namedStat.Mode&unix.S_IFMT != unix.S_IFREG || namedStat.Mode&0777 != 0600 {
		return value, fmt.Errorf("missing protected grant")
	}
	fd, err := unix.Open(grantPath, unix.O_RDONLY|unix.O_NOFOLLOW, 0)
	if err != nil {
		return value, fmt.Errorf("missing protected grant")
	}
	defer unix.Close(fd)
	if unix.Fstat(fd, &openedStat) != nil || openedStat.Uid != 0 || openedStat.Mode&unix.S_IFMT != unix.S_IFREG || openedStat.Mode&0777 != 0600 || openedStat.Dev != namedStat.Dev || openedStat.Ino != namedStat.Ino {
		return value, fmt.Errorf("protected grant identity changed")
	}
	file := os.NewFile(uintptr(fd), "service-lasso-darwin-qualification-grant")
	if file == nil {
		return value, fmt.Errorf("invalid protected grant")
	}
	// Read from the opened descriptor rather than the grant pathname, so the
	// bytes remain bound to the identity validated above even if a later actor
	// replaces the name.
	bytes, err := io.ReadAll(file)
	if err != nil || json.Unmarshal(bytes, &value) != nil {
		return value, fmt.Errorf("invalid protected grant")
	}
	if _, err := base64.StdEncoding.DecodeString(value.Capability); err != nil {
		return value, fmt.Errorf("invalid grant capability")
	}
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
	policy, err := readGrant()
	if err != nil {
		os.Exit(2)
	}
	expected, err := base64.StdEncoding.DecodeString(policy.Capability)
	if err != nil || len(expected) != 32 {
		os.Exit(2)
	}
	presented := make([]byte, 32)
	if count, readErr := unix.Read(*capabilityFD, presented); readErr != nil || count != len(presented) || subtle.ConstantTimeCompare(expected, presented) != 1 {
		os.Exit(2)
	}
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
