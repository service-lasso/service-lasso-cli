import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import test from 'node:test';
import { fileURLToPath } from 'node:url';
import { isAbsolute } from 'node:path';
const sharedRoot = process.env.CLI39_REVIEWED_SHARED_SOURCE_ROOT;
const officialHost = process.env.CLI39_REVIEWED_POWERSHELL_EXECUTABLE;
test('publisher actual owning functions retain surrogate chain and policy observations', {
  skip: process.platform !== 'win32' || !sharedRoot || !officialHost ? 'Requires admitted Windows source-case command and exact reviewed shared source root; not qualification.' : false,
}, () => {
  assert.ok(isAbsolute(sharedRoot), 'Reviewed shared source root must be absolute; routing is not admission.');
  assert.ok(isAbsolute(officialHost), 'Reviewed official host must be absolute; PATH is not authority.');
  const fixture = fileURLToPath(new URL('./fixtures/publisher-source-cases32.ps1', import.meta.url));
  const result = spawnSync(officialHost,
    ['-NoProfile', '-NonInteractive', '-File', fixture, '-SharedSourceRoot', sharedRoot], { encoding: 'utf8' });
  assert.equal(result.error, undefined);
  assert.equal(result.status, 0, result.stdout + result.stderr);
  assert.match(result.stdout, /publisher-source-cases-passed-surrogate-only/);
});