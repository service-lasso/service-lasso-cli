import test from "node:test";
import assert from "node:assert/strict";
import { spawn } from "node:child_process";
import { fileURLToPath } from "node:url";
import { waitForOriginalChildFinish } from "../dist/child-finish.js";
import { CliError } from "../dist/errors.js";

test("finish deadline settles its operation while the original child remains live", async () => {
  const child = spawn(process.execPath, [fileURLToPath(new URL("./fixtures/owned-finish-peer.mjs", import.meta.url))], { stdio: ["pipe", "pipe", "pipe", "pipe"], windowsHide: true });
  const originalClose = new Promise(resolve => child.once("close", (code, signal) => resolve({ code, signal })));
  // Keep an owning error listener even after the shared waiter rejects.
  let originalError;
  child.on("error", error => { originalError = error; });
  child.stdio[3].on("error", error => { originalError ??= error; });
  const ready = new Promise((resolve, reject) => {
    let bytes = Buffer.alloc(0);
    const remove = () => { child.stdout.removeListener("data", data); child.removeListener("error", error); child.removeListener("close", close); };
    const error = cause => { remove(); reject(cause); };
    const close = () => error(new Error("Original fixture closed before readiness."));
    const data = chunk => {
      bytes = Buffer.concat([bytes, chunk]);
      if (bytes.length > 5) { error(new Error("Unexpected original fixture readiness bytes.")); return; }
      if (bytes.length === 5) { remove(); resolve(bytes); }
    };
    child.stdout.on("data", data); child.once("error", error); child.once("close", close);
  });
  child.stderr.resume();
  try {
    assert.equal((await ready).toString("utf8"), "live\n");
    child.stdout.resume();
    const finishing = waitForOriginalChildFinish(child, () => 0);
    child.stdin.end();
    const deadline = setTimeout(() => finishing.reject(new CliError("template_session_unavailable", "The original template authoring session is unavailable.")), 25);
    try { await assert.rejects(finishing.completion, { code: "template_session_unavailable" }); } finally { clearTimeout(deadline); }
    assert.equal(child.exitCode, null);
    assert.equal(child.signalCode, null);
    assert.equal(child.stdio[3].destroyed, false);
    // Operation rejection has neither killed the child nor manufactured native
    // close/EOF. The SAME owned release pipe now requests natural fixture exit.
  } finally {
    if (!child.stdio[3].destroyed) child.stdio[3].end("release\n");
    const closed = await originalClose;
    assert.deepEqual(closed, { code: 0, signal: null });
    if (originalError) throw originalError;
  }
});
