import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdtemp, readFile, rm } from "node:fs/promises";
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

test("protected candidate workflow freezes develop, builds target bytes once, and gates write access", async () => {
  const workflow = await readFile(new URL("../.github/workflows/release.yml", import.meta.url), "utf8");
  assert.match(workflow, /workflow_dispatch:/);
  assert.match(workflow, /github\.ref == 'refs\/heads\/develop'/);
  assert.match(workflow, /native:\n[\s\S]*win32-x64[\s\S]*linux-x64[\s\S]*darwin-arm64/);
  assert.match(workflow, /npm run smoke:native/);
  assert.match(workflow, /assemble:protected-candidate/);
  assert.match(workflow, /environment:\n      name: development-candidate/);
  assert.match(workflow, /contents: write/);
  assert.match(workflow, /DEVELOPMENT_CANDIDATE_TOKEN/);
  assert.match(workflow, /publish:protected-candidate/);
  assert.doesNotMatch(workflow, /persist-credentials: true/);
  assert.doesNotMatch(workflow, /gh release create/);
});
