#!/usr/bin/env node
import { spawn } from 'node:child_process';
import { createHash } from 'node:crypto';
import { copyFile, mkdir, readFile, realpath, stat, symlink, writeFile, access } from 'node:fs/promises';
import { dirname, isAbsolute, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const sources = ['index', 'config', 'errors', 'core-client', 'scaffold', 'template', 'completion'].map(x => `src/${x}.ts`);
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
export function options(argv) {
  const result = {};
  const names = new Set(['core-dir', 'core-revision', 'core-fixture-helper', 'output-dir']);
  for (let i = 0; i < argv.length; i += 2) {
    const name = argv[i]?.replace(/^--/, '');
    if (!argv[i]?.startsWith('--') || !names.has(name) || !argv[i + 1] || argv[i + 1].startsWith('--') || name in result) throw new Error('Expected unique --core-dir, --core-revision, --output-dir and optional --core-fixture-helper values.');
    result[name] = argv[i + 1];
  }
  if (!result['core-dir'] || !/^[a-f0-9]{40}$/.test(result['core-revision'] ?? '') || !isAbsolute(result['output-dir'] ?? '')) throw new Error('Explicit Core directory, full lowercase revision and absolute fresh output directory required.');
  result['core-dir'] = resolve(result['core-dir']);
  result['core-fixture-helper'] = resolve(result['core-fixture-helper'] ?? join(result['core-dir'], 'scripts/operator-e2e-fixture.mjs'));
  return result;
}
function childEnvironment(home, extra = {}) {
  return { PATH: process.env.PATH ?? '', ...(process.env.SystemRoot ? { SystemRoot: process.env.SystemRoot } : {}), HOME: home, USERPROFILE: home, LANG: 'C.UTF-8', ...extra };
}
export async function command(file, args, { cwd, env, timeout = 10000 } = {}) {
  const child = spawn(file, args, { cwd, env, stdio: ['ignore', 'pipe', 'pipe'] });
  let stdout = '', stderr = '', exceeded = false, timedOut = false;
  const collect = key => chunk => {
    if (stdout.length + stderr.length + chunk.length > 2 * 1024 * 1024) { exceeded = true; child.kill('SIGKILL'); return; }
    if (key === 'stdout') stdout += chunk; else stderr += chunk;
  };
  child.stdout.on('data', collect('stdout')); child.stderr.on('data', collect('stderr'));
  const timer = setTimeout(() => { timedOut = true; child.kill('SIGKILL'); }, timeout);
  try {
    const result = await new Promise((yes, no) => { child.once('error', no); child.once('close', (code, signal) => yes({ code, signal })); });
    return { ...result, stdout, stderr, timedOut, exceeded };
  } finally { clearTimeout(timer); }
}
async function checked(file, args, settings) {
  const result = await command(file, args, settings);
  if (result.code !== 0 || result.timedOut || result.exceeded) throw new Error('Required build or source-binding command failed (private stage output retained).');
  return result;
}
async function put(path, value) { await writeFile(path, typeof value === 'string' ? value : JSON.stringify(value, null, 2) + '\n', { mode: 0o600 }); }
export async function run(argv) {
  const selected = options(argv), output = selected['output-dir'];
  await mkdir(output, { mode: 0o700 }); // Exclusive: never reuse an earlier run.
  const report = { schemaVersion: 1, lane: 'ordinary-source-cli', status: 'FAIL', stages: [], fullLifecycle: { status: 'BLOCKED', reason: 'Authentic admitted template/native authoring and complete lifecycle dependencies are separately required.' } };
  let helper, helperClose, connectionFile, token = '', helperText = '', helperError = '', cleanupError;
  const record = (stage, status, details = {}) => report.stages.push({ stage, status, ...details });
  const home = join(output, 'home'); await mkdir(home, { mode: 0o700 });
  const env = childEnvironment(home);
  try {
    const nodeVersion = process.versions.node;
    if (!nodeVersion.startsWith('22.')) throw new Error('Node22 is required.');
    const cliRevision = (await checked('git', ['-C', root, 'rev-parse', 'HEAD'], { env })).stdout.trim();
    const coreRevision = (await checked('git', ['-C', selected['core-dir'], 'rev-parse', 'HEAD'], { env })).stdout.trim();
    if (coreRevision !== selected['core-revision']) throw new Error('Core revision does not match explicit input.');
    const hashes = {};
    const build = join(output, 'cli'); await mkdir(join(build, 'src'), { recursive: true });
    for (const file of sources) { const bytes = await readFile(join(root, file)); hashes[file] = digest(bytes); await copyFile(join(root, file), join(build, file)); }
    await put(join(build, 'package.json'), { type: 'module' });
    await symlink(await realpath(join(root, 'node_modules')), join(build, 'node_modules'), process.platform === 'win32' ? 'junction' : 'dir');
    const compiler = join(root, 'node_modules/typescript/bin/tsc');
    const compilerBytes = await readFile(join(root, 'node_modules/typescript/lib/tsc.js'));
    const buildResult = await command(process.execPath, [compiler, '--target', 'ES2022', '--module', 'NodeNext', '--moduleResolution', 'NodeNext', '--rootDir', 'src', '--outDir', 'dist', '--strict', '--esModuleInterop', '--skipLibCheck', '--types', 'node', ...sources], { cwd: build, env, timeout: 30000 });
    await put(join(output, 'cli-build.log'), buildResult.stdout + buildResult.stderr);
    if (buildResult.code !== 0 || buildResult.timedOut || buildResult.exceeded) throw new Error('CLI explicit ordinary source build failed.');
    report.identity = { cliRevision, coreRevision, nodeVersion, compilerSHA256: digest(compilerBytes), sources: hashes, cliArtifactSHA256: digest(await readFile(join(build, 'dist/index.js'))) };
    record('cli-source-build', 'PASS');
    const fixtureOutput = join(output, 'core-fixture');
    helper = spawn(process.execPath, [selected['core-fixture-helper'], '--core-dir', selected['core-dir'], '--expected-core-revision', coreRevision, '--output-dir', fixtureOutput, '--build-core'], { env, stdio: ['pipe', 'pipe', 'pipe'] });
    helperClose = new Promise((yes, no) => { helper.once('error', no); helper.once('close', (code, signal) => yes({ code, signal })); });
    let settleReady, rejectReady;
    const ready = new Promise((yes, no) => { settleReady = yes; rejectReady = no; });
    helper.stdout.on('data', bytes => {
      helperText += bytes;
      if (helperText.length > 1024 * 1024) { helper.kill('SIGKILL'); rejectReady(new Error('Fixture readiness output exceeded bound.')); return; }
      for (const line of helperText.split('\n')) { try { const value = JSON.parse(line); if (value.ready === true) settleReady(value); } catch {} }
    });
    helper.stderr.on('data', bytes => { helperError = (helperError + bytes).slice(0, 1024 * 1024); });
    let timer;
    const hello = await Promise.race([ready, helperClose.then(() => { throw new Error('Fixture exited before readiness.'); }), new Promise((_, no) => { timer = setTimeout(() => no(new Error('Fixture startup deadline exceeded.')), 180000); })]).finally(() => clearTimeout(timer));
    const expectedConnection = join(fixtureOutput, 'connection.json');
    if (resolve(hello.connectionFile ?? '') !== expectedConnection || resolve(hello.receiptFile ?? '') !== join(fixtureOutput, 'receipt.json')) throw new Error('Fixture readiness paths do not match owned fixture directory.');
    connectionFile = expectedConnection;
    if (process.platform !== 'win32' && ((await stat(connectionFile)).mode & 0o077)) throw new Error('Fixture credentials are not private.');
    const connection = JSON.parse(await readFile(connectionFile, 'utf8'));
    const url = new URL(connection.url);
    if (url.protocol !== 'http:' || url.hostname !== '127.0.0.1' || url.port === '17883' || url.pathname !== '/' || url.username || url.password || url.search || url.hash || typeof connection.token !== 'string' || !connection.token) throw new Error('Fixture connection must be private owned loopback with ephemeral credential.');
    token = connection.token;
    const initialReceipt = JSON.parse(await readFile(hello.receiptFile, 'utf8'));
    if (initialReceipt.coreRevision !== coreRevision) throw new Error('Fixture receipt source revision mismatch.');
    record('core-fixture-ready', 'PASS', { coreArtifactSHA256: initialReceipt.coreArtifactSHA256 });
    const invocations = [ ['help', ['--help']], ['version', ['--version']], ['status', ['instance', 'status', '--json']], ['inspect', ['instance', 'inspect', '--json']], ['services', ['service', 'list', '--json']] ];
    for (const [name, args] of invocations) {
      const result = await command(process.execPath, [join(build, 'dist/index.js'), '--core-url', url.origin, ...args], { cwd: build, env: childEnvironment(home, { SERVICE_LASSO_CORE_TOKEN: token }) });
      if ((result.stdout + result.stderr).includes(token)) throw new Error('CLI output contained fixture credential.');
      await put(join(output, `${name}.stdout`), result.stdout); await put(join(output, `${name}.stderr`), result.stderr);
      if (result.code !== 0 || result.stderr || result.timedOut || result.exceeded) throw new Error(`CLI ${name} command failed.`);
      if (!['help', 'version'].includes(name)) JSON.parse(result.stdout);
      record(name, 'PASS', { exitCode: result.code, stdoutSHA256: digest(result.stdout) });
    }
    report.status = 'PASS';
  } catch (error) {
    report.status = report.identity ? 'FAIL' : 'BLOCKED';
    record('runner', report.status, { reason: error instanceof Error ? error.message.replaceAll(token || '\0', '[REDACTED]') : 'Runner failed.' });
  } finally {
    if (helper) {
      let timer;
      helper.stdin.on('error', () => {}); helper.stdin.end('stop\n');
      try {
        const closed = await Promise.race([helperClose, new Promise((_, no) => { timer = setTimeout(() => { helper.kill('SIGKILL'); no(new Error('Fixture cleanup deadline exceeded.')); }, 15000); })]);
        if (closed.code !== 0) throw new Error('Fixture cleanup exited unsuccessfully.');
        if (connectionFile) { try { await access(connectionFile); throw new Error('Fixture credential remained after shutdown.'); } catch (error) { if (error.code !== 'ENOENT') throw error; } }
        record('owned-fixture-cleanup', 'PASS');
      } catch { cleanupError = true; record('owned-fixture-cleanup', 'FAIL'); report.status = 'FAIL'; }
      finally { clearTimeout(timer); if (cleanupError) await helperClose.catch(() => {}); }
    }
    await put(join(output, 'result.json'), report);
  }
  return report;
}
if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  run(process.argv.slice(2)).then(report => { process.stdout.write(JSON.stringify(report) + '\n'); process.exitCode = report.status === 'PASS' ? 0 : 1; }, () => { process.stderr.write('E2E runner could not initialize; use explicit fresh output and Core inputs.\n'); process.exitCode = 1; });
}
