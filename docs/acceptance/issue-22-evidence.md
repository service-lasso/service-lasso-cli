# Issue #22 evidence record

## Observed evidence

| Field | Value |
| --- | --- |
| Record version | `service-lasso-cli.issue-22.evidence.v1` |
| CLI revision | `af6eae418b1076345da1a67d83c5c0ad24f985cc` |
| Core source revision | `d6dc5558307f13c654194ddc944e3be40c940675` |
| Core review-tree relation | `tree-equal:454d1590698a36194847755a4aabc4a59d6c5ec4` |
| Evidence class | `hosted-linux-direct-source-built-core` |
| CI record | `36780507321` |
| Direct-Core outcome | `passed` |
| Direct-Core test budget | `4 total; 4 passed; 0 failed; 0 skipped` |
| Covered tests | `durable-journey`, `durable-pin`, `read-journey`, `read-pin` |
| Durable journey scope | `preview, changed-context rejection, start, same-key replay, operation readback, unsupported cancellation, unrelated-service preservation` |
| Packaged Core | `unobserved` |
| Release, deployment and GA | `unobserved` |
| Windows local 10-second attempt | `incomplete` |
| Windows terminal receipt | `unobserved` |
| Windows cleanup | `unobserved` |

This record intentionally contains no raw stdout, assertion text, credentials,
paths, URLs, or service-control values. It records observed class and result,
not a reconstruction of unretained output or cleanup.

## Future record contract

Every future direct-Core run records the following bounded fields:

| Field | Allowed value or rule |
| --- | --- |
| `recordVersion` | `service-lasso-cli.issue-22.evidence.v1` |
| `sourceRevision`, `coreRevision` | full lowercase Git SHA only |
| `evidenceClass` | `fixture`, `local-source-built-core`, `hosted-linux-direct-source-built-core`, or `packaged-core` |
| `outcome` | `passed`, `failed`, `incomplete`, or `unobserved` |
| `testBudget` | non-negative total, passed, failed and skipped counts only |
| `timeBudgetMs` | non-negative integer; a fixed budget is never widened by a rerun |
| `cleanup` | `passed`, `failed`, or `unobserved` with no inferred value |
| `receipt` | `present` or `unobserved` with no copied output |
| `coverageCodes` | allowlisted requirement or journey codes only |

The record must reject raw process output, assertion text, tokens, filesystem
paths, URLs, headers and service-control data. A failure records only its
allowed outcome, budget and cleanup/receipt observations; it never retries,
rewrites, or upgrades a prior result.
