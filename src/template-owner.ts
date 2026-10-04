import { TemplateAdmission, TemplateInventoryEntry, TEMPLATE_ASSET_NAMES, TEMPLATE_ASSET_LIMITS, sha256 } from "./template-admissions.js";
import { decodeOriginalArchive, TemplateFile } from "./template-archive.js";
import { canonicalBytes, closed, invalidTemplate, portablePath, sortedJson, strictJson, utf8 } from "./template-json.js";
import { CliError } from "./errors.js";

const quotaKeys = ["maximumFiles", "maximumTotalBytes", "maximumManifestBytes", "maximumProvenanceBytes", "maximumConfigFiles", "maximumConfigBytes", "maximumArchiveBytes", "maximumArchiveExpandedBytes", "maximumArchiveEntries", "maximumArchivePathDepth"] as const;
const fields = ["/id", "/name", "/description", "/version", "/enabled", "/meta/developers", "/meta/repository/url", "/meta/tags"] as const;
function same(a: unknown, b: unknown): boolean { return sortedJson(a) === sortedJson(b); }
function string(value: unknown, maximum: number): string { if (typeof value !== "string" || Buffer.byteLength(value) > maximum || /[\x00-\x1f\x7f]/.test(value)) return invalidTemplate(); return value; }
function positive(value: unknown, maximum: number): number { if (!Number.isSafeInteger(value) || (value as number) < 1 || (value as number) > maximum) return invalidTemplate(); return value as number; }
function strings(value: unknown, maximum: number): string[] { if (!Array.isArray(value) || value.length > maximum || value.some(item => typeof item !== "string")) return invalidTemplate(); return value as string[]; }
export function validateAuthoring(id: string, name?: string): void {
  if (!/^[a-z][a-z0-9-]{0,62}$/.test(id)) throw new CliError("invalid_service_id", "Service id must use 1-63 lowercase letters, numbers, and hyphens, beginning with a letter.");
  if (name !== undefined && !/^[A-Za-z0-9][A-Za-z0-9 .,'()/_-]{0,119}$/.test(name)) throw new CliError("invalid_service_name", "Service name does not satisfy the template owner rule.");
}
export interface VerifiedTemplate { admission: TemplateAdmission; policy: Record<string, unknown>; files: TemplateFile[]; baselineId: string; baselineName?: string; }
export function validateOriginalAssets(assets: readonly Buffer[], entry: TemplateAdmission): VerifiedTemplate {
  if (assets.length !== 4) return invalidTemplate();
  assets.forEach((asset, i) => { if (asset.length !== entry.assets[i].size || asset.length > TEMPLATE_ASSET_LIMITS[i] || sha256(asset) !== entry.assets[i].sha256) invalidTemplate(); });
  const policy = closed(strictJson(assets[2], 32768), ["schemaVersion", "templateVersion", "contractDigest", "inventory", "quotas", "authoring", "provenance"]);
  const descriptor = closed(strictJson(assets[1], 1024), ["schemaVersion", "kind", "templateCommit", "templateVersion", "contractDigest", "contractSha256", "archiveSha256", "releaseTag"]);
  if (policy.schemaVersion !== 1 || policy.templateVersion !== entry.templateVersion || policy.contractDigest !== entry.contractDigest || descriptor.schemaVersion !== 1 || descriptor.kind !== entry.kind || descriptor.templateCommit !== entry.commit || descriptor.templateVersion !== entry.templateVersion || descriptor.contractDigest !== entry.contractDigest || descriptor.contractSha256 !== entry.contractSha256 || descriptor.archiveSha256 !== entry.archiveSha256 || descriptor.releaseTag !== entry.tag || !canonicalBytes(policy).equals(assets[2]) || !canonicalBytes(descriptor).equals(assets[1])) invalidTemplate();
  const semantic = Object.fromEntries(Object.entries(policy).filter(([key]) => key !== "contractDigest"));
  if (sha256(sortedJson(semantic)) !== entry.contractDigest) invalidTemplate();
  const quotas = closed(policy.quotas, quotaKeys);
  const hardCaps = [128, 768000, 6298, 479, 16, 65536, 262144, 524288, 128, 12];
  quotaKeys.forEach((key, i) => { positive(quotas[key], hardCaps[i]); if (quotas[key] !== entry.quotas[key]) invalidTemplate(); });
  const inventory = policy.inventory;
  if (!Array.isArray(inventory) || inventory.length < 1 || inventory.length + 2 > (quotas.maximumFiles as number) || inventory.length !== entry.inventory.length) invalidTemplate();
  let previous = "", total = 0;
  for (let i = 0; i < (inventory as unknown[]).length; i++) {
    const file = closed((inventory as unknown[])[i], ["path", "sha256", "mode", "bytes"]);
    const path = portablePath(file.path, quotas.maximumArchivePathDepth as number);
    if (path <= previous || !/^[a-f0-9]{64}$/.test(file.sha256 as string) || !["0644", "0755"].includes(file.mode as string) || !Number.isSafeInteger(file.bytes) || (file.bytes as number) < 0 || !same(file, entry.inventory[i])) invalidTemplate();
    previous = path; total += file.bytes as number; if (total > (quotas.maximumTotalBytes as number)) invalidTemplate();
  }
  const authoring = closed(policy.authoring, ["provenanceFile", "manifest", "configuration"]);
  if (authoring.provenanceFile !== "template-provenance.json") invalidTemplate();
  const manifest = closed(authoring.manifest, ["path", "fields"]); if (manifest.path !== "service.json") invalidTemplate();
  const rules = closed(manifest.fields, fields);
  for (const pointer of fields) {
    const rule = rules[pointer];
    if (pointer === "/enabled") { const row = closed(rule, ["mode", "type"]); if (row.mode !== "author-editable" || row.type !== "boolean") invalidTemplate(); }
    else if (pointer === "/meta/developers") {
      const row = closed(rule, ["mode", "type", "maximumItems", "items"]); if (row.mode !== "author-editable" || row.type !== "array") invalidTemplate(); positive(row.maximumItems, 8);
      const items = closed(row.items, ["type", "properties"]); if (items.type !== "object") invalidTemplate();
      const properties = closed(items.properties, ["name"]); const name = closed(properties.name, ["type", "maximumBytes", "pattern"]); if (name.type !== "string") invalidTemplate(); positive(name.maximumBytes, 120); string(name.pattern, 512);
    } else if (pointer === "/meta/tags") {
      const row = closed(rule, ["mode", "type", "maximumItems", "items"]); if (row.mode !== "author-editable" || row.type !== "array") invalidTemplate(); positive(row.maximumItems, 12);
      const items = closed(row.items, ["type", "maximumBytes", "pattern"]); if (items.type !== "string") invalidTemplate(); positive(items.maximumBytes, 48); string(items.pattern, 512);
    } else {
      const row = closed(rule, ["mode", "type", "maximumBytes", "pattern"]); if (row.type !== "string" || row.mode !== (pointer === "/meta/repository/url" ? "github-derived-identity" : "author-editable")) invalidTemplate(); positive(row.maximumBytes, pointer === "/description" ? 512 : pointer === "/meta/repository/url" ? 256 : pointer === "/id" ? 63 : pointer === "/version" ? 64 : 120); string(row.pattern, 512);
    }
  }
  const configuration = closed(authoring.configuration, ["pathPrefix", "allowedPaths", "forbiddenNamePattern", "forbiddenValuePattern"]);
  if (configuration.pathPrefix !== "config/" || !same(configuration.allowedPaths, ["config/example.env"])) invalidTemplate(); string(configuration.forbiddenNamePattern, 1024); string(configuration.forbiddenValuePattern, 1024);
  const provenance = closed(policy.provenance, ["required", "templateRepository", "originKinds", "maximumRepositoryBytes"]);
  if (!same(provenance.required, ["schemaVersion", "templateRepository", "templateCommit", "templateVersion", "contractDigest", "origin"]) || provenance.templateRepository !== entry.repository || !same(provenance.originKinds, ["local-archive", "github-derived"])) invalidTemplate(); positive(provenance.maximumRepositoryBytes, 140);
  const sums = utf8(assets[3]);
  const expectedSums = [0, 1, 2].map(i => `${entry.assets[i].sha256}  ${TEMPLATE_ASSET_NAMES[i]}\n`).join("");
  if (sums !== expectedSums) invalidTemplate();
  const files = decodeOriginalArchive(assets[0], assets[2], entry);
  const immutableBytes = entry.inventory.filter(file => file.path !== "service.json" && !(configuration.allowedPaths as string[]).includes(file.path)).reduce((sum, file) => sum + file.bytes, 0);
  if (quotas.maximumTotalBytes !== immutableBytes + assets[2].length + (quotas.maximumManifestBytes as number) + (quotas.maximumProvenanceBytes as number) + (quotas.maximumConfigBytes as number)) invalidTemplate();
  const baseline = files.find(file => file.path === "service.json"); if (!baseline || baseline.bytes.length > (quotas.maximumManifestBytes as number)) return invalidTemplate();
  const baselineManifest = strictJson(baseline.bytes, quotas.maximumManifestBytes as number) as Record<string, unknown>;
  if (!baselineManifest || typeof baselineManifest !== "object" || Array.isArray(baselineManifest) || !canonicalBytes(baselineManifest).equals(baseline.bytes) || typeof baselineManifest.id !== "string" || (baselineManifest.name !== undefined && typeof baselineManifest.name !== "string")) invalidTemplate();
  validateAuthoring(baselineManifest.id as string, baselineManifest.name as string | undefined);
  return { admission: entry, policy, files, baselineId: baselineManifest.id as string, baselineName: baselineManifest.name as string | undefined };
}
export function deriveTemplate(template: VerifiedTemplate, authoring?: { id: string; name?: string }): TemplateFile[] {
  const id = authoring?.id ?? template.baselineId, name = authoring?.name ?? template.baselineName;
  validateAuthoring(id, name);
  const files = template.files.map(file => ({ ...file, bytes: Buffer.from(file.bytes) }));
  const manifest = files.find(file => file.path === "service.json")!;
  const baseline = strictJson(manifest.bytes, template.admission.quotas.maximumManifestBytes) as Record<string, unknown>;
  const derived = { ...baseline, id, ...(name === undefined ? {} : { name }) };
  manifest.bytes = canonicalBytes(derived);
  if (manifest.bytes.length > template.admission.quotas.maximumManifestBytes) invalidTemplate();
  const origin = { kind: "local-archive", archiveSha256: template.admission.archiveSha256 };
  const provenance = canonicalBytes({ schemaVersion: 1, templateRepository: template.admission.repository, templateCommit: template.admission.commit, templateVersion: template.admission.templateVersion, contractDigest: template.admission.contractDigest, origin });
  if (provenance.length > template.admission.quotas.maximumProvenanceBytes) invalidTemplate();
  files.push({ path: "template-provenance.json", mode: 0o644, bytes: provenance }); files.sort((a, b) => a.path < b.path ? -1 : a.path > b.path ? 1 : 0);
  verifyHeldPlan(template, files, id, name); return files;
}
export function verifyHeldPlan(template: VerifiedTemplate, files: readonly TemplateFile[], id: string, name?: string): void {
  validateAuthoring(id, name);
  if (files.length !== template.admission.inventory.length + 2 || files.length > template.admission.quotas.maximumFiles) invalidTemplate();
  const originals = new Map(template.files.map(file => [file.path, file])); let previous = "", total = 0, configBytes = 0, configs = 0;
  for (const file of files) {
    const path = portablePath(file.path); if (path <= previous || ![0o644, 0o755].includes(file.mode)) invalidTemplate(); previous = path; total += file.bytes.length;
    if (total > template.admission.quotas.maximumTotalBytes) invalidTemplate();
    if (path === "template-provenance.json") {
      const value = closed(strictJson(file.bytes, template.admission.quotas.maximumProvenanceBytes), ["schemaVersion", "templateRepository", "templateCommit", "templateVersion", "contractDigest", "origin"]);
      const origin = closed(value.origin, ["kind", "archiveSha256"]);
      if (file.mode !== 0o644 || value.schemaVersion !== 1 || value.templateRepository !== template.admission.repository || value.templateCommit !== template.admission.commit || value.templateVersion !== template.admission.templateVersion || value.contractDigest !== template.admission.contractDigest || origin.kind !== "local-archive" || origin.archiveSha256 !== template.admission.archiveSha256 || !canonicalBytes(value).equals(file.bytes)) invalidTemplate();
      continue;
    }
    const original = originals.get(path); if (!original || original.mode !== file.mode) invalidTemplate();
    if (path === "service.json") {
      const baseline = strictJson(original.bytes, template.admission.quotas.maximumManifestBytes) as Record<string, unknown>;
      const actual = strictJson(file.bytes, template.admission.quotas.maximumManifestBytes) as Record<string, unknown>;
      if (!same(actual, { ...baseline, id, ...(name === undefined ? {} : { name }) }) || !canonicalBytes(actual).equals(file.bytes)) invalidTemplate();
    } else if (!original.bytes.equals(file.bytes)) invalidTemplate();
    if (path.startsWith("config/")) {
      const authoring = template.policy.authoring as Record<string, unknown>; const configuration = authoring.configuration as Record<string, unknown>;
      if (!(configuration.allowedPaths as string[]).includes(path)) invalidTemplate();
      const content = utf8(file.bytes);
      if (new RegExp(configuration.forbiddenNamePattern as string, "i").test(path) || new RegExp(configuration.forbiddenValuePattern as string, "im").test(content)) invalidTemplate();
      configs++; configBytes += Buffer.byteLength(content);
    }
  }
  if (configs > template.admission.quotas.maximumConfigFiles || configBytes > template.admission.quotas.maximumConfigBytes) invalidTemplate();
}
