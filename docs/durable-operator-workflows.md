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
proves only the CLI's request and safe-output boundary. CI run `36782380749`
observed four passing Ubuntu direct-Core tests at tested CLI implementation
`1458c5167fd5c525fb85744f68002cfec762b011`: the guarded, source-built Core
durable lifecycle journey; its explicit Core pin; the source-built Core read
journey; and its explicit read pin. This is direct Linux-hosted evidence against
the pinned source-built Core only. It is not fixture-only evidence, a packaged
Core result, release, deployment, cleanup receipt, or GA qualification. A
later documentation-only commit that records this binding is not the tested
implementation and has no claimed CI result until a separate natural CI
terminal record exists.

A separate local Windows attempt did not finish within its fixed 10-second
budget. Its terminal receipt and cleanup observation are
unobserved; this documentation does not reconstruct either result. The durable
test itself covers preview, changed-context rejection, start, same-key replay,
inspection, unsupported cancellation, and unrelated-service preservation.
Cancel, update, removal and inbox coverage remain tracked by Core #1538, #1542,
#1543 and #1544. See `docs/acceptance/issue-22-evidence.md` for the safe,
structured evidence record and future-run schema.
