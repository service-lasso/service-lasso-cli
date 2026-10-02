import assert from "node:assert/strict";
import { spawn, spawnSync } from "node:child_process";
import { createHash } from "node:crypto";
import { readFile, writeFile } from "node:fs/promises";
import { createServer } from "node:http";
import { resolve, join } from "node:path";
import { nativeQualificationStdio } from "./native-qualification-stdio.mjs";

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
    const child = spawn(executable, args, { env: environment, stdio: nativeQualificationStdio(["pipe", "pipe", "pipe"], environment) });
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
const expectedVersion = argument("--expected-version");
assert.match(expectedSourceSha, /^[0-9a-f]{40}$/i, "expected source SHA must be a full Git revision");
assert.match(expectedVersion, /^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i, "expected version must be the frozen candidate version");
const executableName = process.platform === "win32" ? "service-lassoctl.exe" : "service-lassoctl";
const executable = join(directory, executableName);
const provenance = JSON.parse(await readFile(join(directory, "provenance.json"), "utf8"));
assert.equal(provenance.source.commit, expectedSourceSha);
assert.equal(provenance.executable.name, executableName);
assert.equal(provenance.executable.platform, process.platform);
assert.equal(provenance.executable.architecture, process.arch);
assert.equal(provenance.candidate.version, expectedVersion);
assert.equal(provenance.executable.version, expectedVersion);
assert.equal(provenance.confinedWriter.platform, process.platform);
assert.equal(provenance.confinedWriter.architecture, process.arch);
if (process.platform === "darwin") {
  assert.deepEqual(provenance.darwinImmutableHelper?.platform, "darwin");
  assert.equal(provenance.darwinImmutableHelper?.architecture, "arm64");
  const helper = join(directory, provenance.darwinImmutableHelper.name);
  assert.equal(createHash("sha256").update(await readFile(helper)).digest("hex"), provenance.darwinImmutableHelper.sha256, "Darwin privileged helper must retain its accepted digest");
}
const confinedWriter = join(directory, provenance.confinedWriter.name);
assert.equal(createHash("sha256").update(await readFile(confinedWriter)).digest("hex"), provenance.confinedWriter.sha256, "confined writer must retain its accepted digest");

const withoutNode = nodeFreeEnvironment();
const nodeLookup = spawnSync("node", ["--version"], { encoding: "utf8", env: withoutNode });
assert.equal(nodeLookup.error?.code, "ENOENT", "native smoke must remove node from PATH");
const helperSmoke = await run(confinedWriter, ["--self-test"], withoutNode);
assert.equal(helperSmoke.status, 0, helperSmoke.stderr);
assert.equal(helperSmoke.stdout.trim(), "ok");
const versionResult = await run(executable, ["--version"], withoutNode);
assert.equal(versionResult.status, 0, versionResult.stderr);
assert.equal(versionResult.stdout.trim(), expectedVersion, "native executable must report the exact frozen candidate version");
for (const args of [
  ["--help"], ["operator", "--help"], ["operator", "status", "--help"],
  ["operator", "setup", "--help"], ["operator", "health", "--help"], ["operator", "dependencies", "--help"],
  ["operator", "availability", "--help"], ["operator", "preview", "--help"], ["operator", "execute", "--help"],
  ["operator", "operation", "--help"], ["operator", "operation", "get", "--help"], ["operator", "operation", "wait", "--help"],
  ["operator", "operation", "cancel", "--help"],
]) {
  const result = await run(executable, args, withoutNode);
  assert.equal(result.status, 0, result.stderr);
}

const sentinel = "native-cli-credential-sentinel";
const fixture = { preview: 0, effect: "stopped", operationId: "native-durable-operation-0001" };
async function requestJson(request) {
  let body = "";
  for await (const chunk of request) body += chunk;
  return body ? JSON.parse(body) : {};
}
function operation() {
  return { contractVersion: "service-lasso-mcp-operation.v1", operation: { operationId: fixture.operationId, action: "service_start", status: "succeeded", phase: "completed", progress: 100, summary: "started", targetIds: ["demo"], cancellationSupported: false, outcome: "succeeded" } };
}
const server = createServer(async (request, response) => {
  if (request.headers.authorization !== `Bearer ${sentinel}`) {
    response.statusCode = 401;
    response.end(JSON.stringify({ error: "denied", privateCredential: sentinel }));
    return;
  }
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
  if (path === "/api/operator/lifecycle/operations" && request.method === "POST") {
    const body = await requestJson(request);
    if (body.execute !== true) {
      fixture.preview += 1;
      const changed = fixture.preview === 1;
      return response.end(JSON.stringify({ contractVersion: "service-lasso-mcp-guarded-action.v1", action: "service_start", confirmation: { status: "pending", id: changed ? "native-confirmation-stale" : "native-confirmation-current", expiresAt: "2030-01-01T00:10:00.000Z", confirmationPhrase: changed ? "confirm service-start stale" : "confirm service-start current" }, preflight: { targets: ["demo"], effects: ["start demo"], executable: true, skippedReason: null, requiredProfile: "operator" }, privateCredential: sentinel }));
    }
    if (body.confirmationId === "native-confirmation-stale") {
      response.statusCode = 409;
      return response.end(JSON.stringify({ error: "changed_context", privateCredential: sentinel }));
    }
    assert.deepEqual(body, { action: "start", serviceId: "demo", execute: true, idempotencyKey: "native-durable-key-0001", confirmationId: "native-confirmation-current", confirmationPhrase: "confirm service-start current" });
    fixture.effect = "running";
    return response.end(JSON.stringify(operation()));
  }
  if (path === `/api/operator/lifecycle/operations/${fixture.operationId}` && request.method === "GET") return response.end(JSON.stringify(operation()));
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
  const denied = await run(executable, ["operator", "availability", "demo", "--json"], nodeFreeEnvironment({ SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${address.port}`, SERVICE_LASSO_CORE_TOKEN: "native-invalid-token" }));
  assert.equal(denied.status, 1);
  assert.match(denied.stderr, /Error \[core_api_error\]: Core returned HTTP 401\./);
  assert.equal(`${denied.stdout}${denied.stderr}`.includes("native-invalid-token"), false);
  const stalePreview = await run(executable, ["operator", "preview", "start", "demo", "--json"], environment);
  assert.equal(stalePreview.status, 0, stalePreview.stderr);
  const stale = JSON.parse(stalePreview.stdout);
  const changed = await run(executable, ["operator", "execute", "start", "demo", "--confirm", "--confirmation-id", stale.confirmation.id, "--confirmation-phrase", stale.confirmation.phrase, "--idempotency-key", "native-durable-key-0001", "--json"], environment);
  assert.equal(changed.status, 1);
  assert.match(changed.stderr, /Error \[core_api_error\]: Core returned HTTP 409\./);
  assert.equal(`${changed.stdout}${changed.stderr}`.includes(stale.confirmation.phrase), false);
  const currentPreview = await run(executable, ["operator", "preview", "start", "demo", "--json"], environment);
  assert.equal(currentPreview.status, 0, currentPreview.stderr);
  const current = JSON.parse(currentPreview.stdout);
  const executeArgs = ["operator", "execute", "start", "demo", "--confirm", "--confirmation-id", current.confirmation.id, "--confirmation-phrase", current.confirmation.phrase, "--idempotency-key", "native-durable-key-0001", "--wait-ms", "10000", "--json"];
  const execute = await run(executable, executeArgs, environment);
  assert.equal(execute.status, 0, execute.stderr);
  const accepted = JSON.parse(execute.stdout);
  assert.equal(accepted.operation.operationId, fixture.operationId);
  assert.equal(fixture.effect, "running");
  const replay = await run(executable, executeArgs, environment);
  assert.equal(replay.status, 0, replay.stderr);
  assert.equal(JSON.parse(replay.stdout).operation.operationId, fixture.operationId);
  const inspected = await run(executable, ["operator", "operation", "get", fixture.operationId, "--json"], environment);
  assert.equal(inspected.status, 0, inspected.stderr);
  const waited = await run(executable, ["operator", "operation", "wait", fixture.operationId, "--wait-ms", "10000", "--json"], environment);
  assert.equal(waited.status, 0, waited.stderr);
  const cancellation = await run(executable, ["operator", "operation", "cancel", fixture.operationId, "--json"], environment);
  assert.equal(cancellation.status, 0, cancellation.stderr);
  assert.equal(JSON.parse(cancellation.stdout).cancellation.result, "unsupported");
} finally {
  await new Promise((resolveClose, rejectClose) => server.close((error) => error ? rejectClose(error) : resolveClose()));
}

if (process.argv.includes("--write-host-acceptance")) {
  const executableSha256 = createHash("sha256").update(await readFile(executable)).digest("hex");
  assert.equal(executableSha256, provenance.executable.sha256, "accepted executable must retain its provenance digest");
  const identity = `service-lasso-native-acceptance-v1\n${expectedSourceSha}\n${expectedVersion}\n${process.platform}\n${process.arch}\n${executableSha256}\nnode-absent\npassed\n`;
  const acceptance = { schemaVersion: 1, sourceSha: expectedSourceSha, version: expectedVersion, platform: process.platform, architecture: process.arch, executableSha256, nodeAbsentFromPath: true, status: "passed", evidenceDigest: createHash("sha256").update(Buffer.from(identity, "utf8")).digest("hex") };
  await writeFile(join(directory, "host-acceptance.json"), `${JSON.stringify(acceptance, null, 2)}\n`);
  process.stdout.write(`${JSON.stringify({ ...acceptance, emittedAfter: "native-no-node-fixture" })}\n`);
}
