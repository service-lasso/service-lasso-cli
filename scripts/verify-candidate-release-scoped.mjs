import { runScopedCli } from './cli-platform-scope.mjs';
import { resolve } from 'node:path';
import { verifyCandidateDirectory, fail } from './protected-scoped-candidate-lib.mjs';
import { scopedArguments } from './cli-platform-scope.mjs';
await runScopedCli(async () => {
const args = scopedArguments(process.argv.slice(2), ['--expected', '--actual', '--version', '--source-sha']);
const expected = await verifyCandidateDirectory(resolve(args.one('--expected')), args.one('--version'), args.one('--source-sha'));
const actual = await verifyCandidateDirectory(resolve(args.one('--actual')), args.one('--version'), args.one('--source-sha'));
if (expected.held.size !== 8 || actual.held.size !== 8) fail('scoped readback file count differs.');
for (const [name, bytes] of expected.held) if (!actual.held.get(name)?.equals(bytes)) fail('scoped readback differs from the tested held bytes.');
process.stdout.write(`${JSON.stringify({ candidateTag: expected.manifest.candidateTag, fileCount: 8, result: 'same-bytes' })}\n`);

});
