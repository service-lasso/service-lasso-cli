import { fail } from './protected-scoped-candidate-lib.mjs';

// Source-owned fnmatch pattern has no slash wildcard ambiguity: every governed
// candidate is a single tag component. Broader/unknown patterns are not proof.
export const CANDIDATE_TAG_PATTERN = 'refs/tags/cli-v*-candidate-*';
export function assertScopedTagPolicy(rulesets, tag) {
  if (!/^cli-v\d+\.\d+\.\d+-dev\.([0-9a-f]{7})-candidate-\1$/u.test(tag) ||
      !Array.isArray(rulesets) || rulesets.length === 0 || rulesets.length >= 100)
    fail('complete candidate tag rules are unavailable.');
  const ids = new Set(); let protectedTag = false;
  for (const rule of rulesets) {
    if (!Number.isSafeInteger(rule?.id) || rule.id < 1 || ids.has(rule.id)) fail('candidate ruleset identity is ambiguous.');
    ids.add(rule.id);
    if (rule.target !== 'tag' || rule.enforcement !== 'active') continue;
    const refs = rule.conditions?.ref_name;
    if (!Array.isArray(refs?.include) || !Array.isArray(refs?.exclude) || !Array.isArray(rule.rules) ||
        !Array.isArray(rule.bypass_actors)) fail('active tag ruleset is incomplete.');
    if (refs.include.length !== 1 || refs.include[0] !== CANDIDATE_TAG_PATTERN || refs.exclude.length !== 0) continue;
    const types = rule.rules.map(value => value?.type);
    if (new Set(types).size !== types.length) fail('candidate tag restrictions are ambiguous.');
    if (types.includes('update') && types.includes('deletion') && rule.bypass_actors.length === 0) protectedTag = true;
  }
  if (!protectedTag) fail('candidate tags require enforced update/deletion restrictions without bypass.');
}
