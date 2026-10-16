# CLI39 complete native table preownership method

2026-10-12. Source construction proposal within selected A/SP1 and the existing
private arena owner. This is not SOURCE GO, execution admission or numerical fit.
The current pairwise table checks and repeated arena scans remain unqualified.

## Actual scale problem

`source_association_native.c` and `source_unit12_native.c` retain one distinct
body/readback/capture allocation and one deadline/preclock allocation per range.
Their pairwise exclusion loops are quadratic in complete table count. Removing
only those loops is insufficient: `slcli_arena_live_block/live_range` start at
the first allocation for every query, and `input_deadline.c` repeats them before
and after each native call. Calling a complete table matcher separately for every
member also repeats the whole domain. Finite loop bounds alone prove no fit under
the unchanged original D. No source members, captures or exclusions may be dropped.

## Fixed owning factory layout

The genuine installer owns a fixed combined A/unit table factory, not a general
callback or caller-defined layout. Before the first source-object IO it preowns
all controls, pointer arrays, range rows, body/readback/capture allocations and
deadline/preclock allocations from the exact independently supplied complete A
and unit geometry. Every allocation and partial effect stays retained on failure.
Actual failure is not converted to a smaller table or renewed reservation.

The factory allocates each body immediately followed by its deadline/preclock
block, in table/range order: all A ranges, then all unit ranges. Logical block
starts and ends are strictly increasing without overlap. Each body has the exact
existing inline output/capture geometry; each deadline block has its exact inline
preclock geometry. Padding/allocator headers are charged, not interpreted as
payload or omitted. Table controls and all borrowed caller/D/prior/sink/factory
objects are excluded against every mutable allocation before any table mutation.
Equal hashes/bytes never collapse distinct allocations or source occurrences.

## One exact allocator walk and original borrow

Add a private, source-owned read-only arena walk over the actual native allocation
chain. It starts at the actual original extent base and checks every traversed
header's size/state/requested/next/bounds/alignment under the existing owner,
extent and acquisition tuple. It advances monotonically once through the entire
relevant chain; no binary search over unobserved addresses or forged pointer table
supplies an allocation. Each exact selected block is joined to its actual body or
deadline object and retained in preowned walk rows. Missing/extra/overlapping or
out-of-order selected blocks deny. Unrelated allocator blocks remain observed,
charged and excluded; they are never erased or silently incorporated.

The SAME arena's existing native serialization must span factory preparation,
complete table IO and consumption, with an explicit absorbing source-read borrow.
All arena allocation/resize/free/coalesce entry points must deny mutation while
that borrow is active; its failed/pending borrowers retain the gate and backing.
No unlocked counter, copied positive flag or decoded source field grants it.
The owning native call path must establish the actual exclusive serialization
before acquisition. The source-read borrow is private existing-owner bookkeeping,
not global state, transferable authority, physical retirement or a new pool.
Returning consumers rejoin the same walk/owner/borrow and actual headers, not a
serialized claim that a walk previously succeeded.

Parent whole-method disposition selects this source construction direction,
including actual factory allocation cost. Repeated allocation search from the
arena floor is itself quadratic for this complete factory: the fixed factory must
retain and advance an actual disjoint free-tail allocation cursor, while every
header/split/charge and partial allocation remains verified/retained. Existing
general Node/arena allocation semantics are not silently changed. Actual exclusive
serialization/issuing-thread establishment is still missing caller work; the
existing arena comment is only a requirement, never evidence of a held lock.

## Coupled checking scope

Preparation, `input_deadline` readiness/before/after, native table returning
matchers and A/unit joining must use the SAME original acquired block relations.
They check the exact current containing block header and own row/tuple in bounded
work under the held arena borrow, rather than restarting a whole allocator walk.
All source/native clock operands, predecessor exclusions, per-range outputs,
short-read rows, raw EOF and readback remain independently reconstructed. Full
table structural replay and every A/unit row correspondence still occur once in
the whole owning verifier. No cached structural result authenticates membership.

After complete native table replay the same genuine owning verifier traverses
every actual member and original Git/non-Git edge. It uses fixed table lookup with
project included and retains raw originals/publication/provider relations. Later
member consumers join that nonpublic SAME acquired owner and current native
borrowers; they do not rerun every table for every bounded window or accept a
Boolean substitute. Genuine source authority and outside nonmutation/native
lifetimes remain separate inputs. No future ROOT/control/self association enters A.

The Windows control sink is part of the same method. Its current per-native-capture
write path walks all body/capture targets to locate one original; replacing arena
lookups alone does not make it linear. The fixed factory preowns exact ordered
body-to-target intervals and each source capture's original containing relation.
Actual native table traversal supplies that SAME relation directly to the private
sink under its retained owning borrow. A pointer/index/positive cache flag alone
cannot select a target. Returning full-roster reconstruction still checks every
interval/target/header/readback once, and independently checks each individual
write's exact source/body/payload relation in bounded work. No unrelated capture
is accepted, no output is reused, and failed sink copies leave original body/native
outputs retained. Actual positive read allowance is derived from source body size,
preowned range payload capacity and unchanged BQ recordMaximum minus48-byte header
and actual native capture sizeof; a logical16384-byte name range uses fresh bounded
native requests as needed. Full current source/review and numeric fit remain open.

## Admission and evidence

Implementation must update the six operative mappings before dependent bodies,
cover all arena mutation entry points and all failure/pending paths, and connect
the actual installer factory/caller. A standalone cursor/parser patch is not a
complete delivery or review freeze. The constructor has no useful default
principal/instance; independently selected complete initial inputs remain required.

Whole source review and NEW authentic ROOT/exact-command admission precede any
target execution. Required evidence includes full actual source scale, raw native
reads/captures and original-D timing, all distinct allocation/header charges,
failed-prefix storage retention, mutation denial while borrowed, malformed chain
and alias negatives, actual last-borrower closure and measured complete B0/B2/R.
No new quota, wider deadline, compressed/sample source or skipped edge is proposed.
