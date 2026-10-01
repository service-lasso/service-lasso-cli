import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdir, mkdtemp, readFile, rename, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { assertProviderPreflight, canonicalPublicAssetUrl, expectedAssets, parseStrictJson, sha256, validateManifest, verifyCandidateDirectory } from "../scripts/protected-candidate-lib.mjs";

const node = process.execPath;
const sourceSha = "0123456789abcdef0123456789abcdef01234567";
const version = "0.1.0-dev.0123456";

async function nativeDirectory(root, target) {
  const directory = join(root, target.id);
  await mkdir(directory, { recursive: true });
  const executable = target.id === "win32-x64" ? "service-lassoctl.exe" : "service-lassoctl";
  const bytes = Buffer.from(`native-${target.id}`);
  await writeFile(join(directory, executable), bytes);
  const digest = sha256(bytes);
  await writeFile(join(directory, "provenance.json"), `${JSON.stringify({ schemaVersion: 1, command: "service-lassoctl", candidate: { version, tag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}` }, source: { commit: sourceSha }, executable: { name: executable, sha256: digest, platform: target.platform, architecture: target.architecture, version }, tools: {}, sea: {} })}\n`);
  await writeFile(join(directory, "ci-context.json"), `${JSON.stringify({ schemaVersion: 1, eventName: "workflow_dispatch", sourceSha, testedBaseSha: null, mergeContextSha: sourceSha })}\n`);
  const evidenceDigest = sha256(Buffer.from(`service-lasso-native-acceptance-v1\n${sourceSha}\n${version}\n${target.platform}\n${target.architecture}\n${digest}\nnode-absent\npassed\n`, "utf8"));
  await writeFile(join(directory, "host-acceptance.json"), `${JSON.stringify({ schemaVersion: 1, sourceSha, version, platform: target.platform, architecture: target.architecture, executableSha256: digest, nodeAbsentFromPath: true, status: "passed", evidenceDigest })}\n`);
  execFileSync("tar", ["-czf", `service-lassoctl-${version}-${target.id}.tar.gz`, executable, "provenance.json", "ci-context.json", "host-acceptance.json"], { cwd: directory });
  await rm(join(directory, executable));
  return directory;
}

async function rebindCandidate(directory) {
  const manifestPath = join(directory, "development-candidate.json");
  const manifest = JSON.parse(await readFile(manifestPath, "utf8"));
  for (const asset of manifest.assets) { const bytes = await readFile(join(directory, asset.name)); asset.sha256 = sha256(bytes); asset.size = bytes.length; }
  const manifestBytes = Buffer.from(`${JSON.stringify(manifest, null, 2)}\n`);
  await writeFile(manifestPath, manifestBytes);
  const sums = [{ name: "development-candidate.json", sha256: sha256(manifestBytes) }, ...manifest.assets].sort((left, right) => left.name.localeCompare(right.name)).map((asset) => `${asset.sha256}  ${asset.name}\n`).join("");
  await writeFile(join(directory, "SHA256SUMS.txt"), sums);
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

test("CLI30 raw JSON parser rejects duplicate keys before JSON.parse loses them", () => {
  assert.throws(() => parseStrictJson(Buffer.from('{"source":{"commit":"a","commit":"b"}}'), "fixture"), /duplicate JSON key/);
});

test("CLI30 verifier rejects actual hostile archive bytes and keeps held bytes after path replacement", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-hostile-"));
  try {
    const portable = join(root, "portable");
    execFileSync(node, ["scripts/package-candidate.mjs", "--output", portable, "--version", version, "--source-sha", sourceSha], { encoding: "utf8" });
    const argumentsForAssembly = ["scripts/assemble-protected-candidate.mjs", "--output", join(root, "candidate"), "--portable", portable, "--version", version, "--source-sha", sourceSha];
    for (const target of [{ id: "win32-x64", platform: "win32", architecture: "x64" }, { id: "linux-x64", platform: "linux", architecture: "x64" }, { id: "darwin-arm64", platform: "darwin", architecture: "arm64" }]) argumentsForAssembly.push("--native", `${target.id}=${await nativeDirectory(root, target)}`);
    execFileSync(node, argumentsForAssembly, { encoding: "utf8" });
    const verified = await verifyCandidateDirectory(join(root, "candidate"), version, sourceSha);
    const archiveName = `service-lassoctl-${version}-linux-x64.tar.gz`;
    const heldArchive = verified.held.get(archiveName);
    await rename(join(root, "candidate", archiveName), join(root, "candidate", `${archiveName}.saved`));
    await writeFile(join(root, "candidate", archiveName), "replacement bytes");
    assert.equal(verified.held.get(archiveName), heldArchive);
    assert.notEqual(sha256(verified.held.get(archiveName)), sha256(await readFile(join(root, "candidate", archiveName))));
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("CLI30 archive and identity boundaries fail closed after checksum rebinding", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-boundaries-"));
  try {
    const portable = join(root, "portable");
    execFileSync(node, ["scripts/package-candidate.mjs", "--output", portable, "--version", version, "--source-sha", sourceSha], { encoding: "utf8" });
    const argumentsForAssembly = ["scripts/assemble-protected-candidate.mjs", "--output", join(root, "candidate"), "--portable", portable, "--version", version, "--source-sha", sourceSha];
    for (const target of [{ id: "win32-x64", platform: "win32", architecture: "x64" }, { id: "linux-x64", platform: "linux", architecture: "x64" }, { id: "darwin-arm64", platform: "darwin", architecture: "arm64" }]) argumentsForAssembly.push("--native", `${target.id}=${await nativeDirectory(root, target)}`);
    execFileSync(node, argumentsForAssembly, { encoding: "utf8" });
    const candidate = join(root, "candidate");
    const nativeArchive = join(candidate, `service-lassoctl-${version}-linux-x64.tar.gz`);
    const nativeBytes = await readFile(nativeArchive);
    await writeFile(nativeArchive, Buffer.concat([nativeBytes, nativeBytes]));
    await rebindCandidate(candidate);
    await assert.rejects(() => verifyCandidateDirectory(candidate, version, sourceSha), /gzip footer|unclaimed/);
    await writeFile(nativeArchive, nativeBytes);
    const provenance = join(candidate, "provenance-linux-x64.json");
    await writeFile(provenance, '{"schemaVersion":1,"schemaVersion":1}');
    await rebindCandidate(candidate);
    await assert.rejects(() => verifyCandidateDirectory(candidate, version, sourceSha), /duplicate JSON key/);
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("CLI30 schema rejects duplicates, unexpected fields and forged source identity", () => {
  const assets = expectedAssets(version).map((asset) => ({ ...asset, sha256: "b".repeat(64), size: 1 }));
  const valid = { schemaVersion: 1, version, candidateTag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}`, source: { repository: "service-lasso/service-lasso-cli", commit: sourceSha }, checksums: { algorithm: "sha256", file: "SHA256SUMS.txt", entries: ["development-candidate.json", ...assets.map((asset) => asset.name)].sort() }, assets };
  validateManifest(valid, version, sourceSha);
  assert.throws(() => validateManifest({ ...valid, attacker: true }, version, sourceSha));
  assert.throws(() => validateManifest({ ...valid, assets: [...assets, assets[0]] }, version, sourceSha));
  assert.throws(() => validateManifest({ ...valid, source: { repository: "service-lasso/service-lasso-cli", commit: "f".repeat(40) } }, version, sourceSha));
});

test("CLI30 provider preflight and public URL fail closed", () => {
  const preflight = { immutable: { enabled: true }, branch: { name: "develop", protected: true }, protection: { required_status_checks: { strict: true, contexts: ["CI"] }, required_pull_request_reviews: {}, enforce_admins: { enabled: true }, allow_force_pushes: { enabled: false } }, environment: { name: "development-candidate", protection_rules: [{ type: "required_reviewers" }], deployment_branch_policy: { custom_branch_policies: true, protected_branches: false } }, branchPolicies: [{ name: "develop" }] };
  assert.doesNotThrow(() => assertProviderPreflight(preflight));
  assert.throws(() => assertProviderPreflight({ ...preflight, immutable: { enabled: false } }));
  assert.doesNotThrow(() => canonicalPublicAssetUrl("https://github.com/service-lasso/service-lasso-cli/releases/download/x/service-lassoctl-a.tgz", "service-lassoctl-a.tgz", "x"));
  assert.throws(() => canonicalPublicAssetUrl("https://attacker.invalid/a", "a"));
});
