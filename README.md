# Service Lasso CLI

`service-lassoctl` is an automation-first client for scaffolding service packages
and operating a running Service Lasso Core instance through its public HTTP
API.

## Development

```powershell
npm ci
npm test
npm run start -- --help
```

## Commands

```text
service-lassoctl config path
service-lassoctl config get core-url
service-lassoctl config set core-url http://127.0.0.1:17883
service-lassoctl instance status
service-lassoctl instance inspect --json
service-lassoctl service list
service-lassoctl service start <service-id> --confirm
service-lassoctl service init <service-id> --directory ./lasso-example
```

`SERVICE_LASSO_CORE_URL` overrides the saved Core URL. Mutation commands always
require `--confirm`; the CLI does not prompt or silently mutate Core.

For a remote Core whose policy accepts local-admin-token authentication, inject
`SERVICE_LASSO_CORE_TOKEN` through the CI secret mechanism and use an HTTPS
Core URL. The token is sent as a Bearer credential, is never accepted as a
command-line argument, and is never saved in the local config file or printed
by the CLI. The CLI rejects a token for non-loopback HTTP origins; its local
`http://127.0.0.1:17883` default remains supported. `instance inspect` reads
the documented Core health, instance, and capability endpoints without
mutation.

Service Lasso Core retains the `service-lasso` executable for its local-runtime
operator workflows. This package intentionally installs `service-lassoctl` and
does not provide a `service-lasso` alias. See the [Core compatibility
contract](docs/core-compatibility.md).

## Foundation scope and machine contract

This first slice implements deterministic local configuration, non-destructive
authoring scaffolds, read-only Core status/service discovery, and explicitly
confirmed lifecycle requests. It deliberately does not claim authenticated
remote registration, artifact transfer, durable operation polling, service
removal, or distribution binaries. See the [capability matrix](docs/capability-matrix.md)
for the exact boundary.

For `--json` command results, JSON is written to stdout. Diagnostics and errors
are written to stderr; the initial error format is a stable `Error [code]:`
line and all command failures exit non-zero. Core response bodies are never
echoed, because they can contain operator-sensitive data.

This repository does not publish packages, releases, or deployments from local
development commands.
