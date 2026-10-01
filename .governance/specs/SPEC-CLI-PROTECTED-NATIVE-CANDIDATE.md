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
| CLI30-CLOSED-INVENTORY | A schema-closed manifest names exactly the portable archive, three native archives, their provenance records and `SHA256SUMS.txt`; every declared digest is recomputed before upload and from final public downloads. The verifier preserves verified `Buffer` objects rather than later paths. It raw-tokenizes all trusted JSON to reject duplicate keys before parsing; it bounds asset bytes before allocation; and it accepts only gzip-tar archives with a documented finite inventory. Portable archives must contain only the npm `package/` payload allowed by this contract and expose the declared package/version/command in the actual archive. Each native archive must contain exactly its target executable, matching provenance, schema-closed CI context, and a schema-closed no-Node host-acceptance record whose digest is calculated from fixed identity fields rather than supplied prose. Tar and gzip parsing rejects concatenation, prefixes, suffixes, gaps, links, unsafe PAX, traversal, normalized-name collisions, hostile compression metadata, and resource-limit breaches. | Valid producer fixtures and adversarial byte fixtures cover duplicate JSON keys, archive structure, checksums, platform/event/version identity, bounded resources, and retained-byte path replacement. |
| CLI30-IMMUTABILITY | Before every write, the publisher obtains GitHub's immutable-releases setting, `develop` protection, a configured pull-request review policy, administrator enforcement, nonempty strict (up-to-date) checks, blocked direct/force pushes, and the `development-candidate` environment's actual reviewed `develop`-only branch policy. It validates configured policy shape without inventing reviewer identities or approval counts. Existing state is accepted only when it is a complete immutable exact collision whose annotated tag recursively dereferences to the frozen commit; no overwrite, delete, recreate or retag is available. | Fixed-endpoint adapter tests cover unavailable provider permission, each required policy field, incomplete/mismatched collisions and lightweight/annotated tag dereferencing. |
| CLI30-SECRET-SAFE-READBACK | Credentials remain environment-only and never enter argv, raw response/error output or public-download requests. API metadata/uploads use fixed GitHub endpoints. While a release is private, every held byte is fetched through its authenticated fixed release-asset ID endpoint, with any redirect followed headerlessly only to a constrained host. Only complete private inventory and byte verification permits the single draft-to-published transition. Public readback uses canonical GitHub HTTPS URLs headerlessly. Every metadata and asset response is duplicate-key checked, streamed under finite byte limits and a fixed request deadline; redirects are constrained before a follow-up. Accepted bytes are retained from verification through upload/readback. | The actual injectable publisher adapter tests preflight-before-write ordering, policy negatives, exact collisions, private asset-ID reads, single publish transition, canonical public reads, duplicate JSON, bounded/stalled/redirected responses, secret sentinel handling, and held-byte replacement. |

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

The archive verifier accepts a native `.tar.gz` only when its exact four-member
inventory is the target executable, `provenance.json`, `ci-context.json`, and
`host-acceptance.json`. `ci-context.json` has schema version 1 and contains
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
