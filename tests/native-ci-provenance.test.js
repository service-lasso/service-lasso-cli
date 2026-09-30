import { spawnSync } from "node:child_process";
import { createHash } from "node:crypto";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
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

function verifierArguments(directory, { eventName, expectedBaseSha, expectedMergeContextSha, expectedSourceSha = sourceSha, expectedPlatform = process.platform, expectedArchitecture = process.arch }) {
  return ["--directory", directory, "--expected-source-sha", expectedSourceSha, "--expected-event", eventName, "--expected-base-sha", expectedBaseSha, "--expected-merge-context-sha", expectedMergeContextSha, "--expected-platform", expectedPlatform, "--expected-architecture", expectedArchitecture];
}

function verify(directory, options) {
  const result = runVerifier(verifierArguments(directory, options));
  assert.equal(result.status, 0, result.stderr);
}

function verifyFailure(arguments_) {
  const result = runVerifier(arguments_);
  assert.notEqual(result.status, 0, "verifier unexpectedly accepted invalid event/base input");
  return result.stderr;
}

test("native CI provenance verifier accepts exact push and pull-request event contexts", async () => {
  const directory = await createNativeAssets();
  try {
    await writeFile(join(directory, "ci-context.json"), `${JSON.stringify({ schemaVersion: 1, eventName: "push", sourceSha, mergeContextSha: sourceSha })}\n`);
    verify(directory, { eventName: "push", expectedBaseSha: "", expectedMergeContextSha: sourceSha });

    await writeFile(join(directory, "ci-context.json"), `${JSON.stringify({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: mergeSha })}\n`);
    verify(directory, { eventName: "pull_request", expectedBaseSha: baseSha, expectedMergeContextSha: mergeSha });
  } finally {
    await rm(directory, { recursive: true, force: true });
  }
});

test("native CI provenance verifier rejects malformed, missing, and contradictory event, base, and merge inputs", async () => {
  const directory = await createNativeAssets();
  try {
    const writeContext = (context) => writeFile(join(directory, "ci-context.json"), `${JSON.stringify(context)}\n`);

    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, mergeContextSha: mergeSha });
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "pull_request", expectedBaseSha: baseSha, expectedMergeContextSha: mergeSha })), /pull-request base/);

    await writeContext({ schemaVersion: 1, eventName: "push", sourceSha, testedBaseSha: baseSha, mergeContextSha: sourceSha });
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "push", expectedBaseSha: "", expectedMergeContextSha: sourceSha })), /push CI context must omit or null/);

    await writeContext({ schemaVersion: 1, eventName: "push", sourceSha, mergeContextSha: sourceSha });
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "push", expectedBaseSha: "not-a-sha", expectedMergeContextSha: sourceSha })), /push events must pass an explicitly empty/);
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "push", expectedBaseSha: "", expectedMergeContextSha: "not-a-sha" })), /merge context SHA must be a full Git revision/);
    assert.match(verifyFailure(["--directory", directory, "--expected-source-sha", sourceSha, "--expected-event", "push", "--expected-base-sha", "", "--expected-platform", process.platform, "--expected-architecture", process.arch]), /Missing required --expected-merge-context-sha argument/);

    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: "not-a-sha" });
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "pull_request", expectedBaseSha: baseSha, expectedMergeContextSha: mergeSha })), /CI merge context must be a full Git revision/);

    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: mergeSha });
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "pull_request", expectedBaseSha: baseSha, expectedMergeContextSha: "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" })), /triggering merge context/);
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "pull_request", expectedBaseSha: "not-a-sha", expectedMergeContextSha: mergeSha })), /pull-request target base must be a full Git revision/);
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "pull_request", expectedBaseSha: sourceSha, expectedMergeContextSha: mergeSha })), /CI context must record the pull-request base separately/);

    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: sourceSha, mergeContextSha: mergeSha });
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "pull_request", expectedBaseSha: sourceSha, expectedMergeContextSha: mergeSha })), /source and target base must differ/);

    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: sourceSha });
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "pull_request", expectedBaseSha: baseSha, expectedMergeContextSha: sourceSha })), /merge context and source must differ/);

    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: baseSha });
    assert.match(verifyFailure(verifierArguments(directory, { eventName: "pull_request", expectedBaseSha: baseSha, expectedMergeContextSha: baseSha })), /merge context and target base must differ/);
  } finally {
    await rm(directory, { recursive: true, force: true });
  }
});

test("native CI provenance verifier keeps source, event, digest, and host checks closed", async () => {
  const directory = await createNativeAssets();
  try {
    const writeContext = (context) => writeFile(join(directory, "ci-context.json"), `${JSON.stringify(context)}\n`);
    const expected = { eventName: "pull_request", expectedBaseSha: baseSha, expectedMergeContextSha: mergeSha };
    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: mergeSha });

    assert.match(verifyFailure(verifierArguments(directory, { ...expected, expectedSourceSha: "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" })), /native provenance must name the checked-out source revision/);
    assert.match(verifyFailure(verifierArguments(directory, { ...expected, expectedSourceSha: "not-a-sha" })), /source SHA must be a full Git revision/);
    assert.match(verifyFailure(verifierArguments(directory, { ...expected, eventName: "workflow_dispatch" })), /event must be pull_request or push/);

    await writeContext({ schemaVersion: 1, eventName: "pull_request", testedBaseSha: baseSha, mergeContextSha: mergeSha });
    assert.match(verifyFailure(verifierArguments(directory, expected)), /CI context source SHA must be a full Git revision/);
    await writeContext({ schemaVersion: 1, eventName: "push", sourceSha, testedBaseSha: baseSha, mergeContextSha: mergeSha });
    assert.match(verifyFailure(verifierArguments(directory, expected)), /CI context must record the triggering event/);
    await writeContext({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: mergeSha });

    assert.match(verifyFailure(verifierArguments(directory, { ...expected, expectedPlatform: "other-platform" })), /Expected values to be strictly equal/);
    assert.match(verifyFailure(verifierArguments(directory, { ...expected, expectedArchitecture: "other-architecture" })), /Expected values to be strictly equal/);

    const provenancePath = join(directory, "provenance.json");
    const provenance = JSON.parse(await readFile(provenancePath, "utf8"));
    provenance.executable.sha256 = "0".repeat(64);
    await writeFile(provenancePath, `${JSON.stringify(provenance)}\n`);
    assert.match(verifyFailure(verifierArguments(directory, expected)), /provenance digest must match/);
  } finally {
    await rm(directory, { recursive: true, force: true });
  }
});
