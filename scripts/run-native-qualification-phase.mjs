import { createHash } from "node:crypto";
import { readFile, writeFile } from "node:fs/promises";
import { basename, dirname, join, resolve } from "node:path";
import { spawn } from "node:child_process";
import { assertClosedObject, parseStrictJson, readHeldFile } from "./protected-candidate-lib.mjs";

function option(name) {
  const index = process.argv.indexOf(name);
  if (index < 0 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`);
  return process.argv[index + 1];
}
const phase = option("--phase");
if (!new Set(["smoke", "route", "archive"]).has(phase)) throw new Error("Unknown qualification phase.");
const receiptDirectory = resolve(option("--receipt-directory"));
const executable = process.argv.includes("--executable") ? resolve(option("--executable")) : null;
const artifact = process.argv.includes("--artifact") ? resolve(option("--artifact")) : null;
const qualificationStatus = process.argv.includes("--qualification-status") ? option("--qualification-status") : "verified";
if (!new Set(["verified", "unavailable"]).has(qualificationStatus)) throw new Error("Unknown qualification status.");
const expectedExit = process.argv.includes("--expected-exit") ? option("--expected-exit") : "zero";
if (!new Set(["zero", "nonzero"]).has(expectedExit)) throw new Error("--expected-exit must be zero or nonzero.");
const separator = process.argv.indexOf("--");
if (separator < 0 || separator === process.argv.length - 1) throw new Error("A literal -- followed by the owned child command is required.");
const command = process.argv[separator + 1], args = process.argv.slice(separator + 2);
const initial = JSON.parse(await readFile(join(receiptDirectory, "initial.json"), "utf8"));
if (initial.schemaVersion !== 2 || initial.ownedBirth !== true || initial.actualClose !== null) throw new Error("Initial receipt is not open and owned.");
const sha256 = async (path) => createHash("sha256").update(await readFile(path)).digest("hex");
const routeResultPath = phase === "route" && qualificationStatus === "unavailable" ? resolve(option("--route-result")) : null;
if (routeResultPath && (expectedExit !== "zero" || !executable || !process.env.SERVICE_LASSO_NATIVE_ROUTE_RESULT || resolve(process.env.SERVICE_LASSO_NATIVE_ROUTE_RESULT) !== routeResultPath)) throw new Error("Unavailable route requires a zero-exit test runner and explicit inner-result path.");
if (routeResultPath) {
  try { await readFile(routeResultPath); throw new Error("Route result must be absent before invocation."); }
  catch (error) { if (error.code !== "ENOENT") throw error; }
}

let result;
try {
  result = await new Promise((resolveChild) => {
    const child = spawn(command, args, { stdio: "inherit", shell: false });
    child.once("error", (error) => resolveChild({ code: null, signal: null, spawnError: error.code ?? "spawn_error" }));
    child.once("close", (code, signal) => resolveChild({ code, signal, spawnError: null }));
  });
} finally {
  // The raw result below is written after the child has closed, including a
  // signal or spawn failure. A later close step can never substitute success.
}
const record = {
  schemaVersion: 1,
  phase,
  sourceHead: initial.sourceHead,
  rawHeadSha256: initial.rawHeadSha256,
  recursiveHeadTreeSha256: initial.recursiveHeadTreeSha256,
  command: basename(command),
  argumentsSha256: createHash("sha256").update(JSON.stringify(args)).digest("hex"),
  nativeExecutableSha256: executable ? await sha256(executable) : null,
  artifactSha256: artifact ? await sha256(artifact) : null,
  qualificationStatus,
  expectedExit,
  rawClose: result,
  passed: expectedExit === "zero"
    ? result.code === 0 && result.signal === null && result.spawnError === null
    : typeof result.code === "number" && result.code !== 0 && result.signal === null && result.spawnError === null,
};
record.innerRoute = null;
if (routeResultPath && record.passed) {
  try {
    const inner = parseStrictJson(await readHeldFile(dirname(routeResultPath), basename(routeResultPath), undefined, { maxBytes: 16 * 1024 }), "native inner route");
    assertClosedObject(inner, ["schemaVersion", "status", "executableSha256", "innerClose", "destinationAbsent", "hostileHelperAbsent"], "native inner route");
    assertClosedObject(inner.innerClose, ["code", "signal"], "native inner close");
    if (inner.schemaVersion !== 1 || inner.status !== "unavailable" || inner.executableSha256 !== record.nativeExecutableSha256 || !Number.isInteger(inner.innerClose.code) || inner.innerClose.code <= 0 || inner.innerClose.signal !== null || inner.destinationAbsent !== true || inner.hostileHelperAbsent !== true) throw new Error("Invalid inner route.");
    record.innerRoute = inner;
  } catch { record.passed = false; }
}
await writeFile(join(receiptDirectory, `phase-${phase}.json`), `${JSON.stringify(record, null, 2)}\n`, { flag: "wx" });
process.stdout.write(`${JSON.stringify({ phase, expectedExit, rawClose: result, passed: record.passed })}\n`);
if (!record.passed) process.exitCode = 1;
