import { cp, mkdir, readFile, rm, stat, writeFile } from "node:fs/promises";
import { join, resolve } from "node:path";
import { expectedAssets, sha256, verifyCandidateDirectory, fail } from "./protected-candidate-lib.mjs";

function option(name) { const index = process.argv.indexOf(name); if (index < 0 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`); return process.argv[index + 1]; }
function nativeDirectories() { const values = []; for (let index = 0; index < process.argv.length; index += 1) if (process.argv[index] === "--native") values.push(process.argv[index + 1]); return values; }

const output = resolve(option("--output"));
const portable = resolve(option("--portable"));
const version = option("--version");
const sourceSha = option("--source-sha");
const native = new Map(nativeDirectories().map((value) => value.split("=", 2)));
const expected = expectedAssets(version);
if (native.size !== 3 || [...native.keys()].sort().join(",") !== "darwin-arm64,linux-x64,win32-x64") fail("all three native target directories are required.");
await rm(output, { recursive: true, force: true });
await mkdir(output, { recursive: true });

for (const asset of expected) {
  const source = asset.kind.startsWith("portable") ? join(portable, asset.name) : asset.kind === "native" ? join(resolve(native.get(asset.target)), `service-lassoctl-${version}-${asset.target}.tar.gz`) : join(resolve(native.get(asset.target)), "provenance.json");
  await cp(source, join(output, asset.name));
}
const assets = [];
for (const item of expected) { const path = join(output, item.name); const bytes = await readFile(path); assets.push({ ...item, sha256: sha256(bytes), size: (await stat(path)).size }); }
const manifest = { schemaVersion: 1, version, candidateTag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}`, source: { repository: "service-lasso/service-lasso-cli", commit: sourceSha }, checksums: { algorithm: "sha256", file: "SHA256SUMS.txt", entries: ["development-candidate.json", ...assets.map((asset) => asset.name)].sort() }, assets };
const manifestBytes = Buffer.from(`${JSON.stringify(manifest, null, 2)}\n`);
await writeFile(join(output, "development-candidate.json"), manifestBytes);
await writeFile(join(output, "SHA256SUMS.txt"), [{ name: "development-candidate.json", sha256: sha256(manifestBytes) }, ...assets].sort((a, b) => a.name.localeCompare(b.name)).map((asset) => `${asset.sha256}  ${asset.name}\n`).join(""));
await verifyCandidateDirectory(output, version, sourceSha);
process.stdout.write(`${JSON.stringify({ candidateTag: manifest.candidateTag, sourceSha, assets: assets.map((asset) => asset.name) })}\n`);
