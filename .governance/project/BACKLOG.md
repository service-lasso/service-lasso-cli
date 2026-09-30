# CLI delivery backlog

This is the implementation backlog for the active CLI specification. It does not replace the governing epic; it makes remaining work visible by bounded requirement group.

| Status | Issue | Requirement / next action | Evidence boundary |
| --- | --- | --- | --- |
| In review | [#20](https://github.com/service-lasso/service-lasso-cli/issues/20) | Record the Commander/Cobra/Kong/clap decision and correct the historical ADR claim. | Source-backed comparison, documentation/contract verification; no implementation or release claim. |
| In progress | [#1](https://github.com/service-lasso/service-lasso-cli/issues/1) | Complete authenticated connections, source-safe authoring, registration, remote transfer, lifecycle and durable-operation workflows. | Each API/template dependency and real-instance workflow needs its own focused issue and direct evidence. |
| Planned | [#1](https://github.com/service-lasso/service-lasso-cli/issues/1) | Define shell completion, a documented JSON/error/exit-code contract, and Windows/Linux/macOS native-binary distribution. | The current Node 22 archive is not standalone-binary proof. A Node SEA or Go/Rust route requires a dedicated decision and three-platform acceptance. |
