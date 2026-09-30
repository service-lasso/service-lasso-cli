import { readFile } from "node:fs/promises";
import { join, resolve } from "node:path";
import { assertPreflight, fail, sha256, verifyCandidateDirectory } from "./protected-candidate-lib.mjs";

function option(name) { const index = process.argv.indexOf(name); if (index < 0 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`); return process.argv[index + 1]; }
const directory = resolve(option("--directory"));
const version = option("--version");
const sourceSha = option("--source-sha");
const repository = option("--repository");
const token = process.env.DEVELOPMENT_CANDIDATE_TOKEN;
if (!token) fail("the protected environment token is unavailable.");
const [owner, name] = repository.split("/");
if (!owner || !name || repository !== "service-lasso/service-lasso-cli") fail("repository endpoint is not allowed.");
const manifest = await verifyCandidateDirectory(directory, version, sourceSha);
const tag = manifest.candidateTag;
const apiBase = "https://api.github.com";
const uploadBase = "https://uploads.github.com";

async function api(path, method = "GET", body) {
  let response;
  try {
    response = await fetch(`${apiBase}${path}`, { method, redirect: "error", headers: { accept: "application/vnd.github+json", authorization: `Bearer ${token}`, ...(body ? { "content-type": "application/json" } : {}) }, body: body ? JSON.stringify(body) : undefined });
  } catch { fail("GitHub metadata request failed without a readable response."); }
  if (!response.ok) return { status: response.status, value: null };
  return { status: response.status, value: await response.json() };
}
async function upload(releaseId, asset) {
  const bytes = await readFile(join(directory, asset));
  let response;
  try {
    response = await fetch(`${uploadBase}/repos/${repository}/releases/${releaseId}/assets?name=${encodeURIComponent(asset)}`, { method: "POST", redirect: "error", headers: { accept: "application/vnd.github+json", authorization: `Bearer ${token}`, "content-type": "application/octet-stream" }, body: bytes });
  } catch { fail("GitHub asset upload failed without a readable response."); }
  if (!response.ok) fail(`GitHub asset upload failed with HTTP ${response.status}.`);
}
async function publicReadback(release) {
  const expected = [...manifest.assets.map((asset) => asset.name), "development-candidate.json", "SHA256SUMS.txt"].sort();
  const assets = Array.isArray(release.assets) ? release.assets : [];
  if (assets.length !== expected.length || assets.map((asset) => asset.name).sort().some((asset, index) => asset !== expected[index])) fail("existing release has an incomplete or extra asset inventory.");
  for (const asset of assets) {
    if (typeof asset.browser_download_url !== "string" || asset.browser_download_url.includes("\n")) fail("release asset URL is invalid.");
    let response;
    try { response = await fetch(asset.browser_download_url, { redirect: "manual", headers: { accept: "application/octet-stream" } }); } catch { fail("public readback failed without a readable response."); }
    if (response.status !== 200 || response.headers.get("location")) fail("public readback rejected a redirect or non-success response.");
    const actual = Buffer.from(await response.arrayBuffer());
    const expectedBytes = await readFile(join(directory, asset.name));
    if (!actual.equals(expectedBytes) || sha256(actual) !== sha256(expectedBytes)) fail(`public readback does not match ${asset.name}.`);
  }
}

const branch = await api(`/repos/${repository}/branches/develop`);
const environment = await api(`/repos/${repository}/environments/development-candidate`);
if (branch.status !== 200 || environment.status !== 200) fail("required protected branch or environment is unavailable.");
const rules = Array.isArray(environment.value.protection_rules) ? environment.value.protection_rules : [];
assertPreflight({ develop: { name: branch.value.name, protected: branch.value.protected === true }, environment: { name: environment.value.name, protected: true, reviewersRequired: rules.some((rule) => rule.type === "required_reviewers"), selectedBranches: "develop" }, releases: { immutable: process.env.DEVELOPMENT_CANDIDATE_RELEASES_IMMUTABLE === "true" } }, sourceSha);

const releaseResult = await api(`/repos/${repository}/releases/tags/${encodeURIComponent(tag)}`);
const refResult = await api(`/repos/${repository}/git/ref/tags/${encodeURIComponent(tag)}`);
if (releaseResult.status === 200) {
  if (refResult.status !== 200 || releaseResult.value.tag_name !== tag || releaseResult.value.target_commitish !== sourceSha || releaseResult.value.prerelease !== true || releaseResult.value.draft !== false) fail("existing candidate release is not the exact immutable collision.");
  await publicReadback(releaseResult.value);
  process.stdout.write(JSON.stringify({ result: "readback-only", tag, sourceSha }) + "\n");
} else {
  if (releaseResult.status !== 404 || refResult.status !== 404) fail("existing tag or release collision is not safe to create.");
  const created = await api(`/repos/${repository}/releases`, "POST", { tag_name: tag, target_commitish: sourceSha, name: `Service Lasso CLI development candidate ${version}`, prerelease: true, draft: false, generate_release_notes: false, body: "Checksum-bound development candidate. Not GA, deployment, Core qualification, or package publication." });
  if (created.status !== 201 || !Number.isSafeInteger(created.value?.id)) fail("immutable candidate release creation failed.");
  for (const asset of [...manifest.assets.map((asset) => asset.name), "development-candidate.json", "SHA256SUMS.txt"]) await upload(created.value.id, asset);
  const finalRelease = await api(`/repos/${repository}/releases/tags/${encodeURIComponent(tag)}`);
  if (finalRelease.status !== 200 || finalRelease.value.target_commitish !== sourceSha || finalRelease.value.draft !== false || finalRelease.value.prerelease !== true) fail("final immutable candidate release readback failed.");
  await publicReadback(finalRelease.value);
  process.stdout.write(JSON.stringify({ result: "published", tag, sourceSha }) + "\n");
}
