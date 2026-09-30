import { spawnSync } from "node:child_process";
import { createHash } from "node:crypto";
import { mkdtemp, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import assert from "node:assert/strict";
import test from "node:test";

const node = process.execPath;
const sourceSha = "0123456789abcdef0123456789abcdef01234567";
const baseSha = "fedcba9876543210fedcba9876543210fedcba98";
const mergeSha = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

async function createNativeAssets() {
  const directory = await mkdtemp(join(tmpdir(), "service-lassoctl-native-ci-provenance-"));
  const executableName = "service-lassoctl";
  const executable = Buffer.from("native executable fixture");
  await writeFile(join(directory, executableName), executable);
  await writeFile(join(directory, "provenance.json"), `${JSON.stringify({ source: { commit: sourceSha }, executable: { name: executableName, sha256: createHash("sha256").update(executable).digest("hex"), platform: process.platform, architecture: process.arch } })}\n`);
  return directory;
}

function runVerifier(arguments_) {
  return spawnSync(node, ["scripts/verify-native-ci-provenance.mjs", ...arguments_], { encoding: "utf8" });
}

function verifierArguments(directory, eventName, expectedBaseSha) {
  return ["--directory", directory, "--expected-source-sha", sourceSha, "--expected-event", eventName, "--expected-base-sha", expectedBaseSha, "--expected-platform", process.platform, "--expected-architecture", process.arch];
}

function verify(directory, eventName, expectedBaseSha) {
  const result = runVerifier(verifierArguments(directory, eventName, expectedBaseSha));
  assert.equal(result.status, 0, result.stderr);
}

function verifyFailure(arguments_) {
  const result = runVerifier(arguments_);
  assert.notEqual(result.status, 0, "verifier unexpectedly accepted invalid event/base input");
  return result.stderr;
}

test("native CI provenance verifier accepts the explicit push empty base and full pull-request base", async () => {
  const directory = await createNativeAssets();
  try {
    await writeFile(join(directory, "ci-context.json"), `${JSON.stringify({ schemaVersion: 1, eventName: "push", sourceSha, mergeContextSha: sourceSha })}\n`);
    verify(directory, "push", "");

    await writeFile(join(directory, "ci-context.json"), `${JSON.stringify({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: mergeSha })}\n`);
    verify(directory, "pull_request", baseSha);
  } finally {
    await rm(directory, { recursive: true, force: true });
  }
});

test("native CI provenance verifier rejects malformed, missing, and contradictory event/base inputs", async () => {
  const directory = await createNativeAssets();
  try {
    const writeContext = (context) => writeFile(join(directory, "ci-context.json"), `${JSON.stringify(context)}\n`);

    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, mergeContextSha: mergeSha });
    assert.match(verifyFailure(verifierArguments(directory, "pull_request", baseSha)), /pull-request base/);

    await writeContext({ schemaVersion: 1, eventName: "push", sourceSha, testedBaseSha: baseSha, mergeContextSha: sourceSha });
    assert.match(verifyFailure(verifierArguments(directory, "push", "")), /push CI context must omit or null/);

    await writeContext({ schemaVersion: 1, eventName: "push", sourceSha, mergeContextSha: sourceSha });
    assert.match(verifyFailure(verifierArguments(directory, "push", "not-a-sha")), /push events must pass an explicitly empty/);
    assert.match(verifyFailure(["--directory", directory, "--expected-source-sha", sourceSha, "--expected-event", "push", "--expected-platform", process.platform, "--expected-architecture", process.arch]), /Missing required --expected-base-sha argument/);
  } finally {
    await rm(directory, { recursive: true, force: true });
  }
});
