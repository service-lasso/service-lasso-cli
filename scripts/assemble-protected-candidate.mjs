import { cp, mkdir, readFile, rm, stat, writeFile } from "node:fs/promises";
import { join, resolve } from "node:path";
import { expectedAssets, releaseServiceManifest, sha256, verifyCandidateDirectory, fail, targetsForSchema } from "./protected-candidate-lib.mjs";

function option(name) { const index = process.argv.indexOf(name); if (index < 0 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`); return process.argv[index + 1]; }
function nativeDirectories() { const values = []; for (let index = 0; index < process.argv.length; index += 1) if (process.argv[index] === "--native") values.push(process.argv[index + 1]); return values; }

const output = resolve(option("--output"));
const portable = resolve(option("--portable"));
const version = option("--version");
const sourceSha = option("--source-sha");
const native = new Map(nativeDirectories().map((value) => value.split("=", 2)));
const scopeIndex = process.argv.indexOf("--scope");
const scope = scopeIndex < 0 ? "historical-three-target" : process.argv[scopeIndex + 1];
if (!["historical-three-target", "windows-linux"].includes(scope)) fail("unsupported candidate scope.");
const schemaVersion = scope === "windows-linux" ? 3 : 2;
const targetIds = targetsForSchema(schemaVersion).map(target => target.id).sort();
const expected = expectedAssets(version, schemaVersion);
if (native.size !== targetIds.length || [...native.keys()].sort().join(",") !== targetIds.join(",")) fail("exact selected native target directories are required.");
await rm(output, { recursive: true, force: true });
await mkdir(output, { recursive: true });

for (const asset of expected.filter((item) => item.kind !== "service-manifest")) {
  const source = asset.kind.startsWith("portable") ? join(portable, asset.name) : asset.kind === "native" ? join(resolve(native.get(asset.target)), `service-lassoctl-${version}-${asset.target}.tar.gz`) : join(resolve(native.get(asset.target)), "provenance.json");
  await cp(source, join(output, asset.name));
}
const assets = [];
for (const item of expected.filter((item) => item.kind !== "service-manifest")) { const path = join(output, item.name); const bytes = await readFile(path); assets.push({ ...item, sha256: sha256(bytes), size: (await stat(path)).size }); }
const serviceBytes = Buffer.from(`${JSON.stringify(releaseServiceManifest(version, sourceSha, assets, schemaVersion), null, 2)}\n`);
await writeFile(join(output, "service.json"), serviceBytes);
assets.unshift({ name: "service.json", kind: "service-manifest", target: null, sha256: sha256(serviceBytes), size: serviceBytes.length });
const manifest = { schemaVersion, version, candidateTag: `cli-v${version}-candidate-${sourceSha.slice(0, 7)}`, source: { repository: "service-lasso/service-lasso-cli", commit: sourceSha }, checksums: { algorithm: "sha256", file: "SHA256SUMS.txt", entries: ["development-candidate.json", ...assets.map((asset) => asset.name)].sort() }, assets };
const manifestBytes = Buffer.from(`${JSON.stringify(manifest, null, 2)}\n`);
await writeFile(join(output, "development-candidate.json"), manifestBytes);
await writeFile(join(output, "SHA256SUMS.txt"), [{ name: "development-candidate.json", sha256: sha256(manifestBytes) }, ...assets].sort((a, b) => a.name.localeCompare(b.name)).map((asset) => `${asset.sha256}  ${asset.name}\n`).join(""));
await verifyCandidateDirectory(output, version, sourceSha);
process.stdout.write(`${JSON.stringify({ candidateTag: manifest.candidateTag, sourceSha, assets: assets.map((asset) => asset.name) })}\n`);
