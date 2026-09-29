#!/usr/bin/env node
import { Command } from "commander";
import { pathToFileURL } from "node:url";
import { configPath, loadConfig, resolveCoreToken, resolveCoreUrl, saveConfig } from "./config.js";
import { CoreClient } from "./core-client.js";
import { asCliError, CliError } from "./errors.js";
import { createServiceScaffold } from "./scaffold.js";

interface Output {
  write(value: string): void;
  error(value: string): void;
}

const output: Output = { write: (value) => process.stdout.write(value), error: (value) => process.stderr.write(value) };

function print(value: unknown, json: boolean, sink = output): void {
  if (json) sink.write(`${JSON.stringify(value, null, 2)}\n`);
  else if (typeof value === "string") sink.write(`${value}\n`);
  else sink.write(`${JSON.stringify(value, null, 2)}\n`);
}

async function client(coreUrl?: string): Promise<CoreClient> {
  const config = await loadConfig();
  return new CoreClient({
    baseUrl: resolveCoreUrl({ cliValue: coreUrl, environment: process.env, config }),
    token: resolveCoreToken(),
  });
}

function requireConfirmation(options: { confirm?: boolean }): void {
  if (!options.confirm) throw new CliError("confirmation_required", "This action changes a running Core instance. Re-run with --confirm after reviewing the target.");
}

export function createProgram(): Command {
  const program = new Command();
  program
    .name("service-lassoctl")
    .description("Automation-first service authoring and Service Lasso Core operations.")
    .version("0.1.0")
    .option("--core-url <url>", "Service Lasso Core origin; overrides environment and saved config")
    .showSuggestionAfterError();

  const config = program.command("config").description("Read and write local CLI configuration.");
  config.command("path").description("Print the config file path.").action(() => print(configPath(), false));
  config.command("get").argument("<key>", "configuration key (core-url)").option("--json", "print JSON").action(async (key: string, options: { json?: boolean }) => {
    if (key !== "core-url") throw new CliError("unknown_config_key", `Unknown configuration key: ${key}`);
    const saved = await loadConfig();
    print({ key, value: resolveCoreUrl({ environment: process.env, config: saved }) }, Boolean(options.json));
  });
  config.command("set").argument("<key>", "configuration key (core-url)").argument("<value>", "configuration value").option("--json", "print JSON").action(async (key: string, value: string, options: { json?: boolean }) => {
    if (key !== "core-url") throw new CliError("unknown_config_key", `Unknown configuration key: ${key}`);
    await saveConfig({ coreUrl: value });
    print({ key, value: (await loadConfig()).coreUrl, path: configPath() }, Boolean(options.json));
  });

  const instance = program.command("instance").description("Read a configured Core instance.");
  instance.command("status").description("Read Core health.").option("--json", "print JSON").action(async (options: { json?: boolean }) => {
    const { coreUrl } = program.opts<{ coreUrl?: string }>();
    print(await (await client(coreUrl)).health(), Boolean(options.json));
  });
  instance.command("inspect").description("Read Core health, instance identity and API capabilities.").option("--json", "print JSON").action(async (options: { json?: boolean }) => {
    const { coreUrl } = program.opts<{ coreUrl?: string }>();
    print(await (await client(coreUrl)).inspect(), Boolean(options.json));
  });

  const service = program.command("service").description("Scaffold and manage services.");
  service.command("list").description("List services from Core.").option("--json", "print JSON").action(async (options: { json?: boolean }) => {
    const { coreUrl } = program.opts<{ coreUrl?: string }>();
    print(await (await client(coreUrl)).services(), Boolean(options.json));
  });
  for (const action of ["start", "stop", "restart"] as const) {
    service.command(action).argument("<service-id>", "managed service id").option("--confirm", "confirm this mutation").option("--json", "print JSON").description(`${action[0].toUpperCase()}${action.slice(1)} a managed service.`).action(async (serviceId: string, options: { confirm?: boolean; json?: boolean }) => {
      requireConfirmation(options);
      const { coreUrl } = program.opts<{ coreUrl?: string }>();
      print(await (await client(coreUrl)).lifecycle(serviceId, action), Boolean(options.json));
    });
  }
  service.command("init").argument("<service-id>", "lowercase service id").option("--directory <path>", "new package directory").option("--name <name>", "display name").option("--dry-run", "show files without creating them").option("--json", "print JSON").description("Create a non-destructive service-package starter.").action(async (serviceId: string, options: { directory?: string; name?: string; dryRun?: boolean; json?: boolean }) => {
    const result = await createServiceScaffold({ id: serviceId, directory: options.directory ?? `lasso-${serviceId}`, name: options.name, dryRun: options.dryRun });
    print(result, Boolean(options.json));
  });
  return program;
}

export async function run(argv = process.argv): Promise<void> {
  await createProgram().parseAsync(argv);
}

if (process.argv[1] && import.meta.url === pathToFileURL(process.argv[1]).href) {
  run().catch((error) => {
    const safe = asCliError(error);
    output.error(`Error [${safe.code}]: ${safe.message}\n`);
    if (safe.hint) output.error(`Hint: ${safe.hint}\n`);
    process.exitCode = 1;
  });
}
