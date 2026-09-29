# CLI capability matrix

This matrix binds issue [#1](https://github.com/service-lasso/service-lasso-cli/issues/1)
to the active `SPEC-CLI-FOUNDATION` slice. "Implemented" means covered by this
repository's focused tests, not a release or a claim that a remote runtime has
been qualified. See [Core compatibility](core-compatibility.md) for the
external-CLI/Core command boundary and the missing integration contracts.

| Workflow | Status | Contract and boundary |
| --- | --- | --- |
| Configured Core origin and token | Implemented | Flag > `SERVICE_LASSO_CORE_URL` > saved config > local default; origins reject credentials, paths and non-HTTP(S) schemes. `SERVICE_LASSO_CORE_TOKEN` is environment-only, never saved or printed, and may travel only over HTTPS or loopback HTTP. |
| Read Core health, identity and capabilities | Implemented | `GET /api/health`, `GET /api/runtime/instance`, and `GET /api/runtime/capabilities`; JSON-only response and safe HTTP-status errors. |
| List services | Implemented | `GET /api/services`; safe, read-only discovery. |
| Start, stop, restart | Implemented foundation | `POST /api/services/{id}/{action}` only after local `--confirm`; server-side permission and confirmation contracts remain authoritative. |
| Local authoring scaffold | Implemented foundation | Creates only a caller-selected absent directory; `--dry-run` has no writes. Generated manifest is a starter and must be completed against the versioned service-template before registration. |
| Authentication and named connections | Planned | Requires an agreed Core auth/profile contract; credentials will not be placed in arguments or output. |
| Validate, register, transfer and install | Planned | Requires Core/template registration and remote-acquisition contracts; no shared-path assumption is made. |
| Durable operation wait/reconcile | Planned | Requires Core durable-operation API and idempotency contract. |
| Remove, inbox/history and logs | Planned | Requires the owning Core API and data-access contracts. |
| Shell completion and standalone binaries | Planned | Deferred to the hardening/distribution slice after the command/API contract settles. |
| Core executable compatibility | Implemented foundation | This package installs `service-lassoctl`; Core retains `service-lasso` for its in-process, local-runtime CLI. |

## Safety properties in this slice

- Mutations require `--confirm` and are never prompted for.
- Result data uses stdout; diagnostic/error data uses stderr.
- HTTP response bodies are not copied into errors, logs, or test assertions.
- Scaffolding refuses an existing destination and does not contact Core.
- A client-side confirmation never substitutes for Core authentication,
  permissions, or server-issued confirmation requirements.
