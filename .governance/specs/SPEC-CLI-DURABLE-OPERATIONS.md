# Durable external operator workflows

## Scope

Issue #22 implements the external `service-lassoctl` adapter for the reviewed
Core durable lifecycle HTTP contract at frozen Core commit
`cf0b1b8eb24120479f86b8e6f9bc8e219c7396f3`. This is Development work. The
contract is a dependency, not evidence that it is merged, published, or
qualified in a real Core runtime.

## Requirements

- `CLI-DURABLE-001`: The executable is `service-lassoctl` and uses only a configured HTTP(S) origin and environment-only credentials.
- `CLI-DURABLE-002`: Reads expose runtime status, service health, setup, dependencies, and lifecycle availability without mutation.
- `CLI-DURABLE-003`: The client gates durable actions on advertised contract version and per-service action availability.
- `CLI-DURABLE-004`: Preview is server validated and has no mutation effect.
- `CLI-DURABLE-005`: Execute requires local `--confirm`, a server confirmation id and phrase, and a caller-supplied idempotency key.
- `CLI-DURABLE-006`: Install, configure, start, stop, and restart preserve the exact action/service/parameter contract supplied to Core.
- `CLI-DURABLE-007`: Accepted work returns an opaque operation ID; inspect, bounded wait, and reconciliation by operation ID are available.
- `CLI-DURABLE-008`: Cancellation is attempted only when the operation says it is supported; unsupported and too-late results are reported without retry.
- `CLI-DURABLE-009`: The client never retries or resubmits a mutation. A transport loss is `uncertain` and is reconciled by operation ID or the same server idempotency key.
- `CLI-DURABLE-010`: JSON result records go to stdout and diagnostics go to stderr without credentials, confirmation phrases, or response-body echoes.
- `CLI-DURABLE-011`: Noninteractive missing values fail deterministically; `--confirm` does not bypass server authentication or confirmation.
- `CLI-DURABLE-012`: The compiled binary is tested against an owned disposable loopback fixture, including one mutation, wait, transport reconciliation, negative input, and unrelated-service preservation.

## Dependencies and non-goals

Core #1538 cancellation transport, #1542 update, #1543 removal and #1544 inbox remain external tracked dependencies. Existing released-reference registration is separate. Core #1541 / PR #1545 is retained as a wrong-repository, superseded attempt and is not source input for this implementation.
