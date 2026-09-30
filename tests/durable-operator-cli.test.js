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

test("compiled CLI allowlists the actual durable contract shape and never spreads caller parameters", async () => {
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
