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
const exit = Number(argument("--exit"));
if (!Number.isInteger(exit)) throw new Error("--exit must be an integer.");
const initial = JSON.parse(await readFile(join(receiptDirectory, "initial.json"), "utf8"));
if (initial.nativeExecutableSha256 !== null || initial.actualCloseExit !== null || initial.ownedBirth !== true) throw new Error("Initial receipt is not an open, owned qualification record.");
const bytes = await readFile(executable);
initial.nativeExecutableSha256 = createHash("sha256").update(bytes).digest("hex");
initial.actualCloseExit = exit;
await writeFile(join(receiptDirectory, "closed.json"), `${JSON.stringify(initial, null, 2)}\n`, { flag: "wx" });
process.stdout.write(`${JSON.stringify({ executableSha256: initial.nativeExecutableSha256, actualCloseExit: exit })}\n`);
