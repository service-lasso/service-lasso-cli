import { access, mkdir, writeFile } from "node:fs/promises";
import { join, resolve } from "node:path";
import { CliError } from "./errors.js";

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
  return {
    "service.json": `${JSON.stringify({
      id,
      name,
      description: `Service Lasso package for ${name}.`,
      enabled: false,
      version: "0.1.0",
      logoutput: true,
      icon: [{ provider: "lucide", name: "box" }],
      meta: {
        repository: { type: "git", url: "https://github.com/service-lasso/lasso-REPLACE-ME.git" },
        tags: ["service-lasso", id],
      },
      actions: {
        install: { description: "Prepare the package runtime." },
        config: { description: "Materialize local runtime configuration." },
        start: { description: "Start the managed service." },
        stop: { description: "Stop the managed service gracefully." },
      },
      execconfig: {
        execcwd: "runtime",
        executable: "REPLACE-ME",
        depend_on: [],
      },
      healthchecks: [{ id: "process-ready", type: "process" }],
    }, null, 2)}\n`,
    "README.md": `# ${name}\n\nThis is a Service Lasso package scaffolded by \`service-lasso service init\`.\n\nBefore enabling it, replace every \`REPLACE-ME\` value, provide the real runtime package, declare supported release artifacts and checksums, and prove install/start/health in a clean consumer workspace.\n`,
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
