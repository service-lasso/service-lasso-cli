import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdir, mkdtemp, readFile, rename, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { assertProviderPreflight, canonicalPublicAssetUrl, expectedAssets, parseStrictJson, sha256, validateManifest, verifyCandidateDirectory } from "../scripts/protected-candidate-lib.mjs";
import { publishProtectedCandidate } from "../scripts/publish-protected-candidate.mjs";

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

async function candidateDirectory(root) {
  const portable = join(root, "portable");
  execFileSync(node, ["scripts/package-candidate.mjs", "--output", portable, "--version", version, "--source-sha", sourceSha], { encoding: "utf8" });
  const args = ["scripts/assemble-protected-candidate.mjs", "--output", join(root, "candidate"), "--portable", portable, "--version", version, "--source-sha", sourceSha];
  for (const target of [{ id: "win32-x64", platform: "win32", architecture: "x64" }, { id: "linux-x64", platform: "linux", architecture: "x64" }, { id: "darwin-arm64", platform: "darwin", architecture: "arm64" }]) args.push("--native", `${target.id}=${await nativeDirectory(root, target)}`);
  execFileSync(node, args, { encoding: "utf8" });
  return join(root, "candidate");
}

function json(value, status = 200) { return new Response(JSON.stringify(value), { status, headers: { "content-type": "application/json" } }); }

function providerPolicy() {
  return { immutable: { enabled: true }, branch: { name: "develop", protected: true }, protection: { required_status_checks: { strict: true, contexts: ["ci"] }, required_pull_request_reviews: { required_approving_review_count: 1 }, enforce_admins: { enabled: true }, allow_force_pushes: { enabled: false } }, environment: { name: "development-candidate", protection_rules: [{ type: "required_reviewers", reviewers: [{ type: "User", reviewer: { id: 1 } }] }], deployment_branch_policy: { custom_branch_policies: true, protected_branches: false } }, policies: { branch_policies: [{ name: "develop" }] } };
}

function responseBytes(bytes, { declared, chunks } = {}) {
  const values = chunks ?? [bytes];
  const stream = new ReadableStream({ start(controller) { for (const value of values) controller.enqueue(value); controller.close(); } });
  return new Response(stream, { status: 200, headers: declared === undefined ? {} : { "content-length": String(declared) } });
}

async function withCandidate(run) {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-adapter-"));
  try { await run(root, await candidateDirectory(root)); } finally { await rm(root, { recursive: true, force: true }); }
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
  const preflight = { immutable: { enabled: true }, branch: { name: "develop", protected: true }, protection: { required_status_checks: { strict: true, contexts: ["CI"] }, required_pull_request_reviews: { required_approving_review_count: 1 }, enforce_admins: { enabled: true }, allow_force_pushes: { enabled: false } }, environment: { name: "development-candidate", protection_rules: [{ type: "required_reviewers", reviewers: [{ type: "User", reviewer: { id: 1 } }] }], deployment_branch_policy: { custom_branch_policies: true, protected_branches: false } }, branchPolicies: [{ name: "develop" }] };
  assert.doesNotThrow(() => assertProviderPreflight(preflight));
  assert.throws(() => assertProviderPreflight({ ...preflight, immutable: { enabled: false } }));
  assert.throws(() => assertProviderPreflight({ ...preflight, protection: { ...preflight.protection, required_pull_request_reviews: null } }));
  assert.throws(() => assertProviderPreflight({ ...preflight, protection: { ...preflight.protection, required_pull_request_reviews: { required_approving_review_count: 0 } } }));
  assert.throws(() => assertProviderPreflight({ ...preflight, protection: { ...preflight.protection, enforce_admins: { enabled: false } } }));
  assert.doesNotThrow(() => canonicalPublicAssetUrl("https://github.com/service-lasso/service-lasso-cli/releases/download/x/service-lassoctl-a.tgz", "service-lassoctl-a.tgz", "x"));
  assert.throws(() => canonicalPublicAssetUrl("https://attacker.invalid/a", "a"));
});

test("CLI30 actual publisher verifies private asset IDs before its one publish transition and keeps bearer off public reads", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-publisher-"));
  try {
    const directory = await candidateDirectory(root), verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const events = [], assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    let draft = true;
    const fetchImpl = async (url, init) => {
      const parsed = new URL(url); events.push({ path: parsed.pathname, method: init.method, authorization: init.headers?.authorization });
      const policy = { immutable: { enabled: true }, branch: { name: "develop", protected: true }, protection: { required_status_checks: { strict: true, contexts: ["ci"] }, required_pull_request_reviews: { required_approving_review_count: 1 }, enforce_admins: { enabled: true }, allow_force_pushes: { enabled: false } }, environment: { name: "development-candidate", protection_rules: [{ type: "required_reviewers", reviewers: [{ type: "User", reviewer: { id: 1 } }] }], deployment_branch_policy: { custom_branch_policies: true, protected_branches: false } }, policies: { branch_policies: [{ name: "develop" }] } };
      if (parsed.hostname === "uploads.github.com") return json({ id: 99 }, 201);
      if (parsed.pathname.endsWith("/immutable-releases")) return json(policy.immutable);
      if (parsed.pathname.endsWith("/branches/develop")) return json(policy.branch);
      if (parsed.pathname.endsWith("/branches/develop/protection")) return json(policy.protection);
      if (parsed.pathname.endsWith("/environments/development-candidate")) return json(policy.environment);
      if (parsed.pathname.endsWith("/deployment-branch-policies")) return json(policy.policies);
      if (parsed.pathname.includes("/releases/tags/")) return json({}, 404);
      if (parsed.pathname.includes("/git/ref/tags/")) return json({}, 404);
      if (parsed.pathname.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201);
      if (parsed.pathname.endsWith("/git/refs")) return json({}, 201);
      if (parsed.pathname.endsWith("/releases") && init.method === "POST") return json({ id: 77 }, 201);
      if (parsed.pathname.endsWith("/releases/77") && init.method === "GET") return json({ id: 77, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets });
      if (parsed.pathname.endsWith("/releases/77") && init.method === "PATCH") { draft = false; return json({ id: 77, immutable: true, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets }); }
      const privateMatch = /\/releases\/assets\/(\d+)$/.exec(parsed.pathname); if (privateMatch) return new Response(null, { status: 302, headers: { location: `https://objects.githubusercontent.com/private/${privateMatch[1]}` } });
      const privateRedirect = /\/private\/(\d+)$/.exec(parsed.pathname); if (parsed.hostname === "objects.githubusercontent.com" && privateRedirect) return new Response(assets[Number(privateRedirect[1]) - 1].bytes, { status: 200 });
      const publicAsset = assets.find((asset) => parsed.href === asset.browser_download_url); if (publicAsset) return new Response(publicAsset.bytes, { status: 200 });
      throw new Error(`unexpected ${parsed.href}`);
    };
    const result = await publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl });
    assert.equal(result.result, "published");
    const firstWrite = events.findIndex((event) => event.method !== "GET"); assert.ok(firstWrite > 4);
    const publish = events.findIndex((event) => event.path.endsWith("/releases/77") && event.method === "PATCH");
    const privateReads = events.filter((event) => /\/releases\/assets\/\d+$/.test(event.path)); assert.equal(privateReads.length, assets.length); assert.ok(privateReads.every((event) => event.authorization === "Bearer secret-sentinel")); assert.ok(publish > events.lastIndexOf(privateReads.at(-1)));
    const privateRedirects = events.filter((event) => event.path.startsWith("/private/")); assert.equal(privateRedirects.length, assets.length); assert.ok(privateRedirects.every((event) => event.authorization === undefined));
    const publicReads = events.filter((event) => event.path.includes(`/releases/download/${tag}/`)); assert.equal(publicReads.length, assets.length); assert.ok(publicReads.every((event) => event.authorization === undefined));
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("CLI30 actual adapter rejects every unavailable or unsafe provider preflight before a write", async () => {
  await withCandidate(async (_root, directory) => {
    const variants = [
      ["immutable unavailable", "immutable", 404], ["branch unavailable", "branch", 404], ["protection unavailable", "protection", 404], ["environment unavailable", "environment", 404], ["policy unavailable", "policies", 404],
      ["reviews absent", "reviews", null], ["reviews empty", "reviews", { required_approving_review_count: 0 }], ["admin disabled", "admin", false], ["checks non-strict", "strict", false], ["checks empty", "contexts", []], ["force allowed", "force", true], ["branch unprotected", "protected", false], ["immutable disabled", "enabled", false], ["environment malformed", "environment", {}], ["branch policy wrong", "policies", { branch_policies: [{ name: "other" }] }]
    ];
    for (const [name, field, value] of variants) {
      const events = [], policy = providerPolicy();
      const fetchImpl = async (url, init) => {
        const path = new URL(url).pathname; events.push({ path, method: init.method });
        if (field === "immutable" && path.endsWith("/immutable-releases")) return json({}, value);
        if (field === "branch" && path.endsWith("/branches/develop")) return json({}, value);
        if (field === "protection" && path.endsWith("/branches/develop/protection")) return json({}, value);
        if (field === "environment" && path.endsWith("/environments/development-candidate")) return json(typeof value === "number" ? {} : (value ?? policy.environment), typeof value === "number" ? value : 200);
        if (field === "policies" && path.endsWith("/deployment-branch-policies")) return json(value ?? policy.policies, typeof value === "number" ? value : 200);
        if (path.endsWith("/immutable-releases")) { if (field === "enabled") policy.immutable.enabled = value; return json(policy.immutable); }
        if (path.endsWith("/branches/develop")) { if (field === "protected") policy.branch.protected = value; return json(policy.branch); }
        if (path.endsWith("/branches/develop/protection")) { if (field === "reviews") policy.protection.required_pull_request_reviews = value; if (field === "admin") policy.protection.enforce_admins.enabled = value; if (field === "strict") policy.protection.required_status_checks.strict = value; if (field === "contexts") policy.protection.required_status_checks.contexts = value; if (field === "force") policy.protection.allow_force_pushes.enabled = value; return json(policy.protection); }
        if (path.endsWith("/environments/development-candidate")) return json(policy.environment);
        if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        throw new Error(`unexpected ${name}`);
      };
      await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }), /provider protection|reviewed|administrator|immutable|protected|environment/);
      assert.equal(events.some((event) => event.method !== "GET"), false, name);
    }
  });
});

test("CLI30 actual adapter accepts only complete immutable annotated collisions and never writes them", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    const run = async ({ mutate = () => {}, annotated = true } = {}) => {
      const events = [], policy = providerPolicy(), release = { id: 9, immutable: true, tag_name: tag, draft: false, prerelease: true, target_commitish: sourceSha, assets };
      mutate(release); const fetchImpl = async (url, init) => {
        const parsed = new URL(url), path = parsed.pathname; events.push({ url: parsed.href, method: init.method, authorization: init.headers?.authorization });
        if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        if (path.includes("/releases/tags/")) return json(release); if (path.includes("/git/ref/tags/")) return json({ object: annotated ? { type: "tag", sha: "a".repeat(40) } : { type: "commit", sha: sourceSha } }); if (path.endsWith(`/git/tags/${"b".repeat(40)}`)) return json({ object: { type: "commit", sha: sourceSha } }); if (path.includes("/git/tags/")) return json({ object: { type: "tag", sha: "b".repeat(40) } });
        const publicAsset = assets.find((asset) => parsed.href === asset.browser_download_url); if (publicAsset) return responseBytes(publicAsset.bytes); throw new Error(`unexpected ${path}`);
      };
      return { events, result: await publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }) };
    };
    const exact = await run(); assert.equal(exact.result.result, "readback-only"); assert.equal(exact.events.some((event) => event.method !== "GET"), false); assert.equal(exact.events.filter((event) => event.url.includes("/releases/download/")).every((event) => event.authorization === undefined), true);
    for (const mutate of [release => { release.assets = release.assets.slice(1); }, release => { release.target_commitish = "f".repeat(40); }, release => { release.immutable = false; }, release => { release.draft = true; }]) await assert.rejects(() => run({ mutate }), /collision|readback|inventory/);
    await assert.rejects(() => run({ annotated: false }), /not annotated/);
  });
});

test("CLI30 actual adapter holds upload bytes, verifies every private asset, then publishes once", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    const events = [], policy = providerPolicy(); let draft = true, uploads = 0, published = 0;
    const fetchImpl = async (url, init) => {
      const parsed = new URL(url), path = parsed.pathname; events.push({ url: parsed.href, method: init.method, authorization: init.headers?.authorization, body: init.body });
      if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
      if (path.includes("/releases/tags/") || path.includes("/git/ref/tags/")) return json({}, 404); if (path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201); if (path.endsWith("/git/refs")) return json({}, 201); if (path.endsWith("/releases") && init.method === "POST") return json({ id: 77 }, 201);
      if (parsed.hostname === "uploads.github.com") { const name = parsed.searchParams.get("name"); assert.deepEqual(init.body, verified.held.get(name)); uploads += 1; if (uploads === 1) await writeFile(join(directory, name), "replaced after verification"); return json({}, 201); }
      if (path.endsWith("/releases/77") && init.method === "GET") return json({ id: 77, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets });
      if (path.endsWith("/releases/77") && init.method === "PATCH") { published += 1; draft = false; return json({ id: 77, immutable: true, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets }); }
      const privateId = /\/releases\/assets\/(\d+)$/.exec(path); if (privateId) return responseBytes(assets[Number(privateId[1]) - 1].bytes); const publicAsset = assets.find((asset) => asset.browser_download_url === parsed.href); if (publicAsset) return responseBytes(publicAsset.bytes); throw new Error(`unexpected ${path}`);
    };
    const result = await publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }); assert.equal(result.result, "published"); assert.equal(uploads, assets.length); assert.equal(published, 1);
    const publishIndex = events.findIndex((event) => event.method === "PATCH"), privateReads = events.filter((event) => /\/releases\/assets\/\d+$/.test(new URL(event.url).pathname)); assert.equal(privateReads.length, assets.length); assert.ok(events.findIndex((event) => event.method !== "GET") > 4); assert.ok(publishIndex > events.lastIndexOf(privateReads.at(-1))); assert.equal(events.filter((event) => event.url.includes("/releases/download/")).every((event) => event.authorization === undefined), true);
  });
});

test("CLI30 actual adapter fails draft inventory and byte defects before publication", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    for (const defect of ["missing", "duplicate-id", "wrong-bytes", "invalid-id"]) {
      const assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: defect === "duplicate-id" ? 1 : defect === "invalid-id" ? 0 : index + 1, name, bytes: defect === "wrong-bytes" && index === 0 ? Buffer.from("wrong") : bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` })); if (defect === "missing") assets.pop();
      const policy = providerPolicy(); let patch = 0;
      const fetchImpl = async (url, init) => { const parsed = new URL(url), path = parsed.pathname;
        if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies); if (path.includes("/releases/tags/") || path.includes("/git/ref/tags/")) return json({}, 404); if (path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201); if (path.endsWith("/git/refs")) return json({}, 201); if (path.endsWith("/releases") && init.method === "POST") return json({ id: 77 }, 201); if (parsed.hostname === "uploads.github.com") return json({}, 201); if (path.endsWith("/releases/77") && init.method === "GET") return json({ id: 77, tag_name: tag, draft: true, prerelease: true, target_commitish: sourceSha, assets }); if (path.endsWith("/releases/77") && init.method === "PATCH") { patch += 1; return json({}, 200); } const match = /\/releases\/assets\/(\d+)$/.exec(path); if (match) return responseBytes(assets[Number(match[1]) - 1]?.bytes ?? Buffer.from("")); throw new Error(`unexpected ${path}`); };
      await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl })); assert.equal(patch, 0, defect);
    }
  });
});

test("CLI30 actual adapter bounds raw metadata streams and aborts stalled fetches and bodies without writes", async () => {
  await withCandidate(async (_root, directory) => {
    const cases = [
      ["duplicate raw JSON", () => new Response('{"enabled":true,"enabled":true}', { status: 200 })],
      ["malformed length", () => new Response("{}", { status: 200, headers: { "content-length": "not-a-number" } })],
      ["declared mismatch", () => new Response("{}", { status: 200, headers: { "content-length": "99" } })],
      ["stream overflow", () => responseBytes(Buffer.alloc(1024 * 1024 + 1), { chunks: [Buffer.alloc(1024 * 1024), Buffer.alloc(1)] })]
    ];
    for (const [name, first] of cases) {
      const policy = providerPolicy(), events = [];
      const fetchImpl = async (url, init) => { const path = new URL(url).pathname; events.push(init.method); if (path.endsWith("/immutable-releases")) return first(); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies); throw new Error("unreachable"); };
      await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl })); assert.equal(events.some((method) => method !== "GET"), false, name);
    }
    for (const kind of ["fetch", "body"]) {
      const policy = providerPolicy(); let aborted = false, cancelled = false; const timers = [];
      const fetchImpl = (url, init) => { const path = new URL(url).pathname; if (path.endsWith("/immutable-releases")) {
        if (kind === "fetch") return new Promise((_resolve, reject) => init.signal.addEventListener("abort", () => { aborted = true; reject(new Error("abort")); }, { once: true }));
        return new Response(new ReadableStream({ start() { setImmediate(() => timers.forEach((callback) => callback())); }, cancel() { cancelled = true; } }), { status: 200 });
      } if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies); throw new Error("unreachable"); };
      const requestOptions = { deadlineMs: 1, setTimer(callback) { timers.push(callback); if (kind === "fetch") queueMicrotask(callback); return callback; }, clearTimer() {} };
      await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl, requestOptions }), /request failed/); await new Promise((resolve) => setImmediate(resolve)); if (kind === "fetch") assert.equal(aborted, true); else assert.equal(cancelled, true);
    }
  });
});

test("CLI30 actual adapter strips bearer on constrained redirects and rejects malicious locations without leaking sentinels", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag, assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    const run = async (location) => { const policy = providerPolicy(), events = []; const fetchImpl = async (url, init) => { const parsed = new URL(url), path = parsed.pathname; events.push({ url: parsed.href, authorization: init.headers?.authorization, method: init.method }); if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies); if (path.includes("/releases/tags/")) return json({ id: 9, immutable: true, tag_name: tag, draft: false, prerelease: true, target_commitish: sourceSha, assets }); if (path.includes("/git/ref/tags/")) return json({ object: { type: "tag", sha: "a".repeat(40) } }); if (path.endsWith(`/git/tags/${"a".repeat(40)}`)) return json({ object: { type: "commit", sha: sourceSha } }); const match = /\/releases\/download\/.+\/(.+)$/.exec(path); if (match) return new Response(null, { status: 302, headers: { location: location.replace("{name}", match[1]) } }); if (parsed.hostname === "objects.githubusercontent.com") return responseBytes(assets.find((asset) => path.endsWith(encodeURIComponent(asset.name))).bytes); throw new Error("unreachable"); }; return { events, value: publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }) }; };
    const allowed = await run("https://objects.githubusercontent.com/private-object/{name}"); await allowed.value; const redirected = allowed.events.find((event) => event.url.includes("objects.githubusercontent.com")); assert.equal(redirected.authorization, undefined);
    for (const bad of ["http://objects.githubusercontent.com/a", "https://attacker.invalid/a", "https://objects.githubusercontent.com:444/a", "https://objects.githubusercontent.com/a#fragment"]) { const outcome = await run(bad); await assert.rejects(() => outcome.value, /redirect|public/); }
  });
});
