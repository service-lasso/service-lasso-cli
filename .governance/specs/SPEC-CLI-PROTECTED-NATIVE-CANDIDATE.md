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
