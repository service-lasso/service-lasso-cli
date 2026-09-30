import { createHash } from "node:crypto";
import { readFile, readdir } from "node:fs/promises";
import { join } from "node:path";

export const NATIVE_TARGETS = Object.freeze([
  Object.freeze({ id: "win32-x64", platform: "win32", architecture: "x64" }),
  Object.freeze({ id: "linux-x64", platform: "linux", architecture: "x64" }),
  Object.freeze({ id: "darwin-arm64", platform: "darwin", architecture: "arm64" }),
]);

const REPOSITORY = "service-lasso/service-lasso-cli";
const SHA = /^[0-9a-f]{40}$/i;
const DIGEST = /^[0-9a-f]{64}$/i;
const VERSION = /^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i;
const NAME = /^[a-z0-9][a-z0-9._-]*$/i;

export function sha256(bytes) { return createHash("sha256").update(bytes).digest("hex"); }
export function fail(message) { throw new Error(`Protected candidate rejected: ${message}`); }
export function assertClosedObject(value, keys, label) {
  if (!value || typeof value !== "object" || Array.isArray(value)) fail(`${label} must be an object.`);
  const actual = Object.keys(value).sort();
  const expected = [...keys].sort();
  if (actual.length !== expected.length || actual.some((key, index) => key !== expected[index])) fail(`${label} has an unexpected schema.`);
}
export function expectedAssets(version) {
  if (!VERSION.test(version)) fail("candidate version is invalid.");
  return [
    { name: `service-lassoctl-${version}.tgz`, kind: "portable", target: null },
    { name: "candidate.json", kind: "portable-record", target: null },
    ...NATIVE_TARGETS.flatMap((target) => [
      { name: `service-lassoctl-${version}-${target.id}.tar.gz`, kind: "native", target: target.id },
      { name: `provenance-${target.id}.json`, kind: "provenance", target: target.id },
    ]),
  ];
}

function assertAsset(asset, expected) {
  assertClosedObject(asset, ["kind", "name", "sha256", "size", "target"], `asset ${expected.name}`);
  if (asset.name !== expected.name || asset.kind !== expected.kind || asset.target !== expected.target || !DIGEST.test(asset.sha256) || !Number.isSafeInteger(asset.size) || asset.size < 1) fail(`asset ${expected.name} is invalid.`);
}

export function validateManifest(manifest, version, sourceSha) {
  if (!VERSION.test(version) || !SHA.test(sourceSha)) fail("expected identity is invalid.");
  assertClosedObject(manifest, ["assets", "candidateTag", "schemaVersion", "source", "version"], "development candidate manifest");
  if (manifest.schemaVersion !== 1 || manifest.version !== version || manifest.candidateTag !== `cli-v${version}-candidate-${sourceSha.slice(0, 7)}`) fail("candidate identity does not match the frozen source.");
  assertClosedObject(manifest.source, ["commit", "repository"], "candidate source");
  if (manifest.source.repository !== REPOSITORY || manifest.source.commit !== sourceSha) fail("candidate source is not the expected full SHA.");
  if (!Array.isArray(manifest.assets)) fail("candidate assets must be an array.");
  const expected = expectedAssets(version);
  if (manifest.assets.length !== expected.length) fail("candidate asset inventory is incomplete or contains extras.");
  const byName = new Map();
  for (const asset of manifest.assets) {
    if (!asset || typeof asset.name !== "string" || !NAME.test(asset.name) || byName.has(asset.name)) fail("candidate asset names must be unique and safe.");
    byName.set(asset.name, asset);
  }
  for (const item of expected) assertAsset(byName.get(item.name), item);
  return manifest;
}

function validateNativeProvenance(value, target, sourceSha) {
  assertClosedObject(value, ["command", "executable", "schemaVersion", "sea", "source", "tools"], `native provenance ${target.id}`);
  assertClosedObject(value.source, ["commit"], `native provenance source ${target.id}`);
  if (value.schemaVersion !== 1 || value.command !== "service-lassoctl" || value.source.commit !== sourceSha) fail(`native provenance ${target.id} has a forged source.`);
  assertClosedObject(value.executable, ["architecture", "name", "platform", "sha256"], `native executable ${target.id}`);
  if (value.executable.platform !== target.platform || value.executable.architecture !== target.architecture || !DIGEST.test(value.executable.sha256)) fail(`native provenance ${target.id} has an invalid executable identity.`);
}

export async function verifyCandidateDirectory(directory, version, sourceSha) {
  const manifestBytes = await readFile(join(directory, "development-candidate.json"));
  const manifest = validateManifest(JSON.parse(manifestBytes.toString("utf8")), version, sourceSha);
  const expectedFiles = new Set(["development-candidate.json", "SHA256SUMS.txt", ...manifest.assets.map((asset) => asset.name)]);
  const actualFiles = new Set(await readdir(directory));
  if (actualFiles.size !== expectedFiles.size || [...actualFiles].some((name) => !expectedFiles.has(name))) fail("candidate directory is not a closed inventory.");
  for (const asset of manifest.assets) {
    const bytes = await readFile(join(directory, asset.name));
    if (bytes.length !== asset.size || sha256(bytes) !== asset.sha256) fail(`asset ${asset.name} does not match its manifest digest.`);
  }
  for (const target of NATIVE_TARGETS) {
    const name = `provenance-${target.id}.json`;
    validateNativeProvenance(JSON.parse((await readFile(join(directory, name))).toString("utf8")), target, sourceSha);
  }
  const expectedSums = [...manifest.assets, { name: "development-candidate.json", sha256: sha256(manifestBytes) }]
    .map((asset) => `${asset.sha256}  ${asset.name}\n`).join("");
  if ((await readFile(join(directory, "SHA256SUMS.txt"), "utf8")) !== expectedSums) fail("checksum inventory is not exact.");
  return manifest;
}

export function assertPreflight(value, sourceSha) {
  if (!SHA.test(sourceSha)) fail("preflight source SHA is invalid.");
  assertClosedObject(value, ["develop", "environment", "releases"], "preflight");
  assertClosedObject(value.develop, ["name", "protected"], "develop protection");
  assertClosedObject(value.environment, ["name", "protected", "reviewersRequired", "selectedBranches"], "candidate environment");
  assertClosedObject(value.releases, ["immutable"], "release policy");
  if (value.develop.name !== "develop" || value.develop.protected !== true || value.environment.name !== "development-candidate" || value.environment.protected !== true || value.environment.reviewersRequired !== true || value.environment.selectedBranches !== "develop" || value.releases.immutable !== true) fail("required branch, environment, or immutable-release protection is absent.");
}

export function assertPublicResponse(response, secret) {
  if (response.request?.headers?.authorization || response.request?.headers?.Authorization) fail("public readback must not send authorization.");
  if (response.status >= 300 && response.status < 400) fail("public readback must reject redirects.");
  if (response.status !== 200) fail("public readback did not return an asset.");
  if (secret && `${response.body ?? ""}`.includes(secret)) fail("public response leaked a secret sentinel.");
}
