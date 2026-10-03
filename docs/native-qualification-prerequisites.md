# Native qualification prerequisites and retained failures

Issue #30 remains source preparation. Original PR33 attempt 37003590854 is
retained; source head 2cf605b6bf889d1282d0bbbf722abd308dbd5144, base develop
b79d984347042fb163979b7075741659807dfeb1 and merge context
8945011d9d30a0f9ebbe97f13d876c2459af2532 are distinct evidence identities.
The Windows npm launch failure preceded product start. Linux smoke passed but
the zero-exit route test runner was misclassified. Darwin startup failed without
a stage-specific historical diagnostic; its precise internal cause is unproved.

## Darwin owner-controlled admission contract

Shipping the helper does not grant authority. Before any primary invocation,
including help/version, a separate host-owner integration must supply all of:

- An absolute, clean helper path in `SERVICE_LASSO_DARWIN_PRIVILEGED_HELPER`,
  naming the exact compiled helper digest embedded in that primary. Helper and
  parent must be root-owned regular file/directory with no group/other writes;
  ownership and digest checks remain in the primary.
- Root-owned, non-writable directory ancestors `/`, `/private`, `/private/var`,
  `/private/var/db`, `/private/var/db/service-lasso` and the canonical
  `/private/var/db/service-lasso/darwin-qualification-grant.json`, a root-owned
  no-follow regular mode0600 file, held identity checked by the helper.
  The current grant binds exactly one device/inode and one 32-byte capability.
  `/var` is the normal macOS alias; it is never the helper's authority path.
  No ancestor or grant alias is followed or accepted. Provision the canonical
  path directly; changing the system alias is neither required nor allowed.
  A static grant for a different object cannot authorise a new primary's
  temporary SEA, writer and parent. The owner-controlled integration must
  coordinate each actual object-specific set/clear operation and retain its
  identity evidence; general chflags authority is prohibited.
- A real inherited descriptor numbered at least3, identified by
  `SERVICE_LASSO_DARWIN_IMMUTABILITY_CAPABILITY_FD`, delivering exactly the
  reviewed capability for each helper invocation. The helper consumes32 bytes
  per invocation. A pathname, environment value, packaged token or one reused
  consumed stream is insufficient. Transport through wrapper, smoke, test and
  primary process boundaries must be reviewed explicitly; Node's default
  three-entry stdio does not inherit arbitrary descriptors.
- Noninteractive `/usr/bin/sudo -n -C 5 --` permission restricted to this exact
  helper and descriptor protocol, and actual SF_IMMUTABLE readback plus denied
  pre-open writer evidence for both images and their held parent. Clear and
  recovery outcomes must be retained without unknown-object deletion.

Current hosted workflows supply neither object-grant coordination nor descriptor
transport. Their explicit prerequisite step therefore records `blocked`, exits
nonzero and retains the initial receipt before any product smoke invocation.
It is deliberately not an environment-toggle bypass. A future separately
authorised owner integration must replace that blocked step through a reviewed
develop PR and new complete-input admission. All actual native smoke, hostile
helper/controlled admission tests, Darwin process-exit checks and same-byte
Core/operator qualification remain required. No test is converted to a pass
or skipped assertion. Ordinary macOS source tests retain their genuine failures
until that external authority is integrated.

Canonical grant regression source is in
`native/darwin-immutable-helper/main_darwin_test.go`. After fresh entire source
review and complete-input ROOT admission, a separately authorised owner-run
Darwin qualification must supply the real canonical grant and root execution.
The positive case reads that actual grant despite the normal `/var` alias;
isolated owner-owned temporary fixtures exercise ancestor/leaf symlinks, missing
paths, owner/type/mode rejection and actual named-versus-held inode replacement.
Subprocesses invoke the production helper entry point with short/wrong capability
and an ungranted held object, require exit2, and verify unchanged object flags.
These tests do not write the live grant, provision authority, run sudo or qualify
successful immutable set/clear. They are UNEXECUTED; owner provisioning remains
UNPERFORMED and successful native immutable qualification remains pending.

## Core dependency gate

The current workflows retain exact Core pin
`9bef20259e5b43f6bcd2e9796da0f35396305425`. Its actual build failed TS1005 at
src/server/index.ts(9179,1); no CLI acceptance ran. Current Core develop differs,
but a syntax difference does not qualify a replacement. Parent-owned governed
Core review, complete-input admission, compatible runtime/contract evidence and
landing must establish an exact replacement before a pin update and same-byte
direct acceptance. No retry, moving ref or guessed pin closes this gate.

Windows ZIP, Linux/macOS TAR, public immutable bytes and identical published
Core input remain full-programme requirements. Source review and these blocked
records are not native, runtime, publication or GA acceptance.

## Exact source seam and owner run proposal (UNPERFORMED)

`native/primary-gate/darwin_owner.go` is an unprivileged coordination client;
it is not an implemented or admitted privileged broker. The existing primary
still invokes the exact restricted sudo/helper FD3/FD4 protocol. Before doing
so it requires a real inherited Unix stream owner socket (FD3..64) named by
`SERVICE_LASSO_DARWIN_OWNER_CHANNEL_FD`, distinct from the capability FD.
Darwin LOCAL_PEERCRED must identify uid0. A socket path or environment token
cannot supply authority. Each operation holds the local client mutex through
helper completion/readback and sends the actual held object with SCM_RIGHTS.

The bounded1024byte newline protocol uses canonical compact JSON field order:

- Request: `{schemaVersion:1,operation:<fresh64hex>,mode:"set"|"clear",device:<actual>,inode:<actual>,owner:<actual>,kind:"image"|"parent",digest:<embedded-image-SHA256-or-empty-parent>}` plus exactly one held object FD. Only finite embedded SEA/writer byte sizes and digests, or a held directory, may be requested. Caller assertions are evidence to verify, never enrolment authority.
- Ready: `{schemaVersion:1,operation:<same>,device:<same>,inode:<same>,mode:<same>,status:"ready"}`. Unknown/duplicate fields, alternate serialization, trailing data, wrong operation/object/mode, missing/malformed/non-root peer and timeout fail closed. Request/ack IO has a fixed5second bound; existing product/compiler/test bounds are not widened.
- Completion: `{schemaVersion:1,operation:<same>,status:"readback-passed"|"failed"}`. This is the unprivileged client's observation of helper+flag readback only, not independent native custody, denied-preopen-writer proof or full acceptance. Loss/failure retains objects for owner recovery. The owner must never accept this message alone as privileged/native evidence.

Before ready, the independently admitted ephemeral owner endpoint must prove
membership from its exact approved primary/source/run profile and actual held
objects: source image digest, physical no-alias run root, job UID, parent/leaf
relationships and complete private parent inventory. It must reject any
unapproved object, even if the job supplies a plausible digest/device/inode.
It must own exclusive run custody of the canonical root grant, serialize
operations across ALL primaries, bind actual root grant identity/bytes to this
one held object, and deliver a fresh matching32byte capability to the capability
pipe for EACH set or clear. Root helper installation, grant replacement, sudo
FD preservation and recovery are actual owner actions, none performed here.
No always-running service, general flag API, self-enrolment or cross-run store
is introduced. The source client leaves these privileged endpoint obligations
explicitly unresolved; it cannot implement independent membership by trusting
its own caller request.

`scripts/native-qualification-stdio.mjs` explicitly maps both real inherited
FDs at wrapper -> smoke/test -> primary spawn boundaries while preserving
runtime/security ENV. Text-only/closed/shared/non-socket descriptors fail. It
performs transport, not root admission. The actual root peer remains checked
inside the primary. New Darwin source regressions check closed acknowledgement
contradictions, real SCM_RIGHTS object identity and completion, missing/invalid
FD denial, and actual Node child mapping/32byte pipe consumption. These are
UNEXECUTED seam/transport tests, never successful immutable helper qualification.
The root-positive seam test requires admitted root execution; its temporary
object is not a live grant and no flags/helper/sudo are used. The Node transport
test requires an exact physical `SERVICE_LASSO_QUALIFICATION_NODE` in the complete
ROOT packet and includes its literal generated child source/argv.

### Mandatory test caller admission and aggregate

Ordinary `node --test` runs native-distribution positives (historically tests
33/34/35) without owner provisioning. They MUST remain failed/blocked; the
negative external-helper/prerequisite checks do not satisfy their positives.
The nested fixture removes only NODE_TEST_CONTEXT, preserving all runtime and
security environment values. This repairs the ordinary selected-test recursion
failure on Windows/Linux, but it cannot invent Darwin owner descriptors.
Node's test coordinator spawns worker children without arbitrary extra FDs;
transport at an enclosing wrapper alone therefore does not admit a test worker.
The source has not replaced Node's internal worker launcher or injected runtime
hooks. An owner proposal must concretely select a separately reviewed direct
single-process node:test lane or an actual descriptor-delivery mechanism for
EVERY worker/selected runner. All unchanged positive test bodies, unavailable
inner-route digest/assertion checks, assertion-negative control, smoke journeys,
process-exit and canonical-grant tests remain mandatory, with terminal aggregate
failure until owner-native lane evidence passes. No skip/conditional assertion
or aggregate-pass substitution is prepared.

Reviewable owner choices before implementation/provision: actual authorized
Darwin arm64 host and job/root identities; exact source/tools/physical Node and
helper digests; private run roots; independent membership acquisition and
root-peer creation; exclusive existing grant/helper custody with retained prior
state; exact restricted sudo FD policy; test-worker/direct-lane choice; per-call
capability replenishment and cross-primary serialization; independent actual
native status/denied-writer/readback/recovery receipts and their private keeper.
No choice is inferred, and no provision/execution is authorized by this document.

## Current natural000b repair boundaries

Actual000b Windows smoke/route pass; archive failed GNU tar drive-colon output,
then ENOENT hashing discarded the phase record. The producer now keeps the
archive basename local to its owned output cwd. TAR candidate/public contract
stays unchanged. Issue #35 explicitly retires the misplaced CLI inner-ZIP
assertion; Core #1534 staged-service Windows ZIP and outer release Windows ZIP
remain mandatory.
The phase wrapper persists observed raw closure with present/absent/error digest
outcomes and fails closed on requested missing/unreadable bytes. Actual failing
producer, zero-close missing/unreadable artifact, failed-present artifact and
missing-executable subprocess regression sources are mandatory. They and the
Windows drive/spaced-path GNU-tar actual producer regression are UNEXECUTED.
Current Core9bef TS1005 remains a separate pending qualified replacement gate.
