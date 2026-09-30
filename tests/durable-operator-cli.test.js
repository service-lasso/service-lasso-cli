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

test("compiled service-lassoctl uses preview, one durable mutation, wait, health and operation reconciliation against an owned loopback fixture", async () => {
  const state = { mutations: 0, unrelated: "unchanged", reads: 0, confirmation: "fixture-phrase-not-a-credential" };
  const server = createServer(async (request, response) => {
    const url = new URL(request.url, "http://127.0.0.1");
    const send = (status, body) => { response.writeHead(status, { "content-type": "application/json" }); response.end(JSON.stringify(body)); };
    if (request.headers.authorization !== "Bearer fixture-token") return send(401, { error: "denied" });
    if (url.pathname === "/api/operator/lifecycle/services/demo/availability") return send(200, { contractVersion: "service-lasso-durable-lifecycle-operation.v1", actions: ["install", "configure", "start", "stop", "restart"].map((action) => ({ action, available: true, cancellationSupported: false })) });
    if (url.pathname === "/api/operator/lifecycle/operations" && request.method === "POST") {
      let raw = ""; for await (const chunk of request) raw += chunk;
      const body = JSON.parse(raw || "{}");
      if (!body.execute) return send(200, { confirmation: { status: "pending", id: "fixture-confirmation", confirmationPhrase: state.confirmation }, preflight: { mutated: false } });
      assert.equal(body.confirmationId, "fixture-confirmation");
      assert.equal(body.confirmationPhrase, state.confirmation);
      assert.equal(body.idempotencyKey, "fixture-key-0001");
      state.mutations += 1;
      return send(202, { accepted: true, operation: { operationId: "fixture-operation-0001", cancellationSupported: false, outcome: null } });
    }
    if (url.pathname === "/api/operator/lifecycle/operations/fixture-operation-0001") {
      state.reads += 1;
      return send(200, { operation: { operationId: "fixture-operation-0001", cancellationSupported: false, outcome: state.reads > 1 ? "succeeded" : null } });
    }
    if (url.pathname === "/api/services/demo/health") return send(200, { serviceId: "demo", status: "healthy" });
    return send(404, { error: "not_found" });
  });
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  const address = server.address();
  const environment = { SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${address.port}`, SERVICE_LASSO_CORE_TOKEN: "fixture-token" };
  try {
    const preview = await run(["operator", "preview", "start", "demo", "--json"], environment);
    assert.equal(preview.code, 0, preview.stderr);
    assert.equal(JSON.parse(preview.stdout).preflight.mutated, false);
    const denied = await run(["operator", "execute", "start", "demo", "--confirmation-id", "fixture-confirmation", "--confirmation-phrase", state.confirmation, "--idempotency-key", "fixture-key-0001", "--json"], environment);
    assert.equal(denied.code, 1);
    assert.equal(state.mutations, 0);
    const execute = await run(["operator", "execute", "start", "demo", "--confirm", "--confirmation-id", "fixture-confirmation", "--confirmation-phrase", state.confirmation, "--idempotency-key", "fixture-key-0001", "--wait-ms", "1000", "--json"], environment);
    assert.equal(execute.code, 0, execute.stderr);
    assert.equal(JSON.parse(execute.stdout).operation.outcome, "succeeded");
    assert.equal(state.mutations, 1);
    const reconciled = await run(["operator", "operation", "get", "fixture-operation-0001", "--json"], environment);
    assert.equal(reconciled.code, 0, reconciled.stderr);
    assert.equal(JSON.parse(reconciled.stdout).operation.outcome, "succeeded");
    const health = await run(["operator", "health", "demo", "--json"], environment);
    assert.equal(health.code, 0, health.stderr);
    assert.equal(JSON.parse(health.stdout).status, "healthy");
    assert.equal(state.unrelated, "unchanged");
  } finally {
    server.close();
    await once(server, "close");
  }
});
