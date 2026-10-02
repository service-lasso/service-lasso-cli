//go:build darwin

package main

import (
	"encoding/json"
	"net"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"testing"
	"time"

	"golang.org/x/sys/unix"
)

func TestOwnerAckRejectsWrongObjectOperationAndNonClosedBytes(t *testing.T) {
	request := darwinOwnerRequest{1, "operation", "set", 7, 9, 501, "image", "digest"}
	ack := darwinOwnerAck{1, request.Operation, request.Device, request.Inode, request.Mode, "ready"}
	bytes, _ := json.Marshal(ack)
	if decodeOwnerAck(bytes, request) != nil { t.Fatal("closed exact acknowledgement rejected") }
	for _, mutation := range []darwinOwnerAck{{1, "wrong", 7, 9, "set", "ready"}, {1, "operation", 8, 9, "set", "ready"}, {1, "operation", 7, 10, "set", "ready"}, {1, "operation", 7, 9, "clear", "ready"}, {1, "operation", 7, 9, "set", "denied"}} {
		bytes, _ := json.Marshal(mutation)
		if decodeOwnerAck(bytes, request) == nil { t.Fatal("contradictory owner acknowledgement accepted") }
	}
	for _, malformed := range []string{string(bytes) + "{}", `{"schemaVersion":1,"schemaVersion":1,"operation":"operation","device":7,"inode":9,"mode":"set","status":"ready"}`, `{"schemaVersion":1,"operation":"operation","device":7,"inode":9,"mode":"set","status":"ready","extra":true}`} {
		if decodeOwnerAck([]byte(malformed), request) == nil { t.Fatal("nonclosed acknowledgement accepted") }
	}
}

func TestDarwinOwnerInheritedChannelBindsActualHeldObjectAndCompletion(t *testing.T) {
	// This positive seam test is part of separately admitted root-owner source
	// qualification; ordinary unprivileged runs must fail, never skip it.
	if os.Geteuid() != 0 { t.Fatal("actual root-owner seam admission is required") }
	pair, err := unix.Socketpair(unix.AF_UNIX, unix.SOCK_STREAM, 0)
	if err != nil { t.Fatal(err) }
	defer unix.Close(pair[0]); defer unix.Close(pair[1])
	t.Setenv("SERVICE_LASSO_DARWIN_OWNER_CHANNEL_FD", strconv.Itoa(pair[0]))
	capability, err := os.Open(os.DevNull)
	if err != nil { t.Fatal(err) }
	defer capability.Close()
	t.Setenv("SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD", strconv.Itoa(int(capability.Fd())))
	client, err := connectDarwinOwner()
	if err != nil { t.Fatal(err) }
	defer client.connection.Close()
	peerFD, err := unix.Dup(pair[1])
	if err != nil { t.Fatal(err) }
	peerFile := os.NewFile(uintptr(peerFD), "owner-test-peer")
	defer peerFile.Close()
	peerConnection, err := net.FileConn(peerFile)
	if err != nil { t.Fatal(err) }
	peer := peerConnection.(*net.UnixConn)
	defer peer.Close()
	_ = peer.SetDeadline(time.Now().Add(5 * time.Second))
	image, err := os.CreateTemp(t.TempDir(), "held-image")
	if err != nil { t.Fatal(err) }
	defer image.Close()
	if _, err = image.Write(seaBytes); err != nil { t.Fatal(err) }
	if err = image.Sync(); err != nil { t.Fatal(err) }
	done := make(chan error, 1)
	go func() {
		request, err := client.begin(int(image.Fd()), "set")
		if err == nil { err = client.complete(request, true) }
		done <- err
	}()
	bytes, rights := make([]byte, 1024), make([]byte, unix.CmsgSpace(4))
	n, oobn, flags, _, err := peer.ReadMsgUnix(bytes, rights)
	if err != nil || flags & (unix.MSG_TRUNC|unix.MSG_CTRUNC) != 0 { t.Fatal("actual descriptor request unavailable", err) }
	var request darwinOwnerRequest
	if json.Unmarshal(bytes[:n], &request) != nil || request.Mode != "set" || request.Kind != "image" { t.Fatal("request malformed") }
	messages, err := unix.ParseSocketControlMessage(rights[:oobn])
	if err != nil || len(messages) != 1 { t.Fatal("held object rights unavailable") }
	fds, err := unix.ParseUnixRights(&messages[0])
	if err != nil || len(fds) != 1 { t.Fatal("held descriptor missing") }
	defer unix.Close(fds[0])
	var stat unix.Stat_t
	if unix.Fstat(fds[0], &stat) != nil || uint64(stat.Dev) != request.Device || stat.Ino != request.Inode || stat.Uid != request.Owner { t.Fatal("request does not bind actual held object") }
	ack, _ := json.Marshal(darwinOwnerAck{1, request.Operation, request.Device, request.Inode, "set", "ready"})
	if _, err = peer.Write(append(ack, '\n')); err != nil { t.Fatal(err) }
	n, err = peer.Read(bytes)
	if err != nil { t.Fatal(err) }
	var completion struct { SchemaVersion int `json:"schemaVersion"`; Operation string `json:"operation"`; Status string `json:"status"` }
	if json.Unmarshal(bytes[:n], &completion) != nil || completion.Operation != request.Operation || completion.Status != "readback-passed" { t.Fatal("completion not bound") }
	if err = <-done; err != nil { t.Fatal(err) }
}

func TestDarwinOwnerChannelRejectsAbsentInvalidAndSharedDescriptors(t *testing.T) {
	t.Setenv("SERVICE_LASSO_DARWIN_OWNER_CHANNEL_FD", "")
	t.Setenv("SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD", "3")
	if _, err := connectDarwinOwner(); err == nil { t.Fatal("absent owner channel accepted") }
	file, err := os.Open(os.DevNull)
	if err != nil { t.Fatal(err) }
	defer file.Close()
	t.Setenv("SERVICE_LASSO_DARWIN_OWNER_CHANNEL_FD", strconv.Itoa(int(file.Fd())))
	t.Setenv("SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD", strconv.Itoa(int(file.Fd()) + 1))
	if _, err := connectDarwinOwner(); err == nil { t.Fatal("non-socket owner channel accepted") }
	t.Setenv("SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD", strconv.Itoa(int(file.Fd())))
	if _, err := connectDarwinOwner(); err == nil { t.Fatal("shared owner/capability descriptor accepted") }
}

func TestDarwinNodeCallerActuallyMapsBothInheritedOwnerDescriptors(t *testing.T) {
	node := os.Getenv("SERVICE_LASSO_QUALIFICATION_NODE")
	if node == "" || !filepath.IsAbs(node) { t.Fatal("exact admitted physical Node is required") }
	capRead, capWrite, err := os.Pipe()
	if err != nil { t.Fatal(err) }
	defer capRead.Close(); defer capWrite.Close()
	if _, err = capWrite.Write(make([]byte, 32)); err != nil { t.Fatal(err) }
	pair, err := unix.Socketpair(unix.AF_UNIX, unix.SOCK_STREAM, 0)
	if err != nil { t.Fatal(err) }
	defer unix.Close(pair[0]); defer unix.Close(pair[1])
	channelFD, err := unix.Dup(pair[0])
	if err != nil { t.Fatal(err) }
	channel := os.NewFile(uintptr(channelFD), "transport-test-channel")
	defer channel.Close()
	// The child checks actual FD3/FD4, consumes the finite32byte pipe, and
	// returns its own natural exit through the real JS transport adapter.
	code := `import { spawnSync } from "node:child_process"; import { nativeQualificationStdio } from "./scripts/native-qualification-stdio.mjs"; const child = spawnSync(process.execPath, ["-e", "const fs=require('node:fs'); const b=Buffer.alloc(32); if(fs.readSync(3,b,0,32,null)!==32 || !fs.fstatSync(4).isSocket()) process.exit(7); process.stdout.write('held-descriptors');"], { encoding:"utf8", stdio:nativeQualificationStdio(["ignore","pipe","pipe"]) }); if(child.status!==0 || child.stdout!=='held-descriptors') process.exit(8); process.stdout.write(child.stdout);`
	command := exec.Command(node, "--input-type=module", "-e", code)
	command.Dir, err = filepath.Abs("../..")
	if err != nil { t.Fatal(err) }
	command.ExtraFiles = []*os.File{capRead, channel}
	command.Env = append(os.Environ(), "SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD=3", "SERVICE_LASSO_DARWIN_OWNER_CHANNEL_FD=4")
	bytes, err := command.Output()
	if err != nil || string(bytes) != "held-descriptors" { t.Fatal("actual Node descriptor transport failed", err) }
}
