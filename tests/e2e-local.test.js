import assert from 'node:assert/strict';
import test from 'node:test';
import { options, command } from '../scripts/e2e-local.mjs';

test('E2E requires explicit exact Core identity and fresh absolute evidence destination', () => {
  assert.throws(() => options([]));
  assert.throws(() => options(['--core-dir', '/tmp/core', '--core-revision', 'develop', '--output-dir', '/tmp/proof']));
  assert.throws(() => options(['--core-dir', '/tmp/core', '--core-revision', 'a'.repeat(40), '--output-dir', 'relative']));
  assert.throws(() => options(['--core-dir', '/tmp/core', '--core-dir', '/tmp/other']));
  const selected = options(['--core-dir', '/tmp/core', '--core-revision', 'a'.repeat(40), '--output-dir', '/tmp/proof']);
  assert.equal(selected['core-fixture-helper'], '/tmp/core/scripts/operator-e2e-fixture.mjs');
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
  const receipt = { schemaVersion: 1, coreRevision: 'a'.repeat(40), coreTree: 'b'.repeat(40), cleanSource: true, classification: 'source-built-ordinary-smoke', coreArtifactSHA256: 'c'.repeat(64), sourceInputSHA256: 'd'.repeat(64), artifactInventorySHA256: 'e'.repeat(64), tools: {}, initializationAPI: 'fixture', stage: 'ready', status: 'running' };
  assert.doesNotThrow(() => validateReceipt(receipt, 'a'.repeat(40)));
  assert.throws(() => validateReceipt({ ...receipt, classification: 'unqualified-artifact' }, 'a'.repeat(40)));
  assert.throws(() => validateReceipt({ ...receipt, sourceInputSHA256: null }, 'a'.repeat(40)));
  assert.throws(() => validateReceipt({ ...receipt, stage: 'closed', status: 'passed', cleanup: { status: 'complete', serverClosed: true, portClosed: false, credentialRemoved: true } }, 'a'.repeat(40), true));
});

test('controlled helper protocol faults never become actual-Core PASS', { timeout: 30000 }, async () => {
  const { mkdtemp, writeFile, readFile, access } = await import('node:fs/promises');
  const { tmpdir } = await import('node:os');
  const { join, resolve } = await import('node:path');
  const { execFileSync } = await import('node:child_process');
  const { run } = await import('../scripts/e2e-local.mjs');
  const parent = await mkdtemp(join(tmpdir(), 'cli-e2e-adversarial-'));
  const revision = execFileSync('git', ['rev-parse', 'HEAD'], { encoding: 'utf8' }).trim();
  for (const fault of ['wrong-receipt', 'leftover-credential', 'premature-ready', 'timeout']) {
    const helper = join(parent, `${fault}.mjs`), output = join(parent, fault);
    await writeFile(helper, `import {mkdir,writeFile,rm} from 'node:fs/promises';import {join} from 'node:path';
const args=process.argv.slice(2);const output=args[args.indexOf('--output-dir')+1];await mkdir(output,{mode:0o700});
const connectionFile=join(output,'connection.json'),receiptFile=join(output,'receipt.json');
if('${fault}'!=='premature-ready'&&'${fault}'!=='timeout'){await writeFile(connectionFile,JSON.stringify({url:'http://127.0.0.1:1',token:'controlled-helper-test-only'}),{mode:0o600});await writeFile(receiptFile,JSON.stringify({coreRevision:'wrong'}));}
if('${fault}'!=='timeout')console.log(JSON.stringify({ready:true,connectionFile,receiptFile}));
process.stdin.on('data',async()=>{if('${fault}'!=='leftover-credential')await rm(connectionFile,{force:true});process.exit(0);});`);
    const result = await run(['--core-dir', resolve('.'), '--core-revision', revision, '--core-fixture-helper', helper, '--output-dir', output], { startup: fault === 'timeout' ? 100 : 5000, cleanup: 5000 });
    assert.notEqual(result.status, 'PASS', fault);
    const retained = JSON.parse(await readFile(join(output, 'result.json'), 'utf8'));
    assert.equal(JSON.stringify(retained).includes('controlled-helper-test-only'), false);
    if (fault === 'leftover-credential') await access(join(output, 'core-fixture/connection.json'));
    assert.ok(result.stages.some(row => row.stage === 'owned-fixture-cleanup' && row.status === 'FAIL'));
  }
  // Controlled failure evidence is retained; it never represents Core acceptance.
});
