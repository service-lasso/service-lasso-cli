import { createHash } from "node:crypto";
import { cp, mkdir, readFile, rename, rm, stat, writeFile } from "node:fs/promises";
import { basename, join, resolve } from "node:path";
import { execFileSync } from "node:child_process";

function option(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`);
  return process.argv[index + 1];
}

function sha256(value) {
  return createHash("sha256").update(value).digest("hex");
}

const output = resolve(option("--output"));
const version = option("--version");
const sourceSha = option("--source-sha");
if (!/^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i.test(version)) throw new Error("Candidate version must be <semver>-dev.<short-sha>.");
if (!/^[0-9a-f]{40}$/i.test(sourceSha)) throw new Error("Source SHA must be a 40-character commit ID.");

const root = resolve(import.meta.dirname, "..");
const staging = join(output, ".package");
await rm(output, { recursive: true, force: true });
await mkdir(staging, { recursive: true });
await cp(join(root, "dist"), join(staging, "dist"), { recursive: true });
await cp(join(root, "README.md"), join(staging, "README.md"));

const manifest = JSON.parse(await readFile(join(root, "package.json"), "utf8"));
const sourceVersion = manifest.version;
manifest.version = version;
await writeFile(join(staging, "package.json"), `${JSON.stringify(manifest, null, 2)}\n`);

const entrypoint = join(staging, "dist", "index.js");
const entrypointSource = await readFile(entrypoint, "utf8");
const versionCall = `.version("${sourceVersion}")`;
if (!entrypointSource.includes(versionCall)) throw new Error("CLI entrypoint version marker was not found.");
await writeFile(entrypoint, entrypointSource.replace(versionCall, `.version("${version}")`));

const npm = process.platform === "win32" ? "npm.cmd" : "npm";
const packed = JSON.parse(execFileSync(npm, ["pack", "--json", "--pack-destination", output], {
  cwd: staging,
  encoding: "utf8",
  shell: process.platform === "win32",
}));
if (!Array.isArray(packed) || packed.length !== 1 || typeof packed[0]?.filename !== "string") throw new Error("npm pack did not return one archive.");
const archiveName = `service-lassoctl-${version}.tgz`;
const packedPath = join(output, packed[0].filename);
const archivePath = join(output, archiveName);
await rename(packedPath, archivePath);
const archive = await readFile(archivePath);
const archiveHash = sha256(archive);
const archiveSize = (await stat(archivePath)).size;
const candidate = {
  schemaVersion: 1,
  candidateTag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}`,
  version,
  source: { repository: "service-lasso/service-lasso-cli", commit: sourceSha },
  package: { name: manifest.name, command: "service-lassoctl", entrypoint: "dist/index.js", node: manifest.engines.node },
  platforms: ["win32", "linux", "darwin"],
  assets: [{ name: archiveName, sha256: archiveHash, size: archiveSize }],
};
const candidateBytes = Buffer.from(`${JSON.stringify(candidate, null, 2)}\n`);
await writeFile(join(output, "candidate.json"), candidateBytes);
await writeFile(join(output, "SHA256SUMS.txt"), `${archiveHash}  ${archiveName}\n${sha256(candidateBytes)}  candidate.json\n`);
await rm(staging, { recursive: true, force: true });
console.log(JSON.stringify({ archive: archiveName, candidateTag: candidate.candidateTag, sha256: archiveHash }));
