# CLI foundation

## Scope

Establish a Node.js TypeScript CLI with deterministic help, local configuration,
service-package scaffolding, and safe Core API read/mutation commands.

## Acceptance

1. `--help` and invalid input are clear, stable and non-interactive.
2. A configurable Core URL has an environment override and a local config file.
3. `service init` produces a minimally valid service-package starter without
   overwriting an existing directory.
4. `instance status` and `service list` use Core's public read endpoints.
5. Lifecycle mutations require `--confirm` and use the public lifecycle route.
6. Unit tests cover parsing, config precedence, scaffold safety, API errors and
   mutation confirmation.
