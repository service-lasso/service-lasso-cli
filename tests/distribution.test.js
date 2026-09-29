import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { cp, mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";

const node = process.execPath;

test("candidate packager creates a checksum-bound Node 22 archive and clean-consumer smoke passes", async () => {
  const output = await mkdtemp(join(tmpdir(), "service-lassoctl-candidate-"));
  const sourceSha = "0123456789abcdef0123456789abcdef01234567";
  const version = "0.1.0-dev.0123456";
  try {
    execFileSync(node, ["scripts/package-candidate.mjs", "--output", output, "--version", version, "--source-sha", sourceSha], { encoding: "utf8" });
    const record = JSON.parse(await readFile(join(output, "candidate.json"), "utf8"));
    assert.equal(record.source.commit, sourceSha);
    assert.equal(record.package.command, "service-lassoctl");
    assert.equal(record.assets[0].name, `service-lassoctl-${version}.tgz`);
    execFileSync(node, ["scripts/smoke-candidate.mjs", "--directory", output, "--expected-version", version, "--expected-source-sha", sourceSha], { encoding: "utf8" });
  } finally {
    await rm(output, { recursive: true, force: true });
  }
});

test("candidate workflow is manual develop-only and publishes a prerelease only after smoke", async () => {
  const workflow = await readFile(new URL("../.github/workflows/release.yml", import.meta.url), "utf8");
  assert.match(workflow, /workflow_dispatch:/);
  assert.match(workflow, /github\.ref == 'refs\/heads\/develop'/);
  assert.match(workflow, /publish-candidate:/);
  assert.match(workflow, /needs: \[build, smoke\]/);
  assert.match(workflow, /gh release create/);
  assert.match(workflow, /--prerelease/);
  assert.match(workflow, /GH_TOKEN: \$\{\{ github\.token \}\}/);
  assert.match(workflow, /persist-credentials: true/);
  assert.match(workflow, /refs\/tags\/\$\{tag\}\^\{\}/);
  assert.equal((workflow.match(/verify-candidate-release\.mjs/g) ?? []).length, 1);
  assert.equal((workflow.match(/readback_release_assets/g) ?? []).length, 3);
  assert.match(workflow, /readback_release_assets\(\)/);
  assert.match(workflow, /while \[ "\$attempt" -le 3 \]/);
  assert.match(workflow, /grep -Eq 'HTTP 5\[0-9\]\[0-9\]'/);
  assert.match(workflow, /rm -rf release-verify release-download\.err/);
  assert.doesNotMatch(workflow, /softprops\/action-gh-release/);
});

test("candidate workflow identity shell block parses when bash is available", () => {
  execFileSync(node, ["scripts/check-candidate-workflow-shell.mjs"], { encoding: "utf8" });
});

test("candidate readback shell fails closed and retries only transient failures", () => {
  execFileSync(node, ["scripts/test-candidate-readback-shell.mjs"], { encoding: "utf8" });
});

test("existing prerelease verification rejects bytes that differ from the tested candidate", async () => {
  const expected = await mkdtemp(join(tmpdir(), "service-lassoctl-expected-"));
  const actual = await mkdtemp(join(tmpdir(), "service-lassoctl-actual-"));
  const sourceSha = "0123456789abcdef0123456789abcdef01234567";
  const version = "0.1.0-dev.0123456";
  try {
    execFileSync(node, ["scripts/package-candidate.mjs", "--output", expected, "--version", version, "--source-sha", sourceSha], { encoding: "utf8" });
    await cp(expected, actual, { recursive: true });
    execFileSync(node, ["scripts/verify-candidate-release.mjs", "--expected", expected, "--actual", actual], { encoding: "utf8" });
    await writeFile(join(actual, "SHA256SUMS.txt"), "different\n");
    assert.throws(
      () => execFileSync(node, ["scripts/verify-candidate-release.mjs", "--expected", expected, "--actual", actual], { stdio: "ignore" }),
      (error) => error.status === 1,
    );
  } finally {
    await Promise.all([rm(expected, { recursive: true, force: true }), rm(actual, { recursive: true, force: true })]);
  }
});
