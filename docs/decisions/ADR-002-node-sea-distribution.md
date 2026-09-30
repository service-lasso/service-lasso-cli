# ADR-002: Build standalone CLI binaries with Node SEA

**Status:** Accepted for Issue #26 on 2026-10-01. This is a bounded binary
distribution decision, not release, publication, deployment, Core
qualification, or GA approval.

## Context

The current `service-lassoctl` archive contains JavaScript and requires a Node
22 consumer. It does not meet Issue #1's standalone Windows, Linux and macOS
binary acceptance. ADR-001 retained TypeScript and Commander and required a
focused issue before committing to a native-binary approach. Issue #26 provides
that issue and is mapped to the `service-lasso Delivery` project.

## Decision

Keep the TypeScript/Commander application and package it as Node 22.23.2 Single
Executable Applications (SEA). The build compiles TypeScript, bundles the ESM
entry into one CommonJS script using `esbuild` 0.28.2, creates the preparation
blob with Node 22.23.2, then injects it into the matching Node 22.23.2 host
executable using `postject` 1.0.0-alpha.6.

Node v22.23.2 documents that SEA runs one embedded CommonJS script, requires
the same Node version for blob generation and injection target, and documents
the `postject` resource name, sentinel fuse, and macOS segment requirements.
It also states that SEA is regularly tested on Windows, macOS and Linux. The
implementation therefore builds and executes each target on its matching host;
cross-built assets are not execution proof.

Official evidence: [Node v22.23.2 SEA documentation](https://nodejs.org/download/release/v22.23.2/docs/api/single-executable-applications.html).

## Consequences and acceptance

- Emit `service-lassoctl.exe` for Windows x64 and `service-lassoctl` for Linux
  x64 and macOS arm64, each with source commit and SHA-256 provenance.
- Direct execution must run help, version, JSON read, safe invalid input and
  credential-sentinel non-leak with Node removed from `PATH`.
- The local Windows proof is useful direct evidence only for Windows. Hosted
  Linux and macOS target-host jobs remain pending until their natural CI run.
- No signing is claimed. The existing Node archive, Core integration, release
  candidate, publishing and deployment paths remain unchanged.
