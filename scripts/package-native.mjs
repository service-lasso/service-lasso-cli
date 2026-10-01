import { build } from "esbuild";
import { copyFile, mkdir, readFile, readdir, rm, writeFile } from "node:fs/promises";
import { spawnSync } from "node:child_process";
import { createHash, generateKeyPairSync } from "node:crypto";
import { fileURLToPath } from "node:url";
import { basename, dirname, join, resolve } from "node:path";

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

function command(file, args, cwd = root) {
  const result = spawnSync(file, args, { cwd, encoding: "utf8" });
  if (result.status !== 0 || result.error) throw new Error(`${basename(file)} failed: ${result.error?.message ?? result.stderr ?? result.stdout}`);
}

function sha256(bytes) {
  return createHash("sha256").update(bytes).digest("hex");
}

async function confinedWriterSourceSha256() {
  const directory = join(root, "native", "confined-scaffold");
  const names = (await readdir(directory)).filter((name) => name.endsWith(".go") || name === "go.mod" || name === "go.sum").sort();
  const contents = [];
  for (const name of names) contents.push(Buffer.from(`${name}\0`, "utf8"), await readFile(join(directory, name)), Buffer.from("\0", "utf8"));
  return sha256(Buffer.concat(contents));
}

if (process.versions.node !== nodeVersion) throw new Error(`Node ${nodeVersion} is required; found ${process.versions.node}.`);

const output = resolve(argument("--output"));
const sourceSha = argument("--source-sha");
const version = argument("--version");
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
const embeddedSea = join(output, embeddedSeaName);
const seaConfig = join(output, "sea-config.json");

await rm(output, { recursive: true, force: true });
await mkdir(output, { recursive: true });
command("go", ["build", "-trimpath", "-o", confinedWriter, "."], join(root, "native", "confined-scaffold"));
const confinedWriterSha256 = sha256(await readFile(confinedWriter));
const gateKeyPair = generateKeyPairSync("ed25519");
const gatePublicKey = gateKeyPair.publicKey.export({ format: "der", type: "spki" }).toString("base64");
const gatePrivateDer = gateKeyPair.privateKey.export({ format: "der", type: "pkcs8" });
const gateSeed = gatePrivateDer.subarray(-32);
await build({
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
    __SERVICE_LASSO_PRIMARY_GATE_PUBLIC_KEY__: JSON.stringify(gatePublicKey),
  },
  legalComments: "none",
});
await writeFile(seaConfig, `${JSON.stringify({ main: bundle, output: blob, disableExperimentalSEAWarning: true, useCodeCache: false, execArgvExtension: "none" }, null, 2)}\n`);
command(process.execPath, ["--experimental-sea-config", seaConfig]);
await copyFile(process.execPath, executable);
if (process.platform === "darwin") command("codesign", ["--remove-signature", executable]);
const postject = join(root, "node_modules", "postject", "dist", "cli.js");
const postjectArgs = [executable, "NODE_SEA_BLOB", blob, "--sentinel-fuse", fuse];
if (process.platform === "darwin") postjectArgs.push("--macho-segment-name", "NODE_SEA");
command(process.execPath, [postject, ...postjectArgs]);
if (process.platform === "darwin") command("codesign", ["--sign", "-", executable]);

// The published command is a compiled resident primary.  It embeds both
// target-host images, so a normal invocation never chooses a helper from a
// mutable archive neighbour.  Keep the checked writer alongside the archive
// as an auditable member; it is not executable authority for the primary.
await copyFile(executable, embeddedSea);
const gateSource = join(root, "native", "primary-gate");
const gateBuild = join(output, ".primary-gate-build");
await mkdir(join(gateBuild, "assets"), { recursive: true });
await Promise.all([
  copyFile(join(gateSource, "go.mod"), join(gateBuild, "go.mod")),
  copyFile(join(gateSource, "go.sum"), join(gateBuild, "go.sum")),
  copyFile(join(gateSource, "main.go"), join(gateBuild, "main.go")),
  copyFile(join(gateSource, "ipc.go"), join(gateBuild, "ipc.go")),
  copyFile(join(gateSource, "main_windows.go"), join(gateBuild, "main_windows.go")),
  copyFile(join(gateSource, "main_unix.go"), join(gateBuild, "main_unix.go")),
  copyFile(join(gateSource, "main_linux.go"), join(gateBuild, "main_linux.go")),
  copyFile(embeddedSea, join(gateBuild, "assets", "service-lassoctl.sea")),
  copyFile(confinedWriter, join(gateBuild, "assets", "service-lasso-confined-scaffold")),
]);
await writeFile(join(gateBuild, "gate_auth.go"), `package main\n\nvar gateSigningSeed = [32]byte{${[...gateSeed].map((value) => `0x${value.toString(16).padStart(2, "0")}`).join(", ")}}\n`);
command("go", ["build", "-trimpath", "-o", executable, "."], gateBuild);
await rm(gateBuild, { recursive: true, force: true });
await rm(embeddedSea, { force: true });

const provenance = {
  schemaVersion: 1,
  command: "service-lassoctl",
  candidate: { version, tag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}` },
  source: { commit: sourceSha },
  executable: { name: executableName, sha256: sha256(await readFile(executable)), platform: process.platform, architecture: process.arch, version },
  confinedWriter: { name: confinedWriterName, sha256: confinedWriterSha256, sourceSha256: await confinedWriterSourceSha256(), platform: process.platform, architecture: process.arch },
  tools: { node: nodeVersion, esbuild: esbuildVersion, postject: postjectVersion },
  sea: { mainFormat: "commonjs", useCodeCache: false, execArgvExtension: "none" },
};
await writeFile(join(output, "provenance.json"), `${JSON.stringify(provenance, null, 2)}\n`);
process.stdout.write(`${JSON.stringify(provenance)}\n`);
