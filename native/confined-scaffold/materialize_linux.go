//go:build linux

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
	parts := strings.Split(strings.TrimPrefix(clean, "/"), "/")
	if len(parts) == 0 || parts[0] == "" {
		return nil, fmt.Errorf("root destination")
	}
	return parts, nil
}
func openFilesystemRoot() (int, error) { return unix.Open("/", unix.O_RDONLY|unix.O_DIRECTORY, 0) }
func exclusiveCommit(parent int, candidate, leaf string) error {
	return unix.Renameat2(parent, candidate, parent, leaf, unix.RENAME_NOREPLACE)
}
