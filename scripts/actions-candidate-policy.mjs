// Operator dispatch preflight owns admin-only settings. The Actions token
// independently rereads these visible controls immediately before each write.
export const ACTIONS_REQUIRED_CHECKS = Object.freeze([
  "Node 22 CLI checks (ubuntu-latest)", "Node 22 CLI checks (windows-latest)",
  "Native SEA direct execution (Linux x64)", "Native SEA direct execution (Windows x64)",
  "Completion shell checks (ubuntu-latest)", "Direct real-Core reads (ubuntu-latest)",
]);
const deny = () => { throw new Error("Protected candidate rejected: Actions candidate policy denied."); };
export function assertActionsCandidatePolicy({ branch, environment, branchPolicies, checks }, sha, operatorPreflightSHA) {
  if (!/^[a-f0-9]{40}$/.test(sha) || operatorPreflightSHA !== sha) deny();
  if (branch?.name !== "develop" || branch?.protected !== true || branch?.commit?.sha !== sha || branch?.protection?.enabled !== true) deny();
  const required = branch.protection.required_status_checks;
  if (required?.enforcement_level !== "everyone" || !Array.isArray(required.checks) || required.checks.length !== ACTIONS_REQUIRED_CHECKS.length) deny();
  if (new Set(required.checks.map(check => check.context)).size !== ACTIONS_REQUIRED_CHECKS.length || !required.checks.every(check => check.app_id === 15368 && ACTIONS_REQUIRED_CHECKS.includes(check.context))) deny();
  const wait = environment?.protection_rules?.filter(rule => rule.type === "wait_timer");
  if (environment?.name !== "development-candidate" || wait?.length !== 1 || !Number.isInteger(wait[0].wait_timer) || wait[0].wait_timer < 1 || wait[0].wait_timer > 30 || environment?.deployment_branch_policy?.protected_branches !== false || environment?.deployment_branch_policy?.custom_branch_policies !== true) deny();
  if (branchPolicies?.total_count !== 1 || !Array.isArray(branchPolicies.branch_policies) || branchPolicies.branch_policies.length !== 1 || branchPolicies.branch_policies[0]?.name !== "develop" || branchPolicies.branch_policies[0]?.type !== "branch") deny();
  if (!Number.isInteger(checks?.total_count) || checks.total_count > 100 || !Array.isArray(checks.check_runs) || checks.total_count !== checks.check_runs.length) deny();
  for (const name of ACTIONS_REQUIRED_CHECKS) {
    const matching = checks.check_runs.filter(check => check.name === name && check.app?.id === 15368);
    if (matching.length !== 1 || matching[0].head_sha !== sha || matching[0].status !== "completed" || matching[0].conclusion !== "success") deny();
  }
  return JSON.stringify({ sha, requiredChecks: [...required.checks].sort((a,b) => a.context.localeCompare(b.context)), waitMinutes: wait[0].wait_timer, customBranch: "develop" });
}
