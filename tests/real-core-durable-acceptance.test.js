import assert from "node:assert/strict";
import { once } from "node:events";
import { createServer } from "node:http";
import { mkdtemp, mkdir, readFile, rm } from "node:fs/promises";
import { spawn, execFileSync, spawnSync } from "node:child_process";
import { createRequire } from "node:module";
import os from "node:os";
import path from "node:path";
import test from "node:test";
import { fileURLToPath, pathToFileURL } from "node:url";

const cliRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const coreRoot = process.env.SERVICE_LASSO_CORE_DIR;
const nativeExecutable = process.env.SERVICE_LASSO_CLI_NATIVE_EXE;
const coreRevision = process.env.SERVICE_LASSO_CORE_EXPECTED_REVISION;

function run(args, env) {
  return new Promise((resolve, reject) => {
    const nativeEnvironment = nativeExecutable ? nodeFreeEnvironment(env) : { ...process.env, ...env };
    const child = nativeExecutable
      ? spawn(nativeExecutable, args, { env: nativeEnvironment, stdio: ["ignore", "pipe", "pipe"] })
      : spawn(process.execPath, [path.join(cliRoot, "dist", "index.js"), ...args], { env: nativeEnvironment, stdio: ["ignore", "pipe", "pipe"] });
    let stdout = "";
    let stderr = "";
    child.stdout.on("data", (chunk) => { stdout += chunk; });
    child.stderr.on("data", (chunk) => { stderr += chunk; });
    child.once("error", reject);
    child.once("exit", (code) => resolve({ code, stdout, stderr }));
  });
}

function nodeFreeEnvironment(extra) {
  const environment = { ...process.env, ...extra };
  delete environment.NODE_OPTIONS;
  delete environment.NODE_PATH;
  if (process.platform === "win32") {
    delete environment.PATH;
    delete environment.Path;
    environment.Path = `${process.env.SystemRoot}\\System32;${process.env.SystemRoot}`;
  } else environment.PATH = "/usr/bin:/bin";
  return environment;
}

async function startJwksServer(coreRequire) {
  const { exportJWK, generateKeyPair } = coreRequire("jose");
  const { publicKey, privateKey } = await generateKeyPair("RS256");
  const jwk = await exportJWK(publicKey);
  Object.assign(jwk, { kid: "cli-durable-acceptance-key", alg: "RS256", use: "sig" });
  const server = createServer((request, response) => {
    if (request.url !== "/jwks") { response.statusCode = 404; response.end(); return; }
    response.setHeader("content-type", "application/json");
    response.end(JSON.stringify({ keys: [jwk] }));
  });
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  const address = server.address();
  return { privateKey, url: `http://127.0.0.1:${address.port}/jwks`, async stop() { const closed = once(server, "close"); server.close(); await closed; } };
}

test("compiled external CLI exercises merged actual Core durable lifecycle contract safely", { skip: !coreRoot }, async () => {
  assert.match(coreRevision ?? "", /^[a-f0-9]{40}$/, "direct Core acceptance requires the exact candidate revision");
  assert.equal(execFileSync("git", ["-C", coreRoot, "rev-parse", "HEAD"], { encoding: "utf8" }).trim(), coreRevision);
  if (nativeExecutable) {
    assert.equal(spawnSync("node", ["--version"], { env: nodeFreeEnvironment(), encoding: "utf8" }).error?.code, "ENOENT", "native Core acceptance must not find Node on the child PATH");
  }
  const coreRequire = createRequire(path.join(coreRoot, "package.json"));
  const { SignJWT } = coreRequire("jose");
  const { startApiServer } = await import(pathToFileURL(path.join(coreRoot, "dist", "server", "index.js")).href);
  const { writeExecutableFixtureService } = await import(pathToFileURL(path.join(coreRoot, "tests", "test-helpers.js")).href);
  const root = await mkdtemp(path.join(os.tmpdir(), "service-lasso-cli-durable-core-"));
  const servicesRoot = path.join(root, "services");
  const workspaceRoot = path.join(root, "workspace");
  const cliHome = path.join(root, "cli-home");
  let server;
  let jwks;
  try {
    await mkdir(servicesRoot, { recursive: true });
    await mkdir(workspaceRoot, { recursive: true });
    await mkdir(cliHome, { recursive: true });
    await mkdir(path.join(root, 'registry'), { recursive: true });
    process.env.SERVICE_LASSO_INSTANCE_REGISTRY_PATH = path.join(root, 'registry', 'instances.json');
    process.env.SERVICE_LASSO_HOST_PORT_REGISTRY_PATH = path.join(root, 'registry', 'ports.json');
    await writeExecutableFixtureService(servicesRoot, "durable-cli-service", { healthcheck: { type: "process" } });
    await writeExecutableFixtureService(servicesRoot, "durable-unrelated-service", { healthcheck: { type: "process" } });
    const unrelatedManifest = await readFile(path.join(servicesRoot, "durable-unrelated-service", "service.json"), "utf8");
    jwks = await startJwksServer(coreRequire);
    const now = Math.floor(Date.now() / 1000);
    const token = await new SignJWT({ client_id: "cli-durable-client", scope: "service-lasso:read service-lasso:lifecycle:write" })
      .setProtectedHeader({ alg: "RS256", kid: "cli-durable-acceptance-key" })
      .setIssuer("https://cli-durable-issuer.example")
      .setAudience("cli-durable-audience")
      .setSubject("cli-durable-actor")
      .setIssuedAt(now)
      .setExpirationTime(now + 300)
      .sign(jwks.privateKey);
    server = await startApiServer({
      host: "127.0.0.1",
      port: 0,
      servicesRoot,
      workspaceRoot,
      autostart: false,
      noAutostart: true,
      mcpHttpIdentity: { env: {
        SERVICE_LASSO_MCP_MODE: "guarded",
        SERVICE_LASSO_MCP_OAUTH_ISSUER: "https://cli-durable-issuer.example",
        SERVICE_LASSO_MCP_OAUTH_JWKS_URI: jwks.url,
        SERVICE_LASSO_MCP_RESOURCE_URI: "https://cli-durable-resource.example/api/mcp",
        SERVICE_LASSO_MCP_OAUTH_AUDIENCE: "cli-durable-audience",
      } },
    });
    const environment = { HOME: cliHome, USERPROFILE: cliHome, SERVICE_LASSO_CORE_URL: server.url, SERVICE_LASSO_CORE_TOKEN: token };

    const preview = await run(["operator", "preview", "durable-cli-service", "service_start", "--json"], environment);
    assert.equal(preview.code, 0, preview.stderr);
    const plan = JSON.parse(preview.stdout);
    assert.equal(plan.serviceId, "durable-cli-service");
    assert.equal(plan.actionId, "service_start");
    assert.equal(typeof plan.planId, "string");
    assert.equal(typeof plan.confirmation.id, "string");
    assert.match(plan.confirmation.phraseKey, /^confirm service-start /);
    assert.equal(preview.stdout.includes(token), false);

    const rejected = await run(["operator", "availability", "durable-cli-service", "--json"], { ...environment, SERVICE_LASSO_CORE_TOKEN: "invalid-token-must-never-appear" });
    assert.equal(rejected.code, 1);
    assert.match(rejected.stderr, /Error \[core_api_error\]: Core returned HTTP 401\./);
    assert.equal(rejected.stderr.includes("invalid-token-must-never-appear"), false);

    await writeExecutableFixtureService(servicesRoot, "durable-cli-service", { healthcheck: { type: "process" }, env: { DURABLE_CONTEXT: "changed" } });
    const changed = await run([
      "operator", "execute", "durable-cli-service",
      "--plan-id", plan.planId,
      "--idempotency-key", "cli-durable-context-key-0001",
      "--confirmation-id", plan.confirmation.id,
      "--confirmation-response", plan.confirmation.phraseKey,
      "--expected-revision", plan.expectedRevision,
      "--confirm", "--json",
    ], environment);
    assert.equal(changed.code, 1);
    assert.match(changed.stderr, /Error \[core_api_error\]: Core returned HTTP 409\./);
    assert.equal(changed.stderr.includes(plan.confirmation.phraseKey), false);
    await writeExecutableFixtureService(servicesRoot, "durable-cli-service", { healthcheck: { type: "process" } });

    const acceptedPreview = await run(["operator", "preview", "durable-cli-service", "service_start", "--json"], environment);
    assert.equal(acceptedPreview.code, 0, acceptedPreview.stderr);
    const acceptedPlan = JSON.parse(acceptedPreview.stdout);
    const executeArgs = [
      "operator", "execute", "durable-cli-service",
      "--plan-id", acceptedPlan.planId,
      "--idempotency-key", "cli-durable-replay-key-0001",
      "--confirmation-id", acceptedPlan.confirmation.id,
      "--confirmation-response", acceptedPlan.confirmation.phraseKey,
      "--expected-revision", acceptedPlan.expectedRevision,
      "--confirm", "--json",
    ];
    const execute = await run(executeArgs, environment);
    assert.equal(execute.code, 0, execute.stderr);
    const operationId = JSON.parse(execute.stdout).operation.operationId;
    let readback;
    for (let attempt = 0; attempt < 100; attempt += 1) {
      const inspected = await run(["operator", "operation", operationId, "--json"], environment);
      assert.equal(inspected.code, 0, inspected.stderr);
      readback = JSON.parse(inspected.stdout);
      if (!["queued", "running", "cancelling"].includes(readback.operation.status)) break;
      await new Promise((resolve) => setTimeout(resolve, 100));
    }
    assert.equal(readback.operation.status, "succeeded");
    const replay = await run(executeArgs, environment);
    assert.equal(replay.code, 0, replay.stderr);
    assert.equal(JSON.parse(replay.stdout).operation.operationId, operationId);
    const cancellation = await run(["operator", "cancel", operationId, "--confirm", "--json"], environment);
    assert.equal(cancellation.code, 0, cancellation.stderr);
    assert.ok(["requested", "unsupported", "too_late"].includes(JSON.parse(cancellation.stdout).status));
    assert.equal(await readFile(path.join(servicesRoot, "durable-unrelated-service", "service.json"), "utf8"), unrelatedManifest);
  } finally {
    await server?.stop().catch(() => undefined);
    await jwks?.stop().catch(() => undefined);
    await rm(root, { recursive: true, force: false });
  }
});
test("real durable acceptance requires an explicit Core candidate pin", () => {
  assert.ok(coreRevision === undefined || /^[a-f0-9]{40}$/.test(coreRevision));
});
