import { assertProviderPreflight, canonicalPublicAssetUrl, allowedRedirect, fail, parseStrictJson, sha256, verifyCandidateDirectory } from "./protected-candidate-lib.mjs";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const API = "https://api.github.com", UPLOADS = "https://uploads.github.com", REPOSITORY = "service-lasso/service-lasso-cli";
const METADATA_LIMIT = 1024 * 1024, ASSET_LIMIT = 256 * 1024 * 1024, DEADLINE_MS = 30_000, FULL_SHA = /^[0-9a-f]{40}$/i;
const argument = (name, args) => { const index = args.indexOf(name); if (index < 0 || !args[index + 1]) throw new Error(`Missing ${name}.`); return args[index + 1]; };
const headers = (token, extra = {}) => ({ accept: "application/vnd.github+json", authorization: `Bearer ${token}`, ...extra });

async function bytes(response, limit, label, signal) {
  const length = response.headers?.get("content-length");
  const declared = length === null ? null : Number(length);
  if (length !== null && (!/^\d+$/.test(length) || !Number.isSafeInteger(declared) || declared > limit)) fail(`${label} exceeds its byte limit.`);
  const reader = response.body?.getReader?.();
  if (!reader) fail(`${label} has no bounded streaming body.`);
  const parts = []; let total = 0;
  const expired = signal?.aborted ? Promise.reject(new Error("deadline")) : new Promise((_, reject) => signal?.addEventListener("abort", () => reject(new Error("deadline")), { once: true }));
  try { while (true) {
    const next = await Promise.race([reader.read(), expired]);
    if (!next || typeof next !== "object" || typeof next.done !== "boolean") fail(`${label} has an invalid stream result.`);
    if (next.done) break;
    // The Fetch body reader yields Uint8Array chunks.  Validate both the type
    // and the prospective total before making a defensive copy with Buffer.
    const value = next.value;
    if (!(value instanceof Uint8Array) || !Number.isSafeInteger(value.byteLength) || value.byteLength < 0) fail(`${label} has an invalid stream chunk.`);
    if (value.byteLength > limit - total) fail(`${label} exceeds its byte limit.`);
    const part = Buffer.from(value);
    total += part.length;
    parts.push(part);
  } } catch (error) { void reader.cancel?.().catch?.(() => {}); throw error; } finally { reader.releaseLock?.(); }
  if (declared !== null && total !== declared) fail(`${label} content length does not match its body.`);
  return Buffer.concat(parts, total);
}

function discardRedirectBody(response) {
  const reader = response.body?.getReader?.();
  if (!reader) return;
  try { void Promise.resolve(reader.cancel?.()).catch(() => {}); } catch { /* A redirect body is never trusted or read. */ }
  try { reader.releaseLock?.(); } catch { /* Disposal must not delay the request boundary. */ }
}

async function request(fetchImpl, url, init, label, limit, { allowRedirect = false, deadlineMs = DEADLINE_MS, setTimer = setTimeout, clearTimer = clearTimeout } = {}) {
  if (!Number.isSafeInteger(deadlineMs) || deadlineMs < 1 || deadlineMs > DEADLINE_MS) fail("request deadline is invalid.");
  const controller = new AbortController(), timer = setTimer(() => controller.abort(), deadlineMs);
  let response;
  try { const expired = controller.signal.aborted ? Promise.reject(new Error("deadline")) : new Promise((_, reject) => controller.signal.addEventListener("abort", () => reject(new Error("deadline")), { once: true })); response = await Promise.race([fetchImpl(url, { ...init, signal: controller.signal }), expired]); if (!response || !Number.isSafeInteger(response.status) || response.status < 100 || response.status > 599) fail(`${label} has an invalid response.`); if (allowRedirect && response.status >= 300 && response.status < 400) { discardRedirectBody(response); return { kind: "redirect", response }; } return { kind: "body", response, body: await bytes(response, limit, label, controller.signal) }; } catch (error) { if (error instanceof Error && error.message.startsWith("Protected candidate rejected:")) throw error; fail(`${label} request failed.`); } finally { clearTimer(timer); }
}

export async function publishProtectedCandidate({ directory, version, sourceSha, repository = REPOSITORY, token, fetchImpl = fetch, requestOptions = {} }) {
  if (!token) fail("the protected environment token is unavailable.");
  if (repository !== REPOSITORY) fail("repository endpoint is not allowed.");
  const { manifest, held } = await verifyCandidateDirectory(directory, version, sourceSha), tag = manifest.candidateTag;
  const api = async (path, method = "GET", body) => {
    if (method !== "GET") await preflight();
    const result = await request(fetchImpl, `${API}${path}`, { method, redirect: "error", headers: headers(token, body ? { "content-type": "application/json" } : {}), body: body ? JSON.stringify(body) : undefined }, "GitHub metadata", METADATA_LIMIT, requestOptions);
    let value = null; if (result.body.length) value = parseStrictJson(result.body, "GitHub metadata");
    return { status: result.response.status, value };
  };
  const upload = async (id, name) => { await preflight(); const result = await request(fetchImpl, `${UPLOADS}/repos/${repository}/releases/${id}/assets?name=${encodeURIComponent(name)}`, { method: "POST", redirect: "error", headers: headers(token, { "content-type": "application/octet-stream" }), body: held.get(name) }, "GitHub asset upload", METADATA_LIMIT, requestOptions); if (!result.response.ok) fail("GitHub asset upload failed."); };
  const fetchedAsset = async (url, privateAsset, expectedBytes) => {
    const first = await request(fetchImpl, url, { method: "GET", redirect: "manual", headers: privateAsset ? headers(token, { accept: "application/octet-stream" }) : { accept: "application/octet-stream" } }, privateAsset ? "private asset readback" : "public readback", expectedBytes, { ...requestOptions, allowRedirect: true });
    if (first.kind === "body") return first;
    const location = first.response.headers?.get("location"); if (!location) fail("asset readback redirect has no location."); allowedRedirect(location);
    return request(fetchImpl, location, { method: "GET", redirect: "error", headers: { accept: "application/octet-stream" } }, privateAsset ? "private asset readback" : "public readback", expectedBytes, requestOptions);
  };
  const inventory = async (release, privateAssets) => {
    const names = [...held.keys()].sort(), assets = release?.assets;
    if (!Array.isArray(assets) || assets.length !== names.length || assets.map((asset) => asset.name).sort().some((name, index) => name !== names[index])) fail("release asset inventory is incomplete or extra.");
    const ids = new Set();
    for (const asset of assets) {
      if (!Number.isSafeInteger(asset?.id) || asset.id < 1 || ids.has(asset.id)) fail("release asset inventory has an invalid asset id."); ids.add(asset.id);
      const url = privateAssets ? `${API}/repos/${repository}/releases/assets/${asset.id}` : canonicalPublicAssetUrl(asset.browser_download_url, asset.name, tag).toString();
      const result = await fetchedAsset(url, privateAssets, held.get(asset.name).length);
      if (result.response.status !== 200 || result.body.length !== held.get(asset.name).length || !result.body.equals(held.get(asset.name)) || sha256(result.body) !== sha256(held.get(asset.name))) fail(`asset readback does not match ${asset.name}.`);
    }
  };
  const preflight = async () => {
    const [immutable, branch, protection, environment, policies] = await Promise.all([api(`/repos/${repository}/immutable-releases`), api(`/repos/${repository}/branches/develop`), api(`/repos/${repository}/branches/develop/protection`), api(`/repos/${repository}/environments/development-candidate`), api(`/repos/${repository}/environments/development-candidate/deployment-branch-policies`)]);
    if ([immutable, branch, protection, environment, policies].some((value) => value.status !== 200)) fail("required GitHub provider protection is unavailable.");
    assertProviderPreflight({ immutable: immutable.value, branch: branch.value, protection: protection.value, environment: environment.value, branchPolicies: policies.value?.branch_policies });
  };
  await preflight();
  const [release, ref] = await Promise.all([api(`/repos/${repository}/releases/tags/${encodeURIComponent(tag)}`), api(`/repos/${repository}/git/ref/tags/${encodeURIComponent(tag)}`)]);
  if (release.status === 200) {
    if (ref.status !== 200 || release.value?.immutable !== true || release.value?.tag_name !== tag || release.value?.target_commitish !== sourceSha || release.value?.prerelease !== true || release.value?.draft !== false) fail("existing candidate collision is not exact.");
    let object = ref.value?.object; if (object?.type !== "tag") fail("candidate tag is not annotated."); for (let depth = 0; depth < 4 && object?.type === "tag"; depth += 1) { if (!FULL_SHA.test(object.sha)) fail("candidate tag identity is invalid."); const tagObject = await api(`/repos/${repository}/git/tags/${object.sha}`); if (tagObject.status !== 200) fail("candidate tag cannot be dereferenced."); object = tagObject.value?.object; }
    if (object?.type !== "commit" || object.sha !== sourceSha) fail("candidate tag does not resolve to frozen commit."); await inventory(release.value, false); return { result: "readback-only", tag, sourceSha };
  }
  if (release.status !== 404 || ref.status !== 404) fail("existing tag or release collision is not safe to create.");
  const annotated = await api(`/repos/${repository}/git/tags`, "POST", { tag, message: `Protected development candidate ${version}`, object: sourceSha, type: "commit", tagger: { name: "service-lasso candidate publisher", email: "noreply@service-lasso.invalid", date: new Date().toISOString() } });
  if (annotated.status !== 201 || !FULL_SHA.test(annotated.value?.sha)) fail("annotated candidate tag creation failed.");
  const tagRef = await api(`/repos/${repository}/git/refs`, "POST", { ref: `refs/tags/${tag}`, sha: annotated.value.sha }); if (tagRef.status !== 201) fail("annotated candidate tag reference creation failed.");
  const created = await api(`/repos/${repository}/releases`, "POST", { tag_name: tag, target_commitish: sourceSha, name: `Service Lasso CLI development candidate ${version}`, prerelease: true, draft: true, generate_release_notes: false, body: "Checksum-bound development candidate. Not GA, deployment, Core qualification, or package publication." });
  if (created.status !== 201 || !Number.isSafeInteger(created.value?.id) || created.value.id < 1 || created.value?.tag_name !== tag || created.value?.target_commitish !== sourceSha || created.value?.draft !== true || created.value?.prerelease !== true || !Array.isArray(created.value?.assets) || created.value.assets.length !== 0) fail("candidate draft creation did not produce the exact empty private release.");
  for (const name of held.keys()) await upload(created.value.id, name);
  const staged = await api(`/repos/${repository}/releases/${created.value.id}`); if (staged.status !== 200 || staged.value?.id !== created.value.id || staged.value?.tag_name !== tag || staged.value?.draft !== true || staged.value?.prerelease !== true || staged.value?.target_commitish !== sourceSha) fail("private candidate verification failed."); await inventory(staged.value, true);
  const published = await api(`/repos/${repository}/releases/${created.value.id}`, "PATCH", { draft: false, prerelease: true, target_commitish: sourceSha }); if (published.status !== 200 || published.value?.id !== created.value.id || published.value?.immutable !== true || published.value?.tag_name !== tag || published.value?.draft !== false || published.value?.target_commitish !== sourceSha || published.value?.prerelease !== true) fail("immutable candidate publication failed."); await inventory(published.value, false); return { result: "published", tag, sourceSha };
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  const result = await publishProtectedCandidate({ directory: resolve(argument("--directory", process.argv)), version: argument("--version", process.argv), sourceSha: argument("--source-sha", process.argv), repository: argument("--repository", process.argv), token: process.env.DEVELOPMENT_CANDIDATE_TOKEN });
  process.stdout.write(`${JSON.stringify(result)}\n`);
}
