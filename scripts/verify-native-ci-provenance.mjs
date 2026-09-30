import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { readFile } from "node:fs/promises";
import { join, resolve } from "node:path";

function argument(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing required ${name} argument.`);
  return process.argv[index + 1];
}

function sha256(bytes) {
  return createHash("sha256").update(bytes).digest("hex");
}

const directory = resolve(argument("--directory"));
const sourceSha = argument("--expected-source-sha");
const eventName = argument("--expected-event");
const baseSha = argument("--expected-base-sha");
const platform = argument("--expected-platform");
const architecture = argument("--expected-architecture");
const provenance = JSON.parse(await readFile(join(directory, "provenance.json"), "utf8"));
const context = JSON.parse(await readFile(join(directory, "ci-context.json"), "utf8"));

assert.match(sourceSha, /^[0-9a-f]{40}$/i, "source SHA must be a full Git revision");
assert.equal(provenance.source.commit, sourceSha, "native provenance must name the checked-out source revision");
assert.equal(context.sourceSha, sourceSha, "CI context and native provenance must name the same source revision");
assert.equal(context.eventName, eventName, "CI context must record the triggering event");
assert.equal(context.testedBaseSha ?? "", baseSha, "CI context must record the pull-request base separately");
if (eventName === "pull_request") {
  assert.match(baseSha, /^[0-9a-f]{40}$/i, "pull-request target base must be a full Git revision");
  assert.match(context.mergeContextSha, /^[0-9a-f]{40}$/i, "pull-request merge context must be recorded separately");
}
assert.equal(provenance.executable.platform, platform);
assert.equal(provenance.executable.architecture, architecture);
assert.equal(
  provenance.executable.sha256,
  sha256(await readFile(join(directory, provenance.executable.name))),
  "provenance digest must match the uploaded native executable",
);
