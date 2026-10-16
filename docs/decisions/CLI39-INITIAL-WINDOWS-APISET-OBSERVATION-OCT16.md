# CLI39 initial Windows API-set observation — pre-code selection

Status: proposed private loader-input observation only. No body, target parser,
native call, emission or execution is supplied by this document. The complete A/B
builder, genuine initial source/profile construction, useful debugger continuation
and product installer owner remain unfinished. producerComplete stays false.

The selected Windows A native owner already holds actual child PBI/process/token,
complete source files, native NT paths and stopped mapped image bytes. Its ordinary,
bound, delay and exported symbol records cannot guess API-set host DLL selection.
This unit proposes obtaining the actual namespace rather than inventing a basename
mapping. It introduces no public action/flag, caller syscall, new loader, credential,
process creation mechanism or change to CLR/OS state.

## Private ABI and qualified input boundary

The candidate is Windows AMD64, PBI.PebBaseAddress at offset8 and the PEB API-set
pointer slot at offset0x68. Only namespace version6 is selected. The private layout
reference is [phnt ntpebteb.h at 53fbbdc5b5d2b08761db1c7b26bfa8c820924356](https://github.com/winsiderss/phnt/blob/53fbbdc5b5d2b08761db1c7b26bfa8c820924356/ntpebteb.h).
It is a layout reference, not Microsoft's stable ABI guarantee or qualification of
any installed Windows. The actual independently admitted native OS/engine/source
tuple and SDK/private ABI correspondence must establish this choice before entry;
matching version6 or a plausible pointer never supplies that history/authority.

The v6 source records use these fixed byte extents and unsigned32 fields:

| Record | Bytes | Offsets |
|---|---:|---|
| Namespace header | 28 | version0, size4, flags8, count12, entryOffset16, hashOffset20, hashFactor24 |
| Namespace entry | 24 | flags0, nameOffset4, nameBytes8, hashedBytes12, valueOffset16, valueCount20 |
| Value entry | 20 | flags0, aliasOffset4, aliasBytes8, hostOffset12, hostBytes16 |
| Hash entry | 8 | hash0, namespaceIndex4 |

All offsets are relative to the SAME complete native/source namespace body.
UTF16 byte spans are even, in bounds and strictly decoded; string normalization,
truncation, guessed host extensions and default aliases are not source proof.
Unknown versions/flags/forms or ambiguous value selection remain refused until
their exact interpretation is selected and implemented. This proposal does not
silently claim that reading v6 records completes loader resolution.

## Actual source original and prebirth construction

The fixed A constructor consumes a private selected schema-member reference from
its complete ToolModuleMembers originals. It is the SAME already captured native
readonly file/ancestor/body/EOF/metadata/NT-path occurrence; no second file search
or duplicate artifact acquisition is selected. The actual original PE section
roster must contain exactly one eight-byte `.apiset` name. Its complete raw source
span supplies the v6 header and exact declared body length. Raw/source/map equality
is correspondence, not authentication of that source or initial ROOT.

Before compiler birth the owner preowns source/map/native controls, finite rows,
complete native body capacity B, independent header28/pointer8 readbacks and their
returned-count cells, plus finite native NT-path capacity U. Native region controls
use at most R one-use64-byte query/read records. B/R/U and all managed metadata,
decode/compare/hash work belong to the independently admitted whole initial physical
profile; source counts cannot issue financing. Arithmetic and source body length
must fit that original capacity before any child exists.

## Fixed native effect schedule

1. Consume only SAME stopped root CREATE_PROCESS event and its actual captured
   PBI process handle/PebBaseAddress, never an external PID or caller pointer.
2. Record actual creator TID. Query the PEB pointer-slot region, then read exactly
   eight bytes with a distinct native returned-count original. Unknown/short/guard/
   unreadable/overflow results retain raw outputs and refuse.
3. Query the actual map header region and read exactly28 bytes. Require selected
   version, bounded size and equality with the complete held original header.
4. Capture actual K32GetMappedFileNameW NT pathname for that actual native mapping;
   join it ordinally to the held schema original. Missing/private/unattributed
   mappings are retained refusals; no synthetic path or guessed backing file.
5. Walk exactly the declared map byte span with actual VirtualQueryEx geometry and
   exact bounded ReadProcessMemory results. Retain allocation/type/protection and
   every actual byte/count before postclock; no zero-filled unreadable gaps.
6. Reread the native pointer slot and independent complete header. Require SAME
   pointer/header and complete body equality with the source `.apiset` span.
7. Parse and retain all bounded namespace/entry/value/hash records and UTF16 spans.
   Compare complete source/map bytes, not only selected names or a digest.

Only fixed existing A interop functions are called. Every API call has its actual
raw return/output and original before/after QPC/frequency cuts, five native calls
per successful API operation including those clock cuts. Each body region consumes
at most one query/read pair (ten calls); explicit processing start/end adds four
clock calls. Failure/error/overflow/native/OS costs remain separately charged.
No native allocation/protection/write/resume/close/load is performed in the child.

## Output, exclusion and failure lifetime

The constructor roots all outputs before effects and excludes the complete source/
PBI/token/event/command/module/profile graph from every native destination. Source
files, native map/body/header/pointer/path outputs, rows and first/later failures
remain under the SAME issuing owner until genuine last borrowers finish. A stopped
child, uncertain read or failed/late cut prevents useful progress and script return;
no reset, retry, kill/detach or new deadline is granted.

Output is an actual source/native namespace correspondence occurrence. It is not a
public JSON profile/issuer, opaque external-pointer API or complete loader/ROOT
factory. Actual alias/importer/host/export/forwarder resolution, mutable loader-state
source, full event/descendant/stream/natural output/alias closure and A/B/runtime
handoff remain required. Whole source review and NEW exact genuine ROOT/input/
command admission still precede all target actions.
