# Bootstrap checklist

- [x] Create a source-independent `develop` baseline.
- [x] Add repository governance and an active foundation specification.
- [x] Define Issue #6 `CLI-DISTRIBUTION` candidate requirements: a manually
  dispatched `develop` workflow, immutable candidate record, SHA-256 manifest,
  Node 22 package archive, three-platform clean-consumer smoke, and a
  job-local GitHub Actions tagger identity for the post-smoke prerelease tag.
- [x] Define Issue #8 named origin selection and pinned template authoring:
  flag/environment/profile precedence, credentials kept environment-only, and
  the reviewed service-template tag, commit and manifest checksum recorded in
  each non-destructive local scaffold.
- [x] Define Issue #16 `CLI-REAL-CORE-ACCEPTANCE`: run the source-built compiled
  CLI against pinned Core `develop` `d9e2ae799244317940c862fe1261dfd22b7bdda1`
  on an ephemeral loopback port with temporary workspace and service roots,
  proving read-only inspect/list behavior for an enabled, autostart-eligible
  service while startup is suppressed. The source-built CLI-to-Core proof is
  separate from checksum-bound CLI candidate distribution evidence and creates
  no registration, lifecycle, credentials, or retained runtime state.
- [ ] Complete Issue #1 through bounded slices. Issue #18 implements the
  released-service registration/readback contract; transfer, install,
  lifecycle, distribution qualification and GA remain open. The active
  boundaries are maintained in `SPEC-CLI-FOUNDATION.md` and
  `docs/capability-matrix.md`.
- [ ] Issue #22: implement `SPEC-CLI-DURABLE-OPERATIONS` external durable
  operator slice against merged Core `develop` `d6dc5558307f13c654194ddc944e3be40c940675`.
  Record compiled external-CLI to source-built Core evidence with disposable
  JWT-authenticated fixtures separately from merged-Core, packaged-Core,
  release, deployment, and GA qualification.
- [x] Evaluate the Issue #1 framework/language candidates in child Issue #20 and record the foundation decision in `docs/decisions/ADR-001-cli-framework.md`.
- [x] Complete Issue #24 `CLI-COMPLETION`: deterministic Commander-derived PowerShell, bash and zsh completion source, safe `--` handling, concise install/CI examples and focused shell evidence. This is a bounded Issue #1 automation/documentation acceptance; no native binary, release or full workflow claim follows.
- [ ] Issue #26: directly validate checksum-bound Node 22.23.2 SEA binaries on Windows x64, Linux x64 and macOS arm64. Each target needs a native no-Node fixture journey; Linux additionally runs the native executable against the pinned actual guarded Core lifecycle route. The portable archive remains separate.
- [ ] Complete the remaining Issue #1 workflow and automation-contract acceptance recorded in `.governance/project/BACKLOG.md` and `docs/capability-matrix.md`.
