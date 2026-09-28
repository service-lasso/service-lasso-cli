import assert from "node:assert/strict";
import test from "node:test";
import { CoreClient } from "../dist/core-client.js";

test("uses public read routes and URL-encodes lifecycle service ids", async () => {
  const calls = [];
  const fetch = async (url, init) => {
    calls.push({ url: String(url), init });
    return new Response(JSON.stringify({ ok: true }), { status: 200 });
  };
  const client = new CoreClient({ baseUrl: "http://127.0.0.1:17883", fetch });
  await client.health();
  await client.services();
  await client.lifecycle("service/a", "restart");
  assert.equal(calls[0].url, "http://127.0.0.1:17883/api/health");
  assert.equal(calls[1].url, "http://127.0.0.1:17883/api/services");
  assert.equal(calls[2].url, "http://127.0.0.1:17883/api/services/service%2Fa/restart");
  assert.equal(calls[2].init.method, "POST");
  assert.equal(calls[2].init.body, '{"confirm":true}');
});

test("turns failed Core responses into safe errors without echoing the body", async () => {
  const client = new CoreClient({ baseUrl: "http://127.0.0.1:17883", fetch: async () => new Response("bad request", { status: 400 }) });
  await assert.rejects(
    () => client.services(),
    (error) => error.code === "core_api_error" && error.message === "Core returned HTTP 400.",
  );
});
