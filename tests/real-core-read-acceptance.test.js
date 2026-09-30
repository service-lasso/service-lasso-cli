import assert from "node:assert/strict";
import { once } from "node:events";
import { mkdtemp, mkdir, rm, writeFile } from "node:fs/promises";
import os from "node:os";
import path from "node:path";
import { spawn } from "node:child_process";
import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";
import readline from "node:readline";
import test from "node:test";

const cliRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const coreRoot = process.env.SERVICE_LASSO_CORE_DIR;
const coreRevision = "d9e2ae799244317940c862fe1261dfd22b7bdda1";
const startupTimeoutMs = 20_000;
const shutdownTimeoutMs = 10_000;

function run(processFile, args, options = {}) {
  const child = spawn(processFile, args, { ...options, stdio: ["ignore", "pipe", "pipe"] });
  let stdout = "";
  let stderr = "";
  child.stdout.on("data", (chunk) => { stdout += chunk; });
  child.stderr.on("data", (chunk) => { stderr += chunk; });
  return { child, output: () => ({ stdout, stderr }) };
}

function within(promise, timeoutMs, message) {
  let timer;
  const timeout = new Promise((_, reject) => {
    timer = setTimeout(() => reject(new Error(message)), timeoutMs);
  });
  return Promise.race([promise, timeout]).finally(() => clearTimeout(timer));
}

async function waitForCoreUrl(child, output) {
  const lines = readline.createInterface({ input: child.stdout });
  try {
    return await within(
      (async () => {
        for await (const line of lines) {
          const message = JSON.parse(line);
          if (typeof message.url === "string") return message.url;
        }
        throw new Error("Core runner exited before reporting a URL.");
      })(),
      startupTimeoutMs,
      `Core runner did not report a URL within ${startupTimeoutMs}ms. stderr: ${output().stderr}`,
    );
  } catch (error) {
    throw new Error(`Core runner did not report a URL: ${error instanceof Error ? error.message : String(error)}`);
  } finally {
    lines.close();
  }
}

async function stop(child) {
  if (child.exitCode !== null) return;
  const waitForExit = () => within(once(child, "exit"), shutdownTimeoutMs, `Core runner did not stop within ${shutdownTimeoutMs}ms.`);
  const gracefulExit = waitForExit();
  child.kill("SIGTERM");
  try {
    await gracefulExit;
  } catch (error) {
    const forcedExit = waitForExit();
    child.kill("SIGKILL");
    await forcedExit.catch((killError) => {
      throw new AggregateError([error, killError], "Core runner did not stop and could not be forcefully terminated.");
    });
    throw new Error(`Core runner required SIGKILL during cleanup: ${error instanceof Error ? error.message : String(error)}`);
  }
}

test("compiled CLI reads an autostart-eligible real Core service without starting it", { skip: !coreRoot }, async () => {
  assert.equal(execFileSync("git", ["-C", coreRoot, "rev-parse", "HEAD"], { encoding: "utf8" }).trim(), coreRevision);
  const root = await mkdtemp(path.join(os.tmpdir(), "service-lasso-cli-real-core-"));
  const servicesRoot = path.join(root, "services");
  const workspaceRoot = path.join(root, "workspace");
  const cliHome = path.join(root, "cli-home");
  const runnerPath = path.join(root, "start-core.mjs");
  const coreEntry = path.join(coreRoot, "dist", "server", "index.js");
  let core;
  let failure;

  try {
    await mkdir(path.join(servicesRoot, "read-only-service"), { recursive: true });
    await mkdir(workspaceRoot, { recursive: true });
    await mkdir(cliHome, { recursive: true });
    await writeFile(path.join(servicesRoot, "read-only-service", "service.json"), JSON.stringify({
      id: "read-only-service",
      name: "Read-only service",
      description: "Fixture manifest for CLI direct read acceptance.",
      enabled: true,
      autostart: true,
    }, null, 2));
    await writeFile(runnerPath, `
import { startApiServer } from ${JSON.stringify(new URL(`file:///${coreEntry.replaceAll("\\\\", "/")}`).href)};
const server = await startApiServer({
  host: "127.0.0.1",
  port: 0,
  servicesRoot: process.env.CORE_SERVICES_ROOT,
  workspaceRoot: process.env.CORE_WORKSPACE_ROOT,
  autostart: false,
  noAutostart: true,
  version: "cli-real-core-read-acceptance",
});
console.log(JSON.stringify({ url: server.url }));
const shutdown = async () => { await server.stop(); process.exit(0); };
process.once("SIGTERM", () => { void shutdown(); });
process.once("SIGINT", () => { void shutdown(); });
`.trim());

    core = run(process.execPath, [runnerPath], {
      env: {
        ...process.env,
        CORE_SERVICES_ROOT: servicesRoot,
        CORE_WORKSPACE_ROOT: workspaceRoot,
      },
    });
    const coreUrl = await waitForCoreUrl(core.child, core.output);
    const command = run(process.execPath, [path.join(cliRoot, "dist", "index.js"), "instance", "inspect", "--json"], {
      env: { ...process.env, HOME: cliHome, USERPROFILE: cliHome, SERVICE_LASSO_CORE_URL: coreUrl },
    });
    const inspectExit = await once(command.child, "exit");
    const inspectOutput = command.output();
    assert.equal(inspectExit[0], 0, inspectOutput.stderr);
    const inspect = JSON.parse(inspectOutput.stdout);
    assert.equal(inspect.instance.instance.version, "cli-real-core-read-acceptance");
    assert.equal(inspect.capabilities.capabilities.runtime.version, "cli-real-core-read-acceptance");
    assert.equal(inspect.capabilities.capabilities.features.autostart, false);
    assert.deepEqual(inspect.capabilities.capabilities.baseline.serviceRoles, [{
      id: "read-only-service",
      role: "service",
      enabled: true,
      defaultBaseline: false,
    }]);

    const listCommand = run(process.execPath, [path.join(cliRoot, "dist", "index.js"), "service", "list", "--json"], {
      env: { ...process.env, HOME: cliHome, USERPROFILE: cliHome, SERVICE_LASSO_CORE_URL: coreUrl },
    });
    const listExit = await once(listCommand.child, "exit");
    const listOutput = listCommand.output();
    assert.equal(listExit[0], 0, listOutput.stderr);
    const services = JSON.parse(listOutput.stdout).services;
    assert.deepEqual(services.map((service) => service.id), ["read-only-service"]);
    assert.equal(services[0].enabled, true);
    assert.equal(services[0].lifecycle.running, false);
  } catch (error) {
    failure = error;
  }

  const cleanupFailures = [];
  if (core) {
    try {
      await stop(core.child);
    } catch (error) {
      cleanupFailures.push(error);
    }
  }
  try {
    await rm(root, { recursive: true, force: false });
  } catch (error) {
    cleanupFailures.push(error);
  }
  if (failure && cleanupFailures.length > 0) {
    throw new AggregateError([failure, ...cleanupFailures], "Real Core read acceptance failed and cleanup also failed.");
  }
  if (failure) throw failure;
  if (cleanupFailures.length > 0) {
    throw new AggregateError(cleanupFailures, "Real Core read acceptance cleanup failed.");
  }
});

test("real Core acceptance pins the reviewed Core source revision", () => {
  assert.equal(coreRevision, "d9e2ae799244317940c862fe1261dfd22b7bdda1");
});
