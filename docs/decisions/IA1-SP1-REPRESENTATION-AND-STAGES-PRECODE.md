# IA1 repair04: proposed original source representation and stage cuts

PROSPECTIVE WHOLE AMENDMENT. All R03-01..03 are proposed together; none is independently cleared.
The ten-section INPUT-AUTHORITY-ISSUER-ALTERNATIVES-PRECODE decision and this
normative appendix require a new different entire architecture review together.
SP1 is a NEW representation, not a previously admitted container. No generator,
issuer, principal, key, source/native provider or runtime is implemented or selected
by this document. Ordinary approved PA1/N2 source work continues. Full Node22.23.2,
full ICU/Buffer/API/library/SEA, service1MiB/store4MiB/Core128MiB and Q12/C13 remain.
Q32 remains qualification-only; measured production B0/B2 remain UNKNOWN.

## 1. Original unit, membership and source custody

Propose one independently admitted immutable native SourceUnit. Each source member
is a newly born original byte range of THAT physical unit. It is not described as
the original kernel object of a former loose Git file. Independent human-selected
BQ/source-provider authority binds the original captures to the new unit, complete
membership, native identity, EOF and retained custody. Source/hash/path/constructor
alone supplies none of that authority. Historical originals remain in EvidenceROOT.

SP1 holds exact raw selected source/tool/SDK/CRT/license and capture associations,
including complete selected Node and Monocypher ancestry/commit/tree/blob/tag bytes.
No compression, delta, symlink, external URI, executable effect or embedded credential.
Distinct physical captures with equal hashes have distinct records and disjoint ranges.
The conservative positive source fixture retains even every repeated historical
association as a separate range; it needs no invented physical identity deduplication.

Header is exactly64 bytes, little-endian: magic8 SLSPK001; version u32=1;
recordStride u32=128; recordCount u64; tableOffset u64=64; tableBytes u64;
bodyOffset u64; fileBytes u64; reserved u64=0. tableBytes=count*128 and
bodyOffset=64+tableBytes, all arithmetic checked. Exact native immutable size/EOF
equals fileBytes, maximum8589934592. Record128: originalAssociationId32,
nativeCaptureAssociationId32, rawSha25632, memberOffset u64, memberBytes u64,
kind u32, sourceProject u32, reserved u64=0. kind raw0/commit1/tree2/blob3/tag4.
IDs select independently authenticated A associations, not principals or credentials.
Records sort by sourceProject then originalAssociationId strictly uniquely; payload
ranges are contiguous in that order, beginning bodyOffset and ending exact EOF.
Genuine empty bodies keep distinct association/range records; no absent/null leaves.
All selected A associations occur once; extras/missing/overlaps/trailer deny.
A contains original names/type/Git IDs/project revision/root/capture provenance;
A is independently held outside SourceUnit and does not index itself or later ROOTs.

For every Git body recompute type + ASCII original length + NUL + original body
SHA1, plus original raw SHA256/length. Traverse full commit parents/root trees,
binary tree mode/name/NUL/20-byte IDs and tags through bounded pread and fixed-table
binary search with project in each lookup. Non-Git SDK/CRT/tool originals require
complete original manifest/native membership rather than fabricated Git ancestry.
The native BQ reader must actually read every member and edge, not accept a count
or Boolean. Source/native provider/image/caller association precedes effects.

Actual candidate02 metadata has388495 associations with7722113187 body bytes.
Conservatively duplicating every association gives49727360 table bytes and
7771840611 total bytes, including64-byte header: below8589934592 without compression
or deduplication. These are source-data arithmetic observations from the complete
original manifest, not a generated SourceUnit, native memory fit or trust grant.
New actual source/tool/native selections need a complete updated manifest and exact
table/body enumeration before construction; exceeding the bound requires a separate
reviewed choice, never truncation. Historical193377 Git/229283 distinct-path custody
is retained; paths do not establish native identity. Current complete positive fixture
must contain full selected Node/verifier/native/BQ/issuer/tool/CRT source, not samples.

Windows: no-follow read-only original file, no write/delete sharing, full FILE_ID,
volume/owner/protected-rights observations and held/loaded/reopened correlation/EOF.
Linux: originally born sealed memfd with all four seals independently observed,
read-only held capability and fstat owner/device/inode/size/EOF; construction writable
handles/maps actually closed before admission. F_DUPFD_CLOEXEC preserves same original
unit only. Same-process Linux provider/installer/BQ entries remain separately admitted;
cross-process Linux handoff is the NEW SCM_RIGHTS topology in section4,
requiring whole review; F_DUPFD_CLOEXEC alone crosses no birth. Windows transfer retains full original
sender/receiver birth/token/image/rights correlation. No neighbouring path lookup.
Immutable-source backing, IO, cache, mapping and reader ownership are independently
observed and fully charged where owned. An8GiB file bound creates no free memory pool.
Unknown ownership denies fit; service-owned source views/backing remain inside1MiB.

## 2. Original capture journal and bounded access

Propose a BQ-controlled mutable PresealJournal followed by a separately admitted
immutable native SourceAuditUnit. These are distinct authority/lifetime states;
no immutable final-unit claim authenticates an early retirement.
Each journal record is an original byte range with producer/native identity and raw
observations, never serialized claims returned as proof. Journal is born outside
the experiment's initial R, joins later E/certification, and never rewrites R.
Equal-hash separate journals remain separate. This is a second explicit SP1 reference
representation amendment, not automatic packing of old independently born captures.

Header16: magic8 SLSAJ001, version u32=1,reserved u32=0. Each record fixed48:
u32 rawLength,u32 kind,u64 sequence,u64 sourceMemberOffset,u64 sourceReadOffset,
u32 requestedBytes,u32 returnedBytes,u32 nativeCaptureBytes,u32 reserved=0;
then exact original capture bytes and returned original read bytes. rawLength=48+
nativeCaptureBytes+returnedBytes; sequence starts1/increases without wrap.
kind before1/read2/EOF3/after4/graph-edge5/measurement6/disposition7.
Read/EOF requests1..16384; returned0..requested. Measurement/disposition carry
zero requested/returned bytes and actual admitted native raw observation ABI;
they never manufacture a measurement or retirement Boolean. All native status,
tick/counter/source-owner/capture identity data remain original and independently
read back. Bounded original capture bytes precede every reuse decision.
Actual EOF needs an original successful zero-byte read capture; promised size is
insufficient. Native capture ABI/source is independently admitted, not portable JSON.
Graph-edge records retain actual bounded original member bytes/read lineage.
Footer24: magic8 SLEND001,u64 recordCount,u64 exactBytes. External digest only.
Missing footer/partial/failure remains retained UNKNOWN.

Each serial reader has one16KiB source window, bounded native capture/control storage,
one16KiB journal write window and one independently owned16KiB readback window.
All simultaneous stack/crypto/index/native/IO/image/windows are charged together.
Reuse only after original raw observations are written/flushed/independently read back
and authentic retention disposition authorizes retirement of that local buffer.
Absent disposition retains buffers and denies capacity; no GC/timeout retirement.
Retention provider uses independently admitted finite P.retention maximumBytes and
maximumRecordBytes, never service ledger store4MiB or a foreign unbounded pool.
No whole-unit mmap/read-all. Actual record+capture+read stays within768000; exact
capture ABI/layout/lifetime is independently proved before execution. C13 evidence
may select journal unit/range through original verified membership without new keys.
One genuinely held immutable unit counts once; member offsets/selector metadata and
every simultaneous duplicate-handle storage are charged inside the existing ledger.
Distinct native originals always occupy different slots, irrespective of digest.

### Normative preseal authority, write/readback and final transition (R03-01)

BQ's independently selected native retention entry is the SOLE writer of slot105.
The experiment producer never receives that writable file/map/capability. It sends
one bounded original source/capture window at a time over the admitted channel;
BQ authenticates its actual birth/image/entry/owner, fixed sequence and source-unit
range before accepting bytes. BQ retains the producer's original capture until the
complete append and independent readback cut; a Boolean response is insufficient.
The independently admitted BQ writer and readback entry capabilities have separate
source bindings under birth41 and caller69, not a self-authenticating producer.
Actual raw write/flush/readback/disposition observations enter the journal itself.

States: UNBORN -> BQ_WRITABLE_PRESEAL -> APPEND_PENDING -> COMMITTED_EXTENT ->
FOOTER_PENDING -> WRITER_CLOSED -> FINAL_IMMUTABLE_ADMITTED -> ARCHIVE_ACK -> RETIRED.
FAILED/PARTIAL/CHANGED/UNKNOWN is absorbing for that attempt, preserving all born
objects/captures/windows and charges; no next stage can reuse slot105. Every append
uses checked current end/sequence, exact native offset and bounded length. BQ's
source permits only writes beginning at committedEnd, never truncate/punch/map,
seek overwrite or mutation of a committed prefix. It owns no writable alias outside
the fixed admitted entry, and retains original handle/fstat/FILE_ID observations
before and after every write. Authority is the independently admitted BQ entry and
exclusive native writer custody, not a claim that the growing file is OS immutable.
BQ compromise/source change denies qualification; the producer has no rewrite right.

Windows preseal opens a newly created non-follow original file with read/write
access and no write/delete sharing; BQ holds the original handle throughout. Its
independent readback entry uses that held object through an independently admitted
read capability, never a path-selected replacement. Linux preseal is an original
BQ-exclusive O_NOFOLLOW/O_EXCL regular retention file, owner/rights and ancestor
custody admitted independently; writable descriptors/maps are confined to BQ's
append entry. Readback receives only an original read capability. Linux preseal is
NOT a sealed memfd. No timeout, journal digest or fstat alone grants prefix custody.

For each record BQ loops partial writes at the exact remaining tail, retains all
source/capture windows, performs native durability flush, independently preads the
committed extent through original readback98, compares EVERY raw byte and records
native completion/identity/size observations. The independently selected retention
entry then commits an original disposition RANGE bound to stage/birth/journal identity/
sequence/offset/length/original capture and actual readback. That admitted operation
removes only the corresponding producer window charge after all pending native IO
has actually completed; its raw disposition remains journaled and independently
read back. The bounded capture/control/readback/write/source storage and handle
metadata remain in the SAME owning budgets. Its authentication is held original BQ
capability/source/peer custody, not a serialized acknowledgment or future signature.

When the final raw source/measurement/disposition record completes, BQ writes and
flushes the exact footer, reads the complete journal including native zero-byte EOF,
checks every sequence/range and closes every writer/map with original closure proof.
Windows retains the SAME file object with an independently correlated read-only
handle/no-write-delete sharing; final immutability begins only after writer closure.
Linux copies the complete durable staging original105 in bounded windows into a
new memfd reserved in the stage's final slot91..96 (or104 for BQ certificate journal),
independently compares all original bytes/EOF, closes writable mappings, applies and
observes F_SEAL_WRITE|F_SEAL_GROW|F_SEAL_SHRINK|F_SEAL_SEAL and read-only custody. The
new unit is honestly a NEW original with authenticated provenance from staging,
never the former regular-file kernel object relabeled. Staging105 stays durably held
until independent archive99/100 readback acknowledges complete raw capture retention.
Final native admission checks original authority/member ranges/EOF and all seals;
only then may the final SourceAuditUnit satisfy C/ROOT membership. No later append.

Positive simultaneous witness: prior final journals stay91..96/104; current durable
preseal uses105; its newly born immutable output uses its previously reserved final
slot; independent readback98 and archive99/100 are already separately counted. Windows
needs no second file original; its duplicate handles still occupy charged metadata.
Linux preseal+final overlap uses TWO different physical originals105+final slot.
The three16KiB windows are charged ranges, never uncounted native objects. Duplicate
references to the SAME original need separate charged handle/copy metadata, not a
new physical-original slot; distinct objects ALWAYS need separate slots. Any extra
native readback/output/channel/buffer owner original denies before effects. Slot105
is reused only after genuine prior staging close and admitted archive retirement;
failed staging keeps it occupied. A process crash that loses live originals produces
UNKNOWN and recovery quarantine, never reconstructed native authority from paths.

Required whole-source fixtures retain actual partial write, wrong offset/sequence,
mutated committed extent, unauthorized writer/alias, missing/failed flush/readback,
readback wrong birth/object, pending IO, partial/missing footer, trailing byte,
writer/map still live, missing seal, final-copy mismatch, crash, archive denial and
capacity-old/new overlap. Success must stream MORE than one window and demonstrate
original early retirement under preseal BQ custody followed by exact final EOF/seal.
None runs or creates a journal in this documentation proposal.

## 3. Ordered authority, input and output table

All stages retain original source/image/caller/process/token/capacity lineage and
the exact SAME128 native-unit table in section4. Unsigned stages use no fifteen-key
statement at all. Future outputs are absent, without null/empty/signature-looking
placeholders. Capacity reservations for unborn objects are not observed references.

| Stage | Already independent authority and original preeffect inputs | Outputs outside that stage's initial R |
|---|---|---|
| R0 native/bootstrap | Human-selected BQ private native principal/entry/source/image/tool/realm/capacity; unsigned R0,A,T,SourceUnit,native P/G,Q/case/capacity and three indexes, original actor/tools/native/caller objects. No C1/C4 produced by this experiment, IA1 S/V/anchor/key/future issuer C. Service1MiB/store4MiB/independent observer policy unchanged. | Original native/bootstrap C1/C4, original observation/source audit journals and failure/fit evidence; independently BQ read back before use. |
| R1 issuer initial | Same independent BQ, completed R0 native/bootstrap certificates/evidence; unsigned R1,A,T,SourceUnit,issuer P/G/Q0/case/capacity,four indexes selecting PREEXISTING native/bootstrap C, full native/tool/image/caller/owner originals. Domain0 same full Node owner under Q32. No S/V/key/anchor/future issuer C/production B. | Actual nonsecret issuer initial C0/raw peak/custody/source audit/exit evidence. |
| R2 readback | BQ independently retains and reads R1+actual outputs and R0 originals/source/native realm. No new initial R2 payload is invented. | Nonsecret qualification certificate only; no complete credential-inclusive production B claim. |
| R3a credential calibration | Independently selected issuer principal/public32 anchor/PKCS8 custody/scope plus original BQ R2 certificate. NEW unsigned R3a,A,T,SourceUnit,own issuer P/G/Q0/case/capacity with credential cases, prior genuine C/native/source evidence, encrypted original PKCS8, original echo-disabled console/session and native owner/image/birth. STILL Q32 qualification, before production B. HA2 key is separate. | Actual credential enrollment/derive/key-match/signing-to-private-qualification-output and all simultaneous Node/OpenSSL/PKCS8/Buffer/native/IO peaks; retained original credential calibration C0 and source/evidence journals. Qualification output is not a consumer authority grant. |
| R3b certification/production | BQ independently reads actual R3a outputs and genuine issuer exit/custody, certifies finite COMPLETE credential-inclusive issuer B0 against full cases/overlap. New unsigned R3b binds that certificate and exact own P/G/case/capacity/native/source/credential/anchor. Preliminary finite numeric/source-closure decision first; original production P/G/Q/capacity birth and native admission second; final derived exact-profile C128 third. Production effects only after final C128 exists. | Production credential operation under final exact-profile certified B0 and detached consumer S/V after complete R4 original admission. Unknown/new peak/changed tuple denies, retains evidence and requires new qualification, never expands B. |
| R4 signed consumer | Independently selected issuer/anchor with genuine R2/R3a/R3b source/native/custody chain and separately admitted installer/provider/caller/HA2 key. Complete immutable R4,A,T,consumer P/G,four indexes,Q0/Q2 and every required measured C0/C2/native C1/C4/case/capacity/source/image/actor/evidence original. Issuer signs only after R4 EOF/hash/membership validation. Detached exact15-key S and64-byte V are consumer inputs NOW. | One-use actual native consumer references/effects and later E/archive/ACK; never edits R4 or uses later E as R. |

R3a credential calibration requires separate explicit human authority for the actual
credential/native realm; mechanism review alone grants no secret operation. All
encrypted/passphrase/KeyObject/OpenSSL/private copies remain conservatively charged
through genuine original issuer exit, including failures. R3b new issuer entry has
its own native birth and same certified source/credential/cases; old evidence remains.
Independent readback uses already selected BQ, not a future self-signature.
Consumer full B0/B2 remains a separate complete native qualification obligation.

Private index purpose adds bootstrap and issuer-credential-qualification, alongside
issuer-initial/issuer-production/consumer-production/observer. Root4/entry5/Q12/C13
keys are unchanged. R0 qualification/case/capacity indexes have native inputs; its
calibration index is absent because its C is output. R1 has four indexes but selects
only preexisting native C; R3a calibration selects prior actual C, not its future C;
R3b includes actual credential-inclusive certificate; only R4 has S's four digests.
Own issuer/native/consumer P/G joins are explicit, never substituted by consumer P.

## 4. Complete retained positive native-unit table

This sourceful engineering witness deliberately retains the ENTIRE stage lineage.
It amends reference representation; it does not assert actual selected originals.
All14 source/image role joins are present; the explicit brokered distinct-birth engineering
topology below shares ONLY genuinely observed same-original native objects.
Initial stage selects only its already born inputs; subsequent stages fill reserved
output slots after actual births. Every required extra original/old-new overlap must
be assigned before effects; this exact witness uses128, so an extra distinct object
requires a different reviewed fitting table or denies. It provides no second table.

| Slots | Exact original units/roles |
|---|---|
|1..11|R4,A,T,consumer P,G,issuer P,G,qualification/calibration/case/capacity R4 indexes, in that order.|
|12|Complete SourceUnit, all selected original source/tool/SDK/CRT/ancestry members.|
|13..26|Fourteen separate role source associations in canonical T order.|
|27..40|Fourteen role executable/module images in T order.|
|41..49|BQ bootstrap/installer/input-provider process; issuer-initial process; issuer-credential-qualification process; issuer-production process; service/ledger-provider process; native-primary/SEA/embedded-facade process; confined-writer process; observer process; original Node-parent/library-transport/loaded-facade process. These are NINE distinct original births.|
|50..54|Credential-qualification issuer P,G; production issuer P,G; original loaded-library-module loader/native membership reference. Initial issuer P,G are slots6/7.|
|55..63|Nine distinct original process token/UID observation units, matching41..49.|
|64..68|Credential-qualification issuer Q0; production issuer Q0; issuer production capacity; issuer credential qualification capacity; original BQ actual independent stage-certification input/caller association.|
|69..72|BQ caller,issuer caller,installer/provider private endpoint,consumer private endpoint.|
|73..76|Detached S,detached V,independent public32 anchor,independently admitted HA2 key-provider lease.|
|77..82|Consumer Q0,Q2,domain0 case,domain2 case,domain0 capacity,domain2 capacity.|
|83..90|Native C1,native C4,issuer nonsecret C0,issuer credential C0,consumer domain0 caseA C0,domain0 caseB C0,domain2 caseA C2,domain2 caseB C2.|
|91..96|R0 native/bootstrap source+measurement journal,R1 nonsecret issuer source+measurement journal,R3a credential issuer source+measurement journal,R3b production issuer source+measurement journal,R4 consumer domain0 source+measurement journal,R4 consumer domain2 source+measurement journal. Every stage produces a DISTINCT original immutable journal, containing complete indexed raw source/measurement/disposition membership; no later append mutates a previously admitted journal.|
|97..100|Source directory,retention directory,retention independent readback,archive independent readback originals.|
|101..104|Original native tool authority,original SDK/CRT source authority,original loaded-image authority,original BQ certification journal (preliminary decision/derivation records then immutable).|
|105..108|105: single serial independently BQ-owned durable PresealJournal staging original; 106..108: three retained archive/output originals. Stage final journal91..96 and certificate-journal104 slots are reserved before birth, not fabricated references. No next staging reuse until authentic prior archive/retirement; failure blocks advance.|
|109..112|Original unsigned R0,R1,R3a,R3b; R2 reads retained R1 rather than new root.|
|113..116|Original stage-index units: R0,R1,R3a,R3b, each newly born ORIGINAL immutable index unit with qualification/calibration/case/capacity member ranges and exact stage membership.|
|117..118|Native bootstrap own P,G.|
|119..121|Encrypted original PKCS8,native console,native session.|
|122..124|Issuer initial Q0,complete nonsecret+credential case set,complete initial qualification capacity plan.|
|125..127|Native bootstrap Q,complete native case set,complete native capacity plan.|
|128|Issuer-production C0, independently BQ-certified from complete original credential qualification observations, bound to production issuer P52/Q65/capacity66; original credential enrollment/native evidence remains in journal93.|

Slots113..116 introduce explicitly four immutable StageIndexUnit ORIGINAL objects;
their exact member ranges are consecutive raw index bodies under the same fixed
SP1 table/header/EOF/native-custody semantics, kind raw only. Their member records
join their independent original STAGE CONTROL associations in the enclosing
unsigned R, rather than pretending that source-only A predicts future index bytes.
A remains complete source associations only; it never indexes later stage controls.
This is proposed new representation too, not arbitrary wrappers
for old files. Each unit has only its stage's3 or4 complete indexes; distinct raw
indexes retain ranges and digests. R4 indexes1..11 remain separate conservatively.
StageIndexUnit member associations exclude the unit itself and enclosing R. These controls are
newly produced before their stage R, admitted independently, never outputs of
that R's experiment. Whole source review must assess this explicit extension.

All9 C selections (83..90 and128) are specifically named by domain/case above,
not a universal nine-C limit. Production C128 is a separately born original
certificate with production profileDigest, never C86 renamed or modified.
Its original evidence joins the actual complete credential-qualification journal93
and BQ certification104; derived certification is not claimed as a new measurement.
Each caseA/B pair must demonstrate genuine simultaneous ownership
in its complete original plan and own original evidence. C13/P/Q layouts unchanged.
Issuer initial P/G6/7, credential P/G50/51 and production P/G52/53 are distinct
originals with own Q122/64/65 and capacity124/67/66. Only the genuinely same held
case-set123 is shared where the independently selected cases actually match.
No P changes after qualification; production P is born only after the preliminary finite-B/source-closure decision,
and final C128 is born only after that production tuple has been independently admitted. All three issuer process births42..44 and original tokens56..58 are
retained separately; no exited process reference is reused for another birth.
BQ/installer/provider same-process private entries, service/ledger-provider private
entries, primary/SEA/embedded facade, and original Node-parent/library/loaded facade
are the exact positive engineering topology selected for this fixture. Each group
requires actual same-process/native-image/owner correlation plus distinct source
entry capabilities; T role equality or digest equality cannot establish sharing.
Slots27..40 conservatively retain every role image even when sharing is allowed.
Actual distinct helper/module/process/token/endpoint originals outside this table
require another explicit fitting witness or deny before effects. This topology
does not pretend to select current real BQ/provider/native actors.

The witness is therefore an explicit reviewable representation/reference proposal,
not native qualification or a universal production shape. It retains full history,
full source and all indexed original evidence without equal-hash aliasing. Native
source/tool/credential/actor selection, exact per-stage birth table and generated
positive fixture remain later independent authority/fit obligations. No implementation
of ANY SP1/StageIndexUnit/journal builder proceeds before whole review and required
actual independent authority selection. Mere overflow denial is never positive proof.

### Normative original cross-birth transport (R03-02)

Propose Linux AF_UNIX SOCK_SEQPACKET socketpair made by the independently admitted
BQ supervisor BEFORE child creation: originals71 and72. BQ holds71;72 is the serial
child endpoint retained by BQ and explicitly passed at the source-bound child birth.
BQ's native source uses fork/execve with an exact FD allowlist, closes every other
alias before exec, sets SO_PASSCRED on71 and receives SCM_CREDENTIALS plus SCM_RIGHTS
with MSG_CMSG_CLOEXEC. It observes each child through original process-birth lease
42..49 (pidfd plus independently admitted UID/image/entry correlation), not PID alone.
No public listener/pathname, ptrace, pidfd_getfd or numeric-fd enrollment is selected.
Existing private caller endpoints69/70 authenticate original external BQ/issuer callers;
these are not extra sockets. External Node-parent admission must arrive by its already
admitted loader/caller channel69/70 with the same original rights-transfer obligations;
if it lacks that channel the positive route is unavailable, never a pathname reopen.

Exactly ONE active child holds72 at a time. After its transfer transaction it closes
all72 references under original observation; BQ retains its own72 solely for the next
source-bound birth. Exited issuer births remain42..44 separately. Concurrent children
may continue already admitted ordinary work but have no72 descriptor or read right.
A child with a retained channel alias blocks every subsequent transaction. The
socketpair never becomes a new object for each stage: both physical endpoint originals
are71/72, every duplicate descriptor/cmsg/queue/buffer is charged. At most ONE original
FD is sent per record, fixed stage/sequence/object selector and no unsolicited rights;
MSG_CTRUNC/truncated records/extras/missing/changed peer/replay deny and preserve originals.
BQ marks the exact transfer attempt one-use BEFORE send/native effects, and verifies
receiver birth/token/image/entry, received fstat/rights/seals/member/EOF and original
independent readback BEFORE use. Sender retains the original through receiver archive
ACK/actual closure; copying descriptor numbers or digests grants no lease. A received
FD is a duplicate reference to the same original, not a new object or inferred identity.

Cuts: R0 provider/BQ41 -> admitted service45/observer48 native Q/case/capacity/source
objects and back to BQ for raw C1/C4/measurement originals; R1 BQ41 -> initial issuer42
complete original unsigned inputs, issuer42 -> BQ41 nonsecret outputs then real exit;
R2 is BQ's independent readback, no transport or new issuer birth; R3a BQ41 ->
credential issuer43 exact R3a/anchor/PKCS8/console/session/native originals, issuer43 ->
BQ41 credential outputs and actual exit; R3b BQ41 admits production controls before
production issuer44 birth, hands its complete tuple/C128/credential references to44;
R4 provider41 routes original consumer R4 through44 for signing,44 ->41 detached S/V,
then41 -> service45/primary46/writer47/observer48/Node-parent49 under each original
private entry before their effects. Each receiver is admitted serially with72 or its
already original caller channel; endpoints are actually closed before another receiver.
Private issuer copies remain charged to their genuine birth until exit, even after FD
transfer. Initial/nonsecret/credential/production/output/source journals retain each
channel/cmsg/send/receive/closure original observation under the preseal contract.

Windows uses DuplicateHandle with the original birth/token/loaded-image/file-ID/rights/
noninheritance and receiver correlation already required; Linux credential/SCM proof
never qualifies Windows and vice versa. Cross-birth native peer/rights/once-only/EOF/
archive/failure proofs are mandatory at EVERY cut above; missing additional object
capacity denies. This is a proposed reviewed transport choice, selecting no real
principal, tool, peer, socket, native credential or executable action now.

### Normative finite-B and exact production certificate cuts (R03-03)

Cut A: after R3a issuer43 genuinely exits, independently selected BQ retains original
P50/G51/Q64/capacity67/cases123/journal93 and all source/image/tool/credential provenance.
It derives a PRELIMINARY numeric B decision inside journal104 from complete original
traces PLUS reviewed finite source closure of EVERY unexercised owning path and overlap.
The decision is NOT C128, has no production-profileDigest, admits no production effects,
and never modifies C86. It binds exact original measurement tuple and closed derivation
rule, measured maxima and conservative source-owned reserves. Unknown closure denies.

Cut B: only after Cut A, BQ births original P52/G53/Q65/capacity66 and independently
admits their native objects/raw EOF/source/membership in stage R3b. Exact transformation:
retain identical selected issuer source/entry/image/tool/CRT/API/full Node/ICU, principals,
credential original+anchor, cases123 and all owner domains, lifetime/exit/retention/channel/
copy/stack/IO behavior; substitute ONLY qualification allocation ceilings with the
preliminary finite B in existing budget fields and exact source-derived capacity offsets/
counts fitting that B. New authorization/session IDs and production purpose are new
control bytes, never changed behavior. Fixed constructor, parser/codec, issuer enrollment,
signing and error paths are the SAME source for Q32 and production, including quota-
rejection paths. No production-only module/entry/helper/allocator/branch/channel or
credential-copy lifetime may enter by this transform. Q65 joins the new P52 digest,
capacity66 and held cases123; original P50/Q64/capacity67 remain unchanged.

Cut C: BQ independently compares every source/owner/control field at Cut B against
Cut A's explicit allowlist, reads actual new native controls and signs NO IA1 grant:
it emits a newly born original C128 bound to exact P52/Q65/capacity66 and original
journal93+certificate-journal104 lineage under its independent native authority.
C128 is derived certification, not a new observed production measurement. Conservative
maximumBytes is the maximum complete simultaneous owning trace/source-closure envelope,
INCLUDING production control constructor+P/G/Q/capacity parsing/objects, BQ certification/
new-old journal/channel/readback/metadata overlap, quota-error+failure retention, complete
Node/OpenSSL/credential/private copies and genuine process exit. Any reserve must have
reviewed source geometry/native ownership evidence; no guessed margin or summed unrelated
peaks. The source closure must establish that reducing allocator quota cannot increase
live backing/copy/control/failure overlap beyond that envelope. Otherwise no derivation:
retain UNKNOWN and require another independently admitted qualification tuple/cases.

BQ original readback verifies C128 and journal104 before R3b consumer effects; final
journal104 follows the same preseal105 -> immutable104 transition. R3b initial ROOT binds
already completed C128, never predicts it; all its production tuple inputs existed before
that ROOT. C13/P/Q grammars remain unchanged; derivation records live in existing original
evidence, not extra profile keys. No final C128 is prerequisite for its own P52 birth.
B0/B2 actual values remain UNKNOWN; this derivation is an unexecuted source mechanism.
Changed source/image/tool/owner/credential/anchor/case/API/entry/overlap/lifetime/domain or
nonallowlisted quota/control transformation denies, preserves old/new originals and needs
fresh original qualification. Whole positive fixture must show the three distinct cuts,
correct own-profile joins and complete conservative closure, plus every changed-tuple/
quota-failure/native-overlap rejection. The128 table reserves old/new P/Q/capacity/C128,
journal104/preseal105/readback/channel objects together before any effect.

## 5. Source inventory and review gates

Prospective dependent source: original-unit fixed-table reader/bounded graph walker,
original capture journal and stage-index-unit reader; native BQ bootstrap admission;
stage-specific exact index reader; issuer nonsecret/credential qualification and
independent BQ readback; native console/PKCS8 enrollment and detached issuer output;
original Windows/Linux brokered cross-birth handoff; verifier algorithm/range/stack/IO
integration; full authentic-versus-synthetic positive and negative fixtures.
These additions are absent/unimplemented, not a new permission under PA1/N2.

Review together all ten canonical sections, this appendix, all five intent/spec/
backlog/INIT/traceability mappings, complete actual dirty/committed source, entire
frozen02 candidate/review and historical failures/Node/verifier/tool/Git raw bodies.
No partial source/packaging/all-denial delivery claim. Distinct whole architecture
review precedes dependent source; authentic BQ/issuer/anchor/credential/native realm
selection is separate. Whole positive source+meaningful TC01..12/CA01..08 then
different final ENTIRE SOURCE GO/new complete input admission before execution.
Mac Deferred never PASS; publication and identical-byte Core/native/operator/TUI
acceptance remain separate. No release/GA/promotion/deployment/settings authority.
