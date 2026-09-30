import { execFileSync } from "node:child_process";
import { mkdtemp, rm, readFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import assert from "node:assert/strict";
import test from "node:test";

const node = process.execPath;

test("native SEA packager records a direct host executable and smoke runs without Node on PATH", async () => {
  const output = await mkdtemp(join(tmpdir(), "service-lassoctl-native-"));
  const sourceSha = "0123456789abcdef0123456789abcdef01234567";
  try {
    execFileSync(node, ["scripts/package-native.mjs", "--output", output, "--source-sha", sourceSha], { encoding: "utf8" });
    const provenance = JSON.parse(await readFile(join(output, "provenance.json"), "utf8"));
    assert.equal(provenance.source.commit, sourceSha);
    assert.equal(provenance.executable.platform, process.platform);
    assert.equal(provenance.executable.architecture, process.arch);
    execFileSync(node, ["scripts/smoke-native.mjs", "--directory", output, "--expected-source-sha", sourceSha], { encoding: "utf8" });
  } finally {
    await rm(output, { recursive: true, force: true });
  }
});
