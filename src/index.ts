#!/usr/bin/env node
import { Command } from "commander";
import { basename } from "node:path";
import { isSea } from "node:sea";
import { configPath, loadConfig, resolveCoreToken, resolveCoreUrl, resolveLocalAdminToken, saveConfig, updateCoreUrl, validateConnectionName } from "./config.js";
import { CoreClient, LifecycleAction } from "./core-client.js";
import { asCliError, CliError } from "./errors.js";
import { createServiceScaffold, previewServiceScaffold } from "./scaffold.js";
import { completionSource, supportedCompletionShells } from "./completion.js";

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

async function client(coreUrl?: string, connection?: string): Promise<CoreClient> {
  const config = await loadConfig();
  return new CoreClient({
    baseUrl: resolveCoreUrl({ cliValue: coreUrl, connection, environment: process.env, config }),
    token: resolveCoreToken(),
    localAdminToken: resolveLocalAdminToken(),
  });
}

function requireConfirmation(options: { confirm?: boolean }): void {
  if (!options.confirm) throw new CliError("confirmation_required", "This action changes a running Core instance. Re-run with --confirm after reviewing the target.");
}

const durableActions: LifecycleAction[] = ["install", "config", "start", "stop", "restart"];

function operationOutcome(value: unknown): "accepted" | "succeeded" | "failed" | "cancelled" | "timeout" | "uncertain" {
  const operation = value && typeof value === "object" && "operation" in value ? (value as { operation?: unknown }).operation : value;
  const outcome = operation && typeof operation === "object" ? (operation as { outcome?: unknown }).outcome : undefined;
  if (outcome === "succeeded" || outcome === "failed" || outcome === "cancelled") return outcome;
  if (outcome === "skipped") return "failed";
  if (outcome === "unknown_after_crash") return "uncertain";
  return "accepted";
}

async function waitForOperation(core: CoreClient, operationId: string, waitMs: number): Promise<unknown> {
  const deadline = Date.now() + waitMs;
  for (;;) {
    const result = await core.lifecycleOperation(operationId);
    if (operationOutcome(result) !== "accepted") return result;
    if (Date.now() >= deadline) return { operationId, outcome: "timeout", reconciliation: "read the operation by id; the client did not cancel or resubmit it" };
    await new Promise((resolve) => setTimeout(resolve, Math.min(250, deadline - Date.now())));
  }
}

function setOutcomeExit(value: unknown): void {
  const code = operationOutcome(value);
  process.exitCode = ({ accepted: 2, succeeded: 0, failed: 3, cancelled: 4, timeout: 5, uncertain: 6 } as const)[code];
}

export function createProgram(): Command {
  const program = new Command();
  program
    .name("service-lassoctl")
    .description("Automation-first service authoring and Service Lasso Core operations.")
    .version(process.env.SERVICE_LASSO_CANDIDATE_VERSION ?? "0.1.0")
    .option("--core-url <url>", "Service Lasso Core origin; overrides environment and saved config")
    .option("--connection <name>", "saved Core connection; overrides environment and default connection")
    .showSuggestionAfterError();

  program.command("completion")
    .argument("<shell>", `shell (${supportedCompletionShells.join(", ")})`)
    .description("Print static shell completion source; it never reads Core or local state.")
    .action((shell: string) => {
      if (!supportedCompletionShells.includes(shell as (typeof supportedCompletionShells)[number])) {
        throw new CliError("unsupported_completion_shell", `Unsupported completion shell: ${shell}. Choose powershell, bash, or zsh.`);
      }
      print(completionSource(shell, program), false);
    });

  const config = program.command("config").description("Read and write local CLI configuration.");
  config.command("path").description("Print the config file path.").action(() => print(configPath(), false));
  config.command("get").argument("<key>", "configuration key (core-url)").option("--json", "print JSON").action(async (key: string, options: { json?: boolean }) => {
    if (key !== "core-url") throw new CliError("unknown_config_key", `Unknown configuration key: ${key}`);
    const saved = await loadConfig();
    print({ key, value: resolveCoreUrl({ environment: process.env, config: saved }) }, Boolean(options.json));
  });
  config.command("set").argument("<key>", "configuration key (core-url)").argument("<value>", "configuration value").option("--json", "print JSON").action(async (key: string, value: string, options: { json?: boolean }) => {
    if (key !== "core-url") throw new CliError("unknown_config_key", `Unknown configuration key: ${key}`);
    await updateCoreUrl(value);
    print({ key, value: (await loadConfig()).coreUrl, path: configPath() }, Boolean(options.json));
  });

  const connection = program.command("connection").description("Manage named local Core origins without credentials.");
  connection.command("list").option("--json", "print JSON").description("List saved connection names and origins.").action(async (options: { json?: boolean }) => {
    const saved = await loadConfig();
    const values = Object.entries(saved.connections ?? {}).sort(([left], [right]) => left.localeCompare(right)).map(([name, value]) => ({ name, coreUrl: value.coreUrl, default: saved.defaultConnection === name }));
    print(values, Boolean(options.json));
  });
  connection.command("set").argument("<name>", "connection name").argument("<core-url>", "Core origin").option("--json", "print JSON").description("Save a named Core origin; credentials are never accepted.").action(async (name: string, coreUrl: string, options: { json?: boolean }) => {
    const saved = await loadConfig();
    const safeName = validateConnectionName(name);
    const connections = { ...(saved.connections ?? {}), [safeName]: { coreUrl } };
    await saveConfig({ ...saved, connections, defaultConnection: saved.defaultConnection ?? safeName });
    const updated = await loadConfig();
    print({ name: safeName, coreUrl: updated.connections?.[safeName]?.coreUrl, default: updated.defaultConnection === safeName }, Boolean(options.json));
  });
  connection.command("use").argument("<name>", "saved connection name").option("--json", "print JSON").description("Set the default saved Core connection.").action(async (name: string, options: { json?: boolean }) => {
    const saved = await loadConfig();
    const safeName = validateConnectionName(name);
    if (!saved.connections?.[safeName]) throw new CliError("unknown_connection", "The selected Core connection is not saved locally.");
    await saveConfig({ ...saved, defaultConnection: safeName });
    print({ name: safeName, default: true }, Boolean(options.json));
  });

  const instance = program.command("instance").description("Read a configured Core instance.");
  instance.command("status").description("Read Core health.").option("--json", "print JSON").action(async (options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).health(), Boolean(options.json));
  });
  instance.command("inspect").description("Read Core health, instance identity and API capabilities.").option("--json", "print JSON").action(async (options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).inspect(), Boolean(options.json));
  });

  const operator = program.command("operator").description("Use the operator/v2 contract.");
  operator.command("discovery").option("--json", "print JSON").description("Read operator/v2 discovery.").action(async (options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).operatorV2Discovery(), Boolean(options.json));
  });
  operator.command("services").option("--json", "print JSON").description("List services through operator/v2.").action(async (options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).operatorV2Services(), Boolean(options.json));
  });
  operator.command("detail").argument("<service-id>").option("--json", "print JSON").description("Read service detail through operator/v2.").action(async (serviceId: string, options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).operatorV2Detail(serviceId), Boolean(options.json));
  });
  operator.command("availability").argument("<service-id>").option("--json", "print JSON").description("Read operator/v2 lifecycle availability.").action(async (serviceId: string, options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).operatorV2LifecycleAvailability(serviceId), Boolean(options.json));
  });
  operator.command("preview").argument("<service-id>").argument("<action-id>")
    .option("--definition-revision <revision>", "definition revision", "definition-1")
    .option("--expected-revision <revision>", "expected revision", "definition-1")
    .option("--json", "print JSON")
    .description("Ask Core to preview an operator/v2 lifecycle action.")
    .action(async (serviceId: string, actionId: string, options: { definitionRevision: string; expectedRevision: string; json?: boolean }) => {
      const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
      print(await (await client(coreUrl, connection)).operatorV2Preview(serviceId, {
        actionId,
        definitionRevision: options.definitionRevision,
        expectedRevision: options.expectedRevision,
        input: {},
      }), Boolean(options.json));
    });
  operator.command("execute").argument("<service-id>")
    .requiredOption("--plan-id <id>", "server-issued plan id")
    .requiredOption("--idempotency-key <key>", "opaque idempotency key")
    .requiredOption("--confirmation-id <id>", "server-issued confirmation id")
    .requiredOption("--confirmation-response <phrase>", "server-issued confirmation phrase")
    .requiredOption("--expected-revision <revision>", "expected revision")
    .option("--confirm", "confirm this mutation")
    .option("--json", "print JSON")
    .description("Execute an operator/v2 lifecycle action.")
    .action(async (serviceId: string, options: { planId: string; idempotencyKey: string; confirmationId: string; confirmationResponse: string; expectedRevision: string; confirm?: boolean; json?: boolean }) => {
      requireConfirmation(options);
      const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
      print(await (await client(coreUrl, connection)).operatorV2Execute(serviceId, {
        planId: options.planId,
        idempotencyKey: options.idempotencyKey,
        confirmationId: options.confirmationId,
        confirmationResponse: options.confirmationResponse,
        expectedRevision: options.expectedRevision,
      }), Boolean(options.json));
    });
  operator.command("operation").argument("<operation-id>").option("--json", "print JSON").description("Read an operator/v2 durable operation.").action(async (operationId: string, options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).operatorV2Operation(operationId), Boolean(options.json));
  });

  const service = program.command("service").description("Scaffold and manage services.");
  service.command("template")
    .description("Inspect the template-contract gate without writing a project.")
    .option("--json", "print JSON")
    .action((options: { json?: boolean }) => {
      print(previewServiceScaffold({ id: "template-preview", directory: "service-template-preview", dryRun: true }), Boolean(options.json));
    });
  service.command("list").description("List services through operator/v2.").option("--json", "print JSON").action(async (options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).operatorV2Services(), Boolean(options.json));
  });
  service.command("init").argument("<service-id>", "lowercase service id").option("--directory <path>", "new package directory").option("--name <name>", "display name").option("--template-root <path>", "already-acquired accepted template bundle").option("--dry-run", "validate and list files without creating them").option("--json", "print JSON").description("Create from an accepted immutable service template.").action(async (serviceId: string, options: { directory?: string; name?: string; templateRoot?: string; dryRun?: boolean; json?: boolean }) => {
    const input = { id: serviceId, directory: options.directory ?? `lasso-${serviceId}`, name: options.name, templateRoot: options.templateRoot, dryRun: options.dryRun };
    const result = options.templateRoot ? await createServiceScaffold(input) : options.dryRun ? previewServiceScaffold(input) : await createServiceScaffold(input);
    print(result, Boolean(options.json));
  });
  service.command("register")
    .requiredOption("--repo <owner/repository>", "allowlisted release repository")
    .requiredOption("--tag <tag>", "allowlisted release tag")
    .requiredOption("--expected-commit <sha>", "full release commit SHA")
    .requiredOption("--expected-manifest-sha256 <sha256>", "canonical manifest SHA-256")
    .requiredOption("--idempotency-key <key>", "caller-supplied retry key")
    .option("--confirm", "confirm this registration")
    .option("--json", "print JSON")
    .description("Register an allowlisted released service through Core.")
    .action(async (options: { repo: string; tag: string; expectedCommit: string; expectedManifestSha256: string; idempotencyKey: string; confirm?: boolean; json?: boolean }) => {
      requireConfirmation(options);
      const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
      print(await (await client(coreUrl, connection)).registerReleasedService({ ...options, confirm: true }), Boolean(options.json));
    });
  service.command("operation")
    .argument("<operation-id>", "service registration operation id")
    .option("--json", "print JSON")
    .description("Read an actor-scoped released-service registration operation.")
    .action(async (operationId: string, options: { json?: boolean }) => {
      const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
      print(await (await client(coreUrl, connection)).releasedServiceOperation(operationId), Boolean(options.json));
    });
  return program;
}

export async function run(argv = process.argv): Promise<void> {
  await createProgram().parseAsync(argv);
}

// npm may invoke the bin through its POSIX `service-lassoctl` symlink, while
// Windows invokes the underlying `index.js`. The SEA entry imports this module
// through `sea-entry`, so neither form matches the native executable itself.
if (!isSea() && process.argv[1] && ["index.js", "service-lassoctl"].includes(basename(process.argv[1]))) {
  run().catch((error) => {
    const safe = asCliError(error);
    output.error(`Error [${safe.code}]: ${safe.message}\n`);
    if (safe.hint) output.error(`Hint: ${safe.hint}\n`);
    process.exitCode = 1;
  });
}
