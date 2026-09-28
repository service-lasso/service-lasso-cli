# Project intent

`service-lasso-cli` is the keyboard-first, automation-friendly client for
creating Service Lasso service packages and operating a running Service Lasso
Core instance through its public HTTP API.

The CLI must be safe by default: it must not print credentials, must not
silently issue mutations, and must make the target Core URL explicit and
configurable.
