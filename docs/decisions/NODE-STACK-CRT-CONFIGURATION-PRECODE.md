# Whole N1 stack/CRT supplement candidate S1

Status: prospective source architecture candidate, awaiting a different entire
review. It does not alter the original N1 candidate or its source-only GO.
No code, compiler, import, build, Node process, test, native action or image
admission is authorized by this document. The complete product remains
unfinished. This candidate chooses configuration explicitly so that review
can assess it; these settings are not inherited permission from N1.

## Original source and concrete selection

Keep original Node22.23.2 commit
`aa4c77582be995286fc6e00aaf530dc7ade102a9`, esbuild0.28.2 and
postject1.0.0-alpha.6. The original complete Node source, genuine Git object
bodies and all47,300 associations remain in the accepted N1 input custody.
`NODE-STACK-SOURCE-COUNTEREVIDENCE.md` binds the original Windows executable
8MiB main stack and the original Release `/MT` versus `/MD` selection.

S1 chooses an instrumented, statically linked source-built CRT for both
approved x64 targets, with these exact prospective source settings:

* Windows Node executable main stack reservation: 1,048,576 bytes. Change
  `node.gyp` executable VCLinkerTool `StackReserveSize` from `0x800000` to
  `0x100000`. The held final PE header/native initial stack observation must
  agree; changing the source number alone is not actual allocation evidence.
* Each default Node platform worker and its delayed-task scheduler stack:
  1,048,576 bytes. Change both explicit creation sites in
  `src/node_platform.cc` to the original owned `uv_thread_create_ex` route
  with `UV_THREAD_HAS_STACK_SIZE`, instead of inheriting ambient defaults.
* Node platform pool: exactly one worker plus its existing one delayed-task
  scheduler. Change `PerProcessOptions::v8_thread_pool_size` from four to one
  and make this configuration source-fixed before options parsing. Neither
  `--v8-pool-size` nor another argv/environment value can replace it. The
  normal platform task queue and scheduler remain, with real owned native
  synchronization, timers and failure captures; no success-only substitute.
* libuv global work pool: exactly one worker with 1,048,576 reserved stack
  bytes. Change `deps/uv/src/threadpool.c` `init_threads` from its four-entry
  default/`UV_THREADPOOL_SIZE` selection and explicit `8u << 20` stack to
  this source-fixed one-worker/`1u << 20` choice. The environment override,
  expansion allocation and fallback pool selection cannot change this image.
* V8 x64 normal JavaScript stack limit: 864KiB, source-fixed before isolate
  initialization. Change the selected x64 `V8_DEFAULT_STACK_SIZE_KB` branch
  in `deps/v8/src/common/globals.h` from984 to864; do not set a runtime flag.
  The original ARM/IA32 branches already use864, but that precedent does not
  prove x64 native stack sufficiency. Preserve the original compilation
  reserve and deoptimization slack checks and assess all actual stack paths.
  Change the selected `flag-definitions.h` stack-size declaration to the
  original `FLAG_READONLY(INT, int, ...)` mechanism: a default alone would
  still permit argv/V8 flag mutation to replace the admitted limit. Node
  options/NODE_OPTIONS and runtime flag APIs cannot change this constant.
* Linux main stack: the trusted original native launcher reserves/classifies
  the initial kernel startup stack before source-owned release. Heap-free
  pre-CRT startup authenticates its original owner, allocates an actual fixed
  1MiB native main-stack mapping with separately charged guards/TLS, and
  switches the original x64 stack pointer through its source-owned entry
  assembly before CRT/constructors/Node. Original argv/environment/auxiliary
  data is copied into prospectively owned bounded native storage only after
  owner authentication. Retain the old startup mapping until every original
  pointer/readback obligation has ended, then record its actual native unmap;
  otherwise retain its full charge. Main execution cannot rely on implicit
  grow-down outside this original fixed arena. A lower pre-launch RLIMIT can
  bound the temporary kernel allowance, but is not physical proof or an
  allocator. Neither startup nor main stack is a second budget, and no
  existing process or CLI environment switch selects this allowance.

These choices reduce concurrency and JavaScript recursion headroom. They do
not claim equivalence to an unchanged stock Node image. The public CLI/library
API signatures, native-v3/host/inspection/proof grammars, original deadlines,
rights, caps and original-parent copy retirement remain unchanged. The
complete semantic/native fixtures must cover those compatibility consequences.

Additional explicitly requested stacks are not silently clamped to1MiB.
Node Worker resource limits, Wasm stack switching and every other thread/stack
creation must either receive complete original prospective ownership for the
actual requested/reserved region or fail through the actual owning error path.
Any feature/configuration change beyond those enumerated here returns for its
own whole source decision. Unadmitted native modules cannot add allocator,
thread, image or source authority.

## CRT source choice and pre-initializer owning entry

Windows prospective CRT source inputs are the original installed
MSVC14.44.35207 `crt/src` and UCRT10.0.26100.0 source bodies. All991 actual
original files were copied and size/SHA256 reread before modification, at
`D:/projects/service-lasso/_audit/cli39-n1-stack-crt-supplement-precode-oct05-01/original-windows-crt/`.
The complete original source-association manifest SHA256 is
`5eac0d40421884809e31e5e4f5371791687c8e3d4b7cd4201ae14909fac73c5f`.
These original source inputs are candidates, not an admitted compiler, CRT
library/image, SDK, installation or grant. No compiled import library is
provided by this capture. The full eventual source/build/tool input ROOT must
include every actually used original header, assembler, library and tool.

The original `vcruntime/exe_common.inl` invokes security-cookie initialization,
then `__scrt_common_main_seh`, CRT initialization, `_initterm_e`, `_initterm`,
dynamic TLS initialization and only then `invoke_main`. Its `tlsdyn.cpp`
explicitly distinguishes early PROCESS_ATTACH from later THREAD_ATTACH.
Therefore S1 selects a source-owned CRT entry preceding all source-owned
allocation/static initialization and parsing, not a hook in `wmain`.

The entry must preserve security-cookie initialization and guarded exception
semantics. Its pre-CRT bootstrap is bounded, heap-free, without C++ constructors
or allocating diagnostics, using original native leases and a complete private
allocation owner authenticated under PA1/N1. It cannot initialize source trust
from argv, environment, a path, PID, constructor, pointer or boolean. The
original trusted launcher reserves/classifies the initial source image,
stack, TLS and native startup objects before releasing source-owned execution;
bootstrap rechecks those originals before later CRT/Node input processing.
An unknown pre-entry allocation or TLS callback retains the original attempt
and denies positive activation, rather than labelling it OS/foreign overhead.

Build the selected Windows executable/dependency CRT closure statically under
the original non-shared Release `/MT` branch; no `/MD` fallback or separately
loaded source-owned CRT can initialize outside this owner. Modify all UCRT
allocation/reallocation/aligned/recalloc/free and failure/new-handler routes
and all direct owned HeapAlloc/HeapReAlloc/VirtualAlloc paths to the real
native reservation/arena owner. This includes CRT argv/env/locale/stdio/thread
state and startup/exit/TLS bookkeeping. `__acrt_heap` currently selecting the
ambient process heap is not sufficient. Actual original heap/native regions,
metadata, requested versus reserved bytes and release must be recorded.

Linux prospective CRT is source-built static musl1.2.5, rather than assuming
unobserved ambient glibc/loader allocations can be hooked. Original official
tag/commit `0784374d561435f7c787a555aeab8ede699ed298`, root tree
`2deb5f7c62d8c9e9733c9ed77d9210b708bbb69e`, and all2,932 original full-tree
associations were captured. All2,708 unique raw Git bodies, including commit
and root, were independently verified with genuine object headers and actual
physical SHA256 at the same external supplement custody. Association manifest
SHA256 `dbe25f5cd0d7fed6b2259a52ac7f97cd84cd37ebb6df82c21fea75c36cf0bce8`.
The repository/tag/source capture is not native source or image authority.

This is an explicit Linux libc/static-link/packaging compatibility change.
S1 does not infer this change from stock Node's musl comments. The selected
source-built entry must authenticate the original startup owner before
`__libc_start_main`/`__init_libc` scans argv/environment/auxv or initializes
TLS/constructors. Instrument `__init_tls`, mallocng metadata/brk/mmap paths,
pthread creation/guard/TLS/native stack paths and all direct mmap/mremap/munmap
owned routes. The real source-built C++ runtime/exception/atomic/unwind closure
also uses this owner; static libc alone does not establish coverage. Initial
kernel image/stack/auxiliary vector custody is classified by the retained
trusted launcher; later source processing cannot replace it with a snapshot.
No dynamic loader, compiler/source pin or new authority is invented here.

The final original musl/C++ runtime/tool source/build tuple is separately
admitted. Native library/addon interoperability with a stock glibc process
cannot be asserted; only the independently admitted instrumented Node image
and built-in original CLI binding may activate this route. The generic Node
version string does not admit the changed image or allocator ABI.

S1 explicitly selects source-built static GCC14.3.0 libstdc++/libsupc++/libgcc
and required libatomic/unwind runtime closure for Linux. The prospective
original official tag is `releases/gcc-14.3.0`, tag object
`bb24b4c804f3d95b0ba95b74965bd04f425f8467`, peeled commit
`c9cd41fba9ebd288c4f101e4b99da934bcb96a11`, root tree
`5fd546268e6e883acbff512321d98dc1ee93233e`. All141,173 original full-tree
associations and all140,046 unique raw Git object bodies, including tag,
commit and root, were genuinely header/hash and actual physical-byte verified
in the external supplement packet. Complete association manifest SHA256
`1b6b425d5ca476952a67007ed7848b294910bccca1ad47275d52af44a5f924cf`.
This selection is a source/build/runtime
compatibility change, not approval of an installed GCC/compiler/image.

The original `libstdc++-v3/libsupc++/eh_alloc.cc` emergency exception pool may
parse `GLIBCXX_TUNABLES` and allocate at static initialization. S1 selects its
existing `_GLIBCXX_EH_POOL_STATIC` branch, with the original fixed x64 threaded
default object count256 and object-size factor6. Its complete static reserve is
the original formula `256 * (6*sizeof(void*) +
sizeof(__cxa_refcounted_exception) + sizeof(__cxa_dependent_exception))`,
including native alignment/mutex/free-list/image metadata. The actual target
layout and complete image mapping are charged and independently qualified;
no guessed byte size or separate emergency budget is assigned. Environment
tunables cannot change it, and the static pool is not an uncharged fallback.
Instrument ordinary exception allocation, overflow/alignment, termination and
release alongside all C++ allocation/unwind/thread/runtime paths. A source
failure capture cannot allocate an unowned exception or diagnostic after OOM.

## Complete one-domain ledger and prospective fit rule

Use the original8,388,608-byte primary/SEA or Node-client domain, with no
hosting quota, second bootstrap domain or relabelled runtime/image category.
At full startup with both pools present, the four selected stack reservations
alone sum4,194,304 bytes: main, platform worker, delayed scheduler and libuv
worker. This is a source reservation lower bound, not actual total usage. The
exact original native page/granularity/guard/TLS/header allocations can make
the measured total larger; they must be charged at their real sizes.

The original ledger accounts these complete disjoint lifetime categories:

| Category | Original owning allocation and prospective charge |
| --- | --- |
| Source image/code/data | Complete actual owned native image mappings/reservations, including static CRT/C++/Node/V8/libuv/OpenSSL/ICU/zlib and private binding; original image bytes and section identities retained |
| Main/worker stacks | Each real reserved stack region, guards/growth and thread native object; no requested-byte or committed-only substitute |
| TLS/CRT startup | Actual TLS/native thread state, argv/env/locale/stdio, loader/relocation/startup/exit and initializer metadata in their same original owner |
| Runtime heap/pages | CRT/C++ malloc/new/realloc/aligned, V8 heap/zone/code/data/trusted/native metadata, Node containers and dependency allocations; actual full page/arena reservation |
| Backing stores/copies | Buffer/ArrayBuffer/external backing, public/internal immutable/mutable copy sets and their wrappers/properties/strings; complete prospective copy set before any returned allocation |
| Native IO/control | Original channel/socket/pipe/request/OVERLAPPED/event/timer/crypto contexts, buffers and actual pending/late/cancelled lifetimes |
| Ledger/evidence | Fixed native owner records, event/copy ledgers, original raw capture/readback/scratch/failure records and allocator metadata; exhaustion retains rather than dropping observations |
| Additional stacks/images | Actual explicitly requested extra thread/Wasm/module regions; same finite owner and no default fallback allocation |

Every original region is associated with one actual owner and native identity,
while conservative copy retention may charge bytes longer than physical local
use. Overlap/aliases are independently checked: one mapping cannot stand for
two originals, and an actual owned region cannot disappear from the ledger.
Uncertain attribution denies positive admission. Original bytes, raw native
observations, sizes, source associations and retained lifetimes are preserved.

The fit condition is the complete native sum of all these categories <=
8,388,608, with every prospective reservation preceding allocation and every
real allocation result correlated. After the four chosen stacks, at most
4,194,304 bytes remain for **all** other categories and any additional actual
stack/guard/TLS overhead. This is a finite remaining bound, not a claim that
the unchanged or modified Node image/runtime fits it. A final image larger
than the remaining bound denies before releasing source-owned input execution.
No image-file size, working set, requested malloc count, GC target, addon
counter or small public record proves that fit.

Original bootstrap calibration/reservation and C bodies must be independently
authenticated before parsing subsequent untrusted data, under the same
PA1/N1 authority and original consumer scope. Existing closed C fields and
raw-evidence/source associations are unchanged. Runtime/image/stack/IO event
storage itself consumes this same owner. No measured value is guessed or
enrolled by a body hash. Actual CRT/compiler/source image/bootstrap/provider/
caller/ROOT/HA2 key/catalog/calibration are still unestablished.

## Allocation, overflow and failure implications

Before any multiplication, alignment, page rounding, stack/guard/TLS addition,
pool/array growth or native size conversion, check overflow and the complete
remaining owner capacity. Windows original libuv rounds stack size and passes
it to `_beginthreadex`; S1 must explicitly use reservation semantics and
correlate the actual returned suspended thread stack before release. Neither
page rounding nor a successful thread handle proves requested==reserved.
Linux pthread stack/guard/TLS allocation must likewise be measured from the
original source-owned mapping, not `RLIMIT_STACK` or `pthread_attr` alone.
The fixed main-stack switch preserves SysV x64 alignment, red-zone, unwind,
signal/alternate-stack and libc stack-base metadata semantics. The original
temporary kernel stack is an explicit additional startup lifetime/category;
it is never excluded because later execution switched stacks. Any signal or
callback alternate stack is separately prospectively owned and charged.

No fallback to ambient allocators, shared heaps, a larger pool/default stack,
extra worker, unchecked JIT/code mapping, unadmitted module, allocating failure
message, repeated constructor or new native owner is selected. Unknown native
allocation/close/IO completion retains originals/charge and denies positive
activation. Existing request/phase/absolute deadlines are enforced by their
original owners, not weakened because concurrency is smaller. No allocator
exhaustion, native stack fault or fatal CRT abort is relabelled normal-zero.

Returned copies remain conservatively charged until the authenticated original
Node physical exit. Actual native arena/data release does not erase retained
metadata or future observer/archive obligations. GC/finalizers, JS dispose,
module reload, detached buffers and an operation success cannot retire that
original parent. Source-owned control/service accounting remains its original
separate aggregate1MiB; no part of this Node domain is pushed into it.

## Whole alternatives and required review/qualification

The stock Windows main8MiB and libuv worker8MiB configurations cannot satisfy
the selected full8MiB domain with any other positive owned category. Retain
that exact source counterevidence, without claiming universal Node/RSS
impossibility. S1 is a concrete complete configuration candidate whose actual
fit and stack safety are unproven. Source-only architecture GO may permit its
implementation; it cannot supply successful native calibration or authority.

The complete alternative remains N2: a separately reviewed original native
embedding entry with full CRT/runtime/allocator/stack/source/image ownership
and unchanged public API/wire/caps. N2 does not obtain a cap waiver or automatic
fit merely by changing its entry. If complete actual S1 source/image evidence
proves excess or unsafe mandatory stack requirements, retain the full exact
counterevidence and return the entire viable source alternatives for an
explicit decision. Do not silently prune API/runtime ownership categories.

Required full source/native-path fixtures include pre-CRT/TLS/static/input
allocation, all stack/thread routes and alias/overflow/exhaustion, V8 native
compilation/GC/JS stack overflow, libuv async work/cancellation/late IO and
pool saturation, OpenSSL/ICU/zlib/image/native allocation closure, all returned
copy sets and original-parent retirement, semantic derivation/native writer/
inspection/bootstrap/provider/catalog/observer/archive lifetimes and all
existing TC01..12/CA01..08 paths. Their source must exist before final source
review; none is executed or labelled PASS in this candidate.

The different entire architecture review must inspect this complete decision,
original Node/N1 history, both complete CRT source custodies, all original
input associations and owning target/compatibility/ledger/failure changes.
After any acceptance, actual implementation still requires the complete
different final product SOURCE GO/new input ROOT before compilation, imports,
tests or native qualification. Independent command/observer/provider/ordinary
entry source work continues while this supplement is pending.
