# CLI39 fixed BQ arena caller and combined table factory

2026-10-12. Refinement of selected TABLE-LINEAR-PREOWNERSHIP35, for parent
source-topology disposition before dependent bodies. No source GO, execution
admission, native qualification or numeric fit follows from this method.

## Actual existing-owner mismatch

`source_windows.c:live` requires the Service arena with limit1048576 and its
store/primary/workspaces. Its SRW and issuing-thread bookkeeping cannot be used
as the domain0 BQ lock by passing a different arena. `early_allocator.c` owns
the Node Runtime marker and bound arena. It cannot serialize BQ allocations.
`arena.h` describes an owning lock but provides no actual BQ caller holding it.
The combined factory must establish that missing native relation itself.

## One BQ-owned caller, no transferable lock authority

Current call chains are `bq_source_association_windows_observe_original` and
`bq_source_unit_windows_observe_original` -> sink preparation -> table preparation
-> namespace acquisition -> read_prepared -> returning table/sink match -> finish.
Their bodies accept preowned allocations but no genuine serialized BQ caller.
`bq_installation_capture_windows` continues source observation -> native member
window -> first105, so completing either table is not the last borrower. The new
fixed installer composition must own BOTH table chains, all-member graph traversal
and that later capture/journal/archive continuation under one original caller.

The genuine installer preowns a fixed BQ caller inside the SAME original BQ
arena, with exact original owner/extent/acquisition/deadline/caller references.
Its source-selected Windows SRWLOCK is initialized once by the installer before
publishing the caller. Linux uses an always-lock-free atomic_flag with a native
gettid observation; neither path creates a kernel mutex, pool or global state.
Full caller storage, initialization/native calls/captures, TLS/runtime and failed
prefix costs belong to the existing BQ domain and remain numerically UNKNOWN.

The actual fixed installer function uses one nonwaiting exclusive acquisition,
records the actual current native thread, and directly calls the fixed combined
A-then-unit constructor. No arbitrary callback, supplied lock-held bit or token
can substitute this call path. Contention fails closed without waiting/retry;
the original failure and already-owned prefix remain. An active BQ caller is
published in the SAME arena only while this actual exclusive acquisition is held.
Every participating allocation validates the exact original caller and issuing
thread. General arena users retain their existing contract when no BQ caller is
attached; they cannot detach, overwrite or evade an attached BQ caller.

## Fixed free-tail allocator and single chain walk

Under that acquisition, the constructor walks the entire real allocator chain
from extent base, checks every header/bound/state/requested/next/alignment and
retains the exact final free-tail header. Earlier free holes remain observed but
are not reused by this factory. Missing free tail denies capacity honestly.
The fixed factory alone advances the retained free-tail cursor after each exact
split; no scan from allocation_floor is performed per allocation. Every new
header/request/padding charge is checked and every partial remains reachable.
Other allocation/free/realloc/resize/coalesce entry points deny while the fixed
factory cursor is active. The cursor cannot be supplied to a generic allocator.

All controls, row rosters and A/unit pointer arrays precede interleaved body then
deadline blocks, first every A range and then every unit range. The factory owns
their exact geometry and monotonic addresses. Complete borrowed controls and
caller/deadline/prior/sink/native views are excluded before mutable effects.
One complete subsequent real-chain walk joins every actual allocation to its
preowned block row, including unrelated and failed-prefix blocks. Per-row lookup
thereafter checks the original native owner tuple and actual exact current header
in bounded work; it never accepts a cached positive result as membership.

## Absorbing original source borrow and actual lifetime

Before any source IO the SAME caller installs a source-read borrow that cannot
be replaced or cleared on failure, unknown/pending native calls or expired D.
Every arena mutation path checks it before touching any header/payload, including
calloc/realloc, free(NULL), in-place resize and coalescing. The exclusive caller
and issuing-thread original stay held through all source IO, full table replay,
every actual member/Git/nonGit/provider/publication join, sink/archive/journal
consumers and independently proved actual last native/CPU borrower. Returning
an intermediate observation never releases the borrow. Last-borrower closure is
a fixed whole-owner function over actual retained consumers, not a boolean or
callback argument. An incomplete owner remains retained and unavailable; no
automatic unlock, thread teardown, cancellation, ACK or CLOSED implies retirement.

## Exact per-body sink lane

The existing deadline owner retains its actual body allocation, original inline
capture address/count, SAME actual sink and exact firstTarget/count interval.
Preparation binds this once under the above original factory/borrow. Windows
`input_deadline_after` dispatches directly to the private fixed sink with that
SAME deadline owner; capture_original continues to denote the real sink. The
sink reconstructs capture ordinal using checked address subtraction, alignment
and original capture extent, then derives the target inside the retained interval.
It checks actual row/header/currentness/borrow/body/native raw operands/payload
before each copy. Neither caller-supplied ordinal nor generic callback chooses it.
Full returning reconstruction checks every body/interval/target once, and both
Windows actual callers require the same factory ownership. Generic sinks retain
their separate existing path, without acquiring BQ authority.

## Complete-unit boundary

This refinement is not an optimization-only freeze. Entire authentic A/member/
Git/nonGit/provider/capture/publication and installer/module/native entry/runtime
source construction remains required. No numeric R/B0/B2, original-D time bound,
source reduction, new quota, extra Runtime or copied-source authority is selected.
Whole fresh independent review of the frozen/pushed cumulative bundle and NEW
authentic complete ROOT/exact commands precede all target execution.

## Native operands and branch disposition

Existing Windows `source_windows_enter` uses one TryAcquireSRWLockExclusive on
`&source->lock`, GetCurrentThreadId, and Interlocked owner fields, then releases
its transition SRW before IO. Existing Linux `source_linux_enter` uses
atomic_flag_test_and_set_explicit(acquire), actual slcli_kernel_call(__NR_gettid,
0,0,0,0,0,0), atomic
owner fields and release-clear before IO. Those are pattern/source references
only: their Service marker and recursion/leave rules must not be copied as BQ
authority or release eligibility. BQ owns a distinct lock inside its existing
original domain, not an existing Service object relabelled.

For BQ, lock initialization has its own preowned called/returned record; native
GetCurrentThreadId has a separate called/returned/DWORD cell; the nonwaiting
try uses ONLY `&bqCaller->lock`, storing actual BOOLEAN before postclock. Neither
native call uses GetLastError as a success classifier. The exact entered native
call and raw return remain retained if a later clock or constructor fails.
Release is VOID with independent called/returned markers, exact SAME lock and
actual SAME issuing thread. No second unlock after uncertain entered release.
Microsoft documents nonrecursive SRW ownership and release by the acquiring
thread; a BQ nested fixed helper checks the existing owner and never reacquires.
Sources read on 2026-10-12:

- https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-tryacquiresrwlockexclusive
- https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-releasesrwlockexclusive
- https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentthreadid

Linux records the actual __NR_gettid raw long/entered/returned independently
of atomic acquisition result. It uses only its preowned flag address with C11
acquire/test-and-set and release/clear, compile-time always-lock-free requirements
for published owner/unresolved fields. No pthread allocation or libc mutex path.
Current `source_linux.c` is an existing source pattern; the attempted man7 gettid
page fetch timed out, so no complete external page or selected kernel ABI claim
is made. Exact target kernel/syscall/compiler codegen qualification remains owed.

Prebirth installer geometry includes caller+native records+one complete arena
walk state, exact combined range/body/deadline rows, all call/branch/clock outputs,
and native/currentness observations for every public entry. Their count derives
from the complete actual source schedule and checked geometry; no invented fixed
attempt limit. The installer must already genuinely own these preowned bytes and
the initialization window before attaching the caller; parsed geometry cannot
bootstrap its lock, arena or source. A retained exact current-thread observation
and source-owned same-thread direct call chain are not substitute process/thread
lifetime evidence. Unknown issuing-thread exit retains the lock and borrow;
thread-ID reuse never permits recovery, takeover or unlock.

Branch matrix:

| Branch | Retained disposition |
| --- | --- |
| invalid preowned input before native lock entry | No acquisition asserted; keep supplied/constructor originals; no arena mutation. |
| try returned zero | Keep attempt/clock; no ownership, no factory, no unlock, no retry. |
| try entered without known return | Acquisition unknown; keep entire caller/backing and deny all activation/reconstruction. |
| try succeeded then identity/deadline/factory fails | Keep acquired lock, all effects and absorbing failed state; fixed custody only. |
| allocation shortage/invalid chain/partial split | Keep actual prefix and last valid header/cursor; no shrinking or general allocator escape. |
| source/native/sink/parser/graph failure | Borrow stays installed and lock stays acquired; original body/raw/native outputs survive. |
| native pending/unknown or CPU consumer unsettled | SAME issuing-thread custody service only; no release/close/free or new source transaction. |
| original D expires | Useful work denies; raw settlement/custody observation continues without renewing D. |
| intermediate table/member/ACK/CLOSED returns | No borrow/lock transition; not last-borrower evidence. |
| fixed whole-owner last-borrower reconstruction succeeds | Close future starts first, retain closure original; same issuing thread may once release lock. Source borrow remains permanently closed to mutation/reuse. |
| unlock entered without known return | Keep uncertainty, never repeat; no reclaimed state or admitted next transaction. |

The source-read borrow is permanently absorbing for these retained immutable table
allocations even after known last use. Last-borrower completion permits lock
release only, not clearing the arena borrow for unrelated allocation. Physical
retirement uses the separate genuine native owner path and cannot be inferred.

Allocator coverage is explicit: `arena_alloc` checks before floor scan/split;
`arena_calloc` before dependent allocation/zero; `arena_free` before NULL shortcut,
erase and both forward/backward coalesces; `arena_resize_in_place` before neighbor
merge/erase/split; `arena_realloc` before NULL/zero shortcuts, header scan, copy,
replacement allocation and old free. `arena_init` and fixed bootstrap tail/adopt
entry points reject an already attached caller/borrow, and may not reset it.
Secondary table APIs require actual fixed-factory membership; generic deadline
bind/sink-write entry cannot manufacture the BQ lane. Mutation checks use exact
SAME arena/owner/extent/acquisition/attached original and native thread while held,
not a caller-provided flag. Failure must deny without mutating an unrelated
memory-owner failed bit outside actual serialization.

## Owner-level attachment and ledger closure (CLI36 refinement)

2026-10-12. The attachment must reside in the SAME original memory owner, not
only the arena descriptor. A copied arena descriptor must still encounter the
original owner attachment and deny before touching headers or failed bits. Add
native-only immutable original owner/arena/caller/extent/acquisition geometry,
with an absorbing attached state. These fields are absent from the frozen wire;
all genuine zero/prebirth decoder constructors leave them zero. Keep the actual
64-byte extent stride and existing field-wise wire version. Never reinterpret
old wire bytes as a larger native owner or erase an existing owner to initialize
these fields. Native owner sizeof changes are charged by actual constructors.

The unpublished installer owns the original zeroed descriptor/owner storage and
initialization window before any attachment check. This is a source lifetime
premise, not a general initializer accepting arbitrary uninitialized objects.
Existing-owner entries receive initialized native owner storage only; they may
inspect its immutable attachment, never use a fresh-field read to legitimize an
unknown or reset object. Before carving the caller block, the fixed bootstrap
checks the complete real chain and exact final free-tail geometry, computes the
caller address/size with checked arithmetic, then attaches the original owner
and original arena to that exact not-yet-published caller address. Attachment
precedes caller header/body writes and native lock initialization. All failed
prefixes retain attachment and partially initialized caller/native records.
No generic arena allocator operates between attachment and caller publication.
Only the fixed bootstrap carves this first exact block; subsequent private tail
splits occur in the synchronous fixed constructor after actual native acquisition.
The general allocators always deny for an attached owner, including before the
source-read borrow. They do not obtain permission from supplied thread/token bits.
The private tail splitter is static to the fixed constructor and has no exported
allocator entry or callable cursor. Its direct call chain cannot leave/reenter on
a different thread. This narrows the implementation to avoid a general lock token.

Guard the attached original in every owner mutation before any side effect:
`owner_memory.c` prepare/partial_acquired/acquired/backing_acquired/backing_mapped/
unobserved/prepare_release/backing_view_unmapped/retired, sequence/failure writes,
freeze/continue and prefix reconstruction. Frozen/prebirth shape rejects native
attachment; child destination zero checking and field-wise decoding remain real
constructors, with no native pointer export. Continue must reject either attached
original or attached initialized destination before copying. Prefix comparison
cannot erase or transfer an attachment. Owner mutation denial does not set the
original failed bit outside its exclusive caller. Read-only extent/header checks
can still inspect the original attachment and exact native geometry; they cannot
substitute owner_writable for a mutation permission.

Ledger denial alone is insufficient if a native effect already happened. Actual
platform acquire/release enters prepare before mmap/VirtualAlloc/munmap/VirtualFree;
keep this order and reject attached originals before those calls. Additional
source paths requiring the same pre-native check are source_image_windows.c,
preentry_backing_windows.c/linux.c, node-owner transport_windows.c/linux.c,
writer capture_extent_windows.c and capture_backing_windows.c. Their retained
request/section/view/backing originals cannot be freed or remapped through an
alternative entry. In particular, writer capture backing can reach registration
after native section creation; its entry must deny attachment before that creation,
not rely on late backing_acquired denial. Any entered unknown native prefix keeps
its original raw observation and prevents attachment until settled; bootstrap may
not attach a ledger with an outstanding acquisition/release borrower.

`node-owner/bootstrap.c` directly writes arena floor and arena_initialized in its
prebirth tail adoption. That entry must first reject native BQ attachment, before
either write; ordinary Node owners remain distinct and cannot adopt a BQ extent.
`bootstrap_transport.c` copies decoded frozen_owner by sizeof; accepted frozen
shape already requires zero native attachment. Every native sizeof consumer and
serializer remains part of the coupled source review, including the original
runtime/capture owner allocations. This refinement grants no useful activation:
provider/preinput/root/currentness/all-member and real pending/last-borrower chains
remain closed until their complete actual source relations exist.

CLI36 precode reset repair: general arena_init is a fresh-zero descriptor constructor only, never reinitialization. Check complete descriptor bytes as unsigned char before descriptor/header writes, so supplied different owner cannot overwrite an attached original; no indeterminate owner pointer read. Actual Node prebirth tailadopter likewise checks complete fresh-zero c->arena before clocks/scan/header/descriptor mutation. Existing initialized descriptors remain retained. Actual preentry/bootstrap/launcher constructors must supply their original zeroed owned descriptor storage. No execution/sourceGO.


CLI36 precode factory final-clock repair: charge a fourth native clock stage in original caller before allocation. After all actual control/body/deadline zeroing and full final real-chain/row replay, install permanent source borrow and take fresh retained original-D finish observation. Only known complete tick>=actual Try postclock and tick<original expires may publish storage_prepared/native ownerstate2/result1. Failed/unknown/expired finish retains native acquiredlock, caller/blocks/borrow and ownerstate1; no unlock/retry/reuse. Native dispatch separately needs its actual current gate. Earlier Try clock never supplies preparation finish. Full D/wholecost remain UNKNOWN; no execution.

CLI36 complete-header roster refinement: initial real allocator-chain count P is retained before roster sizing. Exact native roster capacity is P + 16 + 2*(A ranges + unit ranges), including P-1 unchanged prefix headers, every new allocation, and the final free tail. Each row retains original header offset, bytes, requested, next, state and actual payload address. One final chain replay joins every row; failure remains attached and held. Actual sizeof row and full walk/deadline costs remain UNKNOWN; no source admission or execution follows storage preparation.

CLI36 retained row operands: the private constructor captures its exact body row and immediately following deadline row addresses from the monotonically appended roster and retains both on that SAME deadline owner, together with the actual fixed caller. They are native-only pointers, never wire exports or caller-supplied row ordinals. Positive currentness must reconstruct exact current headers/owner/extent/acquisition/native issuing thread and absorbing borrow; pointer retention alone leaves generic IO closed. Native deadline sizeof and all inline-clock placement use actual types; complete finite fit remains UNKNOWN.

### CLI36 per-native issuing-thread operand refinement (source construction; admission remains closed)
The fixed caller's initial GetCurrentThreadId output is not reused as proof of a later executing thread. Before each actual source-table native observation/read/EOF/reobserve, the SAME preowned original capture row must retain its own direct GetCurrentThreadId call/return and DWORD raw result, bracketed by that row's existing original deadline-before clock and one new retained thread-after clock. No generic callback supplies a thread value. The fixed source branch compares this actual returned DWORD against the constructor's actual issuing_thread only after exact SAME caller/owner/arena/extent/acquisition/header/borrow joins. It advances previous_tick to the retained thread-after clock before issuing the native file operation. Unknown/expired/backward clock, zero/different thread or incomplete call leaves the pending capture, in-flight deadline lane, exclusive native lock, absorbing source borrow and all original output storage held; no clearing or second thread attempt.

Native row storage therefore gains a DWORD thread output, called/returned cells and a real slcli_native_clock thread-after observation plus result/returned cells. Every native capture sizeof, raw record payload maximum, body/capture row allocation, sink target/record length/copy/readback/tail and native roster total must consume the changed actual type. The row is selected only by exact SAME body inline allocation arithmetic plus actual next/pending; no caller-supplied ordinal. This check does not establish initial BQ principal/source/provider or native ownership and cannot open the currently closed attached-owner dispatch by itself.

The actual fixed outer A-then-unit entry also needs a distinct preowned one-use native thread/clock observation before A and before unit. Its first clock predecessor is final preparation clock3[0]; the unit predecessor is the actual completed A source finish. Storage failure, authentication absence, thread termination/ID-reuse uncertainty, native pending IO or missing last-borrower closure stays absorbing. No scalar active flag, copied SRW object, source marker, current UID/SID or retained initial-thread equality is authority. These new observations are full original-D/source/native cost inputs; whole fit remains UNKNOWN.

CLI36 actual native primitive initialization closure: file_windows.c observe/read/write/flush/view flush/unmap currently zero the entire capture on native entry. Preserve the pre-native source-thread prefix by zeroing only the native file-observation suffix starting at actual offsetof(original), for every primitive. Genuine capture rows remain fresh zeroed before first use; operation/retained-native-IO guards still forbid reuse. This preserves original source thread observations through real native helper entry and full raw capture copying, without making prefix contents authority.

CLI36 bounded original control range joins: SAME fixed caller retains seven exact source operands (installer, deadline, original origin, original prior clock, A template, unit template, bootstrap) with actual byte lengths. During the ONE final chain replay, a bounded seven-way comparison joins each to its actual LIVE prefix header and checked contained offset within original requested bytes; every operand must join exactly once before publication. Returning storage matcher reconstructs each current real header and exact subject pointer/offset/length, never generic arena scans or supplied row indices. This is storage correspondence only; full nested factory/provider/native/source graph references remain owed and source IO remains closed. All seven native reference structs feed actual caller sizeof and finite cost UNKNOWN.
