//go:build !windows

package main

import (
	"fmt"
	"golang.org/x/sys/unix"
	"path/filepath"
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
type owned struct {
	parent    int
	name      string
	dev, ino  uint64
	directory bool
	handle    int
}

func openDir(name string, parent int) (int, error) {
	return unix.Openat(parent, name, unix.O_RDONLY|unix.O_DIRECTORY|unix.O_NOFOLLOW, 0)
}
func id(fd int) (uint64, uint64, error) {
	var st unix.Stat_t
	if err := unix.Fstat(fd, &st); err != nil {
		return 0, 0, err
	}
	return uint64(st.Dev), uint64(st.Ino), nil
}
func closeAll(handles []int) {
	for i := len(handles) - 1; i >= 0; i-- {
		_ = unix.Close(handles[i])
	}
}
func materialize(destination string, entries []entry) error {
	clean := filepath.Clean(destination)
	if clean != destination {
		return fmt.Errorf("noncanonical destination")
	}
	parts := strings.Split(strings.TrimPrefix(clean, "/"), "/")
	if len(parts) == 0 {
		return fmt.Errorf("root destination")
	}
	// "/" is the process-independent filesystem root, not an untrusted
	// descendant. Darwin rejects O_NOFOLLOW on this already-rooted directory;
	// every caller-controlled component below still opens through openDir.
	root, err := unix.Open("/", unix.O_RDONLY|unix.O_DIRECTORY, 0)
	if err != nil {
		return err
	}
	handles := []int{root}
	parent := root
	ownedEntries := []owned{}
	defer closeAll(handles)
	rollback := func() {
		// Never resolve a leaf again for deletion.  An attacker can rename it after
		// a stat, and POSIX offers no identity-anchored unlink.  Its held descriptor
		// is still closed below, so no handle leaks across a failed invocation.
		for i := len(ownedEntries) - 1; i >= 0; i-- {
			if ownedEntries[i].handle >= 0 {
				_ = unix.Close(ownedEntries[i].handle)
				ownedEntries[i].handle = -1
			}
		}
	}
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
	if err = unix.Mkdirat(parent, leaf, 0700); err != nil {
		return err
	}
	project, e := openDir(leaf, parent)
	if e != nil {
		rollback()
		return e
	}
	d, n, e := id(project)
	if e != nil {
		rollback()
		return e
	}
	ownedEntries = append(ownedEntries, owned{parent, leaf, d, n, true, project})
	handles = append(handles, project)
	for _, item := range entries {
		current := project
		parts := strings.Split(item.path, "/")
		for _, part := range parts[:len(parts)-1] {
			child, e := openDir(part, current)
			if e != nil {
				if e != unix.ENOENT {
					rollback()
					return e
				}
				if e = unix.Mkdirat(current, part, 0700); e != nil {
					rollback()
					return e
				}
				child, e = openDir(part, current)
				if e != nil {
					rollback()
					return e
				}
				cd, cn, e := id(child)
				if e != nil {
					rollback()
					return e
				}
				ownedEntries = append(ownedEntries, owned{current, part, cd, cn, true, child})
			}
			handles = append(handles, child)
			current = child
		}
		name := parts[len(parts)-1]
		fd, e := unix.Openat(current, name, unix.O_WRONLY|unix.O_CREAT|unix.O_EXCL|unix.O_NOFOLLOW, uint32(item.mode))
		if e != nil {
			rollback()
			return e
		}
		fdDev, fdIno, e := id(fd)
		if e != nil {
			_ = unix.Close(fd)
			rollback()
			return e
		}
		ownedEntries = append(ownedEntries, owned{current, name, fdDev, fdIno, false, fd})
		written := 0
		for written < len(item.bytes) {
			count, we := unix.Write(fd, item.bytes[written:])
			if we != nil {
				rollback()
				return we
			}
			if count == 0 {
				rollback()
				return fmt.Errorf("short native write")
			}
			written += count
		}
		if e = unix.Fsync(fd); e != nil {
			rollback()
			return e
		}
		if err = testGate("after-file-write"); err != nil {
			rollback()
			return err
		}
	}
	return nil
}
