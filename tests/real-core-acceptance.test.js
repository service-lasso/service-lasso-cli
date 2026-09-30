import assert from "node:assert/strict";
import { execFile } from "node:child_process";
import { createServer } from "node:http";
import { promisify } from "node:util";
import test from "node:test";
import { fileURLToPath } from "node:url";

const runFile = promisify(execFile);
const cli = fileURLToPath(new URL("../dist/index.js", import.meta.url));
const fixtureToken = "fixture-token-not-for-output";
const fixtureSensitiveBody = "fixture-response-not-for-output";

async function withAuthenticatedCoreFixture(run) {
  const requests = [];
  const server = createServer((request, response) => {
    requests.push({ path: request.url, authorization: request.headers.authorization });
    if (request.headers.authorization !== `Bearer ${fixtureToken}`) {
      response.writeHead(401, { "content-type": "application/json" });
      response.end(JSON.stringify({ detail: fixtureSensitiveBody }));
      return;
    }
    const payloads = {
      "/api/health": { status: "healthy" },
      "/api/runtime/instance": { id: "fixture-core", apiVersion: "v1" },
      "/api/runtime/capabilities": { reads: ["health", "services"] },
      "/api/services": { services: [{ id: "fixture-service", state: "ready" }] },
    };
    const payload = payloads[request.url];
    if (!payload) {
      response.writeHead(404, { "content-type": "application/json" });
      response.end(JSON.stringify({ detail: fixtureSensitiveBody }));
      return;
    }
    response.writeHead(200, { "content-type": "application/json" });
    response.end(JSON.stringify(payload));
  });
  await new Promise((resolve) => server.listen(0, "127.0.0.1", resolve));
  const address = server.address();
  assert.ok(address && typeof address === "object");
  try {
    return await run({ url: `http://127.0.0.1:${address.port}`, requests });
  } finally {
    await new Promise((resolve, reject) => server.close((error) => error ? reject(error) : resolve()));
  }
}

async function invoke(url, args, token = fixtureToken) {
  return runFile(process.execPath, [cli, "--core-url", url, ...args], {
    env: { ...process.env, SERVICE_LASSO_CORE_TOKEN: token },
    encoding: "utf8",
  });
}

test("compiled CLI performs authenticated read-only operator inspection and service discovery against a Core-shaped fixture", async () => {
  await withAuthenticatedCoreFixture(async ({ url, requests }) => {
    const inspect = await invoke(url, ["instance", "inspect", "--json"]);
    const services = await invoke(url, ["service", "list", "--json"]);

    assert.deepEqual(JSON.parse(inspect.stdout), {
      health: { status: "healthy" },
      instance: { id: "fixture-core", apiVersion: "v1" },
      capabilities: { reads: ["health", "services"] },
    });
    assert.deepEqual(JSON.parse(services.stdout), { services: [{ id: "fixture-service", state: "ready" }] });
    assert.equal(inspect.stderr, "");
    assert.equal(services.stderr, "");
    assert.deepEqual(requests.map((request) => request.path).sort(), [
      "/api/health",
      "/api/runtime/capabilities",
      "/api/runtime/instance",
      "/api/services",
    ]);
    assert.equal(requests.every((request) => request.authorization === `Bearer ${fixtureToken}`), true);
    assert.equal(`${inspect.stdout}${inspect.stderr}${services.stdout}${services.stderr}`.includes(fixtureToken), false);
  });
});

test("compiled CLI reports a rejected fixture credential without exposing the token or Core response body", async () => {
  await withAuthenticatedCoreFixture(async ({ url }) => {
    await assert.rejects(
      () => invoke(url, ["instance", "status", "--json"], "wrong-fixture-token"),
      (error) => error.code === 1
        && error.stderr.includes("Error [core_api_error]: Core returned HTTP 401.")
        && !error.stderr.includes("wrong-fixture-token")
        && !error.stderr.includes(fixtureSensitiveBody)
        && !error.stdout.includes("wrong-fixture-token")
        && !error.stdout.includes(fixtureSensitiveBody),
    );
  });
});
