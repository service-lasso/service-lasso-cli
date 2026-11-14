// Client adapter only: platform privacy policy is owned by the selected Core.
import { createHash } from 'node:crypto';
import { readFile } from 'node:fs/promises';
import { join } from 'node:path';
import { pathToFileURL } from 'node:url';
const hash = bytes => createHash('sha256').update(bytes).digest('hex');
export function publicPrivacyProof(proof) {
  if (proof?.verified !== true || typeof proof.platform !== 'string' || typeof proof.policy !== 'string') throw new Error('Private setup did not return verified policy evidence.');
  return { platform: proof.platform, policy: proof.policy, verified: true, proofSHA256: hash(JSON.stringify(proof)) };
}
export async function loadPrivateSetup({ coreDir, revision, environment, execute }) {
  const git = async args => {
    const result = await execute('git', ['-C', coreDir, ...args], { env: environment, timeout: 10000 });
    if (result.code !== 0 || result.timedOut || result.exceeded) throw new Error('Private setup Core source binding failed.');
    return result.stdout;
  };
  if ((await git(['rev-parse', 'HEAD'])).trim() !== revision || (await git(['status', '--porcelain', '--untracked-files=all'])).trim()) throw new Error('Exact clean Core source required before private setup import.');
  const files = {};
  for (const file of ['scripts/operator-e2e-fixture-private.mjs', 'scripts/operator-e2e-fixture-private-dir.ps1']) {
    const expected = await git(['show', `${revision}:${file}`]), actual = await readFile(join(coreDir, file));
    if (hash(actual) !== hash(expected)) throw new Error('Core private setup bytes differ from selected source.');
    files[file] = hash(actual);
  }
  const api = await import(pathToFileURL(join(coreDir, 'scripts/operator-e2e-fixture-private.mjs')).href);
  for (const name of ['createPrivateDirectory', 'verifyPrivateDirectory', 'verifyPrivateFile', 'writePrivateFile', 'writePrivateJsonFile']) if (typeof api[name] !== 'function') throw new Error('Selected Core private setup API is unavailable.');
  const writeEvidence = (file, value) => api.writePrivateFile(file, typeof value === 'string' ? value : JSON.stringify(value, null, 2) + '\n');
  return { api, files, writeEvidence };
}
