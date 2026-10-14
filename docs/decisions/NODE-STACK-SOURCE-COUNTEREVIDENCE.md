# Original Windows Node stack reservation and N1 implementation boundary

This is concrete source counterevidence, not an amendment, runtime result or
permission to change the selected stack configuration. The accepted N1 whole
architecture decision remains source implementation only. All source-owned
stack reservations, runtime allocations, images, metadata and native custody
remain inside the original 8MiB primary/SEA and client domains.

The original Node22.23.2 commit is
`aa4c77582be995286fc6e00aaf530dc7ade102a9` and its original `node.gyp` blob is
`f9ee71dd9a395fa1de99a6f62ba60acdf8be12a1`. Its complete raw body is retained at
`D:/projects/service-lasso/_audit/cli39-node-startup-allocator-architecture-precode-oct05-01/original-raw-objects/f9/f9ee71dd9a395fa1de99a6f62ba60acdf8be12a1.raw`.
The actual raw SHA256 rechecked for this observation is
`b7f1544d40de42a76c8c6c88a8615db560db43d05dbfeb3bcfca72e9f35d9aac`.
The full original graph/byte association is already in the frozen accepted N1
75,551-association input packet; no executable was built or loaded.

Lines 527..550 select the executable target and its `src/node_main.cc` entry.
Lines 554..566 give that target's Windows VCLinkerTool settings. In particular,
line566 sets `StackReserveSize` to `0x800000`: 8,388,608 reserved stack bytes.
The adjacent original comment states that this raises the default MSVC 1MiB
reservation. This is the source-selected executable main-stack reservation,
separate from the already recorded libuv 8MiB worker-stack counterexample.

Under accepted N1, the source-owned main stack reservation is counted in full,
including no-access reserved pages. Thus this exact unmodified Windows link
configuration alone consumes the entire 8MiB domain. Any positive additional
owned image/runtime/metadata/IO reservation would exceed it. This conclusion
uses the selected reservation and accounting domain, not generic RSS, working
set, committed-page statistics, pointer-compression cages or an inferred
universal minimum for every possible Node build. Actual pointer compression
and sandbox defaults remain zero in the original selected gyp inputs.

The original `common.gypi` blob is
`0ffa5b137195157fa9b44205d5db45cff1b2e10c`, raw SHA256
`276374a2a16ea6a31fda5e702383061e9ad622d5179d3b3edd2d5c393623e27b`.
Lines183..190 choose Release `/MT` for non-shared Node and `/MD` for shared
Node. Therefore moving one hook into `wmain` also does not establish coverage
of the selected CRT startup/static initializer routes. The actual selected
CRT source/build/image and compiler association remain independently
unestablished; an installed compiler or source capture is not admission.

A complete N1 stack/configuration supplement must select the exact Windows
main-stack link setting and each actual worker/callback stack route together
with Linux main/worker reservations, guard/TLS/native-object ownership,
failure semantics and complete original source/build associations. It must
preserve the original caps and prove all-or-deny prospective reservations.
No smaller numeric stack, disabled feature, changed private ABI, additional
hosting quota or exclusion of runtime/image categories is selected here.

The viable whole-source routes to assess remain the accepted N1 instrumented
Node image with a separately reviewed complete stack configuration, or the
previously documented N2 native embedding alternative with its own complete
entry/CRT/runtime/stack/source interface review. Neither alternative obtains
fit or image/native authority merely by avoiding this original link default.
Its actual calibrated peak, stack sufficiency, original native allocation and
closure evidence remain required. Addon-only hooks, GC limits and relabelling
the main stack as foreign cannot repair this concrete N1 source obligation.

Independent native command, observer, source-lease and original custody source
work continues. This evidence does not turn absent provider/profile/key/ROOT
resources into a blanket source blocker, and does not authorize execution or
replace the required different entire final product source review.
