# Proposed protection settings for CLI development candidates

This is Issue #30 review material. It is not an instruction to change GitHub
settings and does not assert that any setting is currently enabled.

| Control | Proposed setting | Why the publisher depends on it |
| --- | --- | --- |
| `develop` branch | Require pull requests with the repository's approved review policy, nonempty strict (up-to-date) required checks, administrator enforcement, and blocked force/direct pushes. Restrict workflow dispatch to reviewed `develop`. | A manual candidate must name a reviewed integration SHA without inventing a reviewer-count policy in source. |
| `development-candidate` environment | Required release-owner reviewers using GitHub `User` or `Team` identities; prevent self-review; deployment branches: custom policy containing exactly `develop`; environment secret: `DEVELOPMENT_CANDIDATE_TOKEN` with repository `contents:write` only. Owner approval must also explicitly choose the environment UI's administrator-bypass setting. | The publisher queries the environment and its branch-policy endpoint. It refuses a missing `required_reviewers` rule, `prevent_self_review: true`, identifiable reviewer IDs, or any policy other than exactly `develop`. GitHub's documented environment GET response exposes those fields but does not expose the administrator-bypass UI choice, so source must not claim or invent that operational control. |
| Immutable releases | Enable GitHub immutable releases. | The publisher performs `GET /repos/service-lasso/service-lasso-cli/immutable-releases` and requires the provider response to state `enabled: true`; an environment variable is not evidence. |
| GitHub Actions | Default `contents: read`; only the publish job receives `contents: write`; use the reviewed full-commit action pins below; retain build artifacts long enough for review. | Build/test jobs must not be able to write tags or releases. |
| Releases and tags | Protect `cli-v*-candidate-*`; allow creation only through the approved publisher identity; disallow mutable/recreated release assets operationally. | Exact collisions can be read back safely, while every partial or mismatched collision fails closed. |
| Repository ruleset | Require the candidate workflow's exact-head checks before `develop` merge, and require linear, reviewable history. | A frozen SHA is meaningful only when its source review and checks are attributable. |

The operational approval must verify these settings directly in GitHub before
dispatching: required status checks must be nonempty and strict (up-to-date),
the configured pull-request review policy and administrator enforcement must be
present, force/direct pushes must be disabled, the branch must remain protected,
and the environment's administrator-bypass choice must be recorded. Source
tests only verify that the publisher rejects missing or inconsistent provider
responses. The reviewed action pins are `actions/checkout@08c6903cd8c0fde910a37f88322edcfb5dd907a8`,
`actions/setup-node@a0853c24544627f65ddf259abe73b1d18a591444`,
`actions/upload-artifact@ea165f8d65b6e75b540449e92b4886f43607fa02`, and
`actions/download-artifact@d3f86a106a0bac45b974a628896c90dbdf5c8093`.
