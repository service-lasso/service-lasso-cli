import { createReadStream } from "node:fs";

// Source-only liveness fixture. It admits no template/provider/profile and emits
// no native qualification receipt. FD3 is this test owner's original release
// channel; stdin EOF deliberately does not retire the genuinely live child.
process.stdin.resume();
const hold = setInterval(() => {}, 1000);
const release = createReadStream("", { fd: 3, autoClose: true });
let command = "";
release.on("data", bytes => {
  command += bytes.toString("utf8");
  if (command === "release\n") { clearInterval(hold); release.destroy(); }
  else if (command.length > 32) { process.exitCode = 1; clearInterval(hold); release.destroy(); }
});
release.on("error", () => { process.exitCode = 1; clearInterval(hold); release.destroy(); });
process.stdout.write("live\n");
