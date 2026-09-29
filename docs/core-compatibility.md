# Core compatibility contract

## Command ownership

Service Lasso Core already publishes an in-process CLI named `service-lasso`.
Installing this package under the same executable name would make npm's bin
resolution depend on installation order and could invoke the wrong operator
surface. This external package therefore installs `service-lassoctl`. The
`ctl` suffix describes a remote-control client and remains distinct from Core's
host-local command.

`service-lasso` remains Core's local-runtime command. `service-lassoctl` owns
caller-local authoring and future remote, HTTP-contract-based control. Neither
tool replaces the other, and this package does not import Core internals.

## Migration and aliases

There is no published external binary to preserve: this repository's baseline
has not released a package. The package provides no `service-lasso` alias,
because an alias would recreate the installation-order collision. Scripts must
invoke `service-lassoctl` explicitly. A future migration from a released name
requires a versioned deprecation plan and must retain Core's command ownership.

## Current contract evidence

| CLI adapter | Repository evidence | Integration qualification |
| --- | --- | --- |
| `GET /api/health` | Core `develop` source at `02785268392318f14af3d0596df1ca5414957ce8`; dependency-injected client test | No packaged-Core or live-runtime qualification. |
| `GET /api/services` | Core source route; dependency-injected client test | No live Core response-schema or permission evidence. |
| `GET /api/runtime/instance` and `GET /api/runtime/capabilities` | Core source routes; dependency-injected client test | No packaged-Core or remote-auth qualification. |
| `POST /api/services/{id}/{action}` | Core source route; unit transport test; local `--confirm` gate | No durable-operation or idempotency evidence. |
| `SERVICE_LASSO_CORE_TOKEN` Bearer credential | Core accepts a local-admin token from the Bearer header; token is environment-only in this client | Named profiles and non-local-admin identity-provider contracts remain unimplemented. |
| Authoring scaffold | Local filesystem tests | Starter manifest is not a Core registration contract. |

Core exposes `GET /api/runtime/actions/importService/plan`, but the current
`POST /api/runtime/actions/*` implementation does not accept `importService`.
The plan route neither transfers a caller-local artifact nor performs
registration. This CLI therefore deliberately has no `service register` or
`service install` command.

## Core package handoff

[Core issue #1461](https://github.com/service-lasso/service-lasso/issues/1461)
owns integration and release qualification. Its current Core package contract
is `@service-lasso/service-lasso` with bin `service-lasso` mapped to `cli.js`.
The Core staging script records that package as
`service-lasso-package-<version>` and verifies that `cli.js` reports the
staged package version.

This external package has no released identity yet. Its future package
contract is `@service-lasso/cli`, bin `service-lassoctl`, entrypoint
`dist/index.js`. Core must add it only through a versioned operator-tools
manifest containing the exact source tag, commit, asset digest, installed path,
command name and supported platform. Core must reject mutable latest downloads,
wrong identities, malformed manifests, path traversal and checksum mismatches.

## Required Core-owned contract before workflow expansion

Core must publish versioned public contracts for:

1. authentication and named connection profiles, including CI-safe credential
   injection and TLS verification;
2. runtime identity, API version and capability discovery;
3. service validation, registration/import and duplicate identity handling;
4. remote artifact transfer or acquisition, with no shared-filesystem
   assumption;
5. install, configuration and lifecycle requests with server-side confirmation,
   permission and idempotency semantics; and
6. durable operation identifiers, result/status lookup, bounded waiting and
   cancellation.

Until those contracts exist, this repository must not add registration, install,
authentication, operation waiting or remote-transfer commands. A Core contract
release should be consumed through an explicit compatibility issue and an
integration test against a supported running instance.

## Issue #1 acceptance inventory

| Issue #1 criterion | Status in this repository | Core dependency or evidence gap |
| --- | --- | --- |
| Framework evaluation | Implemented in ADR-001 | None for the foundation decision. |
| Noninteractive versioned-template project creation | Partial | Local starter is deterministic and safe, but no versioned service-template contract is consumed. |
| Preserve existing files and reject invalid inputs | Implemented foundation | Local scaffold tests cover absence and invalid IDs; no runtime mutation occurs. |
| Add service through supported API | Blocked | No Core HTTP registration/import mutation exists. |
| Remote client-local input transfer | Blocked | No acquisition/upload contract exists. |
| Registration, install, setup, start and health independently composable | Partial | Health, list and start/stop/restart adapters exist; registration/install/setup contract remains unimplemented. |
| Duplicate identity and retry safety | Blocked | Requires registration conflict, idempotency and reconciliation contract. |
| Authentication, permission and confirmation | Partial | Environment-only local-admin Bearer token plus Core server permission/confirmation route exist; named profiles and identity-provider client flow are absent. |
| JSON, stdout/stderr, exits and no-prompt behavior | Implemented foundation | Local focused tests; no packaged consumer evidence. |
| Durable IDs, bounded wait and reconciliation | Blocked | No agreed lifecycle durable-operation API. |
| Removal and retained-data effects | Blocked | No agreed public deregistration/removal contract. |
| End-to-end real-instance workflow | Blocked | Needs the missing registration/transfer contract and a supported packaged Core fixture. |
| Windows, Linux and macOS release validation | Blocked | Requires release assets and Core #1461 operator-tool packaging qualification. |
| Installation, auth, completion and recovery documentation | Partial | Foundation usage and boundaries documented; distribution, completion and recovery await the remaining contracts. |
