import assert from "node:assert/strict";
import { once } from "node:events";
import { createServer } from "node:http";
import { spawn } from "node:child_process";
import test from "node:test";
import { fileURLToPath } from "node:url";

const cli = fileURLToPath(new URL("../dist/index.js", import.meta.url));

function run(args, env) {
  return new Promise((resolve, reject) => {
    const child = spawn(process.execPath, [cli, ...args], { env: { ...process.env, ...env }, stdio: ["ignore", "pipe", "pipe"] });
    let stdout = "";
    let stderr = "";
    child.stdout.on("data", (chunk) => { stdout += chunk; });
    child.stderr.on("data", (chunk) => { stderr += chunk; });
    child.once("error", reject);
    child.once("exit", (code) => resolve({ code, stdout, stderr }));
  });
}

test("compiled CLI allowlists the actual durable contract shape, validates availability, and never spreads caller parameters", async () => {
  await assertAvailabilityValidation();
  const state = { mutations: 0, confirmation: "confirm service-start fixture-1234567890abcdef", cancellationPosts: 0 };
  const server = createServer(async (request, response) => {
    const url = new URL(request.url, "http://127.0.0.1");
    const send = (status, body) => { response.writeHead(status, { "content-type": "application/json" }); response.end(JSON.stringify(body)); };
    if (request.headers.authorization !== "Bearer fixture-token") return send(401, { error: "denied", secret: "fixture-token" });
    if (url.pathname === "/api/operator/lifecycle/services/demo/availability") return send(200, { contractVersion: "service-lasso-durable-lifecycle-operation.v1", actions: ["install", "config", "start", "stop", "restart"].map((action) => ({ action, available: true, cancellationSupported: false })) });
    if (url.pathname === "/api/operator/lifecycle/operations" && request.method === "POST") {
      let raw = ""; for await (const chunk of request) raw += chunk;
      const body = JSON.parse(raw || "{}");
      assert.deepEqual(Object.keys(body).sort(), body.execute ? ["action", "confirmationId", "confirmationPhrase", "execute", "idempotencyKey", "serviceId"] : ["action", "serviceId"]);
      if (!body.execute) return send(200, { contractVersion: "service-lasso-mcp-guarded-action.v1", action: "service_start", confirmation: { status: "pending", id: "fixture-confirmation", expiresAt: "2030-01-01T00:00:00.000Z", confirmationPhrase: state.confirmation }, preflight: { targets: ["demo"], effects: ["start demo"], executable: true, skippedReason: null, requiredProfile: "operator" }, accidentalSecret: "must-not-print" });
      assert.equal(body.confirmationId, "fixture-confirmation");
      assert.equal(body.confirmationPhrase, state.confirmation);
      state.mutations += 1;
      return send(202, { contractVersion: "service-lasso-mcp-operation-accepted.v1", accepted: true, operation: { operationId: "mcp-operation-fixture-0001", action: "service_start", status: "running", phase: "running", progress: 20, summary: "running", targetIds: ["demo"], cancellationSupported: false, outcome: null }, accidentalSecret: "must-not-print" });
    }
    if (url.pathname === "/api/operator/lifecycle/operations/mcp-operation-fixture-0001") return send(200, { contractVersion: "service-lasso-mcp-operation.v1", operation: { operationId: "mcp-operation-fixture-0001", action: "service_start", status: "succeeded", phase: "completed", progress: 100, summary: "done", targetIds: ["demo"], cancellationSupported: false, outcome: "succeeded" }, accidentalSecret: "must-not-print" });
    if (url.pathname.endsWith("/cancel")) { state.cancellationPosts += 1; return send(500, {}); }
    return send(404, { error: "not_found" });
  });
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  const address = server.address();
  const environment = { SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${address.port}`, SERVICE_LASSO_CORE_TOKEN: "fixture-token" };
  try {
    const preview = await run(["operator", "preview", "start", "demo", "--json"], environment);
    assert.equal(preview.code, 0, preview.stderr);
    assert.deepEqual(JSON.parse(preview.stdout), { contractVersion: "service-lasso-mcp-guarded-action.v1", action: "service_start", confirmation: { id: "fixture-confirmation", expiresAt: "2030-01-01T00:00:00.000Z", phrase: state.confirmation }, preflight: { targets: ["demo"], effects: ["start demo"], executable: true, skippedReason: null, requiredProfile: "operator" }, safety: { mutating: false, redacted: true } });
    assert.equal(preview.stdout.includes("must-not-print"), false);
    const denied = await run(["operator", "execute", "start", "demo", "--confirmation-id", "fixture-confirmation", "--confirmation-phrase", state.confirmation, "--idempotency-key", "fixture-key-0001", "--json"], environment);
    assert.equal(denied.code, 1);
    assert.equal(state.mutations, 0);
    const execute = await run(["operator", "execute", "start", "demo", "--confirm", "--confirmation-id", "fixture-confirmation", "--confirmation-phrase", state.confirmation, "--idempotency-key", "fixture-key-0001", "--wait-ms", "1", "--json"], environment);
    assert.equal(execute.code, 0, execute.stderr);
    assert.equal(JSON.parse(execute.stdout).operation.outcome, "succeeded");
    assert.equal(state.mutations, 1);
    const cancelled = await run(["operator", "operation", "cancel", "mcp-operation-fixture-0001", "--json"], environment);
    assert.equal(cancelled.code, 0, cancelled.stderr);
    assert.equal(JSON.parse(cancelled.stdout).cancellation.result, "unsupported");
    assert.equal(state.cancellationPosts, 0);
  } finally {
    server.close();
    await once(server, "close");
  }
});

async function assertAvailabilityValidation() {
  const validActions = [
    { action: "start", available: true, reason: null, permission: "service-lasso:lifecycle:write", requiresConfirmation: true, privateDiagnostic: "must-not-print" },
    { action: "reload", available: false, reason: "durable_operation_unavailable", permission: "service-lasso:lifecycle:write", requiresConfirmation: true },
  ];
  const cases = [
    { name: "null entry", actions: [null] },
    { name: "truncated entry", actions: [{ action: "start" }] },
    { name: "duplicate action", actions: [{ action: "start", available: true }, { action: "start", available: false }] },
    { name: "unsupported action", actions: [{ action: "delete", available: true }] },
  ];
  let payload = { contractVersion: "service-lasso-durable-lifecycle-operation.v1", actions: validActions };
  const server = createServer((request, response) => {
    if (request.headers.authorization !== "Bearer fixture-token") { response.writeHead(401, { "content-type": "application/json" }); response.end(JSON.stringify({ secret: "fixture-token" })); return; }
    response.writeHead(200, { "content-type": "application/json" });
    response.end(JSON.stringify(payload));
  });
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  const address = server.address();
  const environment = { SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${address.port}`, SERVICE_LASSO_CORE_TOKEN: "fixture-token" };
  try {
    const extended = await run(["operator", "availability", "demo", "--json"], environment);
    assert.equal(extended.code, 0, extended.stderr);
    assert.deepEqual(JSON.parse(extended.stdout), { contractVersion: "service-lasso-durable-lifecycle-operation.v1", actions: [{ action: "start", available: true }, { action: "reload", available: false }] });
    assert.equal(extended.stdout.includes("must-not-print"), false);
    for (const malformed of cases) {
      payload = { contractVersion: "service-lasso-durable-lifecycle-operation.v1", actions: malformed.actions };
      const result = await run(["operator", "availability", "demo", "--json"], environment);
      assert.equal(result.code, 1, malformed.name);
      assert.equal(result.stdout, "", malformed.name);
      assert.match(result.stderr, /Error \[invalid_core_response\]: Core returned an invalid durable lifecycle availability response\./, malformed.name);
      assert.equal(result.stderr.includes("fixture-token"), false, malformed.name);
    }
  } finally {
    server.close();
    await once(server, "close");
  }
}

test("compiled CLI validates and redacts Core operator reads", async () => {
  let malformed = false;
  const server = createServer((request, response) => {
    const send = (body) => { response.writeHead(200, { "content-type": "application/json" }); response.end(JSON.stringify(body)); };
    if (request.headers.authorization !== "Bearer fixture-token") return send({ error: "denied" });
    const path = new URL(request.url, "http://127.0.0.1").pathname;
    if (path === "/api/runtime/instance") return send(malformed ? { registry: { activeCount: -1, staleCount: 0, unknownCount: 0 } } : { instance: { instanceId: "instance-1", generationId: "generation-1", phase: "running", status: "active", version: "1.2.3", startedAt: "2030-01-01T00:00:00.000Z", updatedAt: "2030-01-01T00:00:01.000Z", apiUrl: "https://private.example/token", workspaceRoot: "C:\\private" }, registry: { activeCount: 1, staleCount: 0, unknownCount: 0, path: "C:\\private" }, selection: { private: "must-not-print" } });
    if (path === "/api/setup/status") return send(malformed ? { setup: { contractVersion: "bad" } } : { setup: { contractVersion: "service-lasso.setup-status.v1", state: "setup_complete", setupMode: false, vault: { required: true, ready: true, path: "C:\\private" }, operator: { osUsername: "private-user" }, auth: { token: "fixture-token" } } });
    if (path === "/api/services/demo/health") return send(malformed ? { serviceId: "demo", health: { type: "process", healthy: "yes" } } : { serviceId: "demo", health: { type: "process", healthy: true, detail: "https://private.example/health", checks: [{ id: "process", type: "process", required: true, healthy: true, attempts: 1, detail: "private-token" }] }, history: { transitions: [{ detail: "private" }] } });
    if (path === "/api/dependencies") return send(malformed ? { dependencies: { nodes: [{ id: "demo" }], edges: [{ from: "private/secret", to: "demo" }] } } : { dependencies: { nodes: [{ id: "demo", name: "demo private name" }, { id: "database", name: "database" }], edges: [{ from: "database", to: "demo" }], private: "fixture-token" } });
    response.writeHead(404); response.end();
  });
  server.listen(0, "127.0.0.1"); await once(server, "listening");
  const environment = { SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${server.address().port}`, SERVICE_LASSO_CORE_TOKEN: "fixture-token" };
  const cases = [
    { args: ["operator", "status", "--json"], expected: { instance: { instanceId: "instance-1", generationId: "generation-1", phase: "running", status: "active", version: "1.2.3", startedAt: "2030-01-01T00:00:00.000Z", updatedAt: "2030-01-01T00:00:01.000Z" }, registry: { activeCount: 1, staleCount: 0, unknownCount: 0 } }, error: "operator status" },
    { args: ["operator", "setup", "--json"], expected: { setup: { contractVersion: "service-lasso.setup-status.v1", state: "setup_complete", setupMode: false, vault: { required: true, ready: true } } }, error: "setup status" },
    { args: ["operator", "health", "demo", "--json"], expected: { serviceId: "demo", health: { type: "process", healthy: true, checks: [{ id: "process", type: "process", required: true, healthy: true, attempts: 1 }] } }, error: "service health" },
    { args: ["operator", "dependencies", "demo", "--json"], expected: { serviceId: "demo", dependencies: ["database"] }, error: "service dependency" },
  ];
  try {
    for (const entry of cases) {
      const result = await run(entry.args, environment);
      assert.equal(result.code, 0, result.stderr);
      assert.deepEqual(JSON.parse(result.stdout), entry.expected);
      assert.equal(`${result.stdout}${result.stderr}`.includes("private"), false);
      assert.equal(`${result.stdout}${result.stderr}`.includes("fixture-token"), false);
    }
    malformed = true;
    for (const entry of cases) {
      const result = await run(entry.args, environment);
      assert.equal(result.code, 1, entry.error);
      assert.equal(result.stdout, "", entry.error);
      assert.match(result.stderr, /Error \[invalid_core_response\]: Core returned an invalid/, entry.error);
      assert.equal(`${result.stdout}${result.stderr}`.includes("fixture-token"), false, entry.error);
    }
  } finally { server.close(); await once(server, "close"); }
});
