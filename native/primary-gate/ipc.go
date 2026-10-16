package main

import (
 "crypto/rand"
 "encoding/base64"
 "fmt"
 "os"
 "io"
 "strings"
)

// The capability is original launch authority, never catalogue authority.
var gateCapability [32]byte
func newGateCapability()bool{_,e:=rand.Read(gateCapability[:]);return e==nil}

func gateEnvironment(parent []string, platform, pipe string, fd int) []string {
	result := make([]string, 0, len(parent)+4)
	for _, item := range parent {
		if strings.HasPrefix(item, "SERVICE_LASSO_PRIMARY_GATE=") || strings.HasPrefix(item, "SERVICE_LASSO_PRIMARY_GATE_PIPE=") || strings.HasPrefix(item, "SERVICE_LASSO_PRIMARY_GATE_FD=") || strings.HasPrefix(item, "SERVICE_LASSO_PRIMARY_GATE_CAPABILITY=") { continue }
		result = append(result, item)
	}
	result = append(result, "SERVICE_LASSO_PRIMARY_GATE="+platform, "SERVICE_LASSO_PRIMARY_GATE_CAPABILITY="+base64.StdEncoding.EncodeToString(gateCapability[:]))
	if pipe != "" { result = append(result, "SERVICE_LASSO_PRIMARY_GATE_PIPE="+pipe) }
	if fd > 0 { result = append(result, fmt.Sprintf("SERVICE_LASSO_PRIMARY_GATE_FD=%d", fd)) }
	return result
}


var heldHelper *os.File
var heldExecutionPath string

type byteReader struct{value []byte}
func bytesReader(value []byte)*byteReader{return &byteReader{value:value}}
func(r *byteReader)Read(target []byte)(int,error){if len(r.value)==0{return 0,io.EOF};n:=copy(target,r.value);r.value=r.value[n:];return n,nil}
