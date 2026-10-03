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
| CLI-8-CANONICAL-AUTHORING | Materialize a complete accepted template bundle only after its immutable tag, archive and contract checksums, complete inventory/modes, provenance and catalog admission validate. | The compiled primary gate embeds the matching SEA and confined writer and is the sole materialization authority. Linux executes sealed kernel memfd bytes. Darwin requires a verified `SF_IMMUTABLE` leaf and parent, including denial of an already-open writer, and relative launch from the held immutable directory; it fails closed when non-interactive privilege or flag readback is unavailable. Windows uses `NtCreateFile` relative to held no-reparse ancestor/leaf handles, protected owner-only DACLs, retained no-write/no-delete image handles, and a held live-process object for named-pipe admission. An unlink, DACL, chmod, hash/readback, owner-wide endpoint, later pathname open, mutable Unix socket pathname, PID value, or unsynchronised child-state snapshot alone is not that proof. Unix gates pass a connected private socket descriptor directly to the SEA and retain the other endpoint; there is no routable listener to replace. Each launch also creates a fresh inherited runtime capability: gate and SEA prove it in both directions, so neither a package-readable private key nor a self-signed ambient greeting is authority. The Windows staged writer is started only by its protected exact staged path; it is never selected through `PATH`. Every admission is serialized with held child termination state and a commit guard; a request admitted while the child is live is distinct from one presented after child exit, and no implementation claims an impossible atomic process lifetime. Linux uses the child pidfd, Windows the held process object, and Darwin a registered kqueue process event for the original child. Each gate grants exactly one schema-closed materialization request. The SEA never spawns a helper by a re-resolvable pathname. POSIX writer semantics remain held-descriptor `openat`/`mkdirat`, owned temporary candidate and exclusive absent-destination commit. Windows writer semantics remain `NtCreateFile` relative to held handles with `OBJ_DONT_REPARSE`, `FILE_OPEN_REPARSE_POINT`, and a verified protected owner-only DACL for each project object. A package fails closed if this execution or IPC contract is unavailable. `provenance.json` remains archive evidence only. The owner catalog remains empty: package tests may use a controlled already-admitted bundle, but must never fabricate a production admission or publication approval. |
| CLI-REGISTRATION | Register an allowlisted released service and reconcile its actor-scoped durable operation. | Issue #18 consumes Core `develop` `387726b` / merged #1464 through `POST /api/runtime/actions/importService` and `GET /api/operator/operations/{operationId}`. It sends only repo, tag, full commit, manifest SHA-256, idempotency key and `confirm: true`; Core remains authoritative for allowlisting, provenance, identity, permission, audit and durable state. Caller-local paths, staged bytes, transfer, install and lifecycle remain outside this slice. |
| CLI-OPERATIONS | Read status and services; invoke supported lifecycle actions with local confirmation. | Read and lifecycle routes are unit-tested transport adapters only until Core publishes the exact API/version/permission contract. |
| CLI-REAL-CORE-ACCEPTANCE | Exercise the compiled CLI against an explicitly selected authenticated Core-shaped HTTP endpoint using only public read routes, and classify fixture proof separately from live-Core proof. | Issue #14 adds a loopback fixture contract for `instance inspect` and `service list`. The fixture proves environment-only Bearer transport and secret-safe failures, but no live Core credential or supported runtime is available in this repository. |
| CLI-REAL-CORE-LOCAL-READ | Exercise the compiled CLI against the explicitly supplied actual Core revision using only loopback public read routes. | The run requires `SERVICE_LASSO_CORE_EXPECTED_REVISION`, checks the checked-out Core against it before import, starts a disposable Core at port `0` with temporary workspace and service roots, then verifies `instance inspect --json` and `service list --json`. It is direct local read proof only: no service is started, registered, installed, or mutated; no remote identity-provider flow, release, deployment, or GA claim follows. |
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

## Complete canonical authoring amendment (#37)

Acceptance3 is complete canonical authoring, not a reduced starter. [ADR-002](../../docs/decisions/ADR-002-canonical-authoring-native-v3.md) is normative for TC01..TC12, all75 exact owner paths, original four read-only assets, seven/eight/six field shapes, independent source admission, genuine Git TAR, owner id/name and typed changes, opaque one-use library compatibility, binary native v3 READ/PLAN/CLOSE, full held final readback and finite retained resources/deadlines. No reduced fixtures or caller bundle grants substitute for production admission. Complete shared full dry-run resolves/validates/derives then CLOSEs without writer. Empty catalog remains zero-IO blocked.

The coordinated Core #1633 source-safe contract owns CA01..CA08 and trusted remote staging/preflight/nested confirmation/commit/safe operation readback/full materialization; CLI packages only the chosen local output and never sends its path. Actual released CLI plus fresh packaged Core Windows/Linux create→remote discovery→separately authorized setup/install/start→actual running/health/process/lifecycle readback is required; Darwin deferred under canonical Core platform policy. Publication of four original assets and immutable readback precede independent CLI/Core catalog pins and consumer qualification. Pending CLI PR36/Core PR1631 errata and their fixed two-role register remain separately owned dependencies; current source/catalog/provider/native grants are absent, fail closed. This docs amendment needs fresh entire review and develop landing before whole source implementation. Full delivery remains incomplete.

## Issue #35 coordinated release-contract errata

Active SPEC-CLI-SCOPED-RELEASE-ERRATA CLI35-ARCHIVE-ORIGIN/CLI35-TEMPLATE-ROLE maps Core #1628 SPEC-008 R2/R4/R5/T1–T5/R6/R7: selected protected2/portable2 native TAR 6/7/8; explicit retirement of agent-added CLI inner-ZIP blocker with Core staged-service/outer ZIP retained; fixed CLI/Core roles preserving all TC01–12/CA01–08, exact failure grammar and prospective source-owned admission/proof paths. Existing source and failed evidence remain historical, never pass. Actual source catalog/owner publication, native/helper/primary/Core/operator proof and separate complete Core admission architecture remain blocked. Darwin deferred never PASS. Four-repo source-only bundle requires fresh distinct ENTIRE cumulative amendment review and develop landing before held producer implementation; NEW complete-input admission precedes later execution. No tests/build/native/parser/provider settings/publication executed here.

## Current integration authority (2026-10-04)

Core PR1631 and CLI PR36 are landed dependencies, integrated through current develop Core33e19ea24aaaff62e5dbfbb8d57001576fb2fb16 and CLIcf95d4577b5d9e4bb37c02807e738b512dec8be9. The landed four-repository errata/register takes precedence over historical CLI inner-ZIP blocker wording: CLI protected/portable authoring uses native TAR; Core #1534 staged-service and outer-release Windows ZIP remains required. Original TUI five actions and same published tool bytes in Core/npm remain required. Historical pending/review text above records earlier checkpoints, not current ownership or a reopened dependency.

Complete inventory means every file in the independently admitted owner tuple, with its exact path, mode, bytes or explicitly typed difference, policy self-member and generated provenance. Template PR25 owns the new prospective source tuple (reviewed source d3f9c86fc55f3bb4a31f0d127c2b3d5bad291885; 83 inventory members plus policy =84 source files) and its publication/source binding; generated provenance is derived separately. This contract grants it no admission and never makes84 a permanent inventory count. Historical 73+policy+provenance=75 tables and their size/quota arithmetic are informational bindings to the old frozen tuple only, never a 75-file cap or a grant for a newer tuple. All normative full75/all75/expected75 references above mean the complete inventory of the separately admitted tuple; no current or future file may be truncated to fit that historical example. Current owner quotas must be independently validated and all retained-copy/frame/parser/native limits met before enablement; an incompatible newer tuple fails closed pending an explicit reviewed limit amendment, never silently drops members or inherits old hashes/quotas. Native v3 HMAC/one-use session, full held readback, TC01..TC12/CA01..CA08, upload/stage/preflight/confirm/commit/poll and journal/key/fixed-ID exact-payload durable Audit no-loss outbox remain complete and unchanged. Missing immutable publication, catalog/provider/native capabilities and direct Windows/Linux released journey remain unmet.

This integration is source-only documentation. Different fresh ENTIRE final-source review is required for both complete integrated contracts before governed landing. Product source authorship, imports/parsers/build/npm/tests/native execution, new API decisions, publication and deployment are outside this integration unit.

## Issue #39 complete native v3 implementation boundary

Current complete CLI contract landed at develop c073495b620cb0759738eba6098a8af893792319. Full TC01..TC12 plus protected2/portable2 Windows/Linux native TAR source success route remains required. Fresh sole source author discovered missing externally authenticated host-admission/reservation/recovery and private TC native proof boundaries. Parent explicitly directed WHOLE blueprint freeze before implementation; see docs/decisions/BLUEPRINT-003-complete-native-v3-host-admission.md. This proposed service/persistence/proof amendment awaits DIFFERENT ENTIRE architecture review and governed landing. No product stubs, implementation, imports/parser/compiler/build/npm/tests/native/ACL/lifecycle/workflow actions, admission/publication/qualification or full-goal claim. Existing catalogs stay empty until independently qualified immutable tuple/pins; new profile/service/store/observer provisioning authority absent. Actual released compatible Core/F7 remains missing; no repin. Legacy Darwin runtime/distribution and Core staged-service/outer Windows ZIP remain separately required.
### #39 HA3 two-stage closure amendment (2026-10-04)

Entire architecture review at070c76 confirmed F1: primary/channel closure before FREE made its CLOSE_RESULT undeliverable. Historical whole NO-GO and failed proposal remain preserved. BLUEPRINT-003 HA1..HA4 and exact appendix now select protocol/profile/ledger v2, durable CLOSE_INTENT_ACK while original primary/channel remain charged, CLOSE_PENDING, independent original natural-zero/streamEOF/channel/resource closure, committed FREE and separate already-admitted observer TERMINAL_RECEIPT/archive ACK. No primary retry/reconnect, no live-primary exclusion, no finite kernel closure promise. Fixed4/user16/host accounting, original10/10/10/20s and40s deadlines and everyTC01..TC12/scoped Windows/Linux native TAR/Core/operator/publication gate remain required. Observer control resources have a separately bounded selected service-control charge and finite pre-reserved90-day evidence retention; resource FREE does not clear unacknowledged terminal storage. Service crash loses handles and quarantines used slots; undelivered FREE cells remain occupied without replay.

FOUNDATION requirement CLI39-HA3-CLOSE maps the whole blueprint HA1..HA4/appendix and TC06/TC12 to realizable normal closure, malformed/abnormal/lost observer/deadline/storage/crash failure proof. Private prospective service-lasso.cli-template-native-proof.v1 closure has exact additional closeIntent/finalization rows; parent relays complete grammar to Core #1644 without this author modifying Core. This is a full six-document architecture proposal correction, awaiting a DIFFERENT fresh ENTIRE cumulative architecture review before any product implementation. No actual service/profile/catalog/tuple provisioning, product execution, provider controls, publication or qualification. Sole writer accepted the existing clean feature/39-complete-native-v3 checkout at070c76/basec073 under explicit successor custody; preserve open PR40 branch until governed develop landing.

### #39 HA5 production launch ownership amendment (2026-10-04)

CLI39-HA5-LAUNCH maps BLUEPRINT-003 HA1..HA5 and the exact launch appendix to all ordinary released CLI, exported library load/materialize/full async preview and qualification paths. Entire76bd review confirmed sole F2: qualification-only observer ownership leaves normal production without pre-primary identity/capture binding. Before any architecture or product implementation, intent now selects the independently provisioned source-owned host service as production launch owner, a fixed public native facade transport, an external service-owned observer and gated original approved primary, with actual streams/source/run prebound before RESERVE. Library calls use the same native facade transport and one-use remote session, not caller enrollment or a local compiler. Finite charged forwarding and library terminal success/error follow external disposition; synchronous blocked preview remains truthful. New launch/profile/private-proof grammar is prospective and awaits DIFFERENT ENTIRE architecture review and governed landing. Preserve all original TC01..TC12, full owner inventory, native v3 caps/id/name, four-user/sixteen-host limits, two-stage closure, original deadlines/no retries, complete publication/Core/operator gates. No implementation, tests, native execution or provisioning authority. Parent must relay exact amended launch proof to Core #1644 before final freeze/review; existing76bd reader pin does not cover new bytes.

### #39 HA5 original input-root routing amendment (2026-10-04)

CLI39-HA5-INPUT maps BLUEPRINT-003 HA5/its exact launch appendix, ADR-002 READ/library compatibility and TC01..TC12. Entire0265 architecture NO-GO sole F3 is the missing preserved local-root dataflow. Before blueprint amendment or implementation, intent selects a private bounded S inputRoot in LAUNCH, immutable once per run, forwarded unchanged to the original primary/SEA/native READ; LIBRARY_ACTION cannot replace it. Existing loader/scaffold/fullpreview signatures remain, relative caller roots resolve once against the actual caller cwd before strict UTF8 encoding. Root is routing only, never catalog/profile/source authority; original four fixed no-follow held reads and independently source-owned tuple/semantic validation/input preservation remain required. No ambient/default/catalog-root substitution. Exact phase, copies, budget, raw custody and positive/denial proof are defined in the whole blueprint. closure.launch adds inputRouteRef and parent relays this closed grammar to Core1644 before final freeze; old76bd pin remains held until architecture GO/final alignment. Preserve F1/F2, native v3 caps/codes, complete inventory/TAR/id/name, 4-user16-host plus finite control, true physical closure/two-stage FREE/archive ACK/no self-digest, original deadlines, scoped publication and Core/operator gates. Source-only six-document proposal, different fresh ENTIRE review required; no product execution/provisioning authority. Sole successor owns clean stopped0265 branch/openPR40, retained pending governed develop landing.


## #39 F4 complete library inspection transfer proposal (2026-10-04)

CLI39-HA5-INSPECTION binds the ENTIRE BLUEPRINT-003 HA1..HA5/native v3/library/publication proposal, ADR-002 TC01..TC12 and the original Buffer-copy API. Actual owner2b9 policy blob51c449dc37ed73c04fb7ad21e127c74522a2131c is18891 bytes/SHA256ee2d0f0696c8022d1cc5bca76eec8751acbe3e4fd60415cbb366f4d85eddf69f; its83 inventory members contain381008 bytes. Even the raw policy self-member cannot fit the selected16384-byte LAUNCH_PENDING frame. Existing load/accepted.files and acceptedTemplateFiles callers require complete independent Buffer data. HA5 currently supplies only pending inspection, actual-stream OUTPUT and compact terminal metadata: no full-file transfer operation. This is a confirmed source-contract feasibility gap, not absent provisioning or a failed product attempt.

Before implementation, the whole amendment proposes a distinct authenticated private full-inspection BEGIN/CHUNK/END route from original SEA through original primary/service/facade to source-owned library, with every frame<=16384, complete dynamic inventory/policy/provenance, independent raw/source/receipt/equality checks and actual lifetime accounting. Native v3 READ/PLAN/CLOSE codes/caps, public OUTPUT bytes, original root route, F1/F2/F3, owner id/name, deadlines,4-user16-host/control/store limits and all publication/Core/operator gates remain required. New private data frames and explicit external client-copy accounting require fresh different ENTIRE architecture review and coordinated Core reader source binding; they are not implementation authority. closure.launch adds inspectionRouteRef under the exact blueprint amendment; parent coordinates Core only, this author cannot cross-write it. Existing landed Core1644 binding remains historical and cannot cover this new row until reconciled.

Complete source preparation remains the intended unit after architecture review/landing. No narrow parser, EMPTY-capability product stub, guessed tuple, alternate observer, cap widening, stdout data tunnelling, input cleanup or compiler fallback is permitted. Actual source/profile/key/store/native actor/catalog/immutable tuple/Core F7 remain absent/unqualified. Different ENTIRE product source review plus NEW complete-input ROOT still precede every product import/parser/compiler/build/test/native/ACL/lifecycle/workflow action. This source-only proposal is blocked for architecture review, not implementation complete or acceptance.
