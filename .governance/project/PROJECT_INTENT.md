# Project intent

`service-lasso-cli` is the keyboard-first, automation-friendly client for
creating Service Lasso service packages and operating a running Service Lasso
Core instance through its public HTTP API.

The CLI must be safe by default: it must not print credentials, must not
silently issue mutations, and must make the target Core URL explicit and
configurable.

Issue #22 extends this intent with durable, server-authoritative operator
workflows. The external binary is `service-lassoctl`. It consumes the reviewed
Core lifecycle-operation HTTP contract as a dependency; it does not recreate
Core lifecycle semantics, authorization, confirmation, or operation storage.
