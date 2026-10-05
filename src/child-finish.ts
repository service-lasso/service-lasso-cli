import type { ChildProcessWithoutNullStreams } from "node:child_process";
import { CliError } from "./errors.js";

export interface OriginalChildFinish {
  completion: Promise<void>;
  reject: (error: unknown) => void;
}
/** Settles the original operation without destroying, killing or retiring its
 * native child. Ordinary close/EOF here is not an authoring qualification proof;
 * native custody and independently validated capture/retirement remain separate. */
export function waitForOriginalChildFinish(child: ChildProcessWithoutNullStreams, remaining: () => number): OriginalChildFinish {
  let settled = false, closed = false;
  let stdoutEOF = child.stdout.readableEnded, stderrEOF = child.stderr.readableEnded;
  let resolve!: () => void, reject!: (error: unknown) => void;
  const completion = new Promise<void>((yes, no) => { resolve = yes; reject = no; });
  const unavailable = () => new CliError("template_session_unavailable", "The original template authoring session is unavailable.");
  const remove = () => {
    child.removeListener("close", onClose); child.removeListener("error", onError);
    child.stdout.removeListener("end", onStdout); child.stderr.removeListener("end", onStderr);
    child.stdin.removeListener("error", onError); child.stdout.removeListener("error", onError); child.stderr.removeListener("error", onError);
  };
  const fail = (error: unknown) => { if (!settled) { settled = true; remove(); reject(error); } };
  const done = () => {
    if (settled || !closed || !stdoutEOF || !stderrEOF) return;
    try { if (remaining() !== 0) { fail(unavailable()); return; } } catch { fail(unavailable()); return; }
    settled = true; remove(); resolve();
  };
  const onClose = (code: number | null, signal: NodeJS.Signals | null) => {
    if (code !== 0 || signal !== null) { fail(unavailable()); return; }
    closed = true; done();
  };
  const onError = () => fail(unavailable());
  const onStdout = () => { stdoutEOF = true; done(); };
  const onStderr = () => { stderrEOF = true; done(); };
  child.once("close", onClose); child.once("error", onError);
  child.stdout.once("end", onStdout); child.stderr.once("end", onStderr);
  child.stdin.once("error", onError); child.stdout.once("error", onError); child.stderr.once("error", onError);
  return { completion, reject: fail };
}
