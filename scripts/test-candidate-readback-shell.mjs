import assert from "node:assert/strict";
import { chmod, mkdtemp, readFile, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { spawnSync } from "node:child_process";

const workflow = (await readFile(new URL("../.github/workflows/release.yml", import.meta.url), "utf8")).replace(/\r\n/g, "\n");
const match = workflow.match(/          readback_release_assets\(\) \{\n(?<body>(?: {12}.*\n)+?)          \}\n          if gh release view/);
if (!match?.groups?.body) throw new Error("Candidate readback helper was not found.");
const helper = `readback_release_assets() {\n${match.groups.body.replace(/^ {12}/gm, "")}\n}\n`;

async function run(sequence, expectedStatus, expectedVerifies) {
  const root = await mkdtemp(join(tmpdir(), "lasso-readback-"));
  const bin = join(root, "bin");
  await (await import("node:fs/promises")).mkdir(bin);
  await writeFile(join(bin, "gh"), "#!/usr/bin/env bash\nset -eu\ni=0\nif test -f .attempt; then i=$(cat .attempt); fi\ni=$((i+1)); echo $i > .attempt\ncode=$(echo \"$GH_SEQUENCE\" | cut -d, -f$i)\nif [ \"$code\" = 200 ]; then touch release-verify/candidate.json release-verify/SHA256SUMS.txt release-verify/service-lassoctl.tgz; exit 0; fi\necho \"HTTP $code\" >&2; exit 23\n", { mode: 0o755 });
  await writeFile(join(bin, "node"), "#!/usr/bin/env bash\necho verify >> .verify\nexit 0\n", { mode: 0o755 });
  await chmod(join(bin, "gh"), 0o755); await chmod(join(bin, "node"), 0o755);
  const result = spawnSync("C:/Program Files/Git/bin/bash.exe", ["-c", `set -e; tag=x; ${helper} readback_release_assets`], { cwd: root, env: { ...process.env, PATH: `${bin}:/usr/bin:/bin`, GH_SEQUENCE: sequence }, encoding: "utf8" });
  assert.equal(result.status, expectedStatus, `${sequence}: ${result.stderr}`);
  const verifies = (await readFile(join(root, ".verify"), "utf8").catch(() => "")).trim().split("\n").filter(Boolean).length;
  assert.equal(verifies, expectedVerifies, sequence);
}

await run("404", 23, 0);
await run("500,500,200", 0, 1);
await run("500,500,500", 1, 0);
