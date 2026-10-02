import { createHash } from "node:crypto";
import { readFile, writeFile } from "node:fs/promises";
import { join, resolve } from "node:path";

function argument(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`);
  return process.argv[index + 1];
}
const receiptDirectory = resolve(argument("--receipt-directory"));
const executable = resolve(argument("--executable"));
if (process.argv.includes("--exit")) throw new Error("A receipt close derives exit status from durable child records; --exit is forbidden.");
const initial = JSON.parse(await readFile(join(receiptDirectory, "initial.json"), "utf8"));
if (initial.schemaVersion !== 2 || initial.actualClose !== null || initial.ownedBirth !== true || initial.nativeExecutableSha256 !== null) throw new Error("Initial receipt is not an open, owned qualification record.");
const executableSha256 = createHash("sha256").update(await readFile(executable)).digest("hex");
const phases = {};
for (const phase of ["smoke", "route", "archive"]) {
  const record = JSON.parse(await readFile(join(receiptDirectory, `phase-${phase}.json`), "utf8"));
  if (record.schemaVersion !== 1 || record.phase !== phase || record.sourceHead !== initial.sourceHead || record.rawHeadSha256 !== initial.rawHeadSha256 || record.recursiveHeadTreeSha256 !== initial.recursiveHeadTreeSha256 || record.nativeExecutableSha256 !== executableSha256 || record.qualificationStatus !== "verified" || record.passed !== true || record.rawClose?.code !== 0 || record.rawClose?.signal !== null || record.rawClose?.spawnError !== null) {
    throw new Error(`Qualification phase ${phase} is unresolved or does not bind the accepted executable.`);
  }
  if (phase === "archive" && (!/^[0-9a-f]{64}$/i.test(record.artifactSha256 ?? ""))) throw new Error("Archive phase does not contain a retained archive digest.");
  phases[phase] = record;
}
const closed = {
  ...initial,
  nativeExecutableSha256: executableSha256,
  phases,
  actualClose: {
    executableSha256,
    rawPhaseResults: Object.fromEntries(Object.entries(phases).map(([name, record]) => [name, record.rawClose])),
    outcome: "passed",
  },
};
await writeFile(join(receiptDirectory, "closed.json"), `${JSON.stringify(closed, null, 2)}\n`, { flag: "wx" });
process.stdout.write(`${JSON.stringify({ executableSha256, actualClose: closed.actualClose })}\n`);
