import { copyFile, mkdir, writeFile } from "node:fs/promises";
import { join } from "node:path";

// This is the actual package-native build inventory, shared with its staged
// producer regression. Test-only source files never enter the candidate build.
export async function stagePrimaryGate({ source, build, sea, writer, platform, darwinHelperSha256 }) {
  await mkdir(join(build, "assets"), { recursive: true });
  const names = ["go.mod", "go.sum", "main.go", "ipc.go", "main_windows.go", "main_unix.go", "main_linux.go"];
  if (platform === "darwin") names.push("main_darwin_helper.go", "darwin_owner.go");
  await Promise.all([
    ...names.map((name) => copyFile(join(source, name), join(build, name))),
    copyFile(sea, join(build, "assets", "service-lassoctl.sea")),
    copyFile(writer, join(build, "assets", "service-lasso-confined-scaffold")),
    ...(platform === "darwin" ? [writeFile(join(build, "assets", "service-lasso-darwin-immutable-helper.sha256"), `${darwinHelperSha256}\n`)] : []),
  ]);
}
