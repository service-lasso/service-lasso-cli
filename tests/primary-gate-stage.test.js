import assert from "node:assert/strict";
import test from "node:test";
import { execFileSync } from "node:child_process";
import { mkdtemp, readFile, readdir, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join, resolve } from "node:path";
import { stagePrimaryGate } from "../scripts/stage-primary-gate.mjs";

test("actual package-native Darwin stage includes exact owner client and builds the staged primary", async () => {
  const root = await mkdtemp(join(tmpdir(), "service-lasso-primary-stage-"));
  try {
    const source = resolve("native/primary-gate"), build = join(root, ".primary-gate-build");
    const sea = join(root, "sea"), writer = join(root, "writer"), helperDigest = "a".repeat(64);
    await writeFile(sea, "staged-sea-bytes"); await writeFile(writer, "staged-writer-bytes");
    await stagePrimaryGate({ source, build, sea, writer, platform: "darwin", darwinHelperSha256: helperDigest });
    assert.deepEqual((await readdir(build)).sort(), ["assets", "darwin_owner.go", "go.mod", "go.sum", "ipc.go", "main.go", "main_darwin_helper.go", "main_linux.go", "main_unix.go", "main_windows.go"]);
    for (const name of (await readdir(build)).filter((name) => name !== "assets")) {
      assert.deepEqual(await readFile(join(build, name)), await readFile(join(source, name)), name);
    }
    assert.deepEqual(await readFile(join(build, "assets/service-lassoctl.sea")), await readFile(sea));
    assert.deepEqual(await readFile(join(build, "assets/service-lasso-confined-scaffold")), await readFile(writer));
    assert.equal(await readFile(join(build, "assets/service-lasso-darwin-immutable-helper.sha256"), "utf8"), `${helperDigest}\n`);
    // Build the directory populated by the actual producer, never the source
    // tree whose extra files could mask an omitted staged dependency. No
    // primary/helper or privilege path is executed by this regression.
    execFileSync("go", ["build", "-trimpath", "-o", join(root, "primary"), "."], { cwd: build, env: { ...process.env, GOOS: "darwin", GOARCH: "arm64" }, encoding: "utf8" });
  } finally { await rm(root, { recursive: true, force: true }); }
});
