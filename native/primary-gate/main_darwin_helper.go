//go:build darwin

package main

import _ "embed"

//go:embed assets/service-lasso-darwin-immutable-helper.sha256
var darwinHelperSHA256 string
