import assert from 'node:assert/strict';
import test from 'node:test';
test('controlled helper protocol faults never become actual-Core PASS', { timeout: 30000 }, async () => {
  const { mkdtemp, writeFile, readFile, access, realpath } = await import('node:fs/promises');
  const { tmpdir } = await import('node:os');
  const { join, resolve } = await import('node:path');
  const { execFileSync } = await import('node:child_process');
  const { run } = await import('./e2e-local.mjs');
  const parent = await realpath(await mkdtemp(join(tmpdir(), 'cli-e2e-adversarial-')));
  const core = process.env.SERVICE_LASSO_TEST_CORE_DIR, revision = process.env.SERVICE_LASSO_TEST_CORE_REVISION;
  assert.ok(core && /^[a-f0-9]{40}$/.test(revision ?? ''), 'platform integration requires explicit pinned Core private setup source');
  assert.equal(execFileSync('git', ['-C', core, 'rev-parse', 'HEAD'], { encoding: 'utf8' }).trim(), revision);
  for (const fault of ['wrong-receipt', 'leftover-credential', 'premature-ready', 'timeout']) {
    const helper = join(parent, `${fault}.mjs`), output = join(parent, fault);
    await writeFile(helper, `import {mkdir,writeFile,rm} from 'node:fs/promises';import {join} from 'node:path';
const args=process.argv.slice(2);const output=args[args.indexOf('--output-dir')+1];await mkdir(output,{mode:0o700});
const connectionFile=join(output,'connection.json'),receiptFile=join(output,'receipt.json');
if('${fault}'!=='premature-ready'&&'${fault}'!=='timeout'){await writeFile(connectionFile,JSON.stringify({url:'http://127.0.0.1:1',token:'controlled-helper-test-only'}),{mode:0o600});await writeFile(receiptFile,JSON.stringify({coreRevision:'wrong'}));}
if('${fault}'!=='timeout')console.log(JSON.stringify({ready:true,connectionFile,receiptFile}));
process.stdin.on('data',async()=>{if('${fault}'!=='leftover-credential')await rm(connectionFile,{force:true});process.exit(0);});`);
    const result = await run(['--core-dir', core, '--core-revision', revision, '--core-fixture-helper', helper, '--output-dir', output], { startup: fault === 'timeout' ? 100 : 5000, cleanup: 5000 });
    assert.notEqual(result.status, 'PASS', fault);
    const retained = JSON.parse(await readFile(join(output, 'result.json'), 'utf8'));
    assert.equal(JSON.stringify(retained).includes('controlled-helper-test-only'), false);
    if (fault === 'leftover-credential') await access(join(output, 'core-fixture/connection.json'));
    assert.ok(result.stages.some(row => row.stage === 'owned-fixture-cleanup' && row.status === 'FAIL'));
  }
  // Controlled failure evidence is retained; it never represents Core acceptance.
});
