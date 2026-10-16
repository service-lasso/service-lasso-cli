import { runScopedCli } from './cli-platform-scope.mjs';
import { pathToFileURL } from 'node:url';
import { POLICY_SHA256, readSourceScope } from './cli-platform-scope.mjs';
import { fail } from './protected-scoped-candidate-lib.mjs';
export async function assertScopedWorkflowSource(env) {
  const scope = await readSourceScope();
  const positive = value => typeof value === 'string' && /^[1-9][0-9]*$/u.test(value) && Number.isSafeInteger(Number(value));
  if (env.GITHUB_REPOSITORY !== 'service-lasso/service-lasso-cli' || env.GITHUB_EVENT_NAME !== 'workflow_dispatch' ||
      env.GITHUB_REF !== 'refs/heads/develop' || !/^[a-f0-9]{40}$/u.test(env.GITHUB_SHA ?? '') ||
      env.GITHUB_WORKFLOW_SHA !== env.GITHUB_SHA ||
      env.GITHUB_WORKFLOW_REF !== 'service-lasso/service-lasso-cli/.github/workflows/release-scoped.yml@refs/heads/develop' ||
      (env.CANDIDATE_SHA !== undefined && env.CANDIDATE_SHA !== env.GITHUB_SHA) ||
      env.SCOPE_POLICY_SHA256 !== POLICY_SHA256 || !positive(env.GITHUB_RUN_ID) || !positive(env.GITHUB_RUN_ATTEMPT))
    fail('scoped workflow source, policy or current run/attempt differs.');
  return { sourceSha: env.GITHUB_SHA, policySha256: scope.policySha256,
    runId: Number(env.GITHUB_RUN_ID), runAttempt: Number(env.GITHUB_RUN_ATTEMPT) };
}
if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  await runScopedCli(async () => {
  if (process.argv.length !== 2) fail('scoped source binding takes no caller inputs.');
  process.stdout.write(`${JSON.stringify(await assertScopedWorkflowSource(process.env))}\n`);
  });
}
