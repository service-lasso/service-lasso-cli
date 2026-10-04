import { spawn, ChildProcessWithoutNullStreams } from "node:child_process";
import { randomBytes, timingSafeEqual } from "node:crypto";
import { Buffer } from "node:buffer";
import { resolve } from "node:path";
import { CliError } from "./errors.js";
import { TemplateAdmission } from "./template-admissions.js";
import { Cursor, S, u8, u32, frameMac, protocolDenied } from "./native-authoring-wire.js";

interface InstalledTransport {
  schema: "service-lasso.cli-library-transport.v3";
  platform: "win32" | "linux";
  facade: string;
  facadeImageSha256: string;
  facadeSourceSha256: string;
  nodeImageSha256: string;
  packageSourceSha256: string;
  maximumBytes: 8388608;
  maximumCopySets: 128;
  allocationOverhead: { objectBytes: number; stringByteFactor: number; bufferBytes: number; slabBytes: number };
}
declare const __SERVICE_LASSO_INSTALLED_LIBRARY_TRANSPORTS__: readonly InstalledTransport[] | undefined;
const transports = typeof __SERVICE_LASSO_INSTALLED_LIBRARY_TRANSPORTS__ === "undefined" ? [] : __SERVICE_LASSO_INSTALLED_LIBRARY_TRANSPORTS__;
function unavailable(): never { throw new CliError("template_identity_unavailable", "The independently admitted native authoring transport is unavailable."); }
function sessionUnavailable(): never { throw new CliError("template_session_unavailable", "The original template authoring session is unavailable."); }
function installedTransport(): InstalledTransport {
  const matches = transports.filter(binding => binding.platform === process.platform);
  if (matches.length !== 1) return unavailable(); const binding = matches[0];
  if (binding.schema !== "service-lasso.cli-library-transport.v3" || !binding.facade || resolve(binding.facade) !== binding.facade || [binding.facadeImageSha256, binding.facadeSourceSha256, binding.nodeImageSha256, binding.packageSourceSha256].some(value => !/^[a-f0-9]{64}$/.test(value)) || binding.maximumBytes !== 8388608 || binding.maximumCopySets !== 128 || Object.keys(binding.allocationOverhead).sort().join(",") !== "bufferBytes,objectBytes,slabBytes,stringByteFactor" || Object.values(binding.allocationOverhead).some(value => !Number.isSafeInteger(value) || value < 1 || value > 65536)) return unavailable();
  return binding;
}
export interface CopyRecord { sequence: number; bytes: number; cumulativeBytes: number; paths: readonly { path: string; size: number; sha256: string; mode: number }[]; }
export class ClientAllocation {
  private cumulative = 0; private sets = 0; private failed = false;
  readonly records: CopyRecord[] = [];
  constructor(private readonly binding: InstalledTransport, private readonly cancel: () => void) {}
  reserve(bytes: number): void {
    if (this.failed || !Number.isSafeInteger(bytes) || bytes < 0 || bytes > this.binding.maximumBytes - this.cumulative) { this.failed = true; this.cancel(); sessionUnavailable(); }
    this.cumulative += bytes;
  }
  objectCharge(bytes: number, objects = 1): number {
    const overhead = this.binding.allocationOverhead;
    const result = bytes + objects * overhead.objectBytes + overhead.bufferBytes + overhead.slabBytes;
    if (!Number.isSafeInteger(result)) sessionUnavailable(); return result;
  }
  copySet(files: readonly { path: string; bytes: Buffer; mode: number }[], hashes: readonly string[]): Array<{ path: string; bytes: Buffer; mode: number }> {
    if (this.failed || this.sets >= this.binding.maximumCopySets || files.length !== hashes.length) { this.cancel(); this.failed = true; return sessionUnavailable(); }
    const overhead = this.binding.allocationOverhead;
    const charge = files.reduce((sum, file) => sum + this.objectCharge(file.bytes.length + Buffer.byteLength(file.path) * overhead.stringByteFactor, 3), this.objectCharge(files.length * 8, 2));
    this.reserve(charge); // One all-or-deny reservation BEFORE the first Buffer.
    const set = files.map(file => ({ path: file.path, mode: file.mode, bytes: Buffer.from(file.bytes) }));
    const paths = files.map((file, i) => ({ path: file.path, size: file.bytes.length, mode: file.mode, sha256: hashes[i] }));
    this.records.push({ sequence: ++this.sets, bytes: charge, cumulativeBytes: this.cumulative, paths });
    return set;
  }
  // Returned Buffers never release charge here. Only the service's observation
  // of the actual original Node-parent exit can retire this client binding.
  get bytes(): number { return this.cumulative; }
}
export class LibraryChannel {
  readonly allocation: ClientAllocation;
  private readonly nonce: Buffer; private readonly capability: Buffer;
  private readonly child: ChildProcessWithoutNullStreams;
  private sequence = 0; private sendSequence = 0; private pending = Buffer.alloc(0);
  private waiter?: { cap: number; operations: readonly number[]; resolve: (body: Buffer) => void; reject: (error: unknown) => void };
  private closed = false; private ending = false; private timer: NodeJS.Timeout;
  readonly deadline: number;
  private constructor(private readonly binding: InstalledTransport) {
    this.deadline = performance.now() + 40000;
    this.nonce = randomBytes(32); this.capability = randomBytes(32);
    this.allocation = new ClientAllocation(binding, () => this.cancel());
    // This fixed installed facade authenticates the actual Node parent/package
    // and the original service image natively. No PATH, compiler or env pin.
    this.child = spawn(binding.facade, ["--library-v3"], { stdio: ["pipe", "pipe", "pipe"], windowsHide: true, env: process.env });
    this.child.stdout.on("data", (bytes: Buffer) => this.receive(bytes));
    this.child.stderr.on("data", () => this.fail()); // Private transport has no diagnostic stream.
    this.child.once("error", () => this.fail()); this.child.once("exit", () => { if (!this.ending) this.fail(); });
    this.child.stdout.once("end", () => { if (!this.ending || this.pending.length) this.fail(); });
    this.timer = setTimeout(() => this.fail(), 40000);
    this.allocation.reserve(this.allocation.objectCharge(256, 16));
    // The inherited private bootstrap provides no caller image/hash authority:
    // facade independently acquires the original parent/native source objects.
    this.child.stdin.write(Buffer.concat([Buffer.from("SLCLI-LIBRARY-3\0", "ascii"), this.nonce, this.capability]));
  }
  static open(): LibraryChannel { return new LibraryChannel(installedTransport()); }
  private fail(): void {
    if (this.closed) return; this.closed = true; clearTimeout(this.timer); this.child.stdin.destroy(); this.child.stdout.destroy();
    const waiter = this.waiter; this.waiter = undefined; waiter?.reject(new CliError("template_session_unavailable", "The original template authoring session is unavailable."));
    // No kill/PID cleanup or process-exit assertion. Native service retains all
    // unresolved objects, parent/copy/control reservations and original evidence.
  }
  cancel(): void { this.fail(); }
  private receive(bytes: Buffer): void {
    if (this.closed || performance.now() >= this.deadline || bytes.length > 16420 || this.pending.length + bytes.length > 32840) { this.fail(); return; }
    this.allocation.reserve(this.allocation.objectCharge(this.pending.length + bytes.length));
    this.pending = Buffer.concat([this.pending, bytes]);
    this.drain();
  }
  private drain(): void {
    if (!this.waiter || this.pending.length < 4) return;
    const length = this.pending.readUInt32BE();
    if (length < 5 || length > this.waiter.cap || length > 16384) { this.fail(); return; }
    if (this.pending.length < length + 36) return;
    const body = this.pending.subarray(4, 4 + length), mac = this.pending.subarray(4 + length, length + 36);
    if (!this.waiter.operations.includes(body[0]) || body.readUInt32BE(1) !== this.sequence || !timingSafeEqual(mac, frameMac("SLCLI-INSPECTION-LIBRARY-1\0", 0, this.nonce, this.capability, this.sequence, body))) { this.fail(); return; }
    this.sequence++; const waiter = this.waiter; this.waiter = undefined;
    this.allocation.reserve(this.allocation.objectCharge(body.length)); const copied = Buffer.from(body);
    this.pending = this.pending.subarray(length + 36); waiter.resolve(copied);
  }
  read(operations: readonly number[], cap = 16384): Promise<Buffer> {
    if (this.closed || this.waiter || performance.now() >= this.deadline) return Promise.reject(new CliError("template_session_unavailable", "The original template authoring session is unavailable."));
    return new Promise((resolve, reject) => { this.waiter = { operations, cap, resolve, reject }; this.drain(); });
  }
  send(operation: number, payload: Buffer): void {
    if (this.closed || performance.now() >= this.deadline || payload.length > 16379) sessionUnavailable();
    this.allocation.reserve(this.allocation.objectCharge(payload.length + 41));
    const sequence = this.sendSequence++, body = Buffer.concat([u8(operation), u32(sequence), payload]);
    const mac = frameMac("SLCLI-INSPECTION-LIBRARY-1\0", 1, this.nonce, this.capability, sequence, body);
    this.child.stdin.write(Buffer.concat([u32(body.length), body, mac]));
  }
  launch(root: string, entry: TemplateAdmission): void { this.send(1, Buffer.concat([u8(1), randomBytes(32), S(root), S(entry.catalogIdentity, 256), S("", 4096, true), u8(0), u8(0)])); }
  action(run: Buffer, token: Buffer, action: 0 | 1 | 2, destination: string, id?: string, name?: string): void {
    this.send(3, Buffer.concat([run, token, u8(action), S(destination, 4096, true), u8(id === undefined ? 0 : 1), ...(id === undefined ? [] : [S(id, 63)]), u8(name === undefined ? 0 : 1), ...(name === undefined ? [] : [S(name, 120)])]));
  }
  available(): boolean { return !this.closed && performance.now() < this.deadline; }
  async finish(): Promise<void> {
    if (!this.available()) sessionUnavailable(); this.ending = true; this.child.stdin.end();
    await new Promise<void>((resolve, reject) => {
      let exit = false, stdoutEOF = this.child.stdout.readableEnded, stderrEOF = this.child.stderr.readableEnded;
      const done = () => { if (exit && stdoutEOF && stderrEOF && this.pending.length === 0) { clearTimeout(this.timer); this.closed = true; resolve(); } };
      this.child.stdout.once("end", () => { stdoutEOF = true; done(); }); this.child.stderr.once("end", () => { stderrEOF = true; done(); });
      this.child.once("close", code => { if (code !== 0) { this.fail(); reject(new CliError("template_session_unavailable", "The original template authoring session is unavailable.")); } else { exit = true; done(); } });
      this.child.once("error", error => { this.fail(); reject(error); }); done();
    });
  }
}
