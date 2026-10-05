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

## #39 actual whole product-source implementation ownership (2026-10-04)

The coordinated architecture pair CLI41/Core1653 is normally landed. This fresh Development SOURCE ONLY unit starts at CLI develop e5c431de9e383a8ca5a307bcc00b55b8f6997020/tree d9e7d2713972a46f59b857a7b9e2fcadc88972b4 and owns feature/39-actual-product-f4 in the isolated cli39-actual-product-f4-oct04 checkout. CLI39-TC01..TC12, CLI39-HA1..HA5, CLI39-HA5-INPUT, CLI39-HA5-INSPECTION and the scoped release requirements govern the COMPLETE production library/scaffold/fullpreview/materialize/dispose/facade/service/primary/SEA/writer/provider/observer/private-reader/resource-accounting source, full original owner inventory and all original error/deadline/publication/Core boundaries. Architecture GO is not product implementation or product SOURCE GO. The authenticated ADR native greeting supplies launchNonce; withdrawn F5 imposes no equality/new field.

Pre-code traceability: canonical parsing/catalog/owner derivation maps TC01..TC05/TC07..TC11; native original source custody/READ/PLAN/CLOSE/writer readback maps TC06/TC12 and HA1/HA4; production service/facade/observer/prebinding/ledger/two-stage closure maps HA1..HA5/TC12; exact root and complete BEGIN/CHUNK/END/private copies/control-retention map CLI39-HA5-INPUT/INSPECTION and TC06/TC11/TC12; original-source scoped protected2/portable2 Windows/Linux TAR maps SPEC-CLI-SCOPED-RELEASE-ERRATA and SPEC-CLI-PROTECTED-NATIVE-CANDIDATE. Meaningful test source is UNEXECUTED. Missing actual profile/provider/keys/catalog/immutable pins/allocator/native grants/F7 and native1640 D1/D4/D8 NO-GO are preserved independently. No product imports/parsers/compiler/build/npm/tests/native/ACL/lifecycle/provisioning/dispatch/rerun before different ENTIRE final product SOURCE GO and NEW complete-input ROOT. Parent owns tracking/landing; every intentional commit is immediately pushed and PR-bound. No main access, nested delegation, other-worker takeover or cleanup.

## CLI39 N2 review03 normalization — current prospective contract

The complete current decision is docs/decisions/NODE-MEASURED-FINITE-BUDGET-N2-PRECODE.md sections1..12. It supersedes rejected current N2 wording while retaining every frozen option, S1 SOURCE_NOGO and review03 original. Human-selected full Node/public library/Buffer/API/full ICU uses measured finite B0/B2; Q32 is finite prospective qualification capacity, never measured production B or a host exemption. Store4MiB and Core128MiB remain unchanged.

PA1's aggregate service-owned1MiB includes bootstrap/runtime/stack/IO/image/lease/parser/capture/control together. Blueprint003 control wording does not erase PA1 ownership. Correct the draft's unsupported blanket attribution of every OS import: original backing and mapped-view ownership/source/image/lifetime/custody must be established individually; unknown denies fit. All service-owned linked/generated helpers, CRT/user images and bootstrap remain charged. This correction needs no new human cap choice. A transfer of genuinely known service-owned resources to larger B1 would require a separate material decision and is unselected.

Traceability: HA1/PA1 requires actual independently admitted provider/caller/ROOT/P/G/C/key held readers; N1/N2 requires full early Node/runtime/image/stack allocation ownership and authenticated parent-copy ledger; HA4 requires positive typed C11 constructor and one complete1MiB aggregate; HA5 INPUT/INSPECTION requires the same four inputs, complete private wire and native returned copies; TC01..12/CA01..08 requires full ordinary Windows/Linux effects, writer/mode/rename/observer/archive closure and meaningful whole fixtures. Decision sections6..10 specify source actors, exact prospective codecs, capacity layout and positive constructor ordering. Stride arithmetic/static PE headers do not prove actual fit or authority.

Fresh DIFFERENT entire architecture review of a NEW complete actual-source/cumulative-input ROOT precedes dependent N2 implementation. Complete positive product source and meaningful full fixtures then require DIFFERENT entire final SOURCE GO/new ROOT before authentic tool/native measurement admission and execution. Existing ordinary-source repairs are unexecuted. No partial GO, all-denial completion, provider/image/key grant, cap waiver, source generation/build/test/import/native/CI/settings action or release claim is authorized by this mapping.
## CLI39 N2 independent whole architecture adoption05 — source implementation only

Parent accepted independent WHOLE_ARCHITECTURE_GO_FOR_SOURCE_IMPLEMENTATION_ONLY for exact normalized candidate04 ROOT4bfd80a7497d13596de8cb0e2446c2ff5ad9676bd3356d9ec2ce37123861eb94 and decision35333 SHA8688b934d4e89933a44f8a6a6db43b6cad21d499284d3ec8ad751da08b1021fd. Review D:/projects/service-lasso/_audit/cli39-normalized-n2-entire-independent-oct05-05 ROOT34d64075c8e490242097b670843f5a12185418a4cc9795a33e31712fc1c83f90; REPORT15043 SHAc9ff0ff6dcc932a81ace1a140ffa8a76de72701cb1b31a2ebe47d69f59d81568. Author read full report/root and verified all9 manifest members plus2 ROOT members actual size/SHA PASS0, including337084596-byte original audited manifest. Frozen candidate04/review05/history remain unchanged.

The selected twelve-section whole architecture is now approved for actual source implementation: complete positive C11 service/primary/writer, early full Node allocator/hooks/builtin/native parent copies, genuine admitted caller/provider/catalog/key readers, production ordinary library/SEA/native engine/writer/observer/archive/packaging and full meaningful TC01..12/CA01..08 fixtures. Declarative interfaces, opaque constructors, all-denial and unused components are not completion. Preserve full ICU/API/flags, B0/B2 measured-finite direction, aggregate service-owned1MiB, store4MiB and Core128MiB. Unknown native image/ownership fit remains unresolved and cannot be foreign relabeled. Q32 is not production B. Actual original source/tool/native actors/provider/ROOT/key/catalog/measurement remains separately absent/unadmitted.

This adoption supersedes current pending-architecture language only for the exact candidate04 source choice. It grants no final product SOURCE GO, native fit, measured B, compiler/import/parser/build/test/native execution, resource/installation/publication/GA authority. Finish complete coherent positive source and whole fixtures, then NEW complete cumulative ROOT and DIFFERENT ENTIRE final SOURCE review and authentic input admission before execution. Same sole issue owner/PR42; every intentional coherent commit immediately pushed, no main or nested agents.
## CLI39 IA1 original-input issuer amendment — prospective, not selected

The complete candidate is docs/decisions/INPUT-AUTHORITY-ISSUER-ALTERNATIVES-PRECODE.md, read with EXTERNAL-INPUT-AUTHORITY-PROVISIONING-SOURCE-DESIGN.md and the entire PA1/N2/Blueprint003 source contract. No existing governing HA1/HA5 issuer implementation, credential-acquisition interface or authentic output has been identified. GitHub identity/Development publication permission and HA2 ledger MAC are not ROOT authority. Option E requires actual existing issuer source/credential/output and independent authority; recommended NEW IA1 requires independent explicit principal/public-anchor selection after whole architecture amendment review. No anchor/key/principal is created or selected here.

IA1 specifies full-Node offline encrypted PKCS8/native console enrollment, exact private statement/actor codecs and original signature preimage, Windows original handle/Linux same-process lease handoff, original caller/image/owner/ROOT associations, one-use attempt transfer and exact-input/revocation lifetimes. New issuer/verifier/codec source is NOT covered by previous PA1/N2 GO and must not be implemented before NEW DIFFERENT ENTIRE amendment review. Shared public APIs/library/SEA/full ICU/flags and proof18/10/11/13/11/memory20 remain unchanged. Complete issuer finite full-Node ownership is explicit, not a hidden pool; service aggregate1MiB/store4MiB/Core128MiB remain unchanged. Original HA2 key-provider authority is separate and unsupplied.

Traceability: HA1/PA1/HA5 INPUT maps exact original issuer/provider/caller/ROOT/P/G/Q/C and native handoff; HA2 maps separately admitted original key lease; HA4/N1/N2 maps complete actual verifier/image/stack/CRT/control/IO custody and early full-Node issuer ownership; HA5 INSPECTION/TC01..12/CA01..08 maps unchanged ordinary Windows/Linux positive effects, private wire, original parent copies and archive/retirement. Strict codecs/positive semantic fixtures do not authenticate originals. Current product source remains incomplete; independent approved ordinary native routes continue. Whole actual current source, intent/spec/backlog/INIT/traceability and cumulative original inputs must accompany IA1 review. No executable/native/tool/profile/CI/settings/release grant follows from this candidate.
## CLI39 IA1 sealed NO_GO and coherent whole repair02 — prospective only

Whole review01 ROOT0ca325bd013a5241555f4ff91438e650a82a05fe218f9bd3b098d360cc87065e / REPORT SHAe943f5542cc3b251bcda75293d6bf699960c9370911963efcfbfc2621b378933 rejects exact frozen421/candidate01. That candidate and every original failed/hash-method/private/custody body remain unchanged. Current canonical docs/decisions/INPUT-AUTHORITY-ISSUER-ALTERNATIVES-PRECODE.md sections1..10 is the complete prospective repair02, superseding the rejected current seven-role/circular-root and inconsistent cardinality proposal only. It has NO architecture/source/execution/authority GO yet.

IA1-01 maps complete genuine commit-to-root-tree-to-all-tree/blob traversal and retained missing421 root correction. IA1-02 maps acyclic payload InputROOT, detached15-key statement/signature and later EvidenceROOT; leaf-only source associations cannot index themselves/envelope/final-root. IA1-03 maps explicitly NEW independently human-selected native BQ initial qualification authority and original source/caller/provider/tool/image/ROOT leases before issuer Q32/secret entry; no future IA1 self-signature/source review/constructor/token string selects that principal. Administrative issuer is domain0 combined same full-Node finite owner, not another pool. Actual principal/key/anchor/enrollment/realm/B remain absent and unselected.

IA1-04 maps exact15 statement keys plus four closed original named-member Q/C/case/capacity indexes, complete actual multi-C/evidence joins and same128 native reference graph before effects; equal hashes never alias distinct originals. IA1-05 maps complete14-role native primary/confined writer/SEA/Node parent/facade/library transport/observer/provider/issuer/BQ executable/source/P joins without changing P or shared APIs/wires. IA1-06 maps exact original Monocypher4.0.3 commit ab2b16dd619ad5f6979a4fbe69cfa324a6fcc35f optional SHA512 Ed25519 verifier, full genuine source graph, original known public-result timing/point-encoding behavior, C11 ABI/build/security/range/workspace and actual service-owned fit prerequisites. Source capture selects no tool execution or authentic trust anchor.

The complete repair must accompany NEW actual current-source/cumulative EvidenceROOT and NEW DIFFERENT ENTIRE architecture review before dependent IA1 code. Human subsequently selects the concrete real authority/anchor/credential/BQ role; no values are invented here. Full Node22.23.2/full ICU/Buffer/API/flags/library/SEA/Windows/Linux and original four authoring inputs remain. Service aggregate1MiB/store4MiB/Core128MiB, HA2 separate key-provider authority, shared18/10/11/13/11/memory20/Q12/C13 and conservative authenticated original-parent exit custody remain unchanged. Independent PA1/N2 approved ordinary actual source implementation continues; whole positive owning product/TC01..12/CA01..08 is unfinished. Different final ENTIRE SOURCE GO/new complete authentic input admission precedes native/compiler/parser/build/import/test/calibration; no CI retry/cancel/main/settings/release action.
## CLI39 IA1 whole repair03 — prospective SP1 representation and ordered stages

Current complete prospective contract is INPUT-AUTHORITY-ISSUER-ALTERNATIVES-PRECODE
sections1..10 WITH docs/decisions/IA1-SP1-REPRESENTATION-AND-STAGES-PRECODE.md.
Sealed entire review02 REPORT39a398e44156c2318ffc89b90a5eba147843d55bcd23c73483be062dafe024b4
and ROOT453b6573dc9de65b31fb8ae98fc6e7358c53ed0e2c005729ed885506f5268ef8 remain
WHOLE_IA1_ARCHITECTURE_NOGO_FOR_DEPENDENT_SOURCE_IMPLEMENTATION. Repair02/frozen01/
review01/original failed-method/private/custody evidence are preserved unchanged.
This current proposal supersedes repair02 current wording only; no partial GO.

R02-01/IA1-01/IA1-04/HA1/HA4/HA5/N2 map NEW explicit SP1 original SourceUnit,
SourceAuditUnit and StageIndexUnit representations, full raw Node/verifier/native/
issuer/BQ/tool/SDK/CRT source and ancestry, independently admitted native immutable
identity/member ranges/EOF, charged bounded streaming and actual retained raw
capture readback. Original files are not falsely relabeled as their old kernel
objects. Complete388495 historical associations without deduplication sum7722113187
body bytes; fixed table49727360 plus header gives7771840611 below8GiB SOURCE FILE
bound, not generated pack/native memory fit/authority. Complete retained128 witness
includes all fourteen roles, every stage control, explicit same-process distinct-entry
topology, multi-C simultaneous cases and THREE separate issuer births. Extra genuine
originals require another fitting witness or truthful denial, never source truncation,
metadata-as-native-reference, equal-hash alias, secret foreign pool or early retirement.

R02-02/IA1-02/IA1-03/IA1-04/HA1/HA2/HA5 map ordered unsigned R0 native/bootstrap ->
unsigned R1 nonsecret issuer Q32 -> independent BQ R2 readback -> explicitly selected
unsigned R3a credential Q32 calibration -> independently certified credential-inclusive
finite B0/R3b production -> signed R4 consumer handoff. Each exact preeffect set omits
its OWN future outputs; only existing native/bootstrap C enters R1, no future issuer
C/S/V/key self-authorizes initial qualification. R3a actual credential/OpenSSL/PKCS8/
Buffer/native overlap calibration occurs BEFORE production B, with original private
copies charged through genuine corresponding issuer exit. Human-selected HA2 ledger
key and issuer signing key remain distinct authorities. Q32 never becomes production B.

IA1-05/IA1-06/HA4/N1/N2/HA5 INSPECTION/TC01..12/CA01..08 map unchanged fourteen-role
source/image/entry/native joins, original Monocypher4.0.3 optional SHA512 Ed25519/full
raw graph/actual known behavior, complete actual1MiB native verifier/image/stack/CRT/
IO/capture/metadata ownership, full Node22.23.2/full ICU/Buffer/API/library/SEA and
complete original four authoring inputs/public-private wire/ordinary positive fixtures.
Service1MiB/store4MiB/Core128MiB unchanged; B0/B2 remain UNKNOWN. Mac Deferred never PASS.

No SP1 generator/index/journal/native BQ/issuer/credential implementation or authentic
principal/anchor/key/source/native realm is authorized by this proposal. NEW DIFFERENT
ENTIRE architecture review of all ten sections/appendix/all five mappings/complete
current dirty+committed source and cumulative originals precedes dependent source;
actual independent human authority selection remains separate. Ordinary approved
PA1/N2 source work continues within existing PR42/issue39 ownership. Full positive
production engine/library/SEA/native owner/writer/observer/archive/TC01..12/CA01..08
remains unfinished, then DIFFERENT ENTIRE final SOURCE GO/NEW complete input admission
before any build/import/parser/compiler/test/native calibration. No CI rerun/cancel,
settings/main/cleanup/release/GA/promotion/deployment or partial delivery claim.