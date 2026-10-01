import assert from "node:assert/strict";
import { execFileSync } from "node:child_process";
import { mkdtemp, readFile, rm } from "node:fs/promises";
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
  assert.match(nativeBuild, /npm run smoke:native .*--expected-version "\$\{\{ needs\.identity\.outputs\.candidate_version \}\}" --write-host-acceptance/);
  assert.match(nativeBuild, /verify-native-ci-provenance\.mjs .*--expected-event workflow_dispatch/);
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
});
