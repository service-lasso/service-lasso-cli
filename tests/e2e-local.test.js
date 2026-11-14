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
