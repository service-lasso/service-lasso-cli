import { runScopedCli } from './cli-platform-scope.mjs';
import { resolve } from 'node:path';
import { verifyCandidateDirectory } from './protected-scoped-candidate-lib.mjs';
import { scopedArguments } from './cli-platform-scope.mjs';
await runScopedCli(async () => {
const args = scopedArguments(process.argv.slice(2), ['--directory', '--version', '--source-sha']);
const { manifest, held } = await verifyCandidateDirectory(resolve(args.one('--directory')), args.one('--version'), args.one('--source-sha'));
process.stdout.write(`${JSON.stringify({ candidateTag: manifest.candidateTag, schemaVersion: 2, assetCount: manifest.assets.length, fileCount: held.size })}\n`);

});
