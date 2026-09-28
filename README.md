# Service Lasso CLI

`service-lasso` is an automation-first client for scaffolding service packages
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
service-lasso config path
service-lasso config get core-url
service-lasso config set core-url http://127.0.0.1:17883
service-lasso instance status
service-lasso service list
service-lasso service start <service-id> --confirm
service-lasso service init <service-id> --directory ./lasso-example
```

`SERVICE_LASSO_CORE_URL` overrides the saved Core URL. Mutation commands always
require `--confirm`; the CLI does not prompt or silently mutate Core.

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
