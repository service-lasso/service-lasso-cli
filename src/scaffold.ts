import { access, mkdir, writeFile } from "node:fs/promises";
import { join, resolve } from "node:path";
import { CliError } from "./errors.js";
import { assertCanonicalTemplateManifest, canonicalTemplateManifest, SERVICE_TEMPLATE_IDENTITY } from "./template.js";

export interface ServiceScaffoldOptions {
  id: string;
  directory: string;
  name?: string;
  dryRun?: boolean;
}

export interface ServiceScaffoldResult {
  directory: string;
  files: string[];
  dryRun: boolean;
}

export function validateServiceId(id: string): string {
  if (!/^[a-z][a-z0-9-]{1,63}$/.test(id)) {
    throw new CliError("invalid_service_id", "Service id must use lowercase letters, numbers, and hyphens (2-64 characters).");
  }
  return id;
}

function titleCase(id: string): string {
  return id.split("-").map((part) => `${part[0]?.toUpperCase() ?? ""}${part.slice(1)}`).join(" ");
}

export function scaffoldFiles(options: ServiceScaffoldOptions): Record<string, string> {
  const id = validateServiceId(options.id);
  const name = options.name?.trim() || titleCase(id);
  const manifest = canonicalTemplateManifest(id, name);
  assertCanonicalTemplateManifest(manifest);
  return {
    "service.json": `${JSON.stringify(manifest, null, 2)}\n`,
    ".service-lasso-template.json": `${JSON.stringify(SERVICE_TEMPLATE_IDENTITY, null, 2)}\n`,
    "README.md": `# ${name}\n\nThis project was authored from the reviewed Service Lasso template release \`${SERVICE_TEMPLATE_IDENTITY.tag}\` at \`${SERVICE_TEMPLATE_IDENTITY.commit}\`. Its canonical source \`service.json\` SHA-256 is \`${SERVICE_TEMPLATE_IDENTITY.serviceJsonSha256}\`. The scaffold applies only these safe transforms: \`id\` and \`name\`, \`enabled: false\`, and removal of the sample's mutable artifact source.\n\nBefore enabling or registering it, define an exact service artifact source and checksum, provide the real runtime package, and prove install/start/health in a clean consumer workspace. This command creates files only: it does not register, install, start, or contact Core.\n`,
    "runtime/.gitkeep": "",
  };
}

export async function createServiceScaffold(options: ServiceScaffoldOptions): Promise<ServiceScaffoldResult> {
  const directory = resolve(options.directory);
  const files = scaffoldFiles(options);
  try {
    await access(directory);
    throw new CliError("target_exists", `Refusing to overwrite existing path: ${directory}`);
  } catch (error) {
    if (error instanceof CliError) throw error;
    if (!(typeof error === "object" && error && "code" in error && (error as { code: string }).code === "ENOENT")) throw error;
  }
  const names = Object.keys(files);
  if (!options.dryRun) {
    await mkdir(directory, { recursive: false });
    for (const [relative, content] of Object.entries(files)) {
      const destination = join(directory, relative);
      await mkdir(resolve(destination, ".."), { recursive: true });
      await writeFile(destination, content, "utf8");
    }
  }
  return { directory, files: names, dryRun: Boolean(options.dryRun) };
}
