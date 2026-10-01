import { createHash, randomUUID } from "node:crypto";
import { mkdir, writeFile } from "node:fs/promises";
import { isAbsolute, join, resolve } from "node:path";
import { execFileSync } from "node:child_process";

function required(name) {
  const value = process.env[name];
  if (!value || !isAbsolute(value)) throw new Error(`${name} must be a non-empty absolute job-owned path.`);
  return resolve(value);
}

const workspaceRoot = required("SERVICE_LASSO_WORKSPACE_ROOT");
const instanceRegistryPath = required("SERVICE_LASSO_INSTANCE_REGISTRY_PATH");
const hostPortRegistryPath = required("SERVICE_LASSO_HOST_PORT_REGISTRY_PATH");
const values = [workspaceRoot, instanceRegistryPath, hostPortRegistryPath];
if (new Set(values).size !== values.length) throw new Error("Qualification runtime paths must be unique.");
const receiptIndex = process.argv.indexOf("--receipt-directory");
if (receiptIndex === -1 || !process.argv[receiptIndex + 1]) throw new Error("--receipt-directory is required.");
const receiptDirectory = resolve(process.argv[receiptIndex + 1]);

await Promise.all([
  mkdir(workspaceRoot, { recursive: true }),
  mkdir(resolve(instanceRegistryPath, ".."), { recursive: true }),
  mkdir(resolve(hostPortRegistryPath, ".."), { recursive: true }),
  mkdir(receiptDirectory, { recursive: true }),
]);
const git = (...args) => execFileSync("git", args, { encoding: "utf8" }).trim();
const head = git("rev-parse", "HEAD");
const rawHead = git("show", "--format=raw", "--no-patch", "HEAD");
const tree = git("ls-tree", "-r", "-t", "HEAD");
const hash = (value) => createHash("sha256").update(value).digest("hex");
const receipt = {
  schemaVersion: 1,
  runId: randomUUID(),
  sourceHead: head,
  rawHeadSha256: hash(rawHead),
  recursiveHeadTreeSha256: hash(tree),
  runtime: {
    workspaceRoot,
    instanceRegistryPath,
    hostPortRegistryPath,
  },
  // The binary does not exist until the later build step. This explicit null
  // prevents a source-tree digest from being misrepresented as a native hash.
  nativeExecutableSha256: null,
  ownedBirth: true,
  actualCloseExit: null,
};
await writeFile(join(receiptDirectory, "initial.json"), `${JSON.stringify(receipt, null, 2)}\n`, { flag: "wx" });
process.stdout.write(`${JSON.stringify({ sourceHead: head, receipt: join(receiptDirectory, "initial.json") })}\n`);
