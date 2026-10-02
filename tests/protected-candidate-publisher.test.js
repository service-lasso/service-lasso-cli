import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdir, mkdtemp, readFile, rename, rm, writeFile, truncate } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join, resolve } from "node:path";
import test from "node:test";
import { assertProviderPreflight, canonicalPublicAssetUrl, expectedAssets, parseStrictJson, readHeldFile, sha256, validateManifest, verifyCandidateDirectory } from "../scripts/protected-candidate-lib.mjs";
import { publishProtectedCandidate } from "../scripts/publish-protected-candidate.mjs";

const node = process.execPath;
const sourceSha = "0123456789abcdef0123456789abcdef01234567";
const version = "0.1.0-dev.0123456";

async function nativeDirectory(root, target, environment = process.env) {
  const directory = join(root, target.id);
  await mkdir(directory, { recursive: true });
  const executable = target.id === "win32-x64" ? "service-lassoctl.exe" : "service-lassoctl";
  const confinedWriter = target.confinedWriter ?? (target.id === "win32-x64" ? "service-lasso-confined-scaffold.exe" : "service-lasso-confined-scaffold");
  const bytes = Buffer.from(`native-${target.id}`);
  const writerBytes = Buffer.from(`confined-writer-${target.id}`);
  await writeFile(join(directory, executable), bytes);
  await writeFile(join(directory, confinedWriter), writerBytes);
  const immutableBytes = Buffer.from(`immutable-helper-${target.id}`);
  if (target.id === "darwin-arm64") await writeFile(join(directory, "service-lasso-darwin-immutable-helper"), immutableBytes);
  const digest = sha256(bytes);
  await writeFile(join(directory, "provenance.json"), `${JSON.stringify({ schemaVersion: 1, command: "service-lassoctl", candidate: { version, tag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}` }, source: { commit: sourceSha }, executable: { name: executable, sha256: digest, platform: target.platform, architecture: target.architecture, version }, confinedWriter: { name: confinedWriter, sha256: sha256(writerBytes), sourceSha256: "b".repeat(64), platform: target.platform, architecture: target.architecture }, ...(target.id === "darwin-arm64" ? { darwinImmutableHelper: { name: "service-lasso-darwin-immutable-helper", sha256: sha256(immutableBytes), sourceSha256: "d".repeat(64), platform: "darwin", architecture: "arm64" } } : {}), tools: {}, sea: {} })}\n`);
  await writeFile(join(directory, "ci-context.json"), `${JSON.stringify({ schemaVersion: 1, eventName: "workflow_dispatch", sourceSha, testedBaseSha: null, mergeContextSha: sourceSha })}\n`);
  const evidenceDigest = sha256(Buffer.from(`service-lasso-native-acceptance-v1\n${sourceSha}\n${version}\n${target.platform}\n${target.architecture}\n${digest}\nnode-absent\npassed\n`, "utf8"));
  await writeFile(join(directory, "host-acceptance.json"), `${JSON.stringify({ schemaVersion: 1, sourceSha, version, platform: target.platform, architecture: target.architecture, executableSha256: digest, nodeAbsentFromPath: true, status: "passed", evidenceDigest })}\n`);
  execFileSync(node, ["scripts/archive-native-candidate.mjs", "--expected-event", "workflow_dispatch", "--expected-base-sha", "", "--expected-merge-context-sha", sourceSha, "--directory", directory, "--output", directory, "--version", version, "--source-sha", sourceSha], { encoding: "utf8", env: environment });
  await rm(join(directory, executable));
  await rm(join(directory, confinedWriter));
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
  for (const target of [{ id: "win32-x64", platform: "win32", architecture: "x64", confinedWriter: "service-lasso-confined-scaffold.exe" }, { id: "linux-x64", platform: "linux", architecture: "x64", confinedWriter: "service-lasso-confined-scaffold" }, { id: "darwin-arm64", platform: "darwin", architecture: "arm64", confinedWriter: "service-lasso-confined-scaffold" }]) args.push("--native", `${target.id}=${await nativeDirectory(root, target)}`);
  execFileSync(node, args, { encoding: "utf8" });
  return join(root, "candidate");
}

function json(value, status = 200) { return new Response(JSON.stringify(value), { status, headers: { "content-type": "application/json" } }); }

function providerPolicy() {
  return { immutable: { enabled: true }, branch: { name: "develop", protected: true }, protection: { required_status_checks: { strict: true, contexts: ["ci"] }, required_pull_request_reviews: { required_approving_review_count: 0, bypass_pull_request_allowances: { users: [], teams: [], apps: [] } }, enforce_admins: { enabled: true }, allow_force_pushes: { enabled: false } }, environment: { name: "development-candidate", protection_rules: [{ type: "wait_timer", wait_timer: 1 }], deployment_branch_policy: { custom_branch_policies: true, protected_branches: false } }, policies: { branch_policies: [{ name: "develop" }] } };
}

function emptyPrivateRelease(id, tag) { return { id, tag_name: tag, draft: true, prerelease: true, target_commitish: sourceSha, assets: [] }; }

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

test("CLI30 candidate pack preserves a literal spaced output path", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl spaced-parent-"));
  const portable = join(root, "portable output with spaces");
  try {
    execFileSync(node, ["scripts/package-candidate.mjs", "--output", portable, "--version", version, "--source-sha", sourceSha], { encoding: "utf8" });
    assert.equal((await readFile(join(portable, "candidate.json"), "utf8")).includes(`"version": "${version}"`), true);
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
  const preflight = { immutable: { enabled: true }, branch: { name: "develop", protected: true }, protection: { required_status_checks: { strict: true, contexts: ["CI"] }, required_pull_request_reviews: { required_approving_review_count: 0, bypass_pull_request_allowances: { users: [], teams: [], apps: [] } }, enforce_admins: { enabled: true }, allow_force_pushes: { enabled: false } }, environment: { name: "development-candidate", protection_rules: [{ type: "wait_timer", wait_timer: 1 }], deployment_branch_policy: { custom_branch_policies: true, protected_branches: false } }, branchPolicies: [{ name: "develop" }] };
  assert.doesNotThrow(() => assertProviderPreflight(preflight));
  assert.throws(() => assertProviderPreflight({ ...preflight, immutable: { enabled: false } }));
  assert.throws(() => assertProviderPreflight({ ...preflight, protection: { ...preflight.protection, required_pull_request_reviews: null } }));
  assert.throws(() => assertProviderPreflight({ ...preflight, protection: { ...preflight.protection, required_pull_request_reviews: { required_approving_review_count: 0 } } }));
  assert.throws(() => assertProviderPreflight({ ...preflight, protection: { ...preflight.protection, enforce_admins: { enabled: false } } }));
  for (const bypasses of [undefined, null, {}, { users: [], teams: [], apps: [], unexpected: [] }, { users: ["owner"], teams: [], apps: [] }, { users: [], teams: ["owners"], apps: [] }, { users: [], teams: [], apps: ["publisher"] }]) assert.throws(() => assertProviderPreflight({ ...preflight, protection: { ...preflight.protection, required_pull_request_reviews: { ...preflight.protection.required_pull_request_reviews, bypass_pull_request_allowances: bypasses } } }));
  assert.doesNotThrow(() => canonicalPublicAssetUrl("https://github.com/service-lasso/service-lasso-cli/releases/download/x/service-lassoctl-a.tgz", "service-lassoctl-a.tgz", "x"));
  assert.throws(() => canonicalPublicAssetUrl("https://attacker.invalid/a", "a"));
});

test("CLI30 automated policy accepts zero human approvers but preserves bounded wait and PR policy", () => {
  const check = policy => assertProviderPreflight({ immutable: policy.immutable, branch: policy.branch, protection: policy.protection, environment: policy.environment, branchPolicies: policy.policies.branch_policies });
  for (const count of [0, 1, 6]) {
    const policy = providerPolicy();
    policy.protection.required_pull_request_reviews.required_approving_review_count = count;
    assert.doesNotThrow(() => check(policy));
  }
  for (const count of [-1, 7, 0.5, "0", undefined]) {
    const policy = providerPolicy();
    policy.protection.required_pull_request_reviews.required_approving_review_count = count;
    assert.throws(() => check(policy));
  }
  for (const wait of [1, 30]) {
    const policy = providerPolicy();
    policy.environment.protection_rules[0].wait_timer = wait;
    assert.doesNotThrow(() => check(policy));
  }
  for (const wait of [undefined, 0, 31, -1, 1.5, "1"]) {
    const policy = providerPolicy();
    policy.environment.protection_rules[0].wait_timer = wait;
    assert.throws(() => check(policy));
  }
  for (const rules of [[], [{ type: "required_reviewers", reviewers: [] }]]) {
    const policy = providerPolicy();
    policy.environment.protection_rules = rules;
    assert.throws(() => check(policy));
  }
});

test("CLI30 rejects malformed created-private release state before any upload or publish", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    for (const mutate of [
      release => { release.id = "77"; }, release => { release.tag_name = "other"; }, release => { release.target_commitish = "f".repeat(40); },
      release => { release.draft = false; }, release => { release.prerelease = false; }, release => { release.assets = [{ id: 1 }]; }
    ]) {
      const policy = providerPolicy(), events = [];
      const fetchImpl = async (url, init) => {
        const parsed = new URL(url), path = parsed.pathname; events.push({ path, hostname: parsed.hostname, method: init.method });
        if (path.endsWith("/immutable-releases")) return json(policy.immutable);
        if (path.endsWith("/branches/develop")) return json(policy.branch);
        if (path.endsWith("/branches/develop/protection")) return json(policy.protection);
        if (path.endsWith("/environments/development-candidate")) return json(policy.environment);
        if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        if (path.includes("/releases/tags/") || path.includes("/git/ref/tags/")) return json({}, 404);
        if (path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201);
        if (path.endsWith("/git/refs")) return json({}, 201);
        if (path.endsWith("/releases") && init.method === "POST") { const release = emptyPrivateRelease(77, tag); mutate(release); return json(release, 201); }
        throw new Error(`unexpected ${path}`);
      };
      await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }), /exact empty private release/);
      assert.equal(events.filter((event) => event.hostname === "uploads.github.com").length, 0);
      assert.equal(events.filter((event) => event.method === "PATCH").length, 0);
    }
  });
});

test("CLI30 actual publisher verifies private asset IDs before its one publish transition and keeps bearer off public reads", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-publisher-"));
  try {
    const directory = await candidateDirectory(root), verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const events = [], assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    let draft = true;
    const fetchImpl = async (url, init) => {
      const parsed = new URL(url); events.push({ path: parsed.pathname, method: init.method, authorization: init.headers?.authorization });
      const policy = providerPolicy();
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
      if (parsed.pathname.endsWith("/releases") && init.method === "POST") return json(emptyPrivateRelease(77, tag), 201);
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
      ["immutable unauthenticated", "immutable", 401], ["branch forbidden", "branch", 403], ["protection unauthenticated", "protection", 401], ["environment forbidden", "environment", 403], ["policy unauthenticated", "policies", 401],
      ["reviews absent", "reviews", null], ["review policy lacks bypass schema", "reviews", { required_approving_review_count: 0 }], ["bypasses missing", "bypasses", undefined], ["bypasses malformed", "bypasses", {}], ["bypasses unknown", "bypasses", { users: [], teams: [], apps: [], unexpected: [] }], ["bypass user", "bypasses", { users: ["owner"], teams: [], apps: [] }], ["bypass team", "bypasses", { users: [], teams: ["owners"], apps: [] }], ["bypass app", "bypasses", { users: [], teams: [], apps: ["publisher"] }], ["admin disabled", "admin", false], ["checks non-strict", "strict", false], ["checks empty", "contexts", []], ["force allowed", "force", true], ["branch unprotected", "protected", false], ["immutable disabled", "enabled", false], ["environment malformed", "environment", {}], ["wait absent", "wait", undefined], ["wait zero", "wait", 0], ["wait excessive", "wait", 31], ["wait noninteger", "wait", 1.5], ["wait string", "wait", "1"], ["branch policy wrong", "policies", { branch_policies: [{ name: "other" }] }]
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
        if (path.endsWith("/branches/develop/protection")) { if (field === "reviews") policy.protection.required_pull_request_reviews = value; if (field === "bypasses") { if (value === undefined) delete policy.protection.required_pull_request_reviews.bypass_pull_request_allowances; else policy.protection.required_pull_request_reviews.bypass_pull_request_allowances = value; } if (field === "admin") policy.protection.enforce_admins.enabled = value; if (field === "strict") policy.protection.required_status_checks.strict = value; if (field === "contexts") policy.protection.required_status_checks.contexts = value; if (field === "force") policy.protection.allow_force_pushes.enabled = value; return json(policy.protection); }
        if (path.endsWith("/environments/development-candidate")) { if (field === "wait") policy.environment.protection_rules[0].wait_timer = value; return json(policy.environment); }
        if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        throw new Error(`unexpected ${name}`);
      };
      await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }), /provider protection|reviewed|administrator|immutable|protected|environment|bypass/);
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

test("CLI30 collision rejections retain event evidence and never repair provider state", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const validAssets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}`, bytes }));
    const cases = [
      ["tag-only", 404, 200, release => release], ["release-only", 200, 404, release => release], ["incomplete", 200, 200, release => { release.assets = []; }],
      ["mismatched-source", 200, 200, release => { release.target_commitish = "f".repeat(40); }], ["draft", 200, 200, release => { release.draft = true; }],
      ["nonimmutable", 200, 200, release => { release.immutable = false; }], ["lightweight", 200, 200, release => { release.ref = { type: "commit", sha: sourceSha }; }],
      ["bad-recursive-sha", 200, 200, release => { release.ref = { type: "tag", sha: "invalid" }; }], ["invalid-asset-id", 200, 200, release => { release.assets = release.assets.map((asset, index) => ({ ...asset, id: index === 0 ? 0 : asset.id })); }]
    ];
    for (const [name, releaseStatus, refStatus, mutate] of cases) {
      const policy = providerPolicy(), events = [], release = { id: 9, immutable: true, tag_name: tag, draft: false, prerelease: true, target_commitish: sourceSha, assets: validAssets, ref: { type: "tag", sha: "a".repeat(40) } };
      mutate(release);
      const fetchImpl = async (url, init) => {
        const path = new URL(url).pathname; events.push({ path, method: init.method });
        if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        if (path.includes("/releases/tags/")) return json(release, releaseStatus); if (path.includes("/git/ref/tags/")) return json({ object: release.ref }, refStatus);
        if (path.includes("/git/tags/")) return json({ object: { type: "commit", sha: sourceSha } });
        throw new Error(`unexpected ${name} ${path}`);
      };
      await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }), /collision|candidate tag|inventory|identity/);
      assert.equal(events.some((event) => ["POST", "PATCH", "DELETE"].includes(event.method)), false, name);
    }
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
      if (path.includes("/releases/tags/") || path.includes("/git/ref/tags/")) return json({}, 404); if (path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201); if (path.endsWith("/git/refs")) return json({}, 201); if (path.endsWith("/releases") && init.method === "POST") return json(emptyPrivateRelease(77, tag), 201);
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
        if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies); if (path.includes("/releases/tags/") || path.includes("/git/ref/tags/")) return json({}, 404); if (path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201); if (path.endsWith("/git/refs")) return json({}, 201); if (path.endsWith("/releases") && init.method === "POST") return json(emptyPrivateRelease(77, tag), 201); if (parsed.hostname === "uploads.github.com") return json({}, 201); if (path.endsWith("/releases/77") && init.method === "GET") return json({ id: 77, tag_name: tag, draft: true, prerelease: true, target_commitish: sourceSha, assets }); if (path.endsWith("/releases/77") && init.method === "PATCH") { patch += 1; return json({}, 200); } const match = /\/releases\/assets\/(\d+)$/.exec(path); if (match) return responseBytes(assets[Number(match[1]) - 1]?.bytes ?? Buffer.from("")); throw new Error(`unexpected ${path}`); };
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

test("CLI30 rejects invalid and single oversized reader chunks before Buffer copying", async () => {
  await withCandidate(async (_root, directory) => {
    for (const [name, value, read] of [
      ["invalid type", "not-a-byte-chunk", async (chunk) => ({ done: false, value: chunk })],
      ["single oversized", new Uint8Array(1024 * 1024 + 1), async (chunk) => ({ done: false, value: chunk })],
      ["malformed result", new Uint8Array(1), async (chunk) => ({ value: chunk })]
    ]) {
      const policy = providerPolicy(), events = [];
      const fetchImpl = async (url, init) => {
        const path = new URL(url).pathname; events.push(init.method);
        if (path.endsWith("/immutable-releases")) return { status: 200, headers: new Headers(), body: { getReader() { return { read: () => read(value), cancel() {}, releaseLock() {} }; } } };
        if (path.endsWith("/branches/develop")) return json(policy.branch);
        if (path.endsWith("/branches/develop/protection")) return json(policy.protection);
        if (path.endsWith("/environments/development-candidate")) return json(policy.environment);
        if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        throw new Error("unreachable");
      };
      const original = Buffer.from; let copies = 0;
      Buffer.from = (...args) => { if (args[0] === value) copies += 1; return original(...args); };
      try { await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl })); }
      finally { Buffer.from = original; }
      assert.equal(copies, 0, name);
      assert.equal(events.some((method) => method !== "GET"), false, name);
    }
  });
});

test("CLI30 masks provider metadata, upload, private-read and public-read error bodies", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    const sentinel = "secret-sentinel /local/private/path";
    for (const stage of ["metadata", "upload", "private", "public"]) for (const kind of ["fetch", "body"]) {
      const policy = providerPolicy(); let draft = true;
      const fetchImpl = async (url, init) => {
        const parsed = new URL(url), path = parsed.pathname;
        const failAt = (matches) => { if (!matches) return null; if (kind === "fetch") throw new Error(sentinel); return new Response(sentinel, { status: 500, headers: { "content-type": "text/plain" } }); };
        const metadataFailure = failAt(stage === "metadata" && path.endsWith("/immutable-releases")); if (metadataFailure) return metadataFailure;
        if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        if (path.includes("/releases/tags/") || path.includes("/git/ref/tags/")) return json({}, 404); if (path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201); if (path.endsWith("/git/refs")) return json({}, 201); if (path.endsWith("/releases") && init.method === "POST") return json(emptyPrivateRelease(77, tag), 201);
        if (parsed.hostname === "uploads.github.com") { const failure = failAt(stage === "upload"); if (failure) return failure; return json({}, 201); }
        if (path.endsWith("/releases/77") && init.method === "GET") return json({ id: 77, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets });
        if (/\/releases\/assets\/\d+$/.test(path)) { const failure = failAt(stage === "private"); if (failure) return failure; return responseBytes(assets[Number(path.split("/").at(-1)) - 1].bytes); }
        if (path.endsWith("/releases/77") && init.method === "PATCH") { draft = false; return json({ id: 77, immutable: true, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets }); }
        if (assets.some((asset) => asset.browser_download_url === parsed.href)) { const failure = failAt(stage === "public"); if (failure) return failure; return responseBytes(assets.find((asset) => asset.browser_download_url === parsed.href).bytes); }
        throw new Error(`unexpected ${path}`);
      };
      let error; try { await publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }); } catch (caught) { error = caught; }
      assert.ok(error instanceof Error, stage);
      assert.equal(error.message.includes("secret-sentinel"), false, `${stage} ${kind}`); assert.equal(error.message.includes("/local/private/path"), false, `${stage} ${kind}`);
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

test("CLI30 actual private-ID and canonical-public adapters reject every bounded-body defect at the route", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const makeAssets = () => [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    const invalidBody = (read, cancelled) => ({ status: 200, headers: new Headers(), body: { getReader() { return { read, cancel() { cancelled.value = true; }, releaseLock() {} }; } } });
    const defects = [
      ["missing body", () => ({ status: 200, headers: new Headers(), body: null })],
      ["malformed reader result", () => invalidBody(async () => ({ done: "no" }), { value: false })],
      ["wrong status", asset => new Response(asset.bytes, { status: 500 })],
      ["declared mismatch", asset => responseBytes(asset.bytes, { declared: asset.bytes.length + 1 })],
      ["truncated body", asset => responseBytes(asset.bytes.subarray(0, asset.bytes.length - 1), { declared: asset.bytes.length })],
      ["invalid chunk type", () => invalidBody(async () => ({ done: false, value: "not-bytes" }), { value: false })],
      ["undeclared single chunk overflow", asset => { const bad = new Uint8Array(asset.bytes.length + 1); return invalidBody(async () => ({ done: false, value: bad }), { value: false }); }]
    ];
    for (const route of ["private", "public"]) for (const [name, defect] of defects) {
      const assets = makeAssets(), policy = providerPolicy(), events = []; let draft = true, copies = 0, overflow = null;
      const fetchImpl = async (url, init) => {
        const parsed = new URL(url), path = parsed.pathname; events.push({ method: init.method, url: parsed.href });
        if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        if (route === "public" && path.includes("/releases/tags/")) return json({ id: 9, immutable: true, tag_name: tag, draft: false, prerelease: true, target_commitish: sourceSha, assets });
        if (route === "public" && path.includes("/git/ref/tags/")) return json({ object: { type: "tag", sha: "a".repeat(40) } });
        if (route === "public" && path.endsWith(`/git/tags/${"a".repeat(40)}`)) return json({ object: { type: "commit", sha: sourceSha } });
        if (route === "private" && (path.includes("/releases/tags/") || path.includes("/git/ref/tags/"))) return json({}, 404);
        if (route === "private" && path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201); if (route === "private" && path.endsWith("/git/refs")) return json({}, 201); if (route === "private" && path.endsWith("/releases") && init.method === "POST") return json(emptyPrivateRelease(77, tag), 201);
        if (route === "private" && parsed.hostname === "uploads.github.com") return json({}, 201);
        if (route === "private" && path.endsWith("/releases/77") && init.method === "GET") return json({ id: 77, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets });
        if (route === "private" && path.endsWith("/releases/77") && init.method === "PATCH") { draft = false; return json({ id: 77, immutable: true, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets }); }
        const privateAsset = /\/releases\/assets\/(\d+)$/.exec(path); const publicAsset = assets.find((asset) => asset.browser_download_url === parsed.href); const asset = privateAsset ? assets[Number(privateAsset[1]) - 1] : publicAsset;
        if (asset && ((route === "private" && privateAsset) || (route === "public" && publicAsset))) { const response = defect(asset); if (name === "undeclared single chunk overflow") overflow = asset.bytes; return response; }
        throw new Error(`unexpected ${route} ${path}`);
      };
      const original = Buffer.from; Buffer.from = (...args) => { if (name === "undeclared single chunk overflow" && args[0] instanceof Uint8Array && args[0].byteLength > (overflow?.length ?? Number.MAX_SAFE_INTEGER)) copies += 1; return original(...args); };
      try { await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl })); } finally { Buffer.from = original; }
      assert.equal(copies, 0, `${route} ${name}`);
      if (route === "private") assert.equal(events.some((event) => event.method === "PATCH"), false, `${route} ${name}`);
      else assert.equal(events.some((event) => ["POST", "PATCH", "DELETE"].includes(event.method)), false, `${route} ${name}`);
    }
  });
});

test("CLI30 aborts stalled private-ID and canonical-public fetches and bodies, cancelling readers without later writes", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    for (const route of ["private", "public"]) for (const kind of ["fetch", "body"]) {
      const assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` })), policy = providerPolicy(), events = [], timers = []; let cancelled = false, aborted = false;
      const stalled = (init) => kind === "fetch" ? new Promise((_resolve, reject) => init.signal.addEventListener("abort", () => { aborted = true; reject(new Error("abort")); }, { once: true })) : ({ status: 200, headers: new Headers(), body: { getReader() { return { read: () => new Promise(() => {}), cancel() { cancelled = true; }, releaseLock() {} }; } } });
      const fetchImpl = async (url, init) => {
        const parsed = new URL(url), path = parsed.pathname; events.push({ method: init.method, url: parsed.href });
        if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        if (route === "public" && path.includes("/releases/tags/")) return json({ id: 9, immutable: true, tag_name: tag, draft: false, prerelease: true, target_commitish: sourceSha, assets }); if (route === "public" && path.includes("/git/ref/tags/")) return json({ object: { type: "tag", sha: "a".repeat(40) } }); if (route === "public" && path.endsWith(`/git/tags/${"a".repeat(40)}`)) return json({ object: { type: "commit", sha: sourceSha } });
        if (route === "private" && (path.includes("/releases/tags/") || path.includes("/git/ref/tags/"))) return json({}, 404); if (route === "private" && path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201); if (route === "private" && path.endsWith("/git/refs")) return json({}, 201); if (route === "private" && path.endsWith("/releases") && init.method === "POST") return json(emptyPrivateRelease(77, tag), 201); if (route === "private" && parsed.hostname === "uploads.github.com") return json({}, 201); if (route === "private" && path.endsWith("/releases/77") && init.method === "GET") return json({ id: 77, tag_name: tag, draft: true, prerelease: true, target_commitish: sourceSha, assets });
        if ((route === "private" && /\/releases\/assets\/\d+$/.test(path)) || (route === "public" && assets.some((asset) => asset.browser_download_url === parsed.href))) return stalled(init);
        throw new Error(`unexpected ${route} ${path}`);
      };
      const requestOptions = { deadlineMs: 1, setTimer(callback) { timers.push(callback); queueMicrotask(callback); return callback; }, clearTimer() {} };
      await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl, requestOptions }), /request failed/);
      await new Promise((resolve) => setImmediate(resolve));
      assert.equal(kind === "fetch" ? aborted : cancelled, true, `${route} ${kind}`);
      if (route === "private") assert.equal(events.some((event) => event.method === "PATCH"), false, `${route} ${kind}`); else assert.equal(events.some((event) => ["POST", "PATCH", "DELETE"].includes(event.method)), false, `${route} ${kind}`);
    }
  });
});

test("CLI30 treats nonempty private and public 3xx bodies as disposable redirect metadata", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const makeAssets = () => [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    for (const route of ["private", "public"]) for (const [name, location, malformed] of [["allowed", "https://objects.githubusercontent.com/candidate/1", false], ["missing location", null, false], ["rejected host", "https://attacker.invalid/candidate/1", false], ["malformed redirect", "https://objects.githubusercontent.com/candidate/1", true]]) for (const cancelMode of name === "allowed" ? ["settled", "stall", "throws"] : ["settled"]) {
      const assets = makeAssets(), policy = providerPolicy(), events = [], redirected = { cancelled: false }; let draft = true;
      const redirect = () => malformed ? { status: "302", headers: new Headers({ location }), body: null } : { status: 302, headers: new Headers(location ? { location } : {}), body: { getReader() { return { async read() { return { done: false, value: Buffer.from("untrusted redirect body") }; }, cancel() { redirected.cancelled = true; if (cancelMode === "stall") return new Promise(() => {}); if (cancelMode === "throws") throw new Error("dispose failure"); }, releaseLock() {} }; } } };
      const fetchImpl = async (url, init) => {
        const parsed = new URL(url), path = parsed.pathname; events.push({ url: parsed.href, method: init.method, authorization: init.headers?.authorization });
        if (path.endsWith("/immutable-releases")) return json(policy.immutable); if (path.endsWith("/branches/develop")) return json(policy.branch); if (path.endsWith("/branches/develop/protection")) return json(policy.protection); if (path.endsWith("/environments/development-candidate")) return json(policy.environment); if (path.endsWith("/deployment-branch-policies")) return json(policy.policies);
        if (route === "public" && path.includes("/releases/tags/")) return json({ id: 9, immutable: true, tag_name: tag, draft: false, prerelease: true, target_commitish: sourceSha, assets });
        if (route === "public" && path.includes("/git/ref/tags/")) return json({ object: { type: "tag", sha: "a".repeat(40) } });
        if (route === "public" && path.endsWith(`/git/tags/${"a".repeat(40)}`)) return json({ object: { type: "commit", sha: sourceSha } });
        if (route === "private" && (path.includes("/releases/tags/") || path.includes("/git/ref/tags/"))) return json({}, 404);
        if (route === "private" && path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201); if (route === "private" && path.endsWith("/git/refs")) return json({}, 201); if (route === "private" && path.endsWith("/releases") && init.method === "POST") return json(emptyPrivateRelease(77, tag), 201);
        if (route === "private" && parsed.hostname === "uploads.github.com") return json({}, 201);
        if (route === "private" && path.endsWith("/releases/77") && init.method === "GET") return json({ id: 77, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets });
        if (route === "private" && path.endsWith("/releases/77") && init.method === "PATCH") { draft = false; return json({ id: 77, immutable: true, tag_name: tag, draft, prerelease: true, target_commitish: sourceSha, assets }); }
        const privateAsset = /\/releases\/assets\/(\d+)$/.exec(path), publicAsset = assets.find((asset) => asset.browser_download_url === parsed.href), asset = privateAsset ? assets[Number(privateAsset[1]) - 1] : publicAsset;
        if (asset && (privateAsset || publicAsset)) return asset.id === 1 ? redirect() : responseBytes(asset.bytes);
        if (parsed.hostname === "objects.githubusercontent.com") return responseBytes(assets[0].bytes);
        throw new Error(`unexpected ${route} ${path}`);
      };
      const outcome = publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl });
      if (name === "allowed") { const result = await Promise.race([outcome, new Promise((_, reject) => setTimeout(() => reject(new Error("redirect disposal exceeded its test bound")), 1000))]); assert.equal(result.result, route === "private" ? "published" : "readback-only"); assert.equal(events.find((event) => event.url.includes("objects.githubusercontent.com")).authorization, undefined); }
      else await assert.rejects(() => outcome, /redirect|response|request failed/);
      if (!malformed) assert.equal(redirected.cancelled, true, `${route} ${name} ${cancelMode}`);
      if (route === "private") assert.equal(events.some((event) => event.method === "PATCH"), name === "allowed", `${route} ${name} ${cancelMode}`);
      else assert.equal(events.some((event) => ["POST", "PATCH", "DELETE"].includes(event.method)), false, `${route} ${name} ${cancelMode}`);
    }
  });
});

test("CLI30 rereads all five policies immediately before every actual mutation and stops midphase", async () => {
  await withCandidate(async (_root, directory) => {
    const verified = await verifyCandidateDirectory(directory, version, sourceSha), tag = verified.manifest.candidateTag;
    const assets = [...verified.held.entries()].map(([name, bytes], index) => ({ id: index + 1, name, bytes, browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${encodeURIComponent(name)}` }));
    const policySuffixes = ["/immutable-releases", "/branches/develop", "/branches/develop/protection", "/environments/development-candidate", "/deployment-branch-policies"];
    // Three initial writes, all ten held public assets, then the single publish.
    const totalWrites = 3 + assets.length + 1;
    for (const [failBefore, deniedEndpoint] of [[null, null], ...Array.from({ length: totalWrites }, (_, index) => [[index, null], ...policySuffixes.map((_, endpoint) => [index, endpoint])]).flat()]) {
      const events = [], policy = providerPolicy(); let writes = 0;
      const fetchImpl = async (url, init) => {
        const parsed = new URL(url), path = parsed.pathname; events.push({ path, method: init.method });
        const policyIndex = policySuffixes.findIndex(suffix => path.endsWith(suffix));
        if (policyIndex >= 0) {
          if (writes === failBefore && deniedEndpoint === policyIndex) return json({}, 403);
          if (writes === failBefore && deniedEndpoint === null && policyIndex === 0) return json({ enabled: false });
          return json([policy.immutable, policy.branch, policy.protection, policy.environment, policy.policies][policyIndex]);
        }
        if (init.method !== "GET") {
          const preceding = events.slice(-6, -1);
          assert.equal(preceding.length, 5);
          assert.deepEqual(preceding.map(event => policySuffixes.findIndex(suffix => event.path.endsWith(suffix))).sort(), [0, 1, 2, 3, 4]);
          assert.ok(preceding.every(event => event.method === "GET")); writes += 1;
        }
        if (path.includes("/releases/tags/") || path.includes("/git/ref/tags/")) return json({}, 404);
        if (path.endsWith("/git/tags")) return json({ sha: "a".repeat(40) }, 201);
        if (path.endsWith("/git/refs")) return json({}, 201);
        if (path.endsWith("/releases") && init.method === "POST") return json(emptyPrivateRelease(77, tag), 201);
        if (parsed.hostname === "uploads.github.com") return json({}, 201);
        if (path.endsWith("/releases/77")) return json({ id: 77, tag_name: tag, target_commitish: sourceSha, prerelease: true, draft: init.method !== "PATCH", immutable: init.method === "PATCH", assets });
        const privateId = /\/releases\/assets\/(\d+)$/.exec(path);
        if (privateId) return responseBytes(assets[Number(privateId[1]) - 1].bytes);
        const asset = assets.find(value => value.browser_download_url === parsed.href);
        if (asset) { assert.equal(init.headers.authorization, undefined); return responseBytes(asset.bytes); }
        throw new Error("Unexpected endpoint");
      };
      if (failBefore === null) { assert.equal((await publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl })).result, "published"); assert.equal(writes, totalWrites); }
      else { await assert.rejects(() => publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl }), /immutable releases|provider protection/); assert.equal(writes, failBefore); assert.ok(events.slice(events.findLastIndex(event => event.method !== "GET") + 1).every(event => event.method === "GET")); }
    }
  });
});

test("CLI30 actual Darwin producer ships helper bytes and verifier rejects omitted or replaced helper despite outer checksum rebinding", async () => {
  await withCandidate(async (root, directory) => {
    const archiveName = `service-lassoctl-${version}-darwin-arm64.tar.gz`, archive = join(directory, archiveName);
    const unpacked = join(root, "darwin-unpacked"); await mkdir(unpacked);
    execFileSync("tar", ["-xzf", archive, "-C", unpacked]);
    const helper = "service-lasso-darwin-immutable-helper";
    const provenance = JSON.parse(await readFile(join(unpacked, "provenance.json"), "utf8"));
    assert.equal(sha256(await readFile(join(unpacked, helper))), provenance.darwinImmutableHelper.sha256);
    const members = ["service-lassoctl", "service-lasso-confined-scaffold", helper, "provenance.json", "ci-context.json", "host-acceptance.json"];
    assert.deepEqual(execFileSync("tar", ["-tzf", archive], { encoding: "utf8" }).trim().split(/\r?\n/).sort(), [...members].sort());
    for (const defect of ["omit", "replace"]) {
      if (defect === "replace") await writeFile(join(unpacked, helper), "hostile replacement");
      execFileSync("tar", ["-czf", archive, "-C", unpacked, "--", ...members.filter(name => defect !== "omit" || name !== helper)]);
      await rebindCandidate(directory);
      await assert.rejects(() => verifyCandidateDirectory(directory, version, sourceSha), /closed inventory|forged identity/);
    }
    assert.throws(() => execFileSync(node, ["scripts/archive-native-candidate.mjs", "--expected-event", "workflow_dispatch", "--expected-base-sha", "", "--expected-merge-context-sha", sourceSha, "--directory", unpacked, "--output", unpacked, "--version", version, "--source-sha", sourceSha], { stdio: "ignore" }));
  });
});
test("CLI30 held local reads reject growth and replacement inside the actual read boundary for assets, manifest and sums", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-held-boundary-"));
  try {
    for (const name of ["asset.tar.gz", "development-candidate.json", "SHA256SUMS.txt"]) {
      const original = Buffer.alloc(128 * 1024, 65), path = join(root, name);
      await writeFile(path, original);
      let calls = 0;
      const allocations = [], allocate = Buffer.alloc;
      Buffer.alloc = (size, ...args) => { allocations.push(size); return allocate(size, ...args); };
      try {
        await assert.rejects(readHeldFile(root, name, name === "asset.tar.gz" ? original.length : undefined, { afterRead: async () => { if (calls++ === 0) await truncate(path, 256 * 1024 * 1024 + 1); } }), /changed during its held read/);
      } finally { Buffer.alloc = allocate; }
      assert.ok(calls > 0, "growth occurred while the held reader was active");
      assert.ok(allocations.length > 0 && allocations.every(size => size <= original.length), "growth never controls a payload allocation");
      await writeFile(path, original);
      const replacement = join(root, `${name}.replacement`);
      await writeFile(replacement, Buffer.alloc(original.length, 66));
      let replacementBlocked = false;
      try {
        const result = await readHeldFile(root, name, undefined, { afterRead: async ({ offset }) => {
          if (offset !== 64 * 1024) return;
          try { await rename(replacement, path); } catch (error) { if (process.platform !== "win32" || !["EPERM", "EACCES", "EBUSY"].includes(error.code)) throw error; replacementBlocked = true; }
        } });
        assert.equal(replacementBlocked, true, "a successful pathname replacement must fail the held read");
        assert.deepEqual(result, original);
      } catch (error) { assert.match(error.message, /changed during its held read/); }
      await writeFile(path, original);
      await assert.rejects(readHeldFile(root, name, undefined, { afterOpen: () => truncate(path, 0) }), /truncated during its held read/);
      await writeFile(path, original);
      await assert.rejects(readHeldFile(root, name, undefined, { beforeOpen: async () => { await rename(path, `${path}.saved`); await mkdir(path); } }), /changed before its held read|EISDIR|EPERM|EACCES/);
      await rm(path, { recursive: true });
      await rename(`${path}.saved`, path);
    }
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("CLI30 actual producer uses local archive names under drive and spaced output with Windows GNU tar", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lasso archive spaces "));
  try {
    const environment = { ...process.env };
    if (process.platform === "win32") {
      assert.match(root, /^[A-Za-z]:\\/);
      // Match the dedicated Git Bash job's GNU tar implementation explicitly,
      // so a different ambient BSD tar cannot mask drive-colon regression.
      const gitExec = execFileSync("git", ["--exec-path"], { encoding: "utf8" }).trim();
      const gnuDirectory = resolve(gitExec, "..", "..", "..", "usr", "bin");
      delete environment.Path;
      environment.PATH = `${gnuDirectory};${process.env.PATH ?? process.env.Path ?? ""}`;
      assert.match(execFileSync(join(gnuDirectory, "tar.exe"), ["--version"], { encoding: "utf8" }), /GNU tar/);
    }
    const directory = await nativeDirectory(root, { id: "win32-x64", platform: "win32", architecture: "x64" }, environment);
    const name = `service-lassoctl-${version}-win32-x64.tar.gz`;
    const bytes = await readFile(join(directory, name));
    assert.ok(bytes.length > 0);
    const members = execFileSync("tar", ["-tzf", `./${name}`], { cwd: directory, env: environment, encoding: "utf8", shell: false }).trim().split(/\r?\n/).sort();
    assert.deepEqual(members, ["service-lassoctl.exe", "service-lasso-confined-scaffold.exe", "provenance.json", "ci-context.json", "host-acceptance.json"].sort());
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("CLI30 actual archive producer binds push, PR and dispatch caller contexts and rejects contradictions", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-archive-events-"));
  const target = { id: "linux-x64", platform: "linux", architecture: "x64" };
  try {
    // Restore payload members removed by the fixture only after the actual
    // dispatch archive was produced; every event below invokes that producer.
    const directory = await nativeDirectory(root, target);
    await writeFile(join(directory, "service-lassoctl"), "native-linux-x64");
    await writeFile(join(directory, "service-lasso-confined-scaffold"), "confined-writer-linux-x64");
    const base = "b".repeat(40), merge = "c".repeat(40);
    for (const event of ["push", "pull_request", "workflow_dispatch"]) {
      const context = { schemaVersion: 1, eventName: event, sourceSha, testedBaseSha: event === "pull_request" ? base : null, mergeContextSha: event === "pull_request" ? merge : sourceSha };
      const args = ["scripts/archive-native-candidate.mjs", "--directory", directory, "--output", directory, "--version", version, "--source-sha", sourceSha, "--expected-event", event, "--expected-base-sha", event === "pull_request" ? base : "", "--expected-merge-context-sha", context.mergeContextSha];
      await writeFile(join(directory, "ci-context.json"), JSON.stringify(context));
      execFileSync(node, args, { encoding: "utf8" });
      for (const mutation of [{ eventName: "unknown" }, { sourceSha: "d".repeat(40) }, { mergeContextSha: "d".repeat(40) }, { testedBaseSha: event === "pull_request" ? sourceSha : base }]) {
        await writeFile(join(directory, "ci-context.json"), JSON.stringify({ ...context, ...mutation }));
        assert.throws(() => execFileSync(node, args, { stdio: "ignore" }));
      }
      await writeFile(join(directory, "ci-context.json"), JSON.stringify(context));
      const wrongEventArgs = [...args]; wrongEventArgs[wrongEventArgs.indexOf("--expected-event") + 1] = event === "push" ? "workflow_dispatch" : "push";
      assert.throws(() => execFileSync(node, wrongEventArgs, { stdio: "ignore" }));
    }
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("CLI30 publication rejects actual push and PR archives even after closed outer checksums are rebound", async () => {
  await withCandidate(async (root, directory) => {
    const archiveName = `service-lassoctl-${version}-linux-x64.tar.gz`, unpacked = join(root, "ci-native");
    await mkdir(unpacked);
    execFileSync("tar", ["-xzf", join(directory, archiveName), "-C", unpacked]);
    for (const eventName of ["push", "pull_request"]) {
      const testedBaseSha = eventName === "push" ? null : "b".repeat(40), mergeContextSha = eventName === "push" ? sourceSha : "c".repeat(40);
      await writeFile(join(unpacked, "ci-context.json"), JSON.stringify({ schemaVersion: 1, eventName, sourceSha, testedBaseSha, mergeContextSha }));
      execFileSync(node, ["scripts/archive-native-candidate.mjs", "--directory", unpacked, "--output", directory, "--version", version, "--source-sha", sourceSha, "--expected-event", eventName, "--expected-base-sha", testedBaseSha ?? "", "--expected-merge-context-sha", mergeContextSha]);
      await rebindCandidate(directory);
      await assert.rejects(verifyCandidateDirectory(directory, version, sourceSha), /dispatch publication identity/);
      let requests = 0;
      await assert.rejects(publishProtectedCandidate({ directory, version, sourceSha, token: "secret-sentinel", fetchImpl: async () => { requests += 1; throw new Error("unexpected request"); } }), /dispatch publication identity/);
      assert.equal(requests, 0);
    }
  });
});
