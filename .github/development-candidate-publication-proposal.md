# Proposed protection settings for CLI development candidates

This is Issue #30 review material. It is not an instruction to change GitHub
settings and does not assert that any setting is currently enabled.

| Control | Proposed setting | Why the publisher depends on it |
| --- | --- | --- |
| `develop` branch | Require pull requests with the repository's approved review policy with at least one approving review (the current source minimum), a schema-complete empty pull-request bypass allowance (`users`, `teams`, and `apps`), nonempty strict (up-to-date) required checks, administrator enforcement, and blocked force/direct pushes. Restrict workflow dispatch to reviewed `develop`. | A manual candidate must name a reviewed integration SHA with the existing source minimum of one approving review or accepting an actor permitted to bypass pull requests. |
| `development-candidate` environment | Required release-owner reviewers using GitHub `User` or `Team` identities; prevent self-review; deployment branches: custom policy containing exactly `develop`; owner-provisioned `DEVELOPMENT_CANDIDATE_TOKEN` whose documented repository permissions match the read/write contract below. Owner approval must also explicitly choose the environment UI's administrator-bypass setting. | The publisher queries the environment and its branch-policy endpoint. It refuses a missing `required_reviewers` rule, `prevent_self_review: true`, identifiable reviewer IDs, or any policy other than exactly `develop`. GitHub's documented environment GET response exposes those fields but does not expose the administrator-bypass UI choice, so source must not claim or invent that operational control. |
| Immutable releases | Enable GitHub immutable releases. | The publisher performs `GET /repos/service-lasso/service-lasso-cli/immutable-releases` and requires the provider response to state `enabled: true`; an environment variable is not evidence. |
| GitHub Actions | Default `contents: read`; the publisher receives the separately owner-provisioned credential described below; use the reviewed full-commit action pins below; retain build artifacts long enough for review. | Build/test jobs must not be able to write tags or releases. An environment secret or workflow permission declaration is not proof that the credential can perform the mandatory policy reads. |
| Releases and tags | Protect `cli-v*-candidate-*`; allow creation only through the approved publisher identity; disallow mutable/recreated release assets operationally. | Exact collisions can be read back safely, while every partial or mismatched collision fails closed. |
| Repository ruleset | Require the candidate workflow's exact-head checks before `develop` merge, and require linear, reviewable history. | A frozen SHA is meaningful only when its source review and checks are attributable. |

## Publisher credential contract

The publisher's fixed endpoint set requires one owner-reviewed, repository-scoped
credential with **Contents: write**, **Actions: read**, and **Administration:
read**. These are distinct needs: release/tag/asset reads and writes use Contents;
the immutable-release and branch-protection reads use Administration; environment
and deployment-branch-policy reads use Actions. The publisher has no setting-write
endpoints. This proposal does not request a provider change or infer capability
from an environment declaration; the owner must provision and review the actual
credential separately.

| Mandatory endpoint class | Permission required | Publisher behavior when unavailable |
| --- | --- | --- |
| `GET /repos/{owner}/{repo}/immutable-releases` and `GET /branches/develop/protection` | Administration: read | A `401`, `403`, `404`, or non-`200` preflight result fails before tag, release, asset, or publish writes. |
| `GET /environments/development-candidate` and `GET /deployment-branch-policies` | Actions: read | A `401`, `403`, `404`, or non-`200` preflight result fails before every write. |
| Release, annotated-tag, and release-asset reads | Contents: read (included by Contents: write) | Collision and readback state is accepted only when complete and exact. |
| Annotated-tag/reference creation, private release/assets, and the one publish transition | Contents: write | These writes are reachable only after all mandatory reads and private byte verification succeed. |

GitHub documents Administration: read for [branch protection](https://docs.github.com/en/rest/branches/branch-protection#get-branch-protection)
and [immutable-release settings](https://docs.github.com/en/rest/repos/repos#check-if-immutable-releases-are-enabled-for-a-repository),
and Actions: read for [environment reads](https://docs.github.com/en/rest/deployments/environments#get-an-environment).
`tests/protected-candidate-publisher.test.js` drives the production adapter
through `401` and `403` failures for every mandatory preflight endpoint and
asserts zero writes; it is source evidence of fail-closed behavior, not proof
of a token's live grants.

The operational approval must verify these settings directly in GitHub before
dispatching: required status checks must be nonempty and strict (up-to-date),
the configured pull-request review policy and administrator enforcement must be
present, the bypass allowance must be schema-complete and empty, force/direct
pushes must be disabled, the branch must remain protected, and the environment's
administrator-bypass choice must be recorded separately in the UI. Source
tests only verify that the publisher rejects missing or inconsistent provider
responses. The reviewed action pins are `actions/checkout@08c6903cd8c0fde910a37f88322edcfb5dd907a8`,
`actions/setup-node@a0853c24544627f65ddf259abe73b1d18a591444`,
`actions/upload-artifact@ea165f8d65b6e75b540449e92b4886f43607fa02`, and
`actions/download-artifact@d3f86a106a0bac45b974a628896c90dbdf5c8093`.

The publisher rereads all five mandatory policy endpoints immediately before each tag object, reference, private draft, individual asset upload and final publish write. A policy change or unavailable reread leaves prior private state retained and stops all remaining writes. This source contract does not approve settings or credential provisioning.
