import { mkdir, rm, writeFile } from "node:fs/promises";
import { dirname, relative, resolve, sep } from "node:path";
import { CliError } from "./errors.js";
import { acceptedTemplateFiles, loadAcceptedTemplateBundle, requireAcceptedTemplateIdentity, templateContractPreview } from "./template.js";
export interface ServiceScaffoldOptions { id: string; directory: string; name?: string; dryRun?: boolean; templateRoot?: string; }
export interface ServiceScaffoldResult { directory: string; files: string[]; dryRun: boolean; }
export function validateServiceId(id: string): string { if (!/^[a-z][a-z0-9-]{1,63}$/.test(id)) throw new CliError("invalid_service_id", "Service id must use lowercase letters, numbers, and hyphens (2-64 characters)."); return id; }
export function previewServiceScaffold(options: ServiceScaffoldOptions): Record<string, unknown> { return { directory: resolve(options.directory), serviceId: validateServiceId(options.id), dryRun: true, files: [], writes: false, runtimeMutation: false, template: templateContractPreview() }; }
export function scaffoldFiles(options: ServiceScaffoldOptions): Record<string, string> { validateServiceId(options.id); return requireAcceptedTemplateIdentity(); }
export async function createServiceScaffold(options: ServiceScaffoldOptions): Promise<ServiceScaffoldResult> {
  validateServiceId(options.id); const destination = resolve(options.directory); if (!options.templateRoot) return requireAcceptedTemplateIdentity();
  const bundle = await loadAcceptedTemplateBundle(options.templateRoot); const files = await acceptedTemplateFiles(bundle);
  if (options.dryRun) return { directory: destination, files: files.map((file) => file.path), dryRun: true };
  let created = false;
  try {
    await mkdir(destination); created = true;
    for (const file of files) { const target = resolve(destination, file.path); if (!target.startsWith(destination + sep) || relative(destination, target).startsWith("..")) throw new CliError("invalid_template_bundle", "Template inventory contains an unsafe path."); await mkdir(dirname(target), { recursive: true }); await writeFile(target, file.bytes, { flag: "wx", mode: file.mode }); }
    return { directory: destination, files: files.map((file) => file.path), dryRun: false };
  } catch (error) { if (created) await rm(destination, { recursive: true, force: true, maxRetries: 0 }); throw error; }
}
