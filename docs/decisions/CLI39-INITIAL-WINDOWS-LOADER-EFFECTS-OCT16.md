# Windows A capture and source-effect correspondence — source only, UNRUN

This implements the existing selected initial Windows A contract's ordering:
native complete capture precedes final correspondence. Capture, construction of
the held source/relocation expected vector, and final correspondence have distinct
markers. Import/export/bound/delay observations consume capture and source
expectation; no observation calls a loader, continues a debugger event or grants
initial authority. The original initial admission remains required separately.

The bounded ordinary-IAT effect uses the PE32+ import semantics described in
[Microsoft's PE format specification](https://learn.microsoft.com/windows/win32/debug/pe-format#import-address-table).
The source-selected import descriptor/lookup/name-or-ordinal and IAT offset come
from the same held complete original. Its actual captured slot is compared with
the target derived from a unique actual mapped module base plus the held source
export RVA, joined to that module's independently checked captured EAT entry.
Actual API-set importer/default values and every source/captured forwarder stay
in that same finite route. Neither a caller-provided address nor the observed IAT
value itself supplies the derived expected target.

Only a full address match can explain a changed ordinary-IAT slot. The effect
retains source, route, mapping, exact eight-byte offset, prior source expectation
and independently derived target. Overlap with another binding or relocation
denies. Original unchanged slots need no effect; unresolved/absent targets cannot
explain changed bytes. This source-effect attribution is correspondence, not a
claim that installed Windows loader implementation/source/case is qualified.

Final comparison still covers every image byte. No writable-section exemption,
unknown-byte mask or observed-to-expected copy exists. A separate bitset and
ImportCapacity effect-reference roster are preowned per selected image; managed
effect records and strings belong to the original admitted finite profile. Each
image now has four expectation clocks and four final-comparison clocks. The
whole effect-attribution phase adds four clocks; failures preserve partial
records and the stopped event under the same owner without freeing/retry.

Exact remaining effect/source gaps: load-config security-cookie initialization,
TLS-index/vector/callback initialization, loader-written module/state fields,
bound/delay runtime state, hotpatch/dynamic-relocation variants and any other
mutable byte have no selected complete source/effect rule in this implementation.
Their differences still deny. Installed OS loader creation-case/source history,
initial profile/source construction and the complete continuation graph remain
unfinished. This document does not silently select rules for these effects or
convert their absence into an external factory dependency. CLI owns that source
work. Full events/descendants/streams/natural closure, fixed compile/link/load/B,
Linux and the distinct product-installer owner remain owed. No local target
parser, emitter, native call, compiler, build or test ran; whole source review and
new exact admission still precede execution. The producer guard remains false.
