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
