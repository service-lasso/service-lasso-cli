# CLI39 product deadline handoff — bounded intent proposal

2026-10-16. CLI39/PR43. PRECODE ONLY; no dependent body or execution authorized.
Review base: 2e9ffb465d918f32c70595f7a9417212be0b0d4f. This resolves the
cross-process interpretation before implementing product child-local entry.

## Existing clauses and actual source

Corrected installer intent 4fc78537cb8edfebe7f0de1d337e5a3ff7bb9a05 says
"All references below are SAME-process retained originals", the child owns its
"own ... clock", and "A child-local operation original ... [is] constructed".
It also requires "The single source deadline retains the enclosing original
origin/expiration" and prohibits raw bytes carrying a clock or source grant.
These clauses must hold together, rather than silently equating C addresses
across processes or permitting a new duration at child entry.

BLUEPRINT-003-complete-native-v3-host-admission.md HA2/HA3/HA4 and phase proof
retain original absolute deadlines, the original phase limits and aggregate40s,
including independent finalization before the unchanged absoluteDeadlineTick.
NODE-MEASURED-FINITE-BUDGET-N2-PRECODE.md sections5..7 retain local reference
domains, complete actor custody and original deadline/lifetime obligations.
No clause identified here explicitly requires a single cross-process physical
clock cell. Fresh intent must decide this interpretation; it is not SOURCE_GO.

Actual product_storage_windows.c constructs the first product clock directly
in a final PRIVATE arena cell. source_deadline.c requires local live ranges,
same caller, actual local origin and unchanged expiration for that caller's
whole predecessor chain. native_clock.c Windows raw output arguments identify
the real local QPC/QPF output cells. A copied parent struct is not that original.
These generic predicates stay unchanged in this proposal.

## Proposed choice: enclosing original plus bounded local admission cuts

The actual parent product owner retains its original operation clock/deadline,
source/profile/reservation and external observer through ALL child startup,
consumer work, nested Go birth, streams, descendants, natural exit and closure.
Every parent effect continues to use that SAME parent origin and absolute
expiration. Birth/receipt/entry never replaces, restarts or extends them.

The child constructs an actual local clock observation directly in its paid
final arena cell. It is explicitly an ADMISSION CUT, not the parent's origin,
not a new phase start, not a budget issuer and not evidence of elapsed parent
history. Its local deadline expiration is at most the already fixed enclosing
absolute expiration. No duration is received or added to the cut. Existing
phase expirations are likewise preserved or shortened, never recomputed from
receipt time. Expired or incomparable clocks deny before product effects.
All same-process TUI consumers borrow this SAME child-local original/deadline;
no consumer, retry, callback or completion may create another local allowance.

For this choice, 4fc's SAME-origin wording means preservation within each
actual original's reference domain and continuing parent custody across birth;
it does not mean child C-pointer identity with the parent's clock. This precise
clarification requires fresh bounded intent BEFORE body. Historical 4fc text
and earlier construction evidence remain unchanged. No public deadline, phase,
proof grammar, cap, privilege, launch action or host trust boundary changes.

## Concrete native handoff and constructor source obligations

CLI owns product_deadline_windows.h/.c and product_entry_windows.h/.c bodies.
Proposed separate retained native types are product parent handoff owner,
child handoff acquisition, and child local deadline binding; none is a public
factory, callback, boolean capability or decoded authority packet.

The parent handoff owner retains actual product_storage, its caller/deadline,
prepared reservation/source occurrence, fixed created child/birth/process/thread
originals, held image/helper/provider originals, and the actual protected
readonly-section transport. Existing SD/transport constructors are usable
mechanisms only: successful bytes or SD correspondence do not issue membership.
The parent publishes fixed bounded deadline-routing scalars exactly once only
after joining those genuine originals. No raw native clock struct or pointer is
published as a child original. The scalar expiration constrains the subsequent
native acquisition; it cannot authenticate or authorize one.

Explicit source-owned STARTUPINFOEX handle-list inheritance transfers only the
fixed readonly routing handle(s), never an ambient inheritable-handle set.
Parent keeps its readonly original and actual created-process handle. Native
DuplicateHandle from that actual process back into the parent and actual
CompareObjectHandles against the retained original join the child's selected
handle to the SAME kernel object; handle numbers, PID, hashes and equal bytes
alone do not. Fixed child-local native acquisition joins its own process/token,
parent birth/principal, held installed image/source and actual readonly mapping
to the genuine installation/operation/reservation occurrence before binding.
No attach, caller-chosen section, default-current-user, replayed birth, arbitrary
SD, file/path reopen or missing source/profile relation may qualify.

Exact object/type/rights/readonly-view/offset/extent captures, actual producer
publication and writable-alias disposal are required. CompareObjectHandles is
only object correspondence, not rights, source, reserve or clock authority.
The child retains acquisition control outside payload, with separate exact
caller/deadline/clock blocks and complete initial-source/output exclusions.
Its initial preparation source/deadline covers acquisition BEFORE the local
cut, without an uncharged prefix. The cut cannot retroactively cover startup;
the enclosing parent original and genuine preparation occurrence cover it.

Native monotonic-domain correspondence must be established for this SAME boot/
host source case, including actual selected QPC frequency/conversion source
and checked overflow/rounding. Equality of frequency or current tick alone is
not that source relation. Clock regression, foreign boot/domain, late admission,
expiration increase or phase mismatch deny and retain the attempt. This design
does not convert the parent's native raw argument addresses into child pointers.

The actual TUI private Go birth remains DEBUG_ONLY_THIS_PROCESS|CREATE_SUSPENDED
under its SAME issuing thread/debug owner. It needs its OWN genuine handoff
acquisition before release and a no-later absolute bound; parent and TUI
originals remain retained. TUI's existing128B root is fully occupied and the
current two-handle inheritance roster has no deadline slot. This proposal
selects a separate private readonly handoff section plus an explicit additional
readonly inherited handle for that fixed Go birth, with CLI-owned native entry
acquisition and TUI-owned fixed list/operand integration. No root128 field is
repurposed, no second debugger attaches, and no scalar-only Go input qualifies.
Exact typed layout/codec, native rows and helper operands must be frozen with
the complete implementation, not inferred from these proposed type names.

## Alternatives and their real costs

A shared physical original could instead use a separately acquired section,
actual source-owned first clock write, readonly mappings, SAME-object/offset
joins, writable-alias closure and a new typed alias predicate. Distinct virtual
addresses cannot pass the existing local raw-output/self-pointer relationship;
forcing one address adds fixed-map failure and child mapping/allocator custody.
It also adds shared-page physical accounting, per-view virtual costs, parent/
child alias/allocator ownership and new cross-arena deadline APIs. This is a
viable alternative for review, not an already required architecture or admitted
mechanism. We propose the preceding local-domain choice because it preserves
the actual local predicate and explicit child-owned-clock clauses.

Copying/serializing a parent clock and relabelling it original, a fresh full
duration at entry, pointer-equality authority, and a new remote clock broker
are not selected. If bounded intent finds an exact canonical clause requiring
physical sharing, report that clause and choose the real alternative before
body; do not paper over it with a copied struct or weakened predicate.

## Finite costs, failures and required source cases

Prebirth source capacities cover fixed parent/TUI/Go handoff count, section/
view rounding and backing, every duplicate/readonly alias, descriptor/codec/
native raw rows and before/after clocks, initial and local cut cells, complete
image/loader/stack/TLS/helper/stream costs, all borrowers and retained failed
overlap. Shared backing is charged once physically with every alias separately
accounted; local controls/copies are individually charged. No second budget or
scope exclusion is inferred from a handoff. Capacity arithmetic and complete
actual source/physical dominance precede each native acquisition/effect.

No observer, section, child process reference, source/helper input, failed row
or deadline is retired before its actual last borrower. Once-use close/unmap
rows distinguish attempted/returned/pending/failed effects; no kill, retry,
finalizer or synthetic EOF/exit is a successful closure. Timeout remains failure
even if later natural settlement allows safe retirement.

Meaningful source cases must deny changed/extended phase or enclosing expiry,
copied origin, cut plus full duration, foreign host/boot/source/principal/birth,
same bytes in different object, unknown rights/offset/extent/writer alias,
late/partial native acquisition, regression/overflow, stale or repeated local
binding, missing startup coverage, nested Go second debugger, extra inherited
handle, early owner close and post-expiry success. Pure scalar/geometry tests
are labelled as such; genuine native positives require full actual qualification.

Status: proposed interpretation and private mechanism require fresh DIFFERENT
bounded intent review. No dependent source body, guard flip, native measurement,
compiler/parser/test/load execution or publication follows from this document.
Complete Windows A/B/full Node, Linux and product installer remain owned work;
whole final source review and NEW exact initial ROOT admission remain mandatory.
