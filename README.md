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
service-lassoctl connection set local http://127.0.0.1:17883
service-lassoctl connection set remote https://core.example
service-lassoctl connection use remote
service-lassoctl --connection local instance status
service-lassoctl instance status
service-lassoctl instance inspect --json
service-lassoctl service list
service-lassoctl service start <service-id> --confirm
service-lassoctl service init <service-id> --directory ./lasso-example
service-lassoctl --connection remote service register --repo service-lasso/lasso-node --tag v1.0.0 --expected-commit <40-lowercase-hex> --expected-manifest-sha256 <64-lowercase-hex> --idempotency-key <opaque-key> --confirm --json
service-lassoctl --connection remote service operation <sro_operation_id> --json
service-lassoctl completion powershell
service-lassoctl completion bash
service-lassoctl completion zsh
```

Core resolution is deterministic: `--core-url`, then `SERVICE_LASSO_CORE_URL`, then
the selected `--connection` / `SERVICE_LASSO_CONNECTION` / saved default, then the
legacy saved origin and local default. Named connections store only Core origins;
credentials remain environment-only. Mutation commands always
require `--confirm`; the CLI does not prompt or silently mutate Core.

`service register` is the bounded released-service registration contract from
Core `develop` `387726b` / merged #1464. It sends only an allowlisted
repository/tag, full expected commit, manifest SHA-256, idempotency key and
server confirmation to Core. Inject `SERVICE_LASSO_CLI_LOCAL_ADMIN_TOKEN`
through a secret environment mechanism; the CLI sends it only in
`x-service-lasso-admin-token`, never accepts it as an argument, saves it, or
prints it. HTTPS is required outside loopback. `service operation` performs
actor-scoped durable readback. These commands have no caller filesystem,
staged-byte/transfer, install, lifecycle, deployment, release or GA behavior.

For a remote Core whose policy accepts local-admin-token authentication, inject
`SERVICE_LASSO_CORE_TOKEN` through the CI secret mechanism and use an HTTPS
Core URL. The token is sent as a Bearer credential, is never accepted as a
command-line argument, and is never saved in the local config file or printed
by the CLI. The CLI rejects a token for non-loopback HTTP origins; its local
`http://127.0.0.1:17883` default remains supported. `instance inspect` reads
the documented Core health, instance, and capability endpoints without
mutation.

Issue #14 adds an executable fixture-contract acceptance check for these
operator reads and `service list`: it runs the compiled CLI against an explicit
loopback HTTP fixture with `SERVICE_LASSO_CORE_TOKEN`, verifies the Bearer
request, and verifies that an authentication rejection exposes neither a token
nor a response body. This is direct evidence for the CLI-to-HTTP fixture
contract only. It is not live Core, remote-authentication, release, or GA
acceptance; that needs a supported authenticated Core endpoint and separately
authorised credentials.

`service init` is based on the pinned `service-template` release
`2026.5.8-d2241fe` (commit `d2241fe9b5fc477f14e99adb1836825de2c7a767`) and
writes the reviewed source identity and canonical manifest SHA-256 to
`.service-lasso-template.json`. The generated manifest is a documented local
authoring baseline informed by that released template; it is not a byte-for-byte
copy or a Core registration assertion. It applies only identity substitution,
`enabled: false`, and removal of the released sample's mutable artifact source.
An exact artifact source and checksum must be supplied before
registration. It makes an absent local directory only; it does not register,
install, or start a service. Registration remains subject to Core's contract in
issue #1463.

Service Lasso Core retains the `service-lasso` executable for its local-runtime
operator workflows. This package intentionally installs `service-lassoctl` and
does not provide a `service-lasso` alias. See the [Core compatibility
contract](docs/core-compatibility.md).

## Foundation scope and machine contract

This first slice implements deterministic local configuration, non-destructive
authoring scaffolds, read-only Core status/service discovery, and explicitly
confirmed lifecycle requests. It deliberately does not claim authenticated
remote registration, artifact transfer, durable operation polling, service
removal, or standalone distribution binaries. The candidate package archive
requires Node 22 or newer; see the [capability matrix](docs/capability-matrix.md)
for the exact boundary.

## Candidate distribution

An explicit GitHub Actions dispatch from `develop` can build a reviewable
candidate archive. It produces `service-lassoctl-<version>.tgz`,
`candidate.json`, and `SHA256SUMS.txt`, then runs clean-consumer smoke on
Windows, Linux, and macOS against a local fixture. This is surrogate client
evidence, not a packaged-Core runtime check. The record pins the source commit
and candidate tag; never select an asset called `latest` for Core packaging.
These artifacts do
do not publish npm, deploy anything, or establish GA. If all three smoke jobs
pass, the workflow creates the recorded candidate Git tag and a clearly marked
GitHub prerelease on that exact source commit. The prerelease is durable review
material for Core #1461, not a GA release.

The dispatch workflow remains at `.github/workflows/release.yml` so GitHub can
resolve the workflow identity that was registered on the default branch. A
`workflow candidate.yml not found` response occurs before a candidate job
starts; it creates no tag, prerelease, or asset.

Extracting the archive alone is insufficient because this is a Node package.
Install it with Node 22 or newer, for example:

```sh
npm install --global ./service-lassoctl-<version>.tgz
service-lassoctl --version
```

For `--json` command results, JSON is written to stdout. Diagnostics and errors
are written to stderr; the initial error format is a stable `Error [code]:`
line and all command failures exit non-zero. Core response bodies are never
echoed, because they can contain operator-sensitive data.

This repository does not publish packages, releases, or deployments from local
development commands.

## Shell completion

`completion` writes a deterministic shell script to stdout. It derives only
subcommand and declared option names from the installed CLI command tree. It
does not contact Core, read saved configuration or credentials, inspect paths,
or suggest service IDs and option values. Once `--` appears, completion returns
no candidates. The generated zsh bootstrap uses `compinit -i -D`: it ignores
insecure completion paths and does not read or write a `.zcompdump` cache.

Save and load the script with the shell's ordinary startup file:

```powershell
service-lassoctl completion powershell | Out-File -Encoding utf8 "$HOME/.service-lassoctl-completion.ps1"
. "$HOME/.service-lassoctl-completion.ps1"
```

```sh
service-lassoctl completion bash >> ~/.bashrc
# or, after `autoload -Uz compinit && compinit` in ~/.zshrc:
service-lassoctl completion zsh >> ~/.zshrc
```

In CI, generate and syntax-check the source without evaluating it or invoking
Core:

```sh
service-lassoctl completion bash > service-lassoctl-completion.bash
bash -n service-lassoctl-completion.bash
service-lassoctl completion zsh > service-lassoctl-completion.zsh
zsh -n service-lassoctl-completion.zsh
```
