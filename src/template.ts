import { createHash } from "node:crypto";
import { lstat, readFile } from "node:fs/promises";
import { relative, resolve, sep } from "node:path";
import { CliError } from "./errors.js";

export const TEMPLATE_CONTRACT_GATE = Object.freeze({
  repository: "service-lasso/service-template", sourceCommit: "bdeb24b84f97e702372ccbcba0794ce30888ad53", templateVersion: "1.0.0-dev",
  contractDigest: "05162aa2966c3656d5809050398ebaeba354ee6639f51b047464bc6a383324cb", contractSha256: "95e022bf381deb1096f42a0699c4f8f0cfed996670b9d6f0655792d6e41c0f0e",
  status: "blocked" as const, blocker: "The current service-template develop contract is a development candidate source, not an accepted immutable template artifact.",
  requiredArtifact: ["immutable tag", "published archive SHA-256", "published template-contract SHA-256", "candidate provenance", "accepted catalog identity"],
});

export interface TemplateInventoryEntry { path: string; sha256: string; mode: string; bytes: number; }
export interface AcceptedTemplateBundle { repository: string; tag: string; commit: string; templateVersion: string; contractDigest: string; contractSha256: string; archiveSha256: string; catalogIdentity: string; inventory: TemplateInventoryEntry[]; root: string; }
function fail(message: string): never { throw new CliError("invalid_template_bundle", message); }
function sha256(bytes: Buffer): string { return createHash("sha256").update(bytes).digest("hex"); }
function safeRelative(path: string): string {
  if (path.includes("\\") || path.startsWith("/") || path.split("/").some((part) => !part || part === "." || part === "..")) fail("Template inventory contains an unsafe path.");
  return path;
}
function isSha(value: unknown, length: number): value is string { return typeof value === "string" && new RegExp(`^[a-f0-9]{${length}}$`).test(value); }

export function templateContractPreview(): Record<string, unknown> { return TEMPLATE_CONTRACT_GATE; }
export function requireAcceptedTemplateIdentity(): never { throw new CliError("template_identity_unavailable", "No accepted immutable service-template identity is available; project creation is blocked until the template owner publishes and accepts the checksum-bound candidate."); }

/** Verify a caller-supplied, already acquired accepted tuple without network I/O. */
export async function loadAcceptedTemplateBundle(templateRoot: string): Promise<AcceptedTemplateBundle> {
  const root = resolve(templateRoot);
  const [contractBytes, candidateBytes, provenanceBytes, archiveBytes] = await Promise.all([
    readFile(resolve(root, "template-contract.json")), readFile(resolve(root, "template-candidate.json")), readFile(resolve(root, "template-provenance.json")), readFile(resolve(root, "service-template.tar.gz")),
  ]).catch(() => fail("Accepted template bundle is incomplete."));
  let contract: any; let candidate: any; let provenance: any;
  try { contract = JSON.parse(contractBytes.toString("utf8")); candidate = JSON.parse(candidateBytes.toString("utf8")); provenance = JSON.parse(provenanceBytes.toString("utf8")); } catch { fail("Accepted template bundle metadata is invalid."); }
  if (!contract || contract.schemaVersion !== 1 || !Array.isArray(contract.inventory) || !candidate || candidate.schemaVersion !== 1 || !provenance || provenance.schemaVersion !== 1) fail("Accepted template bundle metadata is invalid.");
  if (!isSha(candidate.templateCommit, 40) || !isSha(candidate.archiveSha256, 64) || !isSha(candidate.contractSha256, 64) || !isSha(candidate.contractDigest, 64) || candidate.contractSha256 !== sha256(contractBytes) || candidate.archiveSha256 !== sha256(archiveBytes)) fail("Accepted template bundle checksum metadata is invalid.");
  if (typeof candidate.releaseTag !== "string" || candidate.releaseTag !== `template-v${candidate.templateVersion}-${candidate.templateCommit}` || !/^template-v[0-9]+\.[0-9]+\.[0-9]+(?:-[0-9A-Za-z.-]+)?-[a-f0-9]{40}$/.test(candidate.releaseTag) || String(candidate.templateVersion).includes("dev")) fail("Template bundle is not an accepted immutable release.");
  if (provenance.templateRepository !== "service-lasso/service-template" || provenance.templateCommit !== candidate.templateCommit || provenance.templateVersion !== candidate.templateVersion || provenance.contractDigest !== candidate.contractDigest || !provenance.origin || typeof provenance.catalogIdentity !== "string" || provenance.catalogIdentity.length === 0) fail("Accepted template provenance is incomplete.");
  const inventory: TemplateInventoryEntry[] = contract.inventory.map((entry: any) => {
    if (!entry || typeof entry !== "object" || typeof entry.path !== "string" || !isSha(entry.sha256, 64) || !/^(0644|0755)$/.test(entry.mode) || !Number.isSafeInteger(entry.bytes) || entry.bytes < 0) fail("Template inventory is invalid.");
    return { path: safeRelative(entry.path), sha256: entry.sha256, mode: entry.mode, bytes: entry.bytes };
  });
  if (new Set(inventory.map((entry) => entry.path)).size !== inventory.length || inventory.length === 0) fail("Template inventory is invalid.");
  for (const entry of inventory) {
    const path = resolve(root, entry.path); if (!path.startsWith(root + sep)) fail("Template inventory contains an unsafe path.");
    const stat = await lstat(path).catch(() => fail("Template payload does not match its inventory."));
    if (!stat.isFile() || stat.isSymbolicLink() || (process.platform !== "win32" && (stat.mode & 0o777) !== Number.parseInt(entry.mode, 8))) fail("Template payload contains an unsafe file.");
    const bytes = await readFile(path); if (bytes.length !== entry.bytes || sha256(bytes) !== entry.sha256) fail("Template payload does not match its inventory.");
  }
  return { repository: provenance.templateRepository, tag: candidate.releaseTag, commit: candidate.templateCommit, templateVersion: candidate.templateVersion, contractDigest: candidate.contractDigest, contractSha256: candidate.contractSha256, archiveSha256: candidate.archiveSha256, catalogIdentity: provenance.catalogIdentity, inventory, root };
}

export async function acceptedTemplateFiles(bundle: AcceptedTemplateBundle): Promise<Array<{ path: string; bytes: Buffer; mode: number }>> {
  return Promise.all(bundle.inventory.map(async (entry) => ({ path: entry.path, bytes: await readFile(resolve(bundle.root, entry.path)), mode: Number.parseInt(entry.mode, 8) })));
}
