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
- [ ] Issue #8 accepted-template activation: the full-contract materializer is
  ready for an immutable candidate tag, archive/contract checksum tuple,
  provenance and catalog admission. Current `service-template` `develop`
  `bdeb24b84f97e702372ccbcba0794ce30888ad53` is `1.0.0-dev` source-only, so
  default scaffold output and direct generated-project/Core validation stay
  blocked until the owning template release supplies that tuple.
  Its materializer uses a target-host compiled primary launch gate which embeds and verifies both the SEA and a confined writer, retains their execution identity through a private IPC materialization request, and never lets the SEA spawn a verified-then-re-resolved helper pathname. Linux uses descriptor execution; Windows stages with no write/delete sharing and a verified protected DACL held through `CreateProcess`; macOS remains unqualified until target-host native execution proof. The writer itself
  shipped in each native archive. POSIX uses held directory descriptors with
  `openat`/`mkdirat`, an owned temporary candidate, and an exclusive native
  rename into the absent destination; Windows uses `NtCreateFile` relative to held handles with
  `OBJ_DONT_REPARSE`, `FILE_OPEN_REPARSE_POINT`, and a verified protected
  owner-only DACL for every new project object. The SEA never trusts mutable
  provenance for helper execution: it verifies the baked helper digest and
  launches a private copy from the verified bytes. Its request endpoint is an
  inherited non-routable capability, never a mutable Unix socket pathname;
  admission is serialized with the held child lifecycle identity and a commit
  guard rather than a
  PID or an unsynchronised process-state snapshot. The mutual gate proof uses
  a fresh inherited runtime capability, never a packaged private signing key.
  Darwin is fail-closed until
  non-interactive privilege activates and reads back `SF_IMMUTABLE` on the leaf
  and held parent while denying a pre-open writer; Windows pipe requests are
  admitted only from the live SEA PID and capability proof, and starts its
  writer from the held protected staged image rather than `PATH`. It never treats pathname checks as race
  protection. A POSIX write failure leaves the named destination
  absent; a temporary candidate that cannot be proved owned for deletion is
  retained as unknown state rather than being mistaken for a completed project.
  Windows deletes only via held owned handles. Packaged-helper integration tests coordinate parent
  replacement and concurrent unowned failure content; the helper provenance
  hashes its complete source set. Direct template/Core and three-host acceptance
  remain blocked on an owner-published accepted template tuple.
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
- [ ] Issue #30: retain the source-only publisher with actual provider GET
  preflight (review/admin/strict/direct-force policy), closed archive/portable
  verification, bounded streamed metadata/asset reads, authenticated private
  asset-ID verification before one publish transition, and retained-byte
  readback;
  maintain `SPEC-CLI-PROTECTED-NATIVE-CANDIDATE.md`, Project #1 lifecycle,
  and the concrete `.github` settings proposal. Owner approval, provider
  settings, dispatch, release assets, tags, deployment and Core packaging are
  separately authorised and remain unperformed. The bounded verifier increment
  now requires schema-closed CI-context and no-Node host-acceptance members in
  each native archive, raw duplicate-key rejection, finite gzip/tar/npm
  inspection, and retained byte buffers. Native hosted jobs now produce the
  closed workflow-dispatch context and no-Node acceptance records before
  archiving the accepted bytes; terminal CI dispatch acceptance remains later
  work.
- [ ] Issue #28: enforce the event-specific native provenance contract with real verifier-subprocess proof. `github.sha` is an explicit expected merge context; pull requests require distinct full source, tested-base and merge-context SHAs, while pushes require an intentionally empty expected base, null/absent recorded base and source-equal merge context. Source, event, digest and host checks remain strict.
- [ ] Complete the remaining Issue #1 workflow and automation-contract acceptance recorded in `.governance/project/BACKLOG.md` and `docs/capability-matrix.md`.
