import { spawn, spawnSync } from "node:child_process";
import { mkdtempSync, readFileSync } from "node:fs";
import { createHash } from "node:crypto";
import { tmpdir } from "node:os";
import { join, relative, resolve, sep } from "node:path";
import { fileURLToPath } from "node:url";
import { CliError } from "./errors.js";
import { AcceptedTemplateBundle, acceptedTemplateFiles, loadAcceptedTemplateBundle, requireAcceptedTemplateIdentity, templateContractPreview } from "./template.js";
export interface ServiceScaffoldOptions { id: string; directory: string; name?: string; dryRun?: boolean; templateRoot?: string; }
export interface ServiceScaffoldResult { directory: string; files: string[]; dryRun: boolean; }
export function validateServiceId(id: string): string { if (!/^[a-z][a-z0-9-]{1,63}$/.test(id)) throw new CliError("invalid_service_id", "Service id must use lowercase letters, numbers, and hyphens (2-64 characters)."); return id; }
export function previewServiceScaffold(options: ServiceScaffoldOptions): Record<string, unknown> { return { directory: resolve(options.directory), serviceId: validateServiceId(options.id), dryRun: true, files: [], writes: false, runtimeMutation: false, template: templateContractPreview() }; }
export function scaffoldFiles(options: ServiceScaffoldOptions): Record<string, string> { validateServiceId(options.id); return requireAcceptedTemplateIdentity(); }
const helperName = `service-lasso-confined-scaffold${process.platform === "win32" ? ".exe" : ""}`;
let sourceHelper: string | undefined;
function sha256(value: Buffer | string): string { return createHash("sha256").update(value).digest("hex"); }
function candidateIdentity(): { version: string; sourceSha: string } | undefined {
  const version = process.env.SERVICE_LASSO_CANDIDATE_VERSION;
  const sourceSha = process.env.SERVICE_LASSO_CANDIDATE_SOURCE_SHA;
  return /^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i.test(version ?? "") && /^[0-9a-f]{40}$/i.test(sourceSha ?? "") ? { version: version!, sourceSha: sourceSha! } : undefined;
}
function packagedHelper(): string | undefined {
  // package-native replaces these expressions with immutable candidate values
  // while bundling the SEA.  Ordinary source execution never treats ambient
  // environment variables as a packaging claim.
  const identity = candidateIdentity();
  if (!identity) return undefined;
  const { version, sourceSha } = identity;
  const directory = resolve(process.execPath, "..");
  const candidate = resolve(directory, helperName);
  const provenancePath = resolve(directory, "provenance.json");
  try {
    const provenance = JSON.parse(readFileSync(provenancePath, "utf8")) as { schemaVersion?: unknown; candidate?: { version?: unknown }; source?: { commit?: unknown }; confinedWriter?: { name?: unknown; sha256?: unknown; platform?: unknown; architecture?: unknown } };
    const writer = provenance.confinedWriter;
    if (provenance.schemaVersion !== 1 || provenance.candidate?.version !== version || provenance.source?.commit !== sourceSha || writer?.name !== helperName || writer.platform !== process.platform || writer.architecture !== process.arch || typeof writer.sha256 !== "string" || !/^[0-9a-f]{64}$/i.test(writer.sha256) || sha256(readFileSync(candidate)) !== writer.sha256) return undefined;
    return candidate;
  } catch { return undefined; }
}
function helperPath(): string {
  const packaged = packagedHelper();
  if (packaged) return packaged;
  if (candidateIdentity()) throw new CliError("unsafe_scaffold_destination", "The confined writer is unavailable.");
  // Source execution builds the checked-in helper only for local developer and
  // test use.  It is never native-candidate qualification evidence.  The
  // directory is deliberately retained: Node cannot prove a recursively named
  // cleanup target still belongs to this process after an attacker replaces it.
  const helperSource = fileURLToPath(new URL("../native/confined-scaffold", import.meta.url));
  if (sourceHelper) return sourceHelper;
  // Never compile into the repository: an inherited helper binary can be a
  // retained audit artifact.  This output directory is unique to this process
  // and removed only by its owner on exit.
  const helperDirectory = mkdtempSync(join(tmpdir(), "service-lasso-confined-helper-"));
  const helperBinary = join(helperDirectory, helperName);
  const build = spawnSync("go", ["build", "-trimpath", "-o", helperBinary, "."], { cwd: helperSource, encoding: "utf8" });
  if (build.status !== 0 || build.error) throw new CliError("unsafe_scaffold_destination", "The confined writer is unavailable.");
  sourceHelper = helperBinary;
  return sourceHelper;
}
async function confinedMaterialize(destination: string, files: Awaited<ReturnType<typeof acceptedTemplateFiles>>): Promise<void> {
  const input = `${Buffer.from(destination).toString("base64")}\n${files.length}\n${files.map(file => `${file.path}\t${file.mode.toString(8)}\t${file.bytes.toString("base64")}`).join("\n")}\n`;
  const expectedReceipt = sha256(input);
  const outcome = await new Promise<{ stdout: string; stderr: string; code: number | null }>((done, fail) => { const child = spawn(helperPath(), [], { stdio: ["pipe", "pipe", "pipe"], windowsHide: true }); let stdout = "", stderr = ""; child.stdout.on("data", value => { stdout += value; }); child.stderr.on("data", value => { stderr += value; }); child.once("error", fail); child.once("close", code => done({ stdout, stderr, code })); child.stdin.end(input); });
  const result = outcome.stdout.trim();
  if (outcome.code === 0 && outcome.stderr === "" && result === `ok\t${expectedReceipt}`) return;
  // The helper only emits these fixed, path-free failure codes.  Keep the
  // public CLI result stable while rejecting malformed helper output.
  const match = outcome.code === 1 && outcome.stderr === "confined writer failed\n" && /^error\t(destination_exists|parent_missing|permission_denied|write_rejected)$/.exec(result);
  if (!match) throw new CliError("unsafe_scaffold_destination", "The project destination could not be created through the confined writer.");
  throw new CliError(`confined_writer_${match[1]}`, "The project destination could not be created through the confined writer.");
}
export async function createServiceScaffold(options: ServiceScaffoldOptions): Promise<ServiceScaffoldResult> { validateServiceId(options.id); const destination = resolve(options.directory); if (!options.templateRoot) return requireAcceptedTemplateIdentity(); const bundle = await loadAcceptedTemplateBundle(options.templateRoot); return materializeAcceptedTemplate(destination, bundle, Boolean(options.dryRun)); }
/** Materialize only a bundle already admitted by the source-owned resolver. */
export async function materializeAcceptedTemplate(destinationInput: string, bundle: AcceptedTemplateBundle, dryRun = false): Promise<ServiceScaffoldResult> {
  const destination = resolve(destinationInput), files = await acceptedTemplateFiles(bundle);
  if (dryRun) return { directory: destination, files: files.map(file => file.path), dryRun: true };
  if (files.some(file => !resolve(destination, file.path).startsWith(destination + sep) || relative(destination, resolve(destination, file.path)).startsWith(".."))) throw new CliError("invalid_template_bundle", "Template inventory contains an unsafe path.");
  try { await confinedMaterialize(destination, files); } catch (error) { if (error instanceof CliError && error.code.startsWith("confined_writer_")) throw error; throw new CliError("unsafe_scaffold_destination", "The project destination could not be created through the confined writer."); }
  return { directory: destination, files: files.map(file => file.path), dryRun: false };
}
