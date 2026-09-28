import { CliError } from "./errors.js";

export type FetchLike = typeof fetch;

export interface CoreClientOptions {
  baseUrl: string;
  fetch?: FetchLike;
}

export class CoreClient {
  private readonly requestFetch: FetchLike;
  public constructor(private readonly options: CoreClientOptions) {
    this.requestFetch = options.fetch ?? fetch;
  }

  public async health(): Promise<unknown> {
    return this.request("/api/health");
  }

  public async services(): Promise<unknown> {
    return this.request("/api/services");
  }

  public async lifecycle(serviceId: string, action: "start" | "stop" | "restart"): Promise<unknown> {
    if (!serviceId) throw new CliError("invalid_service_id", "Service id is required.");
    return this.request(`/api/services/${encodeURIComponent(serviceId)}/${action}`, {
      method: "POST",
      headers: { "content-type": "application/json" },
      body: JSON.stringify({ confirm: true }),
    });
  }

  private async request(path: string, init?: RequestInit): Promise<unknown> {
    let response: Response;
    try {
      response = await this.requestFetch(new URL(path, this.options.baseUrl), init);
    } catch (error) {
      const detail = error instanceof Error ? error.message : "network request failed";
      throw new CliError("core_unreachable", `Could not reach Service Lasso Core: ${detail}`, "Set SERVICE_LASSO_CORE_URL or run `service-lasso config set core-url <url>`.");
    }
    const raw = await response.text();
    if (!response.ok) {
      // Core response bodies can include operator-specific or sensitive detail.
      // Keep the CLI's public error contract to status metadata only.
      throw new CliError("core_api_error", `Core returned HTTP ${response.status}.`);
    }
    try {
      return raw ? JSON.parse(raw) : {};
    } catch {
      throw new CliError("invalid_core_response", "Core returned a non-JSON response.");
    }
  }
}
