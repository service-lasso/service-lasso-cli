import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { execFile, execFileSync } from "node:child_process";
import { createServer } from "node:http";
import { mkdtemp, readFile, readdir, rm } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join, resolve } from "node:path";

function option(name) {
  const index = process.argv.indexOf(name);
  if (index === -1 || !process.argv[index + 1]) throw new Error(`Missing ${name}.`);
  return process.argv[index + 1];
}
function sha256(value) {
  return createHash("sha256").update(value).digest("hex");
}
function run(command, args, cwd) {
  return execFileSync(command, args, {
    cwd,
    encoding: "utf8",
    stdio: ["ignore", "pipe", "pipe"],
    shell: process.platform === "win32",
  });
}
function runAsync(command, args, cwd) {
  return new Promise((resolveRun, rejectRun) => {
    execFile(command, args, { cwd, encoding: "utf8", shell: process.platform === "win32" }, (error, stdout, stderr) => {
      if (error) {
        error.stderr = stderr;
        rejectRun(error);
      } else {
        resolveRun(stdout);
      }
    });
  });
}

const directory = resolve(option("--directory"));
const expectedVersion = option("--expected-version");
const expectedSourceSha = option("--expected-source-sha");
const candidate = JSON.parse(await readFile(join(directory, "candidate.json"), "utf8"));
assert.equal(candidate.version, expectedVersion);
assert.equal(candidate.source.commit, expectedSourceSha);
assert.equal(candidate.package.command, "service-lassoctl");
assert.equal(candidate.package.node, ">=22.12.0");
assert.deepEqual(candidate.platforms, ["win32", "linux", "darwin"]);
assert.equal(candidate.assets.length, 1);
const asset = candidate.assets[0];
assert.equal(asset.name, `service-lassoctl-${expectedVersion}.tgz`);
const archive = await readFile(join(directory, asset.name));
assert.equal(sha256(archive), asset.sha256);
const sums = await readFile(join(directory, "SHA256SUMS.txt"), "utf8");
assert.match(sums, new RegExp(`^${asset.sha256}  ${asset.name}$`, "m"));

const consumer = await mkdtemp(join(tmpdir(), "service-lassoctl-consumer-"));
const npm = process.platform === "win32" ? "npm.cmd" : "npm";
const command = join(consumer, "node_modules", ".bin", process.platform === "win32" ? "service-lassoctl.cmd" : "service-lassoctl");
try {
  run(npm, ["init", "--yes"], consumer);
  run(npm, ["install", "--ignore-scripts", "--no-audit", "--no-fund", join(directory, asset.name)], consumer);
  assert.match(run(command, ["--help"], consumer), /service-lassoctl/);
  assert.equal(run(command, ["--version"], consumer).trim(), expectedVersion);

  const server = createServer((request, response) => {
    const bodies = {
      "/api/health": { ok: true },
      "/api/runtime/instance": { instance: "fixture" },
      "/api/runtime/capabilities": { readOnly: true },
    };
    const body = bodies[request.url];
    response.writeHead(body ? 200 : 404, { "content-type": "application/json" });
    response.end(JSON.stringify(body ?? {}));
  });
  await new Promise((resolveListen) => server.listen(0, "127.0.0.1", resolveListen));
  try {
    const address = server.address();
    assert(address && typeof address !== "string");
    const result = JSON.parse(await runAsync(command, ["--core-url", `http://127.0.0.1:${address.port}`, "instance", "inspect", "--json"], consumer));
    assert.deepEqual(result, { health: { ok: true }, instance: { instance: "fixture" }, capabilities: { readOnly: true } });
  } finally {
    await new Promise((resolveClose, rejectClose) => server.close((error) => error ? rejectClose(error) : resolveClose()));
  }
} finally {
  await rm(consumer, { recursive: true, force: true });
}
