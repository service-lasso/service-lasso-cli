import { readdir } from 'node:fs/promises';
import { isDeepStrictEqual } from 'node:util';
import { assertScope, readSourceScope } from './cli-platform-scope.mjs';
import { assertClosedObject, fail, parseStrictJson, readHeldFile, readTarArchive, sha256,
  verifyNativeArchive } from './protected-candidate-lib.mjs';
export { assertProviderPreflight, canonicalPublicAssetUrl, allowedRedirect, fail, parseStrictJson,
  readHeldFile, sha256 } from './protected-candidate-lib.mjs';

export const NATIVE_TARGETS = Object.freeze([
  Object.freeze({ id: 'win32-x64', platform: 'win32', architecture: 'x64', executable: 'service-lassoctl.exe', confinedWriter: 'service-lasso-confined-scaffold.exe' }),
  Object.freeze({ id: 'linux-x64', platform: 'linux', architecture: 'x64', executable: 'service-lassoctl', confinedWriter: 'service-lasso-confined-scaffold' }),
]);
const VERSION = /^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/u, SHA = /^[0-9a-f]{40}$/u;
const DIGEST = /^[0-9a-f]{64}$/u, REPOSITORY = 'service-lasso/service-lasso-cli';
export const ascii = (a, b) => a < b ? -1 : a > b ? 1 : 0;
export function identity(version, sourceSha) {
  if (typeof version !== 'string' || !VERSION.test(version) || typeof sourceSha !== 'string' ||
      !SHA.test(sourceSha) || !version.endsWith(`-dev.${sourceSha.slice(0, 7)}`)) fail('scoped frozen identity is invalid.');
  return `cli-v${version}-candidate-${sourceSha.slice(0, 7)}`;
}
export function expectedAssets(version) {
  if (typeof version !== 'string' || !VERSION.test(version)) fail('scoped version is invalid.');
  return [{ name: `service-lassoctl-${version}.tgz`, kind: 'portable', target: null },
    { name: 'candidate.json', kind: 'portable-record', target: null },
    ...NATIVE_TARGETS.flatMap(target => [
      { name: `service-lassoctl-${version}-${target.id}.tar.gz`, kind: 'native', target: target.id },
      { name: `provenance-${target.id}.json`, kind: 'provenance', target: target.id },
    ])].sort((a, b) => ascii(a.name, b.name));
}
export function validateManifest(manifest, version, sourceSha) {
  const tag = identity(version, sourceSha);
  assertClosedObject(manifest, ['schemaVersion', 'assets', 'candidateTag', 'checksums', 'source', 'version', 'scope'], 'scoped protected manifest');
  if (manifest.schemaVersion !== 2 || manifest.version !== version || manifest.candidateTag !== tag)
    fail('scoped protected schema or frozen identity differs.');
  assertScope(manifest.scope);
  assertClosedObject(manifest.source, ['commit', 'repository'], 'scoped source');
  assertClosedObject(manifest.checksums, ['algorithm', 'entries', 'file'], 'scoped checksums');
  const expected = expectedAssets(version);
  if (manifest.source.repository !== REPOSITORY || manifest.source.commit !== sourceSha ||
      manifest.checksums.algorithm !== 'sha256' || manifest.checksums.file !== 'SHA256SUMS.txt' ||
      !Array.isArray(manifest.assets) || manifest.assets.length !== 6 || manifest.assets.length !== expected.length)
    fail('scoped source or six-asset inventory differs.');
  for (let index = 0; index < expected.length; index++) {
    const asset = manifest.assets[index], wanted = expected[index];
    assertClosedObject(asset, ['kind', 'name', 'sha256', 'size', 'target'], 'scoped asset');
    if (asset.kind !== wanted.kind || asset.name !== wanted.name || asset.target !== wanted.target ||
        typeof asset.sha256 !== 'string' || !DIGEST.test(asset.sha256) ||
        !Number.isSafeInteger(asset.size) || asset.size < 1 || asset.size > 256 * 1024 * 1024)
      fail('scoped asset identity, ASCII order or size differs.');
  }
  const entries = ['development-candidate.json', ...expected.map(asset => asset.name)].sort(ascii);
  if (!isDeepStrictEqual(manifest.checksums.entries, entries) || entries.length !== 7)
    fail('scoped checksum declaration is not the exact seven ASCII rows.');
  return manifest;
}
export function verifyPortableRecord(record, bytes, version, sourceSha) {
  const tag = identity(version, sourceSha);
  assertClosedObject(record, ['schemaVersion', 'assets', 'candidateTag', 'package', 'platforms', 'source', 'version', 'scope'], 'scoped portable record');
  assertScope(record.scope);
  assertClosedObject(record.source, ['commit', 'repository'], 'portable source');
  assertClosedObject(record.package, ['command', 'entrypoint', 'name', 'node'], 'portable package');
  if (!Array.isArray(record.assets) || record.assets.length !== 1) fail('portable requires exactly one archive.');
  assertClosedObject(record.assets[0], ['name', 'sha256', 'size'], 'portable archive');
  const asset = record.assets[0];
  if (record.schemaVersion !== 2 || record.candidateTag !== tag || record.version !== version ||
      record.source.repository !== REPOSITORY || record.source.commit !== sourceSha ||
      record.package.name !== '@service-lasso/cli' || record.package.command !== 'service-lassoctl' ||
      record.package.entrypoint !== 'dist/index.js' || record.package.node !== '>=22.12.0' ||
      !isDeepStrictEqual(record.platforms, ['win32', 'linux', 'darwin']) ||
      asset.name !== `service-lassoctl-${version}.tgz` || asset.sha256 !== sha256(bytes) || asset.size !== bytes.length)
    fail('scoped portable identity differs.');
  const members = readTarArchive(bytes);
  for (const name of members.keys()) if (!/^package\/(?:README\.md|package\.json|dist\/[A-Za-z0-9._/-]+)$/u.test(name))
    fail('portable archive contains an undeclared member.');
  if (!members.has('package/package.json') || !members.has('package/README.md') || !members.has('package/dist/index.js'))
    fail('portable archive lacks the required payload.');
  const pkg = parseStrictJson(members.get('package/package.json'), 'portable archive package');
  if (pkg.name !== '@service-lasso/cli' || pkg.version !== version || pkg.engines?.node !== '>=22.12.0' ||
      pkg.bin?.['service-lassoctl'] !== './dist/index.js') fail('portable archive package identity differs.');
  return record;
}
export function verifyScopedNative(archive, provenanceBytes, target, sourceSha, version) {
  if (!NATIVE_TARGETS.includes(target)) fail('native target is outside source-selected scope.');
  // This is the unchanged five-member native1 producer/receipt grammar, not a schema upgrade.
  verifyNativeArchive(archive, provenanceBytes, target, sourceSha, version);
  const provenance = parseStrictJson(provenanceBytes, 'scoped native provenance');
  assertClosedObject(provenance.tools, ['node', 'esbuild', 'postject'], 'native tools');
  assertClosedObject(provenance.sea, ['mainFormat', 'useCodeCache', 'execArgvExtension'], 'native SEA');
  if (!isDeepStrictEqual(provenance.tools, { node: '22.23.2', esbuild: '0.28.2', postject: '1.0.0-alpha.6' }) ||
      !isDeepStrictEqual(provenance.sea, { mainFormat: 'commonjs', useCodeCache: false, execArgvExtension: 'none' }))
    fail('native original tool pins or SEA settings differ.');
}
export function checksumBytes(manifestBytes, assets) {
  return Buffer.from([{ name: 'development-candidate.json', sha256: sha256(manifestBytes) }, ...assets]
    .sort((a, b) => ascii(a.name, b.name)).map(asset => `${asset.sha256}  ${asset.name}\n`).join(''), 'utf8');
}
export async function verifyCandidateDirectory(directory, version, sourceSha) {
  await readSourceScope();
  const manifestBytes = await readHeldFile(directory, 'development-candidate.json');
  const manifest = validateManifest(parseStrictJson(manifestBytes, 'scoped protected manifest'), version, sourceSha);
  const files = await readdir(directory, { withFileTypes: true });
  const wanted = new Set(['development-candidate.json', 'SHA256SUMS.txt', ...manifest.assets.map(asset => asset.name)]);
  if (wanted.size !== 8 || files.length !== 8 || files.some(file => !file.isFile() || !wanted.has(file.name)))
    fail('scoped candidate directory is not exactly eight regular files.');
  const held = new Map([['development-candidate.json', manifestBytes]]);
  for (const asset of manifest.assets) {
    const bytes = await readHeldFile(directory, asset.name, asset.size);
    if (sha256(bytes) !== asset.sha256) fail('scoped asset digest differs.');
    held.set(asset.name, bytes);
  }
  verifyPortableRecord(parseStrictJson(held.get('candidate.json'), 'scoped portable record'),
    held.get(`service-lassoctl-${version}.tgz`), version, sourceSha);
  for (const target of NATIVE_TARGETS) verifyScopedNative(held.get(`service-lassoctl-${version}-${target.id}.tar.gz`),
    held.get(`provenance-${target.id}.json`), target, sourceSha, version);
  const sums = await readHeldFile(directory, 'SHA256SUMS.txt');
  if (!sums.equals(checksumBytes(manifestBytes, manifest.assets))) fail('scoped checksum bytes differ.');
  held.set('SHA256SUMS.txt', sums);
  return { manifest, held };
}

export const SCOPED_REQUIRED_JOBS = Object.freeze([
  'Freeze exact develop candidate identity', 'Build portable archive once for frozen SHA',
  'Build and execute native target (win32-x64)', 'Build and execute native target (linux-x64)',
  'Assemble and verify the closed frozen candidate',
]);
const publisherJob = 'Preflight and publish exact immutable candidate bytes';
const terminalJob = 'Scoped complete current-attempt release gate';
export function assertScopedRunEvidence({ latest, attempt, jobs }, workflowRun, sourceSha) {
  assertClosedObject(workflowRun, ['id', 'attempt', 'workflowSha'], 'scoped current run');
  if (!Number.isSafeInteger(workflowRun.id) || workflowRun.id < 1 || !Number.isSafeInteger(workflowRun.attempt) ||
      workflowRun.attempt < 1 || workflowRun.workflowSha !== sourceSha) fail('scoped run identity differs.');
  for (const run of [latest, attempt]) {
    if (run?.id !== workflowRun.id || run.run_attempt !== workflowRun.attempt || run.head_sha !== sourceSha ||
        run.head_branch !== 'develop' || run.event !== 'workflow_dispatch' ||
        run.path !== '.github/workflows/release-scoped.yml' || run.repository?.full_name !== REPOSITORY ||
        !Number.isSafeInteger(run.workflow_id) || run.workflow_id < 1) fail('provider run/source/attempt differs.');
  }
  if (latest.workflow_id !== attempt.workflow_id) fail('provider workflow identity differs.');
  assertClosedObject(jobs, ['total_count', 'jobs'], 'scoped provider jobs');
  const allowed = new Set([...SCOPED_REQUIRED_JOBS, publisherJob, terminalJob]);
  if (!Array.isArray(jobs.jobs) || !Number.isSafeInteger(jobs.total_count) || jobs.total_count !== jobs.jobs.length ||
      jobs.total_count < SCOPED_REQUIRED_JOBS.length + 1 || jobs.total_count > allowed.size)
    fail('provider current-attempt job inventory is incomplete or extra.');
  const seen = new Set(), ids = new Set();
  for (const job of jobs.jobs) {
    if (!allowed.has(job?.name) || seen.has(job.name) || !Number.isSafeInteger(job.id) || job.id < 1 || ids.has(job.id) ||
        job.run_id !== workflowRun.id || job.run_attempt !== workflowRun.attempt || job.head_sha !== sourceSha)
      fail('provider job identity/source/current attempt differs.');
    seen.add(job.name); ids.add(job.id);
  }
  for (const name of SCOPED_REQUIRED_JOBS) {
    const job = jobs.jobs.find(value => value.name === name);
    if (!job || job.status !== 'completed' || job.conclusion !== 'success') fail('required scoped job did not succeed.');
    if (name.startsWith('Build and execute native target')) {
      const positive = job.steps?.filter(step => step.name === 'Require genuine separate positive native route originals');
      if (!Array.isArray(positive) || positive.length !== 1 || positive[0].status !== 'completed' || positive[0].conclusion !== 'success')
        fail('genuine separate positive native producer/closed receipt did not succeed.');
    }
  }
  const publishing = jobs.jobs.find(job => job.name === publisherJob);
  if (!publishing || publishing.status !== 'in_progress' || publishing.conclusion !== null)
    fail('current scoped publisher is not the actual live provider job.');
}

export const SCOPED_CI_REQUIRED_JOBS = Object.freeze([
  'Node 22 CLI checks (windows-latest)', 'Node 22 CLI checks (ubuntu-latest)',
  'Completion shell checks (ubuntu-latest)', 'Direct real-Core reads (ubuntu-latest)',
  'Native SEA direct execution (Windows x64)', 'Native SEA direct execution (Linux x64)',
]);
const deferredCiJobs = Object.freeze(['Node 22 CLI checks (macos-latest)', 'Native SEA direct execution (macOS arm64)']);
export function assertScopedCiEvidence({ listed, latest, attempt, jobs }, sourceSha) {
  if (!listed || !Number.isSafeInteger(listed.id) || listed.id < 1 || !Number.isSafeInteger(listed.run_attempt) || listed.run_attempt < 1)
    fail('natural develop CI run is unavailable.');
  for (const run of [listed, latest, attempt]) {
    if (run?.id !== listed.id || run.run_attempt !== listed.run_attempt || run.head_sha !== sourceSha || run.head_branch !== 'develop' ||
        run.event !== 'push' || run.path !== '.github/workflows/ci.yml' || run.repository?.full_name !== REPOSITORY ||
        !Number.isSafeInteger(run.workflow_id) || run.workflow_id < 1 || run.workflow_id !== listed.workflow_id)
      fail('CI workflow/source/latest attempt differs.');
  }
  assertClosedObject(jobs, ['total_count', 'jobs'], 'selected CI job inventory');
  const allowed = new Set([...SCOPED_CI_REQUIRED_JOBS, ...deferredCiJobs]);
  if (!Array.isArray(jobs.jobs) || jobs.total_count !== allowed.size || jobs.jobs.length !== allowed.size)
    fail('CI job inventory is incomplete or extra.');
  const names = new Set(), ids = new Set();
  for (const job of jobs.jobs) {
    if (!allowed.has(job?.name) || names.has(job.name) || !Number.isSafeInteger(job.id) || job.id < 1 || ids.has(job.id) ||
        job.run_id !== listed.id || job.run_attempt !== listed.run_attempt || job.head_sha !== sourceSha)
      fail('CI job is unknown, duplicated or not the actual current source/attempt.');
    names.add(job.name); ids.add(job.id);
    if (SCOPED_CI_REQUIRED_JOBS.includes(job.name) && (job.status !== 'completed' || job.conclusion !== 'success'))
      fail('selected Windows/Linux/completion/Core/native CI did not succeed.');
  }
  // Deferred Mac jobs are N/A, never PASS. The old all-three aggregate is not a selected-scope gate.
}
