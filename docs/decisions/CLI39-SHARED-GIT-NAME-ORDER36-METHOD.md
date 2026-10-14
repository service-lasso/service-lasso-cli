# CLI39 shared Git tree-name comparison source method36

Parameterised pure source construction proposal, dependent on shared edge ABI and independent complete A/unit/native graph. No source/graph/native/archive ACK or execution authority. Existing nine-file source_unit12.h/c pair only; no moving export copy. Tree comparator bodies wait for parent and Template78 exact ABI disposition.

## Exact proposed ABI

```c
struct slcli_source_unit12_git_name_order_state {
 const struct slcli_source_unit12_record *member_original;
 const struct slcli_source_unit12_git_edge *previous_original,*current_original;
 struct slcli_source_unit12_record member_value;
 struct slcli_source_unit12_git_edge previous_value,current_value;
 uint64_t previous_cursor,current_cursor,compared;
 uint64_t previous_difference,current_difference;
 uint32_t previous_byte,current_byte,previous_virtual,current_virtual;
 uint32_t need_previous,need_current,started,failed,ordered,finished;
 uint32_t purpose,previous_prefix;int raw_comparison;
};
int slcli_source_unit12_git_name_order_begin(
 struct slcli_source_unit12_git_name_order_state *,
 const struct slcli_source_unit12_record *same_tree_member,
 const struct slcli_source_unit12_git_edge *previous_retained,
 const struct slcli_source_unit12_git_edge *current_retained);
int slcli_source_unit12_git_name_prefix_begin(
 struct slcli_source_unit12_git_name_order_state *,
 const struct slcli_source_unit12_record *same_tree_member,
 const struct slcli_source_unit12_git_edge *candidate_retained,
 const struct slcli_source_unit12_git_edge *current_retained);
int slcli_source_unit12_git_name_order_windows(
 const struct slcli_source_unit12_git_name_order_state *,uint64_t actual_file_bytes,
 size_t positive_maximum,
 struct slcli_source_unit12_window *previous,
 struct slcli_source_unit12_window *current);
int slcli_source_unit12_git_name_order_feed(
 struct slcli_source_unit12_git_name_order_state *,
 uint64_t previous_relative,const unsigned char *previous_raw,size_t previous_bytes,
 uint64_t current_relative,const unsigned char *current_raw,size_t current_bytes,
 size_t *previous_consumed,size_t *current_consumed);
int slcli_source_unit12_git_name_prefix_finish(
 struct slcli_source_unit12_git_name_order_state *);
int slcli_source_unit12_git_name_order_finish(
 struct slcli_source_unit12_git_name_order_state *);
```

## Origin, windows and refusal

Begin requires fresh zeroed caller-owned state and completely disjoint original tree member/previous edge/current edge/state. Both edges must be TREE_ENTRY of the SAME original member's source_project, checked edge and raw-name intervals within member.bytes, name_bytes positive, allowed actual mode and matching target kind, previous edge complete before current edge begins. It retains exact original addresses and initialized fieldwise snapshots; no uninitialized padding comparison. Caller independently owns actual edge-to-original-tree association, full native read/capture/digest/parser evidence and source/target-project authority. Shape alone supplies none of them. Every later operation rechecks SAME retained fieldwise originals. No directory/tree mode can select a native path.

Windows is PURE geometry, zero native IO. Positive maximum is mandatory, even when one name has reached its end. Each result retains member_index/member_offset, relative_offset=name_offset+exact respective cursor, checked source_offset=member.offset+relative_offset, and requested=min(name remaining, maximum). Ended name requested0 is explicit virtual-terminator geometry, never a native read success. Full member/name/actual_file_bytes conversions/additions/interval arithmetic and output/state/original/output-pair byte-disjointness precede writes. Both outputs are local temporaries committed only after complete success; refusal preserves both. Feed consumes only exact original returned spans beginning at each current raw-name relative cursor. An unfinished name requires positive actual bytes within its remaining extent; an ended name requires bytes0/rawNULL. A native short read advances only its actual returned amount. Caller may retain unconsumed suffixes and feed them next without another native read; no hidden comparator reread, callback or read-all.

Feed status0 is refusal; after a valid fully disjoint storage seam, semantic/cursor/order/duplicate failure is absorbing with both consumed outputs unchanged. Invalid storage/alias/pointer-overflow seams refuse before any mutation, preserving aliased outputs/state. Status1 is strict progress needing another pair/one-side window: previous_consumed+current_consumed is positive, each <=actual supplied span, and state.need_previous/current states which unfinished side exhausted its supplied original window. Status2 records strict previous-before-current order at its actual first differing byte or exact virtual terminator and retains the compared cursors/bytes/virtual flags in state; at least one real supplied byte was consumed. The unconsumed suffix remains caller-owned unchanged. Feed cannot emit a hidden edge or overwrite an edge row. Further Feed after ordered/failed/finished refuses. Finish succeeds only after actual Feed2 and unchanged SAME origins, then once-closes structural comparison; it never grants graph/native/source/archive acceptance or permits row retirement. No zero-input Feed invents progress.

## Exact Git order and duplicate rule

Compare unsigned original raw name bytes lexicographically until first difference. For a raw name ending first, virtual comparison byte is slash47 for directory mode0040000 and NUL0 for every other mode. This is Git tree ordering only. If BOTH raw-name cursors reach their exact lengths after equal raw prefix, the names are duplicates and MUST refuse independently of mode BEFORE any virtual-suffix ordering. Thus file foo versus tree foo cannot evade duplicate denial. Raw NUL/slash inside a supplied name range is refusal; exact original parsed name offset/length determines its terminator, not an incoming C string. Dot/dotdot name validation belongs to the agreed full edge parser and must remain source-joined; this comparator cannot substitute for complete edge parsing. Only the prefix necessary to establish ordering is compared; all remaining original tree/name/body bytes are still independently consumed by complete all-member native/parser passes. No raw-name digest or scalar equality substitutes for byte comparison.

## Real native receipts, retention and full cost

The comparator does not read native files. The actual owning caller preowns separate original native observe/read/whole-fileEOF/reobserve receipts and distinct pair outputs for every comparison window before effects, consuming SAME originalD and actual predecessor. Existing2U+4N+7 native two-whole-pass geometry DOES NOT cover these comparison reads. Preown complete actual row-byte costs from original counts and raw name bytes, plus comparison states/feed receipts, retained edge rows and runtime/producer/kernel overlap; no arbitrary name/member/parent cap, uncharged stack, guessed R or row dropping. Empty directory/unit, final names, prefix collisions and arbitrarily long names retain finite complete source-derived geometry UNKNOWN until independently measured full fit. Every pending/partial/expired/exhausted native attempt remains held through real last borrower; pure comparator progress/Finish never reuses output or settles archive/RANGE/retirement.

Caller must separately reconstruct inherited own-writer/birth graph. Provider child KEEP allowlist (including std0/1/2 when kept) requires actual source-owned every descriptor/actor/rights/native identity retained at birth and returning verification. Endpoint/count/procRoot/capabilities and unit-vs-output identity checks cannot prove allowlist completeness. No extra offers/FDs or all-readonly Boolean is introduced here. Complete publication/producer/writable alias/kernel/native lifetime graph remains independently required and closed.

## UNRUN regression cases

Pair-window split at every byte, unequal returned span sizes, one name ending before the other, directory slash versus raw punctuation, equal raw names across differing modes, malformed cursor/extra bytes/NUL/slash, modified/different-address originals, output alias unchanged, pointer/uint64 conversions, zero native short return and receipt exhaustion. Every source output/native observation remains retained. No execution/build/compiler/tests/export/freeze/wholeGO follows this method.

## Complete raw-name uniqueness: persistent candidate source graph

Adjacent Git ordering is NOT complete raw-name uniqueness. The concrete sequence file foo, file foo.bar, tree foo is Git-order sorted but contains a duplicate raw foo. Primary Git fsck.c496-577 describes this semantic case and candidate-stack approach (https://raw.githubusercontent.com/git/git/master/fsck.c); moving research is NOT selected source ancestry or copied code. This method therefore requires both complete adjacent order AND complete candidate checks before a tree can finish. The pure pair order helper alone must never claim uniqueness.

After adjacent strict order, if the previous raw name ended (non-directory virtualNUL0) while the next actual current byte lies strictly between0 and47, retain the previous non-directory edge as a candidate. When a current directory's virtual slash47 follows an actual previous raw byte strictly between0 and47, pop retained candidates and compare each candidate's complete raw name prefix against current directory's raw name through separately owned original native windows. If current does not start with candidate, discard only structural stack progress; retained candidate/original/receipts remain held. Exact equal raw names is duplicate refusal. If candidate is a strict prefix and the next actual current byte is between0 and47, push the SAME candidate back through a NEW immutable persistent stack node and stop; otherwise continue pops. All adjacency order checks still apply. No count or digest replaces these byte comparisons.

Exact additional PrefixBegin ABI above initializes purpose2; OrderBegin initializes purpose1. Both use the SAME window/Feed streaming core, with SAME original fieldwise references and checked member-relative inputs (Feed relative=name_offset+respective raw cursor). In purpose2, Feed2 means the raw prefix relation is determined, not strict Git order: state.previous_prefix records whether current starts with the entire previous candidate name, raw_comparison records unsigned raw lexical -1/0/+1, and current_byte/current_difference retain the actual next current byte when candidate is a strict prefix. Purpose2 uses rawNUL terminator, without directory virtual slash. Exact equal names yields raw_comparison0/previous_prefix1 to the caller, which MUST refuse duplicate before any source/graph/archive use. PrefixFinish requires actual purpose2 Feed2 and same unchanged originals; OrderFinish requires purpose1 strict-order Feed2. A purpose mismatch refuses. Source caller owns the persistent stack/duplicate decision and actual receipts; pure purpose flags are not source authority. Existing state.ordered means structural comparison decided for purpose2 only and never asserts Git order/uniqueness in that mode.

Let E be the complete source-derived maximum tree-entry count (derived before effects from actual complete SourceUnit byte/count geometry; every permitted raw tree entry has at least28 bytes, so floor(unit_bytes/28) is a conservative bound without dropping entries). Complete per-tree original table rows independently reconstruct actual entries and partition monotonic preowned capacity. Initial candidate pushes<=E, stop-and-pushback events<=E, permanent candidate pops<=E, candidate prefix comparisons<=2E; adjacent comparisons<=E. Preown <=2E immutable persistent stack nodes, each exact original edge+prior-node link retained, plus distinct actual operation/feed/finish receipts. Pop never overwrites a node or reuses its output. A fresh node for pushback avoids overwriting earlier stack history. Every native prefix/adjacent comparison window/read/EOF/reobserve receipt and original output has ADDITIONAL checked source-derived capacity/row-byte/native overlap/originalD cost beyond old2U+4N+7. Native short reads consume genuine fresh rows and exhaustion remains absorbing; no guessed attempt count or fit claim. These algebraic bounds do not supply actual native memory/R/physical fit, which remains UNKNOWN.

Actual caller must prove candidate algorithm execution over every tree entry and every required candidate pop, SAME original comparison results and immutable node links, all-members/native/source/auth/lifetime closure. No standalone comparator result, empty candidate stack or structural Finish grants source/graph/archive authority. Complete regression adds nonconsecutive foo/foo.bar/foo-directory duplicates, nested foo.bar/foo.bar.baz/foo.bar-directory/foo-directory collisions, differing high/low punctuation, pushback across multiple directory cuts and native/candidate receipt exhaustion. No comparator body before exact shared disposition.

CLI36 comparator construction cut 2026-10-12: parent ENTIRE selected method7812120B parameterized pure ABI and Template78 ENTIRE acknowledged SAME ABI before bodies. Implement only existing source_unit12.h/c pair; structural origins/windows/paired Feed/purpose Finish, transactional refusal and complete separate candidate uniqueness remain mandatory. E/2E bounds are PER complete traversal: two actual traversals require separate partitions and twice the nodes/comparison receipts, never reuse first-pass originals. Additional native short-read/EOF/reobserve/reservation/row-byte/originalD costs and Layer1/native foundation representation remain UNKNOWN/closed. Purpose2 raw equality must cause real caller duplicate denial. No activation, export, freeze, fullGO or execution; all regression cases UNRUN.
