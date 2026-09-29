import assert from "node:assert/strict";
import { access, mkdtemp } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { createServiceScaffold, scaffoldFiles, validateServiceId } from "../dist/scaffold.js";
import { SERVICE_TEMPLATE_IDENTITY } from "../dist/template.js";

test("rejects unsafe service identifiers", () => {
  assert.throws(() => validateServiceId("Bad_ID"), { code: "invalid_service_id" });
  assert.throws(() => validateServiceId("a"), { code: "invalid_service_id" });
});

test("plans a scaffold without writes and refuses overwrite", async () => {
  const root = await mkdtemp(join(tmpdir(), "lasso-cli-scaffold-"));
  const destination = join(root, "lasso-example");
  const planned = await createServiceScaffold({ id: "example-service", directory: destination, dryRun: true });
  assert.equal(planned.dryRun, true);
  await assert.rejects(() => access(destination));
  const created = await createServiceScaffold({ id: "example-service", directory: destination });
  assert.equal(created.files.includes("service.json"), true);
  await assert.rejects(() => createServiceScaffold({ id: "example-service", directory: destination }), { code: "target_exists" });
  assert.match(scaffoldFiles({ id: "example-service", directory: destination })["service.json"], /"enabled": false/);
});

test("pins the canonical template identity and emits required lifecycle declarations", () => {
  const files = scaffoldFiles({ id: "example-service", directory: "unused" });
  const manifest = JSON.parse(files["service.json"]);
  assert.equal(JSON.parse(files[".service-lasso-template.json"]).commit, SERVICE_TEMPLATE_IDENTITY.commit);
  assert.equal(manifest.execconfig.healthcheck.type, "process");
  assert.deepEqual(manifest.execconfig.depend_on, []);
  for (const action of ["install", "config", "start", "stop"]) assert.ok(manifest.actions[action]);
  assert.doesNotMatch(files["service.json"], /"channel": "latest"/);
});
