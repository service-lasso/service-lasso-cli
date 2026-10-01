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
declare const __SERVICE_LASSO_CANDIDATE_VERSION__: string | undefined;
declare const __SERVICE_LASSO_CANDIDATE_SOURCE_SHA__: string | undefined;
declare const __SERVICE_LASSO_CONFINED_HELPER_SHA256__: string | undefined;
function candidateIdentity(): { version: string; sourceSha: string; helperSha256: string } | undefined {
  // These globals are replaced while the SEA is built.  `typeof` keeps normal
  // source execution independent of ambient packaging-looking environment.
  const version = typeof __SERVICE_LASSO_CANDIDATE_VERSION__ === "string" ? __SERVICE_LASSO_CANDIDATE_VERSION__ : undefined;
  const sourceSha = typeof __SERVICE_LASSO_CANDIDATE_SOURCE_SHA__ === "string" ? __SERVICE_LASSO_CANDIDATE_SOURCE_SHA__ : undefined;
  const helperSha256 = typeof __SERVICE_LASSO_CONFINED_HELPER_SHA256__ === "string" ? __SERVICE_LASSO_CONFINED_HELPER_SHA256__ : undefined;
  return /^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i.test(version ?? "") && /^[0-9a-f]{40}$/i.test(sourceSha ?? "") && /^[0-9a-f]{64}$/i.test(helperSha256 ?? "") ? { version: version!, sourceSha: sourceSha!, helperSha256: helperSha256! } : undefined;
}
function packagedHelper(): string | undefined {
  // A candidate SEA may inspect the adjacent helper only to reject a broken
  // package. It must never turn verified bytes into a temporary pathname and
  // ask Node to spawn that pathname: spawn re-resolves the name after the
  // check. The compiled primary launch gate owns candidate materialization and
  // retains the verified helper identity through process creation over its
  // inherited private IPC channel. This source SEA deliberately fails closed
  // until that gate protocol is present; source execution below remains a
  // developer-only writer path and is never candidate evidence.
  const identity = candidateIdentity();
  if (!identity) return undefined;
  const directory = resolve(process.execPath, "..");
  const candidate = resolve(directory, helperName);
  try {
    const helperBytes = readFileSync(candidate);
    if (sha256(helperBytes) !== identity.helperSha256) return undefined;
    return undefined;
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
  // retained audit artifact.  This output directory is intentionally retained:
  // a pathname-based recursive cleanup cannot prove that a replaced target is
  // still ours.
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
