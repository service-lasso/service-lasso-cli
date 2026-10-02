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
