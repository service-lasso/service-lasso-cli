import { mkdir, readFile, rename, writeFile } from "node:fs/promises";
import { dirname, join } from "node:path";
import { homedir } from "node:os";
import { CliError } from "./errors.js";

export interface CliConfig {
  coreUrl?: string;
}

export const DEFAULT_CORE_URL = "http://127.0.0.1:17883";

export function configPath(home = homedir()): string {
  return join(home, ".service-lasso-cli", "config.json");
}

export function normalizeCoreUrl(value: string): string {
  let url: URL;
  try {
    url = new URL(value);
  } catch {
    throw new CliError("invalid_core_url", "Core URL must be an absolute http(s) URL.");
  }
  if (url.protocol !== "http:" && url.protocol !== "https:") {
    throw new CliError("invalid_core_url", "Core URL must use http or https.");
  }
  if (url.username || url.password || url.pathname !== "/" || url.search || url.hash) {
    throw new CliError("invalid_core_url", "Core URL must be an origin without credentials, path, query, or fragment.");
  }
  return url.origin;
}

function isLoopbackHost(hostname: string): boolean {
  const host = hostname.toLowerCase().replace(/^\[|\]$/g, "");
  if (host === "localhost" || host === "::1" || host === "0:0:0:0:0:0:0:1") return true;
  const octets = host.split(".");
  return octets.length === 4 && octets[0] === "127" && octets.every((octet) => /^\d{1,3}$/.test(octet) && Number(octet) <= 255);
}

/**
 * A local-admin token must never cross a cleartext network connection. HTTP is
 * retained only for the local Core defaults and other explicit loopback use.
 */
export function assertCoreTokenTransport(baseUrl: string, token: string | undefined): void {
  if (!token) return;
  const normalized = new URL(normalizeCoreUrl(baseUrl));
  if (normalized.protocol === "https:" || isLoopbackHost(normalized.hostname)) return;
  throw new CliError(
    "insecure_core_token_transport",
    "SERVICE_LASSO_CORE_TOKEN requires an HTTPS Core URL unless the HTTP origin is loopback.",
  );
}

export async function loadConfig(path = configPath()): Promise<CliConfig> {
  try {
    const raw = await readFile(path, "utf8");
    const parsed: unknown = JSON.parse(raw);
    if (!parsed || typeof parsed !== "object" || Array.isArray(parsed)) {
      throw new CliError("invalid_config", `Config file is not an object: ${path}`);
    }
    const coreUrl = (parsed as CliConfig).coreUrl;
    if (coreUrl !== undefined && typeof coreUrl !== "string") {
      throw new CliError("invalid_config", `Config coreUrl must be a string: ${path}`);
    }
    return coreUrl ? { coreUrl: normalizeCoreUrl(coreUrl) } : {};
  } catch (error) {
    if (typeof error === "object" && error && "code" in error && (error as { code: string }).code === "ENOENT") return {};
    if (error instanceof CliError) throw error;
    if (error instanceof SyntaxError) throw new CliError("invalid_config", `Config file contains invalid JSON: ${path}`);
    throw error;
  }
}

export async function saveConfig(config: CliConfig, path = configPath()): Promise<void> {
  const normalized: CliConfig = config.coreUrl ? { coreUrl: normalizeCoreUrl(config.coreUrl) } : {};
  await mkdir(dirname(path), { recursive: true });
  const temporary = `${path}.tmp`;
  await writeFile(temporary, `${JSON.stringify(normalized, null, 2)}\n`, { encoding: "utf8", mode: 0o600 });
  await rename(temporary, path);
}

export function resolveCoreUrl(options: { cliValue?: string; environment?: NodeJS.ProcessEnv; config: CliConfig }): string {
  const candidate = options.cliValue ?? options.environment?.SERVICE_LASSO_CORE_URL ?? options.config.coreUrl ?? DEFAULT_CORE_URL;
  return normalizeCoreUrl(candidate);
}

/**
 * Core accepts a local-admin token as a Bearer credential. Keep it environment
 * only so CI can inject a secret without adding it to arguments or config.
 */
export function resolveCoreToken(environment: NodeJS.ProcessEnv = process.env): string | undefined {
  const token = environment.SERVICE_LASSO_CORE_TOKEN;
  if (token === undefined || token.length === 0) return undefined;
  if (/\s/.test(token)) throw new CliError("invalid_core_token", "SERVICE_LASSO_CORE_TOKEN must not contain whitespace.");
  return token;
}
