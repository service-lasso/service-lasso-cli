import { spawnSync } from "node:child_process";
import { readFile } from "node:fs/promises";

const workflow = await readFile(new URL("../.github/workflows/release.yml", import.meta.url), "utf8");
const match = workflow.match(/- id: identity\n\s+shell: bash\n\s+run: \|\n(?<script>(?: {10}.*\n)+?) {6}- name: Build immutable candidate archive/);
if (!match?.groups?.script) throw new Error("Candidate identity shell block was not found.");
const script = match.groups.script.replace(/^ {10}/gm, "");
const result = spawnSync("bash", ["-n"], { input: script, encoding: "utf8" });
if (result.error?.code === "ENOENT") {
  console.log("bash is unavailable; shell syntax check deferred to Linux CI.");
} else if (result.status !== 0) {
  throw new Error(`Candidate identity shell syntax is invalid: ${result.stderr || result.stdout}`);
}
