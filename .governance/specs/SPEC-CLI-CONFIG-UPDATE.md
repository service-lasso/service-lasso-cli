# Preserve saved profiles during Core URL updates — CLI #47

Active child of #1/#8; requirement CLI-CONNECTION. The reachable package command
in src/index.ts and alternate src/cli.ts must update only coreUrl, retaining
validated named connections and defaultConnection. Existing flag/environment/
profile precedence stays unchanged. A missing configuration may be created;
an invalid URL must leave existing bytes unchanged.

Implement an explicit updateCoreUrl(value,path=configPath()) config helper:
validate the new URL, load the current config, save the merged validated config.
Use it at both config-set actions. No other configuration contract changes.
Replacing saveConfig semantics was rejected: callers intentionally save complete
validated documents, while this command means a single-key update.

Proof: distinct exact-source review before isolated copied config/errors compile
and unchanged+regression config tests with Node22.23.2/TypeScript5.9.3. Regressions
cover saved profile/default retention and resolution precedence, absent file,
and invalid-input byte preservation. Source review binds both real callsites;
full index/help/native graph execution is not admitted by this issue. No CLI39,
publication, release, provider, Core lifecycle or deployment authority follows.

## Focused proof — 2026-11-14

Distinct reviewer local_test_review accepted cumulative candidate 200bac0 and
explicit copied config/errors/test execution. Node22.23.2 + TypeScript5.9.3
strict ES2022 NodeNext isolated compile PASS; exact candidate config tests PASS
10/10, zero skipped. Private evidence /tmp/cli-local-validation-20261114/issue47/
green/{compile.log,test.log}. Scratch-only controlled reproduction of the old
command write effect (baseline config plus a named adapter calling
saveConfig({coreUrl:value},path)) failed precisely the profile-retention assertion
9/10. This is not execution of the historical CLI command. Both real callsites
were source-reviewed; full entrypoint/native graph execution remains unperformed.
Source copies were compared after proof; no native/publication claim follows.
