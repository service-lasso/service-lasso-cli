import test from 'node:test';
import assert from 'node:assert/strict';
import { nativeLifecycleDiagnostics } from '../scripts/native-lifecycle-diagnostics.mjs';

test('opt-in lifecycle observations cannot echo original private error or path objects', () => {
  const records = [];
  const privateValue = { secret: 'original-input-not-public', toString() { throw new Error('private conversion attempted'); } };
  const observe = nativeLifecycleDiagnostics(true, value => records.push(value));
  observe('native', 'close', privateValue, privateValue);
  const row = JSON.parse(records[0]);
  assert.deepEqual(Object.keys(row), ['diagnostic', 'sequence', 'stage', 'event', 'code', 'signal']);
  assert.equal(row.code, null);
  assert.equal(row.signal, null);
  assert.doesNotMatch(records[0], /original-input-not-public/);
  assert.throws(() => observe(privateValue, 'close'));
  assert.equal(records.length, 1);
});

test('disabled lifecycle observations do not call diagnostic output or coerce private values', () => {
  const privateValue = { toString() { throw new Error('private conversion attempted'); } };
  const observe = nativeLifecycleDiagnostics(false, () => { throw new Error('unexpected diagnostic output'); });
  observe(privateValue, privateValue, privateValue, privateValue);
});