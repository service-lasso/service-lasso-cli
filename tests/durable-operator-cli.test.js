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

const envelope = data => ({
  contractVersion: "service-lasso-operator.v2",
  requestId: "request-v2",
  generatedAt: "2030-01-01T00:00:00.000Z",
  data,
});

test("compiled CLI uses operator/v2 preview, execute and readback", async () => {
  const calls = [];
  const confirmation = "confirm service-start 0123456789abcdef0123456789abcdef";
  const server = createServer(async (request, response) => {
    const url = new URL(request.url, "http://127.0.0.1");
    const send = (body) => { response.writeHead(200, { "content-type": "application/json" }); response.end(JSON.stringify(envelope(body))); };
    let raw = "";
    for await (const chunk of request) raw += chunk;
    calls.push({ method: request.method, path: url.pathname, body: raw ? JSON.parse(raw) : null });
    if (url.pathname === "/api/operator/v2/services/demo/preview") return send({
      planId: "plan-1", serviceId: "demo", actionId: "service_start",
      definitionRevision: "definition-1", expectedRevision: "definition-1",
      generation: "generation-1", modelEpoch: "operator-v2-epoch-1",
      targets: ["demo"], effects: ["start_runtime"],
      availability: { supported: true, available: true, reasonCode: null },
      confirmation: { required: true, id: "confirmation-1", phraseKey: confirmation, expiresAt: "2030-01-01T00:01:00.000Z" },
      expiresAt: "2030-01-01T00:01:00.000Z",
    });
    if (url.pathname === "/api/operator/v2/services/demo/execute") return send({
      kind: "accepted",
      operation: {
        operationId: "mcp-operation-fixture-0001", actionId: "service_start", serviceId: "demo",
        targets: ["demo"], status: "running", phase: "executing", outcome: null,
        correlationId: "correlation-1", createdAt: "2030-01-01T00:00:00.000Z",
        updatedAt: "2030-01-01T00:00:01.000Z", expiresAt: "2030-01-02T00:00:00.000Z",
        cancellationSupported: false,
      },
    });
    if (url.pathname === "/api/operator/v2/operations/mcp-operation-fixture-0001") return send({
      operation: {
        operationId: "mcp-operation-fixture-0001", actionId: "service_start", serviceId: "demo",
        targets: ["demo"], status: "succeeded", phase: "completed", outcome: "succeeded",
        correlationId: "correlation-1", createdAt: "2030-01-01T00:00:00.000Z",
        updatedAt: "2030-01-01T00:00:02.000Z", expiresAt: "2030-01-02T00:00:00.000Z",
        cancellationSupported: false,
      },
    });
    response.writeHead(404); response.end();
  });
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  const environment = { SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${server.address().port}`, SERVICE_LASSO_CORE_TOKEN: "fixture-token" };
  try {
    const preview = await run(["operator", "preview", "demo", "service_start", "--json"], environment);
    assert.equal(preview.code, 0, preview.stderr);
    assert.equal(JSON.parse(preview.stdout).planId, "plan-1");
    assert.deepEqual(calls.at(-1).body, {
      actionId: "service_start", definitionRevision: "definition-1", expectedRevision: "definition-1", input: {},
    });

    const denied = await run(["operator", "execute", "demo", "--plan-id", "plan-1", "--idempotency-key", "key-000000000000", "--confirmation-id", "confirmation-1", "--confirmation-response", confirmation, "--expected-revision", "definition-1", "--json"], environment);
    assert.equal(denied.code, 1);
    assert.equal(calls.filter(call => call.path.endsWith("/execute")).length, 0);

    const execute = await run(["operator", "execute", "demo", "--plan-id", "plan-1", "--idempotency-key", "key-000000000000", "--confirmation-id", "confirmation-1", "--confirmation-response", confirmation, "--expected-revision", "definition-1", "--confirm", "--json"], environment);
    assert.equal(execute.code, 0, execute.stderr);
    assert.equal(JSON.parse(execute.stdout).operation.operationId, "mcp-operation-fixture-0001");
    assert.deepEqual(calls.at(-1).body, {
      planId: "plan-1", idempotencyKey: "key-000000000000", confirmationId: "confirmation-1",
      confirmationResponse: confirmation, expectedRevision: "definition-1",
    });

    const operation = await run(["operator", "operation", "mcp-operation-fixture-0001", "--json"], environment);
    assert.equal(operation.code, 0, operation.stderr);
    assert.equal(JSON.parse(operation.stdout).operation.outcome, "succeeded");
    assert.equal(calls.at(-1).path, "/api/operator/v2/operations/mcp-operation-fixture-0001");
  } finally {
    server.close();
    await once(server, "close");
  }
});

test("compiled CLI rejects a malformed operator/v2 envelope", async () => {
  const server = createServer((request, response) => {
    response.writeHead(200, { "content-type": "application/json" });
    response.end(JSON.stringify({ contractVersion: "wrong", data: {} }));
  });
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  const environment = { SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${server.address().port}`, SERVICE_LASSO_CORE_TOKEN: "fixture-token" };
  try {
    const result = await run(["operator", "discovery", "--json"], environment);
    assert.equal(result.code, 1);
    assert.match(result.stderr, /invalid_core_response/);
  } finally {
    server.close();
    await once(server, "close");
  }
});
