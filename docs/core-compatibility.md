# Core compatibility contract

## Command ownership

Service Lasso Core already publishes an in-process CLI named `service-lasso`.
Installing this package under the same executable name would make npm's bin
resolution depend on installation order and could invoke the wrong operator
surface. This external package therefore installs `service-lassoctl`. The
`ctl` suffix describes a remote-control client and remains distinct from Core's
host-local command.

`service-lasso` remains Core's local-runtime command. `service-lassoctl` owns
caller-local authoring and future remote, HTTP-contract-based control. Neither
tool replaces the other, and this package does not import Core internals.

## Migration and aliases

There is no published external binary to preserve: this repository's baseline
has not released a package. The package provides no `service-lasso` alias,
because an alias would recreate the installation-order collision. Scripts must
invoke `service-lassoctl` explicitly. A future migration from a released name
requires a versioned deprecation plan and must retain Core's command ownership.

## Current contract evidence

| CLI adapter | Repository evidence | Integration qualification |
| --- | --- | --- |
| `GET /api/health` | Dependency-injected unit transport test | No live Core version or capability negotiation evidence. |
| `GET /api/services` | Dependency-injected unit transport test | No live Core response-schema or permission evidence. |
| `POST /api/services/{id}/{action}` | Unit transport test; local `--confirm` gate | No Core confirmation-token, authorization, durable-operation or idempotency evidence. |
| Authoring scaffold | Local filesystem tests | Starter manifest is not a Core registration contract. |

## Required Core-owned contract before workflow expansion

Core must publish versioned public contracts for:

1. authentication and named connection profiles, including CI-safe credential
   injection and TLS verification;
2. runtime identity, API version and capability discovery;
3. service validation, registration/import and duplicate identity handling;
4. remote artifact transfer or acquisition, with no shared-filesystem
   assumption;
5. install, configuration and lifecycle requests with server-side confirmation,
   permission and idempotency semantics; and
6. durable operation identifiers, result/status lookup, bounded waiting and
   cancellation.

Until those contracts exist, this repository must not add registration, install,
authentication, operation waiting or remote-transfer commands. A Core contract
release should be consumed through an explicit compatibility issue and an
integration test against a supported running instance.
