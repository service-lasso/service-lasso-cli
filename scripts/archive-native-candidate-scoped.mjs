import { runScopedCli } from './cli-platform-scope.mjs';
import { readSourceScope } from './cli-platform-scope.mjs';
import { verifyScopedNative, NATIVE_TARGETS, readHeldFile } from './protected-scoped-candidate-lib.mjs';
import { createHash } from "node:crypto";
import { lstat } from "node:fs/promises";
import { basename, join, resolve } from "node:path";
import { spawnSync } from "node:child_process";
import { parseStrictJson, readTarArchive } from "./protected-candidate-lib.mjs";

await runScopedCli(async () => {
await readSourceScope();
function option(name) { const index = process.argv.indexOf(name); if (index < 0 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`); return process.argv[index + 1]; }
function emptyOption(name) { const index = process.argv.indexOf(name); const value = process.argv[index + 1]; if (index < 0 || value === undefined || value.startsWith("--")) throw new Error(`Missing ${name}.`); return value; }
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
const eventName = option("--expected-event"), baseSha = emptyOption("--expected-base-sha"), mergeContextSha = option("--expected-merge-context-sha");
if (!/^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i.test(version) || !/^[0-9a-f]{40}$/i.test(sourceSha)) throw new Error("Version or source SHA is invalid.");
const provenanceBytes = await readHeldFile(directory, "provenance.json");
const provenance = parseStrictJson(provenanceBytes, "Native provenance");
const target = `${provenance?.executable?.platform}-${provenance?.executable?.architecture}`;
if (!new Set(["win32-x64", "linux-x64"]).has(target) || provenance?.source?.commit !== sourceSha || provenance?.candidate?.version !== version || provenance?.candidate?.tag !== `cli-v${version}-candidate-${sourceSha.slice(0, 7)}` || provenance?.executable?.version !== version) throw new Error("Native provenance does not match the frozen target identity.");
if (target !== `${process.platform}-${process.arch}`) throw new Error("Scoped native archive requires the actual matching selected host.");
const executable = provenance.executable.name;
if (executable !== (target === "win32-x64" ? "service-lassoctl.exe" : "service-lassoctl")) throw new Error("Native executable name does not match target.");
const confinedWriter = provenance.confinedWriter?.name;
if (confinedWriter !== (target === "win32-x64" ? "service-lasso-confined-scaffold.exe" : "service-lasso-confined-scaffold")) throw new Error("Native provenance requires a named confined writer.");
const executableBytes = await readHeldFile(directory, executable);
if (sha256(executableBytes) !== provenance.executable.sha256) throw new Error("Native executable does not match its provenance digest.");
const confinedWriterBytes = await readHeldFile(directory, confinedWriter);
if (sha256(confinedWriterBytes) !== provenance.confinedWriter.sha256) throw new Error("Confined writer does not match its provenance digest.");
let contextBytes, acceptanceBytes;
try { contextBytes = await readHeldFile(directory, "ci-context.json"); } catch { throw new Error("Native archive requires ci-context.json from the target-host verification step."); }
try { acceptanceBytes = await readHeldFile(directory, "host-acceptance.json"); } catch { throw new Error("Native archive requires host-acceptance.json from the target-host verification step."); }
const context = closedRecord(contextBytes, ["schemaVersion", "eventName", "sourceSha", "testedBaseSha", "mergeContextSha"], "Native CI context");
if (context.schemaVersion !== 1 || context.sourceSha !== sourceSha || !["push", "pull_request", "workflow_dispatch"].includes(eventName) || context.eventName !== eventName || !/^[0-9a-f]{40}$/i.test(mergeContextSha) || context.mergeContextSha !== mergeContextSha) throw new Error("Native CI context does not bind the caller event and source.");
if (eventName === "pull_request") {
  if (!/^[0-9a-f]{40}$/i.test(baseSha) || context.testedBaseSha !== baseSha || new Set([sourceSha, baseSha, mergeContextSha]).size !== 3) throw new Error("Native CI context does not bind distinct PR source, base and merge identities.");
} else if (baseSha !== "" || context.testedBaseSha !== null || mergeContextSha !== sourceSha) throw new Error("Native CI context does not bind this source event.");
const acceptance = closedRecord(acceptanceBytes, ["schemaVersion", "sourceSha", "version", "platform", "architecture", "executableSha256", "nodeAbsentFromPath", "status", "evidenceDigest"], "Native host acceptance");
const acceptanceIdentity = `service-lasso-native-acceptance-v1\n${sourceSha}\n${version}\n${provenance.executable.platform}\n${provenance.executable.architecture}\n${sha256(executableBytes)}\nnode-absent\npassed\n`;
if (acceptance.schemaVersion !== 1 || acceptance.sourceSha !== sourceSha || acceptance.version !== version || acceptance.platform !== provenance.executable.platform || acceptance.architecture !== provenance.executable.architecture || acceptance.executableSha256 !== sha256(executableBytes) || acceptance.nodeAbsentFromPath !== true || acceptance.status !== "passed" || acceptance.evidenceDigest !== sha256(Buffer.from(acceptanceIdentity, "utf8"))) throw new Error("Native host acceptance does not bind the retained executable bytes.");
const archive = `service-lassoctl-${version}-${target}.tar.gz`;
const archivePath = join(output, archive);
try { await lstat(archivePath); throw new Error("Scoped native archive output must be fresh."); }
catch (error) { if (error?.code !== "ENOENT") throw error; }
// GNU tar interprets a drive-colon archive name as a remote host. Keep the
// fixed local basename relative to the caller's job-owned output directory;
// the absolute input directory is a separate literal -C argument, never -f.
const result = spawnSync("tar", ["-czf", `./${archive}`, "-C", directory, "--", executable, confinedWriter, "provenance.json", "ci-context.json", "host-acceptance.json"], { cwd: output, encoding: "utf8", shell: false });
if (result.status !== 0 || result.error) {
  const failure = new Error("Native archive child failed."); failure.nativeArchiveOriginal = result; throw failure;
}
const archivedBytes = await readHeldFile(output, archive);
const members = readTarArchive(archivedBytes);
for (const [name, original] of [[executable, executableBytes], [confinedWriter, confinedWriterBytes],
  ["provenance.json", provenanceBytes], ["ci-context.json", contextBytes], ["host-acceptance.json", acceptanceBytes]])
  if (!members.get(name)?.equals(original)) throw new Error("Native archive differs from its original consumed byte vector.");
verifyScopedNative(archivedBytes, provenanceBytes, NATIVE_TARGETS.find(item => item.id === target), sourceSha, version);
process.stdout.write(`${JSON.stringify({ archive, sha256: sha256(archivedBytes), executable: basename(executable), target })}\n`);

});
