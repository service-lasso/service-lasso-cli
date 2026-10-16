import { runScopedCli } from './cli-platform-scope.mjs';
import { resolve } from 'node:path';
import { scopedArguments, readSourceScope } from './cli-platform-scope.mjs';
import { NATIVE_TARGETS, readHeldFile, parseStrictJson, verifyScopedNative, fail } from './protected-scoped-candidate-lib.mjs';
import { verifyPositiveNativeReceipt } from './native-positive-scoped-lib.mjs';
await runScopedCli(async () => {
await readSourceScope();
const args = scopedArguments(process.argv.slice(2), ['--receipt-directory', '--native-directory', '--archive-directory', '--version', '--source-sha']);
const target = NATIVE_TARGETS.find(value => value.platform === process.platform && value.architecture === process.arch);
if (!target) fail('positive native verifier requires its matching selected host.');
const version = args.one('--version'), sourceSha = args.one('--source-sha');
const archiveDirectory = resolve(args.one('--archive-directory'));
const archiveBytes = await readHeldFile(archiveDirectory, `service-lassoctl-${version}-${target.id}.tar.gz`);
const provenanceBytes = await readHeldFile(archiveDirectory, 'provenance.json');
verifyScopedNative(archiveBytes, provenanceBytes, target, sourceSha, version);
const result = await verifyPositiveNativeReceipt({ receiptDirectory: resolve(args.one('--receipt-directory')),
  nativeDirectory: resolve(args.one('--native-directory')), archiveBytes, target, version, sourceSha });
const provenance = parseStrictJson(provenanceBytes, 'accepted archive provenance');
if (provenance.executable.sha256 !== result.executableSha256) fail('positive proof executable differs from the accepted native archive.');
process.stdout.write(`${JSON.stringify({ target: target.id, result: 'closed-same-bytes', ...result })}\n`);

});
