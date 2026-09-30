# CLI foundation and Core compatibility

## Scope

Establish a Node.js TypeScript CLI with deterministic help, local configuration,
service-package scaffolding, and safe Core API read/mutation commands. This is
the active foundation slice of Issue #1, not a claim that the whole roadmap epic
is complete.

## Requirement groups

| Group | Foundation requirement | Current boundary |
| --- | --- | --- |
| CLI-CONNECTION | Select a Core origin using flag, environment, named saved configuration and a local default; when a local-admin token is configured, send it only to HTTPS or loopback HTTP, and reject redirects before a request can cross origins. | Named origin profiles are implemented locally with flag > environment > profile/default precedence. Credentials remain environment-only; Core identity-provider profiles await the Core contract. |
| CLI-AUTHORING | Create a non-destructive, caller-selected package starter from a pinned service-template identity. | Implemented against the reviewed `2026.5.8-d2241fe` template tag, commit and canonical manifest SHA-256; Core registration remains blocked on #1463. |
| CLI-REGISTRATION | Validate and register local or remote package input. | Blocked on Core registration/import, remote acquisition and duplicate-identity contracts. |
| CLI-OPERATIONS | Read status and services; invoke supported lifecycle actions with local confirmation. | Read and lifecycle routes are unit-tested transport adapters only until Core publishes the exact API/version/permission contract. |
| CLI-REAL-CORE-ACCEPTANCE | Exercise the compiled CLI against an explicitly selected authenticated Core-shaped HTTP endpoint using only public read routes, and classify fixture proof separately from live-Core proof. | Issue #14 adds a loopback fixture contract for `instance inspect` and `service list`. The fixture proves environment-only Bearer transport and secret-safe failures, but no live Core credential or supported runtime is available in this repository. |
| CLI-REAL-CORE-LOCAL-READ | Exercise the compiled CLI against a pinned actual Core runtime using only loopback public read routes. | Issue #16 starts a disposable Core at port `0` with temporary workspace and service roots, then verifies `instance inspect --json` and `service list --json`. It is direct local read proof only: no service is started, registered, installed, or mutated; no remote identity-provider flow, release, deployment, or GA claim follows. |
| CLI-AUTOMATION | Never prompt, separate stdout from stderr and keep errors secret-safe, including when a transport implementation throws an error. | Implemented for this slice; durable operation identifiers, waits, cancellation and idempotency await Core. |
| CLI-DISTRIBUTION | Produce an exact, checksum-bound candidate that a clean Node 22 consumer can install without colliding with Core's local-runtime executable. | Implemented for manually dispatched `develop` candidates. The portable package archive contains the `service-lassoctl` entrypoint and requires Node 22; standalone native binaries remain deferred. |

The capability matrix is the operation-to-contract record. The Core compatibility
note records the command ownership decision and the dependencies that must close
before a complete create-to-running-service workflow can be implemented.

## Acceptance

1. `--help` and invalid input are clear, stable and non-interactive.
2. A configurable Core URL has an environment override and a local config file.
3. `service init` produces a minimally valid service-package starter without
   overwriting an existing directory.
4. `instance status` and `service list` use Core's public read endpoints.
5. Lifecycle mutations require `--confirm` and use the public lifecycle route.
6. Unit tests cover parsing, config precedence, scaffold safety, API errors and
   mutation confirmation.
7. The installed external command does not shadow Core's `service-lasso`
   executable.
8. A manually dispatched workflow from the exact `develop` revision creates a
   versioned package archive, a SHA-256 manifest, and an immutable candidate
   record that bind the candidate version, commit, entrypoint, supported
   platforms, and Node 22 requirement.
9. Windows, Linux, and macOS each install the produced archive in a clean
   consumer directory and verify `service-lassoctl --help`, `--version`, and
   safe Core health/identity reads against a local fixture. This is candidate
   evidence only; it does not publish a release or qualify a packaged Core.
10. After all smoke jobs pass, the publish job configures the GitHub Actions bot
    identity locally before it creates the annotated candidate tag; no runner
    global Git identity is required.
11. Post-publication readback retries only a transient HTTP 5xx asset-download
    failure, clears partial files before each attempt, and accepts assets only
    after exact candidate manifest and archive equality verification.
12. Named connection selection is deterministic and never stores or prints
    credentials; local authoring records a pinned template tag, commit and
    manifest checksum without registration or runtime mutation.
13. The compiled CLI can use an environment-only local-admin Bearer credential
    against an explicit loopback Core-shaped fixture to read health, identity,
    capabilities, and services. A rejected credential returns a stable,
    secret-safe status error without copying the token or response body. This
    is fixture-contract evidence only; direct authenticated real-Core
    acceptance requires a supported endpoint and credential outside this
    repository.
14. Issue #16 runs the compiled CLI against Core `develop`
    `02785268392318f14af3d0596df1ca5414957ce8` in a temporary loopback runtime
    at an OS-selected port. The direct read proof covers Core health, instance,
    capabilities and a discovered service, then stops Core and removes all test
    state. It neither weakens Core authorization nor proves remote auth,
    registration, lifecycle, packaged-Core qualification, release, or GA.
