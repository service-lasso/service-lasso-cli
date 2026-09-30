import assert from "node:assert/strict";
import { spawn, spawnSync } from "node:child_process";
import { readFile } from "node:fs/promises";
import { createServer } from "node:http";
import { resolve, join } from "node:path";

function argument(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing required ${name} argument.`);
  return process.argv[index + 1];
}

function nodeFreeEnvironment(extra = {}) {
  const environment = { ...process.env, ...extra };
  delete environment.NODE_OPTIONS;
  delete environment.NODE_PATH;
  if (process.platform === "win32") environment.Path = `${process.env.SystemRoot}\\System32;${process.env.SystemRoot}`;
  else environment.PATH = "/usr/bin:/bin";
  return environment;
}

function run(executable, args, environment) {
  return new Promise((resolveRun, rejectRun) => {
    const child = spawn(executable, args, { env: environment });
    let stdout = "";
    let stderr = "";
    child.stdout.on("data", (chunk) => { stdout += chunk; });
    child.stderr.on("data", (chunk) => { stderr += chunk; });
    child.once("error", rejectRun);
    child.once("close", (status) => resolveRun({ status, stdout, stderr }));
  });
}

const directory = resolve(argument("--directory"));
const expectedSourceSha = argument("--expected-source-sha");
const executableName = process.platform === "win32" ? "service-lassoctl.exe" : "service-lassoctl";
const executable = join(directory, executableName);
const provenance = JSON.parse(await readFile(join(directory, "provenance.json"), "utf8"));
assert.equal(provenance.source.commit, expectedSourceSha);
assert.equal(provenance.executable.name, executableName);
assert.equal(provenance.executable.platform, process.platform);
assert.equal(provenance.executable.architecture, process.arch);

const withoutNode = nodeFreeEnvironment();
const nodeLookup = spawnSync("node", ["--version"], { encoding: "utf8", env: withoutNode });
assert.equal(nodeLookup.error?.code, "ENOENT", "native smoke must remove node from PATH");
for (const args of [["--help"], ["--version"]]) {
  const result = await run(executable, args, withoutNode);
  assert.equal(result.status, 0, result.stderr);
}

const sentinel = "native-cli-credential-sentinel";
const server = createServer((request, response) => {
  assert.equal(request.headers.authorization, `Bearer ${sentinel}`);
  response.setHeader("content-type", "application/json");
  response.end(JSON.stringify({ status: "ok" }));
});
await new Promise((resolveListen) => server.listen(0, "127.0.0.1", resolveListen));
try {
  const address = server.address();
  assert(address && typeof address !== "string");
  const environment = nodeFreeEnvironment({ SERVICE_LASSO_CORE_URL: `http://127.0.0.1:${address.port}`, SERVICE_LASSO_CORE_TOKEN: sentinel });
  const read = await run(executable, ["instance", "status", "--json"], environment);
  assert.equal(read.status, 0, read.stderr);
  assert.deepEqual(JSON.parse(read.stdout), { status: "ok" });
  const invalid = await run(executable, ["service", "start"], environment);
  assert.notEqual(invalid.status, 0);
  assert.doesNotMatch(`${read.stdout}${read.stderr}${invalid.stdout}${invalid.stderr}`, new RegExp(sentinel));
} finally {
  await new Promise((resolveClose, rejectClose) => server.close((error) => error ? rejectClose(error) : resolveClose()));
}

process.stdout.write(`${JSON.stringify({ command: provenance.command, executable: provenance.executable, source: provenance.source, nodeAbsentFromPath: true })}\n`);
