# Repeatable isolated real-Core CLI smoke — CLI #51

Active child of #1: CLI-REAL-CORE-LOCAL-READ / CLI-AUTOMATION / CLI-CONNECTION.
The local/CI runner requires an explicit --core-dir and full --core-revision,
verifies exact checked-out Core source, and never silently qualifies old dist.
Build the ordinary seven-module CLI closure into private owned scratch using
installed locked tools, retain source/tool/output hashes, and exercise real argv
for help/version/status/inspect/services plus authenticated-read denial and JSON/
exit diagnostics against a disposable actual Core. Timeouts, output bounds,
secret-safe machine JSON and actual process closure are mandatory.

Fixture contract: only explicit source-built Core, owned private workspace,
services/instance/host registries, port0 readiness, env-only temporary auth and
owned-child cleanup. Root coordinates exact start/ready/token API before code.
No existing17883 instance, credential files or foreign services may be touched.
Source-build and packaged matching-host bytes are distinct evidence lanes.

Missing Core/compiler/template/native/API prerequisites produce explicit BLOCKED
stage/result and nonzero exit; no skip PASS or fabricated admission. Complete
create/register/lifecycle acceptance remains a distinct blocked extension until
all authentic dependencies and supported API are present. Native39 authoring,
SEA/helper execution, publication/deployment/release are not granted here.

Acceptance: repeatable npm entry and dedicated CI dispatch, local actual-Core
positive reads and auth denial with no secrets, current source/artifact identities,
finite start/command/stop and verified owned cleanup, failure-path unit evidence,
distinct review and normal develop landing/postmerge verification. Final runner
interface/fixture implementation choice will be frozen here before runtime code.

## Selected fixture interface

Root-selected Core-owned scripts/operator-e2e-fixture.mjs is the sole fixture
launcher. Spawn Node helper --core-dir DIR --expected-core-revision SHA
--output-dir FRESH --build-core. Bounded stdout readyJSON provides
{ready:true,connectionFile,receiptFile}; private600 connection is {url,token}.
Read token only from this owned fixture output and pass via child environment.
stdin stop newline stops owned Core/removes credential; verify final receipt and
credential absence. Runner takes --core-dir, --core-revision, --output-dir and
optional --core-fixture-helper (default Core helper). npm ci --ignore-scripts
prepares locked compiler/Commander; runner explicitly compiles7files in scratch.
Dedicated CI dispatch requires exact Core revision and builds fresh via helper.
Auth-error acceptance follows actual protected API contract, never assumes public
health is denied. Complete lifecycle remains reported BLOCKED separately from
bounded read smoke; root owns dependency qualification.
