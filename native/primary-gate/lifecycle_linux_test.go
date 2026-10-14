//go:build linux

package main

import (
    "net"
    "testing"
)

// Ordinary connection/transition regression only. No image/provider/capture,
// native retirement, source authority or memory qualification is inferred.
func TestOriginalClaimedChannelIsClosedOnTermination(t *testing.T) {
    original, peer := net.Pipe()
    defer peer.Close()
    lifecycle := &linuxLifecycle{claimed: true, connection: original}
    readFinished := make(chan error, 1)
    go func() { var byte [1]byte; _, err := original.Read(byte[:]); readFinished <- err }()
    lifecycle.terminate()
    if err := <-readFinished; err == nil { t.Fatal("original blocked read did not observe channel closure") }
    lifecycle.mu.Lock()
    if !lifecycle.terminated || !lifecycle.claimed || lifecycle.connection != original { t.Fatal("original one-use claim was discarded") }
    lifecycle.mu.Unlock()
    // Original close is idempotent at this connection layer. It must never
    // clear/reuse the claim or claim native writer/process retirement.
    lifecycle.terminate()
}