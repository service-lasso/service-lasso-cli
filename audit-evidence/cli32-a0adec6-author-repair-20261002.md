# CLI PR #32 author repair record — `a0adec6282069f29ee96f1d6e1319e62687fe546`

**Author record.** This is not an independent review, merge approval, release
qualification, deployment result, or GA conclusion.

## Scope

This repair addresses the four findings in the retained independent review of
`cfb5dea4cd3e74c7053b1d0511b87ad850062636` without changing the owner-held
accepted-template admission. `develop` was refreshed from the provider and is
the current base `de128128008e02036121d006dc9a20151e0b2517`; this issue branch
contains that base and was pushed at the commit named above.

- The native packager now bakes the confined helper SHA-256 into the SEA. The
  runtime does not read `provenance.json` to authorize helper execution. It
  verifies one sidecar byte buffer against the baked digest, writes a private
  `wx` copy from that same buffer, rechecks it, and launches that copy.
- Windows `NtCreateFile` creation now passes a protected owner-only DACL for
  the project root, every created child directory, and every file. Each new
  held handle is verified before it is used. The helper accepts only template
  modes `0644` and `0755`.
- The source-helper temporary-directory comment now accurately records
  intentional retention; no recursive pathname cleanup was introduced.
- The Issue #8 specification and bootstrap checklist record these boundaries.

## Author-run evidence

Each local command used fresh values for `SERVICE_LASSO_WORKSPACE_ROOT`,
`INSTANCE_REGISTRY_PATH`, and `HOST_PORT_REGISTRY_PATH` before invocation.

- `npm run typecheck` passed.
- `npm run build && node --test tests/scaffold.test.js` passed: 8 passed, 1
  platform-specific POSIX symlink skip.
- `go test ./...` and `GOOS=windows GOARCH=amd64 go test ./...` passed in
  `native/confined-scaffold`.
- `node --test tests/native-distribution.test.js` passed: the built Windows
  SEA completed its no-Node smoke, the baked helper digest was asserted from
  the SEA bundle, and the packaged helper replacement/retention journey
  passed.

The local native packager continues to emit its pre-existing CJS
`import.meta` warning for the source-only fallback path. The packaged candidate
path has a baked helper identity and does not fall back to source execution.

## Remaining evidence and handover

The current `TEMPLATE_ADMISSIONS` catalog remains intentionally empty. An
owner-published immutable service-template tuple is still required before a
real admitted-template materialization and generated-project/Core acceptance
can be claimed. This record is only surrogate source and local Windows proof.

PR #32 requires a different fresh reviewer to perform a whole-range review of
the exact final PR head after the natural Windows x64, Linux x64, and macOS
arm64 native gates complete. That reviewer must separately assess the new
staged-helper launch boundary, Windows protected-DACL behavior, and whether
the missing owner template tuple prevents any stronger materialization claim.
