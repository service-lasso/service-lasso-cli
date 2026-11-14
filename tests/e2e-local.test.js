import assert from 'node:assert/strict';
import test from 'node:test';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { options, command } from '../scripts/e2e-local.mjs';

test('E2E requires explicit exact Core identity and fresh absolute evidence destination', () => {
  assert.throws(() => options([]));
  assert.throws(() => options(['--core-dir', '/tmp/core', '--core-revision', 'develop', '--output-dir', '/tmp/proof']));
  assert.throws(() => options(['--core-dir', '/tmp/core', '--core-revision', 'a'.repeat(40), '--output-dir', 'relative']));
  assert.throws(() => options(['--core-dir', '/tmp/core', '--core-dir', '/tmp/other']));
  const core = resolve(tmpdir(), 'core'), proof = resolve(tmpdir(), 'proof');
  const selected = options(['--core-dir', core, '--core-revision', 'a'.repeat(40), '--output-dir', proof]);
  assert.equal(selected['core-fixture-helper'], join(core, 'scripts', 'operator-e2e-fixture.mjs'));
  assert.equal(selected.suite, 'source');
  assert.equal(options(['--core-dir', core, '--core-revision', 'a'.repeat(40), '--output-dir', proof, '--suite', 'portable']).suite, 'portable');
  assert.throws(() => options(['--core-dir', core, '--core-revision', 'a'.repeat(40), '--output-dir', proof, '--suite', 'native']));
});
test('bounded command preserves real exit and stdout/stderr without changing parent environment', async () => {
  const result = await command(process.execPath, ['-e', 'process.stdout.write("output");process.stderr.write("diagnostic");process.exitCode=7'], { env: {} });
  assert.equal(result.code, 7); assert.equal(result.stdout, 'output'); assert.equal(result.stderr, 'diagnostic'); assert.equal(result.timedOut, false);
});
test('deadline terminates and waits for the actual owned child', async () => {
  const result = await command(process.execPath, ['-e', 'setInterval(()=>{},1000)'], { env: {}, timeout: 100 });
  assert.equal(result.timedOut, true); assert.notEqual(result.code, 0);
});
test('output bound terminates actual overflowing child', async () => {
  const result = await command(process.execPath, ['-e', 'process.stdout.write("x".repeat(3*1024*1024));setInterval(()=>{},1000)'], { env: {}, timeout: 5000 });
  assert.equal(result.exceeded, true); assert.ok(result.stdout.length <= 2*1024*1024);
});

test('receipt refuses unqualified artifact, false cleanup and malformed build identities', async () => {
  const { validateReceipt } = await import('../scripts/e2e-local.mjs');
  const receipt = { schemaVersion: 1, coreRevision: 'a'.repeat(40), coreTree: 'b'.repeat(40), cleanSource: true, classification: 'source-built-ordinary-smoke', coreArtifactSHA256: 'c'.repeat(64), sourceInputSHA256: 'd'.repeat(64), artifactInventorySHA256: 'e'.repeat(64), tools: { node: '22', npm: '11', typescript: '5.9', compilerSHA256: 'f'.repeat(64) }, build: { command: ['npm', 'run', 'build'], exitCode: 0, signal: null }, initializationAPI: true, stage: 'ready', status: 'running' };
  assert.doesNotThrow(() => validateReceipt(receipt, 'a'.repeat(40)));
  assert.throws(() => validateReceipt({ ...receipt, classification: 'unqualified-artifact' }, 'a'.repeat(40)));
  assert.throws(() => validateReceipt({ ...receipt, sourceInputSHA256: null }, 'a'.repeat(40)));
  assert.throws(() => validateReceipt({ ...receipt, stage: 'closed', status: 'passed', cleanup: { status: 'complete', serverClosed: true, portClosed: false, credentialRemoved: true } }, 'a'.repeat(40), true));
});

test('public privacy projection contains no raw owner or ACE identities', async () => {
  const { publicPrivacyProof } = await import('../scripts/e2e-private.mjs');
  const value = publicPrivacyProof({ platform: 'win32', policy: 'current-owner-and-system', verified: true, ownerSid: 'controlled-private-owner', rules: [{ sid: 'controlled-private-ACE' }] });
  assert.deepEqual(Object.keys(value).sort(), ['platform', 'policy', 'proofSHA256', 'verified']);
  assert.equal(JSON.stringify(value).includes('controlled-private'), false); assert.match(value.proofSHA256, /^[a-f0-9]{64}$/);
});
test('mismatched Core identity fails before setup import or evidence directory creation', async () => {
  const { mkdtemp, access, realpath } = await import('node:fs/promises');
  const { run } = await import('../scripts/e2e-local.mjs');
  const parent = await realpath(await mkdtemp(join(tmpdir(), 'cli-e2e-no-import-'))), output = join(parent, 'absent');
  await assert.rejects(() => run(['--core-dir', resolve('.'), '--core-revision', '0'.repeat(40), '--output-dir', output]), /Exact clean Core/);
  await assert.rejects(access(output));
});
