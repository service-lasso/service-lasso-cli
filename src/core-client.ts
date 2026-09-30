import { CliError } from "./errors.js";
import { assertCoreTokenTransport } from "./config.js";

export type FetchLike = typeof fetch;

export interface CoreClientOptions {
  baseUrl: string;
  token?: string;
  localAdminToken?: string;
  fetch?: FetchLike;
}

export class CoreClient {
  private readonly requestFetch: FetchLike;
  public constructor(private readonly options: CoreClientOptions) {
    assertCoreTokenTransport(options.baseUrl, options.token);
    assertCoreTokenTransport(options.baseUrl, options.localAdminToken);
    this.requestFetch = options.fetch ?? fetch;
  }

  public async health(): Promise<unknown> {
    return this.request("/api/health");
  }

  public async services(): Promise<unknown> {
    return this.request("/api/services");
  }

  public async inspect(): Promise<unknown> {
    const [health, instance, capabilities] = await Promise.all([
      this.health(),
      this.request("/api/runtime/instance"),
      this.request("/api/runtime/capabilities"),
    ]);
    return { health, instance, capabilities };
  }

  public async lifecycle(serviceId: string, action: "start" | "stop" | "restart"): Promise<unknown> {
    if (!serviceId) throw new CliError("invalid_service_id", "Service id is required.");
    return this.request(`/api/services/${encodeURIComponent(serviceId)}/${action}`, {
      method: "POST",
      headers: { "content-type": "application/json" },
      body: JSON.stringify({ confirm: true }),
    });
  }

  public async operatorStatus(): Promise<unknown> { return this.request("/api/runtime/instance"); }
  public async setup(): Promise<unknown> { return this.request("/api/setup/status"); }
  public async serviceHealth(serviceId: string): Promise<unknown> { return this.request(`/api/services/${this.serviceId(serviceId)}/health`); }
  public async serviceDependencies(serviceId: string): Promise<unknown> { return this.request(`/api/services/${this.serviceId(serviceId)}/dependencies`); }

  public async availability(serviceId: string): Promise<LifecycleAvailability> {
    const result = await this.request(`/api/operator/lifecycle/services/${this.serviceId(serviceId)}/availability`);
    if (!isAvailability(result)) throw new CliError("unsupported_core_contract", "Core did not advertise the durable lifecycle operation contract.");
    if (result.contractVersion !== "service-lasso-durable-lifecycle-operation.v1") {
      throw new CliError("unsupported_core_contract", "Core advertised an unsupported durable lifecycle contract version.");
    }
    return result;
  }

  public async previewLifecycle(serviceId: string, action: LifecycleAction, parameters: Record<string, unknown> = {}): Promise<unknown> {
    await this.assertActionAvailable(serviceId, action);
    return this.request("/api/operator/lifecycle/operations", { method: "POST", headers: { "content-type": "application/json" }, body: JSON.stringify({ action, serviceId, ...parameters }) });
  }

  public async executeLifecycle(input: LifecycleExecute, availability?: LifecycleAvailability): Promise<unknown> {
    this.assertAvailable(availability ?? await this.availability(input.serviceId), input.action);
    if (!input.confirmationId || !input.confirmationPhrase || !/^[A-Za-z0-9][A-Za-z0-9._-]{7,127}$/.test(input.idempotencyKey)) {
      throw new CliError("invalid_durable_operation", "A confirmation id, confirmation phrase, and opaque 8-128 character idempotency key are required.");
    }
    return this.request("/api/operator/lifecycle/operations", { method: "POST", headers: { "content-type": "application/json" }, body: JSON.stringify({ action: input.action, serviceId: input.serviceId, execute: true, idempotencyKey: input.idempotencyKey, confirmationId: input.confirmationId, confirmationPhrase: input.confirmationPhrase, ...input.parameters }) });
  }

  public async lifecycleOperation(operationId: string): Promise<unknown> {
    if (!/^[-A-Za-z0-9_]{8,200}$/.test(operationId)) throw new CliError("invalid_operation_id", "Operation id must be an opaque operation identifier.");
    return this.request(`/api/operator/lifecycle/operations/${encodeURIComponent(operationId)}`);
  }

  public async cancelLifecycleOperation(operationId: string, cancellationSupported: boolean): Promise<unknown> {
    if (!cancellationSupported) throw new CliError("cancellation_unsupported", "Core reports that this operation cannot be cancelled.");
    return this.request(`/api/operator/lifecycle/operations/${encodeURIComponent(operationId)}/cancel`, { method: "POST", headers: { "content-type": "application/json" }, body: "{}" });
  }

  private serviceId(serviceId: string): string {
    if (!serviceId || serviceId.includes("/")) throw new CliError("invalid_service_id", "Service id is required and cannot contain a slash.");
    return encodeURIComponent(serviceId);
  }

  private async assertActionAvailable(serviceId: string, action: LifecycleAction): Promise<void> {
    this.assertAvailable(await this.availability(serviceId), action);
  }

  private assertAvailable(availability: LifecycleAvailability, action: LifecycleAction): void {
    const advertised = availability.actions.find((entry) => entry.action === action);
    if (!advertised?.available) throw new CliError("action_unavailable", "Core does not advertise this durable lifecycle action as available.");
  }

  public async registerReleasedService(input: ReleasedServiceRegistration): Promise<ReleasedServiceOperation> {
    if (!input.confirm) throw new CliError("confirmation_required", "This action changes a running Core instance. Re-run with --confirm after reviewing the target.");
    assertReleasedServiceRegistration(input);
    return this.requestOperation("/api/runtime/actions/importService", {
      method: "POST",
      headers: { "content-type": "application/json" },
      body: JSON.stringify(input),
    }, true);
  }

  public async releasedServiceOperation(operationId: string): Promise<ReleasedServiceOperation> {
    if (!/^sro_[a-f0-9]{32}$/.test(operationId)) {
      throw new CliError("invalid_operation_id", "Operation id must be a service-registration operation id.");
    }
    return this.requestOperation(`/api/operator/operations/${encodeURIComponent(operationId)}`);
  }

  private async requestOperation(path: string, init?: RequestInit, acceptConflict = false): Promise<ReleasedServiceOperation> {
    const response = await this.request(path, init, acceptConflict, true);
    if (!response || typeof response !== "object" || Array.isArray(response) || !("operation" in response) || !isReleasedServiceOperation(response.operation)) {
      throw new CliError("invalid_core_response", "Core returned an invalid service-registration operation response.");
    }
    return response.operation;
  }

  private async request(path: string, init?: RequestInit, acceptConflict = false, useLocalAdminToken = false): Promise<unknown> {
    let response: Response;
    try {
      response = await this.requestFetch(new URL(path, this.options.baseUrl), {
        ...init,
        // A redirect can change the origin after the token transport check.
        // Reject it rather than allowing fetch to replay a request elsewhere.
        redirect: "error",
        headers: {
          accept: "application/json",
          ...(this.options.token ? { authorization: `Bearer ${this.options.token}` } : {}),
          ...(useLocalAdminToken && this.options.localAdminToken ? { "x-service-lasso-admin-token": this.options.localAdminToken } : {}),
          ...init?.headers,
        },
      });
    } catch (error) {
      // Fetch implementations can include request headers in their errors. Do
      // not expose their detail because it may contain the configured token.
      throw new CliError("core_unreachable", "Could not reach Service Lasso Core.", "Set SERVICE_LASSO_CORE_URL or run `service-lassoctl config set core-url <url>`.");
    }
    const raw = await response.text();
    if (!response.ok && !(acceptConflict && response.status === 409)) {
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

export type LifecycleAction = "install" | "configure" | "start" | "stop" | "restart";
export interface LifecycleExecute { action: LifecycleAction; serviceId: string; idempotencyKey: string; confirmationId: string; confirmationPhrase: string; parameters?: Record<string, unknown>; }
export interface LifecycleAvailability { contractVersion: string; actions: Array<{ action: string; available: boolean; cancellationSupported?: boolean }>; }
function isAvailability(value: unknown): value is LifecycleAvailability {
  return Boolean(value && typeof value === "object" && !Array.isArray(value) && typeof (value as { contractVersion?: unknown }).contractVersion === "string" && Array.isArray((value as { actions?: unknown }).actions));
}

export interface ReleasedServiceRegistration {
  repo: string;
  tag: string;
  expectedCommit: string;
  expectedManifestSha256: string;
  idempotencyKey: string;
  confirm: true;
}

export interface ReleasedServiceOperation {
  id: string;
  kind: "service_registration";
  status: "completed" | "conflict" | "unknown";
  replayed: boolean;
  actorId: string;
  repo: string;
  tag: string;
  sourceCommit: string;
  serviceId: string;
  version: string | null;
  createdAt: string;
  completedAt: string | null;
  errorCode: string | null;
}

function assertReleasedServiceRegistration(input: ReleasedServiceRegistration): void {
  if (!/^[A-Za-z0-9_.-]+\/[A-Za-z0-9_.-]+$/.test(input.repo)) throw new CliError("invalid_release_reference", "Repository must be an owner/repository release reference.");
  if (!/^[A-Za-z0-9][A-Za-z0-9._/-]{0,127}$/.test(input.tag)) throw new CliError("invalid_release_reference", "Tag must be a bounded release tag.");
  if (!/^[a-f0-9]{40}$/.test(input.expectedCommit)) throw new CliError("invalid_release_reference", "Expected commit must be a lowercase 40-character commit SHA.");
  if (!/^[a-f0-9]{64}$/.test(input.expectedManifestSha256)) throw new CliError("invalid_release_reference", "Expected manifest SHA-256 must be a lowercase digest.");
  if (!/^[A-Za-z0-9][A-Za-z0-9._-]{7,127}$/.test(input.idempotencyKey)) throw new CliError("invalid_idempotency_key", "Idempotency key must be an opaque 8-128 character key.");
}

function isReleasedServiceOperation(value: unknown): value is ReleasedServiceOperation {
  if (!value || typeof value !== "object" || Array.isArray(value)) return false;
  const operation = value as Partial<ReleasedServiceOperation>;
  return typeof operation.id === "string" && operation.kind === "service_registration"
    && (operation.status === "completed" || operation.status === "conflict" || operation.status === "unknown")
    && typeof operation.replayed === "boolean" && typeof operation.actorId === "string"
    && typeof operation.repo === "string" && typeof operation.tag === "string"
    && typeof operation.sourceCommit === "string" && typeof operation.serviceId === "string"
    && (typeof operation.version === "string" || operation.version === null)
    && typeof operation.createdAt === "string" && (typeof operation.completedAt === "string" || operation.completedAt === null)
    && (typeof operation.errorCode === "string" || operation.errorCode === null);
}
