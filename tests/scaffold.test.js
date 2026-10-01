import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { access, mkdir, mkdtemp, readFile, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, join } from "node:path";
import test from "node:test";
import { createServiceScaffold, previewServiceScaffold, scaffoldFiles, validateServiceId } from "../dist/scaffold.js";
import { TEMPLATE_CONTRACT_GATE } from "../dist/template.js";

const digest = (value) => createHash("sha256").update(value).digest("hex");
async function acceptedBundle(root) {
  const payload = { ".gitattributes": "* text=auto\n", "service.json": "{\"id\":\"template-service\"}\n", "config/example.env": "PORT=8080\n" };
  for (const [path, value] of Object.entries(payload)) { await mkdir(dirname(join(root, path)), { recursive: true }); await writeFile(join(root, path), value); }
  const inventory = Object.entries(payload).map(([path, value]) => ({ path, sha256: digest(value), mode: "0644", bytes: Buffer.byteLength(value) }));
  const contract = { schemaVersion: 1, inventory }; const contractBytes = Buffer.from(`${JSON.stringify(contract)}\n`); const archiveBytes = Buffer.from("future immutable archive bytes");
  const commit = "a".repeat(40); const contractDigest = "b".repeat(64);
  await writeFile(join(root, "template-contract.json"), contractBytes);
  await writeFile(join(root, "service-template.tar.gz"), archiveBytes);
  await writeFile(join(root, "template-candidate.json"), JSON.stringify({ schemaVersion: 1, templateCommit: commit, templateVersion: "1.2.3", contractDigest, contractSha256: digest(contractBytes), archiveSha256: digest(archiveBytes), releaseTag: `template-v1.2.3-${commit}` }));
  await writeFile(join(root, "template-provenance.json"), JSON.stringify({ schemaVersion: 1, templateRepository: "service-lasso/service-template", templateCommit: commit, templateVersion: "1.2.3", contractDigest, catalogIdentity: "service-template/stable/1.2.3", origin: { kind: "local-archive" } }));
  return inventory;
}

test("rejects unsafe service identifiers", () => {
  assert.throws(() => validateServiceId("Bad_ID"), { code: "invalid_service_id" });
  assert.throws(() => validateServiceId("a"), { code: "invalid_service_id" });
});

test("current source contract previews without writes and cannot become a project", async () => {
  const root = await mkdtemp(join(tmpdir(), "lasso-cli-scaffold-")); const destination = join(root, "lasso-example");
  const planned = previewServiceScaffold({ id: "example-service", directory: destination, dryRun: true });
  assert.equal(planned.writes, false); assert.equal(planned.runtimeMutation, false); assert.equal(planned.template.status, "blocked");
  assert.equal(planned.template.contractDigest, TEMPLATE_CONTRACT_GATE.contractDigest);
  await assert.rejects(() => createServiceScaffold({ id: "example-service", directory: destination }), { code: "template_identity_unavailable" });
  assert.throws(() => scaffoldFiles({ id: "example-service", directory: destination }), { code: "template_identity_unavailable" });
  await assert.rejects(() => access(destination));
});

test("materializes the complete verified future bundle and preserves an existing destination", async () => {
  const root = await mkdtemp(join(tmpdir(), "lasso-cli-accepted-template-")); const templateRoot = join(root, "template"); const destination = join(root, "project"); await mkdir(templateRoot); const inventory = await acceptedBundle(templateRoot);
  const preview = await createServiceScaffold({ id: "example-service", directory: destination, templateRoot, dryRun: true });
  assert.deepEqual(preview.files, inventory.map((entry) => entry.path)); await assert.rejects(() => access(destination));
  const created = await createServiceScaffold({ id: "example-service", directory: destination, templateRoot });
  assert.deepEqual(created.files, inventory.map((entry) => entry.path)); assert.equal(await readFile(join(destination, "config/example.env"), "utf8"), "PORT=8080\n");
  await assert.rejects(() => createServiceScaffold({ id: "example-service", directory: destination, templateRoot }), { code: "EEXIST" });
});

test("rejects traversal and checksum mismatches before a project directory exists", async () => {
  const root = await mkdtemp(join(tmpdir(), "lasso-cli-bad-template-")); const templateRoot = join(root, "template"); const destination = join(root, "project"); await mkdir(templateRoot); await acceptedBundle(templateRoot);
  await writeFile(join(templateRoot, "service.json"), "changed");
  await assert.rejects(() => createServiceScaffold({ id: "example-service", directory: destination, templateRoot }), { code: "invalid_template_bundle" });
  await assert.rejects(() => access(destination));
});
