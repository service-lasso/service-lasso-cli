import assert from "node:assert/strict";
import { once } from "node:events";
import { createServer } from "node:http";
import { spawn } from "node:child_process";
import test from "node:test";
import { CoreClient } from "../dist/core-client.js";

const token = "local-admin-test-secret";
const registration = {
  repo: "service-lasso/lasso-node",
  tag: "v1.0.0",
  expectedCommit: "a".repeat(40),
  expectedManifestSha256: "b".repeat(64),
  idempotencyKey: "release-register-0001",
  confirm: true,
};
const completedOperationId = "sro_0123456789abcdef0123456789abcdef";
const unknownOperationId = "sro_11111111111111111111111111111111";
const malformedOperationId = "sro_22222222222222222222222222222222";

function operation(overrides = {}) {
  return {
    id: completedOperationId,
    kind: "service_registration",
    status: "completed",
    replayed: false,
    actorId: "local-admin-token",
    repo: registration.repo,
    tag: registration.tag,
    sourceCommit: registration.expectedCommit,
    serviceId: "lasso-node",
    version: "1.0.0",
    createdAt: "2026-09-30T00:00:00.000Z",
    completedAt: "2026-09-30T00:00:01.000Z",
    errorCode: null,
    ...overrides,
  };
}

async function startServer() {
  const requests = [];
  const server = createServer(async (request, response) => {
    let body = "";
    for await (const chunk of request) body += chunk;
    requests.push({ method: request.method, url: request.url, token: request.headers["x-service-lasso-admin-token"], body });
    if (request.headers["x-service-lasso-admin-token"] !== token) {
      response.writeHead(401, { "content-type": "application/json" });
      response.end(JSON.stringify({ error: "remote_auth_required", detail: token }));
      return;
    }
    if (request.method === "GET" && request.url?.endsWith(malformedOperationId)) {
      response.writeHead(200, { "content-type": "application/json" });
      response.end(JSON.stringify({ operation: { id: malformedOperationId, detail: token } }));
      return;
    }
    if (request.method === "GET" && request.url?.endsWith(unknownOperationId)) {
      response.writeHead(200, { "content-type": "application/json" });
      response.end(JSON.stringify({ operation: operation({
        id: unknownOperationId,
        status: "unknown",
        version: null,
        completedAt: null,
        errorCode: "registration_interrupted",
      }) }));
      return;
    }
    if (request.method === "GET" && request.url?.endsWith(completedOperationId)) {
      response.writeHead(200, { "content-type": "application/json" });
      response.end(JSON.stringify({ operation: operation() }));
      return;
    }
    if (request.method === "GET") {
      response.writeHead(404, { "content-type": "application/json" });
      response.end(JSON.stringify({ error: "operation_not_found", detail: token }));
      return;
    }
    const received = JSON.parse(body);
    if (received.idempotencyKey === "release-conflict-0001") {
      response.writeHead(409, { "content-type": "application/json" });
      response.end(JSON.stringify({ operation: operation({ status: "conflict", errorCode: "target_manifest_exists" }) }));
      return;
    }
    if (received.idempotencyKey === "release-malformed-conflict-0001") {
      response.writeHead(409, { "content-type": "application/json" });
      response.end(JSON.stringify({ operation: { id: completedOperationId, detail: token } }));
      return;
    }
    const replayed = requests.filter((entry) => entry.method === "POST").length > 1;
    response.writeHead(replayed ? 200 : 201, { "content-type": "application/json" });
    response.end(JSON.stringify({ operation: operation({ replayed }) }));
  });
  server.listen(0, "127.0.0.1");
  await once(server, "listening");
  return {
    baseUrl: `http://127.0.0.1:${server.address().port}`,
    requests,
    stop: () => {
      server.close();
      server.closeAllConnections();
    },
  };
}

function runCli(args, env) {
  return new Promise((resolve, reject) => {
    const child = spawn(process.execPath, ["dist/index.js", ...args], { env });
    let stdout = "";
    let stderr = "";
    child.stdout.on("data", (chunk) => { stdout += chunk; });
    child.stderr.on("data", (chunk) => { stderr += chunk; });
    child.on("error", reject);
    child.on("close", (status) => resolve({ status, stdout, stderr }));
  });
}

function registerArgs(idempotencyKey) {
  return ["--core-url", "", "service", "register", "--repo", registration.repo, "--tag", registration.tag, "--expected-commit", registration.expectedCommit, "--expected-manifest-sha256", registration.expectedManifestSha256, "--idempotency-key", idempotencyKey, "--confirm", "--json"];
}

function assertSecretSafe(result) {
  assert.doesNotMatch(`${result.stdout}${result.stderr}`, new RegExp(token));
}

test("released-service registration sends only the Core release contract and preserves replay/conflict results", async () => {
  const server = await startServer();
  try {
    const client = new CoreClient({ baseUrl: server.baseUrl, localAdminToken: token });
    const first = await client.registerReleasedService(registration);
    const replay = await client.registerReleasedService(registration);
    const conflict = await client.registerReleasedService({ ...registration, idempotencyKey: "release-conflict-0001" });
    assert.equal(first.replayed, false);
    assert.equal(replay.replayed, true);
    assert.equal(conflict.status, "conflict");
    assert.equal(server.requests[0].url, "/api/runtime/actions/importService");
    assert.equal(server.requests[0].token, token);
    assert.deepEqual(JSON.parse(server.requests[0].body), registration);
    assert.equal(server.requests.every((entry) => !entry.body.includes(token)), true);
  } finally {
    await server.stop();
  }
});

test("registration denial and unknown readback have secret-safe errors", async () => {
  const server = await startServer();
  try {
    const denied = new CoreClient({ baseUrl: server.baseUrl, localAdminToken: "wrong-token" });
    await assert.rejects(() => denied.registerReleasedService(registration), (error) => error.code === "core_api_error" && error.message === "Core returned HTTP 401." && !error.message.includes(token));
    const client = new CoreClient({ baseUrl: server.baseUrl, localAdminToken: token });
    await assert.rejects(() => client.releasedServiceOperation("sro_ffffffffffffffffffffffffffffffff"), (error) => error.code === "core_api_error" && error.message === "Core returned HTTP 404." && !error.message.includes(token));
  } finally {
    await server.stop();
  }
});

test("invalid registration input makes no request and remote cleartext credentials are rejected", async () => {
  let requests = 0;
  let readHeaders;
  const client = new CoreClient({
    baseUrl: "https://core.example",
    localAdminToken: token,
    fetch: async (_url, init) => {
      requests += 1;
      readHeaders = init.headers;
      return new Response(JSON.stringify({ ok: true }), { status: 200 });
    },
  });
  await assert.rejects(() => client.registerReleasedService({ ...registration, expectedCommit: "invalid" }), { code: "invalid_release_reference" });
  assert.equal(requests, 0);
  await client.health();
  assert.equal(readHeaders["x-service-lasso-admin-token"], undefined);
  assert.throws(() => new CoreClient({ baseUrl: "http://core.example", localAdminToken: token }), (error) => error.code === "insecure_core_token_transport" && !error.message.includes(token));
});

test("compiled command uses only environment local-admin credential and keeps it out of output", async () => {
  const server = await startServer();
  try {
    const args = registerArgs(registration.idempotencyKey);
    args[1] = server.baseUrl;
    assert.doesNotMatch(args.join(" "), new RegExp(token));
    const result = await runCli(args, {
      ...process.env,
      SERVICE_LASSO_CLI_LOCAL_ADMIN_TOKEN: token,
    });
    assert.equal(result.status, 0, result.stderr);
    assert.equal(JSON.parse(result.stdout).id, operation().id);
    assertSecretSafe(result);
  } finally {
    await server.stop();
  }
});

test("compiled registration and operation commands preserve durable states and reject hostile operation bodies", async () => {
  const server = await startServer();
  const environment = { ...process.env, SERVICE_LASSO_CLI_LOCAL_ADMIN_TOKEN: token };
  try {
    const acceptedArgs = registerArgs("release-cli-completed-0001");
    acceptedArgs[1] = server.baseUrl;
    const accepted = await runCli(acceptedArgs, environment);
    assert.equal(accepted.status, 0, accepted.stderr);
    assert.deepEqual(JSON.parse(accepted.stdout), operation());
    assertSecretSafe(accepted);

    const completed = await runCli(["--core-url", server.baseUrl, "service", "operation", completedOperationId, "--json"], environment);
    assert.equal(completed.status, 0, completed.stderr);
    assert.equal(JSON.parse(completed.stdout).status, "completed");
    assertSecretSafe(completed);

    const unknown = await runCli(["--core-url", server.baseUrl, "service", "operation", unknownOperationId, "--json"], environment);
    assert.equal(unknown.status, 0, unknown.stderr);
    assert.deepEqual(JSON.parse(unknown.stdout), operation({
      id: unknownOperationId,
      status: "unknown",
      version: null,
      completedAt: null,
      errorCode: "registration_interrupted",
    }));
    assertSecretSafe(unknown);

    const malformed = await runCli(["--core-url", server.baseUrl, "service", "operation", malformedOperationId, "--json"], environment);
    assert.equal(malformed.status, 1);
    assert.equal(malformed.stdout, "");
    assert.match(malformed.stderr, /Error \[invalid_core_response\]: Core returned an invalid service-registration operation response\./);
    assertSecretSafe(malformed);

    const conflictArgs = registerArgs("release-malformed-conflict-0001");
    conflictArgs[1] = server.baseUrl;
    const malformedConflict = await runCli(conflictArgs, environment);
    assert.equal(malformedConflict.status, 1);
    assert.equal(malformedConflict.stdout, "");
    assert.match(malformedConflict.stderr, /Error \[invalid_core_response\]: Core returned an invalid service-registration operation response\./);
    assertSecretSafe(malformedConflict);

    assert.deepEqual(server.requests.map((request) => ({ method: request.method, url: request.url })), [
      { method: "POST", url: "/api/runtime/actions/importService" },
      { method: "GET", url: `/api/operator/operations/${completedOperationId}` },
      { method: "GET", url: `/api/operator/operations/${unknownOperationId}` },
      { method: "GET", url: `/api/operator/operations/${malformedOperationId}` },
      { method: "POST", url: "/api/runtime/actions/importService" },
    ]);
    assert.equal(server.requests.every((request) => request.token === token), true);
    assert.equal(server.requests.every((request) => !request.body.includes(token)), true);
  } finally {
    await server.stop();
  }
});
