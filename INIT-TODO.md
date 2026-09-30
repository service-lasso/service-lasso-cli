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
- [x] Define Issue #16 `CLI-REAL-CORE-ACCEPTANCE`: run the compiled CLI against
  the pinned Core `develop` runtime on an ephemeral loopback port with temporary
  workspace and service roots, proving read-only inspect/list behavior without
  service autostart, registration, lifecycle actions, credentials, or retained
  runtime state.
- [ ] Complete Issue #1's roadmap epic after Core publishes the registration,
  authentication, remote-acquisition and durable-operation contracts. The
  foundation slice is tracked in `SPEC-CLI-FOUNDATION.md`; its exact boundaries
  are maintained in `docs/capability-matrix.md`.
