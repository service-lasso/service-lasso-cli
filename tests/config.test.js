import assert from "node:assert/strict";
import { mkdtemp, readFile, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { assertCoreTokenTransport, configPath, loadConfig, normalizeCoreUrl, resolveConnectionName, resolveCoreToken, resolveCoreUrl, saveConfig, updateCoreUrl } from "../dist/config.js";
import { asCliError } from "../dist/errors.js";

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
  await assert.rejects(() => loadConfig(path), (error) => error.code === "invalid_config" && error.message === "CLI configuration is invalid." && !error.message.includes(path));
});

test("configuration and unexpected diagnostics never expose path or environment sentinels", async () => {
  const home = await mkdtemp(join(tmpdir(), "lasso-cli-safe-config-"));
  const path = join(home, "private-token-sentinel.json");
  await writeFile(path, JSON.stringify({ coreUrl: 7 }), "utf8");
  await assert.rejects(() => loadConfig(path), (error) => error.code === "invalid_config" && error.message === "CLI configuration is invalid." && !error.message.includes("private-token-sentinel"));
  const unexpected = asCliError(new Error("https://private.example/token=environment-secret-sentinel"));
  assert.equal(unexpected.code, "unexpected_error");
  assert.equal(unexpected.message, "The command could not be completed.");
  assert.equal(unexpected.message.includes("environment-secret-sentinel"), false);
});

test("selects saved named connections after flags and environment without persisting credentials", async () => {
  const home = await mkdtemp(join(tmpdir(), "lasso-cli-profiles-"));
  const path = configPath(home);
  await saveConfig({ connections: { local: { coreUrl: "http://127.0.0.1:17883" }, remote: { coreUrl: "https://core.example" } }, defaultConnection: "local" }, path);
  const saved = await loadConfig(path);
  assert.equal(resolveConnectionName({ environment: { SERVICE_LASSO_CONNECTION: "remote" }, config: saved }), "remote");
  assert.equal(resolveCoreUrl({ connection: "local", environment: { SERVICE_LASSO_CORE_URL: "https://override.example" }, config: saved }), "https://override.example");
  assert.equal(resolveCoreUrl({ environment: { SERVICE_LASSO_CONNECTION: "remote" }, config: saved }), "https://core.example");
  assert.equal(resolveCoreUrl({ cliValue: "https://flag.example", connection: "missing", config: saved }), "https://flag.example");
  assert.equal(resolveCoreUrl({ connection: "missing", environment: { SERVICE_LASSO_CORE_URL: "https://environment.example" }, config: saved }), "https://environment.example");
  await writeFile(path, JSON.stringify({ connections: { remote: { coreUrl: "https://core.example", token: "secret" } } }), "utf8");
  await assert.rejects(() => loadConfig(path), { code: "invalid_config" });
});


test("requires an own saved connection before selecting a Core origin", () => {
  const config = { connections: {}, coreUrl: "https://fallback.example" };
  for (const selection of [
    { connection: "constructor", config },
    { environment: { SERVICE_LASSO_CONNECTION: "constructor" }, config },
    { config: { ...config, defaultConnection: "constructor" } },
  ]) {
    assert.throws(() => resolveCoreUrl(selection), { code: "unknown_connection" });
  }
  assert.equal(resolveCoreUrl({ connection: "constructor", cliValue: "https://flag.example", config }), "https://flag.example");
  assert.equal(resolveCoreUrl({ connection: "constructor", environment: { SERVICE_LASSO_CORE_URL: "https://env.example" }, config }), "https://env.example");
});

test("rejects inherited defaults on load/save and preserves explicitly saved constructor", async () => {
  const home = await mkdtemp(join(tmpdir(), "lasso-cli-membership-"));
  const path = configPath(home);
  await saveConfig({}, path);
  for (const config of [
    { defaultConnection: "constructor" },
    { defaultConnection: "constructor", connections: {} },
  ]) {
    await writeFile(path, JSON.stringify(config), "utf8");
    await assert.rejects(() => loadConfig(path), { code: "invalid_config" });
    await assert.rejects(() => saveConfig(config, path), { code: "invalid_config" });
  }
  const config = { connections: { constructor: { coreUrl: "https://saved.example" } }, defaultConnection: "constructor" };
  await saveConfig(config, path);
  const saved = await loadConfig(path);
  assert.deepEqual(saved, config);
  assert.equal(resolveCoreUrl({ config: saved }), "https://saved.example");
  assert.equal(resolveCoreUrl({ connection: "constructor", config: saved }), "https://saved.example");
});


test("updating core-url preserves named profiles, their default, and URL precedence", async () => {
  const path = join(await mkdtemp(join(tmpdir(), "lasso-config-update-")), "config.json");
  const profiles = { local: { coreUrl: "http://127.0.0.1:17883" }, remote: { coreUrl: "https://core.example" } };
  await saveConfig({ coreUrl: "https://old.example", connections: profiles, defaultConnection: "remote" }, path);
  await updateCoreUrl("https://new.example/", path);
  const config = await loadConfig(path);
  assert.deepEqual(config, { coreUrl: "https://new.example", connections: profiles, defaultConnection: "remote" });
  assert.equal(resolveCoreUrl({ config }), "https://core.example");
  assert.equal(resolveCoreUrl({ config, environment: { SERVICE_LASSO_CORE_URL: "https://env.example" } }), "https://env.example");
  assert.equal(resolveCoreUrl({ config, cliValue: "https://flag.example" }), "https://flag.example");
});

test("updating core-url creates missing configuration and leaves existing bytes intact on invalid input", async () => {
  const path = join(await mkdtemp(join(tmpdir(), "lasso-config-update-")), "nested", "config.json");
  await updateCoreUrl("https://new.example/", path);
  assert.deepEqual(await loadConfig(path), { coreUrl: "https://new.example" });
  const before = await readFile(path);
  await assert.rejects(() => updateCoreUrl("https://user:secret@example.com", path), (error) => error.code === "invalid_core_url");
  assert.deepEqual(await readFile(path), before);
});
