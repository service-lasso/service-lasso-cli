# CLI39 product installer / TUI native entry ABI — proposed selection

2026-10-16. PRECODE ONLY. Owner CLI39/PR43. This document requests bounded
independent intent review; it is not an implemented header, constructor, profile,
ROOT admission, tool qualification, or permission to execute anything.

## Actual dependency and compiler selection

TUI head b7f573e55dee7c2640f18e1f4d3da3471145defb supplies a GNU-style C11
plan, not a selected GNU compiler. Its owner confirmed no exact MinGW/compiler/
CRT tuple is selected. `nativeOwnerCompilePlan` uses -std=c11/-include/-c/-o and
-ladvapi32; workflow Go compile is not a native C driver. No Windows generated
entry or pre-CRT issuer exists. The Linux generated same-TU helpers must not be
claimed as Windows source.

Proposed Windows selection is MSVC14.44.35207-family x64 C11 with Windows SDK/
UCRT10.0.26100.0 source, consistent with the existing CLI N2 native actor choice.
This requires a TUI-owned plan adjustment, never passing cl.exe to GNU argv.
Use /TC /std:c11 /O2 /GS /guard:cf /Gy /Gw /Zl /W4 /WX, explicit include/forced
headers and object outputs; link explicit held objects/import libraries, NX/ASLR/
CFG and the independently selected N2 stack geometry. No default CRT discovery,
PATH compiler, automatic library search, or implicit mainCRTStartup authority.
The exact helper/entry object roster and provider closure must be source-built
and held before a candidate can activate; flags do not establish that closure.
This is a concrete source configuration proposal, not an installed-byte grant.

## Two different owners

Build A/B owns compiler descendants, full Node source construction and addon
compile/load. PRODUCT installer owns the installed facade/TUI launch occurrence,
including the entire actual startup/loader interval before the consumer entry.
Neither may obtain authority by borrowing the other's numeric handle or address.
The production launcher remains the source-owned host service selected by HA5,
with its original external observer and prebound streams/input root. There is no
new user launch action, installer enrollment endpoint, or JS capability issuer.

CLI-owned source to add after intent approval:

- native/initial-installer/product_windows.h/.c: parent installation occurrence,
  fixed tool schedule ownership and prelaunch native child construction.
- native/initial-installer/product_entry_windows.h/.c: child-local transport and
  exact pre-CRT source/principal/reservation rejoin, retained failure owner.
- native/initial-installer/product_start_windows.c: fixed source entry and explicit
  GS/helper initialization under the enclosing native owner before TUI consumer.
- native/initial-installer/product_linux.h/.c and product_entry_linux.h/.c:
  parent occurrence and native-only initial stack/descriptor/source rejoin,
  feeding the existing Linux same-TU prepared charge/deadline entry.

TUI owns its generated C11 consumer and build-plan changes only. CLI must supply
real constructor bodies and the linked entry/helper closure. Merely adding these
names or declarations cannot make a producer complete.

## Concrete private native representation to freeze with header

All references below are SAME-process retained originals. None is a wire field.
The proposed `slcli_product_windows_owner` embeds (rather than accepts a boolean
for) these existing actual types:

- `slcli_memory_owner` memory_original and `slcli_arena` arena_original;
- `slcli_source_deadline_original` deadline_original, plus original native clock
  origin/prior/terminal cells and one original expiration;
- separate `slcli_build_principal_windows` effective_original and primary_original;
- `slcli_build_held_file_windows` image_original and fixed source/tool members;
- `slcli_node_transport_windows` section_original, using its actual readonly
  section/native parent custody operations, not reinterpreting encoded pointers;
- actual fixed birth/debug/stream/process-duplicate rows with distinct aliases,
  and preallocated borrower/retirement rows for the whole installed family.

The owner also retains the actual source expectation occurrence
`slcli_bq_provider_expectations`, complete `slcli_bq_owner_resource_profile`,
source-root/member/loaded-provider originals and the acquired backing roster.
These expectation structs constrain observations; embedding them does not issue
source membership, reserve, designation, or admission. The constructor joins the
source-owned actual occurrence to native acquisitions before its first effect.

The child type `slcli_product_windows_entry` owns its *own* memory ledger, arena,
clock/principal/file/loaded-source/native channel observations and typed TUI
consumer storage. No parent's struct is mapped and dereferenced as a child owner.
A child-local operation original and source-domain original are constructed from
the actual installation/birth/channel/source/reserve relation before use; they
are not supplied pointers, digest-derived identities, or defaults.

Private functions, with no implementation selected by this document:
`slcli_product_windows_prepare_original(owner, source_occurrence)`;
`slcli_product_windows_create_original(owner)`;
`slcli_product_windows_rejoin_original(entry)`;
`slcli_product_windows_enter_tui_original(entry)`;
`slcli_product_windows_settle_original(owner)`.
Their exact C declarations/row layouts require the reviewed complete source
operand roster; a forward factory accepting arbitrary callbacks is excluded.

## Child-local handoff and entry order

The parent preowns complete image/stack/TLS/helper/stream/control/failed-overlap
costs and backing before child creation. It holds the exact installed public
image and private payload, source-root, query-profile and entry/compiler helper
members. Birth retains effective-file principal separately from actual
CreateProcessW primary/child token. No CreateProcessAsUser/WithToken substitution.

Only fixed private readonly section routing/handles and bounded locators cross
birth. The child obtains its own native process/thread/token, actual parent birth,
held object/section rights and source/loaded image observations. Raw bytes never
carry a source-domain pointer, operation original, reservation grant, or clock.
All source/native object and actual exclusion joins precede entry permission.
The single source deadline retains the enclosing original origin/expiration;
no reset at CRT entry, receipt, consumer call, timeout or failed attempt.

The Windows entry is an explicit linked source symbol, proposed
`slcli_product_start_windows`, not main/WinMain or ambient CRT startup. Parent
ownership covers loader execution before that symbol. Its fixed native-only
prologue joins child-local originals before GS/helper/CRT-dependent consumer
code. The GS/cookie/helper selection must be explicit source and charged; the
recent compiler-event opaque cookie observation is NOT a product initializer.
No consumer begins while any required loader/helper/source phase is unresolved.
TUI's actual generated consumer receives the SAME entry-owned operation, source,
profile, acquired charge and family membership; it cannot manufacture them.

Linux uses the actual initial-stack/held-ELF/preclaim/clone custody source, not a
Windows transport cast. The child constructs the SAME containing source and
charge expected by `sl_candidate_prepare_owned_entry`, then passes the SAME
preconstructed/enrolled deadline root/sequence to `sl_candidate_enter_owned_outer`.
Current Linux containing_source graph/domain fields alone are insufficient: the
CLI constructor must retain their authentic installation/operation relation.

## Borrowers, failures, cost and settlement

Build executor invocation/failure wrapper remains strongly retained until its
actual returned/failed owner is retrieved. That owner and all tool/profile/entry
files remain held through the actual native family and loaded-provider borrowers.
TUI closeAfterLastBorrower closes files only; it cannot settle that family.

Parent child-end and parent stdin writer closes are separate once-use rows; fixed
empty stdin closes before expecting EOF. stdout/stderr bytes/idle/EOF, descendant
writers, debug exits, process signaling and all duplicate references stay distinct.
A debug EXIT_PROCESS event does not imply kernel completion: acquire a real
process duplicate before ContinueDebugEvent and wait for its signal afterwards.
Do not double-close debugger-system-owned event handles or coalesce aliases by
numeric equality. Every attempted acquisition retains partial/uncertain outputs.

Finite capacities derive from the exact fixed source schedule and full actor/
loaded graph BEFORE effects. Include all native rows, stack/TLS/CLR/Node prefix,
section/readback copies, query/profile/source/helper spans, process/kernel costs,
borrower/close rows and retained failure overlap. No free-memory query or struct
size alone establishes full physical fit. Capacity exhaustion retains the owner;
no retry, kill, finalizer, synthetic EOF, or successful cleanup claim follows.

Required source cases cover foreign source/operation/family, copied parent
pointer, renewed deadline, changed principal/entry/profile/helper, absent reserve,
unknown pre-CRT phase, short/pending channel, alias/overlap, duplicate close,
failed retrieval, late descendant/provider borrower and exit-before-signal.
No fabricated positive native fixture may stand in for the genuine constructor.

Status: compiler/entry choice and this exact private interface await independent
intent disposition. Complete bodies, helper roster, exact installed source/tool
qualification and whole-source review/new ROOT admission remain CLI-owned work.
