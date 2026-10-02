import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { parseDocument } from "yaml";

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

function assertProtectedCandidateWorkflow(workflow) {
  assert.deepEqual(Object.keys(workflow.on ?? {}), ["workflow_dispatch"]);
  assert.equal(workflow.permissions?.contents, "read");
  assert.equal(workflow.jobs?.identity?.if, "github.ref == 'refs/heads/develop'");
  assert.deepEqual(workflow.jobs?.native?.strategy?.matrix?.include?.map(({ target }) => target), ["win32-x64", "linux-x64", "darwin-arm64"]);
  const nativeBuild = workflow.jobs?.native?.steps?.find((step) => step.name === "Build target executable once, record dispatch context, and prove its no-Node fixture journey")?.run ?? "";
  assert.equal((nativeBuild.match(/npm run package:native/g) ?? []).length, 1, "each target must build its executable once");
  assert.match(nativeBuild, /write-native-ci-context\.mjs --directory native-assets --event-name workflow_dispatch/);
  assert.match(nativeBuild, /--tested-base-sha "" --merge-context-sha "\$\{\{ needs\.identity\.outputs\.source_sha \}\}"/);
  assert.match(nativeBuild, /verify-native-ci-provenance\.mjs .*--expected-event workflow_dispatch/);
  const nativeSteps = workflow.jobs?.native?.steps ?? [];
  const nativeSmoke = nativeSteps.find((step) => step.name === "Record actual native smoke close result")?.run ?? "";
  assert.match(nativeSmoke, /run-native-qualification-phase\.mjs .*--phase smoke/);
  assert.match(nativeSmoke, /npm run smoke:native .*--expected-version "\$\{\{ needs\.identity\.outputs\.candidate_version \}\}" --write-host-acceptance/);
  const custody = nativeSteps.find((step) => step.name === "Establish isolated native qualification custody before dependencies")?.run ?? "";
  assert.match(custody, /SERVICE_LASSO_WORKSPACE_ROOT=/);
  assert.match(custody, /SERVICE_LASSO_INSTANCE_REGISTRY_PATH=/);
  assert.match(custody, /SERVICE_LASSO_HOST_PORT_REGISTRY_PATH=/);
  assert.match(custody, /prepare-native-qualification-receipt\.mjs/);
  assert.ok(nativeSteps.findIndex((step) => step.name === "Establish isolated native qualification custody before dependencies") < nativeSteps.findIndex((step) => step.run === "npm ci"));
  const darwinRoute = nativeSteps.find((step) => step.name === "Require actual Darwin primary-route materialization under system immutability")?.run ?? "";
  assert.match(darwinRoute, /go test \.\/\.\.\./);
  const primaryRoute = nativeSteps.find((step) => step.name === "Record expected unavailable controlled primary-route result")?.run ?? "";
  assert.match(primaryRoute, /run-native-qualification-phase\.mjs .*--phase route/);
  assert.match(primaryRoute, /controlled source admission/);
  assert.equal(nativeSteps.some((step) => /Provision the bounded Darwin descriptor helper/.test(step.name ?? "")), false);
  assert.match(primaryRoute, /SERVICE_LASSO_CONTROLLED_NATIVE_EXE=/);
  assert.match(primaryRoute, /--qualification-status unavailable/);
  assert.match(primaryRoute, /--expected-exit nonzero/);
  const openReceipt = nativeSteps.find((step) => step.name === "Retain the explicitly open native qualification receipt")?.run ?? "";
  assert.match(openReceipt, /verify-native-qualification-open\.mjs/);
  assert.doesNotMatch(openReceipt, /close-native-qualification-receipt\.mjs|--exit/);
  const archive = workflow.jobs?.native?.steps?.find((step) => step.name === "Archive the accepted native bytes without rebuilding")?.run ?? "";
  assert.match(archive, /npm run archive:native/);
  assert.doesNotMatch(archive, /package:native|smoke:native|write-native-ci-context/);
  assert.deepEqual(workflow.jobs?.assemble?.needs, ["identity", "portable", "native"]);
  assert.equal(workflow.jobs?.publish?.environment?.name, "development-candidate");
  assert.equal(workflow.jobs?.publish?.permissions?.contents, "write");
  assert.match(workflow.jobs?.publish?.steps?.find((step) => step.name === "Preflight and write only accepted immutable bytes")?.run ?? "", /npm run publish:protected-candidate/);
  assert.equal(workflow.jobs?.publish?.steps?.find((step) => step.name === "Preflight and write only accepted immutable bytes")?.env?.DEVELOPMENT_CANDIDATE_TOKEN, "${{ secrets.DEVELOPMENT_CANDIDATE_TOKEN }}");
  for (const job of Object.values(workflow.jobs ?? {})) {
    for (const step of job.steps ?? []) {
      assert.notEqual(step.with?.["persist-credentials"], true);
      assert.doesNotMatch(step.run ?? "", /gh release create/);
    }
  }
}

test("protected candidate workflow freezes develop, builds target bytes once, and gates write access", async () => {
  const source = await readFile(new URL("../.github/workflows/release.yml", import.meta.url), "utf8");
  const document = parseDocument(source, { version: "1.2" });
  assert.equal(document.errors.length, 0, document.errors.map((error) => error.message).join("\n"));
  assertProtectedCandidateWorkflow(document.toJS());
});

test("protected candidate workflow semantic assertions reject missing target and unsafe publication changes", async () => {
  const source = await readFile(new URL("../.github/workflows/release.yml", import.meta.url), "utf8");
  const document = parseDocument(source, { version: "1.2" });
  assert.equal(document.errors.length, 0);
  const missingTarget = document.toJS();
  missingTarget.jobs.native.strategy.matrix.include.pop();
  assert.throws(() => assertProtectedCandidateWorkflow(missingTarget));
  const unsafePublish = document.toJS();
  unsafePublish.jobs.publish.permissions.contents = "read";
  assert.throws(() => assertProtectedCandidateWorkflow(unsafePublish));
  const missingAcceptance = document.toJS();
  missingAcceptance.jobs.native.steps.find((step) => step.name === "Build target executable once, record dispatch context, and prove its no-Node fixture journey").run = "npm run package:native -- --output native-assets\nnpm run smoke:native -- --directory native-assets --expected-source-sha x --expected-version 0.1.0-dev.0000000";
  assert.throws(() => assertProtectedCandidateWorkflow(missingAcceptance));
});

test("ordinary native CI passes the frozen seven-character candidate version to its exact-version smoke", async () => {
  const source = await readFile(new URL("../.github/workflows/ci.yml", import.meta.url), "utf8");
  const document = parseDocument(source, { version: "1.2" });
  assert.equal(document.errors.length, 0, document.errors.map((error) => error.message).join("\n"));
  const native = document.toJS().jobs["native-sea"];
  const revision = native.steps.find((step) => step.id === "revision").run;
  const build = native.steps.find((step) => step.name === "Build host-native executable and provenance").run;
  const smoke = native.steps.find((step) => step.name === "Execute native binary with Node absent from PATH").run;
  assert.match(revision, /candidate_version=0\.1\.0-dev\.\$\{source_sha::7\}/);
  assert.match(build, /--version "\$\{\{ steps\.revision\.outputs\.candidate_version \}\}"/);
  assert.match(smoke, /--expected-version "\$\{\{ steps\.revision\.outputs\.candidate_version \}\}"/);
  assert.doesNotMatch(smoke, /expected-version "0\.1\.0-dev\.\$\{\{ steps\.revision\.outputs\.source_sha \}\}"/);
  const steps = native.steps;
  const custody = steps.find((step) => step.name === "Establish isolated native qualification custody before dependencies")?.run ?? "";
  assert.match(custody, /SERVICE_LASSO_WORKSPACE_ROOT=/);
  assert.match(custody, /SERVICE_LASSO_INSTANCE_REGISTRY_PATH=/);
  assert.match(custody, /SERVICE_LASSO_HOST_PORT_REGISTRY_PATH=/);
  assert.match(custody, /prepare-native-qualification-receipt\.mjs/);
  assert.ok(steps.findIndex((step) => step.name === "Establish isolated native qualification custody before dependencies") < steps.findIndex((step) => step.run === "npm ci"));
  const route = steps.find((step) => step.name === "Record expected unavailable controlled primary-route result")?.run ?? "";
  assert.match(route, /run-native-qualification-phase\.mjs .*--phase route/);
  assert.match(route, /controlled source admission/);
  assert.equal(steps.some((step) => /Provision the bounded Darwin descriptor helper/.test(step.name ?? "")), false);
  assert.match(route, /SERVICE_LASSO_CONTROLLED_NATIVE_EXE=/);
  assert.match(route, /--qualification-status unavailable/);
  assert.match(route, /--expected-exit nonzero/);
  const ciOpen = steps.find((step) => step.name === "Retain the explicitly open native qualification receipt")?.run ?? "";
  assert.match(ciOpen, /verify-native-qualification-open\.mjs/);
  assert.doesNotMatch(ciOpen, /close-native-qualification-receipt\.mjs|--exit/);
});

test("native qualification closes only durable, successful child waits bound to the accepted executable", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-receipt-"));
  const receipt = join(root, "receipts"), executable = join(root, "service-lassoctl"), archive = join(root, "accepted.tar.gz");
  const environment = { ...process.env, SERVICE_LASSO_WORKSPACE_ROOT: join(root, "workspace"), SERVICE_LASSO_INSTANCE_REGISTRY_PATH: join(root, "instance", "registry.json"), SERVICE_LASSO_HOST_PORT_REGISTRY_PATH: join(root, "ports", "registry.json") };
  try {
    await writeFile(executable, "accepted-executable");
    await writeFile(archive, "accepted-archive");
    execFileSync(node, ["scripts/prepare-native-qualification-receipt.mjs", "--receipt-directory", receipt], { env: environment, encoding: "utf8" });
    for (const phase of ["smoke", "route", "archive"]) {
      const args = ["scripts/run-native-qualification-phase.mjs", "--receipt-directory", receipt, "--phase", phase, "--executable", executable];
      if (phase === "archive") args.push("--artifact", archive);
      args.push("--", node, "-e", "process.exit(0)");
      execFileSync(node, args, { encoding: "utf8" });
    }
    assert.throws(() => execFileSync(node, ["scripts/close-native-qualification-receipt.mjs", "--receipt-directory", receipt, "--executable", executable, "--exit", "0"], { stdio: "ignore" }));
    execFileSync(node, ["scripts/close-native-qualification-receipt.mjs", "--receipt-directory", receipt, "--executable", executable], { encoding: "utf8" });
    const closed = JSON.parse(await readFile(join(receipt, "closed.json"), "utf8"));
    assert.equal(closed.actualClose.outcome, "passed");
    assert.deepEqual(closed.actualClose.rawPhaseResults.smoke, { code: 0, signal: null, spawnError: null });
  } finally { await rm(root, { recursive: true, force: true }); }
});

test("an unavailable controlled route remains an open receipt and cannot close native qualification", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lassoctl-open-receipt-"));
  const receipt = join(root, "receipts"), executable = join(root, "service-lassoctl"), archive = join(root, "candidate.tar.gz");
  const environment = { ...process.env, SERVICE_LASSO_WORKSPACE_ROOT: join(root, "workspace"), SERVICE_LASSO_INSTANCE_REGISTRY_PATH: join(root, "instance", "registry.json"), SERVICE_LASSO_HOST_PORT_REGISTRY_PATH: join(root, "ports", "registry.json") };
  try {
    await writeFile(executable, "unqualified-executable");
    await writeFile(archive, "unqualified-archive");
    execFileSync(node, ["scripts/prepare-native-qualification-receipt.mjs", "--receipt-directory", receipt], { env: environment, encoding: "utf8" });
    execFileSync(node, ["scripts/run-native-qualification-phase.mjs", "--receipt-directory", receipt, "--phase", "smoke", "--executable", executable, "--", node, "-e", "process.exit(0)"], { encoding: "utf8" });
    execFileSync(node, ["scripts/run-native-qualification-phase.mjs", "--receipt-directory", receipt, "--phase", "route", "--qualification-status", "unavailable", "--expected-exit", "nonzero", "--executable", executable, "--", node, "-e", "process.exit(7)"], { encoding: "utf8" });
    execFileSync(node, ["scripts/run-native-qualification-phase.mjs", "--receipt-directory", receipt, "--phase", "archive", "--executable", executable, "--artifact", archive, "--", node, "-e", "process.exit(0)"], { encoding: "utf8" });
    execFileSync(node, ["scripts/verify-native-qualification-open.mjs", "--receipt-directory", receipt, "--executable", executable], { encoding: "utf8" });
    await assert.rejects(readFile(join(receipt, "closed.json")));
    assert.throws(() => execFileSync(node, ["scripts/close-native-qualification-receipt.mjs", "--receipt-directory", receipt, "--executable", executable], { stdio: "ignore" }));
    const route = JSON.parse(await readFile(join(receipt, "phase-route.json"), "utf8"));
    assert.equal(route.qualificationStatus, "unavailable");
    assert.equal(route.expectedExit, "nonzero");
    assert.equal(route.rawClose.code, 7);
  } finally { await rm(root, { recursive: true, force: true }); }
});
