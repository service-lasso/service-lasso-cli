import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { readFile } from "node:fs/promises";
import { join, resolve } from "node:path";

function argument(name, { allowEmpty = false } = {}) {
  const index = process.argv.indexOf(name);
  const value = process.argv[index + 1];
  if (index === -1 || value === undefined || value.startsWith("--") || (!allowEmpty && value === "")) {
    throw new Error(`Missing required ${name} argument.`);
  }
  return value;
}

function sha256(bytes) {
  return createHash("sha256").update(bytes).digest("hex");
}

const directory = resolve(argument("--directory"));
const sourceSha = argument("--expected-source-sha");
const eventName = argument("--expected-event");
const baseSha = argument("--expected-base-sha", { allowEmpty: true });
const platform = argument("--expected-platform");
const architecture = argument("--expected-architecture");
const provenance = JSON.parse(await readFile(join(directory, "provenance.json"), "utf8"));
const context = JSON.parse(await readFile(join(directory, "ci-context.json"), "utf8"));

assert.match(sourceSha, /^[0-9a-f]{40}$/i, "source SHA must be a full Git revision");
assert.ok(["pull_request", "push"].includes(eventName), "event must be pull_request or push");
assert.equal(context.schemaVersion, 1, "CI context must use schema version 1");
assert.match(context.sourceSha, /^[0-9a-f]{40}$/i, "CI context source SHA must be a full Git revision");
assert.match(provenance.source.commit, /^[0-9a-f]{40}$/i, "native provenance source SHA must be a full Git revision");
assert.match(context.mergeContextSha, /^[0-9a-f]{40}$/i, "CI merge context must be a full Git revision");
assert.equal(provenance.source.commit, sourceSha, "native provenance must name the checked-out source revision");
assert.equal(context.sourceSha, sourceSha, "CI context and native provenance must name the same source revision");
assert.equal(context.eventName, eventName, "CI context must record the triggering event");
if (eventName === "pull_request") {
  assert.match(baseSha, /^[0-9a-f]{40}$/i, "pull-request target base must be a full Git revision");
  assert.match(context.testedBaseSha, /^[0-9a-f]{40}$/i, "CI context pull-request base must be a full Git revision");
  assert.equal(context.testedBaseSha, baseSha, "CI context must record the pull-request base separately");
  assert.match(context.mergeContextSha, /^[0-9a-f]{40}$/i, "pull-request merge context must be recorded separately");
} else {
  assert.equal(baseSha, "", "push events must pass an explicitly empty expected base");
  assert.ok(!Object.hasOwn(context, "testedBaseSha") || context.testedBaseSha === null, "push CI context must omit or null the tested base");
  assert.equal(context.mergeContextSha, sourceSha, "push merge context must equal the source revision");
}
assert.equal(provenance.executable.platform, platform);
assert.equal(provenance.executable.architecture, architecture);
assert.equal(
  provenance.executable.sha256,
  sha256(await readFile(join(directory, provenance.executable.name))),
  "provenance digest must match the uploaded native executable",
);
