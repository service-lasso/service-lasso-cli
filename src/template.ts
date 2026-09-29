import { CliError } from "./errors.js";

/** The reviewed service-template release used for deterministic local authoring. */
export const SERVICE_TEMPLATE_IDENTITY = Object.freeze({
  repository: "service-lasso/service-template",
  tag: "2026.5.8-d2241fe",
  commit: "d2241fe9b5fc477f14e99adb1836825de2c7a767",
  serviceJsonSha256: "535b939b39d96b3e72750c8a4c404d88f68e7f94150bc8d42ae6695baf9e1fd4",
});

export function canonicalTemplateManifest(id: string, name: string): Record<string, unknown> {
  return {
    id,
    name,
    description: `Service Lasso service authored from ${SERVICE_TEMPLATE_IDENTITY.tag}.`,
    enabled: false,
    version: "0.1.0",
    logoutput: true,
    icon: "terminal",
    servicetype: 50,
    servicelocation: 10,
    actions: {
      install: { description: "Prepare the service runtime payload." },
      config: { description: "Materialize effective runtime configuration." },
      start: { description: "Start the managed service." },
      stop: { description: "Stop the managed service gracefully." },
    },
    execconfig: {
      serviceorder: 100,
      serviceport: 0,
      execcwd: "runtime",
      executable: "REPLACE-ME",
      env: {},
      depend_on: [],
      healthcheck: { type: "process" },
    },
  };
}

export function assertCanonicalTemplateManifest(manifest: Record<string, unknown>): void {
  const actions = manifest.actions as Record<string, unknown> | undefined;
  const execconfig = manifest.execconfig as Record<string, unknown> | undefined;
  if (!manifest.id || !manifest.name || !manifest.version || !actions?.install || !actions.config || !actions.start || !actions.stop || !Array.isArray(execconfig?.depend_on) || !execconfig.healthcheck) {
    throw new CliError("invalid_template_manifest", "The pinned service template is missing required identity, lifecycle, or runtime declarations.");
  }
  if (JSON.stringify(manifest).includes('"channel":"latest"')) {
    throw new CliError("mutable_template_reference", "Generated service manifests must not select a mutable latest template channel.");
  }
}
