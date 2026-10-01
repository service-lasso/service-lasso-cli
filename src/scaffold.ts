import { lstat, mkdir, rmdir, unlink, writeFile } from "node:fs/promises";
import { dirname, relative, resolve, sep } from "node:path";
import { CliError } from "./errors.js";
import { AcceptedTemplateBundle, acceptedTemplateFiles, loadAcceptedTemplateBundle, requireAcceptedTemplateIdentity, templateContractPreview } from "./template.js";
export interface ServiceScaffoldOptions { id: string; directory: string; name?: string; dryRun?: boolean; templateRoot?: string; }
export interface ServiceScaffoldResult { directory: string; files: string[]; dryRun: boolean; }
export function validateServiceId(id: string): string { if (!/^[a-z][a-z0-9-]{1,63}$/.test(id)) throw new CliError("invalid_service_id", "Service id must use lowercase letters, numbers, and hyphens (2-64 characters)."); return id; }
export function previewServiceScaffold(options: ServiceScaffoldOptions): Record<string, unknown> { return { directory: resolve(options.directory), serviceId: validateServiceId(options.id), dryRun: true, files: [], writes: false, runtimeMutation: false, template: templateContractPreview() }; }
export function scaffoldFiles(options: ServiceScaffoldOptions): Record<string, string> { validateServiceId(options.id); return requireAcceptedTemplateIdentity(); }
async function normalDirectory(path: string): Promise<void> {
  const stat = await lstat(path).catch(() => { throw new CliError("unsafe_scaffold_destination", "The project destination changed while it was being created."); });
  if (!stat.isDirectory() || stat.isSymbolicLink()) throw new CliError("unsafe_scaffold_destination", "The project destination contains a reparse point or is not a directory.");
}
async function createOwnedDirectory(path: string, directories: string[]): Promise<void> {
  try { await mkdir(path); directories.push(path); } catch (error: any) { if (error?.code === "EEXIST") await normalDirectory(path); else throw error; }
  await normalDirectory(path);
}
async function rollback(files: string[], directories: string[]): Promise<void> {
  for (const file of files.reverse()) { const stat = await lstat(file).catch(() => undefined); if (stat?.isFile() && !stat.isSymbolicLink()) await unlink(file).catch(() => undefined); }
  for (const directory of directories.reverse()) { const stat = await lstat(directory).catch(() => undefined); if (stat?.isDirectory() && !stat.isSymbolicLink()) await rmdir(directory).catch(() => undefined); }
}
export async function createServiceScaffold(options: ServiceScaffoldOptions): Promise<ServiceScaffoldResult> {
  validateServiceId(options.id); const destination = resolve(options.directory); if (!options.templateRoot) return requireAcceptedTemplateIdentity();
  const bundle = await loadAcceptedTemplateBundle(options.templateRoot);
  return materializeAcceptedTemplate(destination, bundle, Boolean(options.dryRun));
}
/** Materialize only a bundle already admitted by the source-owned resolver. */
export async function materializeAcceptedTemplate(destinationInput: string, bundle: AcceptedTemplateBundle, dryRun = false): Promise<ServiceScaffoldResult> {
  const destination = resolve(destinationInput); const files = await acceptedTemplateFiles(bundle);
  if (dryRun) return { directory: destination, files: files.map((file) => file.path), dryRun: true };
  const createdFiles: string[] = []; const createdDirectories: string[] = [];
  try {
    await normalDirectory(dirname(destination)); await createOwnedDirectory(destination, createdDirectories);
    for (const file of files) { const target = resolve(destination, file.path); if (!target.startsWith(destination + sep) || relative(destination, target).startsWith("..")) throw new CliError("invalid_template_bundle", "Template inventory contains an unsafe path."); let current = destination; for (const part of relative(destination, dirname(target)).split(sep).filter(Boolean)) { current = resolve(current, part); await normalDirectory(dirname(current)); await createOwnedDirectory(current, createdDirectories); } await normalDirectory(dirname(target)); await writeFile(target, file.bytes, { flag: "wx", mode: file.mode }); createdFiles.push(target); }
    await normalDirectory(destination);
    return { directory: destination, files: files.map((file) => file.path), dryRun: false };
  } catch (error) { await rollback(createdFiles, createdDirectories); throw error; }
}
