import { CliError } from "./errors.js";
import { createHash } from "node:crypto";

export const TEMPLATE_ASSET_NAMES = Object.freeze(["service-template.tar.gz", "template-candidate.json", "template-contract.json", "SHA256SUMS"] as const);
export const TEMPLATE_ASSET_LIMITS = Object.freeze([262144, 1024, 32768, 512] as const);
export interface TemplateInventoryEntry { path: string; sha256: string; mode: string; bytes: number; }
export interface TemplateAdmission {
  repository: string; tag: string; commit: string; templateVersion: string;
  kind: "development-template-candidate"; contractDigest: string; contractSha256: string;
  archiveSha256: string; catalogIdentity: string;
  assets: readonly { name: typeof TEMPLATE_ASSET_NAMES[number]; size: number; sha256: string }[];
  inventory: readonly TemplateInventoryEntry[];
  quotas: Readonly<Record<string, number>>;
}
declare const __SERVICE_LASSO_CONTROLLED_TEST_ADMISSIONS__: readonly TemplateAdmission[] | undefined;
// Fixture entries are a separately built source catalog, never runtime enrollment.
const selected = typeof __SERVICE_LASSO_CONTROLLED_TEST_ADMISSIONS__ === "undefined" ? [] : __SERVICE_LASSO_CONTROLLED_TEST_ADMISSIONS__;
export const TEMPLATE_ADMISSIONS: readonly TemplateAdmission[] = Object.freeze(selected.map(entry => Object.freeze({ ...entry, assets: Object.freeze(entry.assets.map(asset => Object.freeze({ ...asset }))), inventory: Object.freeze(entry.inventory.map(file => Object.freeze({ ...file }))), quotas: Object.freeze({ ...entry.quotas }) })));
export const sha256 = (bytes: Buffer | string): string => createHash("sha256").update(bytes).digest("hex");
export function catalogEntry(selector?: string): TemplateAdmission {
  const matches = TEMPLATE_ADMISSIONS.filter(entry => selector === undefined || entry.catalogIdentity === selector);
  if (matches.length !== 1) throw new CliError("template_identity_unavailable", "No independently admitted immutable template identity is available.");
  const entry = matches[0];
  if (entry.repository !== "service-lasso/service-template" || entry.kind !== "development-template-candidate" || !/^[a-f0-9]{40}$/.test(entry.commit) || !/^[a-f0-9]{64}$/.test(entry.contractDigest) || !/^[a-f0-9]{64}$/.test(entry.contractSha256) || !/^[a-f0-9]{64}$/.test(entry.archiveSha256) || !entry.templateVersion || entry.tag !== `template-v${entry.templateVersion}-${entry.commit}` || !entry.catalogIdentity || Buffer.byteLength(entry.catalogIdentity) > 256 || /[\x00-\x1f\x7f]/.test(entry.catalogIdentity) || entry.assets.length !== 4 || entry.inventory.length < 1 || entry.inventory.length + 2 > 128) throw new CliError("template_identity_unavailable", "The source-owned template catalog is unavailable.");
  for (let i = 0; i < 4; i++) {
    const asset = entry.assets[i];
    if (asset.name !== TEMPLATE_ASSET_NAMES[i] || !Number.isSafeInteger(asset.size) || asset.size < 1 || asset.size > TEMPLATE_ASSET_LIMITS[i] || !/^[a-f0-9]{64}$/.test(asset.sha256)) throw new CliError("template_identity_unavailable", "The source-owned template catalog is unavailable.");
  }
  if (entry.assets[0].sha256 !== entry.archiveSha256 || entry.assets[2].sha256 !== entry.contractSha256) throw new CliError("template_identity_unavailable", "The source-owned template catalog is unavailable.");
  return entry;
}
