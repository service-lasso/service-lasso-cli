#!/usr/bin/env node
import { Command } from "commander";
import { realpathSync } from "node:fs";
import { pathToFileURL } from "node:url";
import { configPath, loadConfig, resolveCoreToken, resolveCoreUrl, resolveLocalAdminToken, saveConfig, validateConnectionName } from "./config.js";
import { CoreClient, LifecycleAction } from "./core-client.js";
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

const durableActions: LifecycleAction[] = ["install", "configure", "start", "stop", "restart"];

function parseParameters(value: string | undefined): Record<string, unknown> {
  if (!value) return {};
  try {
    const parsed: unknown = JSON.parse(value);
    if (!parsed || typeof parsed !== "object" || Array.isArray(parsed)) throw new Error();
    return parsed as Record<string, unknown>;
  } catch {
    throw new CliError("invalid_parameters", "Parameters must be a JSON object.");
  }
}

function operationOutcome(value: unknown): "accepted" | "succeeded" | "failed" | "cancelled" | "timeout" | "uncertain" {
  const operation = value && typeof value === "object" && "operation" in value ? (value as { operation?: unknown }).operation : value;
  const outcome = operation && typeof operation === "object" ? (operation as { outcome?: unknown }).outcome : undefined;
  if (outcome === "succeeded" || outcome === "failed" || outcome === "cancelled") return outcome;
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
    .version("0.1.0")
    .option("--core-url <url>", "Service Lasso Core origin; overrides environment and saved config")
    .option("--connection <name>", "saved Core connection; overrides environment and default connection")
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

  const operator = program.command("operator").description("Use the server-authoritative durable lifecycle contract.");
  operator.command("status").option("--json", "print JSON").description("Read runtime status.").action(async (options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).operatorStatus(), Boolean(options.json));
  });
  operator.command("setup").option("--json", "print JSON").description("Read Core setup state.").action(async (options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).setup(), Boolean(options.json));
  });
  operator.command("health").argument("<service-id>").option("--json", "print JSON").description("Read service health.").action(async (serviceId: string, options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).serviceHealth(serviceId), Boolean(options.json));
  });
  operator.command("dependencies").argument("<service-id>").option("--json", "print JSON").description("Read service dependency state.").action(async (serviceId: string, options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).serviceDependencies(serviceId), Boolean(options.json));
  });
  operator.command("availability").argument("<service-id>").option("--json", "print JSON").description("Read supported durable lifecycle actions.").action(async (serviceId: string, options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).availability(serviceId), Boolean(options.json));
  });
  operator.command("preview").argument("<action>", "install, configure, start, stop, or restart").argument("<service-id>").option("--parameters-json <json>", "exact action parameters as a JSON object").option("--json", "print JSON").description("Ask Core to validate an action and issue a bound confirmation.").action(async (action: LifecycleAction, serviceId: string, options: { parametersJson?: string; json?: boolean }) => {
    if (!durableActions.includes(action)) throw new CliError("invalid_action", "Action must be install, configure, start, stop, or restart.");
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).previewLifecycle(serviceId, action, parseParameters(options.parametersJson)), Boolean(options.json));
  });
  operator.command("execute").argument("<action>", "install, configure, start, stop, or restart").argument("<service-id>")
    .requiredOption("--confirmation-id <id>", "server-issued confirmation id").requiredOption("--confirmation-phrase <phrase>", "server-issued bound confirmation phrase")
    .requiredOption("--idempotency-key <key>", "caller-supplied opaque idempotency key").option("--parameters-json <json>", "exact action parameters as a JSON object")
    .option("--wait-ms <milliseconds>", "bounded readback wait after acceptance", Number).option("--confirm", "record local execution intent; does not bypass Core").option("--json", "print JSON")
    .description("Submit a server-confirmed durable lifecycle operation.").action(async (action: LifecycleAction, serviceId: string, options: { confirmationId: string; confirmationPhrase: string; idempotencyKey: string; parametersJson?: string; waitMs?: number; confirm?: boolean; json?: boolean }) => {
      if (!durableActions.includes(action)) throw new CliError("invalid_action", "Action must be install, configure, start, stop, or restart.");
      requireConfirmation(options);
      if (options.waitMs !== undefined && (!Number.isSafeInteger(options.waitMs) || options.waitMs < 0 || options.waitMs > 120_000)) throw new CliError("invalid_wait", "Wait must be an integer from 0 to 120000 milliseconds.");
      const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
      const core = await client(coreUrl, connection);
      // This read is deliberately outside the uncertain mutation boundary: a
      // transport failure here has not submitted an action.
      const availability = await core.availability(serviceId);
      let accepted: unknown;
      try { accepted = await core.executeLifecycle({ action, serviceId, confirmationId: options.confirmationId, confirmationPhrase: options.confirmationPhrase, idempotencyKey: options.idempotencyKey, parameters: parseParameters(options.parametersJson) }, availability); }
      catch (error) { if (error instanceof CliError && error.code === "core_unreachable") { const uncertain = { outcome: "uncertain", reconciliation: "read the operation by id if it was returned before disconnect; do not resubmit automatically" }; print(uncertain, Boolean(options.json)); setOutcomeExit(uncertain); return; } throw error; }
      const operation = accepted && typeof accepted === "object" && "operation" in accepted ? (accepted as { operation?: { operationId?: unknown } }).operation : undefined;
      const operationId = operation && typeof operation.operationId === "string" ? operation.operationId : undefined;
      const result = options.waitMs && operationId ? await waitForOperation(core, operationId, options.waitMs) : accepted;
      print(result, Boolean(options.json)); setOutcomeExit(result);
    });
  const operation = operator.command("operation").description("Inspect, wait for, or cancel a durable lifecycle operation.");
  operation.command("get").argument("<operation-id>").option("--json", "print JSON").action(async (operationId: string, options: { json?: boolean }) => { const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>(); const result = await (await client(coreUrl, connection)).lifecycleOperation(operationId); print(result, Boolean(options.json)); setOutcomeExit(result); });
  operation.command("wait").argument("<operation-id>").requiredOption("--wait-ms <milliseconds>", "bounded wait", Number).option("--json", "print JSON").action(async (operationId: string, options: { waitMs: number; json?: boolean }) => { if (!Number.isSafeInteger(options.waitMs) || options.waitMs < 0 || options.waitMs > 120_000) throw new CliError("invalid_wait", "Wait must be an integer from 0 to 120000 milliseconds."); const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>(); const result = await waitForOperation(await client(coreUrl, connection), operationId, options.waitMs); print(result, Boolean(options.json)); setOutcomeExit(result); });
  operation.command("cancel").argument("<operation-id>").option("--json", "print JSON").action(async (operationId: string, options: { json?: boolean }) => { const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>(); const core = await client(coreUrl, connection); const current = await core.lifecycleOperation(operationId) as { operation?: { cancellationSupported?: boolean } }; const result = await core.cancelLifecycleOperation(operationId, current.operation?.cancellationSupported === true); print(result, Boolean(options.json)); });

  const service = program.command("service").description("Scaffold and manage services.");
  service.command("list").description("List services from Core.").option("--json", "print JSON").action(async (options: { json?: boolean }) => {
    const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
    print(await (await client(coreUrl, connection)).services(), Boolean(options.json));
  });
  for (const action of ["start", "stop", "restart"] as const) {
    service.command(action).argument("<service-id>", "managed service id").option("--confirm", "confirm this mutation").option("--json", "print JSON").description(`${action[0].toUpperCase()}${action.slice(1)} a managed service.`).action(async (serviceId: string, options: { confirm?: boolean; json?: boolean }) => {
      requireConfirmation(options);
      const { coreUrl, connection } = program.opts<{ coreUrl?: string; connection?: string }>();
      print(await (await client(coreUrl, connection)).lifecycle(serviceId, action), Boolean(options.json));
    });
  }
  service.command("init").argument("<service-id>", "lowercase service id").option("--directory <path>", "new package directory").option("--name <name>", "display name").option("--dry-run", "show files without creating them").option("--json", "print JSON").description("Create a non-destructive service-package starter.").action(async (serviceId: string, options: { directory?: string; name?: string; dryRun?: boolean; json?: boolean }) => {
    const result = await createServiceScaffold({ id: serviceId, directory: options.directory ?? `lasso-${serviceId}`, name: options.name, dryRun: options.dryRun });
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

if (process.argv[1] && import.meta.url === pathToFileURL(realpathSync(process.argv[1])).href) {
  run().catch((error) => {
    const safe = asCliError(error);
    output.error(`Error [${safe.code}]: ${safe.message}\n`);
    if (safe.hint) output.error(`Hint: ${safe.hint}\n`);
    process.exitCode = 1;
  });
}
