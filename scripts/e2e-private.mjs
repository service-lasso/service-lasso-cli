// Client adapter only: platform privacy policy is owned by the selected Core.
import { createHash } from 'node:crypto';
import { open, readFile, lstat, unlink } from 'node:fs/promises';
import { dirname, join } from 'node:path';
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
  for (const name of ['createPrivateDirectory', 'verifyPrivateDirectory', 'verifyPrivateFile', 'writePrivateJsonFile']) if (typeof api[name] !== 'function') throw new Error('Selected Core private setup API is unavailable.');
  const writeEvidence = async (file, value) => {
    await api.verifyPrivateDirectory(dirname(file));
    let handle, identity;
    try {
      handle = await open(file, 'wx', 0o600); identity = await handle.stat();
      await api.verifyPrivateFile(file); // Empty-file privacy precedes any raw data.
      await handle.writeFile(typeof value === 'string' ? value : JSON.stringify(value, null, 2) + '\n'); await handle.sync();
      const current = await lstat(file);
      if (current.isSymbolicLink() || current.dev !== identity.dev || current.ino !== identity.ino) throw new Error('Private evidence file identity changed.');
      await api.verifyPrivateFile(file);
    } catch (error) {
      if (identity) { const current = await lstat(file).catch(() => null); if (current?.dev === identity.dev && current?.ino === identity.ino) await unlink(file).catch(() => {}); }
      throw error;
    } finally { await handle?.close(); }
  };
  return { api, files, writeEvidence };
}
