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
| CLI-AUTHORING | Create a non-destructive, caller-selected package starter from a pinned service-template identity. | The generic materializer accepts only source-owned versioned admissions and archive-held bytes; no accepted template tuple is currently admitted. |
| CLI-8-CANONICAL-AUTHORING | Materialize a complete accepted template bundle only after its immutable tag, archive and contract checksums, complete inventory/modes, provenance and catalog admission validate. | The compiled primary gate embeds the matching SEA and confined writer and is the sole materialization authority. Linux executes sealed kernel memfd bytes. Darwin requires a verified `SF_IMMUTABLE` leaf and parent, including denial of an already-open writer, and relative launch from the held immutable directory; it fails closed when non-interactive privilege or flag readback is unavailable. Windows uses `NtCreateFile` relative to held no-reparse ancestor/leaf handles, protected owner-only DACLs, retained no-write/no-delete image handles, and a held live-process object for named-pipe admission. An unlink, DACL, chmod, hash/readback, owner-wide endpoint, later pathname open, mutable Unix socket pathname, PID value, or unsynchronised child-state snapshot alone is not that proof. Unix gates pass a connected private socket descriptor directly to the SEA and retain the other endpoint; there is no routable listener to replace. The SEA must use that fixed inherited descriptor only: ambient endpoint names cannot select a server. Windows must authenticate the pipe server with a gate-held launch capability before it accepts a result; a client PID by itself is insufficient server authority. Every admission is serialized with held child termination state and a commit guard; a request admitted while the child is live is distinct from one presented after child exit, and no implementation claims an impossible atomic process lifetime. Linux uses the child pidfd, Windows the held process object, and Darwin's one-shot inherited endpoint closes with the child and never identifies a peer by a reusable PID. Each gate grants exactly one schema-closed materialization request. The SEA never spawns a helper by a re-resolvable pathname. POSIX writer semantics remain held-descriptor `openat`/`mkdirat`, owned temporary candidate and exclusive absent-destination commit. Windows writer semantics remain `NtCreateFile` relative to held handles with `OBJ_DONT_REPARSE`, `FILE_OPEN_REPARSE_POINT`, and a verified protected owner-only DACL for each project object. A package fails closed if this execution or IPC contract is unavailable. `provenance.json` remains archive evidence only. The owner catalog remains empty: package tests may use a controlled already-admitted bundle, but must never fabricate a production admission or publication approval. |
| CLI-REGISTRATION | Register an allowlisted released service and reconcile its actor-scoped durable operation. | Issue #18 consumes Core `develop` `387726b` / merged #1464 through `POST /api/runtime/actions/importService` and `GET /api/operator/operations/{operationId}`. It sends only repo, tag, full commit, manifest SHA-256, idempotency key and `confirm: true`; Core remains authoritative for allowlisting, provenance, identity, permission, audit and durable state. Caller-local paths, staged bytes, transfer, install and lifecycle remain outside this slice. |
| CLI-OPERATIONS | Read status and services; invoke supported lifecycle actions with local confirmation. | Read and lifecycle routes are unit-tested transport adapters only until Core publishes the exact API/version/permission contract. |
| CLI-REAL-CORE-ACCEPTANCE | Exercise the compiled CLI against an explicitly selected authenticated Core-shaped HTTP endpoint using only public read routes, and classify fixture proof separately from live-Core proof. | Issue #14 adds a loopback fixture contract for `instance inspect` and `service list`. The fixture proves environment-only Bearer transport and secret-safe failures, but no live Core credential or supported runtime is available in this repository. |
| CLI-REAL-CORE-LOCAL-READ | Exercise the compiled CLI against a pinned actual Core runtime using only loopback public read routes. | Issue #16 starts a disposable Core at port `0` with temporary workspace and service roots, then verifies `instance inspect --json` and `service list --json`. It is direct local read proof only: no service is started, registered, installed, or mutated; no remote identity-provider flow, release, deployment, or GA claim follows. |
| CLI-AUTOMATION | Never prompt, separate stdout from stderr and keep errors secret-safe, including when a transport implementation throws an error. | Implemented for this slice; durable operation identifiers, waits, cancellation and idempotency await Core. |
| CLI-COMPLETION | Emit deterministic, read-only PowerShell, bash and zsh completion source for the declared CLI command tree. | Issue #24 owns static completion generation. Candidates derive from Commander declarations, never inspect Core, configuration, credentials, service identifiers or the filesystem, and stop suggesting options after `--`. |
| CLI-DISTRIBUTION | Produce an exact, checksum-bound candidate that a clean Node 22 consumer can install without colliding with Core's local-runtime executable. | Issue #26 owns standalone `service-lassoctl` SEA executables. Issue #30 owns the source-only protected development-candidate publisher that combines the portable package archive and those three native archives without rebuilding after acceptance. Provider settings, dispatch, public release bytes, Core packaging, deployment and GA remain separate evidence gates. |

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
14. Issue #16 runs the source-built compiled CLI against source-built Core
    `develop` `d9e2ae799244317940c862fe1261dfd22b7bdda1` in a temporary loopback
    runtime at an OS-selected port. The direct read proof covers Core health,
    instance, capabilities and an enabled, autostart-eligible discovered service
    while Core startup is explicitly suppressed; it uses a bounded startup wait,
    then stops Core and removes all test state with visible cleanup failures. It
    neither weakens Core authorization nor proves remote auth, registration,
    lifecycle, packaged-Core qualification, release, or GA. The checksum-bound
    CLI candidate is separate distribution evidence and is not an input to this
    source-built CLI-to-Core acceptance.
15. Issue #18 adds `service register` and `service operation` through the Core
    released-service registration contract. The CLI reads
    `SERVICE_LASSO_CLI_LOCAL_ADMIN_TOKEN` only from the environment and sends
    it as `x-service-lasso-admin-token`; it rejects cleartext non-loopback
    origins, malformed request values and malformed operation responses before
    exposing result data. Focused transport tests cover denial, replay,
    conflict, unknown readback, secret-safe errors and invalid-input
    no-mutation. This is source/fixture contract evidence only; packaged
    external CLI, hosted exact-head CI and GA remain separate gates.
16. Issue #24 provides `service-lassoctl completion powershell|bash|zsh` as
    deterministic source on stdout. Candidates are derived from the declared
    Commander command/options tree, include no runtime-derived values, and stop
    option suggestions after `--`. The generated source is safe to save and
    source locally; it does not evaluate completion input, contact Core, read
    credentials or write state. PowerShell candidate filtering uses ordinal
    literal-prefix comparison, so wildcard and punctuation prefixes are not
    interpreted as patterns or code. Its zsh bootstrap retains `compinit -i` so
    insecure completion paths are ignored, and adds `-D` to suppress
    `.zcompdump` writes; `-D` may still read an existing dump. Focused generator tests plus actual PowerShell
    checks for literal wildcard, punctuation, whitespace and quote prefixes and
    hosted bash/zsh shell checks establish only CLI completion behavior, not
    a release, native binary or full Issue #1 qualification.
17. Issues #26 and #28 build a bundled CommonJS Node 22.23.2 SEA main script with
    pinned `esbuild` and `postject`, then binds every target-host executable to
    its source commit, SHA-256, operating system and architecture. The native
    fixture journey runs preview, changed-context rejection, one confirmed
    execution, same-key reconciliation, operation get/wait and unsupported
    cancellation with `node` absent from the child `PATH`; it redacts tokens
    and response details. Linux also runs that native executable against the
    separately pinned source-built guarded Core. Fixture and direct-Core proof
    remain distinct from portable-candidate, release, deployment and GA gates.
18. Issue #30 consumes the portable candidate and Issue #26 native packaging
    contracts to construct one full-SHA, closed-asset development candidate.
    Every target-host job executes the matching native executable with Node
    absent from its child `PATH`; the publisher only downloads, verifies and
    uploads those accepted bytes. It fails closed on a non-exact existing
    release, tag, asset, manifest, provenance, checksum, version, SHA or public
    redirect. A complete immutable collision is readback-only. This source
    requirement is not evidence that settings are protected, a workflow ran,
    any bytes are public, Core can consume the assets, a release occurred, or
    GA is qualified.
19. Native CI provenance accepts only two event contracts. The verifier receives
    the full expected merge-context SHA from `github.sha` and requires the
    recorded value to equal it. A `pull_request` requires a full tested-base SHA;
    its source head, base and actual merge context are all distinct full SHAs. A
    `push` requires an explicitly empty expected-base argument and a recorded
    `testedBaseSha` that is null or absent; its expected and recorded merge
    context must equal the source SHA. Both contracts require full source and
    event identity plus executable digest, platform and architecture checks.
    Malformed, missing or contradictory event, base, source, merge-context,
    digest or host data fails verification.

## Framework decision traceability

Issue [#20](https://github.com/service-lasso/service-lasso-cli/issues/20) resolves Issue #1's framework-comparison acceptance for this foundation: Commander, Cobra, Kong and clap are evaluated in `docs/decisions/ADR-001-cli-framework.md`. The selected current direction is TypeScript/Commander. This does not claim a standalone binary, shell completion, cross-platform release acceptance, or the full Issue #1 workflow. Any language migration or native-binary commitment must start as a new bounded issue and update this specification before implementation.
