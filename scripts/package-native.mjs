import { copyFile, mkdir, open, readFile, readdir, writeFile } from "node:fs/promises";
import { createHash } from "node:crypto";
import { gzipSync } from "node:zlib";
import { fileURLToPath } from "node:url";
import { dirname, join, resolve } from "node:path";
import { stagePrimaryGate } from "./stage-primary-gate.mjs";
import { nativeLifecycleDiagnostics } from "./native-lifecycle-diagnostics.mjs";
import { buildSourceRuntime, preparePackageToolOwnerOriginal, runRetainedSourceToolOriginal } from "./build-source-runtime.mjs";

const observe = nativeLifecycleDiagnostics(process.argv.includes("--diagnose-native-lifecycle"));
async function stage(label, operation) {
  observe(label, "start");
  try { const result = await operation(); observe(label, "complete"); return result; }
  catch (error) { observe(label, "failed"); throw error; }
}
const nodeVersion = "22.23.2";
const postjectVersion = "1.0.0-alpha.6";
const esbuildVersion = "0.28.2";
const fuse = "NODE_SEA_FUSE_fce680ab2cc467b6e072b8b5df1996b2";
const root = resolve(dirname(fileURLToPath(import.meta.url)), "..");

// This occurrence remains reachable on a producer failure. SAME-handle read
// association is byte custody only, not held namespace or build-tool admission.
const sourcePackageOriginal = { manifest: null, build: null, producer: null, toolOwner: null,
  toolManifest: null, commands: [], imageWrite: null };
async function writeSourceImageOriginal(path, image) {
  const row = { path, image, body: image.body, handle: null, writes: [], written: 0,
    openAttempted: false, openReturned: false, syncAttempted: false, syncReturned: false,
    closeAttempted: false, closeReturned: false, error: null, closeError: null };
  sourcePackageOriginal.imageWrite = row;
  try {
    if (!Buffer.isBuffer(row.body) || !Number.isSafeInteger(row.body.length) || row.body.length <= 0)
      throw new Error("Missing retained source image bytes");
    // At least one byte advances each successful attempt. One failed attempt
    // is absorbing; this finite source bound is not a qualified capture budget.
    row.writeAttemptMaximum = row.body.length;
    row.openAttempted = true;
    row.handle = await open(path, "wx"); row.openReturned = true;
    while (row.written < row.body.length) {
      if (row.writes.length >= row.writeAttemptMaximum)
        throw new Error("Source image write schedule exhausted");
      const write = { offset: row.written, requested: row.body.length - row.written,
        result: null, returned: false, error: null };
      row.writes.push(write);
      try {
        write.result = await row.handle.write(row.body, write.offset, write.requested, write.offset);
        write.returned = true;
      } catch (error) { write.error = error; throw error; }
      if (write.result.buffer !== row.body || !Number.isSafeInteger(write.result.bytesWritten) ||
          write.result.bytesWritten <= 0 || write.result.bytesWritten > write.requested)
        throw new Error("Invalid source-image write occurrence");
      row.written += write.result.bytesWritten;
    }
    row.syncAttempted = true;
    await row.handle.sync(); row.syncReturned = true;
  } catch (error) { row.error = error; }
  if (row.handle) {
    row.closeAttempted = true;
    try { await row.handle.close(); row.closeReturned = true; }
    catch (error) { row.closeError = error; }
  }
  if (row.error || row.closeError) {
    const failure = new Error("Source image output retained failure", { cause: row.error ?? row.closeError });
    failure.sourcePackageOriginal = sourcePackageOriginal;
    throw failure;
  }
  return row;
}
async function sourcePackageProducer() {
  const row = { path: resolve(argument("--source-runtime-manifest")), handle: null,
    before: null, body: null, after: null, closeAttempted: false, closeReturned: false,
    error: null, closeError: null };
  sourcePackageOriginal.manifest = row;
  try {
    try {
      row.handle = await open(row.path, "r");
      row.before = await row.handle.stat({ bigint: true });
      row.body = await row.handle.readFile();
      row.after = await row.handle.stat({ bigint: true });
      if (!row.before.isFile() || !row.after.isFile() ||
          row.before.dev !== row.after.dev || row.before.ino !== row.after.ino ||
          row.before.size !== BigInt(row.body.length) || row.after.size !== row.before.size ||
          row.before.mtimeNs !== row.after.mtimeNs || row.before.ctimeNs !== row.after.ctimeNs)
        throw new Error("Source-runtime manifest read occurrence changed");
    } catch (error) { row.error = error; }
    if (row.handle) {
      row.closeAttempted = true;
      try { await row.handle.close(); row.closeReturned = true; }
      catch (error) { row.closeError = error; }
    }
    if (row.error) throw row.error;
    if (row.closeError) throw row.closeError;
    // Manifest arguments route the independently admitted build input. They
    // cannot turn bytes/hash/path equality into executable authority.
    sourcePackageOriginal.build = await buildSourceRuntime(row.body,
      argument("--source-runtime-manifest-sha256"), join(output, ".source-runtime-build"));
    const producer = sourcePackageOriginal.build?.packageProducerOriginals;
    if (!producer || producer.buildOriginal !== sourcePackageOriginal.build ||
        producer.productionImage !== sourcePackageOriginal.build.builtOutputs?.find(
          item => item.recipe.target === "node-core" && item.recipe.kind === "image") ||
        producer.buildToolNode !== sourcePackageOriginal.build.packageToolInputs?.node ||
        producer.esbuild !== sourcePackageOriginal.build.packageToolInputs?.esbuild ||
        producer.postject !== sourcePackageOriginal.build.packageToolInputs?.postject)
      throw new Error("Missing SAME source-build package producer originals");
    sourcePackageOriginal.producer = producer;
    return producer;
  } catch (error) {
    sourcePackageOriginal.failure = error;
    const failure = new Error("Source package producer failed", { cause: error });
    failure.sourcePackageOriginal = sourcePackageOriginal;
    throw failure;
  }
}

function argument(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing required ${name} argument.`);
  return process.argv[index + 1];
}

function currentTarget() {
  const target = `${process.platform}-${process.arch}`;
  const supported = new Set(["win32-x64", "linux-x64", "darwin-arm64"]);
  if (!supported.has(target)) throw new Error(`Unsupported native SEA target ${target}; expected Windows x64, Linux x64, or macOS arm64.`);
  return target;
}

async function command(stage, file, args, cwd = root, inputOriginal = null) {
  const owner = sourcePackageOriginal.toolOwner;
  if (!owner || owner.unresolved || owner.next >= owner.invocations.length ||
      owner.invocations[owner.next].stage !== stage) throw new Error("Package tool schedule denied");
  const ordinal = owner.next++, row = owner.invocations[ordinal];
  row.attempted = true; row.executable = file; row.args = args; row.cwd = cwd; row.inputOriginal = inputOriginal;
  sourcePackageOriginal.commands.push(row);
  const expires = owner.deadline, origin = owner.origin;
  row.before = process.hrtime.bigint();
  try {
    if (row.before < owner.previousTick || row.before >= expires)
      throw new Error("Original package tool deadline denied");
    await runRetainedSourceToolOriginal(owner, file, args, cwd, ordinal);
    row.returned = true;
    row.after = process.hrtime.bigint();
    if (owner.deadline !== expires || owner.origin !== origin || row.after < row.before || row.after >= expires)
      throw new Error("Original package tool final clock denied");
    owner.previousTick = row.after;
  } catch (error) {
    row.error = error; owner.unresolved = true;
    error.sourcePackageOriginal = sourcePackageOriginal;
    throw error;
  }
}

async function packageToolOwner() {
  const row = { path: resolve(argument("--package-tool-manifest")), handle: null,
    before: null, after: null, body: null, error: null, closeError: null, closeAttempted: false, closeReturned: false };
  sourcePackageOriginal.toolManifest = row;
  try {
    row.handle = await open(row.path, "r"); row.before = await row.handle.stat({ bigint: true });
    row.body = await row.handle.readFile(); row.after = await row.handle.stat({ bigint: true });
    if (!row.before.isFile() || !row.after.isFile() || row.before.dev !== row.after.dev ||
        row.before.ino !== row.after.ino || row.before.size !== BigInt(row.body.length) ||
        row.before.size !== row.after.size || row.before.mtimeNs !== row.after.mtimeNs ||
        row.before.ctimeNs !== row.after.ctimeNs) throw new Error("Package tool manifest occurrence changed");
  } catch (error) { row.error = error; }
  if (row.handle) {
    row.closeAttempted = true;
    try { await row.handle.close(); row.closeReturned = true; } catch (error) { row.closeError = error; }
  }
  if (row.error || row.closeError) {
    const failure = new Error("Package tool plan read failed", { cause: row.error ?? row.closeError });
    failure.sourcePackageOriginal = sourcePackageOriginal; throw failure;
  }
  try {
    return await preparePackageToolOwnerOriginal(row.body, argument("--package-tool-manifest-sha256"),
      output, process.platform, sourcePackageOriginal);
  } catch (error) {
    error.sourcePackageOriginal = sourcePackageOriginal; throw error;
  }
}

function sha256(bytes) {
  return createHash("sha256").update(bytes).digest("hex");
}

function controlledFixtureArchive(payload) {
  const blocks = [];
  for (const [path, value] of Object.entries(payload)) {
    const bytes = Buffer.from(value), header = Buffer.alloc(512);
    Buffer.from(path).copy(header);
    Buffer.from("0000644\0").copy(header, 100);
    Buffer.from(`${bytes.length.toString(8).padStart(11, "0")}\0`).copy(header, 124);
    header[156] = 48;
    blocks.push(header, bytes, Buffer.alloc((512 - bytes.length % 512) % 512));
  }
  return gzipSync(Buffer.concat([...blocks, Buffer.alloc(1024)]));
}

async function confinedWriterSourceSha256() {
  const directory = join(root, "native", "confined-scaffold");
  const names = (await readdir(directory)).filter((name) => name.endsWith(".go") || name === "go.mod" || name === "go.sum").sort();
  const contents = [];
  for (const name of names) contents.push(Buffer.from(`${name}\0`, "utf8"), await readFile(join(directory, name)), Buffer.from("\0", "utf8"));
  return sha256(Buffer.concat(contents));
}

async function darwinHelperSourceSha256() {
  const directory = join(root, "native", "darwin-immutable-helper");
  const names = (await readdir(directory)).filter((name) => name.endsWith(".go") || name === "go.mod" || name === "go.sum").sort();
  const contents = [];
  for (const name of names) contents.push(Buffer.from(`${name}\0`, "utf8"), await readFile(join(directory, name)), Buffer.from("\0", "utf8"));
  return sha256(Buffer.concat(contents));
}

if (process.versions.node !== nodeVersion) throw new Error(`Node ${nodeVersion} is required; found ${process.versions.node}.`);

const output = resolve(argument("--output"));
const sourceSha = argument("--source-sha");
const version = argument("--version");
const controlledTestAdmission = process.argv.includes("--controlled-test-admission");
if (!/^[0-9a-f]{40}$/i.test(sourceSha)) throw new Error("--source-sha must be a full 40-character Git SHA.");
if (!/^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i.test(version)) throw new Error("--version must be the frozen candidate version.");
const target = currentTarget();
const executableName = process.platform === "win32" ? "service-lassoctl.exe" : "service-lassoctl";
const confinedWriterName = process.platform === "win32" ? "service-lasso-confined-scaffold.exe" : "service-lasso-confined-scaffold";
const embeddedSeaName = process.platform === "win32" ? "service-lassoctl.sea.exe" : "service-lassoctl.sea";
const bundle = join(output, "service-lassoctl.cjs");
const blob = join(output, "service-lassoctl.blob");
const executable = join(output, executableName);
const confinedWriter = join(output, confinedWriterName);
const darwinHelperName = "service-lasso-darwin-immutable-helper";
const darwinHelper = join(output, darwinHelperName);
const embeddedSea = join(output, embeddedSeaName);
const seaConfig = join(output, "sea-config.json");

// A failed or previous attempt remains owned evidence. Never erase it to make
// the next attempt appear fresh.
await mkdir(output, { recursive: false });
// Both source-build denials remain absorbing. No ambient image/tool fallback
// runs after a failed or unfinished source producer.
const sourceProducerOriginal = await stage("source-runtime-build", sourcePackageProducer);
const buildToolNodeOriginal = sourceProducerOriginal.buildToolNode.image;
const sourceRuntimeImageOriginal = sourceProducerOriginal.productionImage;
const esbuildNativeOriginal = sourceProducerOriginal.esbuild.nativeImage;
const postjectCLIOriginal = sourceProducerOriginal.postject.cli;
// Loaded module/tool/source/namespace ancestry must be qualified by the real
// build owner before this route is executable; these object joins do not grant it.
const packageTools = await packageToolOwner();
if (packageTools.environment.NODE_PATH !== join(root, "node_modules")) {
  packageTools.unresolved = true;
  const failure = new Error("Package tool environment must retain the original bundle nodePaths relation");
  failure.sourcePackageOriginal = sourcePackageOriginal; throw failure;
}
const goOriginal = packageTools.toolInputs.go;
await command("writer-build", goOriginal.input.path, ["build", "-trimpath", "-o", confinedWriter, "."], join(root, "native", "confined-scaffold"), goOriginal);
const confinedWriterSha256 = sha256(await readFile(confinedWriter));
let darwinHelperSha256 = null;
if (process.platform === "darwin") {
  await command("darwin-helper-build", goOriginal.input.path, ["build", "-trimpath", "-o", darwinHelper, "."], join(root, "native", "darwin-immutable-helper"), goOriginal);
  darwinHelperSha256 = sha256(await readFile(darwinHelper));
}
// This fixed tuple is solely an executable integration fixture. It is never
// present in a normal candidate and cannot be supplied by a caller at runtime.
const controlledPayload = { "service.json": '{"id":"controlled-primary"}\n', "config/example.env": "PORT=8080\n" };
const controlledInventory = Object.entries(controlledPayload).map(([path, value]) => ({ path, sha256: sha256(value), mode: "0644", bytes: Buffer.byteLength(value) }));
const controlledContract = Buffer.from(`${JSON.stringify({ schemaVersion: 1, contractDigest: "2".repeat(64), inventory: controlledInventory })}\n`);
const controlledArchive = controlledFixtureArchive(controlledPayload);
const controlledAdmissions = controlledTestAdmission ? [{
  repository: "service-lasso/service-template",
  tag: `template-v9.9.9-${"1".repeat(40)}`,
  commit: "1".repeat(40),
  templateVersion: "9.9.9",
  contractDigest: "2".repeat(64),
  contractSha256: sha256(controlledContract),
  archiveSha256: sha256(controlledArchive),
  catalogIdentity: "controlled-source-test-fixture",
}] : [];
const bundleDefines = {
    "process.env.SERVICE_LASSO_CANDIDATE_VERSION": JSON.stringify(version),
    "process.env.SERVICE_LASSO_CANDIDATE_SOURCE_SHA": JSON.stringify(sourceSha),
    __SERVICE_LASSO_CANDIDATE_VERSION__: JSON.stringify(version),
    __SERVICE_LASSO_CANDIDATE_SOURCE_SHA__: JSON.stringify(sourceSha),
    __SERVICE_LASSO_CONFINED_HELPER_SHA256__: JSON.stringify(confinedWriterSha256),
    __SERVICE_LASSO_CONTROLLED_TEST_ADMISSIONS__: JSON.stringify(controlledAdmissions),
};
await command("bundle", esbuildNativeOriginal.input.path,
  [join(root, "dist", "sea-entry.js"), "--bundle", "--format=cjs", `--outfile=${bundle}`,
    "--platform=node", "--target=node22", "--legal-comments=none",
    `--alias:commander=${join(root, "node_modules", "commander", "index.js")}`,
    ...Object.entries(bundleDefines).map(([name, value]) => `--define:${name}=${value}`)], root, esbuildNativeOriginal);
await writeFile(seaConfig, `${JSON.stringify({ main: bundle, output: blob, disableExperimentalSEAWarning: true, useCodeCache: false, execArgvExtension: "none" }, null, 2)}\n`);
await command("sea-blob", buildToolNodeOriginal.input.path, ["--experimental-sea-config", seaConfig], root, buildToolNodeOriginal);
// Consume the retained source-producer bytes, rather than reopening its path.
// Fresh output/write/native identity and derivative ancestry still require the
// actual build owner; write success alone supplies no executable admission.
await writeSourceImageOriginal(executable, sourceRuntimeImageOriginal);
if (process.platform === "darwin") await command("signature-remove", packageTools.toolInputs.codesign.input.path, ["--remove-signature", executable], root, packageTools.toolInputs.codesign);
const postject = postjectCLIOriginal.input.path;
const postjectArgs = [executable, "NODE_SEA_BLOB", blob, "--sentinel-fuse", fuse];
if (process.platform === "darwin") postjectArgs.push("--macho-segment-name", "NODE_SEA");
await command("postject", buildToolNodeOriginal.input.path, [postject, ...postjectArgs], root,
  { node: buildToolNodeOriginal, cli: postjectCLIOriginal });
if (process.platform === "darwin") await command("signature-add", packageTools.toolInputs.codesign.input.path, ["--sign", "-", executable], root, packageTools.toolInputs.codesign);

// The published command is a compiled resident primary.  It embeds both
// target-host images, so a normal invocation never chooses a helper from a
// mutable archive neighbour.  Keep the checked writer alongside the archive
// as an auditable member; it is not executable authority for the primary.
await copyFile(executable, embeddedSea);
const gateSource = join(root, "native", "primary-gate");
const gateBuild = join(output, ".primary-gate-build");
const primaryStageOriginal = await stage("primary-stage", () => stagePrimaryGate({ source: gateSource, build: gateBuild, sea: embeddedSea, writer: confinedWriter, platform: process.platform, darwinHelperSha256 }));
if (!primaryStageOriginal.complete || primaryStageOriginal.build !== gateBuild ||
    primaryStageOriginal.sea !== embeddedSea || primaryStageOriginal.writer !== confinedWriter ||
    primaryStageOriginal.files.some((file) => !file.complete))
  throw new Error("Primary build requires the complete original staging occurrence");
// Keep this exact occurrence through provenance. Its byte observations do not
// supply held namespace, compiler/tool ancestry or executable authority.
const primaryStagingEvidence = {
  schema: "service-lasso.cli39.primary-staging-byte-observations.v1",
  scope: "byte-lineage-only",
  files: primaryStageOriginal.files.map((file) => ({
    name: file.path.slice(gateBuild.length + 1).replaceAll("\\", "/"),
    bytes: file.body.length, sha256: sha256(file.body),
  })),
};
// Required producer evidence is outside the closed publication archive. The
// schema1 candidate provenance below is not silently widened by this record.
await writeFile(join(output, "primary-staging-original.json"), `${JSON.stringify(primaryStagingEvidence, null, 2)}\n`, { flag: "wx" });
await command("primary-build", goOriginal.input.path, ["build", "-trimpath", "-ldflags", `-X main.nativeCandidateVersion=${version} -X main.nativeCandidateSourceSHA=${sourceSha}`, "-o", executable, "."], gateBuild, goOriginal);
if (packageTools.next !== packageTools.invocations.length || packageTools.unresolved ||
    packageTools.invocations.some(row => !row.returned)) {
  packageTools.unresolved = true;
  const failure = new Error("Incomplete original package tool schedule");
  failure.sourcePackageOriginal = sourcePackageOriginal; throw failure;
}
packageTools.finish = process.hrtime.bigint();
if (packageTools.finish < packageTools.previousTick || packageTools.finish >= packageTools.deadline) {
  packageTools.unresolved = true;
  const failure = new Error("Original package tool schedule expired");
  failure.sourcePackageOriginal = sourcePackageOriginal; throw failure;
}

const provenance = {
  schemaVersion: 1,
  command: "service-lassoctl",
  candidate: { version, tag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}` },
  source: { commit: sourceSha },
  executable: { name: executableName, sha256: sha256(await readFile(executable)), platform: process.platform, architecture: process.arch, version },
  confinedWriter: { name: confinedWriterName, sha256: confinedWriterSha256, sourceSha256: await confinedWriterSourceSha256(), platform: process.platform, architecture: process.arch },
  ...(process.platform === "darwin" ? { darwinImmutableHelper: { name: darwinHelperName, sha256: darwinHelperSha256, sourceSha256: await darwinHelperSourceSha256(), platform: process.platform, architecture: process.arch } } : {}),
  tools: { node: nodeVersion, esbuild: esbuildVersion, postject: postjectVersion },
  sea: { mainFormat: "commonjs", useCodeCache: false, execArgvExtension: "none" },
};
await stage("provenance", () => writeFile(join(output, "provenance.json"), `${JSON.stringify(provenance, null, 2)}\n`));
process.stdout.write(`${JSON.stringify(provenance)}\n`);
