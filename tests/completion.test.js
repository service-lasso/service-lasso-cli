import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { chmodSync, mkdtempSync, mkdirSync, rmSync, writeFileSync } from "node:fs";
import { tmpdir } from "node:os";
import { join } from "node:path";
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
  assert.match(complete("zsh").stdout, /compinit -i -D/);
  assert.match(complete("zsh").stdout, /"\$\{\(@\)words\[2,CURRENT-1\]\}"/);
  assert.match(complete("zsh").stdout, /\[\[ \$candidate == "\$current"\* \]\]/);
  assert.doesNotMatch(complete("zsh").stdout, /compinit -u|compinit -C/);
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

test("PowerShell completion uses literal prefixes, follows subcommands and stops after --", { skip: !hasCommand("pwsh") }, () => {
  const source = complete("powershell").stdout;
  assert.match(source, /StartsWith\(\$WordToComplete, \[StringComparison\]::Ordinal\)/);
  assert.doesNotMatch(source, /-like/);
  const script = `${source}\n_ServiceLassoCtlComplete -Words @('service-lassoctl','service') -WordToComplete 'st'\n$prefixes = [ordered]@{ star = '*'; question = '?'; bracket = '['; space = ' '; singleQuote = "'"; doubleQuote = '"'; semicolon = ';' }\nforeach ($name in $prefixes.Keys) { $matches = @(_ServiceLassoCtlComplete -Words @('service-lassoctl','service') -WordToComplete $prefixes[$name]); Write-Output "\${name}:$($matches.Count)" }\n$after = _ServiceLassoCtlComplete -Words @('service-lassoctl','service','--') -WordToComplete ''; Write-Output "after--:$($after.Count)"\n`;
  const result = spawnSync("pwsh", ["-NoLogo", "-NoProfile", "-NonInteractive", "-Command", script], { encoding: "utf8" });
  assert.equal(result.status, 0, result.stderr);
  assert.deepEqual(result.stdout.trim().split(/\r?\n/), ["start", "stop", "star:0", "question:0", "bracket:0", "space:0", "singleQuote:0", "doubleQuote:0", "semicolon:0", "after--:0"]);
});

test("zsh completion parses and computes static candidates", { skip: !hasCommand("zsh") }, () => {
  const source = complete("zsh").stdout;
  const result = spawnSync("zsh", ["-fc", `${source}\nwords=(service-lassoctl service st); CURRENT=3; _service_lassoctl_completion; print -l -- $reply\nwords=(service-lassoctl service init --directory value -- ''); CURRENT=7; _service_lassoctl_completion; print \"after--:$#reply\"`], { encoding: "utf8" });
  assert.equal(result.status, 0, result.stderr);
  assert.deepEqual(result.stdout.trim().split("\n"), ["start", "stop", "after--:0"]);
});

test("zsh bootstrap ignores insecure fpath entries and creates no completion dump", { skip: !hasCommand("zsh") }, () => {
  const temp = mkdtempSync(join(tmpdir(), "service-lassoctl-completion-"));
  const insecure = join(temp, "insecure");
  mkdirSync(insecure);
  writeFileSync(join(insecure, "_service_lassoctl_probe"), "#compdef service-lassoctl-probe\n");
  chmodSync(insecure, 0o777);

  try {
    const source = complete("zsh").stdout;
    const script = `fpath=(${JSON.stringify(insecure)} $fpath)\n${source}\n[[ -z \${_comps[service-lassoctl-probe]-} ]] || exit 42\n[[ ! -e \"$HOME/.zcompdump\" && ! -e \"$ZDOTDIR/.zcompdump\" ]] || exit 43`;
    const result = spawnSync("zsh", ["-fc", script], {
      encoding: "utf8",
      env: { ...process.env, HOME: temp, ZDOTDIR: temp },
    });
    assert.equal(result.status, 0, result.stderr);
  } finally {
    rmSync(temp, { recursive: true, force: true });
  }
});
