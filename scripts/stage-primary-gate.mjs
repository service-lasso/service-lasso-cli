import { lstat, mkdir, open } from "node:fs/promises";
import { join } from "node:path";

// This is the actual package-native build inventory, shared with its staged
// producer regression. Test-only source files never enter the candidate build.
export async function stagePrimaryGate({ source, build, sea, writer, platform, darwinHelperSha256 }) {
  const original = { source, build, sea, writer, platform, files: [], directories: [], complete: false };
  async function directory(path) {
    const row = { path, attempted: true, returned: false };
    original.directories.push(row);
    try { await mkdir(path, { recursive: false }); row.returned = true; }
    catch (error) { row.returned = true; row.error = error; throw error; }
  }
  async function emit(path, input, value) {
    const row = { path, input, openAttempted: false, writeAttempted: false,
      syncAttempted: false, closeAttempted: false, complete: false };
    original.files.push(row);
    let first;
    try {
      if (input !== null) {
        row.pathBefore = await lstat(input, { bigint: true });
        if (!row.pathBefore.isFile() || row.pathBefore.isSymbolicLink()) throw new Error("Primary staging requires a regular original file");
        row.inputOpenAttempted = true;
        row.inputHandle = await open(input, "r"); row.inputOpenReturned = true;
        row.before = await row.inputHandle.stat({ bigint: true });
        if (!row.before.isFile() || row.pathBefore.dev !== row.before.dev || row.pathBefore.ino !== row.before.ino)
          throw new Error("Primary staging opened a different original file");
        row.inputReadAttempted = true;
        row.body = await row.inputHandle.readFile(); row.inputReadReturned = true;
        row.after = await row.inputHandle.stat({ bigint: true });
        if (!row.after.isFile() || row.after.isSymbolicLink() ||
            row.before.dev !== row.after.dev || row.before.ino !== row.after.ino ||
            row.before.size !== row.after.size || row.before.mtimeNs !== row.after.mtimeNs ||
            row.before.ctimeNs !== row.after.ctimeNs || BigInt(row.body.length) !== row.before.size)
          throw new Error("Primary staging original changed during read");
      } else row.body = value;
      row.openAttempted = true;
      row.handle = await open(path, "wx"); row.openReturned = true;
      row.writeAttempted = true;
      await row.handle.writeFile(row.body); row.writeReturned = true;
      row.syncAttempted = true;
      await row.handle.sync(); row.syncReturned = true;
    } catch (error) { first = error; row.error = error; }
    if (row.inputHandle) {
      row.inputCloseAttempted = true;
      try { await row.inputHandle.close(); row.inputCloseReturned = true; }
      catch (error) { row.inputCloseReturned = true; row.inputCloseError = error; first ??= error; }
    }
    if (row.handle) {
      row.closeAttempted = true;
      try { await row.handle.close(); row.closeReturned = true; }
      catch (error) { row.closeReturned = true; row.closeError = error; first ??= error; }
    }
    if (first) throw first;
    row.complete = true;
  }
  try {
  await directory(build);
  await directory(join(build, "assets"));
  const names = ["go.mod", "go.sum", "main.go", "ipc.go", "ipc_v3.go", "authoring_v3.go", "writer_command_v3.go", "plan_paths_v3.go", "input_windows.go", "input_linux.go", "input_darwin.go", "main_windows.go", "main_unix.go", "main_linux.go"];
  if (platform === "darwin") names.push("main_darwin_helper.go", "darwin_owner.go");
  // Sequential ownership preserves the exact failed frontier; later outputs
  // are never started after a failed read/write/sync/close.
  for (const name of names) await emit(join(build, name), join(source, name));
  await emit(join(build, "assets", "service-lassoctl.sea"), sea);
  await emit(join(build, "assets", "service-lasso-confined-scaffold"), writer);
  if (platform === "darwin") await emit(join(build, "assets", "service-lasso-darwin-immutable-helper.sha256"), null,
    Buffer.from(`${darwinHelperSha256}\n`));
  original.complete = true;
  return original;
  } catch (cause) {
    const error = new Error("Primary staging retained an incomplete original", { cause });
    error.primaryStageOriginal = original;
    throw error;
  }
}
