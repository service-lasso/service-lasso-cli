# CLI delivery backlog

This is the implementation backlog for the active CLI specification. It does not replace the governing epic; it makes remaining work visible by bounded requirement group.

| Status | Issue | Requirement / next action | Evidence boundary |
| --- | --- | --- | --- |
| In review | [#20](https://github.com/service-lasso/service-lasso-cli/issues/20) | Record the Commander/Cobra/Kong/clap decision and correct the historical ADR claim. | Source-backed comparison, documentation/contract verification; no implementation or release claim. |
| In progress | [#1](https://github.com/service-lasso/service-lasso-cli/issues/1) | Complete authenticated connections, source-safe authoring, registration, remote transfer, lifecycle and durable-operation workflows. | Each API/template dependency and real-instance workflow needs its own focused issue and direct evidence. |
| Implemented | [#24](https://github.com/service-lasso/service-lasso-cli/issues/24) | Generate deterministic, static-safe PowerShell, bash and zsh completion from the declared Commander tree; document installation and non-interactive CI use. | Generator and shell behavior only: no Core/config/filesystem/credential lookup, native binary, release, deployment or full Issue #1 claim. |
| Planned | [#1](https://github.com/service-lasso/service-lasso-cli/issues/1) | Complete the documented JSON/error/exit-code contract and Windows/Linux/macOS native-binary distribution. | The current Node 22 archive is not standalone-binary proof. A Node SEA or Go/Rust route requires a dedicated decision and three-platform acceptance. |
