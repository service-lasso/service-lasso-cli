# Ordinary source CLI smoke against an isolated real Core

Use Node22 and run `npm ci --ignore-scripts --no-audit --no-fund` in the CLI checkout.
Prepare an **owned fresh Core checkout** at an explicitly reviewed develop commit
containing `scripts/operator-e2e-fixture.mjs`; install its locked dependencies with
`npm ci --ignore-scripts --no-audit --no-fund`. Core `dist` must initially be absent:
do not delete another owner's ignored artifacts to satisfy this condition. The
helper builds the exact selected source before starting anything.

```
npm run e2e:local -- --core-dir /absolute/owned/core --core-revision FULL_40_HEX_SHA --output-dir /absolute/fresh/evidence
```

The CLI checkout must be clean. The runner creates a new private evidence folder,
compiles only the seven ordinary CLI modules into scratch, then owns a temporary
Core child with private workspace/services/registries and random loopback port.
Help/version/status/inspect/services exercise actual CLI argv. A separately labeled
backend security probe verifies the exact CLI Bearer transport's actor identity
with genuine and invalid temporary credentials; loopback fallback is not a401 or
complete permissions proof. Credentials never appear in arguments or public JSON.
Commands are bounded and failures retain private diagnostics. Final receipt,
credential deletion and independent closed-port observation govern cleanup PASS.
The existing local runtime and services are never selected.

`result.json` identifies source/tool/output hashes and individual outcomes. Exit0
means only this ordinary source/read smoke passed. `fullLifecycle.status=BLOCKED`
records missing admitted template/native authoring/lifecycle dependencies; it never
means the whole CLI or a packaged native binary is qualified. Native39 authoring,
SEA/platform packaging, publication, deployment and release remain separate gates.
The dedicated workflow is manually dispatched with an exact Core revision; it
builds fresh and publishes only the secret-free result projection.


## Portable Node distribution suite

Use `npm run e2e:portable --` with the same explicit Core inputs and fresh output.
The seven ordinary modules compile once; packaging changes only the version marker
and generated package metadata. Exactly one `.tgz` archive contains nine declared
files (seven runtime modules, README and package metadata), with no tests/helpers/
credential files. Its SHA256 is checked before and after scripts-disabled install
into a new private consumer. Every installed package file is compared against its
staged byte digest; the executable and Commander resolve inside that consumer.
Installed help/version and real Core reads reuse the same owned fixture checks.
Archive, installed metadata and private evidence are retained for audit; no consumer
process remains running. This portable archive requires Node22; it is never labeled
a standalone SEA/native binary or a native authoring qualification.
