# Current issue #69 Actions publication scope

The owner selects Windows x64/Linux x64 native plus portable schema3 through the existing release.yml. macOS is deferred, not passed. SPEC-CLI-SCOPED-CANDIDATE supersedes historical all-three/custom-token scheduling below for this prospective route. Preserve legacy verifier/admin mode and exact collision recovery.

Operator wrapper must independently GET and verify immutable releases, exact develop SHA, required PR policy including schema-complete empty bypasses/admin enforcement/strict six named checks/no force pushes and the named environment/custom develop-only branch policy/timer1, immediately before dispatch. Bind the exact SHA as operator_preflight_sha. No broad OAuth credential is stored in Actions. The job uses builtin contents:write/actions:read/checks:read token; it does not pretend admin-only reads succeeded. It independently rereads GET branch/source/check enforcement and all six app15368 exact-SHA successful check runs, environment timer and complete custom develop-only policy before every write. Read failures/policy drift fail closed. Root owns separately applying branch protection and dispatch/public receipts after reviewed source merge.

The six exact push check names are: Node 22 CLI checks (ubuntu-latest), Node 22 CLI checks (windows-latest), Native SEA direct execution (Linux x64), Native SEA direct execution (Windows x64), Completion shell checks (ubuntu-latest), Direct real-Core reads (ubuntu-latest). Existing CI emits all on develop push; Darwin results are not selected qualification.

## Historical issue #30 admin-token proposal (retained)

# Automated protected CLI development publication — issue #30

The owner selected no human development release approver and authorised development publication. This proposal records that choice without qualifying any candidate. Parent applies reviewed provider controls; source authors do not mutate settings or credentials. GA, promotion and deployment remain separate owner decisions.

## Required controls

- Protect `develop` with required pull requests and `required_approving_review_count: 0`, schema-complete empty bypass allowances, administrator enforcement, strict current required checks, no force pushes and no branch deletion. Zero human approval does not remove independent source review or technical admission.
- Configure `development-candidate` with `wait_timer: 1`, `reviewers: []`, and custom deployment branch policy containing exactly `develop`. Do not permit administrator bypass. The GET contract does not expose that bypass setting; record its application separately.
- Enable immutable releases. Restrict candidate tag creation/update/deletion to the actually provisioned approved publisher identity using a reviewed tag ruleset. No identity or installation ID is invented here; parent must bind the payload to the real scoped identity before application.
- Keep workflow default permissions `read` and `can_approve_pull_request_reviews: false`. Build jobs retain `contents: read`; existing action full-SHA pins remain. The existing workflow_dispatch is unattended after dispatch through its protected job gates; this source slice does not introduce push publication or broaden triggers.
- Require every current CLI CI context before normal develop merges: `Node 22 CLI checks (windows-latest)`, `Node 22 CLI checks (ubuntu-latest)`, `Node 22 CLI checks (macos-latest)`, `Completion shell checks (ubuntu-latest)`, `Direct real-Core reads (ubuntu-latest)`, `Native SEA direct execution (Windows x64)`, `Native SEA direct execution (Linux x64)`, `Native SEA direct execution (macOS arm64)`. Failing/pending gates must be repaired on the reconciled develop source before qualification/publication. Existing authorised delivery reconciliation does not constitute a technical pass.

## Credential contract and workflow token assessment

The current publisher needs one repository-scoped credential with Contents write, Actions read and Administration read. GitHub documents Administration read for [classic branch protection](https://docs.github.com/en/rest/branches/branch-protection#get-branch-protection) and [immutable release settings](https://docs.github.com/en/rest/repos/repos#check-if-immutable-releases-are-enabled-for-a-repository). Environment reads need [Actions read](https://docs.github.com/en/rest/deployments/environments#get-an-environment).

[Workflow permissions](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax#permissions) do not expose an Administration permission for GITHUB_TOKEN. Giving contents write alone cannot satisfy mandatory Administration reads. Replacing those reads with configuration assertions or ignoring 403/404 would weaken the technical contract, so the source retains DEVELOPMENT_CANDIDATE_TOKEN. A GitHub App installation token or fine-grained token with the three scoped permissions is a possible provisioning route, not an existing credential claim. The current broad authenticated operator OAuth credential must never be retrieved/copied/persisted as a workflow secret. No credential or grant is created by this proposal.

Keep 401/403/404/non200 policy reads zero-write. Reread all five policy endpoints immediately before each tag object/ref, private draft, asset upload and publish mutation. Require exact annotated tag dereference, held-byte closed inventory, authenticated private draft readback, sole publish transition, immutable metadata and headerless public same-byte readback. Sequential reads are not an atomic provider transaction.

## Review and execution boundary

This source only changes the owner-selected human approval policy and its regressions. It retains all mandatory native/Core tests and protected assertions. CLI pinned Core TS1005, Darwin external owner/descriptor and hosted native failures, Windows ZIP programme requirement, TUI macOS producer/native skipped acceptance, and same-byte Core integration remain open. Tests are UNEXECUTED pending a DIFFERENT entire cumulative SOURCE GO and NEW complete-input ROOT admission. Source landing or automatic environment progression is never qualified publication.
