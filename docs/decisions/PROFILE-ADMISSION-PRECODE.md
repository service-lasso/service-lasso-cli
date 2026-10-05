# Original external profile admission: concrete source dependency

Status: exact PA1 ENTIRE architecture GO for source implementation only, 2026-10-05.
Not a new admission, trust root,
product SOURCE GO, provisioned resource or execution authority.
The entire actual CLI39 product source remains unfinished.

The selected authority is BLUEPRINT-003 HA1 line19: the source-owned profile's
real commit/blob/raw SHA256 is independently admitted with native inputs; flags
and environment cannot select or replace it. Line21 separately requires actual
original owner/process/image/channel correlation and says root alone is
insufficient source authority. Line151 fixes the profile's ten closed keys and
the four serviceSource fields, including imageSha256. The v3 amendment at
line201 changes the profile version/clientCopies policy without creating a new
profile authority. HA2 line29 supplies the ledger key from a separately admitted
native provider and explicitly says ledger HMAC is not catalogue authority.
Lines143/149 keep external provisioning and catalogue grants separate. Line177
requires the launcher/observer/primary/library transport original source and
installed native objects to be independently admitted before enablement.

The current `native/host-admission/profile.go` embeds raw profiles in the service
module. If that same final image embeds a profile containing its own completed
image hash, there is a self-hash dependency. Computing the actual held self-image
hash at boot does not by itself authenticate an externally supplied profile.
An OS path, owner SID/uid, caller hash, signed-looking JSON, private provider
constructor or boolean report cannot fill the independent source authority.
This is a source mechanism dependency; absent resource provisioning does not
block independent production-engine/observer/allocator source authoring.

The dependency also affects the current same-module `nativeGrants` entries
keyed by `profileDigest`, and allocation contracts keyed by profile/source
digests in `native/host-admission/accounting.go`. Embedding the completed
profile digest back into the very service image named by that profile retains
the cycle even if the raw profile bytes move outside the image. Replacing raw
profile embedding alone therefore does not resolve the full source dependency.
Each actual original grant/calibration association must remain independently
admitted under the existing selected authority, with native object/owner/source
custody. A request-side digest or unadmitted provider cannot select these tables.
This observation selects no new grant, key, installer or admission mechanism.

Proposed internal caller and interface, before implementation:

* A separately admitted native input-admission provider owns the original
  external profile object and the exact already-admitted commit/blob/raw-byte
  association from the complete native input ROOT. It must independently
  authenticate that actual association under its selected source admission
  authority. The exact original provider source/image/authority implementation
  must be in the full source packet; there is no default provider or enrollment
  from an ordinary runtime request.
* The production native service bootstrap calls a private
  `acquireOriginalProfileAdmission()` on that source provider before creating
  the service endpoint, opening a store, accepting a client or launching an
  actor. Its result is an opaque retained original-object admission containing
  the original read-only profile object, actual independently admitted
  commit/blob/raw SHA256, actual size and original provider/source observation
  references. The caller cannot construct it with a path/digest/boolean.
* Private `loadOriginalProfile(admission)` reads the same held original object
  with the existing 16384-byte bound to real EOF, verifies original native
  owner/type/object identity before/after, genuine Git blob framing/identity,
  exact raw SHA256/size and the original independent source association, then
  applies the existing closed profile grammar. It retains a private original;
  returned fields never become authority. All temporary/native/runtime/IO
  ownership uses the original aggregate service-control account.
* The service computes its actual held self-image SHA256 and correlates original
  owner, birth, image and provider custody with that admitted profile. Other
  actors still undergo independent source/installed-object/native checks. A
  profile naming them does not enroll them.

This proposal does not select a signing algorithm/key, owner, provider image,
catalogue, grant, profile bytes, installer operation or new public field. The
complete existing provider authority and original native input ROOT association
must be concretely resolved and governed before implementing this acquisition
route. If the selected provider has no such original source-admission mechanism,
that exact added private mechanism needs a durable precode contract amendment
and different entire architecture review. The ledger key cannot be silently
promoted to a profile/catalogue trust root. No alternate profile parser, caller
sidecar, environment lookup or missing-mechanism success is permitted.

All selected public signatures, profile keys, native/public/private wire
grammars, rights, caps, original deadlines, retained failures, provider evidence
and complete final input ROOT requirements remain unchanged. Final product
source review must include this resolved mechanism together with the entire
production route, not approve this dependency note as a product checkpoint.

## Minimal whole source architecture amendment candidate PA1

PA1 is the proposed choice for a different ENTIRE architecture review. It is a
concrete private original-input admission extension, not approval, a supplied
provider, provisioning or authority inferred from this document. It replaces
all three circular same-image enrollment tables together. Independent engine,
observer, allocator and genuine empty-denial source work continues.

The circular graph is exact: service image A embeds profile P; P.serviceSource
names SHA256(A). A additionally embeds grants G and allocation contracts C
selected by SHA256(P). Moving P alone leaves A -> G/C -> SHA256(P) -> SHA256(A).
A may safely embed a separately admitted independent provider source/image pin
B only if B does not embed A's eventual digest or P's eventual digest. The
completed P/G/C associations then remain external original admitted objects.
Their hashes never select authority merely by matching themselves.

The existing authority is the independent native-input source admission in
HA1 line19 and HA5 line177. HA2 line29 selects a separately admitted native key
provider for ledger keys; it does NOT currently select an implementation of
native-input source admission or permit its ledger key to authorize P/G/C.
PA1 therefore adds exactly one private provider capability under that existing
independent input authority: acquire the originally admitted P/G/C tuple from
the original admission caller. It must be explicitly admitted alongside the
provider's source/image/owner/native inputs. No current callable provider,
authentic admission receipt, original actor pin or native input ROOT has been
supplied here. The exact implementation of this capability must be included in
the whole source candidate; an opaque Go type alone does not implement it.

The original trusted caller is a separately source-admitted native input
admission launcher/installation consumer. It owns the actual independently
admitted source packet and original read-only object leases. It may create the
service only after its own original image/owner/birth and the packet admission
are independently authenticated by that selected input authority. Ordinary CLI
clients, the service's endpoint requests, argv, environment, an administrator
path, an ACL-only check and the service itself are not this caller. This
caller is a required resource/source role, not a claim that one already exists.
The implementation must expose no enrollment or replacement API to an ordinary
runtime client and must preserve the separately authorized installation gate.

Its private original-object provider capability is acquired once before
endpoint/store/client/child work. The native service bootstrap is its sole
consumer. The provider retains a separate authenticated original caller,
original provider image/owner/birth, original admitted source-packet objects,
and read-only P/G/C objects. `acquireOriginalSourceAdmission()` returns a private
native lease identifying those original objects and their admission references;
it does not return an authority-bearing pathname, user JSON, caller digest or
boolean. A source-coded catalog pin to independent provider B is permitted only
with B's actual independent admission; an arbitrary B pin is not a new trust
root. Provider B may not select a replacement caller or synthesize a receipt.

The lease binds precisely the original provider/caller identity observations,
the original complete native input ROOT admission reference, original source
repository/full commit/genuine Git blob/actual raw SHA256/length association of
each P/G/C object, original native held-object identities and the completed
P digest named by G/C. All source references are independently authenticated by
the input authority before parser acceptance. This is private internal native
provider input, separate from every existing host, launch, inspection and Core
application frame; no shared proof18/closure10/launch11/input10/inspection13/
primitive11 key or operation changes. Calibration retains its existing closed raw format. The current grant is a
nativeGrant source struct, not an existing raw reader; PA1 must add that exact
bounded private grant grammar before code. It cannot claim a reader exists.
Both replace compiled self-enrollment associations. Calibration still requires the actual original
measurement/source associations; equal JSON/hash values are insufficient.

Bootstrap validates all original leases before and after bounded same-object
reads to actual EOF, raw size/SHA256, genuine Git blob framing, exact original
source association and the existing closed P/G/C readers. The service then
hashes its actual original held self-image and correlates original process
birth, owner and image with P. It may neither embed its own completed hash nor
turn the result into profile admission. The native provider's existing ledger
key capability stays separate, independently admitted, versioned and held;
ledger HMAC, signature-shaped bytes and a key returned by an unadmitted actor
cannot establish catalog/profile/grant/calibration authority.

Every lease is acquired once. No retry, reconnect or replacement resets
admission. Original provider/caller/image/source packet/P/G/C references remain
held through final service use and actual bound raw evidence/archive readback;
individual read handles may close only after same-object observations and raw
custody transfer are independently bound. Unknown closure retains the original
objects and conservative charge. Missing provider/caller/ROOT/raw association
produces genuine admission_unavailable before endpoint/store/launch or source
input IO; no initialized ledger or false successful fixture fills the gap.

Admission bootstrap itself must use the one original aggregate 1 MiB service
control domain, including runtime, stack, IO, image custody, lease metadata and
bounded parser/capture storage. It cannot read untrusted calibration and then
claim an unaccounted startup. The complete source-owned native bootstrap
reservation and its independent calibration/source association are part of
provider/native-input admission before bootstrap; no second control allowance,
Go GC/soft target or extra 4 MiB memory domain is created. Published client
allocation reservations remain conservative until actual original parent exit.
The full source implementation must demonstrate these native ownership chains;
PA1 provides no numeric measurement or fit assertion.

### Complete viable choices and disposition

1. PA1 external P/G/C tuple under the independently admitted original native
   input-admission caller/provider described above. It preserves existing
   profile/grant/calibration digest meanings, closed public/private fields,
   installed image/source comparisons and limits. Only the private admission
   capability and original external object ownership are added. Selected as the
   smallest candidate for review. Authentic caller/provider/ROOT provisioning
   stays separately authorized and absent; whole source implementation remains
   required, including real negative paths when absent.
2. Two-stage immutable build plus independently admitted external original
   P/G/C tables, supplied by the same original input authority without a live
   provider. This can break the graph only if the service does not embed their
   completed digests and an actual independently authenticated original
   installation caller transfers their held leases. A sidecar read by pathname
   is not this option. It requires a different concrete installation/lease
   protocol and retains all PA1 source-authority/native-custody duties; it does
   not remove the minimal missing admission mechanism.
3. Change profile image binding to a digest of a normalized image excluding an
   embedded mutable profile region, or split profile identity into a pre-image
   digest and final image digest. This can remove the hash cycle only with a
   precise independently admitted canonical image definition and new digest
   meanings across profile, grants, calibration, consumers and proof readers.
   It changes the existing actual-image contract and is a larger whole
   architecture amendment. It is not selected; no self-hash normalization or
   field weakening is coded.
4. Move the whole immutable authority/catalog to a separately admitted native
   supervisor which retains the complete tuple and launches the service from
   original held images. This can be coherent if the supervisor's independent
   source/owner/ROOT authority and all actual lifetime/IPC/allocator duties are
   fully implemented and admitted. It changes deployment/owner boundaries and
   has more source surface than PA1. It is not selected as a shortcut.

An embedded final profile/hash fixed point, profile-only relocation while G/C
remain embedded, environment/path/ACL trust, self-computed hash enrollment,
ledger-MAC admission, a fabricated provider key/catalog/grant and empty-success
stubs are not viable choices under the selected contract. There is no claim
that generic process RSS makes the selected source caps incompatible.

Review candidate scope: PA1 plus the ENTIRE original BLUEPRINT003/ADR002/F4
product/source inputs, not this document alone. The different reviewer must
assess the exact added authority capability/caller, full original-reference
lifetime, no circular compiled association, bootstrap control ownership,
separate ledger-key authority, empty-denial production path, shared reader
coherence and unchanged API/wire/caps/rights/deadlines. No positive integration
uses PA1 before that whole architecture decision. The whole product final
SOURCE review and NEW complete-input ROOT still precede every executable check.
PA1 supersedes the earlier profile-only acquisition proposal in this preserved
note. Its additional private grant raw object is exactly six keys: schema,
profileDigest, storePath, keyPath, retentionPath and clientUsers. The proposed
schema value is service-lasso-native-input-grant.v1; profileDigest is the
completed externally admitted P raw SHA256. All three paths are bounded
4096-byte, NUL-free strict UTF8 routing values under the platform's existing
absolute/native component rules. clientUsers contains unique original native
owner identities, each within the existing 184-byte owner bound. The complete
raw object is bounded to 16384 bytes, token/depth limits use the existing strict
reader, and the finite list is bounded to 128 entries in this proposed private
format. This proposed bound grants no owner and does not alter host16/user4.
No nativeGrant raw grammar previously existed; the added six-key private reader
is part of the PA1 amendment and review, not a historical source assertion.

keyPath is routing only: it neither returns key authority nor authenticates the
profile or grant. The actual separate HA2 original provider key lease must be
bound independently; the source must replace any pathname-only key read with
that genuine admitted original provider object. Store/retention routes likewise
require their original held root identities, native owner/rights and source
admission. The original provider acquisition owns P, G and every required
original calibration record, keeping the total admission lease references
inside the existing 128-reference original-object domain. Calibration uses its
existing closed schema and actual source/domain/native measurement references;
no universal four-record count, fabricated bytes or new per-actor quota is
selected. Shared primary/SEA/writer budgeting remains one 8 MiB domain and the
service remains one aggregate 1 MiB control domain. A larger required tuple
cannot be truncated to fit this source candidate.
## Exact PA1 source implementation decision

Parent accepted the different ENTIRE independent architecture review of the
17309-byte PA1 candidate, SHA256
ab58ad7d142071d66b982b9cbeb34a1fa76c65be11bdce07ccb1d625da1f0834.
Review REPORT.md is 17326 bytes, SHA256
d9808799c3d26bef0f198b5473bea0c454ebe1978b2f9134305f0f9336703221;
review ROOT SHA256
8e39d61011bba38f065f7d0678a420f4548140f3adc15c8adfd259194bda0597.
The original review remains at
D:/projects/service-lasso/_audit/cli39-pa1-entire-architecture-review-oct05-02.
Parent actual byte audit is
D:/projects/service-lasso/_audit/ga-windows-linux-resume-20261004-01/cli39-pa1-review-parent-byte-audit.json.
Author independently reread all 26606 actual reviewer input observations and
the two original review ROOT members with zero size/SHA mismatches, recorded in
the retained author PA1-REVIEW-AUTHOR-ACTUAL-BYTE-VERIFICATION.json.

The decision authorizes implementation of this exact PA1 private acquisition,
provider/caller/original lease readers, all external P/G/C associations and
separate original HA2 key custody together with the ENTIRE original product.
It does not supply an authentic provider/source actor/ROOT/key/catalog/owner
pin, admit a resource, authorize installation or enable execution. No constructor
or equal hash becomes authority. No alternate authority or codec inherits GO.
All original API/proof/wire fields, modes, rights, caps, deadlines, retention
and coordinated Core boundaries remain required. The exact finished ENTIRE
product still requires DIFFERENT final SOURCE GO and NEW complete-input ROOT
before every executable action. Preserved proposal text describes the reviewed
candidate and alternatives; only the exact selected PA1 mechanism is authorized.