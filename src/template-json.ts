import { CliError } from "./errors.js";
import { TextDecoder } from "node:util";

export const invalidTemplate = (): never => { throw new CliError("invalid_template_bundle", "The original template bytes do not satisfy the admitted owner contract."); };
export const utf8 = (bytes: Uint8Array): string => { try { return new TextDecoder("utf-8", { fatal: true, ignoreBOM: true }).decode(bytes); } catch { return invalidTemplate(); } };
export function closed(value: unknown, keys: readonly string[]): Record<string, unknown> {
  if (!value || typeof value !== "object" || Array.isArray(value) || Object.keys(value).length !== keys.length || Object.keys(value).some(key => !keys.includes(key))) return invalidTemplate();
  return value as Record<string, unknown>;
}
export function strictJson(bytes: Buffer, cap: number): unknown {
  if (bytes.length > cap) return invalidTemplate();
  const source = utf8(bytes); let offset = 0, tokens = 0, members = 0;
  const whitespace = () => { while (offset < source.length && /[ \t\r\n]/.test(source[offset])) offset++; };
  const tick = () => { if (++tokens > 65536) invalidTemplate(); };
  const string = (): string => {
    tick(); const start = offset++;
    while (offset < source.length) {
      const c = source.charCodeAt(offset++);
      if (c === 34) {
        let value: string;
        try { value = JSON.parse(source.slice(start, offset)); } catch { return invalidTemplate(); }
        // JSON escapes may contain a lone surrogate although the raw UTF8 was valid.
        for (let i = 0; i < value.length; i++) { const n = value.charCodeAt(i); if (n >= 0xd800 && n <= 0xdbff) { const next = value.charCodeAt(++i); if (!(next >= 0xdc00 && next <= 0xdfff)) invalidTemplate(); } else if (n >= 0xdc00 && n <= 0xdfff) invalidTemplate(); }
        if (Buffer.byteLength(value) > 32768) invalidTemplate();
        return value;
      }
      if (c < 32) invalidTemplate();
      if (c === 92) { const e = source[offset++]; if (e === "u") { if (!/^[a-fA-F0-9]{4}$/.test(source.slice(offset, offset + 4))) invalidTemplate(); offset += 4; } else if (!'"\\/bfnrt'.includes(e ?? "")) invalidTemplate(); }
    }
    return invalidTemplate();
  };
  const value = (depth: number): unknown => {
    whitespace(); if (depth > 16) invalidTemplate(); tick();
    if (source[offset] === '"') return string();
    if (source[offset] === "{") {
      offset++; whitespace(); const out: Record<string, unknown> = Object.create(null); const seen = new Set<string>();
      if (source[offset] === "}") { offset++; return out; }
      for (;;) {
        whitespace(); if (source[offset] !== '"') invalidTemplate(); const key = string();
        if (seen.has(key) || ++members > 4096) invalidTemplate(); seen.add(key);
        whitespace(); if (source[offset++] !== ":") invalidTemplate(); out[key] = value(depth + 1); whitespace();
        if (source[offset] === "}") { offset++; return out; } if (source[offset++] !== ",") invalidTemplate();
      }
    }
    if (source[offset] === "[") {
      offset++; whitespace(); const out: unknown[] = []; if (source[offset] === "]") { offset++; return out; }
      for (;;) { if (out.length >= 4096) invalidTemplate(); out.push(value(depth + 1)); whitespace(); if (source[offset] === "]") { offset++; return out; } if (source[offset++] !== ",") invalidTemplate(); }
    }
    const match = /^(?:true|false|null|-?(?:0|[1-9][0-9]*)(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?)/.exec(source.slice(offset));
    if (!match) return invalidTemplate(); offset += match[0].length;
    if (match[0] === "true") return true; if (match[0] === "false") return false; if (match[0] === "null") return null;
    const number = Number(match[0]); if (!Number.isFinite(number)) invalidTemplate(); return number;
  };
  const result = value(0); whitespace(); if (offset !== source.length) invalidTemplate(); return result;
}
export function sortedJson(value: unknown): string {
  if (Array.isArray(value)) return `[${value.map(sortedJson).join(",")}]`;
  if (value && typeof value === "object") return `{${Object.keys(value).sort().map(key => `${JSON.stringify(key)}:${sortedJson((value as Record<string, unknown>)[key])}`).join(",")}}`;
  return JSON.stringify(value);
}
export function canonicalBytes(value: unknown): Buffer { return Buffer.from(`${JSON.stringify(value, null, 2)}\n`, "utf8"); }
export function portablePath(value: unknown, depth = 12): string {
  if (typeof value !== "string" || Buffer.byteLength(value) > 240 || !/^[A-Za-z0-9_.@/-]+$/.test(value) || value.includes("\\") || value.startsWith("/") || value.split("/").length > depth || value.split("/").some(part => !part || part === "." || part === ".." || part.endsWith(".") || /^(?:con|prn|aux|nul|com[1-9]|lpt[1-9])(?:\.|$)/i.test(part))) return invalidTemplate();
  return value;
}
