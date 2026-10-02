import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { createHash } from "node:crypto";
import { mkdir, mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";

test("actual archive failure retains raw close and truthful missing or unreadable artifact outcomes", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lasso-phase-failure-"));
  try {
    const input = join(root, "missing-input"), output = join(root, "output");
    await mkdir(input); await mkdir(output);
    const initial = { schemaVersion: 2, ownedBirth: true, actualClose: null, sourceHead: "a".repeat(40), rawHeadSha256: "b".repeat(64), recursiveHeadTreeSha256: "c".repeat(64) };
    for (const label of ["actual-producer-failure", "zero-missing", "zero-unreadable", "failed-present", "missing-executable"]) {
      const receipts = join(root, label); await mkdir(receipts);
      await writeFile(join(receipts, "initial.json"), JSON.stringify(initial));
      const artifact = join(receipts, "artifact.tar.gz");
      if (label === "zero-unreadable") await mkdir(artifact);
      if (label === "failed-present") await writeFile(artifact, "retained failed producer bytes");
      const producer = label === "actual-producer-failure"
        ? ["scripts/archive-native-candidate.mjs", "--directory", input, "--output", output, "--version", "0.1.0-dev.aaaaaaa", "--source-sha", initial.sourceHead, "--expected-event", "workflow_dispatch", "--expected-base-sha", "", "--expected-merge-context-sha", initial.sourceHead]
        : ["-e", label === "failed-present" ? "process.exitCode = 7" : "process.exitCode = 0"];
      const result = spawnSync(process.execPath, ["scripts/run-native-qualification-phase.mjs", "--receipt-directory", receipts, "--phase", "archive", "--artifact", artifact, ...(label === "missing-executable" ? ["--executable", join(root, "absent-executable")] : []), "--", process.execPath, ...producer], { encoding: "utf8" });
      assert.equal(result.status, 1, result.stderr + result.stdout);
      const record = JSON.parse(await readFile(join(receipts, "phase-archive.json"), "utf8"));
      assert.equal(record.passed, false);
      assert.equal(record.rawClose.signal, null); assert.equal(record.rawClose.spawnError, null);
      assert.equal(record.sourceHead, initial.sourceHead);
      assert.equal(record.argumentsSha256, createHash("sha256").update(JSON.stringify(producer)).digest("hex"));
      assert.equal(record.rawClose.code, label === "actual-producer-failure" ? 1 : label === "failed-present" ? 7 : 0);
      if (label === "failed-present") {
        assert.equal(record.artifact.status, "present");
        assert.equal(record.artifactSha256, createHash("sha256").update(await readFile(artifact)).digest("hex"));
      } else {
        assert.equal(record.artifactSha256, null);
        assert.equal(record.artifact.status, label === "zero-unreadable" ? "error" : "absent");
        assert.equal(record.artifact.errorCode, label === "zero-unreadable" ? "EISDIR" : "ENOENT");
      }
      if (label === "missing-executable") assert.deepEqual(record.nativeExecutable, { status: "absent", sha256: null, errorCode: "ENOENT" });
      assert.equal(JSON.parse(await readFile(join(receipts, "initial.json"), "utf8")).actualClose, null);
    }
  } finally { await rm(root, { recursive: true, force: true }); }
});
