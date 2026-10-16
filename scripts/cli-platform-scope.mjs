import { createHash } from 'node:crypto';
import { isDeepStrictEqual } from 'node:util';
import { fileURLToPath } from 'node:url';
import { dirname } from 'node:path';
import { assertClosedObject, fail, parseStrictJson, readHeldFile, sha256 } from './protected-candidate-lib.mjs';

export const POLICY_REFERENCE = Object.freeze({ repository: 'service-lasso/service-lasso',
  commit: 'a0384e676c1b2dbf66b915563ea70c176c09d598',
  path: '.governance/project/ga-platform-scope.json', blob: 'e694c3e314c1bf11e03dc4656730a96467a765b8' });
export const POLICY_SHA256 = '159d644c161cf532c94d3bfe17ed55e32bf94c5d2843928945c450f6d8140c12';
export function scopeIdentity() {
  return { policyId: 'ga-windows-linux-2026-10-04', policySha256: POLICY_SHA256,
    policySource: { ...POLICY_REFERENCE }, requiredPlatforms: ['win32', 'linux'], deferredPlatforms: ['darwin'] };
}
export function assertScope(value) {
  assertClosedObject(value, ['policyId', 'policySha256', 'policySource', 'requiredPlatforms', 'deferredPlatforms'], 'scope');
  assertClosedObject(value.policySource, ['repository', 'commit', 'path', 'blob'], 'scope policy source');
  if (!isDeepStrictEqual(value, scopeIdentity())) fail('scope differs from the independently source-pinned policy.');
  return value;
}
export async function readSourceScope() {
  const location = fileURLToPath(new URL('../.governance/project/ga-platform-scope.json', import.meta.url));
  const bytes = await readHeldFile(dirname(location), 'ga-platform-scope.json', undefined, { maxBytes: 64 * 1024 });
  const blob = createHash('sha1').update(Buffer.from(`blob ${bytes.length}\0`)).update(bytes).digest('hex');
  if (sha256(bytes) !== POLICY_SHA256 || blob !== POLICY_REFERENCE.blob) fail('source policy original Git bytes differ.');
  const policy = parseStrictJson(bytes, 'source policy');
  assertClosedObject(policy, ['schema', 'policyId', 'ownerDecision', 'requiredPlatforms', 'deferredPlatforms', 'requirementsRevision'], 'policy');
  assertClosedObject(policy.ownerDecision, ['date', 'issue', 'authority'], 'owner decision');
  assertClosedObject(policy.requirementsRevision, ['repository', 'commit', 'path'], 'requirements revision');
  if (policy.schema !== 'service-lasso.ga-platform-scope.v1' || policy.policyId !== scopeIdentity().policyId ||
      !isDeepStrictEqual(policy.requiredPlatforms, ['win32', 'linux']) || !isDeepStrictEqual(policy.deferredPlatforms, ['darwin']))
    fail('source policy schema or platform order differs.');
  return scopeIdentity();
}
export function scopedArguments(args, names, repeated = []) {
  const allowed = new Set(names), repeat = new Set(repeated), values = new Map();
  for (let index = 0; index < args.length; index += 2) {
    const name = args[index], value = args[index + 1];
    if (!allowed.has(name) || typeof value !== 'string' || !value || value.startsWith('--') ||
        (values.has(name) && !repeat.has(name))) fail('unknown, duplicate or missing scoped argument.');
    const entries = values.get(name) ?? []; entries.push(value); values.set(name, entries);
  }
  for (const name of names) if (!values.has(name)) fail('missing scoped argument.');
  return { one: name => values.get(name)[0], all: name => [...values.get(name)] };
}

// Diagnostic-only JS custody. This is not a native owner, ROOT or pending-child retention plane.
const privateCliFailures = new Set();
export async function runScopedCli(operation) {
  try { return await operation(); }
  catch (original) {
    privateCliFailures.add(original);
    process.stderr.write('Scoped candidate operation failed.\n');
    process.exitCode = 1;
  }
}
