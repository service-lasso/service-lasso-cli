import { execFileSync, spawn, spawnSync } from "node:child_process";
import { createHash } from "node:crypto";
import { appendFile, lstat, mkdtemp, mkdir, rename, rm, readFile, readdir, symlink, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { gzipSync } from "node:zlib";
import assert from "node:assert/strict";
import test from "node:test";

const node = process.execPath;
const bundle = Object.freeze({ repository: "service-lasso/service-template", tag: "template-v1.2.3-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", commit: "a".repeat(40), templateVersion: "1.2.3", contractDigest: "b".repeat(64), contractSha256: "c".repeat(64), archiveSha256: "d".repeat(64), catalogIdentity: "service-template/stable/1.2.3", inventory: [], files: [{ path: "service.json", bytes: Buffer.from('{"id":"safe"}\n'), mode: 0o644 }, { path: "config/example.env", bytes: Buffer.from("PORT=8080\n"), mode: 0o644 }] });
async function awaitGate(gate, stage) { for (let i = 0; i < 400; i++) { try { if ((await readFile(`${gate}.ready`, "utf8")) === stage) return; } catch {} await new Promise((resolve) => setTimeout(resolve, 10)); } throw new Error(`timed out waiting for ${stage}`); }
async function releaseGate(gate, stage) { await appendFile(`${gate}.continue`, `${stage}\n`); }
function helperInput(destination) { return `${Buffer.from(destination).toString("base64")}\n${bundle.files.length}\n${bundle.files.map((file) => `${file.path}\t${file.mode.toString(8)}\t${file.bytes.toString("base64")}`).join("\n")}\n`; }
function runHeldHelper(helper, destination) { const input = helperInput(destination); return new Promise((resolve, reject) => { const child = spawn(helper, ["--test-gate"], { stdio: ["pipe", "pipe", "pipe"], windowsHide: true }); let stdout = "", stderr = ""; child.stdout.on("data", (value) => { stdout += value; }); child.stderr.on("data", (value) => { stderr += value; }); child.once("error", reject); child.once("close", (code) => { if (code === 0 && stderr === "" && /^ok\t[0-9a-f]{64}\n$/.test(stdout)) resolve(); else { const error = new Error(`held helper failed: ${stdout}${stderr}`); error.code = stdout.match(/^error\t([^\n]+)/)?.[1] ?? "write_rejected"; reject(error); } }); child.stdin.end(input); }); }
function digest(value) { return createHash("sha256").update(value).digest("hex"); }
function archive(payload) { const blocks = []; for (const [path, value] of Object.entries(payload)) { const bytes = Buffer.from(value), header = Buffer.alloc(512); Buffer.from(path).copy(header); Buffer.from("0000644\0").copy(header, 100); Buffer.from(`${bytes.length.toString(8).padStart(11, "0")}\0`).copy(header, 124); header[156] = 48; blocks.push(header, bytes, Buffer.alloc((512 - bytes.length % 512) % 512)); } return gzipSync(Buffer.concat([...blocks, Buffer.alloc(1024)])); }
async function controlledBundle(root) {
  const payload = { "service.json": '{"id":"controlled-primary"}\n', "config/example.env": "PORT=8080\n" };
  const inventory = Object.entries(payload).map(([path, value]) => ({ path, sha256: digest(value), mode: "0644", bytes: Buffer.byteLength(value) }));
  const contract = Buffer.from(`${JSON.stringify({ schemaVersion: 1, contractDigest: "2".repeat(64), inventory })}\n`);
  const archiveBytes = archive(payload);
  const candidate = { schemaVersion: 1, templateCommit: "1".repeat(40), templateVersion: "9.9.9", contractDigest: "2".repeat(64), contractSha256: digest(contract), archiveSha256: digest(archiveBytes), releaseTag: `template-v9.9.9-${"1".repeat(40)}` };
  const provenance = { schemaVersion: 1, templateRepository: "service-lasso/service-template", templateCommit: candidate.templateCommit, templateVersion: candidate.templateVersion, contractDigest: candidate.contractDigest, catalogIdentity: "controlled-source-test-fixture", origin: { kind: "controlled-source-test" } };
  await Promise.all([writeFile(join(root, "template-contract.json"), contract), writeFile(join(root, "template-candidate.json"), `${JSON.stringify(candidate)}\n`), writeFile(join(root, "template-provenance.json"), `${JSON.stringify(provenance)}\n`), writeFile(join(root, "service-template.tar.gz"), archiveBytes), ...Object.entries(payload).map(([path, value]) => { const index = path.lastIndexOf("/"); return (index < 0 ? Promise.resolve() : mkdir(join(root, path.slice(0, index)), { recursive: true })).then(() => writeFile(join(root, path), value)); })]);
}
function runNative(executable, args, environment) { return new Promise((resolve, reject) => { const child = spawn(executable, args, { env: environment, windowsHide: true, stdio: ["ignore", "pipe", "pipe"] }); let stdout = "", stderr = ""; child.stdout.on("data", (value) => { stdout += value; }); child.stderr.on("data", (value) => { stderr += value; }); child.once("error", reject); child.once("close", (code, signal) => resolve({ code, signal, stdout, stderr })); }); }

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
    const seaBundle = await readFile(join(output, "service-lassoctl.cjs"), "utf8");
    assert.match(seaBundle, new RegExp(provenance.confinedWriter.sha256));
    assert.doesNotMatch(seaBundle, /controlled-source-test-fixture/);
    execFileSync(node, ["scripts/write-native-ci-context.mjs", "--directory", output, "--event-name", "workflow_dispatch", "--source-sha", sourceSha, "--tested-base-sha", "", "--merge-context-sha", sourceSha], { encoding: "utf8" });
    const smokeReceipts = join(output, "smoke-receipts"); await mkdir(smokeReceipts);
    await writeFile(join(smokeReceipts, "initial.json"), JSON.stringify({ schemaVersion: 2, ownedBirth: true, actualClose: null, sourceHead: sourceSha, rawHeadSha256: "a".repeat(64), recursiveHeadTreeSha256: "b".repeat(64) }));
    if (process.platform === "darwin") {
      const withoutAuthority = { ...process.env };
      delete withoutAuthority.SERVICE_LASSO_DARWIN_PRIVILEGED_HELPER;
      delete withoutAuthority.SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD;
      const denied = spawnSync(join(output, provenance.executable.name), ["--version"], { encoding: "utf8", env: withoutAuthority });
      assert.equal(denied.status, 1);
      assert.equal(denied.stderr, "The native primary gate could not start (stage=external-helper).\n");
      const blocked = spawnSync(node, ["scripts/record-darwin-qualification-block.mjs", "--receipt-directory", smokeReceipts, "--directory", output], { encoding: "utf8" });
      assert.equal(blocked.status, 1);
      const record = JSON.parse(await readFile(join(smokeReceipts, "darwin-prerequisite-block.json"), "utf8"));
      assert.equal(record.status, "blocked"); assert.equal(record.productStarted, false);
      assert.equal(record.sourceHead, sourceSha); assert.equal(record.executableSha256, provenance.executable.sha256);
      assert.equal(JSON.parse(await readFile(join(smokeReceipts, "initial.json"), "utf8")).actualClose, null);
    }
    // Actual workflow wrapper, literal interpreter and smoke script, including
    // Windows: no shell and no npm.cmd command-string forwarding.
    execFileSync(node, ["scripts/run-native-qualification-phase.mjs", "--receipt-directory", smokeReceipts, "--phase", "smoke", "--executable", join(output, provenance.executable.name), "--", "node", "scripts/smoke-native.mjs", "--directory", output, "--expected-source-sha", sourceSha, "--expected-version", version, "--write-host-acceptance"], { encoding: "utf8" });
    const smokeRecord = JSON.parse(await readFile(join(smokeReceipts, "phase-smoke.json"), "utf8"));
    assert.deepEqual(smokeRecord.rawClose, { code: 0, signal: null, spawnError: null });
    assert.equal(smokeRecord.passed, true);
    const acceptance = JSON.parse(await readFile(join(output, "host-acceptance.json"), "utf8"));
    assert.equal(acceptance.version, version);
    assert.equal(acceptance.sourceSha, sourceSha);
    assert.equal(acceptance.nodeAbsentFromPath, true);
    if (process.platform === "darwin") {
      const provisioned = process.env.SERVICE_LASSO_DARWIN_PRIVILEGED_HELPER;
      assert.ok(provisioned, "Darwin qualification requires the provisioned descriptor helper");
      assert.equal(provenance.darwinImmutableHelper?.name, "service-lasso-darwin-immutable-helper");
      // The same job identity attempts replacement after the primary has
      // checked the helper's root-owned parent and embedded binary digest.
      // The provisioned boundary must reject this before any privileged use.
      await assert.rejects(writeFile(provisioned, "replacement"));
    }
    // The published primary contains its SEA and writer. Mutable archive
    // neighbours are evidence only; replacing either must not change normal
    // primary execution.
    const writerPath = join(output, provenance.confinedWriter.name);
    const originalWriter = await readFile(writerPath);
    await writeFile(writerPath, "replaced");
    await writeFile(join(output, "provenance.json"), "{\"replaced\":true}\n");
    const primary = spawnSync(join(output, provenance.executable.name), ["--help"], { encoding: "utf8", windowsHide: true });
    assert.equal(primary.status, 0, primary.stderr);
    assert.match(primary.stdout, /Usage: service-lassoctl/);
    await writeFile(writerPath, originalWriter);
    await writeFile(join(output, "provenance.json"), `${JSON.stringify(provenance, null, 2)}\n`);
    assert.throws(() => execFileSync(node, ["scripts/smoke-native.mjs", "--directory", output, "--expected-source-sha", sourceSha, "--expected-version", "0.1.0-dev.fffffff"], { stdio: "ignore" }));
    const archiveOutput = join(output, "archive");
    await mkdir(archiveOutput);
    execFileSync(node, ["scripts/archive-native-candidate.mjs", "--expected-event", "workflow_dispatch", "--expected-base-sha", "", "--expected-merge-context-sha", sourceSha, "--directory", output, "--output", archiveOutput, "--source-sha", sourceSha, "--version", version], { encoding: "utf8" });
    const executable = join(output, provenance.executable.name);
    const acceptedBytes = await readFile(executable);
    await writeFile(executable, Buffer.concat([acceptedBytes, Buffer.from("changed")]))
    assert.throws(() => execFileSync(node, ["scripts/archive-native-candidate.mjs", "--expected-event", "workflow_dispatch", "--expected-base-sha", "", "--expected-merge-context-sha", sourceSha, "--directory", output, "--output", archiveOutput, "--source-sha", sourceSha, "--version", version], { stdio: "ignore" }));
    await writeFile(executable, acceptedBytes);
    await rm(join(output, "ci-context.json"));
    assert.throws(() => execFileSync(node, ["scripts/archive-native-candidate.mjs", "--expected-event", "workflow_dispatch", "--expected-base-sha", "", "--expected-merge-context-sha", sourceSha, "--directory", output, "--output", archiveOutput, "--source-sha", sourceSha, "--version", version], { stdio: "ignore" }));
    const baseSha = "fedcba9876543210fedcba9876543210fedcba98";
    await writeFile(join(output, "ci-context.json"), `${JSON.stringify({ schemaVersion: 1, eventName: "pull_request", sourceSha, testedBaseSha: baseSha, mergeContextSha: "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" })}\n`);
    execFileSync(node, ["scripts/verify-native-ci-provenance.mjs", "--directory", output, "--expected-source-sha", sourceSha, "--expected-event", "pull_request", "--expected-base-sha", baseSha, "--expected-merge-context-sha", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "--expected-platform", process.platform, "--expected-architecture", process.arch], { encoding: "utf8" });
  } finally {
    await rm(output, { recursive: true, force: true });
  }
});

test("controlled source admission exercises the native primary to SEA to gate to writer route despite a hostile helper PATH", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-primary-route-"));
  // Qualification can pass the already-built executable here.  In that mode
  // this test proves the truthful empty-catalog result; it must never rebuild
  // a different test-admission binary and attach that result to production
  // bytes.
  const suppliedExecutable = process.env.SERVICE_LASSO_CONTROLLED_NATIVE_EXE;
  const output = suppliedExecutable ? undefined : join(root, "native"), templateRoot = join(root, "controlled-template"), destination = join(root, "generated"), hostileDirectory = join(root, "hostile-path");
  const sourceSha = "0123456789abcdef0123456789abcdef01234567", version = "0.1.0-dev.0123456";
  try {
    await mkdir(templateRoot); await mkdir(hostileDirectory); await controlledBundle(templateRoot);
    const hostileWriter = join(hostileDirectory, `service-lasso-confined-scaffold${process.platform === "win32" ? ".exe" : ""}`);
    await writeFile(hostileWriter, process.platform === "win32" ? "not a valid executable" : `#!/bin/sh\nprintf hostile > '${join(root, "hostile-executed").replace(/'/g, "'\\''")}'\nexit 1\n`);
    if (process.platform !== "win32") execFileSync("chmod", ["0700", hostileWriter]);
    if (!suppliedExecutable) execFileSync(node, ["scripts/package-native.mjs", "--output", output, "--source-sha", sourceSha, "--version", version, "--controlled-test-admission"], { encoding: "utf8" });
    const executable = suppliedExecutable ?? join(output, process.platform === "win32" ? "service-lassoctl.exe" : "service-lassoctl");
    const { SERVICE_LASSO_PRIMARY_GATE, SERVICE_LASSO_PRIMARY_GATE_PIPE, SERVICE_LASSO_PRIMARY_GATE_FD, SERVICE_LASSO_PRIMARY_GATE_CAPABILITY, ...inherited } = process.env;
    const result = await runNative(executable, ["service", "init", "controlled-primary", "--template-root", templateRoot, "--directory", destination, "--json"], { ...inherited, PATH: hostileDirectory, SERVICE_LASSO_PRIMARY_GATE: "hostile", SERVICE_LASSO_PRIMARY_GATE_PIPE: "\\\\.\\pipe\\service-lasso-primary-0000000000000000000000000000000000000000000000000000000000000000", SERVICE_LASSO_PRIMARY_GATE_FD: "7", SERVICE_LASSO_PRIMARY_GATE_CAPABILITY: Buffer.alloc(32).toString("base64") });
    if (suppliedExecutable) {
      assert.ok(Number.isInteger(result.code) && result.code > 0, "the normal empty catalog must reject a controlled test bundle");
      assert.equal(result.signal, null);
      assert.match(result.stderr, /Error \[template_identity_unavailable\]: No accepted immutable service-template identity is available;/, "startup or unrelated failures are not an unavailable catalog result");
      await assert.rejects(readFile(join(destination, "service.json")));
      await assert.rejects(lstat(destination), { code: "ENOENT" });
    } else {
      // This is a source-owned, separately built test admission only. It is
      // deliberately never used as a release-qualification receipt.
      assert.equal(result.code, 0, result.stderr);
      assert.match(result.stdout, /"dryRun": false/);
      assert.equal(await readFile(join(destination, "service.json"), "utf8"), '{"id":"controlled-primary"}\n');
    }
    await assert.rejects(readFile(join(root, "hostile-executed")));
    await assert.rejects(lstat(join(root, "hostile-executed")), { code: "ENOENT" });
    if (suppliedExecutable && process.env.SERVICE_LASSO_NATIVE_ROUTE_RESULT) {
      assert.ok(Number.isInteger(result.code) && result.code > 0, "unavailable route requires actual normal nonzero product closure");
      await writeFile(process.env.SERVICE_LASSO_NATIVE_ROUTE_RESULT, `${JSON.stringify({ schemaVersion: 1, status: "unavailable", executableSha256: digest(await readFile(executable)), innerClose: { code: result.code, signal: null }, destinationAbsent: true, hostileHelperAbsent: true })}\n`, { flag: "wx" });
    }
  } finally {
    await rm(root, { recursive: true, force: true });
  }
});

test("actual route wrapper records successful selected-test closure separately and rejects a selected assertion failure", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-route-wrapper-"));
  const sourceSha = "0123456789abcdef0123456789abcdef01234567", version = "0.1.0-dev.0123456", output = join(root, "native");
  try {
    execFileSync(node, ["scripts/package-native.mjs", "--output", output, "--source-sha", sourceSha, "--version", version], { encoding: "utf8" });
    const executable = join(output, process.platform === "win32" ? "service-lassoctl.exe" : "service-lassoctl");
    for (const [label, supplied] of [["valid", executable], ["assertion-failure", node]]) {
      const receipts = join(root, label); await mkdir(receipts);
      await writeFile(join(receipts, "initial.json"), JSON.stringify({ schemaVersion: 2, ownedBirth: true, actualClose: null, sourceHead: sourceSha, rawHeadSha256: "a".repeat(64), recursiveHeadTreeSha256: "b".repeat(64) }));
      const route = join(receipts, "inner-route.json");
      // Supplying Node as the inner executable produces an unrelated normal
      // nonzero error. The actual selected test must assert that this is not
      // the expected empty-catalog result, and the wrapper must fail too.
      const result = spawnSync(node, ["scripts/run-native-qualification-phase.mjs", "--receipt-directory", receipts, "--phase", "route", "--qualification-status", "unavailable", "--expected-exit", "zero", "--route-result", route, "--executable", supplied, "--", node, "--test", "--test-name-pattern", "controlled source admission", "tests/native-distribution.test.js"], { encoding: "utf8", env: { ...process.env, SERVICE_LASSO_CONTROLLED_NATIVE_EXE: supplied, SERVICE_LASSO_NATIVE_ROUTE_RESULT: route } });
      const record = JSON.parse(await readFile(join(receipts, "phase-route.json"), "utf8"));
      if (label === "valid") {
        assert.equal(result.status, 0, result.stderr + result.stdout);
        assert.equal(record.rawClose.code, 0); assert.equal(record.passed, true);
        assert.equal(record.innerRoute.status, "unavailable");
        assert.ok(record.innerRoute.innerClose.code > 0);
        assert.equal(record.innerRoute.executableSha256, digest(await readFile(executable)));
      } else {
        assert.equal(result.status, 1); assert.ok(record.rawClose.code > 0);
        assert.equal(record.passed, false); assert.equal(record.innerRoute, null);
        await assert.rejects(readFile(route), { code: "ENOENT" });
      }
    }
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("packaged confined helper keeps a held parent through a replacement and retains concurrent unowned failure content", async (t) => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-confined-"));
  const output = join(root, "native");
  const sourceSha = "0123456789abcdef0123456789abcdef01234567";
  const version = "0.1.0-dev.0123456";
  const priorGate = process.env.SERVICE_LASSO_CONFINED_TEST_GATE, priorStage = process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE, priorFail = process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE;
  t.after(async () => { if (priorGate === undefined) delete process.env.SERVICE_LASSO_CONFINED_TEST_GATE; else process.env.SERVICE_LASSO_CONFINED_TEST_GATE = priorGate; if (priorStage === undefined) delete process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE; else process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE = priorStage; if (priorFail === undefined) delete process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE; else process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE = priorFail; await rm(root, { recursive: true, force: true }); });
  execFileSync(node, ["scripts/package-native.mjs", "--output", output, "--source-sha", sourceSha, "--version", version], { encoding: "utf8" });
  const provenance = JSON.parse(await readFile(join(output, "provenance.json"), "utf8"));
  const helper = join(output, provenance.confinedWriter.name);
  const parent = join(root, "parent"), oldParent = join(root, "parent-held"), outside = join(root, "outside"), destination = join(parent, "project"), gate = join(root, "swap-gate");
  await mkdir(parent); await mkdir(outside); process.env.SERVICE_LASSO_CONFINED_TEST_GATE = gate; process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE = "before-project-create";
  const creation = runHeldHelper(helper, destination);
  await awaitGate(gate, "before-project-create");
  let replacementBlocked = false;
  try { await rename(parent, oldParent); await symlink(outside, parent, process.platform === "win32" ? "junction" : "dir"); } catch { replacementBlocked = true; }
  await releaseGate(gate, "before-project-create"); await creation;
  await assert.rejects(readFile(join(outside, "project", "service.json")));
  assert.equal(await readFile(join(replacementBlocked ? parent : oldParent, "project", "service.json"), "utf8"), '{"id":"safe"}\n');
  if (process.platform !== "win32") {
    const leafParent = join(root, "leaf-parent"), leafDestination = join(leafParent, "project"), leafGate = join(root, "leaf-gate");
    await mkdir(leafParent); process.env.SERVICE_LASSO_CONFINED_TEST_GATE = leafGate; process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE = "before-project-commit"; delete process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE;
    const leafCreation = runHeldHelper(helper, leafDestination); leafCreation.catch(() => {});
    await awaitGate(leafGate, "before-project-commit"); await mkdir(leafDestination); await writeFile(join(leafDestination, "unowned.txt"), "preserve"); await releaseGate(leafGate, "before-project-commit");
    await assert.rejects(leafCreation, { code: "destination_exists" });
    assert.equal(await readFile(join(leafDestination, "unowned.txt"), "utf8"), "preserve"); await assert.rejects(readFile(join(leafDestination, "service.json")));
  }
  const failureParent = join(root, "failure-parent"), failureDestination = join(failureParent, "project"), failureGate = join(root, "failure-gate");
  await mkdir(failureParent); process.env.SERVICE_LASSO_CONFINED_TEST_GATE = failureGate; process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE = "after-file-write"; process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE = "after-file-write";
  const failure = runHeldHelper(helper, failureDestination); failure.catch(() => {});
  await awaitGate(failureGate, "after-file-write");
  // The Windows writer deliberately holds its private project directory with
  // FILE_SHARE_READ only.  A concurrent caller therefore cannot add a file
  // while the native rollback boundary is live.  POSIX instead proves that an
  // independently-created replacement is retained without recursive cleanup.
  if (process.platform !== "win32") {
    await mkdir(failureDestination);
    await writeFile(join(failureDestination, "unowned.txt"), "preserve");
  }
  await releaseGate(failureGate, "after-file-write");
  await assert.rejects(failure, { code: "write_rejected" });
  if (process.platform !== "win32") {
    assert.equal(await readFile(join(failureDestination, "unowned.txt"), "utf8"), "preserve");
    await assert.rejects(readFile(join(failureDestination, "service.json")));
  }
  if (process.platform !== "win32") assert.deepEqual(await readdir(failureDestination), ["unowned.txt"]);
  if (process.platform === "win32") {
    delete process.env.SERVICE_LASSO_CONFINED_TEST_GATE;
    delete process.env.SERVICE_LASSO_CONFINED_TEST_GATE_STAGE;
    delete process.env.SERVICE_LASSO_CONFINED_TEST_FAIL_AFTER_GATE;
    // A permissive caller-selected parent must not flow into new project
    // objects. The writer applies its protected OWNER RIGHTS DACL at creation.
    const hostileParent = join(root, "hostile-parent"), hostileDestination = join(hostileParent, "project");
    await mkdir(hostileParent);
    execFileSync("icacls", [hostileParent, "/grant", "*S-1-1-0:(OI)(CI)F"], { stdio: "ignore" });
    await runHeldHelper(helper, hostileDestination);
    for (const target of [hostileDestination, join(hostileDestination, "config"), join(hostileDestination, "service.json"), join(hostileDestination, "config", "example.env")]) {
      const acl = execFileSync("icacls", [target], { encoding: "utf8" });
      assert.match(acl, /OWNER RIGHTS:\(F\)/i);
      assert.doesNotMatch(acl, /Everyone:\(F\)/i);
    }
  }
});
