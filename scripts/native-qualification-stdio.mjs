import { fstatSync } from "node:fs";

// Transport only. Environment descriptor numbers never constitute authority:
// the Darwin primary authenticates the inherited owner socket's root peer.
export function nativeQualificationStdio(base, environment = process.env) {
  if (process.platform !== "darwin") return base;
  const names = ["SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD", "SERVICE_LASSO_DARWIN_OWNER_CHANNEL_FD"];
  if (names.every(name => environment[name] === undefined)) return base;
  const descriptors = names.map(name => {
    if (!/^(?:[3-9]|[1-5][0-9]|6[0-4])$/.test(environment[name] ?? "")) throw new Error("Darwin owner descriptor transport is unavailable.");
    const fd = Number(environment[name]);
    fstatSync(fd); // A textual value without an actual open FD fails closed.
    return fd;
  });
  if (descriptors[0] === descriptors[1] || !fstatSync(descriptors[1]).isSocket()) throw new Error("Darwin owner descriptor transport is unavailable.");
  const stdio = [...base];
  while (stdio.length <= Math.max(...descriptors)) stdio.push("ignore");
  for (const fd of descriptors) stdio[fd] = fd;
  return stdio;
}
