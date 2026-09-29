import assert from "node:assert/strict";
import { mkdtemp, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { assertCoreTokenTransport, configPath, loadConfig, normalizeCoreUrl, resolveConnectionName, resolveCoreToken, resolveCoreUrl, saveConfig } from "../dist/config.js";

test("normalizes a safe Core origin", () => {
  assert.equal(normalizeCoreUrl("https://core.example/"), "https://core.example");
  assert.throws(() => normalizeCoreUrl("https://user:pass@core.example"), { code: "invalid_core_url" });
  assert.throws(() => normalizeCoreUrl("ftp://core.example"), { code: "invalid_core_url" });
});

test("uses an environment-only Core token and rejects unsafe token values", () => {
  assert.equal(resolveCoreToken({ SERVICE_LASSO_CORE_TOKEN: "ci-token" }), "ci-token");
  assert.equal(resolveCoreToken({}), undefined);
  assert.throws(() => resolveCoreToken({ SERVICE_LASSO_CORE_TOKEN: "bad token" }), { code: "invalid_core_token" });
});

test("allows a Core token over HTTPS or loopback HTTP but rejects cleartext remote origins without exposing it", () => {
  assert.doesNotThrow(() => assertCoreTokenTransport("https://core.example", "ci-token"));
  assert.doesNotThrow(() => assertCoreTokenTransport("http://127.0.0.1:17883", "ci-token"));
  assert.doesNotThrow(() => assertCoreTokenTransport("http://localhost:17883", "ci-token"));
  assert.doesNotThrow(() => assertCoreTokenTransport("http://[::1]:17883", "ci-token"));
  assert.throws(
    () => assertCoreTokenTransport("http://core.example", "secret-token-value"),
    (error) => error.code === "insecure_core_token_transport" && !error.message.includes("secret-token-value"),
  );
  assert.throws(() => assertCoreTokenTransport("http://127.example", "ci-token"), { code: "insecure_core_token_transport" });
});

test("environment wins over saved config and config persists atomically", async () => {
  const home = await mkdtemp(join(tmpdir(), "lasso-cli-config-"));
  const path = configPath(home);
  await saveConfig({ coreUrl: "https://saved.example" }, path);
  assert.deepEqual(await loadConfig(path), { coreUrl: "https://saved.example" });
  assert.equal(resolveCoreUrl({ environment: { SERVICE_LASSO_CORE_URL: "https://env.example" }, config: await loadConfig(path) }), "https://env.example");
  await writeFile(path, "{bad", "utf8");
  await assert.rejects(() => loadConfig(path), { code: "invalid_config" });
});

test("selects saved named connections after flags and environment without persisting credentials", async () => {
  const home = await mkdtemp(join(tmpdir(), "lasso-cli-profiles-"));
  const path = configPath(home);
  await saveConfig({ connections: { local: { coreUrl: "http://127.0.0.1:17883" }, remote: { coreUrl: "https://core.example" } }, defaultConnection: "local" }, path);
  const saved = await loadConfig(path);
  assert.equal(resolveConnectionName({ environment: { SERVICE_LASSO_CONNECTION: "remote" }, config: saved }), "remote");
  assert.equal(resolveCoreUrl({ connection: "local", environment: { SERVICE_LASSO_CORE_URL: "https://override.example" }, config: saved }), "https://override.example");
  assert.equal(resolveCoreUrl({ environment: { SERVICE_LASSO_CONNECTION: "remote" }, config: saved }), "https://core.example");
  await writeFile(path, JSON.stringify({ connections: { remote: { coreUrl: "https://core.example", token: "secret" } } }), "utf8");
  await assert.rejects(() => loadConfig(path), { code: "invalid_config" });
});
