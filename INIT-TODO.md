# Bootstrap checklist

- [x] Create a source-independent `develop` baseline.
- [x] Add repository governance and an active foundation specification.
- [x] Define Issue #6 `CLI-DISTRIBUTION` candidate requirements: a manually
  dispatched `develop` workflow, immutable candidate record, SHA-256 manifest,
  Node 22 package archive, three-platform clean-consumer smoke, and a
  job-local GitHub Actions tagger identity for the post-smoke prerelease tag.
- [x] Define Issue #8 named origin selection and pinned template authoring:
  flag/environment/profile precedence, credentials kept environment-only, and
  the reviewed service-template tag, commit and manifest checksum recorded in
  each non-destructive local scaffold.
- [ ] Issue #8 accepted-template activation: the full-contract materializer is
  ready for an immutable candidate tag, archive/contract checksum tuple,
  provenance and catalog admission. Current `service-template` `develop`
  `bdeb24b84f97e702372ccbcba0794ce30888ad53` is `1.0.0-dev` source-only, so
  default scaffold output and direct generated-project/Core validation stay
  blocked until the owning template release supplies that tuple.
  Its materializer uses a target-host compiled primary launch gate which embeds and verifies both the SEA and a confined writer, retains their execution identity through a private IPC materialization request, and never lets the SEA spawn a verified-then-re-resolved helper pathname. Linux uses descriptor execution; Windows stages with no write/delete sharing and a verified protected DACL held through `CreateProcess`; macOS remains unqualified until target-host native execution proof. The writer itself
  shipped in each native archive. POSIX uses held directory descriptors with
  `openat`/`mkdirat`, an owned temporary candidate, and an exclusive native
  rename into the absent destination; Windows uses `NtCreateFile` relative to held handles with
  `OBJ_DONT_REPARSE`, `FILE_OPEN_REPARSE_POINT`, and a verified protected
  owner-only DACL for every new project object. The SEA never trusts mutable
  provenance for helper execution: it verifies the baked helper digest and
  launches a private copy from the verified bytes. Its request endpoint is an
  inherited non-routable capability, never a mutable Unix socket pathname;
  admission is serialized with the held child lifecycle identity and a commit
  guard rather than a
  PID or an unsynchronised process-state snapshot. The mutual gate proof uses
  a fresh inherited runtime capability, never a packaged private signing key.
  Darwin is fail-closed until
  non-interactive privilege activates and reads back `SF_IMMUTABLE` on the leaf
  and held parent while denying a pre-open writer; Windows pipe requests are
  admitted only from the live SEA PID and capability proof, and starts its
  writer from the held protected staged image rather than `PATH`. It never treats pathname checks as race
  protection. A POSIX write failure leaves the named destination
  absent; a temporary candidate that cannot be proved owned for deletion is
  retained as unknown state rather than being mistaken for a completed project.
  Windows deletes only via held owned handles. Packaged-helper integration tests coordinate parent
  replacement and concurrent unowned failure content; the helper provenance
  hashes its complete source set. Direct template/Core and three-host acceptance
  remain blocked on an owner-published accepted template tuple.
- [x] Define Issue #16 `CLI-REAL-CORE-ACCEPTANCE`: run the source-built compiled
  CLI against pinned Core `develop` `d9e2ae799244317940c862fe1261dfd22b7bdda1`
  on an ephemeral loopback port with temporary workspace and service roots,
  proving read-only inspect/list behavior for an enabled, autostart-eligible
  service while startup is suppressed. The source-built CLI-to-Core proof is
  separate from checksum-bound CLI candidate distribution evidence and creates
  no registration, lifecycle, credentials, or retained runtime state.
- [ ] Complete Issue #1 through bounded slices. Issue #18 implements the
  released-service registration/readback contract; transfer, install,
  lifecycle, distribution qualification and GA remain open. The active
  boundaries are maintained in `SPEC-CLI-FOUNDATION.md` and
  `docs/capability-matrix.md`.
- [ ] Issue #22: implement `SPEC-CLI-DURABLE-OPERATIONS` external durable
  operator slice through `SERVICE_LASSO_CORE_EXPECTED_REVISION`; the current
  reviewed Core candidate is `9bef20259e5b43f6bcd2e9796da0f35396305425`.
  Record compiled external-CLI to source-built Core evidence with disposable
  JWT-authenticated fixtures separately from merged-Core, packaged-Core,
  release, deployment, and GA qualification.
- [x] Evaluate the Issue #1 framework/language candidates in child Issue #20 and record the foundation decision in `docs/decisions/ADR-001-cli-framework.md`.
- [x] Complete Issue #24 `CLI-COMPLETION`: deterministic Commander-derived PowerShell, bash and zsh completion source, safe `--` handling, concise install/CI examples and focused shell evidence. This is a bounded Issue #1 automation/documentation acceptance; no native binary, release or full workflow claim follows.
- [ ] Issue #26: directly validate checksum-bound Node 22.23.2 SEA binaries on Windows x64, Linux x64 and macOS arm64. Each target needs a native no-Node fixture journey; Linux additionally runs the native executable against the pinned actual guarded Core lifecycle route. The portable archive remains separate.
- [ ] Issue #30: retain the source-only publisher with actual provider GET
  preflight (review/admin/strict/direct-force policy), closed archive/portable
  verification, bounded streamed metadata/asset reads, authenticated private
  asset-ID verification before one publish transition, and retained-byte
  readback;
  maintain `SPEC-CLI-PROTECTED-NATIVE-CANDIDATE.md`, Project #1 lifecycle,
  and the concrete `.github` settings proposal. Owner approval, provider
  settings, dispatch, release assets, tags, deployment and Core packaging are
  separately authorised and remain unperformed. The bounded verifier increment
  now requires schema-closed CI-context and no-Node host-acceptance members in
  each native archive, raw duplicate-key rejection, finite gzip/tar/npm
  inspection, and retained byte buffers. Native hosted jobs now produce the
  closed workflow-dispatch context and no-Node acceptance records before
  archiving the accepted bytes; terminal CI dispatch acceptance remains later
  work.
- [ ] Issue #28: enforce the event-specific native provenance contract with real verifier-subprocess proof. `github.sha` is an explicit expected merge context; pull requests require distinct full source, tested-base and merge-context SHAs, while pushes require an intentionally empty expected base, null/absent recorded base and source-equal merge context. Source, event, digest and host checks remain strict.
- [ ] Complete the remaining Issue #1 workflow and automation-contract acceptance recorded in `.governance/project/BACKLOG.md` and `docs/capability-matrix.md`.

- [ ] Issue #30 coherent Darwin six-member archive and literal every-write provider reread bundle: entire independent source review then new complete-input admission before any execution; Core staged-service/outer ZIP and CLI native TAR/Core/operator proof remain open; #35 corrects the former CLI inner-ZIP origin.

Issue #30 all-three entire-review repair maps CLI30-TARGET-PROOF to zero-exit selected-test closure plus a separately digest-bound unavailable inner-route record; CLI30-IDENTITY to caller-bound push/PR/dispatch archive contracts with strict dispatch publication; and CLI30-CLOSED-INVENTORY to bounded regular held-handle local reads including manifest and sums. All regressions remain unexecuted pending new entire source review and ROOT admission.

Issue #30 natural-attempt additions: Windows phase wrappers invoke literal Node scripts without shell forwarding. Darwin root-owned helper/grant/capability transport is an external qualification dependency; hosted execution is explicitly blocked and retains a prerequisite record, while secure native checks and actual tests remain mandatory. The historical exact Core pin remains unchanged and blocked by its own parse failure until independently qualified governed replacement. Original attempt 37003590854 and all logs remain retained; no new execution is authorised.

Issue #30 canonical Darwin grant repair maps CLI30-TARGET-PROOF to the fixed /private/var/db/service-lasso grant and strict no-symlink root-owned ancestor chain (including / and /private). Actual Darwin helper grant-read and fail-closed ancestor/file/capability/object regressions are prepared, UNEXECUTED. Host-owner grant provisioning/descriptor transport remains UNPERFORMED; independently qualified Core replacement remains PENDING. Entire new-head review and NEW ROOT complete-input admission precede execution.

Issue #30 final000b natural-source repair maps CLI30-CLOSED-INVENTORY to ambient tar using an owned output cwd and a fixed relative local archive name (Windows drive/spaced actual producer regression); CLI30-TARGET-PROOF to durable raw phase closure plus explicit present/absent/error digest outcomes and fail-closed missing/error records, and to test-only NODE_TEST_CONTEXT removal preserving all runtime/security variables and literal selected-runner argv. Actual positive inner-route and actual assertion-negative regressions stay mandatory. Darwin ordinary positive tests and hosted native gates require the owner admission contract in docs/native-qualification-prerequisites.md; actual per-object coordination/descriptor transport authority remains unresolved and external. No skip, synthetic grant or blocked-as-pass is permitted. Preserve canonical strict grant/helper/root protections, all earlier publisher fixes and Core9bef pending qualified replacement. Issue #35 explicitly retires the misplaced CLI inner-ZIP assertion; Core #1534 staged-service Windows ZIP and outer release Windows ZIP remain mandatory. New entire independent source review and NEW complete-input ROOT admission precede ALL execution.
Issue #30 qualification-only dynamic-owner seam: add a private inherited Unix owner channel, bounded schema-closed object request/ack/completion protocol with root-peer authentication, actual SCM_RIGHTS held descriptor/dev/inode/owner/type/source digest binding and per-operation serialization. Each acknowledgement coordinates the existing canonical one-object grant and replenishes exactly32 capability bytes before the unchanged restricted sudo helper invocation; it never grants general flags authority or enrolls a caller object. Add explicit Darwin inherited-FD mapping to actual wrapper/smoke/native test callers and denial/transport regression source. The externally admitted ephemeral owner endpoint must independently approve membership, acquire root grant custody, serialize cross-primary operations, and retain raw native/helper/readback/recovery receipts. Its privilege-bearing implementation and Node test-worker FD delivery remain external dependencies until an exact owner profile is approved. All existing mandatory positive assertions and blocked aggregate remain; source seam is not an operational broker or admission.
Issue #30 final010 two-finding repair maps CLI30-TARGET-PROOF to the actual package-native staged Darwin source inventory (including darwin_owner.go) and borrowed descriptor lifetime: each helper operation duplicates held image/parent and capability descriptors, marks the duplicates close-on-exec, and deterministically closes only those duplicates after child closure. The shared parent directory has one retained file owner through set/clear/recovery. Regression source exercises the real staging function plus compile, repeated helper-command set/clear labels with a finite replenished pipe under GC, failed launch and descriptor reuse. No privilege, grant, owner membership, immutable assertion or mandatory native gate is relaxed. All regressions are UNEXECUTED; independent ENTIRE new-head review and NEW complete-input ROOT admission remain required before execution.
Issue #30 automated development publication decision: the owner selected no human development-release approver. CLI30-IMMUTABILITY now permits a valid zero approving-review count with required PRs, empty bypasses, strict current checks and administrator enforcement, and requires a 1..30 minute environment wait plus exact develop-only branch policy. The proposed wait is 1 minute. All immutable tag, held-byte private/public, repeated policy and native/Core gates remain required. Credential/provider application and qualification remain separate; new entire independent source review and NEW complete-input ROOT admission precede execution.

- [ ] #37: entire durable ADR-002/FOUNDATION review and develop landing, coordinated with CLI PR36/Core PR1631 and Core #1633; then separately owned complete TC01..TC12 source implementation, whole review, NEW native input admission and Windows/Linux released journey. Missing immutable tuple/pins/provider/key/native capabilities remain fail-closed.

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
