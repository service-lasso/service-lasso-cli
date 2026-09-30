# Durable operator workflows

`service-lassoctl` is an external HTTP client. It does not run services or
store lifecycle state. Set `SERVICE_LASSO_CORE_URL` and, when required by Core,
`SERVICE_LASSO_CORE_TOKEN`; credentials are never accepted as command flags.

With `--json`, result records are JSON on stdout. Diagnostics and errors are
stable text on stderr in the form `Error [code]: message`. Exit `0` means
the requested read or terminal operation succeeded; `2` accepted, `3` failed,
`4` cancelled, `5` timeout, `6` uncertain, and `1` invalid input or API error.
An accepted operation has not succeeded yet.

```powershell
service-lassoctl operator availability demo --json
service-lassoctl operator preview start demo --json
service-lassoctl operator execute start demo --confirm --confirmation-id <id> --confirmation-phrase <phrase> --idempotency-key deploy-demo-0001 --wait-ms 30000 --json
service-lassoctl operator operation get <operation-id> --json
```

Preview calls Core without `execute` and returns the server-issued confirmation.
Execution needs both values from that preview, an explicit idempotency key, and
`--confirm`. `--confirm` is not a permission or server-confirmation bypass.
On disconnect the CLI reports `uncertain`; do not submit a new mutation. Read
the returned operation ID, or repeat the identical request with the same server
idempotency key only when the server contract makes replay safe.

The durable dependency is frozen at merged Core `develop`
`d6dc5558307f13c654194ddc944e3be40c940675`. Its tree is identical to the
reviewed `454d1590698a36194847755a4aabc4a59d6c5ec4` source. The focused transport fixture
proves only the CLI's request and safe-output boundary. Separately, the
repository's real-core acceptance test constructs a guarded Core with a scoped
signed JWT and JWKS, then runs the compiled CLI through preview, changed-context
rejection, start, same-key replay, inspection, unsupported cancellation, and
unrelated-service preservation. That is direct evidence against this merged
source-built Core only. It is not packaged-Core, release, deployment, or GA
qualification. Cancel, update, removal and
inbox coverage remain tracked by Core #1538, #1542, #1543 and #1544.
