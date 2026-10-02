//go:build !windows

package main

import (
	"crypto/rand"
	"encoding/hex"
	"fmt"
	"golang.org/x/sys/unix"
	"strings"
)

// Every lookup below is relative to a descriptor already opened with
// O_NOFOLLOW.  No untrusted descendant is subsequently addressed from the
// process working directory.  Rollback uses the same held parent descriptors
// and records every created leaf before it can be written.  POSIX has no
// unlink-by-handle operation: a pathname identity check followed by unlink is
// racy.  Therefore rollback deliberately retains a named partial leaf rather
// than risk deleting a concurrent replacement.  Windows can delete through
// its held file handle and does so in its platform implementation.
func openDir(name string, parent int) (int, error) {
	return unix.Openat(parent, name, unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW, 0)
}
func closeAll(handles []int) {
	for i := len(handles) - 1; i >= 0; i-- {
		_ = unix.Close(handles[i])
	}
}
func createCandidate(parent int) (string, error) {
	for range 32 {
		bytes := make([]byte, 16)
		if _, err := rand.Read(bytes); err != nil {
			return "", err
		}
		name := ".service-lasso-txn-" + hex.EncodeToString(bytes)
		if err := unix.Mkdirat(parent, name, 0700); err == nil {
			return name, nil
		} else if err != unix.EEXIST {
			return "", err
		}
	}
	return "", fmt.Errorf("candidate name collision")
}
func materialize(destination string, entries []entry) error {
	parts, err := destinationParts(destination)
	if err != nil {
		return err
	}
	root, err := openFilesystemRoot()
	if err != nil {
		return err
	}
	handles := []int{root}
	parent := root
	defer closeAll(handles)
	for _, part := range parts[:len(parts)-1] {
		next, e := openDir(part, parent)
		if e != nil {
			return e
		}
		handles = append(handles, next)
		parent = next
	}
	leaf := parts[len(parts)-1]
	if err = testGate("before-project-create"); err != nil {
		return err
	}
	// Build beneath a held parent under an unguessable, invocation-owned
	// candidate.  The final leaf is untouched until the exclusive commit.
	candidate, e := createCandidate(parent)
	if e != nil {
		return e
	}
	project, e := openDir(candidate, parent)
	if e != nil {
		return e
	}
	handles = append(handles, project)
	for _, item := range entries {
		current := project
		parts := strings.Split(item.path, "/")
		for _, part := range parts[:len(parts)-1] {
			child, e := openDir(part, current)
			if e != nil {
				if e != unix.ENOENT {
					return e
				}
				if e = unix.Mkdirat(current, part, 0700); e != nil {
					return e
				}
				child, e = openDir(part, current)
				if e != nil {
					return e
				}
			}
			handles = append(handles, child)
			current = child
		}
		name := parts[len(parts)-1]
		fd, e := unix.Openat(current, name, unix.O_WRONLY|unix.O_CREAT|unix.O_EXCL|unix.O_NOFOLLOW, uint32(item.mode))
		if e != nil {
			return e
		}
		handles = append(handles, fd)
		written := 0
		for written < len(item.bytes) {
			count, we := unix.Write(fd, item.bytes[written:])
			if we != nil {
				return we
			}
			if count == 0 {
				return fmt.Errorf("short native write")
			}
			written += count
		}
		if e = unix.Fsync(fd); e != nil {
			return e
		}
		if err = testGate("after-file-write"); err != nil {
			return err
		}
	}
	if err = testGate("before-project-commit"); err != nil {
		return err
	}
	// Each supported POSIX host supplies an atomic no-replace rename relative
	// to the same held parent.  A concurrent leaf wins intact; it is never
	// overwritten or removed.  A failed candidate is deliberately retained as
	// unknown state because POSIX has no unlink-by-held-object primitive.
	return exclusiveCommit(parent, candidate, leaf)
}
