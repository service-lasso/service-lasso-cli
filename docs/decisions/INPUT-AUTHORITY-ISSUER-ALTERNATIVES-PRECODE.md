# CLI39 IA1: concrete original-input issuer alternatives

Status: prospective whole architecture amendment, not implementation permission.
This continues the same full CLI39 unit. PA1/N2 source-only approvals, human
full-Node finite-budget choice, service aggregate1MiB, store4MiB, Core128MiB,
public library/SEA/Windows/Linux APIs and original proof wires remain intact.
Nothing here supplies an actual issuer, credential, key, admission or measured B.
The earlier provisioning source design and all failed/history packets are retained.

## Decision and scope

The recommended NEW alternative is IA1: an offline, owner-controlled Ed25519
input-admission issuer, with an independently selected immutable public anchor
and an actual native installation handoff. The alternative is to supply the real
existing HA1/HA5 issuer and adapt its actual authenticated output without changing
its authority. IA1 is recommended because there is presently no identified
existing issuer implementation or credential to call. Recommendation is not
selection of a person, account, key or trust root. A whole independent amendment
review precedes any IA1-dependent product code. After that review, the release
owner must identify the actual issuer principal, public-key bytes/fingerprint,
source and credential-custody policy. No arbitrary local key is acceptable.

This admission grants only reading the exact reviewed original input packet and
constructing the specified Development candidate resources. It grants neither
GA, publication, deployment, settings mutation, CI dispatch nor profile replacement.
The existing protected Development publication authorization is separate.

## Option E: bind an existing original issuer

Required originals are the issuer's selected source revision/full raw associations,
actual provider image and process/owner evidence, actual credential acquisition
interface, its output grammar/signature or retained capability semantics, and
the independent governing statement admitting that issuer for HA1/HA5. The caller
then opens the exact issuer output as a held read-only object, authenticates it
using that selected issuer mechanism, streams the complete original ROOT bytes
and all associated sources, and transfers retained native references to the
service. Its authority must already cover the specific source/profile/actor scope.

The adapter may not translate an unauthenticated report into authority, treat
an opaque constructor result as authentication, select an issuer from an argv
path, borrow HA2's ledger key, or promote a GitHub API token to ROOT authority.
If the actual existing grammar requires a new private codec or caller boundary,
that exact adapter is reviewed as an amendment before implementation. No such
existing original source or credential has been supplied. This option therefore
has concrete required inputs, rather than an imaginary callable provider.

## Option IA1: new offline issuer and independent anchor

The independently approved trust anchor consists of exactly 32 public-key bytes,
their SHA256 fingerprint, issuer principal/owner identity, admitted issuer source
and image associations, and permitted Development admission scope. The public
key may be embedded in the independently reviewed native verifier source: it
does not contain its own image digest or a ROOT that names itself. Each signed
statement names the final admitted consumer images externally after those images
exist. The consumer computes its digest from its held original self-image; it
does not embed that final digest in itself. Embedding the public anchor must not
silently embed the P/G/Q/C digest or own final sourceCommit and recreate a cycle.

Proposed owning sources are `tools/input-admission-issuer/` for the full-Node
offline issuer, `native/host-service/authority_verify.c` for the native verifier,
and `native/host-service/installation_admission.c` for the original native caller
and retained input handoff. These paths are prospective; no implementation or
actual source authority is claimed. Existing selected Node22.23.2 remains the
issuer runtime, including full ICU, Buffer and APIs. The issuer is a new explicit
administrative role in the finite full-Node owning domain, with a separately
certified complete capacity plan before entry; it is not charged to the 1MiB
service or exempted as a hidden host pool. Combined concurrent same-owner resources
and old/new overlaps remain charged to the governing finite domain. This role
and qualification case set are part of the amendment, not inherited silently.

Credential enrollment is an explicit owner action against the reviewed issuer.
The issuer acquires an original encrypted PKCS8 Ed25519 private-key object using
a native read-only held file lease and receives its passphrase from an original
native console input lease with echo disabled. It accepts neither argv nor ENV
passphrases. The caller must independently bind console/session, effective owner,
file identity/rights, issuer process birth and held issuer image before reading.
Failure to establish those original observations denies enrollment/signing.
The derived public key must exactly match the independently approved anchor.
Path, ACL, digest, password success and possession alone are not trust selection.

The signer uses selected Node `createPrivateKey`, `createPublicKey`,
`sign(null, originalPreimage, privateKey)` and `verify(null, originalPreimage,
publicKey, signature)` with the admitted Ed25519 type. Original selected source:
Node22.23.2 commit aa4c77582be995286fc6e00aaf530dc7ade102a9,
`doc/api/crypto.md`, blob c59d7f1042d4ac8ae6adc944845839ab9191d9c1,
sections createPrivateKey/generateKeyPair/sign/verify. Its private-key passphrase
limit is 1024 bytes. That raw source is held in the prior complete Node capture,
not imported or executed here. Encrypted-key, passphrase, KeyObject, OpenSSL and
temporary Buffer allocations belong to the admitted full-Node owner; all original
copies remain conservatively charged through independently observed actual issuer
exit. GC/finalizers and zeroing a public Buffer do not prove OpenSSL-key retirement.
The private signing key is distinct from HA2, never transferred to the service,
ordinary client, CI log or public proof archive. No key is generated in this task.

## Exact proposed private output

IA1 introduces one NEW private statement codec; it does not widen the shared
18/10/11/13/11 or N2 memory20 fields. The UTF8 JSON statement has exactly these
keys: `schema`, `issuerFingerprint`, `scope`, `platform`, `rootSha256`,
`rootByteCount`, `sourceAssociationSha256`, `actorSetSha256`, `profileSha256`,
`grantSha256`, `qualificationSha256`, `calibrationSha256`, `caseSetSha256`,
`capacityPlanSha256`, `authorizationId`. Schema is
`service-lasso-original-input-admission.v1`; scope is
`development-native-input-admission`; platform is `win32` or `linux`.
All digests are exactly 64 lowercase hex; byte count is a positive safe integer;
authorizationId is 32 lowercase hex chosen by the original issuer native entropy
observation. The complete raw statement is at most 16384 bytes. Duplicate decoded
keys, noncanonical integer forms, invalid UTF8, extra/missing keys and trailing
values fail closed. Signing uses exactly the bytes
ASCII `ServiceLassoOriginalInputAdmission1` followed by NUL, big-endian u32 raw
statement length and the ORIGINAL raw statement body, followed by a separate
64-byte Ed25519 signature. No JSON reserialization or digest-looking signature
surrogate is allowed. This proposed codec needs independent whole review.

`sourceAssociationSha256` names the original complete association manifest,
including commit/blob/rawSHA/size for every governing and product member.
`actorSetSha256` names a strict original actor policy member covering installer,
provider, service, primary/writer, observer and Node parent identities/images,
native-owner roles, and HA2 provider source authority. The proposed actor object
has exactly `schema`, `platform`, `actors`; schema is
`service-lasso-original-input-actors.v1`, platform matches the statement. `actors`
contains exactly seven entries in this order: installer, input-provider, service,
primary-writer, observer, node-parent, ledger-provider. Each has exactly `role`,
`sourceCommit`, `sourceAssociationSha256`, `imageSha256`, `ownerIdentity`,
`entrySourceSha256`. sourceCommit is the complete lowercase 40-hex Git commit;
the three SHA256 fields are exactly lowercase64-hex. Windows ownerIdentity is
the canonical decimal SID spelling of the original binary TokenUser SID; Linux
ownerIdentity is canonical unsigned decimal original effective UID. Original
native readers independently correlate binary/numeric identities rather than
trusting the text. Fixed role order forbids duplicates or extra roles. An actor
role's concrete entry source association and full image observation remain
required even where roles share an image; sharing does not create another budget.
Actual process birth belongs to fresh native handoff observation, not a signed
future process prediction. Actor object maximum is 16384 raw bytes and uses the
same strict UTF8/decoded-duplicate/canonical-number rules. Both are ROOT members;
the ROOT binds every member, its bytes, associations and closed policy grammar.
This entire proposed actor grammar is reviewed with IA1, not selected by the old
PA1/N2 approvals. Naming a digest
does not authenticate that object's bytes: the verifier consumes the original
held ROOT and every named held member through actual EOF, exact counts and hashes,
then applies their independently selected existing PA1/N2 source policies.

The detached signature and statement are public admission evidence, not secrets.
They can remain valid only for those exact immutable inputs and actor policy.
There is no wall-clock expiry dependency or remote refresh in IA1. Revocation or
anchor rollover requires a newly reviewed immutable consumer admission policy
and explicit owner selection, with old packets retained as revoked historical
evidence. A signature does not grant a newer image, source, root or scope.

## Actual original handoff and lifetime

The already admitted installation caller invokes the verifier before creating
service resources or reading client-controlled bytes. Windows transfers actual
held object handles with native DuplicateHandle between the independently
observed original caller/provider processes; full process birth/token/image,
FILE_ID_INFO, owner, access and noninheritance observations accompany the original
objects. Linux IA1 initially requires same-process native installer/provider
handoff using original F_DUPFD_CLOEXEC leases. Cross-process SCM_RIGHTS is not
implicitly authorized. Both platforms retain genuine source/image/root/input
objects rather than passing a pathname, digest string or a caller-created seal.

Verifier success produces an internal source-owned admission context containing
the anchor association, verified original statement/signature, original input
lease table and native caller/provider/owner references. The native constructor
accepts only that actual owned context in the same installation attempt; no
public constructor, TypeScript boolean, opaque user number or serialized success
field can construct it. Every context has an original attempt nonce and a
one-use state transition from held to transferred before native effects. Duplicate
transfer fails closed, keeping original references and full charge. A later
attempt reauthenticates the same immutable admission against fresh original
caller/native observations; replay of bytes alone cannot supply those references.
IA1 does not promise globally one-time publication or invent a durable replay
database; admission is exact-input authorization, not a consumable release grant.

P/G/Q/C retain their exact original readers and independent authority; Q32 is
qualification only and measured B is required before production reservation.
HA2 retains its separately selected original provider/key lease and ledger-MAC
role. IA1 binds that provider source in actor policy, but never generates its
key or substitutes the Ed25519 signing key. Real key provisioning remains an
independently authorized provider action and original native output obligation.
All caller/provider/image/ROOT/input references and failure captures survive
through independently read original archive persistence and actual resource
retirement. Validation failure, pending native IO or clock/capture failure retains
UNKNOWN custody; neither signature verification nor logical close retires bytes.

## Native verifier source and complete capacity obligation

Proposed verifier primitive is Monocypher's optional SHA512 Ed25519 verification,
not its default Blake2b EdDSA. The concrete API is
`crypto_ed25519_check(signature64, publicKey32, originalPreimage, length)`.
Its source, exact release, complete raw associations, compiler/CRT tuple and
security regression cases must be independently admitted before use. No release
is pinned or claimed qualified here. The API does not validate caller lengths;
our owned caller must establish exact disjoint ranges and complete original
body custody before verification. Known historical implementation/compiler
timing defects make an unqualified arbitrary version unacceptable. Only public
verification belongs in the service; issuer secret signing stays in the full-Node
owner. Primary source references:
[Ed25519 API](https://monocypher.org/manual/ed25519),
[known defects](https://monocypher.org/bugs),
[original source](https://github.com/LoupVaillant/Monocypher).

All verifier instructions/constants, statement/root streaming buffers, native
capture/context/table metadata, parser/verifier stack, CRT, image backing,
pending IO and installation bootstrap resources are charged to the original
aggregate service1MiB when service-owned. Independently owned OS ABI resources
need original normative ownership/custody evidence; unknown denies fit. No new
service cap, free stack, foreign label or hidden second allowance is proposed.
Actual whole image/stack/layout fit is a qualification obligation, not proved
by this API's small signature or source line count. If the admitted complete
implementation cannot fit, truthful failed qualification and precise counter-
evidence precede any separate material service-budget decision.

## Whole review and positive source acceptance

Review must cover the full IA1 trust selection, new actor-policy codec, statement
and signature grammar, enrollment/console/private-key acquisition, actual
Windows/Linux handoff, early owning constructor and finite issuer qualification,
service aggregate fit, HA2 separation, original input reader/persistence/lifetime,
all public library/SEA/ordinary CLI paths and the cumulative original source ROOT.
Source fixtures must include a genuine admitted-key positive vector, wrong
anchor/issuer/scope/actor/root/member/signature cases, strict decoded duplicates,
original-body modification and truncation, concurrent duplicate transfer,
pending/error native handoff, key/passphrase copies retained through actual exit,
and archive-readback failure with original custody retained. A synthetic key
vector proves the selected algorithm/codec only; it is never an authentic realm.

No IA1 implementation begins under PA1/N2's old approval. Independent ordinary
native owning routes continue meanwhile. A future full source acceptance must
distinguish implemented positive source paths from absent authentic provisioned
anchor/credential/provider/profile/images/B/key/catalog and actual execution.
