# Proposed protection settings for CLI development candidates

This is Issue #30 review material. It is not an instruction to change GitHub
settings and does not assert that any setting is currently enabled.

| Control | Proposed setting | Why the publisher depends on it |
| --- | --- | --- |
| `develop` branch | Require pull requests, one approving review, up-to-date required checks, and block force pushes/direct pushes. Restrict workflow dispatch to reviewed `develop`. | A manual candidate must name a reviewed integration SHA. |
| `development-candidate` environment | Required reviewers: release owners; prevent self-review; deployment branches: `develop` only; environment secret: `DEVELOPMENT_CANDIDATE_TOKEN` with repository `contents:write` only. | The publisher needs an explicit human gate and a narrowly scoped token. |
| GitHub Actions | Default `contents: read`; only the publish job receives `contents: write`; pin action references to reviewed major/version policy; retain build artifacts long enough for review. | Build/test jobs must not be able to write tags or releases. |
| Releases and tags | Protect `cli-v*-candidate-*`; allow creation only through the approved publisher identity; disallow mutable/recreated release assets operationally. | Exact collisions can be read back safely, while every partial or mismatched collision fails closed. |
| Repository ruleset | Require the candidate workflow's exact-head checks before `develop` merge, and require linear, reviewable history. | A frozen SHA is meaningful only when its source review and checks are attributable. |

The operational approval must verify these settings directly in GitHub before
dispatching. Source tests only verify that the publisher rejects missing or
inconsistent preflight assertions supplied by GitHub's API.
