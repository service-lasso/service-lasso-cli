import { createHash, createHmac, timingSafeEqual } from "node:crypto";
import { CliError } from "./errors.js";
import { utf8, portablePath } from "./template-json.js";
import { TemplateAdmission, TEMPLATE_ASSET_NAMES } from "./template-admissions.js";
import { TemplateFile } from "./template-archive.js";

export const RECORD_CAP = 768000, FRAME_CAP = 16384, CHUNK_CAP = 16307;
export function protocolDenied(): never { throw new CliError("confined_writer_invalid_plan", "The native authoring channel rejected the operation."); }
export class Cursor {
  private offset = 0;
  constructor(private readonly bytes: Buffer) {}
  take(length: number): Buffer { if (!Number.isSafeInteger(length) || length < 0 || length > this.bytes.length - this.offset) protocolDenied(); const value = this.bytes.subarray(this.offset, this.offset + length); this.offset += length; return value; }
  u8(): number { return this.take(1)[0]; }
  u32(): number { return this.take(4).readUInt32BE(); }
  u64(): bigint { return this.take(8).readBigUInt64BE(); }
  string(maximum = 4096, empty = false): string { const length = this.u32(); if (length > maximum || (!empty && length === 0)) protocolDenied(); const value = utf8(this.take(length)); if (value.includes("\0")) protocolDenied(); return value; }
  end(): void { if (this.offset !== this.bytes.length) protocolDenied(); }
}
export const u8 = (value: number): Buffer => { if (!Number.isInteger(value) || value < 0 || value > 255) protocolDenied(); return Buffer.from([value]); };
export const u32 = (value: number): Buffer => { if (!Number.isSafeInteger(value) || value < 0 || value > 0xffffffff) protocolDenied(); const out = Buffer.alloc(4); out.writeUInt32BE(value); return out; };
export const u64 = (value: bigint): Buffer => { if (value < 0n || value > 0xffffffffffffffffn) protocolDenied(); const out = Buffer.alloc(8); out.writeBigUInt64BE(value); return out; };
export function S(value: string, cap = 4096, empty = false): Buffer {
  if (typeof value !== "string" || value.includes("\0") || (!empty && value.length === 0) || Buffer.byteLength(value) > cap) protocolDenied();
  const bytes = Buffer.from(value, "utf8"); if (utf8(bytes) !== value) protocolDenied(); return Buffer.concat([u32(bytes.length), bytes]);
}
export function hash(domain: string, ...parts: Buffer[]): Buffer { const value = createHash("sha256").update(domain, "ascii"); parts.forEach(part => value.update(part)); return value.digest(); }
export function frameMac(domain: string, direction: number, nonce: Buffer, capability: Buffer, sequence: number, body: Buffer): Buffer {
  if (nonce.length !== 32 || capability.length !== 32 || ![0, 1].includes(direction)) protocolDenied();
  return createHmac("sha256", capability).update(domain, "ascii").update(u8(direction)).update(nonce).update(u32(sequence)).update(u32(body.length)).update(body).digest();
}
export function authenticateFrame(raw: Buffer, domain: string, direction: number, nonce: Buffer, capability: Buffer, sequence: number, cap: number): Buffer {
  if (raw.length < 41) protocolDenied(); const length = raw.readUInt32BE(); if (length < 5 || length > cap || raw.length !== length + 36) protocolDenied();
  const body = raw.subarray(4, 4 + length), mac = raw.subarray(4 + length);
  if (body.readUInt32BE(1) !== sequence || !timingSafeEqual(mac, frameMac(domain, direction, nonce, capability, sequence, body))) protocolDenied(); return body;
}
export function encodeFrame(body: Buffer, domain: string, direction: number, nonce: Buffer, capability: Buffer, sequence: number, cap: number): Buffer {
  if (body.length < 5 || body.length > cap || body.readUInt32BE(1) !== sequence) protocolDenied();
  return Buffer.concat([u32(body.length), body, frameMac(domain, direction, nonce, capability, sequence, body)]);
}
export function catalogTupleDigest(entry: TemplateAdmission): Buffer {
  return hash("SLCLI-INSPECTION-CATALOG-1\0", S(entry.catalogIdentity, 256), ...entry.assets.flatMap((asset, i) => [u8(i), u32(asset.size), Buffer.from(asset.sha256, "hex")]));
}
export interface InspectionBinding { runIdentity: Buffer; launchNonce: Buffer; readReceipt: Buffer; catalogTupleDigest: Buffer; }
export function inspectionRecord(binding: InspectionBinding, files: readonly TemplateFile[], maximumTotalBytes: number): Buffer {
  if (files.length < 1 || files.length > 128 || [binding.runIdentity, binding.launchNonce, binding.readReceipt, binding.catalogTupleDigest].some(value => value.length !== 32)) protocolDenied();
  let previous = "", total = 0, length = 133;
  for (const file of files) { const path = portablePath(file.path); if (path <= previous || ![0o644, 0o755].includes(file.mode)) protocolDenied(); previous = path; total += file.bytes.length; length += 9 + Buffer.byteLength(path) + file.bytes.length; if (total > maximumTotalBytes || length > RECORD_CAP) protocolDenied(); }
  return Buffer.concat([u8(1), binding.runIdentity, binding.launchNonce, binding.readReceipt, binding.catalogTupleDigest, u32(files.length), ...files.flatMap(file => [S(file.path, 240), u8(file.mode === 0o644 ? 0 : 1), u32(file.bytes.length), file.bytes])], length);
}
export function decodeInspectionRecord(record: Buffer, binding: InspectionBinding, entry: TemplateAdmission): TemplateFile[] {
  if (record.length > RECORD_CAP) protocolDenied(); const cursor = new Cursor(record);
  if (cursor.u8() !== 1 || !cursor.take(32).equals(binding.runIdentity) || !cursor.take(32).equals(binding.launchNonce) || !cursor.take(32).equals(binding.readReceipt) || !cursor.take(32).equals(catalogTupleDigest(entry)) || !binding.catalogTupleDigest.equals(catalogTupleDigest(entry))) protocolDenied();
  const count = cursor.u32(); if (count !== entry.inventory.length + 2 || count > Math.min(128, entry.quotas.maximumFiles)) protocolDenied();
  const files: TemplateFile[] = []; let previous = "", total = 0;
  for (let i = 0; i < count; i++) { const path = portablePath(cursor.string(240)); const mode = cursor.u8(), length = cursor.u32(); if (path <= previous || mode > 1 || length > entry.quotas.maximumTotalBytes - total) protocolDenied(); previous = path; total += length; files.push({ path, mode: mode === 0 ? 0o644 : 0o755, bytes: Buffer.from(cursor.take(length)) }); }
  cursor.end();
  const members = new Map(entry.inventory.map(file => [file.path, file]));
  for (const file of files) {
    if (file.path === "template-contract.json") { if (file.mode !== 0o644 || file.bytes.length !== entry.assets[2].size || hash("", file.bytes).toString("hex") !== entry.contractSha256) protocolDenied(); }
    else if (file.path !== "template-provenance.json") { const member = members.get(file.path); if (!member || file.mode !== Number.parseInt(member.mode, 8) || file.bytes.length !== member.bytes || hash("", file.bytes).toString("hex") !== member.sha256) protocolDenied(); }
  }
  return files;
}
export function inspectionBodies(binding: InspectionBinding, record: Buffer, files: readonly TemplateFile[]): Buffer[] {
  if (record.length > RECORD_CAP) protocolDenied(); const digest = hash("SLCLI-INSPECTION-RECORD-1\0", record), total = files.reduce((sum, file) => sum + file.bytes.length, 0);
  const bodies = [Buffer.concat([u8(1), u32(0), binding.runIdentity, binding.launchNonce, binding.readReceipt, binding.catalogTupleDigest, digest, u32(record.length), u32(files.length), u32(total)])];
  for (let offset = 0, sequence = 1; offset < record.length; sequence++) { const length = Math.min(CHUNK_CAP, record.length - offset); bodies.push(Buffer.concat([u8(2), u32(sequence), binding.runIdentity, digest, u32(offset), u32(length), record.subarray(offset, offset + length)])); offset += length; }
  bodies.push(Buffer.concat([u8(3), u32(bodies.length), binding.runIdentity, digest, u32(record.length), u32(files.length), u32(total)])); return bodies;
}
export class InspectionReceiver {
  private sequence = 0; private offset = 0; private record?: Buffer; private announced?: { digest: Buffer; length: number; count: number; total: number }; private finished = false;
  constructor(private readonly binding: InspectionBinding, private readonly entry: TemplateAdmission, private readonly reserve: (bytes: number) => void) {}
  accept(body: Buffer): Buffer | undefined {
    if (this.finished || body.length > FRAME_CAP) protocolDenied(); const c = new Cursor(body), operation = c.u8(); if (c.u32() !== this.sequence) protocolDenied();
    if (operation === 1) {
      if (this.sequence !== 0 || this.record || !c.take(32).equals(this.binding.runIdentity) || !c.take(32).equals(this.binding.launchNonce) || !c.take(32).equals(this.binding.readReceipt) || !c.take(32).equals(this.binding.catalogTupleDigest)) protocolDenied();
      const digest = Buffer.from(c.take(32)), length = c.u32(), count = c.u32(), total = c.u32(); c.end();
      if (length < 133 || length > RECORD_CAP || count !== this.entry.inventory.length + 2 || count > this.entry.quotas.maximumFiles || total > this.entry.quotas.maximumTotalBytes) protocolDenied();
      this.reserve(length); this.record = Buffer.alloc(length); this.announced = { digest, length, count, total };
    } else if (operation === 2) {
      if (!this.record || !this.announced || this.sequence > 1024 || !c.take(32).equals(this.binding.runIdentity) || !c.take(32).equals(this.announced.digest)) protocolDenied();
      const offset = c.u32(), length = c.u32(); if (offset !== this.offset || length < 1 || length > CHUNK_CAP || length > this.record!.length - this.offset) protocolDenied();
      c.take(length).copy(this.record!, this.offset); c.end(); this.offset += length;
    } else if (operation === 3) {
      if (!this.record || !this.announced || this.offset !== this.record.length || !c.take(32).equals(this.binding.runIdentity) || !c.take(32).equals(this.announced.digest) || c.u32() !== this.announced.length || c.u32() !== this.announced.count || c.u32() !== this.announced.total) protocolDenied(); c.end();
      if (!hash("SLCLI-INSPECTION-RECORD-1\0", this.record!).equals(this.announced!.digest)) protocolDenied(); this.finished = true; return this.record;
    } else protocolDenied();
    this.sequence++; return undefined;
  }
  eof(): void { if (!this.finished) protocolDenied(); }
}
