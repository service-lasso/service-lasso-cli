import assert from "node:assert/strict";
import { access, mkdtemp } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { createServiceScaffold, scaffoldFiles, validateServiceId } from "../dist/scaffold.js";

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
