import { execFileSync } from "node:child_process";
import { mkdtemp, mkdir, rm, readFile, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import assert from "node:assert/strict";
import test from "node:test";

const node = process.execPath;

test("native SEA packager records a direct host executable and smoke runs without Node on PATH", async () => {
  const output = await mkdtemp(join(tmpdir(), "service-lassoctl-native-"));
  const sourceSha = "0123456789abcdef0123456789abcdef01234567";
  try {
    const version = "0.1.0-dev.0123456";
    execFileSync(node, ["scripts/package-native.mjs", "--output", output, "--source-sha", sourceSha, "--version", version], { encoding: "utf8" });
    const provenance = JSON.parse(await readFile(join(output, "provenance.json"), "utf8"));
    assert.equal(provenance.source.commit, sourceSha);
    assert.equal(provenance.executable.platform, process.platform);
    assert.equal(provenance.executable.architecture, process.arch);
    assert.equal(provenance.candidate.version, version);
    execFileSync(node, ["scripts/write-native-ci-context.mjs", "--directory", output, "--event-name", "workflow_dispatch", "--source-sha", sourceSha, "--tested-base-sha", "", "--merge-context-sha", sourceSha], { encoding: "utf8" });
    execFileSync(node, ["scripts/smoke-native.mjs", "--directory", output, "--expected-source-sha", sourceSha, "--expected-version", version, "--write-host-acceptance"], { encoding: "utf8" });
    const acceptance = JSON.parse(await readFile(join(output, "host-acceptance.json"), "utf8"));
    assert.equal(acceptance.version, version);
    assert.equal(acceptance.sourceSha, sourceSha);
    assert.equal(acceptance.nodeAbsentFromPath, true);
    assert.throws(() => execFileSync(node, ["scripts/smoke-native.mjs", "--directory", output, "--expected-source-sha", sourceSha, "--expected-version", "0.1.0-dev.fffffff"], { stdio: "ignore" }));
    const archiveOutput = join(output, "archive");
    await mkdir(archiveOutput);
    execFileSync(node, ["scripts/archive-native-candidate.mjs", "--directory", output, "--output", archiveOutput, "--source-sha", sourceSha, "--version", version], { encoding: "utf8" });
    const executable = join(output, provenance.executable.name);
    const acceptedBytes = await readFile(executable);
    await writeFile(executable, Buffer.concat([acceptedBytes, Buffer.from("changed")]))
    assert.throws(() => execFileSync(node, ["scripts/archive-native-candidate.mjs", "--directory", output, "--output", archiveOutput, "--source-sha", sourceSha, "--version", version], { stdio: "ignore" }));
    await writeFile(executable, acceptedBytes);
    await rm(join(output, "ci-context.json"));
    assert.throws(() => execFileSync(node, ["scripts/archive-native-candidate.mjs", "--directory", output, "--output", archiveOutput, "--source-sha", sourceSha, "--version", version], { stdio: "ignore" }));
    const baseSha = "fedcba9876543210fedcba9876543210fedcba98";
    await writeFile(join(output, "ci-context.json"), `${JSON.stringify({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" })}\n`);
    execFileSync(node, ["scripts/verify-native-ci-provenance.mjs", "--directory", output, "--expected-source-sha", sourceSha, "--expected-event", "pull_request", "--expected-base-sha", baseSha, "--expected-merge-context-sha", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "--expected-platform", process.platform, "--expected-architecture", process.arch], { encoding: "utf8" });
  } finally {
    await rm(output, { recursive: true, force: true });
  }
});
