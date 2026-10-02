import { createHash } from "node:crypto";
import { readFile, rm } from "node:fs/promises";
import { basename, join, resolve } from "node:path";
import { spawnSync } from "node:child_process";
import { parseStrictJson } from "./protected-candidate-lib.mjs";

function option(name) { const index = process.argv.indexOf(name); if (index < 0 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`); return process.argv[index + 1]; }
function sha256(bytes) { return createHash("sha256").update(bytes).digest("hex"); }
function closedRecord(bytes, keys, name) {
  const value = parseStrictJson(bytes, name);
  if (!value || typeof value !== "object" || Array.isArray(value) || Object.keys(value).length !== keys.length || keys.some((key) => !Object.hasOwn(value, key))) throw new Error(`${name} is not schema-closed.`);
  return value;
}
const directory = resolve(option("--directory"));
const output = resolve(option("--output"));
const version = option("--version");
const sourceSha = option("--source-sha");
if (!/^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i.test(version) || !/^[0-9a-f]{40}$/i.test(sourceSha)) throw new Error("Version or source SHA is invalid.");
const provenance = parseStrictJson(await readFile(join(directory, "provenance.json")), "Native provenance");
const target = `${provenance?.executable?.platform}-${provenance?.executable?.architecture}`;
if (!new Set(["win32-x64", "linux-x64", "darwin-arm64"]).has(target) || provenance?.source?.commit !== sourceSha || provenance?.candidate?.version !== version || provenance?.candidate?.tag !== `cli-v${version}-candidate-${sourceSha.slice(0, 7)}` || provenance?.executable?.version !== version) throw new Error("Native provenance does not match the frozen target identity.");
const executable = provenance.executable.name;
if (executable !== (target === "win32-x64" ? "service-lassoctl.exe" : "service-lassoctl")) throw new Error("Native executable name does not match target.");
const confinedWriter = provenance.confinedWriter?.name;
if (confinedWriter !== (target === "win32-x64" ? "service-lasso-confined-scaffold.exe" : "service-lasso-confined-scaffold")) throw new Error("Native provenance requires a named confined writer.");
const executableBytes = await readFile(join(directory, executable));
if (sha256(executableBytes) !== provenance.executable.sha256) throw new Error("Native executable does not match its provenance digest.");
const confinedWriterBytes = await readFile(join(directory, confinedWriter));
if (sha256(confinedWriterBytes) !== provenance.confinedWriter.sha256) throw new Error("Confined writer does not match its provenance digest.");
if (target === "darwin-arm64") {
  const helper = provenance.darwinImmutableHelper;
  if (!helper || helper.name !== "service-lasso-darwin-immutable-helper" || helper.platform !== "darwin" || helper.architecture !== "arm64" || !/^[0-9a-f]{64}$/i.test(helper.sha256) || !/^[0-9a-f]{64}$/i.test(helper.sourceSha256) || sha256(await readFile(join(directory, helper.name))) !== helper.sha256) throw new Error("Darwin privileged helper does not retain its source-built identity.");
}
let contextBytes, acceptanceBytes;
try { contextBytes = await readFile(join(directory, "ci-context.json")); } catch { throw new Error("Native archive requires ci-context.json from the target-host verification step."); }
try { acceptanceBytes = await readFile(join(directory, "host-acceptance.json")); } catch { throw new Error("Native archive requires host-acceptance.json from the target-host verification step."); }
const context = closedRecord(contextBytes, ["schemaVersion", "eventName", "sourceSha", "testedBaseSha", "mergeContextSha"], "Native CI context");
if (context.schemaVersion !== 1 || context.sourceSha !== sourceSha || context.eventName !== "workflow_dispatch" || context.testedBaseSha !== null || context.mergeContextSha !== sourceSha) throw new Error("Native CI context does not bind this workflow-dispatch candidate.");
const acceptance = closedRecord(acceptanceBytes, ["schemaVersion", "sourceSha", "version", "platform", "architecture", "executableSha256", "nodeAbsentFromPath", "status", "evidenceDigest"], "Native host acceptance");
const acceptanceIdentity = `service-lasso-native-acceptance-v1\n${sourceSha}\n${version}\n${provenance.executable.platform}\n${provenance.executable.architecture}\n${sha256(executableBytes)}\nnode-absent\npassed\n`;
if (acceptance.schemaVersion !== 1 || acceptance.sourceSha !== sourceSha || acceptance.version !== version || acceptance.platform !== provenance.executable.platform || acceptance.architecture !== provenance.executable.architecture || acceptance.executableSha256 !== sha256(executableBytes) || acceptance.nodeAbsentFromPath !== true || acceptance.status !== "passed" || acceptance.evidenceDigest !== sha256(Buffer.from(acceptanceIdentity, "utf8"))) throw new Error("Native host acceptance does not bind the retained executable bytes.");
const archive = `service-lassoctl-${version}-${target}.tar.gz`;
const archivePath = join(output, archive);
await rm(archivePath, { force: true });
const result = spawnSync("tar", ["-czf", archivePath, "-C", directory, "--", executable, confinedWriter, ...(target === "darwin-arm64" ? ["service-lasso-darwin-immutable-helper"] : []), "provenance.json", "ci-context.json", "host-acceptance.json"], { encoding: "utf8" });
if (result.status !== 0 || result.error) throw new Error(`Native archive failed: ${result.error?.message ?? result.stderr ?? result.stdout}`);
process.stdout.write(`${JSON.stringify({ archive, sha256: sha256(await readFile(archivePath)), executable: basename(executable), target })}\n`);
