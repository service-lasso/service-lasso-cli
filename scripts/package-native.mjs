import { build } from "esbuild";
import { copyFile, mkdir, readFile, readdir, rm, writeFile } from "node:fs/promises";
import { spawnSync } from "node:child_process";
import { createHash } from "node:crypto";
import { gzipSync } from "node:zlib";
import { fileURLToPath } from "node:url";
import { basename, dirname, join, resolve } from "node:path";
import { stagePrimaryGate } from "./stage-primary-gate.mjs";
import { nativeLifecycleDiagnostics } from "./native-lifecycle-diagnostics.mjs";

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

function command(stage, file, args, cwd = root) {
  observe(stage, "start");
  const result = spawnSync(file, args, { cwd, encoding: "utf8" });
  observe(stage, result.status === 0 && !result.error ? "complete" : "failed", result.status, result.signal);
  if (result.status !== 0 || result.error) throw new Error(`${basename(file)} failed: ${result.error?.message ?? result.stderr ?? result.stdout}`);
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

await rm(output, { recursive: true, force: true });
await mkdir(output, { recursive: true });
command("writer-build", "go", ["build", "-trimpath", "-o", confinedWriter, "."], join(root, "native", "confined-scaffold"));
const confinedWriterSha256 = sha256(await readFile(confinedWriter));
let darwinHelperSha256 = null;
if (process.platform === "darwin") {
  command("darwin-helper-build", "go", ["build", "-trimpath", "-o", darwinHelper, "."], join(root, "native", "darwin-immutable-helper"));
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
await stage("bundle", () => build({
  bundle: true,
  entryPoints: [join(root, "dist", "sea-entry.js")],
  format: "cjs",
  outfile: bundle,
  platform: "node",
  target: "node22",
  nodePaths: [join(root, "node_modules")],
  alias: { commander: join(root, "node_modules", "commander", "index.js") },
  define: {
    "process.env.SERVICE_LASSO_CANDIDATE_VERSION": JSON.stringify(version),
    "process.env.SERVICE_LASSO_CANDIDATE_SOURCE_SHA": JSON.stringify(sourceSha),
    __SERVICE_LASSO_CANDIDATE_VERSION__: JSON.stringify(version),
    __SERVICE_LASSO_CANDIDATE_SOURCE_SHA__: JSON.stringify(sourceSha),
    __SERVICE_LASSO_CONFINED_HELPER_SHA256__: JSON.stringify(confinedWriterSha256),
    __SERVICE_LASSO_CONTROLLED_TEST_ADMISSIONS__: JSON.stringify(controlledAdmissions),
  },
  legalComments: "none",
}));
await writeFile(seaConfig, `${JSON.stringify({ main: bundle, output: blob, disableExperimentalSEAWarning: true, useCodeCache: false, execArgvExtension: "none" }, null, 2)}\n`);
command("sea-blob", process.execPath, ["--experimental-sea-config", seaConfig]);
await copyFile(process.execPath, executable);
if (process.platform === "darwin") command("signature-remove", "codesign", ["--remove-signature", executable]);
const postject = join(root, "node_modules", "postject", "dist", "cli.js");
const postjectArgs = [executable, "NODE_SEA_BLOB", blob, "--sentinel-fuse", fuse];
if (process.platform === "darwin") postjectArgs.push("--macho-segment-name", "NODE_SEA");
command("postject", process.execPath, [postject, ...postjectArgs]);
if (process.platform === "darwin") command("signature-add", "codesign", ["--sign", "-", executable]);

// The published command is a compiled resident primary.  It embeds both
// target-host images, so a normal invocation never chooses a helper from a
// mutable archive neighbour.  Keep the checked writer alongside the archive
// as an auditable member; it is not executable authority for the primary.
await copyFile(executable, embeddedSea);
const gateSource = join(root, "native", "primary-gate");
const gateBuild = join(output, ".primary-gate-build");
await stage("primary-stage", () => stagePrimaryGate({ source: gateSource, build: gateBuild, sea: embeddedSea, writer: confinedWriter, platform: process.platform, darwinHelperSha256 }));
command("primary-build", "go", ["build", "-trimpath", "-o", executable, "."], gateBuild);
await rm(gateBuild, { recursive: true, force: true });
await rm(embeddedSea, { force: true });

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
