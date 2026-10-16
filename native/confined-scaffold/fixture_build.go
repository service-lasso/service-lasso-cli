//go:build service_lasso_controlled_fixture

package main

// This is a separately compiled, separately admitted fixture image. No ordinary
// runtime argument or environment value can change the production build class.
const controlledFixtureBuild=true
