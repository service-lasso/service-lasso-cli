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
- [ ] Complete Issue #1's roadmap epic after Core publishes the registration,
  authentication, remote-acquisition and durable-operation contracts. The
  foundation slice is tracked in `SPEC-CLI-FOUNDATION.md`; its exact boundaries
  are maintained in `docs/capability-matrix.md`.
