import { runScopedCli } from './cli-platform-scope.mjs';
import { assertScopedTagPolicy } from './scoped-tag-policy.mjs';
import { assertScopedWorkflowSource } from './bind-scoped-release-source.mjs';
import { assertScopedRunEvidence, assertScopedCiEvidence } from './protected-scoped-candidate-lib.mjs';
import { readSourceScope, scopedArguments } from './cli-platform-scope.mjs';
import { assertProviderPreflight, canonicalPublicAssetUrl, allowedRedirect, fail, parseStrictJson, sha256, verifyCandidateDirectory } from "./protected-scoped-candidate-lib.mjs";
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

export async function publishProtectedScopedCandidate({ directory, version, sourceSha, repository = REPOSITORY, token, workflowRun, fetchImpl = fetch, requestOptions = {} }) {
  if (!workflowRun || !Number.isSafeInteger(workflowRun.id) || workflowRun.id < 1 ||
      !Number.isSafeInteger(workflowRun.attempt) || workflowRun.attempt < 1 || workflowRun.workflowSha !== sourceSha)
    fail("actual scoped current run/attempt context is required.");
  if (!token) fail("the protected environment token is unavailable.");
  if (repository !== REPOSITORY) fail("repository endpoint is not allowed.");
  const { manifest, held } = await verifyCandidateDirectory(directory, version, sourceSha), tag = manifest.candidateTag;
  let annotatedOid = null, refCreated = false, expectAbsent = false;
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
      if (!Number.isSafeInteger(asset?.id) || asset.id < 1 || ids.has(asset.id) || asset.size !== held.get(asset.name).length) fail("release asset inventory has an invalid asset id."); ids.add(asset.id);
      const url = privateAssets ? `${API}/repos/${repository}/releases/assets/${asset.id}` : canonicalPublicAssetUrl(asset.browser_download_url, asset.name, tag).toString();
      const result = await fetchedAsset(url, privateAssets, held.get(asset.name).length);
      if (result.response.status !== 200 || result.body.length !== held.get(asset.name).length || !result.body.equals(held.get(asset.name)) || sha256(result.body) !== sha256(held.get(asset.name))) fail(`asset readback does not match ${asset.name}.`);
    }
  };
  const tagBinding = async () => {
    if (!refCreated && !expectAbsent) return; // initial collision is handled below
    const ref = await api(`/repos/${repository}/git/ref/tags/${encodeURIComponent(tag)}`);
    if (!refCreated) { if (ref.status !== 404) fail('candidate reference appeared before creation.'); return; }
    if (ref.status !== 200 || ref.value?.ref !== `refs/tags/${tag}` || ref.value?.object?.type !== 'tag' ||
        ref.value.object.sha !== annotatedOid) fail('candidate annotated reference changed.');
    let object = ref.value.object;
    const seen = new Set();
    for (let depth = 0; depth < 4 && object?.type === 'tag'; depth++) {
      if (!FULL_SHA.test(object.sha) || seen.has(object.sha)) fail('candidate annotated chain is invalid.');
      seen.add(object.sha);
      const heldObject = await api(`/repos/${repository}/git/tags/${object.sha}`);
      if (heldObject.status !== 200 || heldObject.value?.sha !== object.sha || (depth === 0 && heldObject.value?.tag !== tag))
        fail('candidate annotated object changed.');
      object = heldObject.value?.object;
    }
    if (object?.type !== 'commit' || object.sha !== sourceSha) fail('candidate annotated object does not bind the frozen source.');
  };
  const preflight = async () => {
    await readSourceScope();
    const [immutable, branch, protection, environment, policies, latest, attempt, jobs] = await Promise.all([api(`/repos/${repository}/immutable-releases`), api(`/repos/${repository}/branches/develop`), api(`/repos/${repository}/branches/develop/protection`), api(`/repos/${repository}/environments/development-candidate`), api(`/repos/${repository}/environments/development-candidate/deployment-branch-policies`),
      api(`/repos/${repository}/actions/runs/${workflowRun.id}`),
      api(`/repos/${repository}/actions/runs/${workflowRun.id}/attempts/${workflowRun.attempt}`),
      api(`/repos/${repository}/actions/runs/${workflowRun.id}/attempts/${workflowRun.attempt}/jobs?per_page=100`)]);
    if ([immutable, branch, protection, environment, policies, latest, attempt, jobs].some((value) => value.status !== 200)) fail("required GitHub provider protection is unavailable.");
    assertScopedRunEvidence({ latest: latest.value, attempt: attempt.value, jobs: jobs.value }, workflowRun, sourceSha);
    const ciList = await api(`/repos/${repository}/actions/workflows/ci.yml/runs?branch=develop&event=push&head_sha=${sourceSha}&per_page=2`);
    if (ciList.status !== 200 || ciList.value?.total_count !== 1 || !Array.isArray(ciList.value.workflow_runs) || ciList.value.workflow_runs.length !== 1)
      fail('one actual natural develop CI run is required for the frozen source.');
    const listed = ciList.value.workflow_runs[0];
    if (!Number.isSafeInteger(listed?.id) || listed.id < 1 || !Number.isSafeInteger(listed.run_attempt) || listed.run_attempt < 1)
      fail('natural CI run identity is invalid.');
    const [ciLatest, ciAttempt, ciJobs] = await Promise.all([
      api(`/repos/${repository}/actions/runs/${listed.id}`),
      api(`/repos/${repository}/actions/runs/${listed.id}/attempts/${listed.run_attempt}`),
      api(`/repos/${repository}/actions/runs/${listed.id}/attempts/${listed.run_attempt}/jobs?per_page=100`),
    ]);
    if ([ciLatest, ciAttempt, ciJobs].some(value => value.status !== 200)) fail('current selected CI provider evidence is unavailable.');
    assertScopedCiEvidence({ listed, latest: ciLatest.value, attempt: ciAttempt.value, jobs: ciJobs.value }, sourceSha);
    if (branch.value?.commit?.sha !== sourceSha) fail("develop moved from the frozen scoped candidate.");
    assertProviderPreflight({ immutable: immutable.value, branch: branch.value, protection: protection.value, environment: environment.value, branchPolicies: policies.value?.branch_policies });
    const list = await api(`/repos/${repository}/rulesets?includes_parents=true&per_page=100`);
    if (list.status !== 200 || !Array.isArray(list.value) || list.value.length >= 100) fail('complete effective tag rulesets are unavailable.');
    const effective = [];
    for (const summary of list.value) {
      if (!Number.isSafeInteger(summary?.id) || summary.id < 1) fail('effective ruleset identity is invalid.');
      const detail = await api(`/repos/${repository}/rulesets/${summary.id}?includes_parents=true`);
      if (detail.status !== 200 || detail.value?.id !== summary.id) fail('effective ruleset readback is unavailable.');
      effective.push(detail.value);
    }
    assertScopedTagPolicy(effective, tag);
    // Provider enforcement excludes ordinary ref writers. These current reads
    // are not an atomic transaction against control administration.
    await tagBinding();
  };
  await preflight();
  const [release, ref] = await Promise.all([api(`/repos/${repository}/releases/tags/${encodeURIComponent(tag)}`), api(`/repos/${repository}/git/ref/tags/${encodeURIComponent(tag)}`)]);
  if (release.status === 200) {
    if (!Number.isSafeInteger(release.value?.id) || release.value.id < 1 || ref.status !== 200 || release.value?.immutable !== true || release.value?.tag_name !== tag || release.value?.target_commitish !== sourceSha || release.value?.prerelease !== true || release.value?.draft !== false) fail("existing candidate collision is not exact.");
    let object = ref.value?.object; if (object?.type !== "tag") fail("candidate tag is not annotated."); for (let depth = 0; depth < 4 && object?.type === "tag"; depth += 1) { if (!FULL_SHA.test(object.sha)) fail("candidate tag identity is invalid."); const tagObject = await api(`/repos/${repository}/git/tags/${object.sha}`); if (tagObject.status !== 200) fail("candidate tag cannot be dereferenced."); object = tagObject.value?.object; }
    if (object?.type !== "commit" || object.sha !== sourceSha) fail("candidate tag does not resolve to frozen commit.");
    annotatedOid = ref.value.object.sha; refCreated = true;
    await tagBinding(); await inventory(release.value, false); await tagBinding(); return { result: "readback-only", tag, sourceSha };
  }
  if (release.status !== 404 || ref.status !== 404) fail("existing tag or release collision is not safe to create.");
  expectAbsent = true;
  const annotated = await api(`/repos/${repository}/git/tags`, "POST", { tag, message: `Protected development candidate ${version}`, object: sourceSha, type: "commit", tagger: { name: "service-lasso candidate publisher", email: "noreply@service-lasso.invalid", date: new Date().toISOString() } });
  if (annotated.status !== 201 || !FULL_SHA.test(annotated.value?.sha)) fail("annotated candidate tag creation failed.");
  annotatedOid = annotated.value.sha;
  const tagRef = await api(`/repos/${repository}/git/refs`, "POST", { ref: `refs/tags/${tag}`, sha: annotated.value.sha }); if (tagRef.status !== 201) fail("annotated candidate tag reference creation failed.");
  refCreated = true;
  await tagBinding();
  const created = await api(`/repos/${repository}/releases`, "POST", { tag_name: tag, target_commitish: sourceSha, name: `Service Lasso CLI development candidate ${version}`, prerelease: true, draft: true, generate_release_notes: false, body: "Checksum-bound development candidate. Not GA, deployment, Core qualification, or package publication." });
  if (created.status !== 201 || !Number.isSafeInteger(created.value?.id) || created.value.id < 1 || created.value?.tag_name !== tag || created.value?.target_commitish !== sourceSha || created.value?.draft !== true || created.value?.prerelease !== true || !Array.isArray(created.value?.assets) || created.value.assets.length !== 0) fail("candidate draft creation did not produce the exact empty private release.");
  for (const name of held.keys()) await upload(created.value.id, name);
  const staged = await api(`/repos/${repository}/releases/${created.value.id}`); if (staged.status !== 200 || staged.value?.id !== created.value.id || staged.value?.tag_name !== tag || staged.value?.draft !== true || staged.value?.prerelease !== true || staged.value?.target_commitish !== sourceSha) fail("private candidate verification failed."); await inventory(staged.value, true);
  const published = await api(`/repos/${repository}/releases/${created.value.id}`, "PATCH", { draft: false, prerelease: true, target_commitish: sourceSha }); if (published.status !== 200 || published.value?.id !== created.value.id || published.value?.immutable !== true || published.value?.tag_name !== tag || published.value?.draft !== false || published.value?.target_commitish !== sourceSha || published.value?.prerelease !== true) fail("immutable candidate publication failed.");
  await tagBinding();
  const immutableReadback = await api(`/repos/${repository}/releases/${created.value.id}`);
  if (immutableReadback.status !== 200 || immutableReadback.value?.id !== created.value.id || immutableReadback.value?.immutable !== true ||
      immutableReadback.value?.draft !== false || immutableReadback.value?.prerelease !== true || immutableReadback.value?.tag_name !== tag ||
      immutableReadback.value?.target_commitish !== sourceSha) fail('immutable publication readback changed.');
  await inventory(immutableReadback.value, false); await tagBinding(); return { result: "published", tag, sourceSha };
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  await runScopedCli(async () => {
  const scopedArgs = scopedArguments(process.argv.slice(2), ['--directory', '--version', '--source-sha', '--repository']);
  const workflow = await assertScopedWorkflowSource(process.env);
  const result = await publishProtectedScopedCandidate({ directory: resolve(scopedArgs.one("--directory")), version: scopedArgs.one("--version"), sourceSha: scopedArgs.one("--source-sha"), repository: scopedArgs.one("--repository"), token: process.env.DEVELOPMENT_CANDIDATE_TOKEN,
    workflowRun: { id: workflow.runId, attempt: workflow.runAttempt, workflowSha: workflow.sourceSha } });
  process.stdout.write(`${JSON.stringify(result)}\n`);
  });
}
