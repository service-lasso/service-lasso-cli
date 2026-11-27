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
service-lassoctl service template --json
service-lassoctl service init <service-id> --directory ./lasso-example --dry-run --json
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

`service init` accepts an already-acquired immutable template bundle through
`--template-root` only when its complete tuple exactly matches a versioned,
owner-controlled CLI admission entry. It verifies duplicate-free closed
metadata, bounded archive members and modes, payload equality, checksums,
provenance and catalog identity before it creates a caller-selected absent
directory. `--dry-run` performs that verification and reports the full file
list without writing. A locally assembled tuple cannot create its own admission.
The command never registers, installs, starts, or contacts Core.

Without `--template-root`, `service template --json` exposes the current
template gate. The observed `service-template` `develop` source is
`bdeb24b84f97e702372ccbcba0794ce30888ad53`, contract `1.0.0-dev`, digest
`05162aa2966c3656d5809050398ebaeba354ee6639f51b047464bc6a383324cb` and
contract-file SHA-256 `95e022bf381deb1096f42a0699c4f8f0cfed996670b9d6f0655792d6e41c0f0e`.
It has no accepted immutable candidate tag, archive tuple, or catalog identity,
so the default command fails closed without creating files. That external
artifact gate blocks direct generated-project/Core qualification, not the
generic accepted-bundle implementation.

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

The existing protected `release.yml` dispatch selects Windows x64 and Linux x64
native archives, the portable Node archive, and a checksum-bound canonical
`@cli` provider manifest. Schema3 binds this scope; historical schema1/2 retain
independent verification/recovery. macOS native qualification is deferred and
is never reported as passed; Darwin source and grant requirements remain.

The owner first verifies immutable releases, strict enforced develop protection,
the exact six required Actions checks, and the development-candidate custom
develop-only environment policy with its wait timer. Dispatch binds that exact
SHA in `operator_preflight_sha`. The built-in Actions token rereads visible
policy and successful exact-SHA checks before each mutation. Native host smoke
and Linux actual Core acceptance precede same-byte assembly, private draft
verification and immutable public readback. Publication is a development
candidate, not GA, deployment or npm publication. Portable record platform
compatibility describes the Node package; it is not native platform proof.

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
insecure completion paths and suppresses `.zcompdump` writes. `-D` may still
read an existing dump.

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

### Core provider release manifest

Root `service.json` describes the on-demand `@cli` provider. Protected candidate
assembly stamps its exact candidate version/tag and native archive SHA-256 values
into the released `service.json`, included in the closed asset/checksum inventory.
Core consumes the standalone Go/SEA archive for its matching OS and architecture;
there is no portable npm fallback or daemon lifecycle. All sibling helpers and
original provenance remain in those native archives. Source discovery metadata
is not an immutable release pin. Assembly requires all existing native acceptance
gates; fixture tests do not qualify native availability or authorize publication.
