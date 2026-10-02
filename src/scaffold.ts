import { spawn, spawnSync } from "node:child_process";
import { mkdtempSync, readFileSync } from "node:fs";
import { createHash, createHmac, timingSafeEqual } from "node:crypto";
import { tmpdir } from "node:os";
import { join, relative, resolve, sep } from "node:path";
import { fileURLToPath } from "node:url";
import { Socket, connect } from "node:net";
import { CliError } from "./errors.js";
import { AcceptedTemplateBundle, acceptedTemplateFiles, loadAcceptedTemplateBundle, requireAcceptedTemplateIdentity, templateContractPreview } from "./template.js";
export interface ServiceScaffoldOptions { id: string; directory: string; name?: string; dryRun?: boolean; templateRoot?: string; }
export interface ServiceScaffoldResult { directory: string; files: string[]; dryRun: boolean; }
export function validateServiceId(id: string): string { if (!/^[a-z][a-z0-9-]{1,63}$/.test(id)) throw new CliError("invalid_service_id", "Service id must use lowercase letters, numbers, and hyphens (2-64 characters)."); return id; }
export function previewServiceScaffold(options: ServiceScaffoldOptions): Record<string, unknown> { return { directory: resolve(options.directory), serviceId: validateServiceId(options.id), dryRun: true, files: [], writes: false, runtimeMutation: false, template: templateContractPreview() }; }
export function scaffoldFiles(options: ServiceScaffoldOptions): Record<string, string> { validateServiceId(options.id); return requireAcceptedTemplateIdentity(); }
const helperName = `service-lasso-confined-scaffold${process.platform === "win32" ? ".exe" : ""}`;
let sourceHelper: string | undefined;
function sha256(value: Buffer | string): string { return createHash("sha256").update(value).digest("hex"); }
declare const __SERVICE_LASSO_CANDIDATE_VERSION__: string | undefined;
declare const __SERVICE_LASSO_CANDIDATE_SOURCE_SHA__: string | undefined;
declare const __SERVICE_LASSO_CONFINED_HELPER_SHA256__: string | undefined;
function candidateIdentity(): { version: string; sourceSha: string; helperSha256: string } | undefined {
  // These globals are replaced while the SEA is built.  `typeof` keeps normal
  // source execution independent of ambient packaging-looking environment.
  const version = typeof __SERVICE_LASSO_CANDIDATE_VERSION__ === "string" ? __SERVICE_LASSO_CANDIDATE_VERSION__ : undefined;
  const sourceSha = typeof __SERVICE_LASSO_CANDIDATE_SOURCE_SHA__ === "string" ? __SERVICE_LASSO_CANDIDATE_SOURCE_SHA__ : undefined;
  const helperSha256 = typeof __SERVICE_LASSO_CONFINED_HELPER_SHA256__ === "string" ? __SERVICE_LASSO_CONFINED_HELPER_SHA256__ : undefined;
  return /^\d+\.\d+\.\d+-dev\.[0-9a-f]{7}$/i.test(version ?? "") && /^[0-9a-f]{40}$/i.test(sourceSha ?? "") && /^[0-9a-f]{64}$/i.test(helperSha256 ?? "") ? { version: version!, sourceSha: sourceSha!, helperSha256: helperSha256! } : undefined;
}
function packagedPrimaryGate(): boolean {
  // A candidate SEA may inspect the adjacent helper only to reject a broken
  // package. It must never turn verified bytes into a temporary pathname and
  // ask Node to spawn that pathname: spawn re-resolves the name after the
  // check. The compiled primary launch gate owns candidate materialization and
  // retains the verified helper identity through process creation over its
  // inherited private IPC channel. This source SEA deliberately fails closed
  // until that gate protocol is present; source execution below remains a
  // developer-only writer path and is never candidate evidence.
  const identity = candidateIdentity();
  if (!identity) return false;
  // The compiled primary, not this SEA, owns the embedded helper bytes. The
  // inherited endpoint is capability-style authority: a neighbouring path,
  // provenance sidecar, or caller environment cannot manufacture it.
  // The gate passes a fixed inherited descriptor. An endpoint name in the
  // environment is routing data controlled by the process launcher, not a
  // capability: never use one as a fallback.
  return true;
}
function helperPath(): string {
  if (candidateIdentity()) throw new CliError("unsafe_scaffold_destination", "The confined writer is available only through the native primary gate.");
  // Source execution builds the checked-in helper only for local developer and
  // test use.  It is never native-candidate qualification evidence.  The
  // directory is deliberately retained: Node cannot prove a recursively named
  // cleanup target still belongs to this process after an attacker replaces it.
  const helperSource = fileURLToPath(new URL("../native/confined-scaffold", import.meta.url));
  if (sourceHelper) return sourceHelper;
  // Never compile into the repository: an inherited helper binary can be a
  // retained audit artifact.  This output directory is intentionally retained:
  // a pathname-based recursive cleanup cannot prove that a replaced target is
  // still ours.
  const helperDirectory = mkdtempSync(join(tmpdir(), "service-lasso-confined-helper-"));
  const helperBinary = join(helperDirectory, helperName);
  const build = spawnSync("go", ["build", "-trimpath", "-o", helperBinary, "."], { cwd: helperSource, encoding: "utf8" });
  if (build.status !== 0 || build.error) throw new CliError("unsafe_scaffold_destination", "The confined writer is unavailable.");
  sourceHelper = helperBinary;
  return sourceHelper;
}
function gateSocket(): Socket | undefined {
  // fd 3 is populated only by the compiled primary on Unix. On Windows the
  // primary supplies a named pipe, but the SEA accepts it only after the
  // signed gate greeting below; its environment spelling alone is never
  // authority.
  if (process.platform !== "win32") {
    try { return new Socket({ fd: 3, readable: true, writable: true }); } catch { return undefined; }
  }
  const pipe = process.env.SERVICE_LASSO_PRIMARY_GATE_PIPE;
  return pipe && /^\\\\\.\\pipe\\service-lasso-primary-[0-9a-f-]+$/i.test(pipe) ? connect(pipe) : undefined;
}
function gateCapability(): Buffer | undefined {
  const encoded = process.env.SERVICE_LASSO_PRIMARY_GATE_CAPABILITY;
  if (typeof encoded !== "string" || !/^[A-Za-z0-9+/]+={0,2}$/.test(encoded)) return undefined;
  try { const value = Buffer.from(encoded, "base64"); return value.length === 32 ? value : undefined; } catch { return undefined; }
}
function gateProof(capability: Buffer, label: string, nonce: string): string { return createHmac("sha256", capability).update(`service-lasso-primary-v2:${label}:${nonce}`).digest("base64"); }
function verifyGateGreeting(value: unknown, capability: Buffer): string | undefined {
  if (!value || typeof value !== "object") return undefined;
  const record = value as { version?: unknown; nonce?: unknown; proof?: unknown };
  if (record.version !== 1 || typeof record.nonce !== "string" || !/^[0-9a-f]{64}$/i.test(record.nonce) || typeof record.proof !== "string") return undefined;
  try { const supplied = Buffer.from(record.proof, "base64"), expected = Buffer.from(gateProof(capability, "gate", record.nonce), "base64"); return supplied.length === expected.length && timingSafeEqual(supplied, expected) ? record.nonce : undefined; } catch { return undefined; }
}
async function primaryMaterialize(input: string): Promise<{ stdout: string; stderr: string; code: number } | undefined> {
  if (!candidateIdentity() || !packagedPrimaryGate()) return undefined;
  const capability = gateCapability();
  if (!capability) return undefined;
  const socket = gateSocket();
  if (!socket) return undefined;
  return await new Promise((done) => {
    let response = "";
    let nonce: string | undefined;
    const close = () => { socket.destroy(); done(undefined); };
    socket.setEncoding("utf8");
    socket.once("error", close);
    socket.once("close", close);
    // An inherited Unix socketpair endpoint is already connected. Waiting for
    // a future connect event would leave the one-shot capability idle; named
    // pipes still use their ordinary asynchronous connection event.
    const writeRequest = () => { /* the signed greeting is always first */ };
    if (process.platform !== "win32") queueMicrotask(writeRequest);
    else socket.once("connect", writeRequest);
    socket.on("data", (chunk: string) => {
      response += chunk;
      const end = response.indexOf("\n");
      if (end < 0) return;
      try {
        const value: unknown = JSON.parse(response.slice(0, end));
        response = response.slice(end + 1);
        if (!nonce) {
          nonce = verifyGateGreeting(value, capability);
          if (!nonce) return close();
          socket.write(`${JSON.stringify({ version: 1, nonce, proof: gateProof(capability, "sea", nonce), input: Buffer.from(input).toString("base64") })}\n`);
          return;
        }
        socket.removeListener("error", close); socket.removeListener("close", close); socket.end();
        if (!value || typeof value !== "object") return done(undefined);
        const record = value as { version?: unknown; nonce?: unknown; code?: unknown; stdout?: unknown; stderr?: unknown };
        if (record.version !== 1 || record.nonce !== nonce || !Number.isInteger(record.code) || typeof record.stdout !== "string" || typeof record.stderr !== "string") return done(undefined);
        done({ code: record.code as number, stdout: Buffer.from(record.stdout, "base64").toString(), stderr: Buffer.from(record.stderr, "base64").toString() });
      } catch { done(undefined); }
    });
  });
}
async function confinedMaterialize(destination: string, files: Awaited<ReturnType<typeof acceptedTemplateFiles>>): Promise<void> {
  const input = `${Buffer.from(destination).toString("base64")}\n${files.length}\n${files.map(file => `${file.path}\t${file.mode.toString(8)}\t${file.bytes.toString("base64")}`).join("\n")}\n`;
  const expectedReceipt = sha256(input);
  const primary = await primaryMaterialize(input);
  const outcome = primary ?? await new Promise<{ stdout: string; stderr: string; code: number | null }>((done, fail) => { const child = spawn(helperPath(), [], { stdio: ["pipe", "pipe", "pipe"], windowsHide: true }); let stdout = "", stderr = ""; child.stdout.on("data", value => { stdout += value; }); child.stderr.on("data", value => { stderr += value; }); child.once("error", fail); child.once("close", code => done({ stdout, stderr, code })); child.stdin.end(input); });
  const result = outcome.stdout.trim();
  if (outcome.code === 0 && outcome.stderr === "" && result === `ok\t${expectedReceipt}`) return;
  // The helper only emits these fixed, path-free failure codes.  Keep the
  // public CLI result stable while rejecting malformed helper output.
  const match = outcome.code === 1 && outcome.stderr === "confined writer failed\n" && /^error\t(destination_exists|parent_missing|permission_denied|write_rejected)$/.exec(result);
  if (!match) throw new CliError("unsafe_scaffold_destination", "The project destination could not be created through the confined writer.");
  throw new CliError(`confined_writer_${match[1]}`, "The project destination could not be created through the confined writer.");
}
export async function createServiceScaffold(options: ServiceScaffoldOptions): Promise<ServiceScaffoldResult> { validateServiceId(options.id); const destination = resolve(options.directory); if (!options.templateRoot) return requireAcceptedTemplateIdentity(); const bundle = await loadAcceptedTemplateBundle(options.templateRoot); return materializeAcceptedTemplate(destination, bundle, Boolean(options.dryRun)); }
/** Materialize only a bundle already admitted by the source-owned resolver. */
export async function materializeAcceptedTemplate(destinationInput: string, bundle: AcceptedTemplateBundle, dryRun = false): Promise<ServiceScaffoldResult> {
  const destination = resolve(destinationInput), files = await acceptedTemplateFiles(bundle);
  if (dryRun) return { directory: destination, files: files.map(file => file.path), dryRun: true };
  if (files.some(file => !resolve(destination, file.path).startsWith(destination + sep) || relative(destination, resolve(destination, file.path)).startsWith(".."))) throw new CliError("invalid_template_bundle", "Template inventory contains an unsafe path.");
  try { await confinedMaterialize(destination, files); } catch (error) { if (error instanceof CliError && error.code.startsWith("confined_writer_")) throw error; throw new CliError("unsafe_scaffold_destination", "The project destination could not be created through the confined writer."); }
  return { directory: destination, files: files.map(file => file.path), dryRun: false };
}
