# Protected native development candidate

## Scope and mode

Issue #30 is a **Development** source-preparation slice. It prepares the
manual `develop` candidate workflow for one frozen full-SHA candidate containing
the existing portable Node archive and native executable archives for Windows
x64, Linux x64 and macOS arm64. It does not execute a dispatch, alter GitHub
settings, create or modify tags/releases/assets, publish packages, deploy, or
claim Core packaging, release qualification, or GA.

## Requirements

| ID | Requirement | Required source evidence |
| --- | --- | --- |
| CLI30-IDENTITY | Candidate version, governed tag, portable package identity, all archive metadata and all provenance records bind the same full 40-character source SHA. The executable itself reports the candidate version and provenance identity. Event/PR context is recorded separately and cannot replace source identity. | Adapter tests reject wrong SHA, version, platform, architecture and forged provenance. |
| CLI30-FROZEN-BYTES | One build produces the portable archive and each target host produces its native archive once. Smoke and publisher jobs consume the uploaded bytes and do not rebuild, retag or mutate accepted assets. | Workflow dependency graph plus actual manifest verifier tests. |
| CLI30-TARGET-PROOF | Windows x64, Linux x64 and macOS arm64 directly execute their matching native binary after `node` is absent from the child PATH. Fixture proof and direct Core proof have distinct records. | Workflow steps invoke the existing no-Node native smoke; Core proof remains separately named. |
| CLI30-CLOSED-INVENTORY | A schema-closed manifest names exactly the portable archive, three native archives, their provenance records and `SHA256SUMS.txt`; every declared digest is recomputed before upload and from final public downloads. The verifier preserves verified `Buffer` objects rather than later paths. It raw-tokenizes all trusted JSON to reject duplicate keys before parsing; it bounds asset bytes before allocation; and it accepts only gzip-tar archives with a documented finite inventory. Each native archive contains its target SEA executable, the target-host compiled `service-lasso-confined-scaffold` helper, matching provenance including both binary digests and the helper-source digest, schema-closed CI context, and a schema-closed no-Node host-acceptance record whose digest is calculated from fixed identity fields rather than supplied prose. Tar and gzip parsing rejects concatenation, prefixes, suffixes, gaps, links, unsafe PAX, traversal, normalized-name collisions, hostile compression metadata, and resource-limit breaches. | Valid producer fixtures and adversarial byte fixtures cover duplicate JSON keys, archive structure, checksums, platform/event/version identity, helper-byte replacement, bounded resources, and retained-byte path replacement. |
| CLI30-IMMUTABILITY | Before every write, the publisher obtains GitHub's immutable-releases setting, `develop` protection, a configured pull-request review policy with schema-complete empty `users`/`teams`/`apps` bypass allowances, administrator enforcement, nonempty strict (up-to-date) checks, blocked direct/force pushes, and the `development-candidate` environment's actual reviewed `develop`-only branch policy. The owner-selected automated development policy accepts a schema-valid approving review count from 0 through 6 (0 for the current no-human-approver decision), while retaining the configured pull-request requirement and empty bypass allowance. The environment must have a GitHub wait-timer rule with an integer 1 through 30 minutes and the exact develop-only custom branch policy; no human environment reviewer is mandatory. The proposed wait is 1 minute. Source does not infer an administrator-bypass field that the read response does not expose. The owner proposal maps immutable/branch-protection reads to Administration: read, environment/policy reads to Actions: read, and tag/release operations to Contents: write; unavailable preflight is zero-write. Existing state is accepted only when it is a complete immutable exact collision whose annotated tag recursively dereferences to the frozen commit; no overwrite, delete, recreate or retag is available. | Fixed-endpoint adapter tests cover `401`/`403` and other unavailable provider reads before writes, every required policy field and bypass shape, wait-timer bounds, valid zero approval count, incomplete/mismatched collisions and lightweight/annotated tag dereferencing. |
| CLI30-SECRET-SAFE-READBACK | Credentials remain environment-only and never enter argv, raw response/error output or public-download requests. API metadata/uploads use fixed GitHub endpoints. The 201 creation response must already name the exact positive release ID, frozen tag/source, private draft/prerelease state and an empty asset array before any upload. While private, every held byte is fetched through its authenticated fixed release-asset ID endpoint, with an explicit redirect result whose nonempty body is discarded without awaiting disposal and whose constrained follow-up is headerless. Only complete private inventory and byte verification permits the single draft-to-published transition. Public readback uses canonical GitHub HTTPS URLs headerlessly. Every metadata and asset response is duplicate-key checked, reads only validated `Uint8Array` chunks, checks the prospective cap before `Buffer` copying, and uses a fixed request deadline; redirects are constrained before a follow-up. Accepted bytes are retained from verification through upload/readback. | The actual injectable publisher adapter tests preflight-before-write ordering, provider-policy negatives, malformed created-private responses with zero upload/publish writes, exact collisions, private asset-ID reads, one publish transition, canonical public reads, duplicate JSON, no-copy bounded/stalled/redirected responses including nonsettling/throwing redirect disposal, secret sentinel handling, and held-byte replacement. |

## Non-goals and dependencies

Issue #26 native packaging and its target-host proof are dependencies. The
portable candidate format remains the existing Core packaging contract and is
not modified here. Issue #29's event-binding repair must first merge into
`develop` through its own review; this issue is not stacked on it.

The proposed provider controls are review material in
`.github/development-candidate-publication-proposal.md`. Owner approval and a
separately authorised operational run are required before any provider mutation
or publication claim.

### Native archive context contract

The archive verifier accepts a native `.tar.gz` only when its exact five-member (Windows/Linux) or six-member (Darwin)
inventory is the target executable, the matching target-host compiled
`service-lasso-confined-scaffold` helper, `provenance.json`, `ci-context.json`,
and `host-acceptance.json`, plus Darwin-only `service-lasso-darwin-immutable-helper`. The verifier recomputes its digest from actual archived bytes. The provenance record binds both executable bytes,
helper bytes, helper source digest, platform, and architecture. `ci-context.json` has schema version 1 and contains
only event name, source SHA, nullable tested-base SHA, and merge-context SHA.
It permits only `push`, `pull_request`, or `workflow_dispatch`; a push or
dispatch has a null base and source-equal merge context, while a pull request
has three distinct full SHAs. `host-acceptance.json` has schema version 1 and
contains only the source, candidate version, target platform/architecture,
executable SHA-256, `nodeAbsentFromPath: true`, `status: "passed"`, and an
`evidenceDigest`. That digest is the SHA-256 of the verifier-defined canonical
identity string. These files are archive members, not unverified sidecar paths.

The candidate workflow is the producer for these records. Each native target
writes a schema-closed `ci-context.json` for the actual `workflow_dispatch`
event (null tested base and source-equal merge context), then emits
`host-acceptance.json` only after its matching no-Node fixture journey has
succeeded. It archives and uploads those same accepted bytes without rebuilding.
The Linux Core journey remains separately named and does not turn the fixture
record into Core proof. Source tests validate the producer and its meaningful
failure paths; a terminal workflow-dispatch run remains the required hosted
acceptance evidence.

## Traceability

- `CLI30-IDENTITY`, `CLI30-FROZEN-BYTES`, `CLI30-CLOSED-INVENTORY`,
  `CLI30-IMMUTABILITY`, and `CLI30-SECRET-SAFE-READBACK` map to
  `tests/protected-candidate-publisher.test.js`, which drives the production
  publisher through an injected fixed-endpoint adapter rather than testing
  helpers in isolation.
- `CLI30-TARGET-PROOF` maps to the target-host native smoke, context and
  acceptance producer steps, plus `tests/native-distribution.test.js` and
  `tests/distribution.test.js`.
- The context and acceptance producer contract maps to
  `tests/native-distribution.test.js` and
  `tests/protected-candidate-publisher.test.js`; it is source test evidence,
  not hosted CI acceptance or candidate publication evidence.

## Issue30 coherent repair continuation

Immediately before each tag-object, tag-reference, draft-create, asset-upload and publish mutation, reread all five approved provider policy endpoints and validate the complete contract. A changed or denied policy stops all remaining mutations and retains existing private state. Actual producer archives and every mutation boundary require regressions. Core #1534 staged-service Windows ZIP and Core outer release Windows ZIP remain full programme scope; #35 corrects the former CLI inner-ZIP attribution. All new regressions are UNEXECUTED until entire independent final-head SOURCE GO and new ROOT complete-input admission.

Issue #30 all-three entire-review repair maps CLI30-TARGET-PROOF to zero-exit selected-test closure plus a separately digest-bound unavailable inner-route record; CLI30-IDENTITY to caller-bound push/PR/dispatch archive contracts with strict dispatch publication; and CLI30-CLOSED-INVENTORY to bounded regular held-handle local reads including manifest and sums. All regressions remain unexecuted pending new entire source review and ROOT admission.

Issue #30 natural-attempt additions: Windows phase wrappers invoke literal Node scripts without shell forwarding. Darwin root-owned helper/grant/capability transport is an external qualification dependency; hosted execution is explicitly blocked and retains a prerequisite record, while secure native checks and actual tests remain mandatory. The historical exact Core pin remains unchanged and blocked by its own parse failure until independently qualified governed replacement. Original attempt 37003590854 and all logs remain retained; no new execution is authorised.

Issue #30 canonical Darwin grant repair maps CLI30-TARGET-PROOF to the fixed /private/var/db/service-lasso grant and strict no-symlink root-owned ancestor chain (including / and /private). Actual Darwin helper grant-read and fail-closed ancestor/file/capability/object regressions are prepared, UNEXECUTED. Host-owner grant provisioning/descriptor transport remains UNPERFORMED; independently qualified Core replacement remains PENDING. Entire new-head review and NEW ROOT complete-input admission precede execution.

Issue #30 final000b natural-source repair maps CLI30-CLOSED-INVENTORY to ambient tar using an owned output cwd and a fixed relative local archive name (Windows drive/spaced actual producer regression); CLI30-TARGET-PROOF to durable raw phase closure plus explicit present/absent/error digest outcomes and fail-closed missing/error records, and to test-only NODE_TEST_CONTEXT removal preserving all runtime/security variables and literal selected-runner argv. Actual positive inner-route and actual assertion-negative regressions stay mandatory. Darwin ordinary positive tests and hosted native gates require the owner admission contract in docs/native-qualification-prerequisites.md; actual per-object coordination/descriptor transport authority remains unresolved and external. No skip, synthetic grant or blocked-as-pass is permitted. Preserve canonical strict grant/helper/root protections, all earlier publisher fixes and Core9bef pending qualified replacement. Issue #35 explicitly retires the misplaced CLI inner-ZIP assertion; Core #1534 staged-service Windows ZIP and outer release Windows ZIP remain mandatory. New entire independent source review and NEW complete-input ROOT admission precede ALL execution.
Issue #30 qualification-only dynamic-owner seam: add a private inherited Unix owner channel, bounded schema-closed object request/ack/completion protocol with root-peer authentication, actual SCM_RIGHTS held descriptor/dev/inode/owner/type/source digest binding and per-operation serialization. Each acknowledgement coordinates the existing canonical one-object grant and replenishes exactly32 capability bytes before the unchanged restricted sudo helper invocation; it never grants general flags authority or enrolls a caller object. Add explicit Darwin inherited-FD mapping to actual wrapper/smoke/native test callers and denial/transport regression source. The externally admitted ephemeral owner endpoint must independently approve membership, acquire root grant custody, serialize cross-primary operations, and retain raw native/helper/readback/recovery receipts. Its privilege-bearing implementation and Node test-worker FD delivery remain external dependencies until an exact owner profile is approved. All existing mandatory positive assertions and blocked aggregate remain; source seam is not an operational broker or admission.
Issue #30 final010 two-finding repair maps CLI30-TARGET-PROOF to the actual package-native staged Darwin source inventory (including darwin_owner.go) and borrowed descriptor lifetime: each helper operation duplicates held image/parent and capability descriptors, marks the duplicates close-on-exec, and deterministically closes only those duplicates after child closure. The shared parent directory has one retained file owner through set/clear/recovery. Regression source exercises the real staging function plus compile, repeated helper-command set/clear labels with a finite replenished pipe under GC, failed launch and descriptor reuse. No privilege, grant, owner membership, immutable assertion or mandatory native gate is relaxed. All regressions are UNEXECUTED; independent ENTIRE new-head review and NEW complete-input ROOT admission remain required before execution.
## Issue #35 coordinated release-contract errata

Active SPEC-CLI-SCOPED-RELEASE-ERRATA CLI35-ARCHIVE-ORIGIN/CLI35-TEMPLATE-ROLE maps Core #1628 SPEC-008 R2/R4/R5/T1–T5/R6/R7: selected protected2/portable2 native TAR 6/7/8; explicit retirement of agent-added CLI inner-ZIP blocker with Core staged-service/outer ZIP retained; fixed CLI/Core roles preserving all TC01–12/CA01–08, exact failure grammar and prospective source-owned admission/proof paths. Existing source and failed evidence remain historical, never pass. Actual source catalog/owner publication, native/helper/primary/Core/operator proof and separate complete Core admission architecture remain blocked. Darwin deferred never PASS. Four-repo source-only bundle requires fresh distinct ENTIRE cumulative amendment review and develop landing before held producer implementation; NEW complete-input admission precedes later execution. No tests/build/native/parser/provider settings/publication executed here.

## Issue #39 October16 source continuation

Sole delivery owner /root/cli39_release_delivery_oct16 works from current develop
 df5c1b62d64061d3d8117aa998e96597fcd5834b in the policy-owned
39-development-release-oct16 checkout. Retained legacy native/scripts/src/tests
source is transferred under CLI39-RETAINED-SOURCE-TRANSFER-OCT16.sha256; the
original mixed index and checkout remain untouched. This restores missing source
dependencies; it does not qualify, freeze or complete the implementation.

The selected prebirth slot3 method now has a once-only native F_GETFD observation
and returning replay. Exact -EBADF permits absence only when no retained known
source occurrence owns3; present3 must match the actual retained custody,
self-pidfd or opened-file occurrence. Foreign3 is refused without close. Clone
chronology consumes the actual probe afterclock. Parent maximum is3*(N0+N1)+14,
and the child has ten distinct raw rows. All source remains UNRUN. Genuine
initial ROOT/source/profile, clone/child-local acquisition, full Windows/Linux
Node/default actions and the entire remaining sequence remain required.

CI38563583179/job115850523828 fails the pinned Core9bef build at
src/server/index.ts(9179,1), TS1005 '}' expected, before CLI acceptance. No repin
is justified by latest HEAD alone. Complete independent source review and NEW
exact authentic ROOT/input/command admission precede local product execution;
natural new-push CI remains separately allowed. Protected Development publication
and same-byte Core integration remain incomplete; macOS N/A, never PASS.

### October16 actual stopped-child producer source

The next owning body consumes the fresh prepared roster and actual slot3 probe,
then performs once clone3(CLONE_PIDFD,SIGCHLD) with no shared VM/FD table. Parent
pidfd output is retained parent-locally; child's raw0 is a separate shared native
observation. Child uses ten fixed rows for TRACEME/selfpid/selftid/STOP/F_GETFD3,
conditional known-owned close3/dup3 or SAME custody3 F_SETFD, held execveat and
cleanup-only exit_group127. It reobserves its own3 and compares the selected
prebirth branch; parent numeric flags never become child-native observations.
Successful exec has only called/before, no invented return or afterclock. Failed
useful steps set failed before once cleanup exit. Unexpected cleanup return keeps
raw/returned/trap-attempted; the subsequent fixed trap is FAILED/UNKNOWN, never
termination or borrower-release proof. No signal disposition or spin is invented.

Clone/child/observer source construction supplies no authentic ROOT/source/profile
association. Complete native descriptor/private stack/loader/control/failed-prefix
precharge and genuine preceding reservation still must be acquired by the fixed
source caller before activation. All source UNRUN; whole fit UNKNOWN. The entire
remaining sequence, independent review, new ROOT admission and protected release
acceptance gates remain unchanged.

October16 returning closure: actual parent completion additionally replays every
child-owned successful-prefix row against SAME clone PID/TID, post-CONT STOP
return, source-selected3 branch and held exec inputs; unused branch/exit rows stay
fresh. Successful exec remains called-only with no returned/raw/postclock, joined
to the independently observed actual exec event. This completes raw stopped-child
mechanics only. Actual postexec child-local acquisition and genuine initial
source/principal/reservation/profile constructor remain absent. No current source
freeze, wholeGO, execution, qualification or release claim follows.

### October16 exec-stop mapping acquisition source

Before source activation, the fixed parent derives /proc/<SAME actual clone PID>/maps
using checked decimal PID geometry, opens once readonly/CLOEXEC/NOFOLLOW and retains
native fstat/getfl/getfd and complete bounded short-read/EOF originals. Complete
source-selected output capacity B has checked B+5 row maximum: four fixed native
calls, at most B positive-short read calls and one EOF/overflow probe. Each row has
two original clock calls, plus one final clock. Buffer-full always probes one extra
byte and refuses actual non-EOF; no truncation-as-complete. All capture/buffer/row
storage excludes SAME birth/wait/control and complete borrowed graph before writes.
Native outputs/descriptor survive errors/expiry. No close, seek, resume or loaded
source grant follows. Proc namespace/mount/kernel/source and complete held-loaded/
interpreter/writer/private-stack graph remain independently required; maps bytes,
path or successful capture are only retained observations. All source UNRUN.

### October16 full source-runtime builder continuation

The active implementation frontier is the actual full Node source builder.
Windows generation and compilation become distinct fixed original vcbuild phases:
release x64 full-icu vs2022 projgen nobuild, retained generated config.gypi and
used configure flags, then release x64 full-icu vs2022 noprojgen. Generated actual
JSON variables must preserve x64/nonshared/static CRT/full ICU/bundled V8/default
platform/snapshot/codecache/WebAssembly/inspector/lite-mode exclusion before the
compiler phase; post-build held readback must equal that complete original config.
The fixed upstream configure source emits JSON after one literal generated header,
so no Python eval or regex-as-config substitution is selected. Complete generated
MSBuild/runtime library/link ancestry and tool/native-owner/loaded closure remain
required. Exact source-derived command/capture geometry is checked before effects.
No producerComplete gate is waived by partial repairs. All source UNRUN; actual
complete Windows/Linux recipe, genuine member/tool inputs, whole source review and
NEW exact ROOT/commands still precede execution. No older binary/fake manifest or
reduced runtime route is accepted.


Issue39 private native build-owner constructor/effect/cost/alias/last-borrower contract is native/build-owner/CONTRACT.md. CLI owns the real factory; current Core types are not a supplied authority. Entire fixed descendant graph and Windows/Linux actual source/loaded/principal/reservation/stream/native terminal conjunction remain mandatory. No new public six-action slot or JSON issuer. Source-only UNRUN; producer guard remains pending complete coherent implementation and fresh whole review/ROOT admission.

CLI39 private source-builder bridge pre-code selection is docs/decisions/CLI39-FIXED-SOURCE-BUILDER-BRIDGE-OCT16.md: proposed fixed Node-API build-plane addon, complete source-owned import/CRT/Node schedule and genuine private native constructor; no public command callback/JSON issuer. New ABI/loading selection awaits parent intent review before adapter implementation. Both OS factories, loaded/stream/financing/ROOT joins remain incomplete; source guard unchanged and all target execution UNRUN.

CLI39 implements the already approved CLI35-ARCHIVE-ORIGIN/R1-R7 scoped release source unit: separate no-input release-scoped.yml; exact source-pinned scope policy; portable2/protected2 six assets, seven ASCII checksum rows and eight files; unchanged native1 grammar and retained protected1/all3 paths. Scoped producer/verifier/publisher/readback and meaningful denial/byte/provider regressions are one coherent review unit. Native full builder, authentic ROOT/profile, actual operator/native/same-byte qualification and provider credential/settings remain separate unmet gates. No scoped execution or publication before complete independent exact-head source review and NEW admission.

CLI39 confirmed scoped tag-binding race repair: scripts/scoped-tag-policy.mjs and publish-protected-candidate-scoped.mjs require current complete effective inherited tag rules, active source-pattern update/deletion restrictions with no bypass, exact retained annotated OID/source joins at mutation boundaries and independent immutable/ref readback after publication. Creation-only publisher restrictions must remain separate. Structural regressions cover weakened policy and ref replacement after creation/during uploads/after publication. Current protected-control custody remains; sequential reads do not claim atomic administrator exclusion. No settings changed; all local target execution remains UNRUN pending fresh ENTIRE review/NEW exact admission. Native bootstrap A/B and full positive qualification remain implementation debt.
CLI39 connected build-bootstrap construction source is native/build-owner/bootstrap_windows.h/c under CONTRACT.md: independently admitted seed A initial source/profile/loaded/principal occurrence precedes the private native body; exact actual GetSystemInfo/VirtualQuery initial prefix rows initialize the SAME finite ledger, actual platform_memory acquisition/arena adoption carves native outputs, and effective/process-primary constructors feed actual canonical compiler ancestor/full-byte/EOF acquisition. Prefix guards/reservations and failed raw results remain charged/retained. This is first compiler acquisition correspondence, not a complete seed/addon/full-runtime builder or qualification. Fixed seed compile/load ownership, full synchronous schedule/closure and Linux peer remain active implementation debt; producerComplete remains false. No local target execution, operator admission, numeric fit or publication is claimed; fresh ENTIRE review/NEW complete input admission remains required.
CLI39 bootstrap compiler is now consumed by native/build-owner/seed_compile_windows.h/c: the private SAME-arena fixed caller derives the C11 bootstrap_windows.c compile command, preowns command/attribute/debug-image capture backing and connects held compiler → actual native root birth → child principal → initial debug image-file join. It accepts no command callback or arbitrary flags and preserves the pending issuing-thread caller contract. This is a stopped real compile-prefix, not natural compiler completion or seed/addon load. Complete input/include/library/source/exclusion joins remain required before invoking it; complete PE/import/event/descendant/stream closure, independently admitted A seed recipe/load, full runtime schedule and Linux peer are still unfinished. Bootstrap failure now makes both containing owner and RAM ledger absorbing. Source remains locally UNRUN and producerComplete=false; no qualification/publication claim.
CLI39 seed compile now preowns source-derived remote PE64 geometry/body/page-row backing before actual tool birth, then consumes the actual stopped debug root in remote_image_windows.c: GetCurrentThreadId, complete bounded VirtualQueryEx region traversal and explicit ReadProcessMemory/raw short results, complete observed PE header correspondence, separate reserved/unreadable spans and unchanged deadline. Pure structural geometry vectors exercise the real helper's bounds/machine/section/alignment failures without a fabricated native owner or executable qualification. These vectors are source UNRUN. Relocation/IAT/export/loader/source ancestry, full event/descendant/stream closure and genuine seed/addon/runtime execution remain unfinished; no ContinueDebugEvent, source grant, positive receipt or guard change follows. All local target execution still requires fresh ENTIRE review and NEW exact ROOT/input admission.