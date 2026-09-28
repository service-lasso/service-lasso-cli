import assert from "node:assert/strict";
import { mkdtemp, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { configPath, loadConfig, normalizeCoreUrl, resolveCoreUrl, saveConfig } from "../dist/config.js";

test("normalizes a safe Core origin", () => {
  assert.equal(normalizeCoreUrl("https://core.example/"), "https://core.example");
  assert.throws(() => normalizeCoreUrl("https://user:pass@core.example"), { code: "invalid_core_url" });
  assert.throws(() => normalizeCoreUrl("ftp://core.example"), { code: "invalid_core_url" });
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
