import { execFileSync } from "node:child_process";
import { appendFile, mkdtemp, mkdir, rename, rm, readFile, readdir, symlink, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import assert from "node:assert/strict";
import test from "node:test";
import { materializeAcceptedTemplate } from "../dist/scaffold.js";

const node = process.execPath;
const bundle = Object.freeze({ repository: "service-lasso/service-template", tag: "template-v1.2.3-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", commit: "a".repeat(40), templateVersion: "1.2.3", contractDigest: "b".repeat(64), contractSha256: "c".repeat(64), archiveSha256: "d".repeat(64), catalogIdentity: "service-template/stable/1.2.3", inventory: [], files: [{ path: "service.json", bytes: Buffer.from('{"id":"safe"}\n'), mode: 0o644 }, { path: "config/example.env", bytes: Buffer.from("PORT=8080\n"), mode: 0o644 }] });
async function awaitGate(gate, stage) { for (let i = 0; i < 400; i++) { try { if ((await readFile(`${gate}.ready`, "utf8")) === stage) return; } catch {} await new Promise((resolve) => setTimeout(resolve, 10)); } throw new Error(`timed out waiting for ${stage}`); }
async function releaseGate(gate, stage) { await appendFile(`${gate}.continue`, `${stage}\n`); }

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
    assert.equal(provenance.confinedWriter.platform, process.platform);
    assert.equal(provenance.confinedWriter.sha256.length, 64);
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

test("packaged confined helper keeps a held parent through a replacement and retains concurrent unowned failure content", async (t) => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-confined-"));
  const output = join(root, "native");
  const sourceSha = "0123456789abcdef0123456789abcdef01234567";
  const version = "0.1.0-dev.0123456";
  const priorHelper = process.env.SERVICE_LASSO_CONFINED_HELPER, priorGate = process.env.SERVICE_LASSO_CONFINED_TEST_GATE, priorStage = process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE, priorFail = process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE;
  t.after(async () => { if (priorHelper === undefined) delete process.env.SERVICE_LASSO_CONFINED_HELPER; else process.env.SERVICE_LASSO_CONFINED_HELPER = priorHelper; if (priorGate === undefined) delete process.env.SERVICE_LASSO_CONFINED_TEST_GATE; else process.env.SERVICE_LASSO_CONFINED_TEST_GATE = priorGate; if (priorStage === undefined) delete process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE; else process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE = priorStage; if (priorFail === undefined) delete process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE; else process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE = priorFail; await rm(root, { recursive: true, force: true }); });
  execFileSync(node, ["scripts/package-native.mjs", "--output", output, "--source-sha", sourceSha, "--version", version], { encoding: "utf8" });
  const provenance = JSON.parse(await readFile(join(output, "provenance.json"), "utf8"));
  process.env.SERVICE_LASSO_CONFINED_HELPER = join(output, provenance.confinedWriter.name);
  const parent = join(root, "parent"), oldParent = join(root, "parent-held"), outside = join(root, "outside"), destination = join(parent, "project"), gate = join(root, "swap-gate");
  await mkdir(parent); await mkdir(outside); process.env.SERVICE_LASSO_CONFINED_TEST_GATE = gate; process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE = "before-project-create";
  const creation = materializeAcceptedTemplate(destination, bundle);
  await awaitGate(gate, "before-project-create");
  let replacementBlocked = false;
  try { await rename(parent, oldParent); await symlink(outside, parent, process.platform === "win32" ? "junction" : "dir"); } catch { replacementBlocked = true; }
  await releaseGate(gate, "before-project-create"); await creation;
  await assert.rejects(readFile(join(outside, "project", "service.json")));
  assert.equal(await readFile(join(replacementBlocked ? parent : oldParent, "project", "service.json"), "utf8"), '{"id":"safe"}\n');
  if (process.platform !== "win32") {
    const leafParent = join(root, "leaf-parent"), leafDestination = join(leafParent, "project"), leafGate = join(root, "leaf-gate");
    await mkdir(leafParent); process.env.SERVICE_LASSO_CONFINED_TEST_GATE = leafGate; process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE = "before-project-commit"; delete process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE;
    const leafCreation = materializeAcceptedTemplate(leafDestination, bundle); leafCreation.catch(() => {});
    await awaitGate(leafGate, "before-project-commit"); await mkdir(leafDestination); await writeFile(join(leafDestination, "unowned.txt"), "preserve"); await releaseGate(leafGate, "before-project-commit");
    await assert.rejects(leafCreation, { code: "confined_writer_destination_exists" });
    assert.equal(await readFile(join(leafDestination, "unowned.txt"), "utf8"), "preserve"); await assert.rejects(readFile(join(leafDestination, "service.json")));
  }
  const failureParent = join(root, "failure-parent"), failureDestination = join(failureParent, "project"), failureGate = join(root, "failure-gate");
  await mkdir(failureParent); process.env.SERVICE_LASSO_CONFINED_TEST_GATE = failureGate; process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE = "after-file-write"; process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE = "after-file-write";
  const failure = materializeAcceptedTemplate(failureDestination, bundle); failure.catch(() => {});
  await awaitGate(failureGate, "after-file-write"); if (process.platform !== "win32") await mkdir(failureDestination); await writeFile(join(failureDestination, "unowned.txt"), "preserve"); await releaseGate(failureGate, "after-file-write");
  await assert.rejects(failure, { code: "confined_writer_write_rejected" });
  assert.equal(await readFile(join(failureDestination, "unowned.txt"), "utf8"), "preserve");
  await assert.rejects(readFile(join(failureDestination, "service.json")));
  if (process.platform !== "win32") assert.deepEqual(await readdir(failureDestination), ["unowned.txt"]);
});
