# Durable operator workflows

`service-lassoctl` is an external HTTP client. It does not run services or
store lifecycle state. Set `SERVICE_LASSO_CORE_URL` and, when required by Core,
`SERVICE_LASSO_CORE_TOKEN`; credentials are never accepted as command flags.

Every result is JSON on stdout. Diagnostics are JSON on stderr. Exit `0` means
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

The test fixture proves client transport behavior only. The dependency contract
is frozen Core `cf0b1b8eb24120479f86b8e6f9bc8e219c7396f3`, reviewed but not
merged, published, or direct-runtime-qualified. Cancel, update, removal and
inbox coverage remain tracked by Core #1538, #1542, #1543 and #1544.
