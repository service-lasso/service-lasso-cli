// service-lasso-confined-scaffold consumes the original bounded v3 command.
// Canonical template semantics belong to the original source SEA; this writer
// enforces structural confinement and observes every original output handle.
package main

import (
	"errors"
	"fmt"
	"os"
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
func componentSafe(value string) bool {
	return value != "" && value != "." && value != ".." && !strings.ContainsAny(value, "\\/")
}

// testGate is inert unless the test-only environment variable is present. It
// lets the integration suite prove held-handle confinement and failure safety.
// It is deliberately unavailable through the materialization input protocol.
func testGate(stage string) error {
	if !controlledFixtureBuild || !testGatesEnabled { return nil }
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
func main() {
	if controlledFixtureBuild && len(os.Args) == 2 && os.Args[1] == "--test-gate" {
		testGatesEnabled = true
	} else if len(os.Args) != 1 {
		fail("invalid arguments")
	}
	if err := runWriterV3(); err != nil {
		// This is intentionally a closed protocol response.  The TypeScript
		// caller may use the fixed code for tests and operator diagnostics, but
		// never forwards OS errors, paths, or private filesystem state.
		fmt.Printf("error\t%s\n", failureCode(err))
		fmt.Fprintln(os.Stderr, "confined writer failed")
		os.Exit(1)
	}
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
