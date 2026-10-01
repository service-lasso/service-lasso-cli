package main

import (
	"bufio"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io"
	"net"
	"os"
	"os/exec"
)

type request struct {
	Version int    `json:"version"`
	Input   string `json:"input"`
}
type response struct {
	Version int    `json:"version"`
	Code    int    `json:"code"`
	Stdout  string `json:"stdout"`
	Stderr  string `json:"stderr"`
}

var heldHelper *os.File
var heldExecutionPath string

func materialize(connection net.Conn, helper string) {
	defer connection.Close()
	line, err := bufio.NewReader(io.LimitReader(connection, 8<<20)).ReadBytes('\n')
	if err != nil {
		return
	}
	var value request
	if json.Unmarshal(line, &value) != nil || value.Version != 1 || len(value.Input) > 6<<20 {
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
	encoded, _ := json.Marshal(response{1, code, base64.StdEncoding.EncodeToString(stdout.Bytes()), base64.StdEncoding.EncodeToString(stderr.Bytes())})
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
