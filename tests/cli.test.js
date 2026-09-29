import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { readFile } from "node:fs/promises";
import test from "node:test";
import { fileURLToPath } from "node:url";

const cli = [process.execPath, "dist/index.js"];

test("help is keyboard-oriented and discoverable", () => {
  const root = spawnSync(cli[0], [...cli.slice(1), "--help"], { encoding: "utf8" });
  const service = spawnSync(cli[0], [...cli.slice(1), "service", "--help"], { encoding: "utf8" });
  assert.equal(root.status, 0);
  assert.equal(service.status, 0);
  assert.match(root.stdout, /instance/);
  assert.match(root.stdout, /service-lassoctl/);
  assert.match(service.stdout, /init/);
});

test("package exposes a command distinct from Core's service-lasso executable", async () => {
  const manifest = JSON.parse(await readFile(new URL("../package.json", import.meta.url), "utf8"));
  assert.deepEqual(manifest.bin, { "service-lassoctl": "./dist/index.js" });
});

test("installed package bin executes through the Unix-compatible entrypoint path", () => {
  const result = spawnSync(process.execPath, [fileURLToPath(new URL("../dist/index.js", import.meta.url)), "--version"], { encoding: "utf8" });
  assert.equal(result.status, 0);
  assert.equal(result.stdout.trim(), "0.1.0");
});

test("mutations require explicit confirmation before Core is contacted", () => {
  const result = spawnSync(cli[0], [...cli.slice(1), "service", "start", "example-service"], { encoding: "utf8" });
  assert.equal(result.status, 1);
  assert.match(result.stderr, /confirmation_required/);
});
