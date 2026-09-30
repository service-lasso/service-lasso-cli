# ADR-001: Retain TypeScript and Commander for current CLI slices

**Status:** Accepted for the current foundation and next API-client slices on 2026-10-01. This is not a release, native-binary, or full Issue #1 acceptance decision.

**Governing work:** [Issue #1](https://github.com/service-lasso/service-lasso-cli/issues/1), child [Issue #20](https://github.com/service-lasso/service-lasso-cli/issues/20), and `.governance/specs/SPEC-CLI-FOUNDATION.md` requirements 1 and 6.

## Decision

Retain Node 22 ESM, TypeScript, and Commander for the existing foundation and the next bounded CLI API-client work. Do not migrate to Go or Rust as an incidental consequence of this comparison.

The current source already has a separable `CoreClient`, command parsing, and stdout/stderr handling. Commander documents nested commands, argument/option validation, generated help, `exitOverride`, and configurable output streams. Those capabilities are sufficient for the current foundation's deterministic, non-interactive contract. They do **not** by themselves prove a complete JSON/exit-code contract, shell completion, a native binary, or any remaining Issue #1 workflow.

## Evidence and comparison

| Criterion | Commander / TypeScript (current) | Cobra / Go | Kong / Go | clap / Rust |
| --- | --- | --- | --- | --- |
| Command structure, validation, help | Nested commands, strict unknown-option and argument checking, generated help are documented. | Nested commands, POSIX flags, generated help and suggestions are documented. | Struct/tag-defined nested commands, help and validation are documented. | Command/argument builder and derive APIs provide help and typed validation. |
| Shell completion | No first-party generator was evidenced in the Commander material reviewed; none is implemented here. | Built-in generation for bash, zsh, fish and PowerShell is documented. | No first-party completion generator was evidenced in the reviewed Kong repository; an additional completion layer would need selection. | `clap_complete` documents generation and a PowerShell integration path; it is an additional crate/configuration. |
| JSON, stderr/stdout, exit codes | Framework output can be overridden and parse errors caught; application owns its JSON schema, diagnostic separation and exit-code taxonomy. | Application-owned policy; parsing/help/completion do not define this Service Lasso protocol. | Application-owned policy; parsing/help/validation do not define this protocol. | Parse errors expose stderr/stdout selection and an exit code; application still owns the Service Lasso result/error schema. |
| Testability and API-client sharing | Existing `CoreClient` accepts injected fetch and Node tests exercise parsing/safe transport. TypeScript can share generated protocol types or an API-client package with a TypeScript TUI, subject to a future TUI decision. | A Go migration creates a separate client implementation unless a generated protocol contract is introduced. | Same language-level trade-off as Cobra. | A Rust migration creates a separate client implementation unless a generated protocol contract is introduced. |
| Distribution | Current `bin` points to `dist/index.js`; the release workflow archives JavaScript and requires Node 22. This is portable Node distribution, **not** a standalone native binary. Node SEA is a future path but needs bundling, target-specific assembly/signing, and acceptance evidence. | Go can produce target binaries through its toolchain; this improves the native-binary path. | Same Go distribution benefit as Cobra. | Rust has a native-binary distribution path; cross-platform packaging and signing still need a dedicated delivery slice. |
| Maintenance fit | One existing runtime/toolchain and least disruption to the tested foundation. | Adds Go toolchain, dependency and client ownership. | Adds Go toolchain, dependency and client ownership. | Adds Rust toolchain, dependency and client ownership. |

Primary-source evidence:

- [Commander README](https://github.com/tj/commander.js/blob/master/Readme.md): strict parsing, generated help, output configuration, and `exitOverride`.
- [Cobra README](https://github.com/spf13/cobra/blob/main/README.md) and [completion guide](https://github.com/spf13/cobra/blob/main/site/content/completions/_index.md): command tree, help, and bash/zsh/fish/PowerShell completion.
- [Kong README](https://github.com/alecthomas/kong): typed command trees, help customization, and validation.
- [clap Error](https://docs.rs/clap/latest/clap/error/struct.Error.html) and [clap_complete](https://docs.rs/clap_complete/latest/clap_complete/): parser error output/exit behavior and completion generation.
- [Node single executable applications](https://nodejs.org/api/single-executable-applications.html): a possible future Node binary route, currently active development and requiring an embedded CommonJS script.

## Consequences and boundaries

- Commander is selected because it meets the foundation's current command and client requirements with the existing source. It is not selected because the other options are incapable.
- Shell completion remains an Issue #1 hardening requirement. Cobra's built-in completion advantage is a reason to re-evaluate if completion becomes the dominant next requirement.
- A release archive containing `dist/` is not a native binary and does not remove the Node 22 installation requirement. The existing release workflow does not satisfy Issue #1's Windows/Linux/macOS binary acceptance.
- The shared API boundary is the versioned Core HTTP contract. Language-level sharing with the TUI is an optimization, not authority to duplicate runtime semantics or bypass Core.
- Re-open this ADR through a focused issue before a Go/Rust migration, native-binary commitment, or TUI-framework selection. A migration needs a bounded compatibility, distribution, and three-platform acceptance plan.

## What this closes and what remains open

This records the framework/language comparison requested by Issue #1. It does not close Issue #1, prove a full workflow, or qualify a release. The remaining acceptance criteria, Core/template dependencies, remote transfer, durable operations, lifecycle coverage, end-to-end real-instance tests, completion and three-platform release-binary validation remain tracked in Issue #1 and the capability matrix.
