#!/usr/bin/env node
import { spawn } from 'node:child_process';
import { createHash } from 'node:crypto';
import { connect } from 'node:net';
import { preparePortable, npmTool } from './e2e-portable.mjs';
import { loadPrivateSetup, publicPrivacyProof } from './e2e-private.mjs';
import { copyFile, readFile, realpath, symlink, access } from 'node:fs/promises';
import { basename, delimiter, dirname, isAbsolute, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const sources = ['index', 'config', 'errors', 'core-client', 'scaffold', 'template', 'completion'].map(x => `src/${x}.ts`);
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
export function options(argv) {
  const result = {};
  const names = new Set(['core-dir', 'core-revision', 'core-fixture-helper', 'output-dir', 'suite']);
  for (let i = 0; i < argv.length; i += 2) {
    const name = argv[i]?.replace(/^--/, '');
    if (!argv[i]?.startsWith('--') || !names.has(name) || !argv[i + 1] || argv[i + 1].startsWith('--') || name in result) throw new Error('Expected unique --core-dir, --core-revision, --output-dir and optional --core-fixture-helper values.');
    result[name] = argv[i + 1];
  }
  if (!result['core-dir'] || !/^[a-f0-9]{40}$/.test(result['core-revision'] ?? '') || !isAbsolute(result['output-dir'] ?? '')) throw new Error('Explicit Core directory, full lowercase revision and absolute fresh output directory required.');
  result.suite ??= 'source';
  if (!['source', 'portable'].includes(result.suite)) throw new Error('Suite must be source or portable.');
  result['core-dir'] = resolve(result['core-dir']);
  result['core-fixture-helper'] = resolve(result['core-fixture-helper'] ?? join(result['core-dir'], 'scripts/operator-e2e-fixture.mjs'));
  return result;
}
export function validateReceipt(receipt, revision, final = false, expectedTools) {
  if (receipt.schemaVersion !== 1 || receipt.coreRevision !== revision || receipt.cleanSource !== true || receipt.classification !== 'source-built-ordinary-smoke'
    || !/^[a-f0-9]{40}$/.test(receipt.coreTree ?? '') || !['coreArtifactSHA256', 'sourceInputSHA256', 'artifactInventorySHA256'].every(key => /^[a-f0-9]{64}$/.test(receipt[key] ?? ''))
    || !receipt.tools || !['node', 'npm', 'typescript'].every(key => typeof receipt.tools[key] === 'string' && receipt.tools[key].length > 0) || !/^[a-f0-9]{64}$/.test(receipt.tools.compilerSHA256 ?? '') || receipt.initializationAPI !== true
    || receipt.build?.exitCode !== 0 || receipt.build?.signal != null || JSON.stringify(receipt.build?.command) !== JSON.stringify(expectedTools ? [process.execPath, expectedTools.script, 'run', 'build'] : ['npm', 'run', 'build'])) throw new Error('Fixture receipt lacks qualified clean fresh source-build binding.');
  if (expectedTools && (receipt.tools.npmCLI?.sha256 !== expectedTools.sha256 || receipt.tools.npmCLI?.path !== expectedTools.script || receipt.tools.npmCLI?.packageVersion !== expectedTools.version || receipt.tools.node !== process.version || receipt.tools.nodeSHA256 !== expectedTools.nodeSHA256)) throw new Error('Fixture literal npm tool binding mismatch.');
  if (final ? receipt.stage !== 'closed' || receipt.status !== 'passed' || !receipt.cleanup || !['serverClosed', 'portClosed', 'credentialRemoved'].every(key => receipt.cleanup[key] === true) || receipt.cleanup.status !== 'complete' : receipt.stage !== 'ready' || receipt.status !== 'running') throw new Error('Fixture receipt stage or cleanup evidence is invalid.');
}
export function portClosed(url) {
  return new Promise(resolveClosed => {
    const address = new URL(url), socket = connect({ host: address.hostname, port: Number(address.port) });
    let done = false;
    const finish = value => { if (!done) { done = true; socket.destroy(); resolveClosed(value); } };
    socket.setTimeout(1500, () => finish(false));
    socket.once('connect', () => finish(false));
    socket.once('error', error => finish(error.code === 'ECONNREFUSED'));
  });
}
function childEnvironment(home, extra = {}) {
  return { PATH: dirname(process.execPath) + delimiter + (process.env.PATH ?? ''), ...(process.env.SystemRoot ? { SystemRoot: process.env.SystemRoot } : {}), HOME: home, USERPROFILE: home, ...(process.platform === 'win32' ? { HOMEDRIVE: home.slice(0, 2), HOMEPATH: home.slice(2), WINDIR: process.env.SystemRoot, ComSpec: join(process.env.SystemRoot, 'System32', 'cmd.exe') } : {}), APPDATA: join(home, 'appdata'), LOCALAPPDATA: join(home, 'localappdata'), XDG_CONFIG_HOME: join(home, 'config'), XDG_CACHE_HOME: join(home, 'cache'), TMPDIR: join(home, 'tmp'), TEMP: join(home, 'tmp'), TMP: join(home, 'tmp'), npm_config_cache: join(home, 'npm-cache'), LANG: 'C.UTF-8', ...extra };
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
export async function run(argv, deadlines = {}) {
  const selected = options(argv);
  selected['core-dir'] = await realpath(selected['core-dir']);
  const output = join(await realpath(dirname(selected['output-dir'])), basename(selected['output-dir']));
  const bootstrapEnv = { PATH: dirname(process.execPath) + delimiter + (process.env.PATH ?? ''), ...(process.env.SystemRoot ? { SystemRoot: process.env.SystemRoot } : {}), GIT_CONFIG_NOSYSTEM: '1', GIT_CONFIG_GLOBAL: process.platform === 'win32' ? 'NUL' : '/dev/null', GIT_TERMINAL_PROMPT: '0' };
  const privacy = await loadPrivateSetup({ coreDir: selected['core-dir'], revision: selected['core-revision'], environment: bootstrapEnv, execute: command });
  const rootProof = await privacy.api.createPrivateDirectory(output);
  await privacy.api.verifyPrivateDirectory(output);
  const put = privacy.writeEvidence;
  await privacy.api.writePrivateJsonFile(join(output, 'privacy-proof.private.json'), rootProof);
  const report = { schemaVersion: 1, lane: selected.suite === 'portable' ? 'portable-node-archive-cli' : 'ordinary-source-cli', status: 'FAIL', privacy: publicPrivacyProof(rootProof), platform: process.platform, applicability: process.platform === 'darwin' ? 'exploratory-deferred-not-GA' : 'ordinary-not-native-qualified', stages: [], fullLifecycle: { status: 'BLOCKED', reason: 'Authentic admitted template/native authoring and complete lifecycle dependencies are separately required.' } };
  let helper, helperClose, connectionFile, receiptFile, fixtureUrl, token = '', helperText = '', helperError = '', cleanupError;
  const record = (stage, status, details = {}) => report.stages.push({ stage, status, ...details });
  const home = join(output, 'home'); await privacy.api.createPrivateDirectory(home);
  for (const name of ['appdata', 'localappdata', 'config', 'cache', 'tmp', 'npm-cache']) await privacy.api.createPrivateDirectory(join(home, name));
  const env = childEnvironment(home);
  try {
    const nodeVersion = process.versions.node;
    if (!nodeVersion.startsWith('22.')) throw new Error('Node22 is required.');
    const cliRevision = (await checked('git', ['-C', root, 'rev-parse', 'HEAD'], { env })).stdout.trim();
    const clean = (await checked('git', ['-C', root, 'status', '--porcelain', '--untracked-files=no'], { env })).stdout;
    if (clean.trim()) throw new Error('CLI checkout tracked source is dirty.');
    const coreRevision = (await checked('git', ['-C', selected['core-dir'], 'rev-parse', 'HEAD'], { env })).stdout.trim();
    if (coreRevision !== selected['core-revision']) throw new Error('Core revision does not match explicit input.');
    const hashes = {};
    const build = join(output, 'cli'); await privacy.api.createPrivateDirectory(build); await privacy.api.createPrivateDirectory(join(build, 'src'));
    for (const file of sources) { const bytes = await readFile(join(root, file)); hashes[file] = digest(bytes); await copyFile(join(root, file), join(build, file)); }
    await put(join(build, 'package.json'), { type: 'module' });
    await symlink(await realpath(join(root, 'node_modules')), join(build, 'node_modules'), process.platform === 'win32' ? 'junction' : 'dir');
    const compiler = join(root, 'node_modules/typescript/bin/tsc');
    const compilerBytes = await readFile(join(root, 'node_modules/typescript/lib/tsc.js'));
    const buildResult = await command(process.execPath, [compiler, '--target', 'ES2022', '--module', 'NodeNext', '--moduleResolution', 'NodeNext', '--rootDir', 'src', '--outDir', 'dist', '--strict', '--esModuleInterop', '--skipLibCheck', '--types', 'node', ...sources], { cwd: build, env, timeout: 30000 });
    await put(join(output, 'cli-build.log'), buildResult.stdout + buildResult.stderr);
    if (buildResult.code !== 0 || buildResult.timedOut || buildResult.exceeded) throw new Error('CLI explicit ordinary source build failed.');
    const packages = {};
    for (const name of ['typescript', 'commander', '@types/node']) packages[name] = JSON.parse(await readFile(join(root, 'node_modules', name, 'package.json'), 'utf8')).version;
    report.identity = { privateSetupSources: privacy.files, cliRevision, coreRevision, nodeVersion, packages, packageLockSHA256: digest(await readFile(join(root, 'package-lock.json'))), compilerSHA256: digest(compilerBytes), sources: hashes, cliArtifactSHA256: digest(await readFile(join(build, 'dist/index.js'))) };
    record('cli-source-build', 'PASS');
    let cliExecutable = join(build, 'dist/index.js'), portableVersion;
    if (selected.suite === 'portable') {
      const portable = await preparePortable({ build, output, root, revision: cliRevision, environment: env, execute: command, createDirectory: privacy.api.createPrivateDirectory, writeEvidence: put });
      cliExecutable = portable.executable; portableVersion = portable.version; report.distribution = portable.identity;
      record('portable-archive-install', 'PASS', { archiveSHA256: portable.identity.archiveSHA256 });
    }
    const fixtureOutput = join(output, 'core-fixture');
    connectionFile = join(fixtureOutput, 'connection.json'); receiptFile = join(fixtureOutput, 'receipt.json');
    const fixtureArgs = [selected['core-fixture-helper'], '--core-dir', selected['core-dir'], '--expected-core-revision', coreRevision, '--output-dir', fixtureOutput, '--build-core'];
    const fixtureNpm = process.platform === 'win32' ? { ...await npmTool(env), nodeSHA256: digest(await readFile(process.execPath)) } : undefined;
    if (fixtureNpm) fixtureArgs.push('--npm-cli', fixtureNpm.script);
    helper = spawn(process.execPath, fixtureArgs, { env, stdio: ['pipe', 'pipe', 'pipe'] });
    helperClose = new Promise((yes, no) => { helper.once('error', no); helper.once('close', (code, signal) => yes({ code, signal })); });
    let settleReady, rejectReady;
    const ready = new Promise((yes, no) => { settleReady = yes; rejectReady = no; });
    helper.stdout.on('data', bytes => {
      helperText += bytes;
      if (helperText.length > 1024 * 1024) { helper.kill('SIGTERM'); rejectReady(new Error('Fixture readiness output exceeded bound.')); return; }
      for (const line of helperText.split('\n')) { try { const value = JSON.parse(line); if (value.ready === true) settleReady(value); } catch {} }
    });
    helper.stderr.on('data', bytes => { helperError = (helperError + bytes).slice(0, 1024 * 1024); });
    let timer;
    const hello = await Promise.race([ready, helperClose.then(() => { throw new Error('Fixture exited before readiness.'); }), new Promise((_, no) => { timer = setTimeout(() => no(new Error('Fixture startup deadline exceeded.')), deadlines.startup ?? 180000); })]).finally(() => clearTimeout(timer));
    const expectedConnection = join(fixtureOutput, 'connection.json');
    if (resolve(hello.connectionFile ?? '') !== expectedConnection || resolve(hello.receiptFile ?? '') !== join(fixtureOutput, 'receipt.json')) throw new Error('Fixture readiness paths do not match owned fixture directory.');
    connectionFile = expectedConnection; receiptFile = hello.receiptFile;
    await privacy.api.verifyPrivateDirectory(fixtureOutput); await privacy.api.verifyPrivateFile(connectionFile);
    const connection = JSON.parse(await readFile(connectionFile, 'utf8'));
    const url = new URL(connection.url);
    if (url.protocol !== 'http:' || url.hostname !== '127.0.0.1' || url.port === '17883' || url.pathname !== '/' || url.username || url.password || url.search || url.hash || typeof connection.token !== 'string' || !connection.token) throw new Error('Fixture connection must be private owned loopback with ephemeral credential.');
    token = connection.token; fixtureUrl = url.origin;
    const initialReceipt = JSON.parse(await readFile(hello.receiptFile, 'utf8'));
    validateReceipt(initialReceipt, coreRevision, false, fixtureNpm);
    if (initialReceipt.url !== url.origin) throw new Error('Fixture receipt URL mismatch.');
    for (const [file, field] of [['source-inputs.json', 'sourceInputSHA256'], ['artifact-inventory.json', 'artifactInventorySHA256']]) {
      if (digest(JSON.stringify(JSON.parse(await readFile(join(fixtureOutput, file), 'utf8')))) !== initialReceipt[field]) throw new Error('Fixture inventory readback digest mismatch.');
    }
    if (digest(await readFile(join(selected['core-dir'], 'dist/server/index.js'))) !== initialReceipt.coreArtifactSHA256) throw new Error('Fixture Core entry artifact readback mismatch.');
    record('core-fixture-ready', 'PASS', { coreArtifactSHA256: initialReceipt.coreArtifactSHA256, sourceInputSHA256: initialReceipt.sourceInputSHA256, artifactInventorySHA256: initialReceipt.artifactInventorySHA256 });
    // Separate backend transport-contract probe: these are not additional CLI commands.
    const security = async credential => {
      const response = await fetch(new URL('/api/runtime/security', url), { headers: { authorization: `Bearer ${credential}` }, redirect: 'error', signal: AbortSignal.timeout(5000) });
      if (!response.ok) throw new Error('Backend bearer identity probe did not return a supported security record.');
      return response.json();
    };
    const positive = await security(token), negative = await security('invalid-e2e-credential-not-an-authority');
    if (positive.auth?.actor?.kind !== 'local-token' || positive.auth?.actor?.authenticated !== true || positive.auth?.policy?.localTokenConfigured !== true) throw new Error('Backend did not identify genuine CLI Bearer transport as local-token.');
    if (negative.auth?.actor?.kind === 'local-token') throw new Error('Backend accepted invalid Bearer credential as local-token.');
    record('backend-bearer-identity-contract', 'PASS', { positiveActor: positive.auth.actor.kind, negativeActor: negative.auth?.actor?.kind ?? null, completePermissionsProof: false, actualCLICommand: false });
    const invocations = [ ['help', ['--help']], ['version', ['--version']], ['status', ['instance', 'status', '--json']], ['inspect', ['instance', 'inspect', '--json']], ['services', ['service', 'list', '--json']] ];
    for (const [name, args] of invocations) {
      const result = await command(process.execPath, [cliExecutable, '--core-url', url.origin, ...args], { cwd: build, env: childEnvironment(home, { SERVICE_LASSO_CORE_TOKEN: token }) });
      if ((result.stdout + result.stderr).includes(token)) throw new Error('CLI output contained fixture credential.');
      await put(join(output, `${name}.stdout`), result.stdout); await put(join(output, `${name}.stderr`), result.stderr);
      if (result.code !== 0 || result.stderr || result.timedOut || result.exceeded) throw new Error(`CLI ${name} command failed.`);
      if (name === 'version' && portableVersion && result.stdout.trim() !== portableVersion) throw new Error('Installed portable CLI reports a different version.');
      if (!['help', 'version'].includes(name)) JSON.parse(result.stdout);
      record(name, 'PASS', { exitCode: result.code, stdoutSHA256: digest(result.stdout) });
    }
    report.status = 'PASS';
  } catch (error) {
    report.status = report.identity ? 'FAIL' : 'BLOCKED';
    record('runner', report.status, { reason: error instanceof SyntaxError ? 'Invalid JSON in required fixture or CLI record.' : error instanceof Error ? error.message.replaceAll(token || '\0', '[REDACTED]') : 'Runner failed.' });
  } finally {
    if (helper) {
      let timer;
      helper.stdin.on('error', () => {}); helper.stdin.end('stop\n');
      try {
        const closed = await Promise.race([helperClose, new Promise((_, no) => { timer = setTimeout(() => { helper.kill('SIGTERM'); no(new Error('Fixture cleanup deadline exceeded; controller owns child recovery.')); }, deadlines.cleanup ?? 30000); })]);
        if (closed.code !== 0) throw new Error('Fixture cleanup exited unsuccessfully.');
        if (connectionFile) { try { await access(connectionFile); throw new Error('Fixture credential remained after shutdown.'); } catch (error) { if (error.code !== 'ENOENT') throw error; } }
        if (!receiptFile || !fixtureUrl) throw new Error('No authenticated fixture readiness to verify cleanup.');
        const finalReceipt = JSON.parse(await readFile(receiptFile, 'utf8'));
        validateReceipt(finalReceipt, selected['core-revision'], true, process.platform === 'win32' ? { ...await npmTool(env), nodeSHA256: digest(await readFile(process.execPath)) } : undefined);
        if (!(await portClosed(fixtureUrl))) throw new Error('Owned fixture port remains open or unknown.');
        record('owned-fixture-cleanup', 'PASS');
      } catch { cleanupError = true; record('owned-fixture-cleanup', 'FAIL'); report.status = 'FAIL'; }
      finally { clearTimeout(timer); if (cleanupError && helper.exitCode === null && helper.signalCode === null) { let recovery; try { await Promise.race([helperClose.catch(() => {}), new Promise(yes => { recovery = setTimeout(yes, 5000); })]); } finally { clearTimeout(recovery); } } }
    }
    await privacy.api.verifyPrivateDirectory(output);
    await put(join(output, 'result.json'), report);
  }
  return report;
}
if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  run(process.argv.slice(2)).then(report => { process.stdout.write(JSON.stringify(report) + '\n'); process.exitCode = report.status === 'PASS' ? 0 : 1; }, () => { process.stderr.write('E2E runner could not initialize; use explicit fresh output and Core inputs.\n'); process.exitCode = 1; });
}
