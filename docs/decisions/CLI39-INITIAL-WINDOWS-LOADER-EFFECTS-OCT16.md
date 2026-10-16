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

## Selected actual raw candidates, inspected without execution

Administrative full-byte/hash and bounded PE-directory reads of the installed
files on 2026-10-16 establish the following required-case candidates. These are
not native held inputs, mapped images, linked-object ancestry or qualification.
The actual compiler path is under `C:/Program Files (x86)/Microsoft Visual
Studio/2022/BuildTools/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64`, not the separate
`Program Files` path. The compiler/linker hashes agree with the retained
CURRENT-TOOL-ARCHIVE-OBSERVATIONS.json. No executable was invoked or copied.

| File | Bytes | SHA256 |
| --- | ---: | --- |
| cl.exe | 677736 | 88c8344236a27a6e727e0a8edc49aaa2690bdc7a9464b9d18cc7abe70a9f1c0d |
| link.exe | 3252576 | ca11e6c45debd34bf652dfe984c5360a531a005ed78bf72852330c9c2590cf0d |
| c1.dll | 3430200 | 48bb561939cfd8fbf194bb73b111b2083912da4fb81af2c181cbe8f4b89e5ad1 |
| c1xx.dll | 12332384 | 688fe28f3065283edf668868e76aeab28f03f80e2171c14c7d4e37f64513f623 |
| c2.dll | 10462008 | 98050f97d60f7e95fcaf3525a7103e4818fba498a43563922476819ce88ed162 |
| mspdbsrv.exe | 186248 | 9221dbe740f39a1059dfe6e917984effad6f97312636f001c9191a6bccb34f93 |
| vcruntime140.dll | 124272 | b23a5c9ee132fbb72d970165de052f332209992eb4568616b7802052edc4f398 |
| vcruntime140_1.dll | 49592 | c3feb5700b1cbe6ace93e64e286b6394970e52d36b3b643d8243a1771077d210 |
| msvcp140.dll | 557496 | 20076bc0e3fcb1842cab77dfe8ef816bc099b887c25329847a83e71c98765639 |

| File | TLS RVA/bytes | Raw TLS template bytes | TLS index VA | Load-config RVA/bytes | Cookie VA | Delay RVA/bytes |
| --- | --- | ---: | ---: | --- | ---: | --- |
| cl.exe | 588416/40 | 12 | 5369367436 | 417424/320 | 5369356352 | 631616/96 |
| link.exe | 2681984/40 | 8 | 5371759000 | 2477392/320 | 5371727936 | 2995844/224 |
| c1.dll | 2894464/40 | 8 | 6445740004 | 1705008/320 | 6445711424 | 3246016/96 |
| c1xx.dll | 10984192/40 | 8 | 6454214868 | 9286208/320 | 6454132800 | 11662136/192 |
| c2.dll | 9390720/40 | 97568 | 6452520232 | 8134304/320 | 6452420672 | 9946772/224 |
| mspdbsrv.exe | 140032/40 | not expanded | not expanded | 139616/320 | 5368873024 | absent |
| vcruntime140.dll | absent | — | — | 89360/320 | 6442553408 | absent |
| vcruntime140_1.dll | absent | — | — | 25200/320 | 6442483776 | absent |
| msvcp140.dll | absent | — | — | 356224/320 | 6442972160 | absent |

All nine raw candidates have an absent ordinary bound directory. All five
expanded TLS directories have zero SizeOfZeroFill; their callback-array VAs are
respectively 5368994704, 5371148024, 6444006400, 6451523312 and 6450434104.
Nonzero pointers do not identify callback source ancestry by themselves.

Raw delay module names are cl: VERSION.dll, PSAPI.DLL; link: pgodb140.dll,
imagehlp.dll, mscoree.dll, ole32.dll, OLEAUT32.dll, msvcdis140.dll; c1: XmlLite.dll,
mscoree.dll; c1xx: XmlLite.dll, mscoree.dll, bcrypt.dll, OLEAUT32.dll, TLBREF.dll;
c2: ole32.dll, dbghelp.dll, OLEAUT32.dll, mscoree.dll, msobj140.dll, pgodb140.dll.
These are declared possible delayed dependencies, not observations that each
executes during the fixed C11 schedule. Required actual delay transitions must be
selected from real source/case and traced; an absent bound directory in these
files does not establish absence in the whole eventual loaded closure.

The System32 candidates ntdll/kernel32/KernelBase/ucrtbase also have nonzero
load-config cookie fields; KernelBase additionally declares TLS. Their full-byte
SHA256 values are respectively
651ac9041af8e6b5ebd331d53163244c0a27d36d1433f0f15cac1f28c5f2e82c,
e132eba4166256e2f414b91fe7d5386590bc5ea8626aee5af9c9738383cf2ae1,
5c29b73622ce0976e1ada566f31f2197ba0f8beaea89c3ec033366126172c3d0,
dedb7611e1abc1ff3852dcac3d4c93a048d729e25b061973b9969f22698f14ca.
This is not a claim that these directory candidates are the actual child modules
or the complete selected closure.

Concrete installed MSVC source candidates under the same 14.44.35207 directory:

| Relative source | Bytes | SHA256 |
| --- | ---: | --- |
| crt/src/vcruntime/gs_support.c | 5723 | c2491ccc54630fa30a7484bdf5da9481822f9e8597e8775540975a1b573e49fe |
| crt/src/vcruntime/tlssup.cpp | 2425 | 9da99f37299cebf3c4685ae16f3f5dc997bf2dd1f08034fb5a9ddf18eda43d8f |
| crt/src/vcruntime/tlsdyn.cpp | 5380 | ff55ef3bea7c74cec3bbc14e573622d41d7a94351e462461fabb6d0871c1f395 |
| include/delayhlp.cpp | 15070 | d31b06279d54f0cf103b03f898391c594c8a1ef0c90d34e6cc2977d2fc4e6912 |

gs_support.c has a real preinitialized-cookie branch that preserves the cookie
and sets its complement, or a fallback based on actual OS time, PID/TID, tick,
QPC and address entropy. tlssup.cpp supplies initial zero _tls_index and its TLS
directory/callback association; tlsdyn.cpp supplies dynamic TLS callbacks.
delayhlp.cpp consumes concrete RVA descriptors/INT/IAT/module-handle state and
performs actual delayed load/get-procedure/atomic handle writes, with hook and
bound/unload alternatives. It does not imply those optional hooks are selected.
These available sources are a basis for implementing selected CRT cases, not
proof that every proprietary compiler/system DLL is linked from those exact
objects. Actual linked/source/creation-case joins remain required.

Therefore TLS and cookie handling are required selected-case implementation;
delayed imports require genuine stage-specific treatment. Refusing unknown
variants alone cannot finish the selected build. The missing loader-randomized
cookie original cannot be supplied by copying a post-creation observed value
into the expected image. Its exact creation-effect/source basis must be joined
to the actual occurrence. That implementation/initial-admission boundary remains
explicit, without inventing a caller-provided mystery original or broad mask.
