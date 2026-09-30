# CLI capability matrix

This matrix binds issue [#1](https://github.com/service-lasso/service-lasso-cli/issues/1)
to the active `SPEC-CLI-FOUNDATION` slice. "Implemented" means covered by this
repository's focused tests, not a release or a claim that a remote runtime has
been qualified. See [Core compatibility](core-compatibility.md) for the
external-CLI/Core command boundary and the missing integration contracts.

## Framework decision boundary

Issue [#20](https://github.com/service-lasso/service-lasso-cli/issues/20) records the source-backed comparison of Commander, Cobra, Kong and clap. `ADR-001` retains TypeScript/Commander for the current slice. The choice does not complete any workflow below and native distribution remains deferred.

| Capability | Current evidence | Status / next owner |
| --- | --- | --- |
| Command tree, parsing and generated help | Commander 15, `src/index.ts`, and CLI tests exercise root/service help and confirmation failures. | Implemented foundation |
| Argument validation | Commander-required options plus application validation for origins, IDs and released-service references. | Implemented foundation |
| Stable JSON, stdout/stderr, exit codes | `--json` exists on current commands; application writes results to stdout and `CliError` diagnostics to stderr. The complete taxonomy and every required workflow test are incomplete. | Partial; Issue #1 automation contract |
| Shell completion | No completion command or completion artifact exists. | Planned; Issue #1 hardening/distribution |
| Portable Node archive | The release workflow packages `dist`, `package.json`, and README for a Node 22 consumer. | Candidate distribution only; not a standalone binary |
| Native Windows/Linux/macOS binary | No native executable assets or three-platform native validation exist. Node SEA needs a dedicated implementation/acceptance slice; Go/Rust migration needs a new decision issue. | Planned; Issue #1 hardening/distribution |
| Shared API-client opportunity with TUI | `CoreClient` is separate from parsing and accepts injected fetch. The authoritative sharing boundary is the versioned Core HTTP contract; a shared TypeScript package is possible only after TUI contract alignment. | Deferred architecture decision |

| Workflow | Status | Contract and boundary |
| --- | --- | --- |
| Configured Core origin and token | Implemented | `--core-url` > `SERVICE_LASSO_CORE_URL` > `--connection` / `SERVICE_LASSO_CONNECTION` / saved default > legacy saved config > local default. Saved connections contain only origins; `SERVICE_LASSO_CORE_TOKEN` is environment-only, never saved or printed, and may travel only over HTTPS or loopback HTTP. |
| Read Core health, identity and capabilities | Implemented | `GET /api/health`, `GET /api/runtime/instance`, and `GET /api/runtime/capabilities`; JSON-only response and safe HTTP-status errors. |
| List services | Implemented | `GET /api/services`; safe, read-only discovery. |
| Start, stop, restart | Implemented foundation | `POST /api/services/{id}/{action}` only after local `--confirm`; server-side permission and confirmation contracts remain authoritative. |
| Local authoring scaffold | Implemented foundation | Creates only a caller-selected absent directory; `--dry-run` has no writes. It records the reviewed `service-template` tag, commit and manifest SHA-256, and emits required lifecycle/dependency/health fields. Core registration is still required before runtime use. |
| Authentication and named connections | Partial | Named local and remote origin profiles are implemented. Released-service registration uses `SERVICE_LASSO_CLI_LOCAL_ADMIN_TOKEN` only as `x-service-lasso-admin-token`, requires HTTPS outside loopback, and never stores or prints it. Core identity-provider profile contracts remain required. |
| Authenticated operator read acceptance | Fixture-contract verified | The compiled CLI reads health, identity, capabilities, and services from an explicit loopback Core-shaped HTTP fixture using only `SERVICE_LASSO_CORE_TOKEN`. Success and rejected-credential paths prove stable, secret-safe CLI behavior. This does not prove a live supported Core, remote identity-provider authentication, or GA readiness. |
| Direct local Core read acceptance | Implemented for pinned Core `develop` | Issue #16 runs source-built compiled `service-lassoctl` against source-built Core `d9e2ae799244317940c862fe1261dfd22b7bdda1`, a temporary service registry and a loopback port selected by Core. It directly proves the local read path for `instance inspect --json` and `service list --json` with an enabled, autostart-eligible service while Core startup is suppressed. It has bounded startup and visible cleanup failures. This is distinct from checksum-bound CLI candidate distribution evidence; it creates no durable runtime, token, registration, install, start, stop, restart, release or GA evidence. |
| Released-service registration and readback | Implemented contract slice | `service register` sends only allowlisted repo/tag/full commit/manifest SHA-256/idempotency key plus server confirmation to Core `POST /api/runtime/actions/importService`; `service operation` reads the actor-scoped durable operation. Core owns allowlisting, provenance, duplicate/conflict semantics, permission, audit and durable state. |
| Validate, transfer and install | Planned | Caller-local paths, staged bytes, remote acquisition, install/setup and lifecycle are outside the released-service registration route; no shared-path assumption is made. |
| Durable operation wait/reconcile | Partial | The registration operation can be read by id after a timeout or disconnect. Bounded waits, cancellation and lifecycle operation reconciliation remain planned. |
| Remove, inbox/history and logs | Planned | Requires the owning Core API and data-access contracts. |
| Checksum-bound CLI candidate distribution | Implemented | A manual workflow dispatch from `develop` produces an exact-version Node 22 package archive, immutable candidate record, and SHA-256 manifest. The archive is smoke-tested by clean consumers on Windows, Linux, and macOS. |
| Shell completion and standalone binaries | Planned | Standalone native binaries remain deferred; the distribution candidate requires Node 22. |
| Core executable compatibility | Implemented foundation | This package installs `service-lassoctl`; Core retains `service-lasso` for its in-process, local-runtime CLI. |

## Safety properties in this slice

- Mutations require `--confirm` and are never prompted for.
- Result data uses stdout; diagnostic/error data uses stderr.
- HTTP response bodies are not copied into errors, logs, or test assertions.
- Scaffolding refuses an existing destination and does not contact Core.
- A client-side confirmation never substitutes for Core authentication,
  permissions, or server-issued confirmation requirements.
