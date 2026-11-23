import assert from "node:assert/strict";
import test from "node:test";
import { CoreClient } from "../dist/core-client.js";

test("uses public read routes, carries an environment token and URL-encodes lifecycle service ids", async () => {
  const calls = [];
  const fetch = async (url, init) => {
    calls.push({ url: String(url), init });
    return new Response(JSON.stringify({ ok: true }), { status: 200 });
  };
  const client = new CoreClient({ baseUrl: "http://127.0.0.1:17883", token: "ci-token", fetch });
  await client.health();
  await client.services();
  await client.lifecycle("service/a", "restart");
  assert.equal(calls[0].url, "http://127.0.0.1:17883/api/health");
  assert.equal(calls[1].url, "http://127.0.0.1:17883/api/services");
  assert.equal(calls[2].url, "http://127.0.0.1:17883/api/services/service%2Fa/restart");
  assert.equal(calls[2].init.method, "POST");
  assert.equal(calls[2].init.body, '{"confirm":true}');
  assert.equal(calls[2].init.headers.authorization, "Bearer ci-token");
  assert.equal(calls.every((call) => call.init.redirect === "error"), true);
});

test("rejects redirects before a lifecycle mutation can be replayed at another origin", async () => {
  const token = "redirect-test-token";
  const calls = [];
  const client = new CoreClient({
    baseUrl: "https://core.example",
    token,
    fetch: async (url, init) => {
      calls.push({ url: String(url), init });
      throw new Error(`redirect to https://other.example rejected with ${init.headers.authorization}`);
    },
  });

  await assert.rejects(
    () => client.lifecycle("service-a", "restart"),
    (error) => error.code === "core_unreachable"
      && error.message === "Could not reach Service Lasso Core."
      && !error.message.includes(token),
  );
  assert.equal(calls.length, 1);
  assert.equal(calls[0].url, "https://core.example/api/services/service-a/restart");
  assert.equal(calls[0].init.redirect, "error");
  assert.equal(calls[0].init.headers.authorization, `Bearer ${token}`);
});

test("fails before making a request when a token would cross a cleartext remote connection", () => {
  let called = false;
  assert.throws(
    () => new CoreClient({
      baseUrl: "http://core.example",
      token: "secret-token-value",
      fetch: async () => {
        called = true;
        return new Response();
      },
    }),
    (error) => error.code === "insecure_core_token_transport" && !error.message.includes("secret-token-value"),
  );
  assert.equal(called, false);
});

test("inspects Core health, instance identity and capabilities without mutation", async () => {
  const calls = [];
  const client = new CoreClient({
    baseUrl: "https://core.example",
    fetch: async (url, init) => {
      calls.push({ url: String(url), init });
      return new Response(JSON.stringify({ ok: true }), { status: 200 });
    },
  });
  await client.inspect();
  assert.deepEqual(calls.map((call) => call.url).sort(), [
    "https://core.example/api/health",
    "https://core.example/api/runtime/capabilities",
    "https://core.example/api/runtime/instance",
  ]);
  assert.equal(calls.every((call) => call.init.method === undefined), true);
});

test("turns failed Core responses into safe errors without echoing the body", async () => {
  const client = new CoreClient({ baseUrl: "http://127.0.0.1:17883", fetch: async () => new Response("bad request", { status: 400 }) });
  await assert.rejects(
    () => client.services(),
    (error) => error.code === "core_api_error" && error.message === "Core returned HTTP 400.",
  );
});

test("classifies response body read failure as secret-safe transport failure without retry", async () => {
  const sentinel = "response-body-secret-sentinel";
  let calls = 0;
  const client = new CoreClient({ baseUrl: "http://127.0.0.1:17883", fetch: async () => {
    calls += 1;
    return new Response(new ReadableStream({ start(controller) { controller.error(new Error(sentinel)); } }));
  } });
  await assert.rejects(() => client.health(), (error) => {
    assert.equal(error.code, "core_unreachable");
    assert.equal(error.message, "Could not reach Service Lasso Core.");
    assert.equal(error.hint, "Set SERVICE_LASSO_CORE_URL or run `service-lassoctl config set core-url <url>`.");
    assert.equal(String(error).includes(sentinel), false);
    return true;
  });
  assert.equal(calls, 1);
});

test("keeps valid JSON and invalid JSON response classification after body read", async () => {
  const valid = new CoreClient({ baseUrl: "http://127.0.0.1:17883", fetch: async () => new Response('{"status":"ok"}') });
  assert.deepEqual(await valid.health(), { status: "ok" });
  const invalid = new CoreClient({ baseUrl: "http://127.0.0.1:17883", fetch: async () => new Response("not-json") });
  await assert.rejects(() => invalid.health(), (error) => error.code === "invalid_core_response" && error.message === "Core returned a non-JSON response.");
});

test("consumes the operator/v2 envelope for discovery, services, preview, execute and readback", async () => {
  const calls = [];
  const envelope = data => JSON.stringify({
    contractVersion: "service-lasso-operator.v2",
    requestId: "request-v2",
    generatedAt: "2026-11-20T12:00:00.000Z",
    data,
  });
  const client = new CoreClient({
    baseUrl: "http://127.0.0.1:17883",
    token: "ci-token",
    fetch: async (url, init) => {
      calls.push({ url: String(url), init });
      return new Response(envelope({ ok: true }), { status: 200 });
    },
  });
  assert.deepEqual(await client.operatorV2Discovery(), { ok: true });
  assert.deepEqual(await client.operatorV2Services(), { ok: true });
  await client.operatorV2Detail("root");
  await client.operatorV2LifecycleAvailability("root");
  await client.operatorV2Preview("root", { actionId: "service_start" });
  await client.operatorV2Execute("root", { planId: "plan-1" });
  await client.operatorV2Operation("mcp-operation-12345678-1234-1234-1234-123456789abc");
  await client.operatorV2Cancel("mcp-operation-12345678-1234-1234-1234-123456789abc");
  assert.deepEqual(calls.map(call => call.url), [
    "http://127.0.0.1:17883/api/operator/v2/discovery",
    "http://127.0.0.1:17883/api/operator/v2/services",
    "http://127.0.0.1:17883/api/operator/v2/services/root",
    "http://127.0.0.1:17883/api/operator/v2/services/root/lifecycle-availability",
    "http://127.0.0.1:17883/api/operator/v2/services/root/preview",
    "http://127.0.0.1:17883/api/operator/v2/services/root/execute",
    "http://127.0.0.1:17883/api/operator/v2/operations/mcp-operation-12345678-1234-1234-1234-123456789abc",
    "http://127.0.0.1:17883/api/operator/v2/operations/mcp-operation-12345678-1234-1234-1234-123456789abc/cancel",
  ]);
  assert.equal(calls[4].init.method, "POST");
  assert.equal(calls[4].init.body, '{"actionId":"service_start"}');
  assert.equal(calls[5].init.method, "POST");
  assert.equal(calls[5].init.body, '{"planId":"plan-1"}');
  assert.equal(calls[7].init.method, "POST");
  assert.equal(calls[7].init.body, "{}");
  assert.ok(calls.every(call => call.init.redirect === "error"));
});

test("rejects a malformed operator/v2 response", async () => {
  const client = new CoreClient({
    baseUrl: "http://127.0.0.1:17883",
    fetch: async () => new Response(JSON.stringify({ ok: true }), { status: 200 }),
  });
  await assert.rejects(
    () => client.operatorV2Discovery(),
    error => error.code === "invalid_core_response",
  );
});
