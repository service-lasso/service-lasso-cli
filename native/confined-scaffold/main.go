// service-lasso-confined-scaffold is deliberately a small, independently
// compiled boundary for the accepted-template writer.  Its input is a line
// protocol (base64 destination, then path/mode/content records), so the
// JavaScript caller never asks it to interpret attacker supplied JSON.
package main

import (
	"bufio"
	"crypto/sha256"
	"encoding/base64"
	"errors"
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"
)

type entry struct {
	path  string
	mode  os.FileMode
	bytes []byte
}

var errDestinationExists = errors.New("confined destination exists")
var testGatesEnabled bool

func fail(format string, args ...any) { fmt.Fprintf(os.Stderr, format+"\n", args...); os.Exit(2) }
func decode(value string) []byte {
	b, err := base64.StdEncoding.DecodeString(value)
	if err != nil {
		fail("invalid protocol encoding")
	}
	return b
}
func componentSafe(value string) bool {
	return value != "" && value != "." && value != ".." && !strings.ContainsAny(value, "\\/")
}

// testGate is inert unless the test-only environment variable is present. It
// lets the integration suite prove held-handle confinement and failure safety.
// It is deliberately unavailable through the materialization input protocol.
func testGate(stage string) error {
	if !testGatesEnabled { return nil }
	if target := os.Getenv("SERVICE_LASSO_CONFINED_TEST_GATE_STAGE"); target != "" && target != stage {
		return nil
	}
	gate := os.Getenv("SERVICE_LASSO_CONFINED_TEST_GATE")
	if gate == "" {
		return nil
	}
	if err := os.WriteFile(gate+".ready", []byte(stage), 0600); err != nil {
		return err
	}
	deadline := time.Now().Add(10 * time.Second)
	for time.Now().Before(deadline) {
		if bytes, err := os.ReadFile(gate + ".continue"); err == nil && strings.Contains(string(bytes), stage) {
			if os.Getenv("SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE") == stage {
				return fmt.Errorf("test-induced write failure")
			}
			return nil
		}
		time.Sleep(5 * time.Millisecond)
	}
	return fmt.Errorf("test gate timed out")
}
func readPlan() (string, []entry, string) {
	s := bufio.NewScanner(os.Stdin)
	s.Buffer(make([]byte, 4096), 256*1024*1024)
	hash := sha256.New()
	scan := func() bool { if !s.Scan() { return false }; _, _ = hash.Write(append([]byte(s.Text()), '\n')); return true }
	if !scan() {
		fail("missing destination")
	}
	destination := string(decode(s.Text()))
	if !strings.HasPrefix(destination, string(os.PathSeparator)) && !(len(destination) > 2 && destination[1] == ':') {
		fail("destination is not absolute")
	}
	if !scan() {
		fail("missing entry count")
	}
	count, err := strconv.Atoi(s.Text())
	if err != nil || count < 0 || count > 4096 {
		fail("invalid entry count")
	}
	out := make([]entry, 0, count)
	seen := map[string]bool{}
	for range count {
		if !scan() {
			fail("truncated entry")
		}
		fields := strings.Split(s.Text(), "\t")
		if len(fields) != 3 {
			fail("invalid entry")
		}
		parts := strings.Split(fields[0], "/")
		if len(parts) == 0 {
			fail("invalid path")
		}
		for _, part := range parts {
			if !componentSafe(part) {
				fail("unsafe path")
			}
		}
		if seen[fields[0]] {
			fail("duplicate path")
		}
		seen[fields[0]] = true
		mode, e := strconv.ParseUint(fields[1], 8, 32)
		if e != nil || (mode != 0644 && mode != 0755) {
			fail("invalid mode")
		}
		out = append(out, entry{fields[0], os.FileMode(mode), decode(fields[2])})
	}
	if s.Scan() || s.Err() != nil {
		fail("trailing protocol data")
	}
	return destination, out, fmt.Sprintf("%x", hash.Sum(nil))
}
func main() {
	if len(os.Args) == 2 && os.Args[1] == "--self-test" {
		fmt.Println("ok")
		return
	}
	if len(os.Args) == 2 && os.Args[1] == "--test-gate" {
		testGatesEnabled = true
	} else if len(os.Args) != 1 {
		fail("invalid arguments")
	}
	destination, entries, receipt := readPlan()
	if err := materialize(destination, entries); err != nil {
		// This is intentionally a closed protocol response.  The TypeScript
		// caller may use the fixed code for tests and operator diagnostics, but
		// never forwards OS errors, paths, or private filesystem state.
		fmt.Printf("error\t%s\n", failureCode(err))
		fmt.Fprintln(os.Stderr, "confined writer failed")
		os.Exit(1)
	}
	fmt.Printf("ok\t%s\n", receipt)
}

func failureCode(err error) string {
	switch {
	case errors.Is(err, os.ErrExist), errors.Is(err, errDestinationExists):
		return "destination_exists"
	case errors.Is(err, os.ErrNotExist):
		return "parent_missing"
	case errors.Is(err, os.ErrPermission):
		return "permission_denied"
	default:
		return "write_rejected"
	}
}
