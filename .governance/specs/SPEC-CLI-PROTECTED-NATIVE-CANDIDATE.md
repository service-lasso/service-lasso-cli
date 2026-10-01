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
| CLI30-IMMUTABILITY | Before a write, the publisher obtains GitHub's immutable-releases setting, `develop` protection, documented required-check/up-to-date/no-force safeguards, and the `development-candidate` environment's actual `develop`-only branch policy. Existing state is accepted only when it is a complete immutable exact collision whose annotated tag recursively dereferences to the frozen commit; no overwrite, delete, recreate or retag is available. | Fixed-endpoint adapter tests cover unavailable provider permission, branch policy, incomplete/mismatched collisions and lightweight/annotated tag dereferencing. |
| CLI30-SECRET-SAFE-READBACK | Credentials remain environment-only and never enter argv, raw response/error output or public-download requests. API metadata/uploads use fixed GitHub endpoints; public download URLs must be canonical GitHub HTTPS asset URLs, redirects are accepted only to fixed GitHub hosts, and accepted bytes are retained from verification through upload/readback. | Tests use a secret sentinel, hostile URL/redirect fixtures and a retained-bytes race. |

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

This verifier-only increment defines and tests the contract. The current
candidate workflow does not yet produce the two required records before calling
the archive tool. The archive tool now refuses to create a closed native archive
without them; making hosted jobs emit and consume them is a later
workflow-acceptance increment and is not claimed here.

## Traceability

- `CLI30-IDENTITY`, `CLI30-FROZEN-BYTES`, `CLI30-CLOSED-INVENTORY`,
  `CLI30-IMMUTABILITY`, and `CLI30-SECRET-SAFE-READBACK` map to
  `tests/protected-candidate-publisher.test.js`.
- `CLI30-TARGET-PROOF` maps to the target-host native smoke workflow steps and
  the existing `tests/native-distribution.test.js` proof boundary.
- This verifier-only context contract maps to
  `tests/protected-candidate-publisher.test.js`; it is source test evidence,
  not hosted CI acceptance or candidate publication evidence.
