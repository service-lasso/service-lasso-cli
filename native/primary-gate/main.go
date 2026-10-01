package main

import _ "embed"

// The packager copies the exact target-host SEA and confined writer bytes into
// assets before this primary is compiled.  Keeping the assets in the primary
// image is intentional: a distribution never starts a mutable sibling binary
// as its authority for service authoring.
//
//go:embed assets/service-lassoctl.sea
var seaBytes []byte

//go:embed assets/service-lasso-confined-scaffold
var confinedWriterBytes []byte
