import assert from 'node:assert/strict';
import test from 'node:test';
import { gzipSync } from 'node:zlib';
import { mkdir, mkdtemp, readFile, writeFile, readdir, rm, lstat } from 'node:fs/promises';
import { join, resolve } from 'node:path';
import { NATIVE_TARGETS, expectedAssets, identity, checksumBytes, validateManifest, verifyCandidateDirectory,
  verifyScopedNative, assertScopedRunEvidence, SCOPED_REQUIRED_JOBS, assertScopedCiEvidence, SCOPED_CI_REQUIRED_JOBS, sha256 } from '../scripts/protected-scoped-candidate-lib.mjs';
import { validateManifest as legacyManifest, readTarArchive } from '../scripts/protected-candidate-lib.mjs';
import { scopeIdentity, assertScope, readSourceScope, POLICY_SHA256 } from '../scripts/cli-platform-scope.mjs';
import { assembleScopedCandidate } from '../scripts/assemble-protected-candidate-scoped.mjs';
import { publishProtectedScopedCandidate } from '../scripts/publish-protected-candidate-scoped.mjs';
import { assertScopedWorkflowSource } from '../scripts/bind-scoped-release-source.mjs';
import { producePositiveNativeQualification } from '../scripts/produce-native-positive-qualification-scoped.mjs';
import { verifyPositiveNativeReceipt } from '../scripts/native-positive-scoped-lib.mjs';

// All archives/receipts/provider responses below are labelled structural fixtures.
// They execute no native binary and establish no genuine ROOT or qualification.
const sourceSha = '0123456789abcdef0123456789abcdef01234567', version = '0.1.0-dev.0123456';
const tag = `cli-v${version}-candidate-0123456`, workflowRun = { id: 47, attempt: 1, workflowSha: sourceSha };
const jsonBytes = value => Buffer.from(`${JSON.stringify(value, null, 2)}\n`);
function archive(members) {
  const parts = [];
  for (const [name, value] of members) {
    const body = Buffer.isBuffer(value) ? value : Buffer.from(value), header = Buffer.alloc(512);
    header.write(name, 0, 100, 'ascii');
    for (const [offset, width, number] of [[100, 8, 0o755], [108, 8, 0], [116, 8, 0], [124, 12, body.length], [136, 12, 0]])
      header.write(`${number.toString(8).padStart(width - 1, '0')}\0`, offset, width, 'ascii');
    header.fill(32, 148, 156); header[156] = 48; header.write('ustar\0', 257, 6, 'ascii'); header.write('00', 263, 2, 'ascii');
    const sum = header.reduce((total, byte) => total + byte, 0);
    header.write(`${sum.toString(8).padStart(6, '0')}\0 `, 148, 8, 'ascii');
    parts.push(header, body, Buffer.alloc((512 - body.length % 512) % 512));
  }
  parts.push(Buffer.alloc(1024)); return gzipSync(Buffer.concat(parts));
}
function nativeFixture(target) {
  const executable = Buffer.from(`FIXTURE_ONLY_NOT_NATIVE_QUALIFICATION_${target.id}`), helper = Buffer.from(`FIXTURE_HELPER_${target.id}`);
  const provenance = { schemaVersion: 1, command: 'service-lassoctl', candidate: { version, tag }, source: { commit: sourceSha },
    executable: { architecture: target.architecture, name: target.executable, platform: target.platform, sha256: sha256(executable), version },
    confinedWriter: { architecture: target.architecture, name: target.confinedWriter, platform: target.platform, sha256: sha256(helper), sourceSha256: 'b'.repeat(64) },
    tools: { node: '22.23.2', esbuild: '0.28.2', postject: '1.0.0-alpha.6' },
    sea: { mainFormat: 'commonjs', useCodeCache: false, execArgvExtension: 'none' } };
  const raw = jsonBytes(provenance);
  const context = jsonBytes({ schemaVersion: 1, eventName: 'workflow_dispatch', sourceSha, testedBaseSha: null, mergeContextSha: sourceSha });
  const acceptance = jsonBytes({ schemaVersion: 1, sourceSha, version, platform: target.platform, architecture: target.architecture,
    executableSha256: sha256(executable), nodeAbsentFromPath: true, status: 'passed',
    evidenceDigest: sha256(Buffer.from(`service-lasso-native-acceptance-v1\n${sourceSha}\n${version}\n${target.platform}\n${target.architecture}\n${sha256(executable)}\nnode-absent\npassed\n`)) });
  const members = new Map([[target.executable, executable], [target.confinedWriter, helper], ['provenance.json', raw],
    ['ci-context.json', context], ['host-acceptance.json', acceptance]]);
  return { executable, provenance, members, raw, archive: archive(members) };
}
async function fixture(t) {
  const container = resolve('.tmp'); await mkdir(container, { recursive: true });
  const info = await lstat(container); assert.equal(info.isDirectory() && !info.isSymbolicLink(), true);
  const root = await mkdtemp(join(container, 'cli39-scoped-'));
  t.after(() => rm(root, { recursive: true, force: true }));
  const portable = join(root, 'portable'); await mkdir(portable);
  const packageBytes = archive(new Map([
    ['package/README.md', 'STRUCTURAL FIXTURE ONLY'],
    ['package/package.json', jsonBytes({ name: '@service-lasso/cli', version, engines: { node: '>=22.12.0' }, bin: { 'service-lassoctl': './dist/index.js' } })],
    ['package/dist/index.js', '/* FIXTURE ONLY: no runtime qualification */'],
  ]));
  const record = { schemaVersion: 2, assets: [{ name: `service-lassoctl-${version}.tgz`, sha256: sha256(packageBytes), size: packageBytes.length }],
    candidateTag: tag, package: { command: 'service-lassoctl', entrypoint: 'dist/index.js', name: '@service-lasso/cli', node: '>=22.12.0' },
    platforms: ['win32', 'linux', 'darwin'], source: { commit: sourceSha, repository: 'service-lasso/service-lasso-cli' }, version, scope: scopeIdentity() };
  await writeFile(join(portable, 'candidate.json'), jsonBytes(record));
  await writeFile(join(portable, `service-lassoctl-${version}.tgz`), packageBytes);
  const native = new Map(), originals = new Map();
  for (const target of NATIVE_TARGETS) {
    const directory = join(root, target.id), value = nativeFixture(target); await mkdir(directory);
    await writeFile(join(directory, `service-lassoctl-${version}-${target.id}.tar.gz`), value.archive);
    await writeFile(join(directory, 'provenance.json'), value.raw); await writeFile(join(directory, target.executable), value.executable);
    native.set(target.id, directory); originals.set(target.id, value);
  }
  const output = join(root, 'candidate');
  await assembleScopedCandidate({ output, portable, native, version, sourceSha });
  return { root, output, portable, native, originals, record };
}
async function rebind(directory) {
  const manifest = JSON.parse(await readFile(join(directory, 'development-candidate.json'), 'utf8'));
  for (const asset of manifest.assets) { const bytes = await readFile(join(directory, asset.name)); asset.size = bytes.length; asset.sha256 = sha256(bytes); }
  const raw = jsonBytes(manifest); await writeFile(join(directory, 'development-candidate.json'), raw);
  await writeFile(join(directory, 'SHA256SUMS.txt'), checksumBytes(raw, manifest.assets)); return manifest;
}
function runEvidence() {
  const run = { id: workflowRun.id, run_attempt: workflowRun.attempt, head_sha: sourceSha, head_branch: 'develop', event: 'workflow_dispatch',
    path: '.github/workflows/release-scoped.yml', repository: { full_name: 'service-lasso/service-lasso-cli' }, workflow_id: 83 };
  const jobs = SCOPED_REQUIRED_JOBS.map((name, index) => ({ id: index + 1, name, run_id: workflowRun.id, run_attempt: 1, head_sha: sourceSha,
    status: 'completed', conclusion: 'success', steps: [{ name: 'Require genuine separate positive native route originals', status: 'completed', conclusion: 'success' }] }));
  jobs.push({ id: 6, name: 'Preflight and publish exact immutable candidate bytes', run_id: workflowRun.id, run_attempt: 1,
    head_sha: sourceSha, status: 'in_progress', conclusion: null });
  return { latest: structuredClone(run), attempt: structuredClone(run), jobs: { total_count: jobs.length, jobs } };
}
function ciEvidence() {
  const listed = { id: 53, run_attempt: 2, head_sha: sourceSha, head_branch: 'develop', event: 'push',
    path: '.github/workflows/ci.yml', repository: { full_name: 'service-lasso/service-lasso-cli' }, workflow_id: 89,
    status: 'completed', conclusion: 'failure' }; // Mac-only aggregate failure is deliberately N/A.
  const names = [...SCOPED_CI_REQUIRED_JOBS, 'Node 22 CLI checks (macos-latest)', 'Native SEA direct execution (macOS arm64)'];
  const jobs = names.map((name, index) => ({ id: index + 101, name, run_id: 53, run_attempt: 2, head_sha: sourceSha,
    status: 'completed', conclusion: index < 6 ? 'success' : 'failure' }));
  return { listed, latest: structuredClone(listed), attempt: structuredClone(listed), jobs: { total_count: 8, jobs } };
}
function policies() {
  return { immutable: { enabled: true }, branch: { name: 'develop', protected: true, commit: { sha: sourceSha } },
    protection: { required_status_checks: { strict: true, contexts: ['selected-ci'] }, required_pull_request_reviews: { required_approving_review_count: 0,
      bypass_pull_request_allowances: { users: [], teams: [], apps: [] } }, enforce_admins: { enabled: true }, allow_force_pushes: { enabled: false } },
    environment: { name: 'development-candidate', protection_rules: [{ type: 'wait_timer', wait_timer: 1 }],
      deployment_branch_policy: { custom_branch_policies: true, protected_branches: false } }, branchPolicies: { branch_policies: [{ name: 'develop' }] } };
}
function provider(held, { changePolicy, changeTag, changeRules, run = runEvidence(), ci = ciEvidence(), collision = false, wrongBytes = false, assetSizeDelta = 0, afterUpload } = {}) {
  let rounds = 0, draft = true, refBorn = collision, refReads = 0; const calls = [], uploads = new Map();
  const assets = () => [...held].map(([name, bytes], index) => ({ id: index + 1, name, size: bytes.length + assetSizeDelta,
    browser_download_url: `https://github.com/service-lasso/service-lasso-cli/releases/download/${tag}/${name}` }));
  const release = () => ({ id: 23, tag_name: tag, target_commitish: sourceSha, draft, prerelease: true, immutable: !draft, assets: assets() });
  const json = (value, status = 200) => new Response(JSON.stringify(value), { status });
  const fetchImpl = async (url, options) => {
    calls.push({ url, options }); const path = new URL(url).pathname, method = options.method;
    const policy = policies(); changePolicy?.(policy, rounds);
    if (path.endsWith('/immutable-releases')) { rounds++; return json(policy.immutable); }
    if (path.endsWith('/branches/develop')) return json(policy.branch);
    if (path.endsWith('/branches/develop/protection')) return json(policy.protection);
    if (path.endsWith('/environments/development-candidate')) return json(policy.environment);
    if (path.endsWith('/deployment-branch-policies')) return json(policy.branchPolicies);
    if (path.endsWith('/rulesets')) return json([{ id: 71 }]);
    if (path.endsWith('/rulesets/71')) {
      const rule = { id: 71, target: 'tag', enforcement: 'active', bypass_actors: [],
        conditions: { ref_name: { include: ['refs/tags/cli-v*-candidate-*'], exclude: [] } }, rules: [{ type: 'update' }, { type: 'deletion' }] };
      changeRules?.(rule, rounds); return json(rule);
    }
    if (path.endsWith('/actions/workflows/ci.yml/runs')) return json({ total_count: 1, workflow_runs: [ci.listed] });
    if (path.endsWith('/actions/runs/53')) return json(ci.latest);
    if (path.endsWith('/actions/runs/53/attempts/2')) return json(ci.attempt);
    if (path.endsWith('/actions/runs/53/attempts/2/jobs')) return json(ci.jobs);
    if (path.endsWith(`/actions/runs/${workflowRun.id}`)) return json(run.latest);
    if (path.endsWith(`/actions/runs/${workflowRun.id}/attempts/1`)) return json(run.attempt);
    if (path.endsWith('/attempts/1/jobs')) return json(run.jobs);
    if (method === 'GET' && path.includes('/releases/tags/')) { if (collision) { draft = false; return json(release()); } return json({}, 404); }
    if (method === 'GET' && path.includes('/git/ref/tags/')) {
      if (!refBorn) return json({}, 404);
      const value = { ref: `refs/tags/${tag}`, object: { type: 'tag', sha: 'a'.repeat(40) } };
      changeTag?.(value, { reads: ++refReads, uploads: uploads.size, published: !draft }); return json(value);
    }
    if (method === 'GET' && path.endsWith(`/git/tags/${'a'.repeat(40)}`)) return json({ sha: 'a'.repeat(40), tag, object: { type: 'commit', sha: sourceSha } });
    if (method === 'POST' && path.endsWith('/git/tags')) return json({ sha: 'a'.repeat(40) }, 201);
    if (method === 'POST' && path.endsWith('/git/refs')) { refBorn = true; return json({}, 201); }
    if (method === 'POST' && path.endsWith('/releases')) return json({ id: 23, tag_name: tag, target_commitish: sourceSha, draft: true, prerelease: true, assets: [] }, 201);
    if (method === 'POST' && path.endsWith('/assets')) { uploads.set(new URL(url).searchParams.get('name'), options.body); await afterUpload?.(uploads.size); return json({}, 201); }
    if (method === 'GET' && path.endsWith('/releases/23')) return json(release());
    if (method === 'PATCH' && path.endsWith('/releases/23')) { draft = false; return json(release()); }
    if (method === 'GET' && path.includes('/releases/assets/')) {
      const item = assets().find(asset => String(asset.id) === path.split('/').at(-1));
      assert.equal(options.headers.authorization, 'Bearer FIXTURE_ONLY'); return new Response(held.get(item.name));
    }
    if (url.startsWith('https://github.com/')) {
      assert.equal(options.headers.authorization, undefined); assert.equal(options.headers.cookie, undefined);
      const name = decodeURIComponent(path.split('/').at(-1)); return new Response(wrongBytes ? Buffer.from('wrong') : held.get(name));
    }
    throw new Error('unexpected fixture endpoint');
  };
  return { fetchImpl, calls, uploads, rounds: () => rounds };
}
const publish = (directory, adapter) => publishProtectedScopedCandidate({ directory, version, sourceSha, workflowRun,
  token: 'FIXTURE_ONLY', fetchImpl: adapter.fetchImpl });

test('scoped policy is exact source Git bytes; reordered/mixed/caller scope never selects a release', async () => {
  assert.deepEqual(await readSourceScope(), scopeIdentity());
  for (const change of [value => value.requiredPlatforms.reverse(), value => value.deferredPlatforms.push('linux'),
    value => value.policySource.commit = 'f'.repeat(40), value => value.policySha256 = '0'.repeat(64), value => value.extra = true]) {
    const value = scopeIdentity(); change(value); assert.throws(() => assertScope(value));
  }
  assert.throws(() => identity(version, 'f'.repeat(40)));
});

test('scoped assembler consumes complete fixture originals into exactly six/seven/eight ASCII inventories', async t => {
  const f = await fixture(t), verified = await verifyCandidateDirectory(f.output, version, sourceSha);
  assert.equal(verified.manifest.schemaVersion, 2); assert.equal(verified.manifest.assets.length, 6); assert.equal(verified.held.size, 8);
  assert.deepEqual(verified.manifest.assets.map(value => value.name), expectedAssets(version).map(value => value.name));
  assert.equal(verified.manifest.checksums.entries.length, 7);
  assert.equal(verified.held.get('SHA256SUMS.txt').toString().trimEnd().split('\n').length, 7);
  assert.throws(() => legacyManifest(verified.manifest, version, sourceSha));
  const legacy = structuredClone(verified.manifest); legacy.schemaVersion = 1; delete legacy.scope;
  assert.throws(() => validateManifest(legacy, version, sourceSha));
  const unordered = structuredClone(verified.manifest); unordered.assets.reverse(); assert.throws(() => validateManifest(unordered, version, sourceSha));
  unordered.assets.reverse(); unordered.checksums.entries.reverse(); assert.throws(() => validateManifest(unordered, version, sourceSha));
  const extraNative = new Map(f.native); extraNative.set('darwin-arm64', f.portable);
  const refused = join(f.root, 'refused');
  await assert.rejects(assembleScopedCandidate({ output: refused, portable: f.portable, native: extraNative, version, sourceSha }));
  await assert.rejects(lstat(refused), { code: 'ENOENT' });
  await assert.rejects(assembleScopedCandidate({ output: f.output, portable: f.portable, native: f.native, version, sourceSha }), { code: 'EEXIST' });
  assert.deepEqual((await verifyCandidateDirectory(f.output, version, sourceSha)).manifest, verified.manifest);
});

test('mixed portable1, duplicate raw keys, extra files and non-ASCII checksum order deny', async t => {
  const f = await fixture(t), record = structuredClone(f.record); record.schemaVersion = 1; delete record.scope;
  await writeFile(join(f.output, 'candidate.json'), jsonBytes(record)); await rebind(f.output);
  await assert.rejects(verifyCandidateDirectory(f.output, version, sourceSha));
  await writeFile(join(f.output, 'candidate.json'), jsonBytes(f.record)); await rebind(f.output);
  const raw = await readFile(join(f.output, 'development-candidate.json'));
  await writeFile(join(f.output, 'development-candidate.json'), raw.toString().replace('"schemaVersion": 2', '"schemaVersion":2,"schemaVersion":2'));
  await assert.rejects(verifyCandidateDirectory(f.output, version, sourceSha), /duplicate/u);
  await writeFile(join(f.output, 'development-candidate.json'), raw);
  const sums = await readFile(join(f.output, 'SHA256SUMS.txt'));
  await writeFile(join(f.output, 'SHA256SUMS.txt'), `${sums.toString().trimEnd().split('\n').reverse().join('\n')}\n`);
  await assert.rejects(verifyCandidateDirectory(f.output, version, sourceSha));
  await writeFile(join(f.output, 'SHA256SUMS.txt'), sums); await writeFile(join(f.output, 'extra'), 'fixture');
  await assert.rejects(verifyCandidateDirectory(f.output, version, sourceSha));
});

test('native tool pin/helper replacement stays rejected after rebinding outer hashes', async t => {
  const f = await fixture(t), target = NATIVE_TARGETS[0], original = f.originals.get(target.id);
  const provenance = structuredClone(original.provenance); provenance.tools.node = '22.0.0';
  const members = new Map(original.members); members.set('provenance.json', jsonBytes(provenance));
  await writeFile(join(f.output, `provenance-${target.id}.json`), jsonBytes(provenance));
  await writeFile(join(f.output, `service-lassoctl-${version}-${target.id}.tar.gz`), archive(members)); await rebind(f.output);
  await assert.rejects(verifyCandidateDirectory(f.output, version, sourceSha), /pins/u);
  members.set('provenance.json', original.raw); members.set(target.confinedWriter, Buffer.from('replaced fixture helper'));
  await writeFile(join(f.output, `provenance-${target.id}.json`), original.raw);
  await writeFile(join(f.output, `service-lassoctl-${version}-${target.id}.tar.gz`), archive(members)); await rebind(f.output);
  await assert.rejects(verifyCandidateDirectory(f.output, version, sourceSha));
});

test('provider current source/attempt/positive jobs cannot be inferred from an aggregate success', () => {
  assert.doesNotThrow(() => assertScopedRunEvidence(runEvidence(), workflowRun, sourceSha));
  for (const mutate of [value => value.latest.run_attempt++, value => value.attempt.head_sha = 'f'.repeat(40),
    value => value.jobs.jobs[0].conclusion = 'skipped', value => value.jobs.jobs[1].run_attempt++,
    value => value.jobs.jobs[2].steps[0].conclusion = 'failure', value => value.jobs.jobs.push(structuredClone(value.jobs.jobs[0])),
    value => value.jobs.jobs[3].name = 'Build and execute native target (darwin-arm64)', value => value.jobs.total_count++]) {
    const value = runEvidence(); mutate(value); assert.throws(() => assertScopedRunEvidence(value, workflowRun, sourceSha));
  }
});

test('publisher fixture rereads all provider controls before all twelve mutations and uses all eight held bytes', async t => {
  const f = await fixture(t), { held } = await verifyCandidateDirectory(f.output, version, sourceSha), adapter = provider(held);
  assert.equal((await publish(f.output, adapter)).result, 'published');
  assert.equal(adapter.uploads.size, 8); assert.equal(adapter.rounds(), 13);
  const writes = adapter.calls.filter(call => call.options.method !== 'GET'); assert.equal(writes.length, 12);
  for (const [name, bytes] of held) assert.ok(adapter.uploads.get(name).equals(bytes));
  assert.equal(adapter.calls.filter(call => call.url.startsWith('https://github.com/')).length, 8);
  assert.equal(adapter.calls.filter(call => call.url.includes('/releases/assets/')).length, 8);
});

test('missing provider policy/current attempt and moved develop block actual writes; later movement preserves prefix', async t => {
  const f = await fixture(t), { held } = await verifyCandidateDirectory(f.output, version, sourceSha);
  for (const changePolicy of [policy => policy.branch.commit.sha = 'f'.repeat(40), policy => policy.immutable.enabled = false,
    policy => policy.protection.required_pull_request_reviews.bypass_pull_request_allowances.apps.push({ id: 1 }),
    policy => policy.environment.protection_rules[0].wait_timer = 0]) {
    const adapter = provider(held, { changePolicy }); await assert.rejects(publish(f.output, adapter));
    assert.equal(adapter.calls.filter(call => call.options.method !== 'GET').length, 0);
  }
  const run = runEvidence(); run.jobs.jobs[2].steps[0].conclusion = 'skipped';
  const unavailable = provider(held, { run }); await assert.rejects(publish(f.output, unavailable));
  assert.equal(unavailable.calls.filter(call => call.options.method !== 'GET').length, 0);
  const moved = provider(held, { changePolicy: (policy, round) => { if (round >= 3) policy.branch.commit.sha = 'f'.repeat(40); } });
  await assert.rejects(publish(f.output, moved)); assert.equal(moved.calls.filter(call => call.options.method !== 'GET').length, 1);
});

test('only exact immutable same-byte collision is readback-only; corrupt public bytes never republish', async t => {
  const f = await fixture(t), { held } = await verifyCandidateDirectory(f.output, version, sourceSha);
  const exact = provider(held, { collision: true }); assert.equal((await publish(f.output, exact)).result, 'readback-only');
  assert.equal(exact.calls.filter(call => call.options.method !== 'GET').length, 0);
  const wrong = provider(held, { collision: true, wrongBytes: true }); await assert.rejects(publish(f.output, wrong));
  assert.equal(wrong.calls.filter(call => call.options.method !== 'GET').length, 0);
});

test('enforced no-bypass update and deletion policy is mandatory before the first mutation', async t => {
  const f = await fixture(t), { held } = await verifyCandidateDirectory(f.output, version, sourceSha);
  for (const changeRules of [rule => rule.enforcement = 'evaluate', rule => rule.target = 'branch',
    rule => rule.bypass_actors.push({ actor_id: 1, actor_type: 'Integration', bypass_mode: 'always' }),
    rule => rule.rules.pop(), rule => rule.conditions.ref_name.exclude.push(`refs/tags/${tag}`),
    rule => rule.conditions.ref_name.include = ['refs/tags/other-*']]) {
    const adapter = provider(held, { changeRules }); await assert.rejects(publish(f.output, adapter));
    assert.equal(adapter.calls.filter(call => call.options.method !== 'GET').length, 0);
  }
});

test('actual ref rebinding after creation, during uploads and after publication never reports success', async t => {
  const f = await fixture(t), { held } = await verifyCandidateDirectory(f.output, version, sourceSha);
  for (const cut of [state => state.reads === 1, state => state.uploads >= 1, state => state.published]) {
    const adapter = provider(held, { changeTag: (value, state) => {
      if (cut(state)) value.object.sha = 'b'.repeat(40); // even a different tag object for the SAME commit is forbidden
    } });
    await assert.rejects(publish(f.output, adapter), /reference changed/u);
    if (!adapter.calls.some(call => call.options.method === 'PATCH')) assert.ok(adapter.uploads.size < 8);
  }
  const weakened = provider(held, { changeRules: (rule, round) => { if (round >= 5) rule.bypass_actors.push({ actor_id: 1 }); } });
  await assert.rejects(publish(f.output, weakened));
  assert.equal(weakened.calls.filter(call => call.options.method === 'PATCH').length, 0);
});

async function positiveStructuralFixture(f, target) {
  const receiptDirectory = join(f.root, 'positive-structural-fixture'); await mkdir(receiptDirectory);
  const original = f.originals.get(target.id), digest = sha256(original.executable);
  const initial = { schemaVersion: 2, runId: '11111111-1111-4111-8111-111111111111', sourceHead: sourceSha,
    rawHeadSha256: 'a'.repeat(64), recursiveHeadTreeSha256: 'b'.repeat(64),
    declaredRuntimePathMetadata: { workspaceRoot: join(f.root, 'workspace'), instanceRegistryPath: join(f.root, 'instance'), hostPortRegistryPath: join(f.root, 'port') },
    nativeExecutableSha256: null, ownedBirth: true, actualClose: null };
  const phases = {};
  for (const phase of ['smoke', 'route', 'archive']) phases[phase] = { schemaVersion: 1, phase, sourceHead: sourceSha,
    rawHeadSha256: initial.rawHeadSha256, recursiveHeadTreeSha256: initial.recursiveHeadTreeSha256, command: 'FIXTURE_ONLY',
    argumentsSha256: 'c'.repeat(64), nativeExecutableSha256: digest, nativeExecutable: { status: 'present', sha256: digest, errorCode: null },
    artifactSha256: phase === 'archive' ? sha256(original.archive) : null,
    artifact: { status: phase === 'archive' ? 'present' : 'not-requested', sha256: phase === 'archive' ? sha256(original.archive) : null, errorCode: null },
    qualificationStatus: 'verified', expectedExit: 'zero', rawClose: { code: 0, signal: null, spawnError: null }, passed: true, innerRoute: null };
  const closed = { ...initial, nativeExecutableSha256: digest, phases,
    actualClose: { executableSha256: digest, rawPhaseResults: Object.fromEntries(Object.entries(phases).map(([name, value]) => [name, value.rawClose])), outcome: 'passed' } };
  const save = async () => {
    await writeFile(join(receiptDirectory, 'initial.json'), jsonBytes(initial)); await writeFile(join(receiptDirectory, 'closed.json'), jsonBytes(closed));
    for (const [name, value] of Object.entries(phases)) await writeFile(join(receiptDirectory, `phase-${name}.json`), jsonBytes(value));
  };
  await save(); return { receiptDirectory, initial, phases, closed, save };
}

test('existing closed-receipt structural regression does not turn negative/open or changed bytes into positive proof', async t => {
  const f = await fixture(t), target = NATIVE_TARGETS[0], p = await positiveStructuralFixture(f, target);
  const options = { receiptDirectory: p.receiptDirectory, nativeDirectory: f.native.get(target.id),
    archiveBytes: f.originals.get(target.id).archive, target, version, sourceSha };
  assert.equal((await verifyPositiveNativeReceipt(options)).executableSha256, sha256(f.originals.get(target.id).executable));
  p.phases.route.qualificationStatus = 'unavailable'; await p.save(); await assert.rejects(verifyPositiveNativeReceipt(options));
  p.phases.route.qualificationStatus = 'verified'; p.phases.route.rawClose.code = 1; await p.save(); await assert.rejects(verifyPositiveNativeReceipt(options));
  p.phases.route.rawClose.code = 0; p.phases.archive.artifactSha256 = 'f'.repeat(64); await p.save(); await assert.rejects(verifyPositiveNativeReceipt(options));
  // No fixture receipt can implement the unavailable genuine producer or emit a success directory.
  const absent = join(f.root, 'real-positive-must-not-exist');
  await assert.rejects(producePositiveNativeQualification({ directory: f.native.get(target.id), receiptDirectory: absent, version, sourceSha }), /unavailable/u);
  await assert.rejects(lstat(absent), { code: 'ENOENT' });
});

test('workflow source binding rejects changed policy/source/attempt without selecting platforms', async () => {
  const env = { GITHUB_REPOSITORY: 'service-lasso/service-lasso-cli', GITHUB_EVENT_NAME: 'workflow_dispatch', GITHUB_REF: 'refs/heads/develop',
    GITHUB_SHA: sourceSha, GITHUB_WORKFLOW_SHA: sourceSha,
    GITHUB_WORKFLOW_REF: 'service-lasso/service-lasso-cli/.github/workflows/release-scoped.yml@refs/heads/develop',
    SCOPE_POLICY_SHA256: POLICY_SHA256, GITHUB_RUN_ID: '47', GITHUB_RUN_ATTEMPT: '1' };
  assert.equal((await assertScopedWorkflowSource(env)).runAttempt, 1);
  for (const patch of [{ GITHUB_WORKFLOW_SHA: 'f'.repeat(40) }, { GITHUB_EVENT_NAME: 'push' }, { SCOPE_POLICY_SHA256: 'f'.repeat(64) },
    { GITHUB_RUN_ATTEMPT: '0' }, { CANDIDATE_SHA: 'f'.repeat(40) }]) await assert.rejects(assertScopedWorkflowSource({ ...env, ...patch }));
});

test('scoped workflow keeps the complete fixed DAG and separate genuine positive prerequisite; historical route is retained', async () => {
  const { parse } = await import('yaml');
  const scoped = parse(await readFile('.github/workflows/release-scoped.yml', 'utf8'));
  const legacy = parse(await readFile('.github/workflows/release.yml', 'utf8'));
  assert.deepEqual(Object.keys(scoped.on), ['workflow_dispatch']); assert.equal(scoped.on.workflow_dispatch, null);
  assert.equal(scoped.env.SCOPE_POLICY_SHA256, POLICY_SHA256);
  assert.deepEqual(scoped.jobs.native.strategy.matrix.include.map(value => value.target), ['win32-x64', 'linux-x64']);
  assert.deepEqual(legacy.jobs.native.strategy.matrix.include.map(value => value.target), ['win32-x64', 'linux-x64', 'darwin-arm64']);
  assert.equal(scoped.jobs['scoped-terminal'].if, 'always()');
  assert.deepEqual(scoped.jobs['scoped-terminal'].needs, ['identity', 'portable', 'native', 'assemble', 'publish']);
  const steps = scoped.jobs.native.steps, positive = steps.find(step => step.name === 'Require genuine separate positive native route originals');
  assert.ok(positive); assert.equal(positive.if, undefined);
  assert.ok(positive.run.indexOf('produce-native-positive-qualification-scoped.mjs') < positive.run.indexOf('close-native-qualification-receipt.mjs'));
  assert.ok(positive.run.indexOf('close-native-qualification-receipt.mjs') < positive.run.indexOf('verify-positive-native-qualification-scoped.mjs'));
  assert.ok(positive.run.includes('native-qualification-receipts-positive')); assert.ok(!positive.run.includes('--qualification-status unavailable'));
  assert.ok(steps.find(step => step.name === 'Record expected unavailable controlled primary-route result').run.includes('--qualification-status unavailable'));
  for (const step of steps.filter(value => value.uses?.startsWith('actions/upload-artifact@'))) {
    assert.ok(step.with.name.includes('${{ github.run_id }}')); assert.ok(step.with.name.includes('${{ github.run_attempt }}'));
  }
  assert.equal(scoped.jobs.publish.environment.name, 'development-candidate');
  assert.ok(scoped.jobs.publish.steps.some(step => step.run?.includes('publish-protected-candidate-scoped.mjs')));
  assert.ok(!legacy.jobs.publish.steps.some(step => step.run?.includes('publish-protected-candidate-scoped.mjs')));
});

test('scoped CLI diagnostic boundary retains opaque failures without reading or printing their fields', async () => {
  const { runScopedCli } = await import('../scripts/cli-platform-scope.mjs');
  const originalWrite = process.stderr.write, previousExit = process.exitCode; let output = '', touched = 0;
  const opaque = new Proxy({}, { get() { touched++; throw new Error('private field must not be inspected'); } });
  try {
    process.stderr.write = value => { output += value; return true; };
    await runScopedCli(async () => { throw opaque; });
    assert.equal(output, 'Scoped candidate operation failed.\n'); assert.equal(touched, 0); assert.equal(process.exitCode, 1);
  } finally { process.stderr.write = originalWrite; process.exitCode = previousExit; }
});

test('selected CI ignores only declared Mac results and rejects every selected failed/missing/old/unknown job', () => {
  assert.doesNotThrow(() => assertScopedCiEvidence(ciEvidence(), sourceSha));
  for (const mutate of [value => value.latest.run_attempt++, value => value.jobs.jobs[0].conclusion = 'failure',
    value => value.jobs.jobs[1].conclusion = 'skipped', value => value.jobs.jobs[2].conclusion = 'cancelled',
    value => value.jobs.jobs[3].run_attempt--, value => value.jobs.jobs[4].head_sha = 'f'.repeat(40),
    value => value.jobs.jobs[5].name = 'unknown proof', value => value.jobs.jobs.pop(),
    value => value.jobs.jobs[1].name = value.jobs.jobs[0].name]) {
    const value = ciEvidence(); mutate(value); assert.throws(() => assertScopedCiEvidence(value, sourceSha));
  }
});

test('a selected CI failure is zero-write despite matching successful scoped producer fixtures', async t => {
  const f = await fixture(t), { held } = await verifyCandidateDirectory(f.output, version, sourceSha);
  const ci = ciEvidence(); ci.jobs.jobs[3].conclusion = 'failure';
  const adapter = provider(held, { ci }); await assert.rejects(publish(f.output, adapter));
  assert.equal(adapter.calls.filter(call => call.options.method !== 'GET').length, 0);
});

test('publisher preserves verified buffers through path replacement and refuses provider size mismatch before publication', async t => {
  const f = await fixture(t), { held } = await verifyCandidateDirectory(f.output, version, sourceSha);
  const adapter = provider(held, { afterUpload: async count => {
    if (count === 1) await writeFile(join(f.output, `service-lassoctl-${version}-linux-x64.tar.gz`), 'changed path after held verification');
  } });
  assert.equal((await publish(f.output, adapter)).result, 'published');
  assert.ok(adapter.uploads.get(`service-lassoctl-${version}-linux-x64.tar.gz`).equals(held.get(`service-lassoctl-${version}-linux-x64.tar.gz`)));
  await writeFile(join(f.output, `service-lassoctl-${version}-linux-x64.tar.gz`), held.get(`service-lassoctl-${version}-linux-x64.tar.gz`));
  const wrongSize = provider(held, { assetSizeDelta: 1 }); await assert.rejects(publish(f.output, wrongSize));
  assert.equal(wrongSize.calls.filter(call => call.options.method === 'PATCH').length, 0);
});

test('missing target, changed scope digest and over-budget declared asset deny without allocating payload', async t => {
  const f = await fixture(t), verified = await verifyCandidateDirectory(f.output, version, sourceSha);
  const missing = structuredClone(verified.manifest); missing.assets.pop(); assert.throws(() => validateManifest(missing, version, sourceSha));
  const over = structuredClone(verified.manifest); over.assets[0].size = 256 * 1024 * 1024 + 1; assert.throws(() => validateManifest(over, version, sourceSha));
  const wrongScope = structuredClone(verified.manifest); wrongScope.scope.policySha256 = 'f'.repeat(64); assert.throws(() => validateManifest(wrongScope, version, sourceSha));
});
