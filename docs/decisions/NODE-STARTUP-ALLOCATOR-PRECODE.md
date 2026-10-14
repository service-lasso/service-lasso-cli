# Complete Node startup and source allocator decision candidate N1

Status: PENDING DIFFERENT ENTIRE ARCHITECTURE REVIEW. This is a concrete whole
source implementation candidate, not SOURCE GO, an admitted Node image, a
runtime/native-input grant, a quota waiver or executable acceptance. PA1's exact
source-only decision does not authorize this additional Node startup consumer,
private owner ABI or changed runtime image/packaging relationship. No Node,
V8, libuv, OpenSSL or compiler code has been executed or modified here.

## Original source and exact existing requirement

BLUEPRINT003, Original API/local copy accounting, requires an original
source/package instance and authenticated actual Node parent, complete native
prospective reservation before every returned/internal Buffer copy and all
record assembly, immutable snapshot, paths, metadata, tokens, framing, UTF8 and
allocator overhead. Each binding remains 8388608 bytes, copy sets at most128,
with conservative charge until that original parent's actual physical exit.
Its finite independently admitted client capacity and at most16 host bindings
are not additional per-call pools. Shared primary/SEA/writer remains one8MiB
source domain; service control remains one aggregate1MiB domain; durable
store/transaction/scratch custody remains4MiB, not extra service RAM. Runtime,
stack, IO and image custody must have coherent original native ownership. This
contract does not define or prove a universal bound on every OS process's RSS.

The existing native/library-client arena/copy ledger, externally created
Buffers, TypeScript counters, prospective calibrated constants and a memory
statistics snapshot do not cover all Node/V8/libuv/crypto/startup allocations.
An addon cannot retroactively replace allocations already made before load.
The current nativeAccounting Go declarations likewise do not complete native
runtime ownership. These deficiencies remain visible; none is an accepted
native receipt or a reason to permanently replace the positive API with empty
catalogue behavior.

Original source is Node repository https://github.com/nodejs/node.git, exact
annotated tag v22.23.2 object490a9fef8f8adcda5a95bd6f96035b05cb43fe5b,
peeled commit aa4c77582be995286fc6e00aaf530dc7ade102a9. Original tag/commit raw
bodies and all47300 path/type/mode associations are retained externally. ALL
41818 unique original tag/commit/tree/blob bodies,518716950 raw bytes, were
physically captured and independently checked against genuine Git object
header SHA1 and actual physical raw SHA256/size. Raw verification manifest
SHA256 2a7a8d726fed716348824304efbc02e6420654b2c28471cb160645e0c4d6600d.
Custody: D:/projects/service-lasso/_audit/cli39-node-startup-allocator-architecture-precode-oct05-01.
The tag, annotation, origin URL and matching hashes supply source custody, not
issuer trust, an independently admitted image, qualification or provisioning.

Exact original source routes, present in that complete object set:

* src/node_main.cc:57..91 allocates Windows argv with C++new before node::Start;
  the Unix entry calls node::Start directly. Installing after this conversion
  already misses source-created argument storage.
* src/node.cc:1112..1130 creates result/vector/string state, reads debug
  environment, calls PlatformInit and argument initialization before V8 init;
  1570..1579 calls uv_setup_args and creates argv strings before initialization.
  A JavaScript preload or addon is later than these actual owners.
* src/node_main_instance.cc:40..49 creates ArrayBufferAllocator/CreateParams and
  assigns the allocator before NewIsolate. src/api/environment.cc:109..136
  allocates/frees through that allocator;339..340 initializes V8 after platform
  registration. Its ArrayBuffer count is one component, not complete runtime.
* src/node_buffer.cc:402 onward copies via V8 backing stores;479 onward also
  permits malloc-owned/external stores. Their data and V8 wrapper/metadata are
  separate original allocations. A backing-store counter misses the latter.
* deps/v8/src/heap/memory-allocator.cc:35..41 retains separate data/code/trusted
  page allocators;129 onward reserves virtual memory. deps/v8/src/init/isolate-
  allocator.cc owns isolate/cage pages. deps/v8/src/utils/allocation.cc and
  deps/v8/src/base/platform/memory.h own native malloc/calloc/realloc/new paths.
  Platform Windows/Posix files contain actual VirtualAlloc/mmap/commit routes.
* deps/uv/src/uv-common.c:116..132 replaces libuv malloc/realloc/calloc/free only;
  it does not own V8/C++/OpenSSL heaps or OS thread-stack reservations.
* deps/openssl/openssl/crypto/mem.c:60..72 accepts CRYPTO_set_mem_functions only
  while allow_customize is true. First crypto allocations close this route;
  installing after import or native connection authentication can be too late.
* deps/uv/src/threadpool.c:232 requests an8MiB stack for EACH worker; Unix/Windows
  thread implementations page-round/reserve thread stacks independently of
  ordinary malloc. src/node_platform.cc creates background worker threads too.
  If even one stock8MiB worker stack belongs to an8MiB source domain, it leaves
  no room for other positive source-owned state. This is an exact conditional
  source counterexample, not a measured whole-process RSS/minimum claim.
* src/node_sea.cc separately serializes plain source, optional snapshot/cache,
  assets and exec arguments. Build/runtime matching cannot be inferred merely
  from both executables printing the same Node version.
* scripts/package-native.mjs currently checks Node22.23.2, invokes process.execPath
  for SEA generation and copies that executable before postject. The current
  file cannot select a reviewed instrumented image by version alone. It must
  not quietly copy a different current process image or an ambient binary.

## Exact proposed choice N1

Implement a source-built Node22.23.2 native startup/owner integration against
these exact original source bytes, with the complete patch and original build
source in the full source packet. Keep Node22.23.2, esbuild0.28.2 and
postject1.0.0-alpha.6 versions; do not call modified bytes the unchanged stock
image. Changes include the Node executable/image digest, source/build inputs,
private native owner ABI, allocator binding and packaging provenance. Every
actual image/source/tool/calibration pin remains separately admitted after
proper review and native qualification. No actual pin exists in this decision.

The source-owned library/facade/SEA allocation domains are instrumented from
native startup, before any untrusted argv/environment/module/JS/body/config.
Every allocation has a concrete original native owner/category/lifetime; every
source-owned byte in the selected domain is charged. Pre-existing foreign OS
allocations cannot be relabelled source-owned to claim universal RSS, nor can
source-owned allocations be relabelled foreign/shared to evade the caps.
Uncertain attribution, startup coverage or physical release denies positive
qualification and retains the actual original observations. No new hosting
quota or implicit runtime grant is invented to absorb an unexplained baseline.

Runtime fit is not asserted. The complete reserved arenas/runtime/stack/IO/
image-custody/metadata mapping must fit the existing selected source domain
with actual original calibrated/native peak and release evidence. If complete
actual source establishes unavoidable excess, the exact source/clauses/full
alternatives must return for an explicit architecture decision; this proposal
cannot waive or raise a cap or silently exclude a category.

## Bootstrap before untrusted input and original authority

A native source-owned bootstrap executes before Windows argv conversion and
Unix Node::Start/uv_setup_args. Its complete source also covers CRT/static
initializer allocation routes that precede these ordinary entries, and records
OS-created stack/image/native-object custody under the original independent
native-input owner. Moving only the main-function line is insufficient. No
constructor, LD_PRELOAD/DLL injection, user-selected allocator, environment
switch, ordinary JS preload, PID/handle argument or claimed profile hash can
install the owner or select an image/resource.

N1 adds a private original-source admission consumer for the Node native
bootstrap under the SAME independently admitted native input authority as PA1.
This is an explicit extension of PA1's service-bootstrap-only consumer boundary
and requires this NEW different ENTIRE decision. It does not add an issuer,
signer, key, provider owner or independent trust root. The provider B must still
be genuinely independently source/image/owner/native-input admitted and may
not embed eventual Node/service/P/G/C hashes or its own final commit/ROOT.
No currently provisioned B/caller/calibration/image/ROOT is supplied here.

The original trusted native launcher/caller, provider image/owner/birth and
original source-packet/P/G/C/bootstrap-calibration leases are retained before
untrusted input. The actual producer/caller/provider/consumer implementations
and original association checks are part of the full source implementation;
an opaque getter or constructor is not authority. Actual original Node image
bytes are hashed from the held native image and compared with independently
admitted original P/image/source objects; Node does not embed its own final
hash. Circular compiled P/G/C/own-ROOT associations remain prohibited. The HA2
ledger key stays service-only original provider custody; the Node bootstrap
cannot obtain it or convert a ledger MAC into profile/catalogue authority.

Native bootstrap calibration/reservation must be independently authenticated
before reading untrusted later C bodies. Bootstrap, parser, capability,
metadata and captures use the SAME eventual domain/owner; no second8MiB client
or primary domain and no second1MiB service account. Once-only admission,
original leases and failure charges never reset on retry/reconnect/package
reimport. Positive activation needs all actual original source/native/image
associations, not merely a version check or signature-shaped JSON.

## Complete allocation hooks and ownership

The private built-in native owner ABI is internal to the reviewed Node image
and CLI native binding. It exposes original retained owner/arena association,
complete prospective allocation reservation, actual native allocation result,
copy-set ledger publication and terminal original-parent observation. It has
no JS setter, public constructor, raw-pointer/PID grant, alternate inspection
frame or replacement handle. Arbitrary package instances cannot manufacture an
owner. ABI version/source/image mismatches deny before source-owned input IO.
The existing public API and private host/inspection/native-v3 wire grammars
remain unchanged. This private source-bound getter/state layout and startup
consumer are additional reviewed interfaces, not inherited permission from PA1.

Required coverage is a complete source closure:

1. Node/C++/CRT malloc,c alloc,realloc,aligned-new,global-new/string/container/
   exception allocations, including static initialization and failure paths.
   Checked overflow and complete prospective source-category reservation
   precede actual kernel/allocator calls. No dynamic diagnostic allocates after
   exhaustion. Actual mapped/reserved native arenas contain measured metadata.
2. V8 ArrayBuffer data, external backing stores, Buffer copies, wrapper/header/
   UTF8/path/token/WeakMap/array/string/object metadata, native zones and code/
   data/trusted pages. NewIsolate receives the real source-owned page and buffer
   allocators BEFORE initialization; later getters cannot replace them.
3. V8 sandbox/pointer-compression/address reservations and commit/decommit:
   record actual regions and full ownership, including no-access reservations
   and their metadata. Do not count only a convenient committed-page statistic.
   If stock large reservation/worker defaults cannot fit the selected source
   domain, explicitly review the exact source-fixed configuration/ABI changes;
   no runtime V8 flag or RSS observation substitutes for that decision.
4. libuv complete allocator callbacks before first allocation, loop handles,
   timers, pipe/socket IO, request buffers, queued-work context and child/worker
   thread native objects. Existing stricter deadlines and private stream EOF/
   original native IO ownership stay unchanged. No thread receives an uncharged
   stack because its malloc callback was hooked.
5. OpenSSL crypto/HMAC native allocations before first crypto use, callbacks,
   secure allocator/provider contexts and failures. Correct callback install
   result is necessary, not full coverage. Crypto/JIT/ICU/zlib/other native
   dependency paths in the actual complete source closure must route to the
   owner or positively deny their use; unreviewed fallback malloc is forbidden.
6. Main/worker/native callback stacks, guards, TLS, page granularity, source-
   owned mapped images/code/data and original image/file/process/token/handle
   custody. Every category's original native allocation/reservation and actual
   owner is captured. Count measured overhead; never assume file size==RAM or
   malloc requested bytes==native reservation. No blanket OS-process RSS claim.
7. Event/ledger/copy-set/private capture storage and every failure/cancellation/
   late IO path. Bounded original native arenas/metadata are reserved before
   events. Saturation retains/quarantines instead of dropping evidence,
   wrapping a counter or allocating an uncharged exception/record.

Each complete Buffer copy set reserves all entries and metadata BEFORE any
allocation. Initial public bundle.files and every acceptedTemplateFiles/internal
copy consume one of128 slots. Returned independent mutable Buffers retain their
real native owner; original immutable snapshot/session has separate original
association. No caller mutation, clone, ArrayBuffer alias, detach/transfer,
finalizer, GC, package reimport or successful action releases conservative
source charge. Original Node physical exit is the release observation. Actual
allocator results and copy ledger/native process/capture observations are
independently correlated by the original observer; no counter claims native
proof. No public raw source/capability/handle/owner/key/path is added.

Source hooks do not authorize a new original Node image. Generic existing Node
processes cannot be retrofitted or positively qualified by loading an addon.
Unsupported original image/source/ABI/native inputs fail with existing safe
unavailable identities. A permanently empty route is not completion: source
must implement the complete compatible positive route and full fixtures while
retaining absent-input denials until actual original authority is provided.

## Packaging, API and source/native qualification closure

Normal public loader/acceptedTemplateFiles/materialize/dispose API signatures
and mutable Buffer compatibility stay unchanged. Native startup state connects
to the SAME original Node parent and package instance before the actual library
transport/inspection receiver. It neither replaces the parent with a helper nor
adds an IPC copy request. Original four held assets/readReceipt, semantic owner
validation, complete inspection record and independent snapshot remain required.
Every original/native owner survives real EOF through bound archive readback.

Packaging must select an independently admitted exact instrumented Node image
and source/build/provenance bytes, rather than process.execPath or version alone.
Plain-source SEA serialization/build/runtime flags must be proven compatible;
if snapshots/code cache/configuration differs, same-version equality is not a
substitute. Complete source-coded build selection and actual admitted build
inputs stay under existing source-only build/publisher authority and cannot
create a runtime grant. Changed private ABI/build/configuration is explicit in
this candidate; no code implementation before its whole review. Tool/native
execution still awaits final ENTIRE product SOURCE GO and NEW full input ROOT.

Keep the existing five native TAR members, eight public candidate/checksum
assets, scoped publisher/actual source/proof/privacy/retention rules and Darwin
legacy/deferred source boundaries. Do not silently add a custom Node installer
to public assets or claim it is provisioned. An original library-parent Node
runtime installation remains separately admitted native input/provisioning;
its actual compatible image and source must be supplied through that existing
boundary before qualification. A changed release asset scope would require a
separate explicit full amendment.

Full original TC01..12/native path fixtures cover startup before untrusted args/
env/JS; missing/wrong original source/provider/caller/image/ABI/bootstrap-C;
all allocation classes, overhead/stack/image granularity, every prospective
reservation/exhaustion path, repeated full copy sets and package instances,
alias/detach/clone/GC/finalizer denial, delayed real parent exit, incomplete
native IO/EOF/archive/ledger and original private instrumentation/readback.
Both actual Windows/Linux native source routes use the same hooks. Every
actual trace binds raw original native allocation/source/parent/image/copy
objects, not a fake physical-memory certificate. Full source fixtures and
production bootstrap/main/facade/provider/observer must be coherent together;
a constructor-only or fixture-only hook is not whole implementation.

## Complete alternatives and minimal decision

N1 is the exact candidate above: source-built Node22.23.2 startup/owner coverage,
explicit source/image/private ABI/provenance changes and extra private PA1
consumer, unchanged public API/wires/caps/rights/deadlines, actual original
inputs and fit still required. It is source-architecturally implementable via
the observed source routes; physical sufficiency is NOT established by this
proposal or the existence of hook methods.

N2 is a separately source-owned Node22.23.2 native embedding launcher that owns
CRT/pre-entry, V8/page/buffer, libuv/crypto and stack/image allocation from the
same early boundary, links the complete matching runtime and exposes the same
public Node/library interfaces. It still changes actual original Node image,
private owner ABI, source/build/SEA provenance and native installation inputs;
embedding APIs alone do not cover V8 native malloc or CRT/pre-entry allocation.
It must meet the same complete hooks, original authority, fit and conservative
exit lifetime. It avoids some Node main patch sites but is NOT selected here.

N3 (addon/externalBuffer counter only) and a separate native helper/remote-only
snapshot are not complete unchanged-API alternatives: actual returned Buffer/
V8/metadata ownership is still in the original Node parent, and a helper exit
cannot release its charge. Changing the API to proxies/remote copies or
changing source-domain scope/caps would be a larger different architecture;
no such alternative is authorized or implemented. Runtime flags, GC targets,
RSS sampling, optimistic calibrated declarations and skipped categories are
not viable repairs. Increasing budgets is outside this candidate.

The smallest additional decision is the WHOLE N1 runtime/startup/owner/private
ABI/packaging and PA1-consumer extension, with its complete original Node source
packet and existing whole product inputs. Independent review must assess all
categories/lifetimes, circularity, original authority and physical source-domain
fit obligations together. No piecemeal hook approval is requested. Independent
engine/writer/archive/public route source work continues; this decision is not
a blanket architecture blocker or a proposal-only completion claim.
## Accepted exact whole N1 source implementation decision — 2026-10-05

The conductor accepts the exact independently reviewed 19844-byte candidate decision (SHA256 f8127448387cdace1635d4147f60ab0c4b5ca1a43e3fb8159a4299e510cdcd82), candidate ROOT 1e656577ff664b88f246900a75b2c7dfc4225bf4e4265135dd9d60bcc5784d72, for WHOLE N1 ARCHITECTURE GO FOR SOURCE IMPLEMENTATION ONLY. Independent review is D:/projects/service-lasso/_audit/cli39-n1-entire-architecture-review-oct05-03: ROOT 6a91294b4841e2b192fe3ca9a0d779789771f7785db1f6bf13f891895c1dd5c9, REPORT 19028 bytes SHA256 7f51af4765be080bd2b895e868d19c7d00bd4a4cade3b0611981c0bf8d3993e0. The author read the complete report/root, independently checked all five root members and freshly reread all 75551 physical associations, zero size/SHA failures; external N1-REVIEW-AUTHOR-ACTUAL-BYTE-VERIFICATION.json SHA256 bb0770e4265a5ee54f3136753d9e60966738d9b04d46c803300d63df0442f490. Parent original-byte audit is cli39-n1-review-parent-byte-audit.json in the conductor custody packet.

This exact decision authorizes whole N1 source implementation, with every original owner/category/cap/lifetime and private authority boundary above. It supplies no authentic provider/caller/ROOT/key/calibration/compiler/CRT/image/catalog, no actual fit, no final product SOURCE GO and no executable check. Original node.gyp/common.gypi pointer-compression/sandbox defaults are zero; large cages are conditional, not a universal impossibility premise. Actual libuv stock positive worker use reserves 8 MiB before any other positive resource and cannot fit that shared 8 MiB domain. An unspecified different fixed stack/configuration/feature/private ABI does not inherit this GO: record the exact whole additional source decision before such a change. No convenient foreign/shared relabeling, hosting quota, category exclusion, reset or cap waiver is permitted. Keep frozen candidate/review untouched and continue complete PA1/N1 production source; before any imports/compiler/build/Node/tests/native require finished whole source, new complete actual input ROOT and different ENTIRE final product review plus original admission.
