import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import { mkdtemp, mkdir, writeFile, access, readFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { delimiter, dirname, join } from 'node:path';
import test from 'node:test';
import { command } from '../scripts/e2e-local.mjs';
import { installPortable, npmTool } from '../scripts/e2e-portable.mjs';
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
async function fixture() {
  const output = await mkdtemp(join(tmpdir(), 'cli-portable-negative-'));
  const env = { HOME: output, USERPROFILE: output, PATH: dirname(process.execPath) + delimiter + process.env.PATH, ...(process.env.SystemRoot ? { SystemRoot: process.env.SystemRoot } : {}) };
  return { output, environment: env, tool: await npmTool(env), execute: command };
}
test('wrong archive hash is rejected before consumer installation or command execution', async () => {
  const context = await fixture(), archive = join(context.output, 'wrong.tgz'), consumer = join(context.output, 'consumer');
  await writeFile(archive, 'controlled bytes'); let calls = 0;
  await assert.rejects(() => installPortable({ ...context, archive, expectedHash: '0'.repeat(64), consumer, execute: async () => { calls++; }, log: join(context.output, 'install.log') }), /digest mismatch/);
  assert.equal(calls, 0); await assert.rejects(access(consumer));
});
test('corrupted archive with its matching digest fails actual scripts-disabled npm install', { timeout: 75000 }, async () => {
  const context = await fixture(), archive = join(context.output, 'corrupt.tgz'), bytes = Buffer.from('not a gzip npm package');
  await writeFile(archive, bytes);
  await assert.rejects(() => installPortable({ ...context, archive, expectedHash: hash(bytes), consumer: join(context.output, 'consumer'), log: join(context.output, 'install.log') }), /npm stage failed/);
  assert.ok((await readFile(join(context.output, 'install.log'), 'utf8')).length > 0);
});
test('installed broken consumer entry fails its real command rather than becoming archive acceptance', { timeout: 75000 }, async () => {
  const context = await fixture(), stage = join(context.output, 'stage'); await mkdir(stage);
  await writeFile(join(stage, 'package.json'), JSON.stringify({ name: 'portable-controlled-negative', version: '1.0.0', bin: { 'portable-negative': './index.js' }, files: ['index.js'] }));
  await writeFile(join(stage, 'index.js'), 'throw new Error("controlled-consumer-failure");\n');
  const packed = await command(process.execPath, [context.tool.script, 'pack', '--ignore-scripts', '--json', '--pack-destination', context.output], { cwd: stage, env: context.environment, timeout: 60000 });
  assert.equal(packed.code, 0);
  const archive = join(context.output, JSON.parse(packed.stdout)[0].filename), bytes = await readFile(archive), consumer = join(context.output, 'consumer');
  await installPortable({ ...context, archive, expectedHash: hash(bytes), consumer, log: join(context.output, 'install.log') });
  const result = await command(process.execPath, [join(consumer, 'node_modules/portable-controlled-negative/index.js')], { env: context.environment });
  assert.equal(result.code, 1); assert.match(result.stderr, /controlled-consumer-failure/);
  assert.equal(hash(await readFile(archive)), hash(bytes));
});
