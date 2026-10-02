import { createHash } from "node:crypto";
import { readFile, writeFile } from "node:fs/promises";
import { join, resolve } from "node:path";

function argument(name) {
  const index = process.argv.indexOf(name);
  if (index < 0 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`);
  return resolve(process.argv[index + 1]);
}
if (process.platform !== "darwin") throw new Error("Darwin prerequisite record requires the actual Darwin host.");
const receipts = argument("--receipt-directory"), directory = argument("--directory");
const initial = JSON.parse(await readFile(join(receipts, "initial.json"), "utf8"));
const provenance = JSON.parse(await readFile(join(directory, "provenance.json"), "utf8"));
const executableSha256 = createHash("sha256").update(await readFile(join(directory, "service-lassoctl"))).digest("hex");
if (initial.schemaVersion !== 2 || initial.ownedBirth !== true || initial.actualClose !== null || provenance.source?.commit !== initial.sourceHead || provenance.executable?.platform !== "darwin" || provenance.executable?.sha256 !== executableSha256) throw new Error("Darwin prerequisite block does not bind the frozen initial source and executable.");
const record = {
  schemaVersion: 1, sourceHead: initial.sourceHead, executableSha256,
  status: "blocked", productStarted: false,
  blocker: "external-darwin-qualification-authority-not-integrated",
  contract: "docs/native-qualification-prerequisites.md",
};
// Hosted workflow steps do not transport the descriptor or coordinate the
// protected per-object grant. Presence of environment text cannot admit it.
await writeFile(join(receipts, "darwin-prerequisite-block.json"), `${JSON.stringify(record, null, 2)}\n`, { flag: "wx" });
process.stderr.write("Darwin native qualification blocked: external helper, per-object grant and inherited capability transport require owner-controlled integration.\n");
process.exitCode = 1;
