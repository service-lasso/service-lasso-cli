package main

import (
	"bufio"
	"crypto/ed25519"
	"crypto/rand"
	"encoding/base64"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"io"
	"net"
	"os"
	"os/exec"
)

type request struct {
	Version int    `json:"version"`
	Nonce   string `json:"nonce"`
	Input   string `json:"input"`
}
type response struct {
	Version int    `json:"version"`
	Nonce   string `json:"nonce"`
	Code    int    `json:"code"`
	Stdout  string `json:"stdout"`
	Stderr  string `json:"stderr"`
}

// gateSigningSeed is generated for each packaged gate and exists only in its
// transient build directory. The SEA pins the paired public key, so a
// launcher-provided pipe name can route a request but cannot forge a primary
// response. The development definition is deliberately excluded from package
// assembly.

func gateGreeting(connection net.Conn) (string, bool) {
	nonceBytes := make([]byte, 32)
	if _, err := rand.Read(nonceBytes); err != nil { return "", false }
	nonce := hex.EncodeToString(nonceBytes)
	signature := ed25519.Sign(ed25519.NewKeyFromSeed(gateSigningSeed[:]), []byte("service-lasso-primary-v1:"+nonce))
	value, err := json.Marshal(struct { Version int `json:"version"`; Nonce string `json:"nonce"`; Signature string `json:"signature"` }{1, nonce, base64.StdEncoding.EncodeToString(signature)})
	if err != nil { return "", false }
	_, err = connection.Write(append(value, '\n'))
	return nonce, err == nil
}

var heldHelper *os.File
var heldExecutionPath string

func materialize(connection net.Conn, helper string) {
	defer connection.Close()
	nonce, ok := gateGreeting(connection)
	if !ok { return }
	line, err := bufio.NewReader(io.LimitReader(connection, 8<<20)).ReadBytes('\n')
	if err != nil {
		return
	}
	var value request
	decoder := json.NewDecoder(bytesReader(line))
	decoder.DisallowUnknownFields()
	if decoder.Decode(&value) != nil || decoder.Decode(&struct{}{}) != io.EOF || value.Version != 1 || value.Nonce != nonce || len(value.Input) == 0 || len(value.Input) > 6<<20 {
		return
	}
	input, err := base64.StdEncoding.DecodeString(value.Input)
	if err != nil {
		return
	}
	child := exec.Command(helper)
	if heldHelper != nil {
		child = exec.Command(heldExecutionPath)
		child.ExtraFiles = []*os.File{heldHelper}
	}
	child.Stdin = bytesReader(input)
	var stdout, stderr limitedBuffer
	child.Stdout, child.Stderr = &stdout, &stderr
	err = child.Run()
	code := 0
	if failure, ok := err.(*exec.ExitError); ok {
		code = failure.ExitCode()
	} else if err != nil {
		return
	}
	encoded, _ := json.Marshal(response{1, nonce, code, base64.StdEncoding.EncodeToString(stdout.Bytes()), base64.StdEncoding.EncodeToString(stderr.Bytes())})
	_, _ = connection.Write(append(encoded, '\n'))
}

type limitedBuffer struct{ value []byte }

func (b *limitedBuffer) Write(value []byte) (int, error) {
	if len(b.value)+len(value) > 1<<20 {
		return 0, fmt.Errorf("protocol output too large")
	}
	b.value = append(b.value, value...)
	return len(value), nil
}
func (b *limitedBuffer) Bytes() []byte { return b.value }

type byteReader struct{ value []byte }

func bytesReader(value []byte) *byteReader { return &byteReader{value} }
func (r *byteReader) Read(target []byte) (int, error) {
	if len(r.value) == 0 {
		return 0, io.EOF
	}
	count := copy(target, r.value)
	r.value = r.value[count:]
	return count, nil
}
