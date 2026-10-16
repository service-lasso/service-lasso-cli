import { runScopedCli } from './cli-platform-scope.mjs';
import { mkdir, writeFile } from 'node:fs/promises';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { readSourceScope, scopedArguments } from './cli-platform-scope.mjs';
import { NATIVE_TARGETS, expectedAssets, sha256, identity, checksumBytes, parseStrictJson, readHeldFile,
  fail, validateManifest, verifyPortableRecord, verifyScopedNative, verifyCandidateDirectory } from './protected-scoped-candidate-lib.mjs';

export async function assembleScopedCandidate({ output, portable, native, version, sourceSha }) {
  const scope = await readSourceScope(), tag = identity(version, sourceSha);
  if (!(native instanceof Map) || native.size !== 2 ||
      [...native.keys()].sort().join(',') !== 'linux-x64,win32-x64') fail('exactly the two scoped native directories are required.');
  const held = new Map(), expected = expectedAssets(version);
  for (const asset of expected) {
    const directory = asset.kind.startsWith('portable') ? portable : native.get(asset.target);
    const name = asset.kind === 'provenance' ? 'provenance.json' : asset.name;
    held.set(asset.name, await readHeldFile(resolve(directory), name));
  }
  verifyPortableRecord(parseStrictJson(held.get('candidate.json'), 'scoped portable record'),
    held.get(`service-lassoctl-${version}.tgz`), version, sourceSha);
  for (const target of NATIVE_TARGETS) verifyScopedNative(held.get(`service-lassoctl-${version}-${target.id}.tar.gz`),
    held.get(`provenance-${target.id}.json`), target, sourceSha, version);
  const assets = expected.map(asset => ({ ...asset, sha256: sha256(held.get(asset.name)), size: held.get(asset.name).length }));
  const manifest = { schemaVersion: 2, assets, candidateTag: tag,
    checksums: { algorithm: 'sha256', entries: ['development-candidate.json', ...assets.map(asset => asset.name)].sort(), file: 'SHA256SUMS.txt' },
    source: { commit: sourceSha, repository: 'service-lasso/service-lasso-cli' }, version, scope };
  validateManifest(manifest, version, sourceSha);
  const manifestBytes = Buffer.from(`${JSON.stringify(manifest, null, 2)}\n`);
  held.set('development-candidate.json', manifestBytes); held.set('SHA256SUMS.txt', checksumBytes(manifestBytes, assets));
  // No erase/recovery/overwrite: validate the complete input before creating a fresh owned destination.
  await mkdir(output, { recursive: false });
  for (const [name, bytes] of held) await writeFile(join(output, name), bytes, { flag: 'wx' });
  const verified = await verifyCandidateDirectory(output, version, sourceSha);
  for (const [name, bytes] of held) if (!verified.held.get(name)?.equals(bytes)) fail('assembled held bytes changed.');
  return { candidateTag: tag, sourceSha, assetCount: assets.length, fileCount: held.size };
}
if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  await runScopedCli(async () => {
  const args = scopedArguments(process.argv.slice(2), ['--output', '--portable', '--version', '--source-sha', '--native'], ['--native']);
  const native = new Map();
  for (const value of args.all('--native')) {
    const equals = value.indexOf('='), target = value.slice(0, equals), directory = value.slice(equals + 1);
    if (equals < 1 || !directory || native.has(target)) fail('duplicate or malformed native directory.');
    native.set(target, directory);
  }
  const result = await assembleScopedCandidate({ output: resolve(args.one('--output')), portable: resolve(args.one('--portable')),
    native, version: args.one('--version'), sourceSha: args.one('--source-sha') });
  process.stdout.write(`${JSON.stringify(result)}\n`);
  });
}
