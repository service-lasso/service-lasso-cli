/* SOURCE-UNRUN. Build-plane inputs must be independently admitted with this
 * exact command after whole SOURCE_GO. A manifest/hash is byte custody, never
 * a runtime actor/source/provider grant or executable qualification. */
import { spawn } from 'node:child_process';
import { open, mkdir, lstat, readlink, readdir, symlink } from 'node:fs/promises';
import { createHash } from 'node:crypto';
import { isAbsolute, join, resolve, relative, sep } from 'node:path';
import { readCoffArchiveMembers } from './coff-archive-members.mjs';

const families = ['libcmt', 'libvcruntime', 'libucrt', 'libcpmt'];
// Draft execution remains closed until genuine retained build-owner/namespace/
// native-input/compiler/member verification and useful full build are wired.
const producerComplete = false;
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
function demand(ok, message) { if (!ok) throw new Error(message); }
function exact(object, names) {
  demand(object && typeof object === 'object' && !Array.isArray(object), 'Expected closed object');
  const keys = Object.keys(object).sort();
  demand(keys.length === names.length && keys.every((key, i) => key === [...names].sort()[i]), 'Unknown/missing input field');
}
function positive(value) { return Number.isSafeInteger(value) && value > 0; }
function leaf(value) { return typeof value === 'string' && /^[A-Za-z0-9][A-Za-z0-9_.-]*$/.test(value) && value !== '.' && value !== '..'; }
function sourceRelative(value) {
  return typeof value === 'string' && value.length > 0 && !value.includes('\\') && !value.includes(':') &&
    !value.includes('\0') && value.split('/').every(part => part && part !== '.' && part !== '..');
}
function sameInput(left, right) {
  return left.path === right.path && left.bytes === right.bytes && left.sha256 === right.sha256;
}
/* Byte/source relation only: an authentic issuer, member provenance and ABI
 * admission remain independent. Consume the complete association body rather
 * than interpreting a syntactically valid digest as that authority. */
async function memberAssociation(original, input, archive, member, compiler) {
  const raw = await regular(original, input);
  const body = JSON.parse(raw.toString('utf8'));
  exact(body, ['schema', 'archiveSHA256', 'memberOffset', 'memberName', 'memberSHA256',
    'compiler', 'source', 'flags', 'includeInputs']);
  demand(body.schema === 'service-lasso.cli39.original-crt-source-relation.v1' &&
    body.archiveSHA256 === archive.original.sha256 && body.memberOffset === member.headerOffset &&
    body.memberName === member.name && body.memberSHA256 === member.sha256 &&
    sameInput(body.compiler, compiler) && Array.isArray(body.flags) &&
    body.flags.every(flag => typeof flag === 'string') && Array.isArray(body.includeInputs),
    'Missing complete original member/source/compiler relation');
  await regular(original, body.compiler);
  await regular(original, body.source);
  for (const header of body.includeInputs) await regular(original, header);
  const retained = { input, raw, body, authority: 'UNSUPPLIED', reachedIncludeClosure: 'UNQUALIFIED' };
  original.sourceRelations ??= [];
  original.sourceRelations.push(retained);
  return retained;
}
function closeFileOriginal(row) {
  if (!row.handle || row.closeAttempted) return row.closePromise;
  row.closeAttempted = true;
  // Start independent cleanup before any expired/failed observation gate.
  // The fulfilled outcome record prevents a late rejection going unobserved.
  try {
    row.closePromise = row.handle.close().then(
      value => { row.closeReturned = true; row.closeValue = value;
        return { status: 'returned', value }; },
      cause => { row.closeReturned = true; row.closeError = cause;
        return { status: 'rejected', cause }; });
  } catch (cause) {
    row.closeReturned = true; row.closeError = cause;
    row.closePromise = Promise.resolve({ status: 'rejected', cause });
  }
  return row.closePromise;
}
async function readFileOriginal(original, row, label) {
  let first;
  original.fileReadOriginals ??= [];
  original.fileReadOriginals.push(row);
  try {
    row.pathObservation = await step(original, `${label}-path-observe`, () => lstat(row.path, { bigint: true }));
    demand(row.pathObservation.isFile() && !row.pathObservation.isSymbolicLink(), 'Original read requires a regular leaf');
    await step(original, `${label}-open`, () => {
      row.openAttempted = true;
      return open(row.path, 'r').then(handle => {
        row.handle = handle; row.openReturned = true;
        // An expired open is not canceled. Its eventual actual handle remains
        // in this original and receives the same independently once-close.
        if (row.failureObserved) closeFileOriginal(row);
        return handle;
      }, cause => { row.openReturned = true; row.openError = cause; throw cause; });
    });
    await step(original, `${label}-handle-before`, () => {
      row.beforeAttempted = true;
      return row.handle.stat({ bigint: true }).then(value => {
        row.before = value; row.beforeReturned = true; return value;
      }, cause => { row.beforeReturned = true; row.beforeError = cause; throw cause; });
    });
    demand(row.before.isFile() && row.pathObservation.dev === row.before.dev &&
      row.pathObservation.ino === row.before.ino, 'Opened original differs from observed leaf');
    await step(original, `${label}-handle-read`, () => {
      row.readAttempted = true;
      return row.handle.readFile().then(value => {
        row.body = value; row.readReturned = true; return value;
      }, cause => { row.readReturned = true; row.readError = cause; throw cause; });
    });
    await step(original, `${label}-handle-after`, () => {
      row.afterAttempted = true;
      return row.handle.stat({ bigint: true }).then(value => {
        row.after = value; row.afterReturned = true; return value;
      }, cause => { row.afterReturned = true; row.afterError = cause; throw cause; });
    });
    demand(row.after.isFile() && row.before.dev === row.after.dev && row.before.ino === row.after.ino &&
      row.before.size === row.after.size && row.before.mtimeNs === row.after.mtimeNs &&
      row.before.ctimeNs === row.after.ctimeNs && BigInt(row.body.length) === row.before.size,
      'Original opened file changed during retained read');
  } catch (cause) { row.failureObserved = true; row.firstError = cause; first = cause; }
  const close = closeFileOriginal(row);
  if (close) {
    try {
      const outcome = await step(original, `${label}-close-observe`, () => close);
      if (outcome.status === 'rejected') throw failed(original, 'Original read once-close rejected', outcome.cause);
    } catch (cause) { row.closeObservationError = cause; first ??= cause; }
  }
  if (first) throw first;
  row.readComplete = true;
  return row.body;
}
async function regular(original, input) {
  exact(input, ['path', 'bytes', 'sha256']);
  demand(isAbsolute(input.path) && positive(input.bytes) && /^[a-f0-9]{64}$/.test(input.sha256), 'Invalid original input binding');
  const row = { input, path: input.path, body: null };
  const bytes = await readFileOriginal(original, row, 'input');
  demand(bytes.length === input.bytes && digest(bytes) === input.sha256, 'Original input bytes mismatched');
  return bytes;
}
async function parents(original, directory) {
  let current = resolve(directory);
  for (;;) {
    const state = await step(original, 'namespace-parent-observe', () => lstat(current));
    demand(state.isDirectory() && !state.isSymbolicLink(), 'Build parent is not an original directory');
    const next = resolve(current, '..');
    if (next === current) break;
    current = next;
  }
}
/* Actual retained package inputs, owned by this build occurrence. The tool
 * Node is a distinct build-plane original; it is never the guarded product
 * image or process.execPath selected as a replacement product image. These
 * reads retain source relation bytes, not an executable qualification token. */
async function packageToolInputs(original, inputs) {
  const roles = ['nodeImage', 'nodeSourceRelation', 'esbuildAPI', 'esbuildNativeImage',
    'esbuildSourceRelation', 'postjectCLI', 'postjectAPI', 'postjectCommander', 'postjectSourceRelation'];
  exact(inputs, roles);
  const retained = { node: {}, esbuild: {}, postject: {}, inputs: [],
    loadedInputClosure: 'UNQUALIFIED', buildContainingOwnership: 'UNQUALIFIED' };
  original.packageToolInputs = retained;
  for (const role of roles) {
    // Install the row before its first fallible read; every partial result is
    // retained through this same build's absorbing failed-original object.
    const row = { role, input: inputs[role], body: null };
    retained.inputs.push(row);
    row.body = await regular(original, row.input);
    row.sha256 = digest(row.body);
    if (role === 'nodeImage') retained.node.image = row;
    else if (role === 'nodeSourceRelation') retained.node.sourceRelation = row;
    else if (role === 'esbuildAPI') retained.esbuild.api = row;
    else if (role === 'esbuildNativeImage') retained.esbuild.nativeImage = row;
    else if (role === 'esbuildSourceRelation') retained.esbuild.sourceRelation = row;
    else if (role === 'postjectCLI') retained.postject.cli = row;
    else if (role === 'postjectAPI') retained.postject.api = row;
    else if (role === 'postjectCommander') retained.postject.commander = row;
    else retained.postject.sourceRelation = row;
  }
  /* Actual versions/generated-source/native-module relationships, immutable
   * namespace and loaded binary identity remain independent source obligations.
   * No relation parser or caller flag promotes this byte graph to authority. */
}
async function materializedSourceTree(original, tree, sourceCommit) {
  exact(tree, ['path', 'manifest']);
  demand(isAbsolute(tree.path), 'Missing exact current Node source tree');
  const manifest = JSON.parse((await regular(original, tree.manifest)).toString('utf8'));
  exact(manifest, ['schema', 'commit', 'entries']);
  demand(manifest.schema === 'service-lasso.cli39.original-node-tree.v1' && manifest.commit === sourceCommit &&
    Array.isArray(manifest.entries) && manifest.entries.length > 0, 'Wrong complete original Node source inventory');
  const entries = new Map();
  for (const entry of manifest.entries) {
    exact(entry, ['path', 'bytes', 'sha256', 'gitBlobOID', 'mode']);
    demand(sourceRelative(entry.path) && !entries.has(entry.path) && Number.isSafeInteger(entry.bytes) && entry.bytes >= 0 &&
      /^[a-f0-9]{64}$/.test(entry.sha256) && /^[a-f0-9]{40}$/.test(entry.gitBlobOID) &&
      [100644, 100755, 120000].includes(entry.mode), 'Unsupported/mismatched original source member variant');
    entries.set(entry.path, entry);
  }
  const actual = new Set();
  const retainedMembers = [];
  const pending = [''];
  original.nodeTree = { tree, manifest, actual, retainedMembers, pending, claimedCommit: sourceCommit,
    gitTreeCommitAuthority: 'UNSUPPLIED', emptyDirectoryClosure: 'UNSUPPLIED',
    retainedWholeTreeBuildOwnerBudget: 'UNQUALIFIED' };
  while (pending.length) {
    const directory = pending.pop();
    const state = await step(original, 'source-directory-observe', () => lstat(join(tree.path, directory), { bigint: true }));
    demand(state.isDirectory() && !state.isSymbolicLink(), 'Source namespace changed/unsupported');
    const names = await step(original, 'source-directory-enumerate', () => readdir(join(tree.path, directory)));
    for (const name of names) {
      demand(sourceRelative(name) && !name.includes('/'), 'Invalid original source child');
      const path = directory ? `${directory}/${name}` : name;
      const before = await step(original, 'source-member-before', () => lstat(join(tree.path, path), { bigint: true }));
      if (before.isDirectory() && !before.isSymbolicLink()) { pending.push(path); continue; }
      demand(entries.has(path) && !actual.has(path), 'Extra/replaced source member');
      const expected = entries.get(path);
      const link = expected.mode === 120000;
      demand(link ? before.isSymbolicLink() : before.isFile() && !before.isSymbolicLink(), 'Wrong original source type');
      const member = { entry: expected, path: join(tree.path, path), namespaceBefore: before,
        before, after: null, body: null, byteCompared: false };
      retainedMembers.push(member);
      if (link) {
        await step(original, 'source-link-read', () => {
          member.linkReadAttempted = true;
          return readlink(member.path, { encoding: 'buffer' }).then(value => {
            member.body = value; member.linkReadReturned = true; return value;
          }, cause => { member.linkReadReturned = true; member.linkReadError = cause; throw cause; });
        });
        await step(original, 'source-link-after', () => {
          member.linkAfterAttempted = true;
          return lstat(member.path, { bigint: true }).then(value => {
            member.after = value; member.linkAfterReturned = true; return value;
          }, cause => { member.linkAfterReturned = true; member.linkAfterError = cause; throw cause; });
        });
        demand(member.after.isSymbolicLink(), 'Original source link replaced');
      } else {
        await readFileOriginal(original, member, 'source-member');
        demand(before.dev === member.before.dev && before.ino === member.before.ino &&
          before.size === member.before.size && before.mtimeNs === member.before.mtimeNs &&
          before.ctimeNs === member.before.ctimeNs, 'Source namespace observation differs from opened member');
      }
      const { body, after } = member;
      const oid = createHash('sha1').update(Buffer.from(`blob ${body.length}\0`, 'ascii')).update(body).digest('hex');
      demand(before.dev === after.dev && before.ino === after.ino && before.size === after.size &&
        before.mtimeNs === after.mtimeNs && before.ctimeNs === after.ctimeNs && body.length === expected.bytes &&
        digest(body) === expected.sha256 && oid === expected.gitBlobOID, 'Wrong original source byte/Git association');
      actual.add(path);
      member.byteCompared = true;
    }
  }
  demand(actual.size === entries.size && [...entries.keys()].every(path => actual.has(path)), 'Incomplete actual original source tree');
  /* Structural full tree equality does not prove namespace lease, immutable
   * source-unit authority, complete symlink target/alias custody, compiler ABI
   * or eventual loaded runtime image. Original link bytes are not followed here. */
  original.nodeTree.byteCompared = true;
}
/* Current flat originals are read directly, without materializing another
 * original tree. Pinned metadata fixes the complete source-byte mapping;
 * it supplies neither a namespace lease nor executable/build authority. */
async function sourceTree(original, tree, sourceCommit) {
  exact(tree, ['path', 'manifest', 'treeMetadata', 'commitMetadata', 'receipts']);
  demand(isAbsolute(tree.path), 'Missing current raw corpus root');
  const pinned = {
    manifest: 'acce47ae514f42696b087dac45b54b65b2b5f6c8bffd14edf4cd79b93c712b80',
    treeMetadata: '428ec8cfdb590a3c963907a9ca2a91df5260d5ebd81e734b8c24917219fb4920',
    commitMetadata: '408aa2bbb8de8c1510c6360811d680326670974e4d1042df9bd60edff63eb2f9',
    receipts: 'c44e4e9ce1d734bcc4ab16d9856ec9740ac4c8e449debeaa64cf9e76ccd85f8e'
  };
  const retained = { tree, metadata: {}, retainedMembers: [], physicalBodies: new Map(),
    byteCompared: false, gitTreeCommitAuthority: 'UNSUPPLIED',
    retainedWholeTreeBuildOwnerBudget: 'UNQUALIFIED', namespaceAuthority: 'UNSUPPLIED' };
  original.nodeTree = retained;
  for (const role of Object.keys(pinned)) {
    const row = { role, input: tree[role], body: null };
    retained.metadata[role] = row;
    demand(row.input.sha256 === pinned[role], 'Current corpus metadata differs from pinned complete input');
    row.body = await regular(original, row.input);
  }
  const manifest = JSON.parse(retained.metadata.manifest.body.toString('utf8'));
  const metadata = JSON.parse(retained.metadata.treeMetadata.body.toString('utf8'));
  const commit = JSON.parse(retained.metadata.commitMetadata.body.toString('utf8'));
  retained.manifest = manifest; retained.treeMetadata = metadata; retained.commitMetadata = commit;
  demand(sourceCommit === 'aa4c77582be995286fc6e00aaf530dc7ade102a9' && commit.sha === sourceCommit &&
    commit.tree.sha === 'cee5f56eb6b8b6b8e300419196dc90e8c7abd64b' && metadata.sha === commit.tree.sha &&
    metadata.truncated === false && metadata.tree.length === 47300 &&
    manifest.schema === 'service-lasso.cli39.current-original-raw-corpus.v1' &&
    manifest.state === 'COMPLETE_RAW_BLOB_CUSTODY_ONLY' && manifest.commit === sourceCommit &&
    manifest.tree === metadata.sha && manifest.entries === 47300 && manifest.blobEntries === 43494 &&
    manifest.logicalBytes === 627103490 && manifest.referencedBodyFiles === 38874 &&
    manifest.referencedPhysicalBytes === 516776675 && manifest.symlinkBlobs === 6 &&
    manifest.linkMaterialized === false && manifest.bodies.length === 38874,
    'Incomplete current original corpus metadata');
  const expected = metadata.tree.filter(entry => entry.type === 'blob');
  const expectedByPath = new Map(expected.map(entry => [entry.path, entry]));
  const receipts = retained.metadata.receipts.body.toString('utf8').trimEnd().split(/\r?\n/).map(line => JSON.parse(line));
  retained.receipts = receipts;
  demand(expected.length === 43494 && expectedByPath.size === expected.length &&
    receipts.length === expected.length, 'Incomplete logical source receipts');
  const physical = new Map();
  for (const body of manifest.bodies) {
    exact(body, ['rawFile', 'bytes', 'gitBlobSHA1', 'sha256']);
    demand(/^raw\/[0-9]{6}\.body$/.test(body.rawFile) && !physical.has(body.rawFile) &&
      Number.isSafeInteger(body.bytes) && body.bytes >= 0 && /^[a-f0-9]{40}$/.test(body.gitBlobSHA1) &&
      /^[A-F0-9]{64}$/.test(body.sha256), 'Invalid referenced physical original');
    physical.set(body.rawFile, body);
  }
  let logicalBytes = 0, physicalBytes = 0, links = 0;
  const paths = new Set(), ordinals = new Set();
  for (let index = 0; index < receipts.length; index++) {
    const receipt = receipts[index], selected = expectedByPath.get(receipt.path);
    exact(receipt, ['ordinal', 'path', 'mode', 'type', 'gitBlobSHA1', 'declaredBytes', 'rawFile',
      'actualBytes', 'actualGitBlobSHA1', 'sha256', 'verified', 'linkMaterialized']);
    const body = physical.get(receipt.rawFile);
    demand(Number.isSafeInteger(receipt.ordinal) && receipt.ordinal >= 0 && receipt.ordinal < expected.length &&
      !ordinals.has(receipt.ordinal) && expected[receipt.ordinal] === selected &&
      sourceRelative(receipt.path) && !paths.has(receipt.path) &&
      selected && receipt.path === selected.path && receipt.mode === selected.mode && receipt.type === selected.type &&
      ['100644', '100755', '120000'].includes(receipt.mode) && receipt.gitBlobSHA1 === selected.sha &&
      receipt.declaredBytes === selected.size && receipt.actualBytes === selected.size &&
      receipt.actualGitBlobSHA1 === selected.sha && receipt.verified === true && receipt.linkMaterialized === false &&
      body && body.bytes === selected.size && body.gitBlobSHA1 === selected.sha && body.sha256 === receipt.sha256,
      'Logical original differs from complete pinned tree/body mapping');
    paths.add(receipt.path);
    ordinals.add(receipt.ordinal);
    const member = { entry: { path: receipt.path, bytes: receipt.actualBytes,
      sha256: receipt.sha256.toLowerCase(), gitBlobOID: receipt.gitBlobSHA1, mode: Number(receipt.mode) },
      receipt, physicalOriginal: null, body: null, byteCompared: false };
    retained.retainedMembers.push(member);
    let held = retained.physicalBodies.get(receipt.rawFile);
    if (!held) {
      held = { path: join(tree.path, receipt.rawFile), expected: body, body: null, byteCompared: false };
      retained.physicalBodies.set(receipt.rawFile, held);
      await readFileOriginal(original, held, 'raw-source-original');
      const oid = createHash('sha1').update(Buffer.from(`blob ${held.body.length}\0`, 'ascii')).update(held.body).digest('hex');
      demand(held.body.length === body.bytes && digest(held.body) === body.sha256.toLowerCase() &&
        oid === body.gitBlobSHA1, 'Actual opened raw original byte mismatch');
      held.byteCompared = true; physicalBytes += held.body.length;
    }
    member.physicalOriginal = held; member.body = held.body; member.byteCompared = true;
    logicalBytes += member.body.length;
    if (receipt.mode === '120000') links++;
  }
  demand(retained.physicalBodies.size === physical.size && logicalBytes === 627103490 &&
    physicalBytes === 516776675 && links === 6, 'Incomplete referenced physical corpus');
  retained.byteCompared = true;
}
async function stageNodeTree(original, nativeDirectory) {
  const runtimeDirectory = join(nativeDirectory, 'node-runtime');
  const destination = join(runtimeDirectory, 'source');
  const retained = { destination, directories: [], members: [],
    namespaceAuthority: 'UNSUPPLIED', modeAndAliasQualification: 'UNSUPPLIED' };
  original.stagedNodeTree = retained;
  for (const directory of [runtimeDirectory, destination]) {
    await step(original, 'fresh-node-source-root', () => mkdir(directory, { recursive: false }));
    retained.directories.push(directory);
  }
  const directories = new Set();
  for (const { entry } of original.nodeTree.retainedMembers) {
    const parts = entry.path.split('/');
    for (let length = 1; length < parts.length; length++) directories.add(parts.slice(0, length).join('/'));
  }
  for (const path of [...directories].sort((left, right) => left.split('/').length - right.split('/').length || left.localeCompare(right))) {
    const directory = join(destination, path);
    await step(original, 'fresh-node-source-directory', () => mkdir(directory, { recursive: false }));
    retained.directories.push(directory);
  }
  for (const source of original.nodeTree.retainedMembers) {
    const path = join(destination, source.entry.path);
    const overlay = original.nodeOverlays.get(source.entry.path);
    const body = overlay ? overlay.body : source.body;
    const output = { source, overlay: overlay ?? null, body, path, emitted: false, readback: null, equal: false };
    retained.members.push(output);
    if (source.entry.mode === 120000) {
      const target = source.body.toString('utf8');
      demand(Buffer.from(target, 'utf8').equals(source.body) && !target.includes('\0') && !isAbsolute(target),
        'Original symbolic source target needs explicit native representation qualification');
      const parent = resolve(path, '..');
      const targetRelative = relative(destination, resolve(parent, target));
      demand(targetRelative && targetRelative !== '..' && !targetRelative.startsWith(`..${sep}`) && !isAbsolute(targetRelative),
        'Original symbolic source target leaves fresh source namespace');
      await step(original, 'fresh-node-source-symbolic-member', () => symlink(target, path));
      output.emitted = true;
      output.readback = await step(original, 'staged-node-symbolic-byte-readback', () => readlink(path, { encoding: 'buffer' }));
    } else {
      await saveFresh(original, path, body);
      output.emitted = true;
      output.readbackOriginal = { path, body: null };
      output.readback = await readFileOriginal(original, output.readbackOriginal, 'staged-node-source');
    }
    demand(output.readback.equals(body), 'Staged Node member differs from retained original/overlay source bytes');
    output.equal = true;
  }
  /* Original executable mode, native symlink type/target identity, complete
   * source-tree/commit authority and writer exclusion are separate admission.
   * No copying from process.execPath or reduced dependency tree occurs here. */
  return retained;
}
async function nodeOverlayRelations(original, associationInput, inputs, sourceCommit) {
  const raw = await regular(original, associationInput);
  const association = JSON.parse(raw.toString('utf8'));
  exact(association, ['fullOriginalCommit', 'records']);
  demand(association.fullOriginalCommit === sourceCommit && Array.isArray(association.records) &&
    Array.isArray(inputs) && inputs.length === association.records.length,
    'Incomplete current Node overlay/source relation');
  original.nodeOverlayRelation = { associationInput, raw, association,
    semanticWholeSourceApproval: 'UNSUPPLIED', originalCommitAuthority: 'UNSUPPLIED' };
  original.nodeOverlays = new Map();
  for (const record of association.records) {
    exact(record, ['path', 'originalOID', 'originalBytes', 'originalSHA256', 'overlaySHA256', 'state']);
    demand(sourceRelative(record.path) && !original.nodeOverlays.has(record.path), 'Duplicate/invalid Node overlay relation');
    const source = original.nodeTree.retainedMembers.find(member => member.entry.path === record.path);
    const matches = inputs.filter(input => input.target === record.path);
    demand(source && source.entry.mode !== 120000 && matches.length === 1 &&
      source.entry.gitBlobOID === record.originalOID && source.body.length === record.originalBytes &&
      digest(source.body) === record.originalSHA256, 'Node overlay does not join exact retained original member');
    exact(matches[0], ['target', 'input']);
    const body = await regular(original, matches[0].input);
    demand(digest(body) === record.overlaySHA256, 'Node overlay bytes differ from current source association');
    original.nodeOverlays.set(record.path, { record, input: matches[0].input, original: source, body });
  }
  /* Current association states and byte joins are not semantic approval,
   * original Git tree/commit authority or native image/source admission. */
}
function nodeBuildInvocation(original, commandInput) {
  const retained = { commandInput, cwd: original.stagedNodeTree.destination,
    generateArgs: ['/d', '/s', '/c', 'vcbuild.bat release x64 full-icu vs2022 projgen nobuild'],
    args: ['/d', '/s', '/c', 'vcbuild.bat release x64 full-icu vs2022 noprojgen'],
    sources: [], gypPythonSources: [], actualLoadedToolClosure: 'UNQUALIFIED' };
  original.nodeBuildInvocation = retained;
  const required = ['vcbuild.bat', 'configure', 'configure.py', 'node.gyp', 'common.gypi',
    'src/node_version.h', 'tools/getnodeversion.py', 'tools/gyp_node.py',
    'tools/msvs/find_python.cmd', 'tools/msvs/find_nasm.cmd', 'tools/msvs/vswhere_usability_wrapper.cmd'];
  for (const path of required) {
    const matches = original.stagedNodeTree.members.filter(member => member.source.entry.path === path);
    demand(matches.length === 1 && matches[0].source.byteCompared && matches[0].emitted && matches[0].equal,
      'Actual Node build entry/helper is not the retained staged original');
    retained.sources.push(matches[0]);
  }
  for (const member of original.stagedNodeTree.members) {
    if (!member.source.entry.path.startsWith('tools/gyp/pylib/')) continue;
    demand(member.source.byteCompared && member.emitted && member.equal,
      'Incomplete staged GYP Python package source');
    retained.gypPythonSources.push(member);
  }
  demand(retained.gypPythonSources.some(member => member.source.entry.path === 'tools/gyp/pylib/gyp/__init__.py'),
    'Actual gyp_node import has no retained source package');
  const version = retained.sources.find(member => member.source.entry.path === 'src/node_version.h');
  demand(!version.overlay, 'Selected original Node version header was replaced');
  const text = version.body.toString('utf8');
  for (const [name, value] of [['MAJOR', '22'], ['MINOR', '23'], ['PATCH', '2']]) {
    const matches = [...text.matchAll(new RegExp(`^#define NODE_${name}_VERSION[ \\t]+([0-9]+)[ \\t]*\\r?$`, 'gm'))];
    demand(matches.length === 1 && matches[0][1] === value, 'Wrong actual source-built Node version');
  }
  /* This binds source operands, not the actual executables selected by batch
   * PATH/discovery or their generated/loaded dependencies. No .git is invented
   * for version computation: this selected helper reads node_version.h. */
  return retained;
}
async function generatedNodeConfiguration(original, prior = null) {
  const retained = { prior, files: [], values: null, sourceProfileCompared: false };
  original.generatedConfigurationCuts ??= [];
  original.generatedConfigurationCuts.push(retained);
  for (const relativePath of ['config.gypi', '.used_configure_flags']) {
    const file = { relativePath, path: join(original.stagedNodeTree.destination, relativePath), body: null };
    retained.files.push(file);
    await readFileOriginal(original, file, 'generated-configuration');
    file.sha256 = digest(file.body);
    if (prior) {
      const before = prior.files.find(row => row.relativePath === relativePath);
      demand(before && before.body.equals(file.body), 'Generated full-runtime configuration changed during compilation');
    }
  }
  const config = retained.files[0].body;
  // The selected Windows Python Path.open text writer translates LF to CRLF.
  const header = Buffer.from('# Do not edit. Generated by the configure script.\r\n', 'utf8');
  demand(config.subarray(0, header.length).equals(header), 'Unexpected original generated config header');
  const raw = config.subarray(header.length);
  const text = raw.toString('utf8');
  demand(Buffer.from(text, 'utf8').equals(raw), 'Invalid generated configuration UTF8');
  const body = JSON.parse(text);
  demand(body && typeof body === 'object' && !Array.isArray(body) &&
    body.variables && typeof body.variables === 'object' && !Array.isArray(body.variables),
    'Missing actual generated Node variables');
  retained.values = body;
  const expected = { target_arch: 'x64', node_shared: 'false', force_dynamic_crt: 0,
    icu_small: 'false', v8_enable_i18n_support: 1, node_use_bundled_v8: 'true',
    node_use_v8_platform: 'true', node_use_node_snapshot: 'true', node_use_node_code_cache: 'true',
    v8_enable_webassembly: 1, v8_enable_inspector: 1, v8_enable_lite_mode: 0 };
  for (const [name, value] of Object.entries(expected))
    demand(Object.hasOwn(body.variables, name) && body.variables[name] === value,
      `Generated full-runtime profile differs: ${name}`);
  for (const name of ['node_shared_libuv', 'node_shared_openssl', 'node_shared_zlib',
    'node_shared_brotli', 'node_shared_cares', 'node_shared_nghttp2'])
    demand(body.variables[name] === 'false', `Generated runtime uses an unbound external dependency: ${name}`);
  const flags = retained.files[1].body.toString('utf8').trim();
  demand(flags === '--with-ltcg --with-intl=full-icu --dest-cpu=x64',
    'Actual original configure flags differ from the selected release recipe');
  retained.sourceProfileCompared = true;
  // Generated JSON/flags equality is configuration correspondence. It does not
  // establish actual MSBuild /MT link, native tool custody or loaded ancestry.
  return retained;
}
function sourceCommandGeometry(plan) {
  demand(Array.isArray(plan.archives) && plan.archives.length === families.length,
    'Missing complete source command families');
  const commands = [{ label: 'kernelbase-import', executable: plan.tools.librarian.path },
    { label: 'ntdll-import', executable: plan.tools.librarian.path }];
  for (let index = 0; index < families.length; index++) {
    const archive = plan.archives[index];
    demand(archive.family === families[index] && Array.isArray(archive.replacements),
      'Wrong complete source command order');
    for (const replacement of archive.replacements) {
      commands.push({ label: `${archive.family}-compile`, executable: plan.tools.compiler.path, replacement });
      commands.push({ label: `${archive.family}-archive`, executable: plan.tools.librarian.path, replacement });
    }
  }
  commands.push({ label: 'node-generate', executable: plan.tools.cmd.path });
  commands.push({ label: 'node-build', executable: plan.tools.cmd.path });
  demand(positive(plan.captureBytes) && commands.length <= Math.floor(Number.MAX_SAFE_INTEGER / 2) &&
    plan.captureBytes <= Math.floor(Number.MAX_SAFE_INTEGER / (2 * commands.length)),
    'Full source command capture arithmetic overflow');
  return { commands: commands.map((row, index) => ({ ...row, ordinal: index + 1,
    issued: false, completed: false, argsOriginal: null, cwdOriginal: null })),
    payloadMaximum: 2 * commands.length * plan.captureBytes };
}
async function nativeSourceInputs(original, inputs) {
  demand(Array.isArray(inputs) && inputs.length > 0, 'Missing complete current native owner source inputs');
  const retained = new Map();
  original.nativeSourceInputs = retained;
  for (const input of inputs) {
    exact(input, ['target', 'input']);
    demand(sourceRelative(input.target) && /^(node-owner|host-service)\//.test(input.target) &&
      /\.(c|cpp|h|def)$/.test(input.target) && !retained.has(input.target), 'Invalid/duplicate native source target');
    const body = await regular(original, input.input);
    retained.set(input.target, { input, body, sourceAuthority: 'UNSUPPLIED' });
  }
  for (const required of ['node-owner/early_allocator.c', 'node-owner/allocator_caller.c',
    'node-owner/thread_windows.c', 'node-owner/cxx_new.cpp', 'host-service/arena.c',
    'host-service/native_clock.c', 'host-service/source_deadline.c',
    'node-owner/preentry.c', 'node-owner/preentry.h', 'node-owner/bootstrap.c', 'node-owner/bootstrap.h',
    'node-owner/bootstrap_transport.c', 'node-owner/bootstrap_transport.h',
    'node-owner/parent_windows.c', 'node-owner/parent_windows.h',
    'node-owner/receive_windows.c', 'node-owner/receive_windows.h',
    'node-owner/selector_windows.c', 'node-owner/selector_windows.h',
    'node-owner/route_windows.c', 'node-owner/route_windows.h',
    'node-owner/entry_windows.c', 'node-owner/entry_windows.h',
    'host-service/source_image_windows.c', 'host-service/source_image_windows.h',
    'host-service/source_file_windows.c', 'host-service/source_file_windows.h',
    'host-service/installed_namespace_windows.c', 'host-service/installed_namespace_windows.h',
    'host-service/source_stream_windows.c', 'host-service/source_stream_windows.h',
    'host-service/route_write_windows.c', 'host-service/route_write_windows.h',
    'node-owner/prefix_windows.c', 'node-owner/prefix_windows.h',
    'node-owner/loader_windows.c', 'node-owner/loader_windows.h',
    'node-owner/capture_layout_windows.c', 'node-owner/capture_layout_windows.h',
    'host-service/owner_handoff_codec.c', 'host-service/owner_handoff_codec.h',
    'host-service/owner_memory.c', 'host-service/platform_memory_windows.c', 'host-service/platform_memory_linux.c',
    'host-service/preentry_backing_linux.c', 'host-service/preentry_backing_linux.h',
    'host-service/preentry_backing_windows.c', 'host-service/preentry_backing_windows.h',
    'host-service/capture_alias_windows.c', 'host-service/capture_alias_windows.h',
    'host-service/runtime_containing_windows.c', 'host-service/runtime_containing_windows.h',
    'host-service/child_mapping_windows.c', 'host-service/child_mapping_windows.h',
    'host-service/arena_layout.h',
    'host-service/kernelbase-preentry.def', 'host-service/ntdll-preentry.def'])
    demand(retained.has(required), 'Missing actual GYP native owner source');
  /* Required direct GYP leaves are not the complete transitive header/native
   * source closure. Whole fresh source review and input admission must bind it. */
}
async function buildEnvironment(original, input) {
  const raw = await regular(original, input);
  const body = JSON.parse(raw.toString('utf8'));
  exact(body, ['schema', 'entries']);
  demand(body.schema === 'service-lasso.cli39.original-build-environment.v1' && Array.isArray(body.entries),
    'Missing original build environment input');
  const environment = Object.create(null), names = new Set();
  for (const entry of body.entries) {
    exact(entry, ['name', 'value']);
    demand(typeof entry.name === 'string' && /^[A-Za-z_][A-Za-z0-9_]*$/.test(entry.name) &&
      typeof entry.value === 'string' && !entry.value.includes('\0'), 'Invalid original environment entry');
    const name = entry.name.toUpperCase();
    demand(!names.has(name) && !['NODE_OPTIONS', 'NODE_PATH', 'CL', '_CL_', 'LINK', '_LINK_',
      'CONFIGURE_FLAGS', 'CONFIG_FLAGS', 'MSBUILD_ARGS'].includes(name), 'Duplicate or unbound build override environment');
    names.add(name); environment[entry.name] = entry.value;
  }
  for (const required of ['SYSTEMROOT', 'COMSPEC', 'PATH', 'INCLUDE', 'LIB', 'LIBPATH', 'TEMP', 'TMP'])
    demand(names.has(required), 'Incomplete explicit original Windows build environment');
  original.environmentInput = { input, raw, body, sourceAndToolHelperAuthority: 'UNSUPPLIED' };
  original.environment = Object.freeze(environment);
  /* This closes ambient inheritance only. PATH/helper DLL/tool/include/LIB
   * resolution and compiler/ENV authority need exact original input admission. */
}
function buildOutputRecipe(original, outputs) {
  demand(Array.isArray(outputs) && outputs.length >= 2, 'Missing full source-built image/library output recipe');
  const names = new Set(), paths = new Set();
  for (const output of outputs) {
    exact(output, ['target', 'path', 'kind']);
    demand(leaf(output.target) && sourceRelative(output.path) && !names.has(output.target) &&
      !paths.has(output.path) && ['image', 'archive', 'metadata'].includes(output.kind), 'Invalid/duplicate original build output');
    names.add(output.target); paths.add(output.path);
  }
  demand(outputs.some(output => output.target === 'node-core' && output.kind === 'image') &&
    outputs.some(output => output.target === 'libnode' && output.kind === 'archive'),
    'Full facade requires actual libnode intermediate and executable outputs');
  original.buildOutputRecipe = { outputs, fullTransitiveSourceGraph: 'UNSUPPLIED',
    generatedProductDirectoryAndLinkAncestry: 'UNQUALIFIED' };
  /* Target labels/path geometry are planned selectors, never authentic source,
   * whole transitive graph, ABI or generated PRODUCT_DIR authority. */
}
/* Fixed AMD64 PE locator extraction from the SAME retained image vector.
 * This proves byte geometry only, never loaded-image or namespace identity. */
function retainedRouteExport(body) {
  const need = (offset, bytes) => demand(Number.isSafeInteger(offset) && offset >= 0 &&
    Number.isSafeInteger(bytes) && bytes >= 0 && offset <= body.length - bytes,
  'Truncated source-built PE route geometry');
  const u16 = offset => { need(offset, 2); return body.readUInt16LE(offset); };
  const u32 = offset => { need(offset, 4); return body.readUInt32LE(offset); };
  need(0, 64); demand(u16(0) === 0x5a4d, 'Route image lacks DOS header');
  const pe = u32(60); need(pe, 24);
  demand(u32(pe) === 0x4550 && u16(pe + 4) === 0x8664, 'Route image is not AMD64 PE');
  const count = u16(pe + 6), optionalBytes = u16(pe + 20), optional = pe + 24;
  demand(count > 0 && optionalBytes >= 120, 'Missing PE section/export geometry');
  need(optional, optionalBytes); demand(u16(optional) === 0x20b && u32(optional + 108) >= 1,
    'Route image is not PE32+ with export directory');
  const imageBytes = u32(optional + 56), headers = u32(optional + 60);
  demand(imageBytes > 0 && headers > 0 && headers <= imageBytes, 'Invalid PE image span');
  const sections = [], table = optional + optionalBytes; need(table, count * 40);
  for (let i = 0; i < count; i++) {
    const at = table + i * 40, virtualBytes = u32(at + 8), rva = u32(at + 12);
    const rawBytes = u32(at + 16), raw = u32(at + 20), flags = u32(at + 36);
    need(raw, rawBytes);
    demand(rva >= headers && virtualBytes > 0 && rva <= imageBytes - virtualBytes &&
      sections.every(s => rva >= s.rva + s.virtualBytes || s.rva >= rva + virtualBytes),
    'Overlapping/outside PE section');
    sections.push({ ordinal: i, rva, virtualBytes, rawBytes, raw, flags });
  }
  const locate = (rva, bytes) => {
    const matches = sections.filter(s => rva >= s.rva && bytes <= s.virtualBytes &&
      rva - s.rva <= s.virtualBytes - bytes && bytes <= s.rawBytes &&
      rva - s.rva <= s.rawBytes - bytes);
    demand(matches.length === 1, 'PE RVA lacks unique initialized section');
    const section = matches[0], offset = section.raw + rva - section.rva;
    need(offset, bytes); return { section, offset };
  };
  const exportRVA = u32(optional + 112), exportBytes = u32(optional + 116);
  demand(exportBytes >= 40 && exportRVA <= imageBytes - exportBytes, 'Missing PE exports');
  const exp = locate(exportRVA, 40).offset;
  const functions = u32(exp + 20), names = u32(exp + 24);
  demand(functions > 0 && names > 0, 'Invalid PE export cardinalities');
  const addressTable = locate(u32(exp + 28), functions * 4).offset;
  const nameTable = locate(u32(exp + 32), names * 4).offset;
  const ordinalTable = locate(u32(exp + 36), names * 2).offset;
  const wanted = Buffer.from('slcli_node_route_cell_v1\0', 'ascii');
  let found = null;
  for (let i = 0; i < names; i++) {
    const nameRVA = u32(nameTable + i * 4), ordinal = u16(ordinalTable + i * 2);
    demand(ordinal < functions, 'PE export ordinal outside original address table');
    const first = locate(nameRVA, 1), available = Math.min(first.section.rawBytes,
      first.section.virtualBytes) - (nameRVA - first.section.rva);
    const nul = body.indexOf(0, first.offset);
    demand(nul >= first.offset && nul - first.offset < available, 'Unterminated PE export name');
    if (nul - first.offset + 1 !== wanted.length || !body.subarray(first.offset, nul + 1).equals(wanted)) continue;
    demand(found === null, 'Duplicate source route export');
    const rva = u32(addressTable + ordinal * 4), located = locate(rva, 128);
    demand(!(rva >= exportRVA && rva < exportRVA + exportBytes) &&
      (located.section.flags & 0x80000000) !== 0 &&
      (located.section.flags & 0x20000000) === 0 &&
      body.subarray(located.offset, located.offset + 128).every(byte => byte === 0),
    'Route export forwarded, executable, nonwritable or not initially zero');
    found = { symbol: 'slcli_node_route_cell_v1', rva, bytes: 128, imageBytes,
      rawOffset: located.offset, section: located.section, nativeImageAssociation: 'UNQUALIFIED' };
  }
  demand(found !== null, 'Source-built image lacks actual route cell export'); return found;
}
async function retainBuildOutputs(original) {
  original.builtOutputs = [];
  for (const recipe of original.buildOutputRecipe.outputs) {
    const output = { recipe, path: join(original.stagedNodeTree.destination, recipe.path),
      body: null, loadedToolAndCRTAncestry: 'UNQUALIFIED', generatedConfiguration: 'UNQUALIFIED',
      constructorAndEntryFlags: 'UNQUALIFIED', archive: null };
    original.builtOutputs.push(output);
    await readFileOriginal(original, output, 'build-output');
    demand(output.body.length > 0, 'Source build output is empty');
    output.sha256 = digest(output.body);
    if (recipe.kind === 'archive') output.archive = readCoffArchiveMembers(output.body);
    if (recipe.target === 'node-core' && recipe.kind === 'image')
      output.routeExport = retainedRouteExport(output.body);
  }
  const productionImages = original.builtOutputs.filter(output =>
    output.recipe.target === 'node-core' && output.recipe.kind === 'image');
  demand(productionImages.length === 1 && original.packageToolInputs,
    'Missing unique actual production image or distinct retained package tools');
  original.packageProducerOriginals = {
    buildOriginal: original, productionImage: productionImages[0],
    buildToolNode: original.packageToolInputs.node,
    esbuild: original.packageToolInputs.esbuild, postject: original.packageToolInputs.postject,
  };
  /* Full ordered raw archive inventories are preserved, not just node.exe.
   * Held native namespace/writer/loaded-source and complete link authority are
   * still independently required before any facade/public image publication. */
}
async function stageNativeSources(original, nativeDirectory) {
  const directories = new Set();
  for (const target of original.nativeSourceInputs.keys()) {
    const parts = target.split('/');
    for (let length = 1; length < parts.length; length++) directories.add(parts.slice(0, length).join('/'));
  }
  for (const target of [...directories].sort((left, right) => left.split('/').length - right.split('/').length || left.localeCompare(right)))
    await step(original, 'fresh-native-source-directory', () => mkdir(join(nativeDirectory, target), { recursive: false }));
  for (const [target, source] of original.nativeSourceInputs) {
    source.outputPath = join(nativeDirectory, target);
    await saveFresh(original, source.outputPath, source.body);
    source.readbackOriginal = { path: source.outputPath, body: null };
    source.readback = await readFileOriginal(original, source.readbackOriginal, 'staged-native-source');
    demand(source.readback.equals(source.body), 'Staged native owner source differs from retained current input');
    source.equal = true;
  }
}
function verifySuccessor(original, predecessor, successorRaw, replacedOffset, replacementBody) {
  const successor = readCoffArchiveMembers(successorRaw);
  const previousMembers = predecessor.members;
  const originalMember = previousMembers.find(member => member.headerOffset === replacedOffset);
  demand(originalMember && replacementBody.length > 0, 'Missing genuine replaced member output');
  const expected = previousMembers.map(member => ({ name: member.name,
    bodySHA256: digest(member.headerOffset === replacedOffset ? replacementBody : member.body) }));
  const key = member => `${member.name.length}:${member.name}:${member.bodySHA256}`;
  const before = expected.map(key).sort();
  const after = successor.members.map(member => key({ name: member.name, bodySHA256: digest(member.body) })).sort();
  demand(before.length === after.length && before.every((value, index) => value === after[index]),
    'Archive successor dropped/changed/duplicated original or compiled member');
  const oldByOffset = new Map(previousMembers.map(member => [member.headerOffset, member]));
  const newByOffset = new Map(successor.members.map(member => [member.headerOffset, member]));
  function symbols(directory, byOffset, replace) {
    return directory.map(row => {
      const member = byOffset.get(row.memberOffset);
      const body = replace && row.memberOffset === replacedOffset ? replacementBody : member.body;
      return `${row.symbol.toString('hex')}:${key({ name: member.name, bodySHA256: digest(body) })}`;
    }).sort();
  }
  const oldSymbols = symbols(predecessor.secondDirectory, oldByOffset, true);
  const newSymbols = symbols(successor.secondDirectory, newByOffset, false);
  demand(oldSymbols.length === newSymbols.length && oldSymbols.every((value, index) => value === newSymbols[index]),
    'Changed original public archive symbol/member contract; explicit source/ABI amendment required');
  original.rows.push({ label: 'complete-archive-successor-verification', returned: true,
    result: { predecessor, successor, replacedOffset, replacementBody } });
  /* Structural byte/member/export preservation does not supply compiler
   * source association, original native namespace custody or image admission. */
  return successor;
}
async function saveFresh(original, path, bytes) {
  const retained = { path, bytes, file: null, handle: null, writes: [],
    written: 0, flushed: false, closed: false };
  let first;
  original.files.push(retained);
  try {
    await step(original, 'fresh-file-open', () => {
      retained.openAttempted = true;
      return open(path, 'wx').then(handle => {
        retained.file = handle; retained.handle = handle; retained.openReturned = true;
        if (retained.failureObserved) closeFileOriginal(retained);
        return handle;
      }, cause => { retained.openReturned = true; retained.openError = cause; throw cause; });
    });
    while (retained.written < bytes.length) {
      const row = { buffer: bytes, offset: retained.written,
        length: bytes.length - retained.written, position: retained.written,
        handle: retained.handle, called: false, returned: false };
      retained.writes.push(row);
      await step(original, 'fresh-file-write', () => {
        row.called = true;
        return row.handle.write(row.buffer, row.offset, row.length, row.position).then(value => {
          row.result = value; row.returned = true; return value;
        }, cause => { row.returned = true; row.error = cause; throw cause; });
      });
      if (!Number.isSafeInteger(row.result.bytesWritten) || row.result.bytesWritten <= 0 ||
        row.result.bytesWritten > row.length) throw failed(original, 'Retained invalid/zero-progress write');
      retained.written += row.result.bytesWritten;
    }
    await step(original, 'fresh-file-flush', () => {
      retained.flushAttempted = true;
      return retained.handle.sync().then(value => {
        retained.flushReturned = true; retained.flushValue = value; return value;
      }, cause => { retained.flushReturned = true; retained.flushError = cause; throw cause; });
    });
    retained.flushed = true;
  } catch (cause) { retained.failureObserved = true; retained.firstError = cause; first = cause; }
  const close = closeFileOriginal(retained);
  if (close) {
    try {
      const outcome = await step(original, 'fresh-file-close-observe', () => close);
      if (outcome.status === 'rejected') throw failed(original, 'Original write once-close rejected', outcome.cause);
      retained.closed = true;
    } catch (cause) { retained.closeObservationError = cause; first ??= cause; }
  }
  if (first) throw first;
}
/* Draft owner integration: timeout denies success and retains custody. It never claims natural exit,
 * original close, native retirement, bounded syscall return or safe cleanup.
 * Every chunk is copied into preallocated raw storage before interpretation;
 * an overflow chunk is retained explicitly and closes the successful gate. */
/* Retained build-plane working ownership, not native actor authority. The
 * admitted owning caller must retain this object and every pending row after
 * rejection. Promise timeout never cancels or completes a native operation. */
function failed(original, message, cause) {
  original.unresolved = true;
  const error = new Error(message);
  error.retainedBuildOriginal = original;
  if (arguments.length === 3) { error.causeObserved = true; error.cause = cause; }
  return error;
}
async function step(original, label, call) {
  if (original.unresolved || process.hrtime.bigint() >= original.deadline)
    throw failed(original, `Original build deadline/gate denied ${label}`);
  const row = { label, attempted: true, returned: false, rejected: false, result: null, error: null };
  original.rows.push(row);
  let native;
  try {
    native = Promise.resolve(call()).then(result => {
      row.result = result; row.returned = true; return { status: 'returned', result };
    }, error => { row.error = error; row.rejected = true; return { status: 'rejected', cause: error }; });
  } catch (error) {
    row.error = error; row.rejected = true;
    throw failed(original, `Synchronous build operation rejected: ${label}`, error);
  }
  row.pending = native;
  const remaining = original.deadline - process.hrtime.bigint();
  const millis = remaining > 0n ? Number((remaining + 999999n) / 1000000n) : 0;
  if (!Number.isSafeInteger(millis) || millis > 2147483647)
    throw failed(original, 'Unrepresentable SAME build deadline');
  let timer;
  const expired = new Promise(resolveExpiry => {
    timer = setTimeout(() => resolveExpiry({ status: 'expired' }), millis);
  });
  const outcome = await Promise.race([native, expired]);
  clearTimeout(timer);
  if (outcome.status === 'expired' || process.hrtime.bigint() >= original.deadline)
    throw failed(original, `Retained pending/late build operation ${label}`);
  if (outcome.status === 'rejected') throw failed(original, `Build operation rejected: ${label}`, outcome.cause);
  if (outcome.status !== 'returned') throw failed(original, `Invalid original operation outcome: ${label}`, outcome);
  return outcome.result;
}

export async function runRetainedSourceToolOriginal(original, executable, args, cwd, ordinal) {
  const { output, captureBytes } = original;
  demand(positive(captureBytes), 'Original build capture geometry unavailable');
  let scheduled = null;
  if (original.sourceCommandGeometry) {
    scheduled = original.sourceCommandGeometry.commands[ordinal - 1];
    demand(scheduled && scheduled.ordinal === ordinal && !scheduled.issued &&
      scheduled.executable === executable && ordinal === original.nextSourceCommand,
      'Source build command left its complete preowned schedule');
    scheduled.issued = true; scheduled.argsOriginal = args; scheduled.cwdOriginal = cwd;
    original.nextSourceCommand++;
  }
  let stdout = null, stderr = null;
  const facts = { ordinal, executable, args, cwd, birthAttempted: false, born: false,
    spawnError: null, spawnErrorObserved: false, exitObserved: false, exitCode: null, exitSignal: null,
    closeObserved: false, closeCode: null, closeSignal: null, stdoutBytes: 0, stderrBytes: 0,
    stdoutEnded: false, stderrEnded: false, overflow: [], writeError: null, writeErrorObserved: false,
    writeRows: [], stdoutError: null, stdoutErrorObserved: false,
    stderrError: null, stderrErrorObserved: false, unresolved: false };
  const retained = { facts, stdout, stderr, rawOut: null, rawErr: null, child: null, pendingWrites: null,
    captures: [
      { path: join(output, `command-${ordinal}.stdout.raw`), stream: 'stdout', handle: null },
      { path: join(output, `command-${ordinal}.stderr.raw`), stream: 'stderr', handle: null }
    ] };
  original.commands.push(retained);
  try {
  stdout = Buffer.alloc(captureBytes); retained.stdout = stdout;
  stderr = Buffer.alloc(captureBytes); retained.stderr = stderr;
  for (const capture of retained.captures) {
    await step(original, `${capture.stream}-open-${ordinal}`, () => {
      capture.openAttempted = true;
      return open(capture.path, 'wx').then(handle => {
        capture.handle = handle; capture.openReturned = true;
        if (retained.failureObserved) closeFileOriginal(capture);
        return handle;
      }, cause => { capture.openReturned = true; capture.openError = cause; throw cause; });
    });
  }
  retained.rawOut = retained.captures[0].handle;
  retained.rawErr = retained.captures[1].handle;
  let pendingWrites = Promise.resolve();
  function deny(message) { facts.unresolved = true; return failed(original, message); }
  function bytes(which, chunk) {
    const destination = which === 'stdout' ? stdout : stderr;
    const field = which === 'stdout' ? 'stdoutBytes' : 'stderrBytes';
    const offset = facts[field];
    if (!Buffer.isBuffer(chunk) || chunk.length === 0 || chunk.length > destination.length - offset) {
      const stream = which === 'stdout' ? retained.child.stdout : retained.child.stderr;
      stream.pause();
      /* At most one delivered overflow chunk per paused original stream. The
       * genuine native reader/chunk/backing bound is still unqualified. */
      if (!facts.overflow.some(row => row.stream === which)) facts.overflow.push({ stream: which, raw: chunk });
      deny(`Retained original ${which} capture overflow`); return;
    }
    chunk.copy(destination, offset); facts[field] += chunk.length;
    const raw = destination.subarray(offset, offset + chunk.length);
    const file = which === 'stdout' ? retained.rawOut : retained.rawErr;
    const row = { stream: which, offset, requested: raw.length, called: false,
      returned: false, bytesWritten: null, error: null };
    facts.writeRows.push(row);
    pendingWrites = pendingWrites.then(async () => {
      const result = await step(original, `${which}-write-${ordinal}`, () => {
        row.called = true;
        return file.write(raw).then(value => {
          row.returned = true; row.bytesWritten = value.bytesWritten; return value;
        }, cause => { row.returned = true; row.error = cause; throw cause; });
      });
      /* Real prefix is retained before the short/late outcome gate. A short
       * write denies complete receipt; no omitted suffix is reported complete. */
      if (result.bytesWritten !== raw.length) throw deny('Retained short raw write; suffix remains in original buffer');
    }).catch(error => {
      /* Later queued writes deny at the absorbing original gate. Preserve the
       * first actual writer cause instead of replacing it with that denial;
       * each attempted step still retains its independent result/error row. */
      if (!facts.writeErrorObserved) { facts.writeErrorObserved = true; facts.writeError = error; }
      deny('Retained raw writer failure');
    });
    retained.pendingWrites = pendingWrites;
  }
  let resolveClose;
  const completion = new Promise(resolve => { resolveClose = resolve; });
  retained.completion = completion;
  await step(original, `child-birth-${ordinal}`, () => {
    facts.birthAttempted = true;
    retained.child = spawn(executable, args,
      { cwd, env: original.environment, windowsHide: true, stdio: ['ignore', 'pipe', 'pipe'], shell: false });
    const child = retained.child;
    child.once('spawn', () => { facts.born = true; });
    child.stdout.on('data', chunk => bytes('stdout', chunk));
    child.stderr.on('data', chunk => bytes('stderr', chunk));
    child.stdout.once('error', error => { facts.stdoutErrorObserved = true; facts.stdoutError = error; deny('Original stdout error'); });
    child.stderr.once('error', error => { facts.stderrErrorObserved = true; facts.stderrError = error; deny('Original stderr error'); });
    child.stdout.once('end', () => { facts.stdoutEnded = true; });
    child.stderr.once('end', () => { facts.stderrEnded = true; });
    child.once('error', error => { facts.spawnErrorObserved = true; facts.spawnError = error; });
    child.once('exit', (code, signal) => { facts.exitObserved = true; facts.exitCode = code; facts.exitSignal = signal; });
    child.once('close', (code, signal) => {
      facts.closeObserved = true; facts.closeCode = code; facts.closeSignal = signal; resolveClose();
    });
    return retained.child;
  });
  retained.completion = completion;
  await step(original, `child-natural-close-${ordinal}`, () => completion);
  await step(original, `retained-raw-writes-${ordinal}`, () => pendingWrites);
  if (!(facts.born && facts.exitObserved && facts.closeObserved && facts.exitCode === 0 &&
    facts.closeCode === 0 && facts.exitSignal === null && facts.closeSignal === null &&
    facts.stdoutEnded && facts.stderrEnded && !facts.spawnErrorObserved && !facts.writeErrorObserved &&
    !facts.unresolved && !facts.stdoutErrorObserved && !facts.stderrErrorObserved && facts.overflow.length === 0))
    throw failed(original, 'Build command retained failure/unknown completion');
  await step(original, `stdout-flush-${ordinal}`, () => retained.rawOut.sync());
  await step(original, `stderr-flush-${ordinal}`, () => retained.rawErr.sync());
  /* Separate raw close outcomes. Failure preserves actual original closed/live facts and
   * all completed native facts; no finally masks a primary failure or cleanup. */
  const closes = retained.captures.map(capture => closeFileOriginal(capture));
  retained.closeObservations = await Promise.allSettled(closes.map((close, index) =>
    step(original, `${retained.captures[index].stream}-close-observe-${ordinal}`, () => close)));
  for (const observation of retained.closeObservations) {
    if (observation.status === 'rejected') throw observation.reason;
    if (observation.value.status === 'rejected')
      throw failed(original, 'Raw command capture once-close rejected', observation.value.cause);
  }
  await saveFresh(original, join(output, `command-${ordinal}.json`), Buffer.from(JSON.stringify(facts, null, 2)));
  if (scheduled) scheduled.completed = true;
  } catch (cause) {
    retained.failureObserved = true; retained.firstError = cause;
    // Both close requests start independently of the failed deadline gate.
    // Late open handlers take the same path; no child or pipe is disposed here.
    retained.failureClosePromises = retained.captures.map(capture => closeFileOriginal(capture));
    throw cause;
  }
}
const command = runRetainedSourceToolOriginal;

/* A distinct package caller/deadline, never a renewed or borrowed build scope.
 * Raw plan/tool byte associations remain unqualified native inputs. */
export async function preparePackageToolOwnerOriginal(raw, expected, output, platform, container) {
  demand(Buffer.isBuffer(raw) && /^[a-f0-9]{64}$/.test(expected) && digest(raw) === expected,
    'Package tool plan bytes differ');
  const plan = JSON.parse(raw.toString('utf8'));
  exact(plan, ['schema', 'milliseconds', 'captureBytes', 'environment', 'tools']);
  demand(plan.schema === 'service-lasso.cli39.package-tools.v1' &&
    positive(plan.milliseconds) && plan.milliseconds <= 2147483647 && positive(plan.captureBytes),
    'Missing finite package tool geometry');
  exact(plan.tools, platform === 'darwin' ? ['go', 'codesign'] : ['go']);
  demand(plan.environment && typeof plan.environment === 'object' && !Array.isArray(plan.environment) &&
    Object.entries(plan.environment).every(([name, value]) => name && !name.includes('=') &&
      !name.includes('\0') && typeof value === 'string' && !value.includes('\0')),
    'Invalid original package tool environment');
  const stages = platform === 'darwin' ?
    ['writer-build', 'darwin-helper-build', 'bundle', 'sea-blob', 'signature-remove', 'postject', 'signature-add', 'primary-build'] :
    ['writer-build', 'bundle', 'sea-blob', 'postject', 'primary-build'];
  demand(plan.captureBytes <= Math.floor(Number.MAX_SAFE_INTEGER / (2 * stages.length)),
    'Package capture schedule arithmetic overflow');
  const origin = process.hrtime.bigint();
  const original = { output: resolve(output), raw, plan, environment: { ...plan.environment },
    origin, previousTick: origin, deadline: origin + BigInt(plan.milliseconds) * 1000000n,
    captureBytes: plan.captureBytes, capturePayloadMaximum: 2 * stages.length * plan.captureBytes,
    rows: [], commands: [], files: [], unresolved: false, stages, next: 0,
    toolInputs: {}, nativeAuthority: 'UNKNOWN', nativeCaptureOverhead: 'UNKNOWN' };
  // Install the whole finite source schedule before any tool-input read/issue.
  original.invocations = stages.map(stage => ({ stage, attempted: false, returned: false,
    executable: null, args: null, cwd: null, inputOriginal: null, error: null }));
  container.toolOwner = original;
  for (const [role, input] of Object.entries(plan.tools)) original.toolInputs[role] = { input, body: null };
  for (const row of Object.values(original.toolInputs)) row.body = await regular(original, row.input);
  original.preparedAfter = process.hrtime.bigint();
  if (original.preparedAfter < origin || original.preparedAfter >= original.deadline)
    throw failed(original, 'Original package tool preparation expired');
  original.previousTick = original.preparedAfter;
  return original;
}
/* Exact source/archive/member recipe comes from separately admitted current
 * build inputs. Unknown member identity or source flags denies before compiler
 * birth. This code cannot manufacture the missing authentic association map. */
export async function buildSourceRuntime(manifestBytes, expectedManifestSHA256, output) {
  let original = null;
  try {
  demand(producerComplete, 'SOURCE-UNRUN unfinished producer: no native effects admitted');
  demand(/^[a-f0-9]{64}$/.test(expectedManifestSHA256) && digest(manifestBytes) === expectedManifestSHA256,
    'Current admitted manifest bytes differ');
  const plan = JSON.parse(manifestBytes.toString('utf8'));
  exact(plan, ['schema', 'sourceCommit', 'platform', 'configuration', 'nodeTree', 'tools', 'archives',
    'overlayInputs', 'nodeOverlayAssociation', 'nodeOverlayInputs', 'nativeSourceInputs', 'environmentInput', 'buildOutputs',
    'buildMilliseconds', 'captureBytes', 'outputParent', 'outputLeaf', 'packageToolInputs']);
  demand(plan.schema === 'service-lasso.cli39.source-build-recipe.v3' &&
    plan.sourceCommit === 'aa4c77582be995286fc6e00aaf530dc7ade102a9' && plan.platform === 'win32-x64' &&
    plan.configuration === 'Release-nonshared-full-icu-default-v8-workers-MT', 'Wrong selected full runtime profile');
  demand(positive(plan.buildMilliseconds) && plan.buildMilliseconds <= 2147483647 &&
    positive(plan.captureBytes) && leaf(plan.outputLeaf) && isAbsolute(plan.outputParent), 'Invalid original build geometry');
  original = { output: resolve(output), deadline: process.hrtime.bigint() + BigInt(plan.buildMilliseconds) * 1000000n,
    captureBytes: plan.captureBytes, rows: [], commands: [], files: [], unresolved: false };
  exact(plan.tools, ['compiler', 'librarian', 'cmd']);
  original.sourceCommandGeometry = sourceCommandGeometry(plan);
  original.nextSourceCommand = 1;
  await sourceTree(original, plan.nodeTree, plan.sourceCommit);
  await nodeOverlayRelations(original, plan.nodeOverlayAssociation, plan.nodeOverlayInputs, plan.sourceCommit);
  await nativeSourceInputs(original, plan.nativeSourceInputs);
  await buildEnvironment(original, plan.environmentInput);
  buildOutputRecipe(original, plan.buildOutputs);
  exact(plan.tools, ['compiler', 'librarian', 'cmd']);
  for (const input of Object.values(plan.tools)) await regular(original, input);
  await packageToolInputs(original, plan.packageToolInputs);
  demand(Array.isArray(plan.archives) && plan.archives.length === 4 && Array.isArray(plan.overlayInputs), 'Incomplete archive recipe');
  const seenSources = new Set();
  for (const input of plan.overlayInputs) {
    await regular(original, input); demand(!seenSources.has(input.path), 'Duplicate overlay source'); seenSources.add(input.path);
  }
  for (let index = 0; index < families.length; index++) {
    const archive = plan.archives[index];
    exact(archive, ['family', 'original', 'memberAssociation', 'replacements']);
    demand(archive.family === families[index] && Array.isArray(archive.replacements), 'Wrong complete archive family');
    const archiveRaw = await regular(original, archive.original);
    const actualMembers = readCoffArchiveMembers(archiveRaw);
    original.rows.push({ label: 'original-archive-structure', returned: true, result: actualMembers });
    await regular(original, archive.memberAssociation);
    const map = JSON.parse((await regular(original, archive.memberAssociation)).toString('utf8'));
    exact(map, ['schema', 'archiveSHA256', 'members']);
    demand(map.schema === 'service-lasso.cli39.qualified-crt-members.v1' &&
      map.archiveSHA256 === archive.original.sha256 && Array.isArray(map.members) && map.members.length > 0,
      'Missing exact original archive/member ancestry');
    const members = new Set();
    demand(map.members.length === actualMembers.members.length, 'Incomplete original archive member inventory');
    for (let memberIndex = 0; memberIndex < map.members.length; memberIndex++) {
      const member = map.members[memberIndex], actual = actualMembers.members[memberIndex];
      exact(member, ['headerOffset', 'name', 'bytes', 'sha256', 'sourceAssociation']);
      demand(member.headerOffset === actual.headerOffset && member.name === actual.name &&
        member.bytes === actual.body.length && digest(actual.body) === member.sha256,
        'Wrong actual original member/source association');
      member.retainedSourceRelation = await memberAssociation(original, member.sourceAssociation,
        archive, member, plan.tools.compiler);
      members.add(member.headerOffset);
    }
    const replaced = new Set();
    for (const replacement of archive.replacements) {
      exact(replacement, ['memberOffset', 'member', 'originalSource', 'source', 'flags', 'includeInputs']);
      const actual = actualMembers.members.find(member => member.headerOffset === replacement.memberOffset);
      demand(members.has(replacement.memberOffset) && actual?.name === replacement.member &&
        leaf(replacement.member) && actualMembers.members.filter(member => member.name === replacement.member).length === 1 &&
        !replaced.has(replacement.memberOffset) &&
        seenSources.has(replacement.source.path) && Array.isArray(replacement.flags) &&
        replacement.flags.includes('/MT') && replacement.flags.includes('/c') &&
        replacement.flags.every(flag => typeof flag === 'string' && !(flag.startsWith('/') && /^(Fo|Fe|Fd|Fa|FI|link|LD|MD)/i.test(flag.slice(1)))),
        'Guessed member or incompatible original compilation flags');
      await regular(original, replacement.source);
      await regular(original, replacement.originalSource);
      demand(Array.isArray(replacement.includeInputs), 'Missing original transitive headers');
      for (const input of replacement.includeInputs) await regular(original, input);
      const relation = map.members.find(member => member.headerOffset === replacement.memberOffset).retainedSourceRelation.body;
      demand(sameInput(relation.source, replacement.originalSource) &&
        relation.flags.length === replacement.flags.length &&
        relation.flags.every((flag, flagIndex) => flag === replacement.flags[flagIndex]) &&
        relation.includeInputs.length === replacement.includeInputs.length &&
        relation.includeInputs.every((input, inputIndex) => sameInput(input, replacement.includeInputs[inputIndex])),
        'Changed original compiler argv/header relation requires independent recipe/source admission');
      replaced.add(replacement.memberOffset);
    }
    original.archives ??= [];
    original.archives.push({ archive, raw: archiveRaw, actualMembers });
  }
  await parents(original, plan.outputParent);
  const wanted = join(resolve(plan.outputParent), plan.outputLeaf);
  demand(resolve(output) === wanted && relative(plan.outputParent, wanted) === plan.outputLeaf &&
    !plan.outputLeaf.includes(sep), 'Output differs from exact fresh owned destination');
  /* No rm, clean, recovery or replacement: an existing destination rejects. */
  await step(original, 'fresh-output-create', () => mkdir(wanted, { recursive: false }));
  await saveFresh(original, join(wanted, 'INPUT-MANIFEST.raw.json'), manifestBytes);
  /* This layout joins the source-owned node.gyp relative archive directory.
   * Every directory is a new retained effect; no existing tree is replaced. */
  const nativeDirectory = join(wanted, 'native');
  const crtDirectory = join(nativeDirectory, 'crt-runtime');
  const crtBuildDirectory = join(crtDirectory, 'build');
  const crtArchitectureDirectory = join(crtBuildDirectory, 'x64');
  const crtReleaseDirectory = join(crtArchitectureDirectory, 'Release');
  for (const directory of [nativeDirectory, crtDirectory, crtBuildDirectory, crtArchitectureDirectory, crtReleaseDirectory])
    await step(original, 'fresh-crt-install-directory', () => mkdir(directory, { recursive: false }));
  original.crtInstall = { directory: crtReleaseDirectory, archives: [],
    namespaceAuthority: 'UNSUPPLIED', sourceABIAdmission: 'UNSUPPLIED' };
  await stageNodeTree(original, nativeDirectory);
  await stageNativeSources(original, nativeDirectory);
  let ordinal = 0;
  /* The API contract names Kernelbase.dll; an installed SDK import library
   * is not assumed. Build the exact admitted definition with the retained
   * original librarian, in this fresh owned output namespace. This establishes
   * an import artifact, not loaded-module/OS/native source qualification. */
  original.nativeImports = [];
  for (const family of ['kernelbase', 'ntdll']) {
  const definitionName = `${family}-preentry.def`;
  const importDefinition = original.nativeSourceInputs.get(`host-service/${definitionName}`);
  demand(importDefinition, 'Missing original native import definition');
  const importPath = join(crtReleaseDirectory, `slcli-${family}.lib`);
  const retainedImport = { family, definition: importDefinition, path: importPath,
    emitted: false, library: null, exportFile: null, moduleAuthority: 'UNSUPPLIED' };
  original.nativeImports.push(retainedImport);
  if (family === 'kernelbase') original.preentryImport = retainedImport;
  retainedImport.libraryReadOriginal = { path: importPath, body: null };
  retainedImport.exportReadOriginal = { path: join(crtReleaseDirectory, `slcli-${family}.exp`), body: null };
  await command(original, plan.tools.librarian.path,
    [`/def:${join(nativeDirectory, 'host-service', definitionName)}`,
      '/machine:x64', `/out:${importPath}`], wanted, ++ordinal);
  retainedImport.emitted = true;
  /* Both output observations are independently eligible at this point. Start
   * them before awaiting either so a first read failure cannot suppress the
   * other original. Each step retains pending/late/raw results independently. */
  retainedImport.reads = await Promise.allSettled([
    readFileOriginal(original, retainedImport.libraryReadOriginal, 'preentry-import-library'),
    readFileOriginal(original, retainedImport.exportReadOriginal, 'preentry-import-export'),
  ]);
  const [libraryRead, exportRead] = retainedImport.reads;
  if (libraryRead.status === 'fulfilled') retainedImport.library = libraryRead.value;
  if (exportRead.status === 'fulfilled') retainedImport.exportFile = exportRead.value;
  if (libraryRead.status === 'rejected' || exportRead.status === 'rejected')
    throw failed(original, 'Retained generated preentry output read failures',
      libraryRead.status === 'rejected' ? libraryRead.reason : exportRead.reason);
  demand(retainedImport.library.length > 0 && retainedImport.exportFile.length > 0,
    'Missing complete generated preentry import/export originals');
  }
  for (const retainedArchive of original.archives) {
    const { archive } = retainedArchive;
    const archiveOutput = join(wanted, `slcli-${archive.family}.lib`);
    /* Emit the already retained original byte vector, never reacquire a mutable
     * original pathname and label its potentially different bytes as this vector. */
    await saveFresh(original, archiveOutput, retainedArchive.raw);
    const objectDirectory = join(wanted, `${archive.family}-objects`);
    await step(original, 'fresh-member-output-directory', () => mkdir(objectDirectory, { recursive: false }));
    let frontierPath = archiveOutput, frontier = retainedArchive.actualMembers;
    for (const replacement of archive.replacements) {
      const object = join(objectDirectory, replacement.member);
      const compiledOriginal = { path: object, body: null, replacement };
      retainedArchive.compiledMemberOriginals ??= [];
      retainedArchive.compiledMemberOriginals.push(compiledOriginal);
      await command(original, plan.tools.compiler.path,
        [...replacement.flags, `/Fo${object}`, replacement.source.path], wanted, ++ordinal);
      const compiled = await readFileOriginal(original, compiledOriginal, 'compiled-member');
      const actualMember = frontier.members.find(member => member.name === replacement.member);
      demand(actualMember, 'Original replacement member left frontier');
      const successorPath = `${archiveOutput}.step-${ordinal + 1}`;
      const successorOriginal = { path: successorPath, body: null, predecessor: frontier, replacementOriginal: compiledOriginal };
      retainedArchive.successorOriginals ??= [];
      retainedArchive.successorOriginals.push(successorOriginal);
      await command(original, plan.tools.librarian.path,
        [`/out:${successorPath}`, `/remove:${replacement.member}`, frontierPath, object], wanted, ++ordinal);
      const successorRaw = await readFileOriginal(original, successorOriginal, 'archive-successor');
      frontier = verifySuccessor(original, frontier, successorRaw, actualMember.headerOffset, compiled);
      frontierPath = successorPath;
      /* Each complete predecessor remains: installation of a later archive
       * frontier is separate qualified ownership, never in-place deletion. */
    }
    retainedArchive.final = { path: frontierPath, body: frontier.raw, sha256: digest(frontier.raw) };
    const installedPath = join(crtReleaseDirectory, `slcli-${archive.family}.lib`);
    const installed = { family: archive.family, predecessor: retainedArchive.final,
      path: installedPath, body: frontier.raw, emitted: false, verified: false,
      readbackOriginal: { path: installedPath, body: null } };
    original.crtInstall.archives.push(installed);
    await saveFresh(original, installedPath, installed.body);
    installed.emitted = true;
    const installedRaw = await readFileOriginal(original, installed.readbackOriginal, 'installed-archive');
    installed.readback = installedRaw;
    demand(installedRaw.equals(installed.body), 'Installed complete archive differs from retained final frontier');
    installed.verified = true;
    /* Equality is byte-lineage only. Genuine held original path/namespace and
     * tool/ABI/loaded image authority must still be joined before execution. */
  }
  /* The fixed original build entry keeps full ICU, ordinary V8/default
   * workers, static Release CRT and the original generator/dependency graph.
   * It is not package/test/upload/sign and receives no caller override flags. */
  const invocation = nodeBuildInvocation(original, plan.tools.cmd);
  await command(original, invocation.commandInput.path, invocation.generateArgs, invocation.cwd, ++ordinal);
  invocation.configurationOriginal = await generatedNodeConfiguration(original);
  await command(original, invocation.commandInput.path, invocation.args, invocation.cwd, ++ordinal);
  invocation.configurationReadback = await generatedNodeConfiguration(original, invocation.configurationOriginal);
  demand(original.nextSourceCommand === original.sourceCommandGeometry.commands.length + 1 &&
    original.sourceCommandGeometry.commands.every(row => row.issued && row.completed),
    'Incomplete actual full source command closure');
  await retainBuildOutputs(original);
  throw failed(original, 'Full original generator/tool/preentry/configuration/image ancestry and installed producer integration remain unfinished');
  } catch (error) {
    if (original) throw failed(original, 'Source build retained failure', error);
    throw error;
  }
}
