//go:build darwin

package main

import (
	"fmt"
	"path/filepath"
	"strings"

	"golang.org/x/sys/unix"
)

func destinationParts(destination string) ([]string, error) {
	clean := filepath.Clean(destination)
	if clean != destination || !strings.HasPrefix(clean, "/") {
		return nil, fmt.Errorf("noncanonical destination")
	}
	// macOS presents /var as a system-owned alias for /private/var.  TMPDIR
	// normally lives below that alias; enter it through its canonical sibling
	// so every caller-controlled component remains an O_NOFOLLOW lookup.
	if clean == "/var" {
		clean = "/private/var"
	} else if strings.HasPrefix(clean, "/var/") {
		clean = "/private" + clean
	}
	parts := strings.Split(strings.TrimPrefix(clean, "/"), "/")
	if len(parts) == 0 || parts[0] == "" {
		return nil, fmt.Errorf("root destination")
	}
	return parts, nil
}
func openFilesystemRoot() (int, error) { return unix.Open("/", unix.O_RDONLY|unix.O_DIRECTORY, 0) }
func exclusiveCommit(parent int, candidate, leaf string) error {
	return unix.RenameatxNp(parent, candidate, parent, leaf, unix.RENAME_EXCL)
}
