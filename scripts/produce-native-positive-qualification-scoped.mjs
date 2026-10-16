import { runScopedCli } from './cli-platform-scope.mjs';
import { pathToFileURL } from 'node:url';
import { resolve } from 'node:path';
import { readSourceScope, scopedArguments } from './cli-platform-scope.mjs';
import { identity, fail } from './protected-scoped-candidate-lib.mjs';

/* Fixed real-producer dependency, deliberately unavailable. This is not an
 * opaque closed.json importer or a receipt/issuer factory. The genuine native
 * source/principal/reserve/child-local route must produce the existing separate
 * positive smoke/route/archive originals before the unchanged close consumer
 * can run. Negative controlled-route evidence cannot enter that directory.
 * No success record, child, receipt or output directory is created here. */
export async function producePositiveNativeQualification({ directory, receiptDirectory, version, sourceSha }) {
  await readSourceScope(); identity(version, sourceSha);
  if (!['win32', 'linux'].includes(process.platform) || process.arch !== 'x64' ||
      typeof directory !== 'string' || typeof receiptDirectory !== 'string' || resolve(directory) === resolve(receiptDirectory))
    fail('positive native producer requires distinct selected-host original custody.');
  fail('genuine positive native source/principal/reserve route producer remains unavailable; no positive receipt emitted.');
}
if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  await runScopedCli(async () => {
  const args = scopedArguments(process.argv.slice(2), ['--directory', '--receipt-directory', '--version', '--source-sha']);
  await producePositiveNativeQualification({ directory: resolve(args.one('--directory')), receiptDirectory: resolve(args.one('--receipt-directory')),
    version: args.one('--version'), sourceSha: args.one('--source-sha') });
  });
}
