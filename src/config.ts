import { mkdir, readFile, rename, writeFile } from "node:fs/promises";
import { dirname, join } from "node:path";
import { homedir } from "node:os";
import { CliError } from "./errors.js";

export interface CliConfig {
  coreUrl?: string;
  defaultConnection?: string;
  connections?: Record<string, { coreUrl: string }>;
}

export const DEFAULT_CORE_URL = "http://127.0.0.1:17883";

export function configPath(home = homedir()): string {
  return join(home, ".service-lasso-cli", "config.json");
}

export function validateConnectionName(name: string): string {
  if (!/^[a-z][a-z0-9-]{1,63}$/.test(name)) {
    throw new CliError("invalid_connection_name", "Connection names must use lowercase letters, numbers, and hyphens (2-64 characters).");
  }
  return name;
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
    const config = parsed as CliConfig;
    const coreUrl = config.coreUrl;
    if (coreUrl !== undefined && typeof coreUrl !== "string") {
      throw new CliError("invalid_config", `Config coreUrl must be a string: ${path}`);
    }
    if (config.defaultConnection !== undefined && typeof config.defaultConnection !== "string") {
      throw new CliError("invalid_config", `Config defaultConnection must be a string: ${path}`);
    }
    const connections: Record<string, { coreUrl: string }> = {};
    if (config.connections !== undefined) {
      if (!config.connections || typeof config.connections !== "object" || Array.isArray(config.connections)) {
        throw new CliError("invalid_config", `Config connections must be an object: ${path}`);
      }
      for (const [name, connection] of Object.entries(config.connections)) {
        validateConnectionName(name);
        if (!connection || typeof connection !== "object" || Array.isArray(connection) || Object.keys(connection).length !== 1 || typeof connection.coreUrl !== "string") {
          throw new CliError("invalid_config", `Config connection entries must contain only coreUrl: ${path}`);
        }
        connections[name] = { coreUrl: normalizeCoreUrl(connection.coreUrl) };
      }
    }
    const defaultConnection = config.defaultConnection ? validateConnectionName(config.defaultConnection) : undefined;
    if (defaultConnection && !connections[defaultConnection]) throw new CliError("invalid_config", `Config defaultConnection must name a saved connection: ${path}`);
    return { ...(coreUrl ? { coreUrl: normalizeCoreUrl(coreUrl) } : {}), ...(defaultConnection ? { defaultConnection } : {}), ...(Object.keys(connections).length ? { connections } : {}) };
  } catch (error) {
    if (typeof error === "object" && error && "code" in error && (error as { code: string }).code === "ENOENT") return {};
    if (error instanceof CliError) throw error;
    if (error instanceof SyntaxError) throw new CliError("invalid_config", `Config file contains invalid JSON: ${path}`);
    throw error;
  }
}

export async function saveConfig(config: CliConfig, path = configPath()): Promise<void> {
  const normalized: CliConfig = {};
  if (config.coreUrl) normalized.coreUrl = normalizeCoreUrl(config.coreUrl);
  if (config.connections) {
    normalized.connections = {};
    for (const [name, connection] of Object.entries(config.connections)) {
      normalized.connections[validateConnectionName(name)] = { coreUrl: normalizeCoreUrl(connection.coreUrl) };
    }
  }
  if (config.defaultConnection) {
    normalized.defaultConnection = validateConnectionName(config.defaultConnection);
    if (!normalized.connections?.[normalized.defaultConnection]) throw new CliError("invalid_config", "Default connection must name a saved connection.");
  }
  await mkdir(dirname(path), { recursive: true });
  const temporary = `${path}.tmp`;
  await writeFile(temporary, `${JSON.stringify(normalized, null, 2)}\n`, { encoding: "utf8", mode: 0o600 });
  await rename(temporary, path);
}

export function resolveConnectionName(options: { cliValue?: string; environment?: NodeJS.ProcessEnv; config: CliConfig }): string | undefined {
  const candidate = options.cliValue ?? options.environment?.SERVICE_LASSO_CONNECTION ?? options.config.defaultConnection;
  return candidate ? validateConnectionName(candidate) : undefined;
}

export function resolveCoreUrl(options: { cliValue?: string; connection?: string; environment?: NodeJS.ProcessEnv; config: CliConfig }): string {
  const connection = resolveConnectionName({ cliValue: options.connection, environment: options.environment, config: options.config });
  if (connection && !options.config.connections?.[connection]) throw new CliError("unknown_connection", "The selected Core connection is not saved locally.");
  const candidate = options.cliValue ?? options.environment?.SERVICE_LASSO_CORE_URL ?? (connection ? options.config.connections?.[connection]?.coreUrl : undefined) ?? options.config.coreUrl ?? DEFAULT_CORE_URL;
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
