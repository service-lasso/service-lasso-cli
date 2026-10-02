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
| CLI30-IMMUTABILITY | Before every write, the publisher obtains GitHub's immutable-releases setting, `develop` protection, a configured pull-request review policy with schema-complete empty `users`/`teams`/`apps` bypass allowances, administrator enforcement, nonempty strict (up-to-date) checks, blocked direct/force pushes, and the `development-candidate` environment's actual reviewed `develop`-only branch policy. The documented required-reviewer rule must set `prevent_self_review: true` and contain identifiable `User` or `Team` reviewer IDs; source does not invent an approval count or an environment bypass field that the read response does not expose. The owner proposal maps immutable/branch-protection reads to Administration: read, environment/policy reads to Actions: read, and tag/release operations to Contents: write; unavailable preflight is zero-write. Existing state is accepted only when it is a complete immutable exact collision whose annotated tag recursively dereferences to the frozen commit; no overwrite, delete, recreate or retag is available. | Fixed-endpoint adapter tests cover `401`/`403` and other unavailable provider reads before writes, every required policy field and bypass shape, reviewer shape, incomplete/mismatched collisions and lightweight/annotated tag dereferencing. |
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

Immediately before each tag-object, tag-reference, draft-create, asset-upload and publish mutation, reread all five approved provider policy endpoints and validate the complete contract. A changed or denied policy stops all remaining mutations and retains existing private state. Actual producer archives and every mutation boundary require regressions. Windows ZIP remains full programme scope. All new regressions are UNEXECUTED until entire independent final-head SOURCE GO and new ROOT complete-input admission.

Issue #30 all-three entire-review repair maps CLI30-TARGET-PROOF to zero-exit selected-test closure plus a separately digest-bound unavailable inner-route record; CLI30-IDENTITY to caller-bound push/PR/dispatch archive contracts with strict dispatch publication; and CLI30-CLOSED-INVENTORY to bounded regular held-handle local reads including manifest and sums. All regressions remain unexecuted pending new entire source review and ROOT admission.

Issue #30 natural-attempt additions: Windows phase wrappers invoke literal Node scripts without shell forwarding. Darwin root-owned helper/grant/capability transport is an external qualification dependency; hosted execution is explicitly blocked and retains a prerequisite record, while secure native checks and actual tests remain mandatory. The historical exact Core pin remains unchanged and blocked by its own parse failure until independently qualified governed replacement. Original attempt 37003590854 and all logs remain retained; no new execution is authorised.

Issue #30 canonical Darwin grant repair maps CLI30-TARGET-PROOF to the fixed /private/var/db/service-lasso grant and strict no-symlink root-owned ancestor chain (including / and /private). Actual Darwin helper grant-read and fail-closed ancestor/file/capability/object regressions are prepared, UNEXECUTED. Host-owner grant provisioning/descriptor transport remains UNPERFORMED; independently qualified Core replacement remains PENDING. Entire new-head review and NEW ROOT complete-input admission precede execution.
