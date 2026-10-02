import { createHash } from "node:crypto";
import { readFile } from "node:fs/promises";
import { join, resolve } from "node:path";

function argument(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`);
  return process.argv[index + 1];
}

const receiptDirectory = resolve(argument("--receipt-directory"));
const executable = resolve(argument("--executable"));
const initial = JSON.parse(await readFile(join(receiptDirectory, "initial.json"), "utf8"));
if (initial.schemaVersion !== 2 || initial.ownedBirth !== true || initial.actualClose !== null || initial.nativeExecutableSha256 !== null) throw new Error("Initial receipt is not an open, owned qualification record.");
const executableSha256 = createHash("sha256").update(await readFile(executable)).digest("hex");
const expectations = {
  smoke: { qualificationStatus: "verified", expectedExit: "zero", code: 0 },
  route: { qualificationStatus: "unavailable", expectedExit: "zero", code: 0 },
  archive: { qualificationStatus: "verified", expectedExit: "zero", code: 0 },
};
const phases = {};
for (const [phase, expectation] of Object.entries(expectations)) {
  const record = JSON.parse(await readFile(join(receiptDirectory, `phase-${phase}.json`), "utf8"));
  if (record.schemaVersion !== 1 || record.phase !== phase || record.sourceHead !== initial.sourceHead || record.rawHeadSha256 !== initial.rawHeadSha256 || record.recursiveHeadTreeSha256 !== initial.recursiveHeadTreeSha256 || record.nativeExecutableSha256 !== executableSha256 || record.qualificationStatus !== expectation.qualificationStatus || record.expectedExit !== expectation.expectedExit || record.passed !== true || record.rawClose?.signal !== null || record.rawClose?.spawnError !== null || (expectation.code === 0 ? record.rawClose?.code !== 0 : !(typeof record.rawClose?.code === "number" && record.rawClose.code !== 0))) {
    throw new Error(`Open qualification phase ${phase} is malformed or does not bind the retained executable.`);
  }
  if (phase === "archive" && !/^[0-9a-f]{64}$/i.test(record.artifactSha256 ?? "")) throw new Error("Archive phase does not contain a retained archive digest.");
  if (phase === "route" && (record.innerRoute?.schemaVersion !== 1 || record.innerRoute.status !== "unavailable" || record.innerRoute.executableSha256 !== executableSha256 || !Number.isInteger(record.innerRoute.innerClose?.code) || record.innerRoute.innerClose.code <= 0 || record.innerRoute.innerClose.signal !== null || record.innerRoute.destinationAbsent !== true || record.innerRoute.hostileHelperAbsent !== true)) throw new Error("Unavailable product route is not independently recorded.");
  phases[phase] = record;
}
process.stdout.write(`${JSON.stringify({ executableSha256, qualification: "open", route: "unavailable", phases: Object.keys(phases) })}\n`);
