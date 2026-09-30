import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import test from "node:test";

const cli = [process.execPath, "dist/index.js"];

function complete(shell) {
  return spawnSync(cli[0], [...cli.slice(1), "completion", shell], { encoding: "utf8" });
}

function hasCommand(command) {
  return spawnSync(command, ["--version"], { encoding: "utf8" }).status === 0;
}

test("completion source is deterministic, read-safe and derived from declared commands", () => {
  const first = complete("bash");
  const second = complete("bash");
  assert.equal(first.status, 0);
  assert.equal(first.stderr, "");
  assert.equal(first.stdout, second.stdout);
  assert.match(first.stdout, /service\/register/);
  assert.match(first.stdout, /--expected-manifest-sha256/);
  assert.doesNotMatch(first.stdout, /SERVICE_LASSO_CORE_TOKEN|SERVICE_LASSO_CLI_LOCAL_ADMIN_TOKEN|service-id|filesystem/i);
});

test("invalid completion shell keeps the stable error contract and emits no script", () => {
  const result = complete("fish");
  assert.equal(result.status, 1);
  assert.equal(result.stdout, "");
  assert.match(result.stderr, /unsupported_completion_shell/);
});

test("bash completion follows subcommands, option value positions and --", { skip: !hasCommand("bash") }, () => {
  const source = complete("bash").stdout;
  const result = spawnSync("bash", ["-s"], {
    encoding: "utf8",
    input: `${source}\nCOMP_WORDS=(service-lassoctl service st); COMP_CWORD=2; _service_lassoctl_completion; printf '%s\\n' \"\${COMPREPLY[@]}\"\nCOMP_WORDS=(service-lassoctl service init --directory value -- ''); COMP_CWORD=6; _service_lassoctl_completion; printf 'after--:%s\\n' \"\${COMPREPLY[@]}\"\n`,
  });
  assert.equal(result.status, 0, result.stderr);
  assert.deepEqual(result.stdout.trim().split("\n"), ["start", "stop", "after--:"]);
});

test("PowerShell completion follows subcommands and --", { skip: !hasCommand("pwsh") }, () => {
  const source = complete("powershell").stdout;
  const result = spawnSync("pwsh", ["-NoProfile", "-Command", "-"], {
    encoding: "utf8",
    input: `${source}\n_ServiceLassoCtlComplete -Words @('service-lassoctl','service') -WordToComplete 'st'\n$after = _ServiceLassoCtlComplete -Words @('service-lassoctl','service','--') -WordToComplete ''; Write-Output \"after--:$($after.Count)\"\n`,
  });
  assert.equal(result.status, 0, result.stderr);
  assert.deepEqual(result.stdout.trim().split(/\r?\n/), ["start", "stop", "after--:0"]);
});

test("zsh completion parses and computes static candidates", { skip: !hasCommand("zsh") }, () => {
  const source = complete("zsh").stdout;
  const result = spawnSync("zsh", ["-fc", `${source}\nwords=(service-lassoctl service st); CURRENT=3; _service_lassoctl_completion; print -l -- $reply`], { encoding: "utf8" });
  assert.equal(result.status, 0, result.stderr);
  assert.deepEqual(result.stdout.trim().split("\n"), ["start", "stop"]);
});
