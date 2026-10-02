//go:build darwin

package main

import (
	"bytes"
	"encoding/base64"
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"testing"

	"golang.org/x/sys/unix"
)

// These actual Darwin tests require a separately admitted root qualification
// process and an owner-provisioned canonical grant. They never provision or
// overwrite the live grant, run sudo, or alter immutable flags. Missing owner
// provision is a failure, not a skipped or accepted qualification.
func requireOwner(t *testing.T) {
	t.Helper()
	if os.Geteuid() != 0 {
		t.Fatal("actual Darwin grant qualification requires owner-controlled root execution")
	}
}

func TestCanonicalOwnerGrant(t *testing.T) {
	requireOwner(t)
	if grantPath != "/private/var/db/service-lasso/darwin-qualification-grant.json" {
		t.Fatal("grant path is not canonical")
	}
	var alias unix.Stat_t
	if err := unix.Lstat("/var", &alias); err != nil || alias.Mode&unix.S_IFMT != unix.S_IFLNK {
		t.Fatal("expected normal Darwin /var system alias")
	}
	if protectedGrantParent("/var") {
		t.Fatal("system alias was accepted as a protected directory")
	}
	policy, err := readGrant()
	if err != nil {
		t.Fatalf("owner canonical grant rejected: %v", err)
	}
	capability, err := base64.StdEncoding.DecodeString(policy.Capability)
	if err != nil || len(capability) != 32 {
		t.Fatal("owner grant lacks the required 32-byte capability")
	}
}

func TestActualProtectedGrantFilesystemNegatives(t *testing.T) {
	requireOwner(t)
	root := t.TempDir()
	parent := filepath.Join(root, "parent")
	if err := os.Mkdir(parent, 0700); err != nil { t.Fatal(err) }
	if !protectedGrantParent(parent) { t.Fatal("root-owned protected directory rejected") }
	alias := filepath.Join(root, "alias")
	if err := os.Symlink(parent, alias); err != nil { t.Fatal(err) }
	if protectedGrantParent(alias) { t.Fatal("ancestor symlink accepted") }
	if protectedGrantParent(filepath.Join(root, "absent")) { t.Fatal("missing ancestor accepted") }
	for _, mode := range []os.FileMode{0720, 0702} {
		if err := os.Chmod(parent, mode); err != nil { t.Fatal(err) }
		if protectedGrantParent(parent) { t.Fatal("writable ancestor accepted") }
	}
	if err := os.Chmod(parent, 0700); err != nil { t.Fatal(err) }
	if err := os.Chown(parent, 65534, -1); err != nil { t.Fatal(err) }
	if protectedGrantParent(parent) { t.Fatal("non-root ancestor accepted") }
	if err := os.Chown(parent, 0, -1); err != nil { t.Fatal(err) }
	path := filepath.Join(parent, "grant.json")
	encoded, err := json.Marshal(grant{Capability: base64.StdEncoding.EncodeToString(bytes.Repeat([]byte{7}, 32)), Device: 1, Inode: 2})
	if err != nil { t.Fatal(err) }
	if err := os.WriteFile(path, encoded, 0600); err != nil { t.Fatal(err) }
	if _, err := readProtectedGrantFile(path); err != nil { t.Fatalf("actual protected file rejected: %v", err) }
	named, opened := unix.Stat_t{}, unix.Stat_t{}
	if err := unix.Lstat(path, &named); err != nil { t.Fatal(err) }
	held, err := os.Open(path)
	if err != nil { t.Fatal(err) }
	defer held.Close()
	if err := unix.Fstat(int(held.Fd()), &opened); err != nil { t.Fatal(err) }
	if !protectedGrantIdentity(named, opened) { t.Fatal("same actual held grant identity rejected") }
	replacement := filepath.Join(parent, "replacement.json")
	if err := os.WriteFile(replacement, encoded, 0600); err != nil { t.Fatal(err) }
	if err := os.Rename(replacement, path); err != nil { t.Fatal(err) }
	if err := unix.Lstat(path, &named); err != nil { t.Fatal(err) }
	if protectedGrantIdentity(named, opened) { t.Fatal("replaced named grant matched old held descriptor") }
	if protectedGrantParent(path) { t.Fatal("regular file accepted as ancestor") }
	if _, err := readProtectedGrantFile(parent); err == nil { t.Fatal("directory accepted as grant") }
	if _, err := readProtectedGrantFile(filepath.Join(parent, "absent")); err == nil { t.Fatal("missing grant accepted") }
	leafAlias := filepath.Join(parent, "leaf-alias")
	if err := os.Symlink(path, leafAlias); err != nil { t.Fatal(err) }
	if _, err := readProtectedGrantFile(leafAlias); err == nil { t.Fatal("grant symlink accepted") }
	for _, mode := range []os.FileMode{0640, 0604, 0400} {
		if err := os.Chmod(path, mode); err != nil { t.Fatal(err) }
		if _, err := readProtectedGrantFile(path); err == nil { t.Fatal("wrong grant mode accepted") }
	}
	if err := os.Chmod(path, 0600); err != nil { t.Fatal(err) }
	if err := os.Chown(path, 65534, -1); err != nil { t.Fatal(err) }
	if _, err := readProtectedGrantFile(path); err == nil { t.Fatal("non-root grant accepted") }
	if err := os.Chown(path, 0, -1); err != nil { t.Fatal(err) }
	if err := os.WriteFile(path, []byte(`{"capability":"invalid base64"}`), 0600); err != nil { t.Fatal(err) }
	if _, err := readProtectedGrantFile(path); err == nil { t.Fatal("malformed capability accepted") }
}

func TestActualHelperDeniesCapabilityAndObjectMismatch(t *testing.T) {
	requireOwner(t)
	policy, err := readGrant()
	if err != nil { t.Fatalf("owner canonical grant required: %v", err) }
	valid, err := base64.StdEncoding.DecodeString(policy.Capability)
	if err != nil || len(valid) != 32 { t.Fatal("owner capability must contain 32 bytes") }
	object, err := os.CreateTemp(t.TempDir(), "ungranted-object")
	if err != nil { t.Fatal(err) }
	defer object.Close()
	var stat unix.Stat_t
	if err := unix.Fstat(int(object.Fd()), &stat); err != nil { t.Fatal(err) }
	if uint64(stat.Dev) == policy.Device && stat.Ino == policy.Inode { t.Fatal("negative fixture unexpectedly granted") }
	wrong := append([]byte(nil), valid...)
	wrong[0] ^= 1
	for name, capability := range map[string][]byte{"wrong-capability": wrong, "short-capability": valid[:31], "wrong-object": valid} {
		t.Run(name, func(t *testing.T) {
			reader, writer, err := os.Pipe()
			if err != nil { t.Fatal(err) }
			defer reader.Close()
			if _, err := writer.Write(capability); err != nil { writer.Close(); t.Fatal(err) }
			if err := writer.Close(); err != nil { t.Fatal(err) }
			executable, err := os.Executable()
			if err != nil { t.Fatal(err) }
			command := exec.Command(executable, "-test.run=^TestActualHelperProcess$", "--", "-fd=3", "-capability-fd=4", fmt.Sprintf("-device=%d", uint64(stat.Dev)), fmt.Sprintf("-inode=%d", stat.Ino), "-mode=clear")
			command.Env = append(os.Environ(), "SERVICE_LASSO_TEST_HELPER_PROCESS=1")
			command.ExtraFiles = []*os.File{object, reader}
			output, err := command.CombinedOutput()
			exit, ok := err.(*exec.ExitError)
			if !ok || exit.ExitCode() != 2 || len(output) != 0 { t.Fatal("actual helper did not securely deny mismatched request") }
			var after unix.Stat_t
			if err := unix.Fstat(int(object.Fd()), &after); err != nil || after.Flags != stat.Flags { t.Fatal("denied helper changed object flags") }
		})
	}
}

func TestActualHelperProcess(t *testing.T) {
	if os.Getenv("SERVICE_LASSO_TEST_HELPER_PROCESS") != "1" { return }
	for index, arg := range os.Args {
		if arg == "--" {
			os.Args = append([]string{os.Args[0]}, os.Args[index+1:]...)
			flag.CommandLine = flag.NewFlagSet(os.Args[0], flag.ExitOnError)
			main()
			os.Exit(0)
		}
	}
	t.Fatal("missing helper argument boundary")
}
