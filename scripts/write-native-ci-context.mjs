import assert from "node:assert/strict";
import { writeFile } from "node:fs/promises";
import { join, resolve } from "node:path";

function argument(name, { allowEmpty = false } = {}) {
  const index = process.argv.indexOf(name);
  const value = process.argv[index + 1];
  if (index === -1 || value === undefined || value.startsWith("--") || (!allowEmpty && value === "")) throw new Error(`Missing required ${name} argument.`);
  return value;
}

const directory = resolve(argument("--directory"));
const eventName = argument("--event-name");
const sourceSha = argument("--source-sha");
const testedBaseSha = argument("--tested-base-sha", { allowEmpty: true });
const mergeContextSha = argument("--merge-context-sha");

assert.match(sourceSha, /^[0-9a-f]{40}$/i, "source SHA must be a full Git revision");
assert.match(mergeContextSha, /^[0-9a-f]{40}$/i, "merge context SHA must be a full Git revision");
assert.ok(["push", "pull_request", "workflow_dispatch"].includes(eventName), "event must be push, pull_request, or workflow_dispatch");
if (eventName === "pull_request") {
  assert.match(testedBaseSha, /^[0-9a-f]{40}$/i, "pull-request base must be a full Git revision");
  assert.equal(new Set([sourceSha, testedBaseSha, mergeContextSha]).size, 3, "pull-request source, base, and merge context must differ");
} else {
  assert.equal(testedBaseSha, "", "push and workflow-dispatch events must use an explicitly empty tested base");
  assert.equal(mergeContextSha, sourceSha, "push and workflow-dispatch merge context must equal the source revision");
}

const context = { schemaVersion: 1, eventName, sourceSha, testedBaseSha: testedBaseSha || null, mergeContextSha };
await writeFile(join(directory, "ci-context.json"), `${JSON.stringify(context, null, 2)}\n`);
process.stdout.write(`${JSON.stringify(context)}\n`);
