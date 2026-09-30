import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdir, mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { assertPreflight, assertPublicResponse, expectedAssets, validateManifest } from "../scripts/protected-candidate-lib.mjs";

const node = process.execPath;
const sourceSha = "0123456789abcdef0123456789abcdef01234567";
const version = "0.1.0-dev.0123456";

async function nativeDirectory(root, target) {
  const directory = join(root, target.id);
  await mkdir(directory, { recursive: true });
  await writeFile(join(directory, `service-lassoctl-${version}-${target.id}.tar.gz`), `archive-${target.id}`);
  await writeFile(join(directory, "provenance.json"), `${JSON.stringify({ schemaVersion: 1, command: "service-lassoctl", source: { commit: sourceSha }, executable: { name: target.id === "win32-x64" ? "service-lassoctl.exe" : "service-lassoctl", sha256: "a".repeat(64), platform: target.platform, architecture: target.architecture }, tools: {}, sea: {} })}\n`);
  return directory;
}

test("CLI30 closed candidate verifier accepts only a complete frozen inventory", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-protected-"));
  try {
    const portable = join(root, "portable");
    execFileSync(node, ["scripts/package-candidate.mjs", "--output", portable, "--version", version, "--source-sha", sourceSha], { encoding: "utf8" });
    const argumentsForAssembly = ["scripts/assemble-protected-candidate.mjs", "--output", join(root, "candidate"), "--portable", portable, "--version", version, "--source-sha", sourceSha];
    for (const target of [{ id: "win32-x64", platform: "win32", architecture: "x64" }, { id: "linux-x64", platform: "linux", architecture: "x64" }, { id: "darwin-arm64", platform: "darwin", architecture: "arm64" }]) argumentsForAssembly.push("--native", `${target.id}=${await nativeDirectory(root, target)}`);
    execFileSync(node, argumentsForAssembly, { encoding: "utf8" });
    execFileSync(node, ["scripts/verify-protected-candidate.mjs", "--directory", join(root, "candidate"), "--version", version, "--source-sha", sourceSha], { encoding: "utf8" });
    await writeFile(join(root, "candidate", "extra.txt"), "forged");
    assert.throws(() => execFileSync(node, ["scripts/verify-protected-candidate.mjs", "--directory", join(root, "candidate"), "--version", version, "--source-sha", sourceSha], { stdio: "ignore" }));
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("CLI30 schema rejects duplicates, unexpected fields and forged source identity", () => {
  const assets = expectedAssets(version).map((asset) => ({ ...asset, sha256: "b".repeat(64), size: 1 }));
  const valid = { schemaVersion: 1, version, candidateTag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}`, source: { repository: "service-lasso/service-lasso-cli", commit: sourceSha }, assets };
  validateManifest(valid, version, sourceSha);
  assert.throws(() => validateManifest({ ...valid, attacker: true }, version, sourceSha));
  assert.throws(() => validateManifest({ ...valid, assets: [...assets, assets[0]] }, version, sourceSha));
  assert.throws(() => validateManifest({ ...valid, source: { repository: "service-lasso/service-lasso-cli", commit: "f".repeat(40) } }, version, sourceSha));
});

test("CLI30 preflight and public readback fail closed", () => {
  const preflight = { develop: { name: "develop", protected: true }, environment: { name: "development-candidate", protected: true, reviewersRequired: true, selectedBranches: "develop" }, releases: { immutable: true } };
  assert.doesNotThrow(() => assertPreflight(preflight, sourceSha));
  assert.throws(() => assertPreflight({ ...preflight, releases: { immutable: false } }, sourceSha));
  assert.doesNotThrow(() => assertPublicResponse({ status: 200, request: { headers: {} }, body: "asset" }, "secret-sentinel"));
  assert.throws(() => assertPublicResponse({ status: 302, request: { headers: {} }, body: "asset" }));
  assert.throws(() => assertPublicResponse({ status: 200, request: { headers: { authorization: "Bearer secret-sentinel" } }, body: "asset" }, "secret-sentinel"));
});
