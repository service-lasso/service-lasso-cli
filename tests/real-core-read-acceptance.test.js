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
const coreRevision = "02785268392318f14af3d0596df1ca5414957ce8";

function run(processFile, args, options = {}) {
  const child = spawn(processFile, args, { ...options, stdio: ["ignore", "pipe", "pipe"] });
  let stdout = "";
  let stderr = "";
  child.stdout.on("data", (chunk) => { stdout += chunk; });
  child.stderr.on("data", (chunk) => { stderr += chunk; });
  return { child, output: () => ({ stdout, stderr }) };
}

async function waitForCoreUrl(child) {
  const lines = readline.createInterface({ input: child.stdout });
  try {
    for await (const line of lines) {
      const message = JSON.parse(line);
      if (typeof message.url === "string") return message.url;
    }
  } catch (error) {
    throw new Error(`Core runner did not report a URL: ${error instanceof Error ? error.message : String(error)}`);
  } finally {
    lines.close();
  }
  throw new Error("Core runner exited before reporting a URL.");
}

async function stop(child) {
  if (child.exitCode !== null) return;
  child.kill("SIGTERM");
  await Promise.race([
    once(child, "exit"),
    new Promise((_, reject) => setTimeout(() => reject(new Error("Core runner did not stop.")), 10_000)),
  ]);
}

test("compiled CLI reads an ephemeral real Core instance without autostarting a service", { skip: !coreRoot }, async () => {
  assert.equal(execFileSync("git", ["-C", coreRoot, "rev-parse", "HEAD"], { encoding: "utf8" }).trim(), coreRevision);
  const root = await mkdtemp(path.join(os.tmpdir(), "service-lasso-cli-real-core-"));
  const servicesRoot = path.join(root, "services");
  const workspaceRoot = path.join(root, "workspace");
  const cliHome = path.join(root, "cli-home");
  const runnerPath = path.join(root, "start-core.mjs");
  const coreEntry = path.join(coreRoot, "dist", "server", "index.js");
  let core;

  try {
    await mkdir(path.join(servicesRoot, "read-only-service"), { recursive: true });
    await mkdir(workspaceRoot, { recursive: true });
    await mkdir(cliHome, { recursive: true });
    await writeFile(path.join(servicesRoot, "read-only-service", "service.json"), JSON.stringify({
      id: "read-only-service",
      name: "Read-only service",
      description: "Fixture manifest for CLI direct read acceptance.",
      enabled: false,
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
    const coreUrl = await waitForCoreUrl(core.child);
    const command = run(process.execPath, [path.join(cliRoot, "dist", "index.js"), "instance", "inspect", "--json"], {
      env: { ...process.env, HOME: cliHome, USERPROFILE: cliHome, SERVICE_LASSO_CORE_URL: coreUrl },
    });
    const inspectExit = await once(command.child, "exit");
    const inspectOutput = command.output();
    assert.equal(inspectExit[0], 0, inspectOutput.stderr);
    const inspect = JSON.parse(inspectOutput.stdout);
    assert.equal(inspect.instance.instance.version, "cli-real-core-read-acceptance");
    assert.equal(inspect.capabilities.capabilities.runtime.version, "cli-real-core-read-acceptance");

    const listCommand = run(process.execPath, [path.join(cliRoot, "dist", "index.js"), "service", "list", "--json"], {
      env: { ...process.env, HOME: cliHome, USERPROFILE: cliHome, SERVICE_LASSO_CORE_URL: coreUrl },
    });
    const listExit = await once(listCommand.child, "exit");
    const listOutput = listCommand.output();
    assert.equal(listExit[0], 0, listOutput.stderr);
    assert.deepEqual(JSON.parse(listOutput.stdout).services.map((service) => service.id), ["read-only-service"]);
  } finally {
    if (core) await stop(core.child).catch(() => undefined);
    await rm(root, { recursive: true, force: true });
  }
});

test("real Core acceptance pins the reviewed Core source revision", () => {
  assert.equal(coreRevision, "02785268392318f14af3d0596df1ca5414957ce8");
});
