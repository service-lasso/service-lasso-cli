import { spawnSync } from "node:child_process";
import { readFile } from "node:fs/promises";

const workflow = (await readFile(new URL("../.github/workflows/release.yml", import.meta.url), "utf8")).replace(/\r\n/g, "\n");
const match = workflow.match(/- id: identity\n\s+shell: bash\n\s+run: \|\n(?<script>(?: {10}.*\n)+?) {6}- name: Build immutable candidate archive/);
if (!match?.groups?.script) throw new Error("Candidate identity shell block was not found.");
const script = match.groups.script.replace(/^ {10}/gm, "");
const result = spawnSync("bash", ["-n"], { input: script, encoding: "utf8" });
if (result.error?.code === "ENOENT") {
  console.log("bash is unavailable; shell syntax check deferred to Linux CI.");
} else if (result.status !== 0) {
  throw new Error(`Candidate identity shell syntax is invalid: ${result.stderr || result.stdout}`);
}

const tagger = workflow.match(/else\n\s+git config user\.name "github-actions\[bot\]"\n\s+git config user\.email "41898282\+github-actions\[bot\]@users\.noreply\.github\.com"\n\s+git tag --annotate/);
if (!tagger) throw new Error("Candidate publish must configure the GitHub Actions bot identity immediately before creating an annotated tag.");
