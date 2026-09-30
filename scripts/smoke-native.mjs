import assert from "node:assert/strict";
import { spawn, spawnSync } from "node:child_process";
import { readFile } from "node:fs/promises";
import { createServer } from "node:http";
import { resolve, join } from "node:path";

function argument(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing required ${name} argument.`);
  return process.argv[index + 1];
}

function nodeFreeEnvironment(extra = {}) {
  const environment = { ...process.env, ...extra };
  delete environment.NODE_OPTIONS;
  delete environment.NODE_PATH;
  if (process.platform === "win32") {
    // Windows environment variable names are case-insensitive. Retaining an
    // inherited PATH alongside Path lets Node resolve the former, leaving
    // `node` available to the smoke process on hosted Windows runners.
    delete environment.PATH;
    delete environment.Path;
    environment.Path = `${process.env.SystemRoot}\\System32;${process.env.SystemRoot}`;
  } else environment.PATH = "/usr/bin:/bin";
  return environment;
}

function run(executable, args, environment) {
  return new Promise((resolveRun, rejectRun) => {
    const child = spawn(executable, args, { env: environment });
    let stdout = "";
    let stderr = "";
    child.stdout.on("data", (chunk) => { stdout += chunk; });
    child.stderr.on("data", (chunk) => { stderr += chunk; });
    child.once("error", rejectRun);
    child.once("close", (status) => resolveRun({ status, stdout, stderr }));
  });
}

const directory = resolve(argument("--directory"));
const expectedSourceSha = argument("--expected-source-sha");
const executableName = process.platform === "win32" ? "service-lassoctl.exe" : "service-lassoctl";
const executable = join(directory, executableName);
const provenance = JSON.parse(await readFile(join(directory, "provenance.json"), "utf8"));
assert.equal(provenance.source.commit, expectedSourceSha);
assert.equal(provenance.executable.name, executableName);
assert.equal(provenance.executable.platform, process.platform);
assert.equal(provenance.executable.architecture, process.arch);

const withoutNode = nodeFreeEnvironment();
const nodeLookup = spawnSync("node", ["--version"], { encoding: "utf8", env: withoutNode });
assert.equal(nodeLookup.error?.code, "ENOENT", "native smoke must remove node from PATH");
for (const args of [
  ["--help"], ["--version"], ["operator", "--help"], ["operator", "status", "--help"],
  ["operator", "setup", "--help"], ["operator", "health", "--help"], ["operator", "dependencies", "--help"],
  ["operator", "availability", "--help"], ["operator", "preview", "--help"], ["operator", "execute", "--help"],
  ["operator", "operation", "--help"], ["operator", "operation", "get", "--help"], ["operator", "operation", "wait", "--help"],
  ["operator", "operation", "cancel", "--help"],
]) {
  const result = await run(executable, args, withoutNode);
  assert.equal(result.status, 0, result.stderr);
}

const sentinel = "native-cli-credential-sentinel";
const server = createServer((request, response) => {
  assert.equal(request.headers.authorization, `Bearer ${sentinel}`);
  response.setHeader("content-type", "application/json");
  const path = new URL(request.url, "http://127.0.0.1").pathname;
  if (path === "/api/health") return response.end(JSON.stringify({ status: "ok" }));
  if (path === "/api/runtime/instance") return response.end(JSON.stringify({
    instance: { instanceId: "native-instance", generationId: "native-generation", phase: "running", status: "active", version: "1.2.3", startedAt: "2030-01-01T00:00:00.000Z", updatedAt: "2030-01-01T00:00:01.000Z" },
    registry: { activeCount: 1, staleCount: 0, unknownCount: 0 }, privateCredential: sentinel,
  }));
  if (path === "/api/setup/status") return response.end(JSON.stringify({ setup: { contractVersion: "service-lasso.setup-status.v1", state: "setup_complete", setupMode: false, vault: { required: true, ready: true } }, privateCredential: sentinel }));
  if (path === "/api/services/demo/health") return response.end(JSON.stringify({ serviceId: "demo", health: { type: "process", healthy: true, checks: [{ id: "native-check", type: "process", required: true, healthy: true, attempts: 1 }] }, privateCredential: sentinel }));
  if (path === "/api/dependencies") return response.end(JSON.stringify({ dependencies: { nodes: [{ id: "demo" }, { id: "database" }], edges: [{ from: "database", to: "demo" }] }, privateCredential: sentinel }));
  if (path === "/api/operator/lifecycle/services/demo/availability") return response.end(JSON.stringify({ contractVersion: "service-lasso-durable-lifecycle-operation.v1", actions: [{ action: "start", available: true }], privateCredential: sentinel }));
  response.statusCode = 404;
  response.end(JSON.stringify({ error: "not_found" }));
});
await new Promise((resolveListen) => server.listen(0, "127.0.0.1", resolveListen));
try {
  const address = server.address();
  assert(address && typeof address !== "string");
  const environment = nodeFreeEnvironment({ SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${address.port}`, SERVICE_LASSO_CORE_TOKEN: sentinel });
  const read = await run(executable, ["instance", "status", "--json"], environment);
  assert.equal(read.status, 0, read.stderr);
  assert.deepEqual(JSON.parse(read.stdout), { status: "ok" });
  for (const args of [["operator", "status", "--json"], ["operator", "setup", "--json"], ["operator", "health", "demo", "--json"], ["operator", "dependencies", "demo", "--json"], ["operator", "availability", "demo", "--json"]]) {
    const operatorRead = await run(executable, args, environment);
    assert.equal(operatorRead.status, 0, operatorRead.stderr);
    assert.equal(`${operatorRead.stdout}${operatorRead.stderr}`.includes(sentinel), false);
  }
  const invalid = await run(executable, ["service", "start"], environment);
  assert.notEqual(invalid.status, 0);
  assert.doesNotMatch(`${read.stdout}${read.stderr}${invalid.stdout}${invalid.stderr}`, new RegExp(sentinel));
} finally {
  await new Promise((resolveClose, rejectClose) => server.close((error) => error ? rejectClose(error) : resolveClose()));
}

process.stdout.write(`${JSON.stringify({ command: provenance.command, executable: provenance.executable, source: provenance.source, nodeAbsentFromPath: true })}\n`);
