import { spawn, spawnSync } from "node:child_process";
import { relative, resolve, sep } from "node:path";
import { fileURLToPath } from "node:url";
import { CliError } from "./errors.js";
import { AcceptedTemplateBundle, acceptedTemplateFiles, loadAcceptedTemplateBundle, requireAcceptedTemplateIdentity, templateContractPreview } from "./template.js";
export interface ServiceScaffoldOptions { id: string; directory: string; name?: string; dryRun?: boolean; templateRoot?: string; }
export interface ServiceScaffoldResult { directory: string; files: string[]; dryRun: boolean; }
export function validateServiceId(id: string): string { if (!/^[a-z][a-z0-9-]{1,63}$/.test(id)) throw new CliError("invalid_service_id", "Service id must use lowercase letters, numbers, and hyphens (2-64 characters)."); return id; }
export function previewServiceScaffold(options: ServiceScaffoldOptions): Record<string, unknown> { return { directory: resolve(options.directory), serviceId: validateServiceId(options.id), dryRun: true, files: [], writes: false, runtimeMutation: false, template: templateContractPreview() }; }
export function scaffoldFiles(options: ServiceScaffoldOptions): Record<string, string> { validateServiceId(options.id); return requireAcceptedTemplateIdentity(); }
const helperName = `service-lasso-confined-scaffold${process.platform === "win32" ? ".exe" : ""}`;
function helperPath(): string {
  if (process.env.SERVICE_LASSO_CONFINED_HELPER) return process.env.SERVICE_LASSO_CONFINED_HELPER;
  // The SEA is shipped next to the accepted helper; source execution builds it
  // from its checked-in source only for local developer/test use.
  if (process.env.SERVICE_LASSO_CANDIDATE_VERSION) return resolve(process.execPath, "..", helperName);
  const helperSource = fileURLToPath(new URL("../native/confined-scaffold", import.meta.url));
  const helperBinary = resolve(helperSource, helperName);
  const build = spawnSync("go", ["build", "-trimpath", "-o", helperBinary, "."], { cwd: helperSource, encoding: "utf8" });
  if (build.status !== 0 || build.error) throw new CliError("unsafe_scaffold_destination", "The confined writer is unavailable.");
  return helperBinary;
}
async function confinedMaterialize(destination: string, files: Awaited<ReturnType<typeof acceptedTemplateFiles>>): Promise<void> {
  const input = `${Buffer.from(destination).toString("base64")}\n${files.length}\n${files.map(file => `${file.path}\t${file.mode.toString(8)}\t${file.bytes.toString("base64")}`).join("\n")}\n`;
  const stdout = await new Promise<string>((done, fail) => { const child = spawn(helperPath(), [], { stdio: ["pipe", "pipe", "ignore"], windowsHide: true }); let out = ""; child.stdout.on("data", value => { out += value; }); child.once("error", fail); child.once("close", code => code === 0 ? done(out) : fail(new Error(out))); child.stdin.end(input); });
  if (stdout.trim() === "ok") return;
  throw new CliError("unsafe_scaffold_destination", "The project destination could not be created through the confined writer.");
}
export async function createServiceScaffold(options: ServiceScaffoldOptions): Promise<ServiceScaffoldResult> { validateServiceId(options.id); const destination = resolve(options.directory); if (!options.templateRoot) return requireAcceptedTemplateIdentity(); const bundle = await loadAcceptedTemplateBundle(options.templateRoot); return materializeAcceptedTemplate(destination, bundle, Boolean(options.dryRun)); }
/** Materialize only a bundle already admitted by the source-owned resolver. */
export async function materializeAcceptedTemplate(destinationInput: string, bundle: AcceptedTemplateBundle, dryRun = false): Promise<ServiceScaffoldResult> {
  const destination = resolve(destinationInput), files = await acceptedTemplateFiles(bundle);
  if (dryRun) return { directory: destination, files: files.map(file => file.path), dryRun: true };
  if (files.some(file => !resolve(destination, file.path).startsWith(destination + sep) || relative(destination, resolve(destination, file.path)).startsWith(".."))) throw new CliError("invalid_template_bundle", "Template inventory contains an unsafe path.");
  try { await confinedMaterialize(destination, files); } catch { throw new CliError("unsafe_scaffold_destination", "The project destination could not be created through the confined writer."); }
  return { directory: destination, files: files.map(file => file.path), dryRun: false };
}
