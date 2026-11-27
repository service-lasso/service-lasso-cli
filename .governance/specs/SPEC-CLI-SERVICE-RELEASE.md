# CLI67 release-native provider manifest

Issue #67; dependency Core #1992. Development source and fixture-only proof.

- CLI67-MANIFEST: canonical root service.json identifies @cli as provider/on-demand; no daemon lifecycle, npm install or portable runtime fallback. Source discovery follows service-template; only assembled releases carry immutable pins.
- CLI67-BINDING: protected assembly emits service.json into its closed checksum inventory with exact candidate tag/version, per-target native TAR asset and archive SHA-256. Targets are win32-x64, linux-x64 and darwin-arm64. Command names match existing archived native primaries.
- CLI67-PROVENANCE: preserve every original primary, confined writer, Darwin immutable helper, provenance, CI context and host acceptance byte. Existing all-three native admission and publication checks remain mandatory; no publication/dispatch/native gate is invoked by this issue.
- CLI67-PROOF: no-publish fixture assembly/verifier proves exact native selection and rejects a rehashed manifest with wrong tag, archive digest, command, platform, role or portable fallback. Fixture native bytes are not native availability qualification.

Existing release.yml already assembles, verifies and publishes every closed inventory member. Adding service.json to that inventory ports the template release-manifest model without changing producer authority or scheduling.

Schema 2 adds the mandatory service-manifest asset. Historical schema 1 keeps its original closed inventory and held-byte verification, while new publication requires schema 2 before any provider call.
