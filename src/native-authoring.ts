import { createHmac, timingSafeEqual } from "node:crypto";
import { CliError } from "./errors.js";
import { Cursor, S, hash, u8, u32, protocolDenied } from "./native-authoring-wire.js";
import { TEMPLATE_ASSET_LIMITS, TemplateAdmission } from "./template-admissions.js";
import { TemplateFile } from "./template-archive.js";
import { VerifiedTemplate, validateOriginalAssets, verifyHeldPlan } from "./template-owner.js";

const failureCodes = Object.freeze(["", "input_unavailable", "input_unsafe", "input_quota", "input_changed", "invalid_phase", "child_unavailable", "invalid_plan", "unsafe_destination", "destination_exists", "writer_unavailable", "writer_failed", "readback_mismatch", "deadline_exceeded", "closure_unresolved", "protocol_mismatch", "session_used", "capacity_exhausted"]);
export function nativeAuthoringFailure(code: number, read = false): never {
  if (!Number.isInteger(code) || code < 1 || code > (read ? 6 : 17)) protocolDenied();
  throw new CliError(`confined_writer_${failureCodes[code]}`, "The native authoring operation could not complete.");
}
export interface OriginalGreetingIdentity { version: string; sourceCommit: string; helperSha256: string; }
// These are exact protocol comparisons inside an already admitted original
// native session. A greeting MAC or matching source text does not authenticate
// its process/image/provider/ROOT, and these functions never enroll a session.
// Actual N1 runtime ownership must cover every Buffer/string/crypto operation
// before the production native owner permits this source route to run.
export function compareOriginalGreeting(raw: Buffer, capability: Buffer, expected: OriginalGreetingIdentity): Buffer {
  if (capability.length !== 32 || raw.length < 4 + 98 + 32 || raw.length > 4 + 512 + 32 || !/^[0-9]+\.[0-9]+\.[0-9]+-dev\.[a-fA-F0-9]{7}$/.test(expected.version) || Buffer.byteLength(expected.version) > 64 || !/^[a-f0-9]{40}$/.test(expected.sourceCommit) || !/^[a-f0-9]{64}$/.test(expected.helperSha256)) protocolDenied();
  const length = raw.readUInt32BE(); if (length > 512 || raw.length !== length + 36) protocolDenied();
  const body = raw.subarray(4, 4 + length), supplied = raw.subarray(4 + length);
  const mac = createHmac("sha256", capability).update("SLCLI-GREETING-3\0", "ascii").update(body).digest();
  if (!timingSafeEqual(mac, supplied)) protocolDenied();
  const cursor = new Cursor(body);
  if (cursor.u8() !== 1 || cursor.u32() !== 0 || cursor.u32() !== 3) protocolDenied();
  const nonce = cursor.take(32); if (nonce.every(byte => byte === 0)) protocolDenied();
  const version = cursor.string(64), source = cursor.take(20), helper = cursor.take(32); cursor.end();
  if (version !== expected.version || source.toString("hex") !== expected.sourceCommit || helper.toString("hex") !== expected.helperSha256) protocolDenied();
  return nonce;
}
export function originalReadBody(inputRoot: string): Buffer { return Buffer.concat([u8(2), u32(0), S(inputRoot, 4096)]); }
export interface OriginalReadResult { receipt: Buffer; template: VerifiedTemplate; assets: readonly Buffer[]; }
export function compareOriginalReadResult(body: Buffer, nonce: Buffer, entry: TemplateAdmission): OriginalReadResult {
  if (body.length > 300000 || nonce.length !== 32) protocolDenied();
  const cursor = new Cursor(body); if (cursor.u8() !== 3 || cursor.u32() !== 0) protocolDenied();
  const status = cursor.u8(); if (status === 1) { const code = cursor.u8(); cursor.end(); return nativeAuthoringFailure(code, true); } if (status !== 0) protocolDenied();
  const receipt = cursor.take(32), assets: Buffer[] = [], preimage: Buffer[] = [];
  for (let index = 0; index < 4; index++) {
    const name = cursor.u8(), size = cursor.u32(); if (name !== index || size < 1 || size > TEMPLATE_ASSET_LIMITS[index] || size !== entry.assets[index]?.size) protocolDenied();
    const raw = cursor.take(size); assets.push(raw); preimage.push(u8(name), u32(size), raw);
  }
  cursor.end(); if (!receipt.equals(hash("SLCLI-READ-3\0", nonce, ...preimage))) protocolDenied();
  return { receipt, assets, template: validateOriginalAssets(assets, entry) };
}
export interface OriginalPlanBody { body: Buffer; planDigest: Buffer; readbackDigest: Buffer; }
export function originalPlanBody(nonce: Buffer, read: OriginalReadResult, destination: string, id: string, name: string | undefined, files: readonly TemplateFile[]): OriginalPlanBody {
  if (nonce.length !== 32 || read.receipt.length !== 32) protocolDenied(); verifyHeldPlan(read.template, files, id, name ?? read.template.baselineName);
  // Compute the exact encoded bound before serialization. The owning native
  // runtime independently reserves actual allocations; this arithmetic never
  // replaces its arena, image/runtime/stack or original-parent copy ledger.
  let bytes = 5 + 32 + 4 + Buffer.byteLength(destination) + 4 + Buffer.byteLength(id) + 1 + (name === undefined ? 0 : 4 + Buffer.byteLength(name)) + 4;
  for (const file of files) { bytes += 4 + Buffer.byteLength(file.path) + 1 + 4 + file.bytes.length; if (!Number.isSafeInteger(bytes) || bytes > 768000) protocolDenied(); }
  if (bytes > 768000) protocolDenied();
  const payload = Buffer.concat([S(destination, 4096), S(id, 63), u8(name === undefined ? 0 : 1), ...(name === undefined ? [] : [S(name, 120)]), u32(files.length), ...files.flatMap(file => [S(file.path, 240), u8(file.mode === 0o644 ? 0 : 1), u32(file.bytes.length), file.bytes])]);
  const body = Buffer.concat([u8(4), u32(1), read.receipt, payload]); if (body.length !== bytes) protocolDenied();
  const planDigest = hash("SLCLI-PLAN-3\0", nonce, read.receipt, payload);
  const readbackDigest = hash("SLCLI-READBACK-3\0", nonce, read.receipt, planDigest, u32(files.length), ...files.flatMap(file => [S(file.path, 240), u8(file.mode === 0o644 ? 0 : 1), u32(file.bytes.length), hash("", file.bytes)]));
  return { body, planDigest, readbackDigest };
}
export function originalCloseBody(): Buffer { return Buffer.concat([u8(6), u32(1)]); }
export function compareOriginalTerminal(body: Buffer, plan?: OriginalPlanBody): void {
  if (body.length > 256) protocolDenied(); const cursor = new Cursor(body);
  if (cursor.u8() !== (plan ? 5 : 7) || cursor.u32() !== 1) protocolDenied();
  const status = cursor.u8(); if (status === 1) { const code = cursor.u8(); cursor.end(); nativeAuthoringFailure(code); } if (status !== 0) protocolDenied();
  if (plan && (!cursor.take(32).equals(plan.planDigest) || !cursor.take(32).equals(plan.readbackDigest))) protocolDenied(); cursor.end();
  // This compares the existing native result body only. Actual original writer
  // natural exit/EOF/readback and selected resource closure remain obligations
  // of the owning native production route, not assertions supplied by this SEA.
}
