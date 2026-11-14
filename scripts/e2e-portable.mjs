import { createHash } from 'node:crypto';
import { createRequire } from 'node:module';
import { cp, mkdir, readFile, realpath, writeFile } from 'node:fs/promises';
import { basename, delimiter, dirname, isAbsolute, join, relative, resolve } from 'node:path';
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
const save = (path, value) => writeFile(path, JSON.stringify(value, null, 2) + '\n', { mode: 0o600 });
export async function npmTool(environment) {
  const possibilities = [process.env.npm_execpath, join(dirname(process.execPath), 'node_modules/npm/bin/npm-cli.js'), ...(environment.PATH ?? '').split(delimiter).map(dir => join(dir, 'npm'))].filter(Boolean);
  for (const path of possibilities) {
    try {
      const script = await realpath(path);
      if (basename(script) !== 'npm-cli.js') continue;
      const bytes = await readFile(script), pkg = JSON.parse(await readFile(join(dirname(dirname(script)), 'package.json'), 'utf8'));
      if (pkg.name === 'npm') return { script, version: pkg.version, sha256: hash(bytes) };
    } catch {}
  }
  throw new Error('Exact npm-cli.js tool could not be resolved.');
}
async function npmRun(tool, args, cwd, environment, execute, log, writeEvidence = (file, value) => writeFile(file, value, { mode: 0o600 })) {
  const result = await execute(process.execPath, [tool.script, ...args], { cwd, env: environment, timeout: 60000 });
  await writeEvidence(log, result.stdout + result.stderr);
  if (result.code !== 0 || result.timedOut || result.exceeded) throw new Error('Portable npm stage failed; private log retained.');
  return result.stdout;
}
export async function installPortable({ archive, expectedHash, consumer, environment, execute, tool, log, createDirectory = path => mkdir(path, { mode: 0o700 }), writeEvidence }) {
  const bytes = await readFile(archive);
  if (!/^[a-f0-9]{64}$/.test(expectedHash) || hash(bytes) !== expectedHash) throw new Error('Portable archive digest mismatch before installation.');
  await createDirectory(consumer);
  await save(join(consumer, 'package.json'), { name: 'portable-e2e-consumer', version: '1.0.0', private: true, type: 'module' });
  await npmRun(tool, ['install', '--ignore-scripts', '--no-audit', '--no-fund', archive], consumer, environment, execute, log, writeEvidence);
  if (hash(await readFile(archive)) !== expectedHash) throw new Error('Portable archive bytes changed during installation.');
  return consumer;
}
export async function preparePortable({ build, output, root, revision, environment, execute, createDirectory = path => mkdir(path, { mode: 0o700 }), writeEvidence }) {
  const manifest = JSON.parse(await readFile(join(root, 'package.json'), 'utf8'));
  if (manifest.dependencies?.commander !== '15.0.0' || manifest.bin?.['service-lassoctl'] !== './dist/index.js') throw new Error('Unexpected portable CLI package contract.');
  const version = `${manifest.version}-dev.${revision.slice(0, 7)}`, tool = await npmTool(environment);
  const stage = join(output, 'portable-stage'), archives = join(output, 'portable-archive'), consumer = join(output, 'portable-consumer');
  await createDirectory(stage); await createDirectory(archives);
  await cp(join(build, 'dist'), join(stage, 'dist'), { recursive: true });
  await cp(join(root, 'README.md'), join(stage, 'README.md'));
  await save(join(stage, 'package.json'), { name: manifest.name, version, private: true, license: manifest.license, description: manifest.description, type: 'module', bin: manifest.bin, engines: manifest.engines, dependencies: { commander: '15.0.0' }, files: ['dist', 'README.md'] });
  const entry = join(stage, 'dist/index.js'), compiled = await readFile(entry, 'utf8');
  const marker = `.version(process.env.SERVICE_LASSO_CANDIDATE_VERSION ?? "${manifest.version}")`;
  if (compiled.split(marker).length !== 2) throw new Error('Portable version transform requires exactly one source marker.');
  await writeFile(entry, compiled.replace(marker, `.version("${version}")`), { mode: 0o600 });
  const packed = JSON.parse(await npmRun(tool, ['pack', '--ignore-scripts', '--json', '--pack-destination', archives], stage, environment, execute, join(output, 'portable-pack.log'), writeEvidence));
  if (!Array.isArray(packed) || packed.length !== 1 || basename(packed[0].filename ?? '') !== packed[0].filename || !packed[0].filename.endsWith('.tgz')) throw new Error('Portable pack must produce exactly one local archive.');
  const expectedFiles = ['README.md', 'package.json', ...['index', 'config', 'errors', 'core-client', 'scaffold', 'template', 'completion'].map(name => `dist/${name}.js`)].sort();
  if (JSON.stringify(packed[0].files?.map(file => file.path).sort()) !== JSON.stringify(expectedFiles)) throw new Error('Portable archive inventory contains missing or unexpected files.');
  const archive = join(archives, packed[0].filename), sha256 = hash(await readFile(archive));
  await installPortable({ archive, expectedHash: sha256, consumer, environment, execute, tool, log: join(output, 'portable-install.log'), createDirectory, writeEvidence });
  const installed = join(consumer, 'node_modules', ...manifest.name.split('/'));
  const installedManifest = JSON.parse(await readFile(join(installed, 'package.json'), 'utf8'));
  if (installedManifest.name !== manifest.name || installedManifest.version !== version || installedManifest.dependencies?.commander !== '15.0.0' || installedManifest.bin?.['service-lassoctl'] !== './dist/index.js') throw new Error('Installed portable package metadata differs from packed identity.');
  const executable = await realpath(resolve(installed, installedManifest.bin['service-lassoctl']));
  const commanderEntry = await realpath(createRequire(join(installed, 'package.json')).resolve('commander'));
  for (const path of [executable, commanderEntry]) if (isAbsolute(relative(consumer, path)) || relative(consumer, path).startsWith('..')) throw new Error('Portable consumer resolved an executable or dependency outside its own installation.');
  const installedCommander = JSON.parse(await readFile(join(consumer, 'node_modules/commander/package.json'), 'utf8'));
  if (installedCommander.version !== '15.0.0' || hash(await readFile(executable)) !== hash(await readFile(entry))) throw new Error('Portable installed byte or dependency identity mismatch.');
  const installedInventory = {};
  for (const file of expectedFiles) {
    const stagedSHA256 = hash(await readFile(join(stage, file))), installedSHA256 = hash(await readFile(join(installed, file)));
    if (stagedSHA256 !== installedSHA256) throw new Error('Installed portable inventory bytes differ from staged archive input.');
    installedInventory[file] = { stagedSHA256, installedSHA256 };
  }
  const identity = { classification: 'portable-node-archive', standaloneNative: false, sourceRevision: revision, version, archiveSHA256: sha256, archiveBytes: (await readFile(archive)).length, inventory: expectedFiles, installedInventory, npm: { version: tool.version, cliSHA256: tool.sha256 }, installedPackage: { name: installedManifest.name, version, bin: installedManifest.bin, entrySHA256: hash(await readFile(executable)), commanderVersion: installedCommander.version, commanderEntrySHA256: hash(await readFile(commanderEntry)) }, consumedArchiveSHA256: hash(await readFile(archive)), consumerDisposition: 'retained-private-evidence-no-live-process' };
  if (writeEvidence) await writeEvidence(join(output, 'portable-identity.json'), identity); else await save(join(output, 'portable-identity.json'), identity);
  return { executable, version, identity };
}
