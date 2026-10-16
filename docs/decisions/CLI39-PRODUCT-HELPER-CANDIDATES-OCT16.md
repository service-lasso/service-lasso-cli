# CLI39 explicit Windows helper candidates — byte audit, UNQUALIFIED

2026-10-16. Owner CLI39/PR43. Administrative raw archive/header/symbol reads and
SHA256 only. No lib.exe, dumpbin, compiler, assembler, linker, loader, parser of
product source, fixture or target code ran. No binary was copied/extracted.
These installed paths are candidate source inputs, NOT held native originals,
source/member provenance, runtime compatibility, startup safety or ROOT admission.
Embedded build-machine member names are labels, not existing local source paths.

## Actual candidate tuple

VC root: `C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.44.35207`.
SDK library root: `C:/Program Files (x86)/Windows Kits/10/Lib/10.0.26100.0`.

| Relative path | Bytes | SHA256 |
| --- | ---: | --- |
| VC lib/x64/libcmt.lib | 6534094 | 8fe40dd342b90b35d62b8f5d55b4e92df7785c122976618365cfe3b014cc857a |
| VC lib/x64/libvcruntime.lib | 2087406 | e4f8daf356d63592ff6d6f7ad90c8c9da4c1230273ad113a9a2b56afd6bfcb04 |
| SDK ucrt/x64/libucrt.lib | 43336396 | 2b247d6a27c22c520c34134b8acf2da3366175a4690886927a51929f37725fdd |
| VC bin/Hostx64/x64/ml64.exe | 646504 | a776cf32777d2df2e201a769bb5bd6502bb72bac98f60c6f53a4bfcecd0db8ed |
| SDK um/x64/kernel32.lib | 311908 | 341c7d56125a03b458e4d5093e4c79b33123ccfdfd610fe236937b8e6f3134bb |
| SDK um/x64/advapi32.lib | 178306 | a0ea53377e6de11c5fdc07e72af11d22d8fa613cecc6ccb1b6de43490392eadc |

All archive offsets below are decimal offsets in the exact whole file above.
Body starts at header+60. Archive traversal reached the exact end, 1193 libcmt
members and58 libvcruntime members. Selected bodies are ordinary AMD64 COFF;
member hashes cover ONLY body bytes, excluding archive header/alignment padding.

| Archive | Header | Member basename / indexed role | Body bytes | Body SHA256 |
| --- | ---: | --- | ---: | --- |
| libcmt | 4926668 | guard_xfg_dispatch.obj | 1962 | 01aa1b871b56a1f6f0345557ef76567d631ae2d6ffe8c403cb1462d2c30a270b |
| libcmt | 4928690 | guard_dispatch.obj | 1882 | 4522352ae01855765c97bdcef7e1e0154f2dd72944587f310146d507a5892977 |
| libcmt | 4932762 | chkstk.obj / __chkstk | 2180 | 64961db4338c5b6c97e58f82a56018812ec1fef2e9fb96ef265960db9841a7bc |
| libcmt | 4937406 | indexed __security_check_cookie | 2090 | 7a75046d52f15332ba01ef1bdb87d0cb4a92355fe66825755b284d7d9aaeeb61 |
| libcmt | 5384878 | loadcfg.obj / _load_config_used | 17647 | f5abc89cbf4e57815c3a0edd5580f29316e4d26755e89056594b922c1023f786 |
| libcmt | 5403994 | guard_support.obj | 38078 | 7d4dfd95b518e79d944830eddc5c03cf56ce467a775d062fd2c1f7095dca0cd5 |
| libcmt | 5459536 | gs_support.obj / __security_init_cookie | 17721 | 2cafe2de4054beadaf940e8ff46458df604eb96b0cb5ce18dc4f27e4b0347ace |
| libcmt | 5477318 | gs_report.obj / __report_gsfailure | 29838 | 3db32e0f0b124d7f68436ee96a974500aa9d9ae306325803071d97dd78431382 |
| libcmt | 5507216 | gs_cookie.obj / cookie and complement | 1580 | 297ddc747fdbcc8ca2888fd774e2cb07bc2f9f8e340332d80f43fcbb44feb29c |
| libvcruntime | 92524 | memset.obj | 10850 | 315246606c03de3e4817f2766d5eb82bfb694f7d9759243f45e27082b600dcbe |
| libvcruntime | 150210 | memcpy.obj / memcpy and memmove | 13578 | a1ca69fa1e094af3910673f4a95dfeb755acee7740069c699caa20cd4ac59f84 |

## Concrete viable closure and remaining body obligations

Missing loose x64 stack/check/dispatch assembly does NOT prove these helpers
are unavailable. The actual retained library members provide a concrete explicit
candidate route under the selected MSVC/SDK contract. It must qualify exact
source/tool/member/native-case/entry effects and physical costs. Neither whole
library presence nor an indexed symbol establishes that qualification. No blanket
source-only prohibition, default CRT discovery, GS/CFG waiver or silent library
search is selected. Explicit fixed held operands must precede link execution.

Raw external-symbol rows show why the listed11 objects are NOT a closed link
roster. Cookie checking depends on cookie plus report; cookie init imports QPC,
process/thread IDs and system FILETIME. The actual candidate's listed externals
do not include GetTickCount64, so loose source with another preprocessor branch
cannot be assumed to describe this member's exact compiled case. Cookie report
imports native context/unwind/unhandled-filter/process/termination/CPU-feature
operations; its failure path is not an ordinary successful return or settlement.

Load config depends on linker-created CFG/IAT/longjmp/EH/volatile/enclave table
symbols, cast-guard and guarded-memcpy cells. guard_support depends on actual
dispatch/XFG helpers. memcpy/memset depend on __isa_available, __favor,
__ImageBase, dispatch/fallback and threshold symbols. Actual feature-state
initialization and full selected branch/helper roster must precede their use;
source byte loops do not automatically avoid compiler-generated helper calls.
Explicit emitted prefix/relocation/import/load-config/CFG/GS/stack/native source
case inspection is required before loading, not inferred from flags or names.

The CLI-owned real earliest-entry body must call qualified initialization before
any protected function or transitive helper, with exact stack/unwind/source/cost
closure. TUI's coherent MSVC plan will consume the resulting actual fixed entry,
objects and import/member operands; it cannot substitute ambient CRT startup.
The genuine entry/child-local source/funding/principal/family constructors,
deadline handoff, native phase correspondence, A/full Node/Linux schedules and
whole source/new execution admission remain unfinished. producerComplete=false.
