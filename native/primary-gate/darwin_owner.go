//go:build darwin

package main

import (
	"bufio"
	"crypto/rand"
	"crypto/sha256"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"net"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"

	"golang.org/x/sys/unix"
)

// This is the unprivileged client seam, not an owner broker or enrolment API.
// The separately admitted root peer independently proves run membership before
// coordinating the fixed one-object grant and replenishing the capability FD.
type darwinOwnerRequest struct {
	SchemaVersion int    `json:"schemaVersion"`
	Operation     string `json:"operation"`
	Mode          string `json:"mode"`
	Device        uint64 `json:"device"`
	Inode         uint64 `json:"inode"`
	Owner         uint32 `json:"owner"`
	Kind          string `json:"kind"`
	Digest        string `json:"digest"`
}
type darwinOwnerAck struct {
	SchemaVersion int    `json:"schemaVersion"`
	Operation     string `json:"operation"`
	Device        uint64 `json:"device"`
	Inode         uint64 `json:"inode"`
	Mode          string `json:"mode"`
	Status        string `json:"status"`
}
type darwinOwnerClient struct {
	mu sync.Mutex
	connection *net.UnixConn
	reader *bufio.Reader
}
var qualificationOwner *darwinOwnerClient

func connectDarwinOwner() (*darwinOwnerClient, error) {
	fd, err := strconv.Atoi(os.Getenv("SERVICE_LASSO_DARWIN_OWNER_CHANNEL_FD"))
	if err != nil || fd < 3 || fd > 64 { return nil, fmt.Errorf("owner channel unavailable") }
	capFD, err := strconv.Atoi(os.Getenv("SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD"))
	if err != nil || capFD == fd { return nil, fmt.Errorf("owner channel unavailable") }
	credentials, err := unix.GetsockoptXucred(fd, unix.SOL_LOCAL, unix.LOCAL_PEERCRED)
	if err != nil || credentials.Uid != 0 || credentials.Version != 0 { return nil, fmt.Errorf("owner peer unavailable") }
	kind, err := unix.GetsockoptInt(fd, unix.SOL_SOCKET, unix.SO_TYPE)
	if err != nil || kind != unix.SOCK_STREAM { return nil, fmt.Errorf("owner channel unavailable") }
	copyFD, err := unix.Dup(fd)
	if err != nil { return nil, fmt.Errorf("owner channel unavailable") }
	file := os.NewFile(uintptr(copyFD), "qualification-owner")
	defer file.Close()
	connection, err := net.FileConn(file)
	if err != nil { return nil, fmt.Errorf("owner channel unavailable") }
	peer, ok := connection.(*net.UnixConn)
	if !ok { connection.Close(); return nil, fmt.Errorf("owner channel unavailable") }
	return &darwinOwnerClient{connection: peer, reader: bufio.NewReaderSize(peer, 1024)}, nil
}

func ownerObjectRequest(fd int, mode string) (darwinOwnerRequest, error) {
	var request darwinOwnerRequest
	if mode != "set" && mode != "clear" { return request, fmt.Errorf("owner operation invalid") }
	var stat unix.Stat_t
	if unix.Fstat(fd, &stat) != nil || stat.Uid != uint32(os.Getuid()) { return request, fmt.Errorf("owner object invalid") }
	kind, digest := "", ""
	switch stat.Mode & unix.S_IFMT {
	case unix.S_IFREG:
		kind = "image"
		// Read only the finite embedded-image size; request digests are evidence
		// for independent owner validation, never self-authorising membership.
		if stat.Size != int64(len(seaBytes)) && stat.Size != int64(len(confinedWriterBytes)) { return request, fmt.Errorf("owner object invalid") }
		bytes := make([]byte, stat.Size)
		for offset := 0; offset < len(bytes); {
			n, err := unix.Pread(fd, bytes[offset:], int64(offset))
			if err != nil || n <= 0 { return request, fmt.Errorf("owner object unavailable") }
			offset += n
		}
		hash := sha256.Sum256(bytes)
		if hash != sha256.Sum256(seaBytes) && hash != sha256.Sum256(confinedWriterBytes) { return request, fmt.Errorf("owner image invalid") }
		digest = hex.EncodeToString(hash[:])
	case unix.S_IFDIR:
		kind = "parent"
	default: return request, fmt.Errorf("owner object invalid")
	}
	nonce := make([]byte, 32)
	if _, err := rand.Read(nonce); err != nil { return request, fmt.Errorf("owner operation unavailable") }
	return darwinOwnerRequest{1, hex.EncodeToString(nonce), mode, uint64(stat.Dev), stat.Ino, stat.Uid, kind, digest}, nil
}

func decodeOwnerAck(bytes []byte, request darwinOwnerRequest) error {
	var ack darwinOwnerAck
	decoder := json.NewDecoder(strings.NewReader(string(bytes)))
	decoder.DisallowUnknownFields()
	if decoder.Decode(&ack) != nil { return fmt.Errorf("owner acknowledgement invalid") }
	var trailing interface{}
	if decoder.Decode(&trailing) != io.EOF || ack.SchemaVersion != 1 || ack.Operation != request.Operation || ack.Device != request.Device || ack.Inode != request.Inode || ack.Mode != request.Mode || ack.Status != "ready" { return fmt.Errorf("owner acknowledgement invalid") }
	// Canonical bytes additionally reject duplicate keys or alternate encodings.
	canonical, _ := json.Marshal(ack)
	if string(bytes) != string(canonical) { return fmt.Errorf("owner acknowledgement invalid") }
	return nil
}

func (owner *darwinOwnerClient) begin(fd int, mode string) (darwinOwnerRequest, error) {
	request, err := ownerObjectRequest(fd, mode)
	if err != nil { return request, err }
	owner.mu.Lock()
	if err = owner.connection.SetDeadline(time.Now().Add(5 * time.Second)); err != nil { owner.mu.Unlock(); return request, err }
	bytes, _ := json.Marshal(request)
	bytes = append(bytes, '\n')
	n, _, err := owner.connection.WriteMsgUnix(bytes, unix.UnixRights(fd), nil)
	if err != nil || n != len(bytes) { owner.connection.Close(); owner.mu.Unlock(); return request, fmt.Errorf("owner request unavailable") }
	ack, err := owner.reader.ReadSlice('\n')
	if err != nil || len(ack) > 1024 || decodeOwnerAck(ack[:len(ack)-1], request) != nil { owner.connection.Close(); owner.mu.Unlock(); return request, fmt.Errorf("owner acknowledgement unavailable") }
	return request, nil // Keep the lease through actual helper and readback.
}

func (owner *darwinOwnerClient) complete(request darwinOwnerRequest, succeeded bool) error {
	defer owner.mu.Unlock()
	if err := owner.connection.SetDeadline(time.Now().Add(5 * time.Second)); err != nil { return err }
	status := "failed"
	if succeeded { status = "readback-passed" }
	// No capability, path, helper diagnostics or public success receipt.
	bytes, _ := json.Marshal(struct { SchemaVersion int `json:"schemaVersion"`; Operation string `json:"operation"`; Status string `json:"status"` }{1, request.Operation, status})
	bytes = append(bytes, '\n')
	n, err := owner.connection.Write(bytes)
	if err != nil || n != len(bytes) { return fmt.Errorf("owner completion unavailable") }
	return nil
}
