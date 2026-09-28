# ADR-001: TypeScript with Commander

## Context

The CLI must be keyboard-first, scriptable, safe around Core mutations, and
compatible with the Node 22 ESM toolchain used by Service Lasso Core.

## Options evaluated

| Option | Strength | Decision |
| --- | --- | --- |
| Cobra (Go) | Mature command tree, completion support and standalone cross-platform binaries. | Rejected for this foundation: it would introduce a separate runtime and duplicate Core's existing Node-based contract work. Reconsider for the distribution slice if standalone binaries become the primary requirement. |
| Kong (Go) | Typed declarative flags and good validation ergonomics. | Rejected for this foundation for the same second-runtime and duplicated-contract cost as Cobra; its declarative model does not outweigh that cost here. |
| clap (Rust) | Excellent argument validation, help and portable binary distribution. | Rejected for this foundation: strongest distribution path, but highest new-toolchain and protocol-duplication cost for a Core-adjacent first slice. Reconsider only with an explicit Rust distribution decision. |
| Commander | Small, maintained Node parser with deterministic nested command help and option validation. | Selected. |

## Decision

Use TypeScript, Node 22 ESM and Commander. This records the requested Cobra,
Kong and clap evaluation before implementation. Keep Core HTTP interaction in a
small dependency-injected client so a future standalone implementation can
reuse the protocol contracts without inheriting command parsing.

## Consequences

The initial CLI is a Node-distributed executable. Packaging as a standalone
binary, interactive TUI, and richer authoring templates remain separate,
explicit follow-up work.
