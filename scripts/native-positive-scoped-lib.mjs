import { isAbsolute } from 'node:path';
import { isDeepStrictEqual } from 'node:util';
import { readSourceScope } from './cli-platform-scope.mjs';
import { NATIVE_TARGETS, identity, fail, parseStrictJson, readHeldFile, sha256, verifyScopedNative } from './protected-scoped-candidate-lib.mjs';
import { assertClosedObject } from './protected-candidate-lib.mjs';

// Existing initial2 + phase1 + closed2 grammar only. Byte/source correspondence
// supplements the fixed real producer; a supplied JSON file cannot mint that producer.
export async function verifyPositiveNativeReceipt({ receiptDirectory, nativeDirectory, archiveBytes, target, version, sourceSha }) {
  await readSourceScope(); identity(version, sourceSha);
  if (!NATIVE_TARGETS.includes(target)) fail('positive receipt target is outside selected source.');
  const read = async name => parseStrictJson(await readHeldFile(receiptDirectory, name, undefined, { maxBytes: 1024 * 1024 }), 'positive native original');
  const initial = await read('initial.json'), closed = await read('closed.json');
  const initialKeys = ['schemaVersion', 'runId', 'sourceHead', 'rawHeadSha256', 'recursiveHeadTreeSha256',
    'declaredRuntimePathMetadata', 'nativeExecutableSha256', 'ownedBirth', 'actualClose'];
  assertClosedObject(initial, initialKeys, 'positive initial receipt');
  assertClosedObject(closed, [...initialKeys, 'phases'], 'positive closed receipt');
  assertClosedObject(initial.declaredRuntimePathMetadata, ['workspaceRoot', 'instanceRegistryPath', 'hostPortRegistryPath'], 'positive custody metadata');
  const executable = await readHeldFile(nativeDirectory, target.executable), digest = sha256(executable);
  const provenanceBytes = await readHeldFile(nativeDirectory, 'provenance.json');
  verifyScopedNative(archiveBytes, provenanceBytes, target, sourceSha, version);
  const provenance = parseStrictJson(provenanceBytes, 'positive executable provenance');
  const paths = Object.values(initial.declaredRuntimePathMetadata);
  if (paths.some(value => typeof value !== 'string' || !isAbsolute(value) || value.includes('\0')) || new Set(paths).size !== 3)
    fail('positive custody metadata paths differ from the existing initial contract.');
  if (provenance.source?.commit !== sourceSha || provenance.candidate?.version !== version || provenance.executable?.sha256 !== digest ||
      initial.schemaVersion !== 2 || initial.sourceHead !== sourceSha || initial.ownedBirth !== true || initial.actualClose !== null ||
      initial.nativeExecutableSha256 !== null || typeof initial.runId !== 'string' || !/^[a-f0-9]{8}-[a-f0-9]{4}-4[a-f0-9]{3}-[89ab][a-f0-9]{3}-[a-f0-9]{12}$/u.test(initial.runId) ||
      !/^[a-f0-9]{64}$/u.test(initial.rawHeadSha256) || !/^[a-f0-9]{64}$/u.test(initial.recursiveHeadTreeSha256) ||
      closed.schemaVersion !== 2 || closed.nativeExecutableSha256 !== digest || closed.ownedBirth !== true)
    fail('positive receipt does not join the actual source/executable originals.');
  for (const key of ['runId', 'sourceHead', 'rawHeadSha256', 'recursiveHeadTreeSha256', 'declaredRuntimePathMetadata'])
    if (!isDeepStrictEqual(closed[key], initial[key])) fail('positive initial/closed original differs.');
  assertClosedObject(closed.phases, ['smoke', 'route', 'archive'], 'positive phase roster');
  assertClosedObject(closed.actualClose, ['executableSha256', 'rawPhaseResults', 'outcome'], 'positive close');
  assertClosedObject(closed.actualClose.rawPhaseResults, ['smoke', 'route', 'archive'], 'positive raw close roster');
  if (closed.actualClose.executableSha256 !== digest || closed.actualClose.outcome !== 'passed') fail('positive close differs.');
  for (const phase of ['smoke', 'route', 'archive']) {
    const value = await read(`phase-${phase}.json`);
    assertClosedObject(value, ['schemaVersion', 'phase', 'sourceHead', 'rawHeadSha256', 'recursiveHeadTreeSha256', 'command',
      'argumentsSha256', 'nativeExecutableSha256', 'nativeExecutable', 'artifactSha256', 'artifact', 'qualificationStatus',
      'expectedExit', 'rawClose', 'passed', 'innerRoute'], 'positive phase');
    assertClosedObject(value.rawClose, ['code', 'signal', 'spawnError'], 'positive raw close');
    assertClosedObject(value.nativeExecutable, ['status', 'sha256', 'errorCode'], 'positive executable outcome');
    assertClosedObject(value.artifact, ['status', 'sha256', 'errorCode'], 'positive artifact outcome');
    if (!isDeepStrictEqual(value, closed.phases[phase]) ||
        !isDeepStrictEqual(value.rawClose, closed.actualClose.rawPhaseResults[phase]) || value.schemaVersion !== 1 || value.phase !== phase ||
        value.sourceHead !== sourceSha || value.rawHeadSha256 !== initial.rawHeadSha256 || value.recursiveHeadTreeSha256 !== initial.recursiveHeadTreeSha256 ||
        value.nativeExecutableSha256 !== digest || value.nativeExecutable.status !== 'present' || value.nativeExecutable.sha256 !== digest ||
        value.nativeExecutable.errorCode !== null || value.qualificationStatus !== 'verified' || value.expectedExit !== 'zero' ||
        value.rawClose.code !== 0 || value.rawClose.signal !== null || value.rawClose.spawnError !== null || value.passed !== true ||
        value.innerRoute !== null || typeof value.command !== 'string' || !value.command || !/^[a-f0-9]{64}$/u.test(value.argumentsSha256))
      fail('positive native phase is unavailable, incomplete or differs from its actual close.');
    if (phase === 'archive') {
      if (value.artifact.status !== 'present' || value.artifact.sha256 !== sha256(archiveBytes) ||
          value.artifactSha256 !== sha256(archiveBytes) || value.artifact.errorCode !== null) fail('positive archive bytes differ.');
    } else if (value.artifact.status !== 'not-requested' || value.artifact.sha256 !== null || value.artifactSha256 !== null ||
        value.artifact.errorCode !== null) fail('positive phase contains an unexpected artifact.');
  }
  return { executableSha256: digest, archiveSha256: sha256(archiveBytes) };
}
