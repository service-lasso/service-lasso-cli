# Durable external operator workflows

## Scope

Issue #22 implements the external `service-lassoctl` adapter for the reviewed
Core durable lifecycle HTTP contract at frozen provisional Core commit
`454d1590698a36194847755a4aabc4a59d6c5ec4`. This is Development work. The
contract is a dependency under review, not evidence that it is merged,
published, or qualified in a real Core runtime.

## Requirements

- `CLI-DURABLE-001`: The executable is `service-lassoctl` and uses only a configured HTTP(S) origin and environment-only credentials.
- `CLI-DURABLE-002`: Reads expose runtime status, service health, setup, dependencies, and lifecycle availability without mutation.
- `CLI-DURABLE-003`: The client gates durable actions on advertised contract version and per-service action availability.
- `CLI-DURABLE-004`: Preview is server validated and has no mutation effect.
- `CLI-DURABLE-005`: Preview emits a schema-validated, allowlisted operator record. It displays only the server-issued confirmation id, expiry and phrase required to execute; it never prints raw Core response bodies or unallowlisted fields.
- `CLI-DURABLE-006`: Execute requires local `--confirm`, a server confirmation id and phrase, and a caller-supplied idempotency key. It uses the authenticated Core contract's exact action names: `install`, `config`, `start`, `stop`, and `restart`.
- `CLI-DURABLE-006A`: Lifecycle requests contain only Core's fixed public fields (`action`, `serviceId`, `execute`, `idempotencyKey`, `confirmationId`, `confirmationPhrase`, and optional `confirmationTtlSeconds`). The external CLI does not accept or spread arbitrary parameter JSON.
- `CLI-DURABLE-007`: Accepted work returns an opaque operation ID; inspect, bounded wait, and reconciliation by operation ID are available.
- `CLI-DURABLE-008`: Cancellation is attempted only when the operation says it is supported; unsupported and too-late results are reported without retry.
- `CLI-DURABLE-009`: The client never retries or resubmits a mutation. A transport loss is `uncertain` and is reconciled by operation ID or the same server idempotency key.
- `CLI-DURABLE-010`: JSON result records go to stdout and diagnostics go to stderr without credentials or response-body echoes. The server-issued confirmation phrase is the sole deliberate display exception and appears only in the validated preview record because execution requires operator re-entry of that phrase.
- `CLI-DURABLE-011`: Noninteractive missing values fail deterministically; `--confirm` does not bypass server authentication or confirmation.
- `CLI-DURABLE-012`: The compiled binary is tested against the actual source-built provisional Core on an owned disposable loopback fixture with a scoped signed JWT, actor/client identity, required profile and scopes. Evidence includes preview, one mutation, idempotent same-key replay, changed-context rejection, error redaction, operation inspection, unavailable cancellation, and unrelated-service preservation.

## Dependencies and non-goals

Core #1538 cancellation transport, #1542 update, #1543 removal and #1544 inbox remain external tracked dependencies. Existing released-reference registration is separate. Core #1541 / PR #1545 is retained as a wrong-repository, superseded attempt and is not source input for this implementation.
