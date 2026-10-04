import { inflateRawSync } from "node:zlib";
import { TemplateAdmission, sha256 } from "./template-admissions.js";
import { invalidTemplate, portablePath, utf8 } from "./template-json.js";

export interface TemplateFile { path: string; bytes: Buffer; mode: number; }
function crc32(bytes: Buffer): number {
  let crc = 0xffffffff;
  for (const byte of bytes) { crc ^= byte; for (let bit = 0; bit < 8; bit++) crc = (crc >>> 1) ^ ((crc & 1) ? 0xedb88320 : 0); }
  return (crc ^ 0xffffffff) >>> 0;
}
function gzip(bytes: Buffer, maximumExpanded: number): Buffer {
  if (bytes.length < 18 || bytes[0] !== 31 || bytes[1] !== 139 || bytes[2] !== 8 || (bytes[3] & 0xe0)) return invalidTemplate();
  const flags = bytes[3]; let offset = 10;
  if (flags & 4) { if (offset + 2 > bytes.length - 8) invalidTemplate(); const length = bytes.readUInt16LE(offset); offset += 2; if (offset + length > bytes.length - 8) invalidTemplate(); offset += length; }
  for (const flag of [8, 16]) if (flags & flag) { const end = bytes.indexOf(0, offset); if (end < 0 || end >= bytes.length - 8 || end - offset > 4096) invalidTemplate(); offset = end + 1; }
  if (flags & 2) { if (offset + 2 > bytes.length - 8 || bytes.readUInt16LE(offset) !== (crc32(bytes.subarray(0, offset)) & 0xffff)) invalidTemplate(); offset += 2; }
  try {
    const value = inflateRawSync(bytes.subarray(offset), { maxOutputLength: maximumExpanded, info: true });
    const consumed = value.engine.bytesWritten;
    // Deflate consumption fixes the trailer of ONE member. Any concatenation/tail denies.
    const trailer = offset + consumed;
    if (!Number.isSafeInteger(consumed) || trailer + 8 !== bytes.length || bytes.readUInt32LE(trailer) !== crc32(value.buffer) || bytes.readUInt32LE(trailer + 4) !== (value.buffer.length >>> 0)) invalidTemplate();
    return value.buffer;
  } catch { return invalidTemplate(); }
}
function tarText(field: Buffer): string {
  const nul = field.indexOf(0); if (nul >= 0 && field.subarray(nul).some(byte => byte !== 0)) invalidTemplate();
  const data = field.subarray(0, nul < 0 ? field.length : nul);
  if (data.some(byte => byte > 127)) invalidTemplate(); return utf8(data);
}
function octal(field: Buffer): number {
  const value = field.toString("ascii"); if (!/^[0-7]+[\x00 ]*$/.test(value)) return invalidTemplate();
  const result = Number.parseInt(value, 8); if (!Number.isSafeInteger(result) || result < 0) invalidTemplate(); return result;
}
export function decodeOriginalArchive(bytes: Buffer, policyBytes: Buffer, entry: TemplateAdmission): TemplateFile[] {
  if (bytes.length !== entry.assets[0].size || sha256(bytes) !== entry.archiveSha256 || bytes.length > entry.quotas.maximumArchiveBytes) return invalidTemplate();
  const tar = gzip(bytes, Math.min(524288, entry.quotas.maximumArchiveExpandedBytes));
  if (tar.length % 512 || tar.length > entry.quotas.maximumArchiveExpandedBytes) invalidTemplate();
  const expected = new Map(entry.inventory.map(file => [file.path, file]));
  if (expected.has("template-contract.json") || expected.has("template-provenance.json")) invalidTemplate();
  const directories = new Set<string>();
  for (const path of [...expected.keys(), "template-contract.json"]) { const parts = path.split("/"); for (let i = 1; i < parts.length; i++) directories.add(parts.slice(0, i).join("/")); }
  const seen = new Set<string>(), seenDirectories = new Set<string>(); const files: TemplateFile[] = [];
  let offset = 0, records = 0, contentBytes = 0, ended = false;
  while (offset + 512 <= tar.length) {
    const header = tar.subarray(offset, offset + 512);
    if (header.every(byte => byte === 0)) {
      if (offset + 1024 > tar.length || tar.subarray(offset + 512, offset + 1024).some(byte => byte !== 0) || tar.subarray(offset + 1024).some(byte => byte !== 0)) invalidTemplate();
      ended = true; break;
    }
    if (++records > Math.min(128, entry.quotas.maximumArchiveEntries)) invalidTemplate();
    const declaredChecksum = octal(header.subarray(148, 156)); let observedChecksum = 0;
    for (let i = 0; i < 512; i++) observedChecksum += i >= 148 && i < 156 ? 32 : header[i];
    if (declaredChecksum !== observedChecksum || tarText(header.subarray(257, 263)) !== "ustar" || tarText(header.subarray(157, 257)) !== "") invalidTemplate();
    const prefix = tarText(header.subarray(345, 500)); const name = `${prefix ? `${prefix}/` : ""}${tarText(header.subarray(0, 100))}`;
    const size = octal(header.subarray(124, 136)), mode = octal(header.subarray(100, 108)), type = header[156];
    const start = offset + 512, end = start + size, next = start + Math.ceil(size / 512) * 512;
    if (!Number.isSafeInteger(next) || end > tar.length || next > tar.length || tar.subarray(end, next).some(byte => byte !== 0)) invalidTemplate();
    if (type === 103) {
      if (offset !== 0 || name !== "pax_global_header" || mode !== 0o666 || size !== 52 || !tar.subarray(start, end).equals(Buffer.from(`52 comment=${entry.commit}\n`, "ascii"))) invalidTemplate();
    } else if (type === 53) {
      if (!name.endsWith("/") || mode !== 0o775 || size !== 0) invalidTemplate();
      const directory = portablePath(name.slice(0, -1), entry.quotas.maximumArchivePathDepth);
      if (!directories.has(directory) || seenDirectories.has(directory)) invalidTemplate(); seenDirectories.add(directory);
    } else if (type === 0 || type === 48) {
      const path = portablePath(name, entry.quotas.maximumArchivePathDepth);
      if (seen.has(path)) invalidTemplate(); seen.add(path);
      const content = tar.subarray(start, end);
      if (path === "template-contract.json") {
        if (mode !== 0o664 || !content.equals(policyBytes)) invalidTemplate();
        files.push({ path, mode: 0o644, bytes: Buffer.from(content) });
      } else {
        const member = expected.get(path); if (!member || size !== member.bytes || sha256(content) !== member.sha256 || mode !== (member.mode === "0644" ? 0o664 : 0o775)) invalidTemplate();
        files.push({ path, mode: member.mode === "0644" ? 0o644 : 0o755, bytes: Buffer.from(content) });
      }
      contentBytes += size; if (contentBytes > entry.quotas.maximumTotalBytes) invalidTemplate();
    } else invalidTemplate();
    offset = next;
  }
  if (!ended || seen.size !== expected.size + 1 || [...expected.keys()].some(path => !seen.has(path)) || seenDirectories.size !== directories.size || [...directories].some(path => !seenDirectories.has(path))) invalidTemplate();
  return files.sort((a, b) => a.path < b.path ? -1 : a.path > b.path ? 1 : 0);
}
