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

Issue #6 introduces manually dispatched `develop` candidates rather than an
automatic published release. Each candidate is a versioned Node 22 package
archive with a `candidate.json` record and `SHA256SUMS.txt`; its source commit,
candidate tag, package name, command, supported platforms, archive name and
SHA-256 are bound together. After all three clean-consumer smoke jobs pass, the
manual dispatch publishes those exact files as a clearly labelled GitHub
prerelease for Core review. It never publishes npm or claims Core qualification.

There is no earlier external executable name to migrate. The package provides
no `service-lasso` alias because an alias would recreate the installation-order
collision. Scripts must invoke `service-lassoctl` explicitly. Future releases
must retain Core's command ownership and use a versioned deprecation plan if an
external command name ever changes.

## Current contract evidence

| CLI adapter | Repository evidence | Integration qualification |
| --- | --- | --- |
| `GET /api/health` | Source-built Core `develop` at `d9e2ae799244317940c862fe1261dfd22b7bdda1`; dependency-injected client test | No packaged-Core or live-runtime qualification. |
| `GET /api/services` | Source-built Core route; dependency-injected client test | No live Core response-schema or permission evidence. |
| `GET /api/runtime/instance` and `GET /api/runtime/capabilities` | Source-built Core routes; dependency-injected client test | No packaged-Core or remote-auth qualification. |
| `POST /api/services/{id}/{action}` | Core source route; unit transport test; local `--confirm` gate | No durable-operation or idempotency evidence. |
| `POST /api/runtime/actions/importService` and `GET /api/operator/operations/{operationId}` | Core `develop` `387726b` / merged #1464; Issue #18 focused CLI transport fixture checks | `service register` accepts only release references and has no caller-path, bytes, transfer, install or lifecycle behavior. Packaged external-CLI and hosted exact-head qualification remain open. |
| `SERVICE_LASSO_CORE_TOKEN` Bearer credential | Core `develop` source accepts the local-admin secret from `Authorization: Bearer`; this client keeps it environment-only and sends it only to HTTPS origins or loopback HTTP. | Remote token authentication depends on Core policy: it is unavailable when Core enforces SSO. Named profiles and non-local-admin identity-provider contracts remain unimplemented. |
| Named origin profiles and authoring scaffold | Local configuration tests plus future accepted-bundle fixture verification of full inventory/modes, provenance and checksum tuple | The CLI materializes only a caller-supplied accepted immutable bundle. Current template `develop` is `1.0.0-dev` at `bdeb24b84f97e702372ccbcba0794ce30888ad53` and lacks a published accepted candidate/catalog identity, so default authoring fails closed and direct generated-project/Core qualification remains blocked. Profiles do not supply an identity-provider credential flow. |
| Durable lifecycle operations | CI `36782380749`, tested CLI implementation `1458c5167fd5c525fb85744f68002cfec762b011`, Ubuntu direct source-built Core at merged `d6dc5558307f13c654194ddc944e3be40c940675` (tree-equal to reviewed `454d1590698a36194847755a4aabc4a59d6c5ec4`) | Four direct-Core tests passed: durable journey and pin, plus read journey and pin. The Windows local 10-second attempt is incomplete with receipt and cleanup unobserved. This is not fixture-only proof, packaged-Core, release, deployment, or GA qualification. |

Core `develop` `387726b` now accepts released-service registration at
`POST /api/runtime/actions/importService` and exposes actor-scoped durable
readback at `GET /api/operator/operations/{operationId}`. This CLI consumes
only that release-reference contract with the environment-only
`SERVICE_LASSO_CLI_LOCAL_ADMIN_TOKEN` header. It deliberately has no caller
filesystem, staged-byte, transfer, install or lifecycle workflow.

Issue #16 is source-built CLI-to-source-built Core evidence only: it starts the
pinned Core source in a disposable loopback runtime and uses the compiled CLI
from this source tree. The checksum-bound CLI candidate is separate distribution
evidence; neither candidate archive nor a packaged Core is consumed by this test.

Issue #22 has separate source-built Core evidence at merged `develop` revision
`d6dc5558307f13c654194ddc944e3be40c940675`, whose tree equals reviewed
`454d1590698a36194847755a4aabc4a59d6c5ec4`. The terminal Ubuntu job in CI
`36782380749` ran tested implementation `1458c5167fd5c525fb85744f68002cfec762b011`
against an owned disposable guarded Core
with a scoped signed JWT, actor/client identity and lifecycle scopes. It passed
the durable journey and the distinct real-Core read journey. The CLI sends only
fixed lifecycle fields and emits allowlisted records; the confirmation phrase is
displayed only because Core requires its re-entry. This proves neither a
packaged Core nor release, deployment, cleanup, or GA authority. The failed-to-
finish Windows 10-second local attempt has no terminal receipt or cleanup proof,
so both remain unobserved.

A later documentation-only commit may record this evidence binding, but it is
not the tested implementation and has no claimed CI result until a separate
natural CI terminal record exists.

## Core package handoff

[Core issue #1461](https://github.com/service-lasso/service-lasso/issues/1461)
owns integration and release qualification. Its current Core package contract
is `@service-lasso/service-lasso` with bin `service-lasso` mapped to `cli.js`.
The Core staging script records that package as
`service-lasso-package-<version>` and verifies that `cli.js` reports the
staged package version.

This external package's candidate archive identity is supplied by its immutable
candidate record; its package contract is `@service-lasso/cli`, bin
`service-lassoctl`, entrypoint `dist/index.js`, and Node 22 runtime requirement.
Core must add it only through a versioned operator-tools manifest containing
the exact source tag, commit, asset digest, installed path, command name and
supported platform. Core must reject mutable latest downloads, wrong
identities, malformed manifests, path traversal and checksum mismatches.

### Core #1461 handoff record

Core packaging must consume a reviewed candidate by reading `candidate.json`
and `SHA256SUMS.txt`, then verifying all of the following before staging:

| Field | Required value |
| --- | --- |
| Source repository | `service-lasso/service-lasso-cli` |
| Candidate source | Exact 40-character `develop` commit recorded in `candidate.json` |
| Candidate tag | `cli-v<package-version>-candidate-<short-sha>` recorded in `candidate.json` |
| Archive | `service-lassoctl-<candidate-version>.tgz`, named in both manifests |
| SHA-256 | Exact archive digest in `candidate.json` and `SHA256SUMS.txt` |
| Command | `service-lassoctl` only; reject `service-lasso` and preserve Core's bin |
| Runtime | Node 22 or newer; no standalone binary is claimed |
| Platforms | `win32`, `linux`, and `darwin` with the documented Node 22 requirement |

The candidate workflow smoke proves a fresh client package can make safe
health/identity reads against a local fixture. It is surrogate-only evidence
for Core #1461: Core still needs its own exact packaged-instance qualification,
operator-tools manifest, and independent review.

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
| Noninteractive versioned-template project creation | Partial | Local scaffold is deterministic, safe, and pins the reviewed template tag/commit/checksum; direct Core registration validation still requires #1463. |
| Preserve existing files and reject invalid inputs | Implemented foundation | Local scaffold tests cover absence and invalid IDs; no runtime mutation occurs. |
| Add released service through supported API | Partial | Issue #18 consumes the approved release-reference registration route. Full generated/existing local-service input remains outside the contract. |
| Remote client-local input transfer | Blocked | No acquisition/upload contract exists. |
| Registration, install, setup, start and health independently composable | Partial | Health, list and start/stop/restart adapters exist; registration/install/setup contract remains unimplemented. |
| Duplicate identity and retry safety | Partial | Core's registration idempotency, conflict result and actor-scoped operation readback are exposed by Issue #18. Lifecycle and full workflow retry semantics remain open. |
| Authentication, permission and confirmation | Partial | Registration uses the environment-only `x-service-lasso-admin-token` contract; the existing general Core adapter uses an environment-only Bearer token. Identity-provider client flow remains absent. |
| JSON, stdout/stderr, exits and no-prompt behavior | Implemented foundation | Local focused tests; no packaged consumer evidence. |
| Durable IDs, bounded wait and reconciliation | Blocked | No agreed lifecycle durable-operation API. |
| Removal and retained-data effects | Blocked | No agreed public deregistration/removal contract. |
| End-to-end real-instance workflow | Blocked | Needs the missing registration/transfer contract and a supported packaged Core fixture. |
| Windows, Linux and macOS release validation | Blocked | Requires release assets and Core #1461 operator-tool packaging qualification. |
| Installation, auth, completion and recovery documentation | Partial | Foundation usage and boundaries documented; distribution, completion and recovery await the remaining contracts. |
