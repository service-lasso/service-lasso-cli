/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_SOURCE_UNIT12_H
#define SLCLI_SOURCE_UNIT12_H
#include <stddef.h>
#include <stdint.h>
/* Streaming representation, separate from unit19's fixed two-member compound.
 * The owner retains every actual raw read/capture and authentic A association.
 * Structural acceptance is never source/member/actor/native admission. */
struct slcli_source_unit12_record {
 unsigned char association[32],capture[32],digest[32];
 uint64_t offset,bytes,index;
 uint32_t kind,source_project;
};
struct slcli_source_unit12_reader {
 unsigned char header[64],attempted_record[128];
 struct slcli_source_unit12_record previous;
 uint64_t count,file_bytes,body_offset,next_offset,next,attempted;
 uint32_t started,failed,table_complete;
};
/* Checked structural routing only. Native caller independently owns actual
 * returned-byte cursor, whole member/readback/digest and retained Layer0 relation. */
struct slcli_source_unit12_window {
 uint64_t member_index,member_offset,relative_offset,source_offset;
 size_t requested;
};
/* Pure streaming Git edges. source_project identifies the ORIGINAL member;
 * independent source/publication relations must select the target project.
 * Caller preowns and retains distinct edge rows and every native raw window. */
enum { SLCLI_SOURCE_GIT_ROOT_TREE=1,SLCLI_SOURCE_GIT_PARENT=2,
 SLCLI_SOURCE_GIT_TREE_ENTRY=3,SLCLI_SOURCE_GIT_TAG_TARGET=4 };
struct slcli_source_unit12_git_edge {
 uint32_t source_project,relation,target_kind,mode;
 unsigned char object_id[20];
 uint64_t edge_offset,edge_bytes,name_offset,name_bytes;
};
struct slcli_source_unit12_git_edge_state {
 uint64_t member_bytes,consumed,line_offset,line_bytes;
 uint64_t edge_offset,name_offset,name_bytes,emitted;
 uint32_t kind,source_project,phase,mode,hex_digits,oid_bytes;
 uint32_t prefix_bytes,header_class,target_kind,parents_closed;
 uint32_t started,failed,complete,pending;
 const struct slcli_source_unit12_git_edge *pending_original;
 struct slcli_source_unit12_git_edge pending_value;
 unsigned char prefix[7],object_id[20],name_prefix[2];
};
/* Pure pair comparison. Complete tree uniqueness/candidate graph is caller-owned. */
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
#ifdef __cplusplus
extern "C" {
#endif
int slcli_source_unit12_header(struct slcli_source_unit12_reader *,
 const unsigned char[64],uint64_t);
int slcli_source_unit12_record(struct slcli_source_unit12_reader *,
 const unsigned char[128]);
/* Refusal preserves the complete output. Empty relative0/requested0 is a
 * structural result only; a nonempty end cursor is not a native read window. */
int slcli_source_unit12_window_geometry(const struct slcli_source_unit12_record *,
 uint64_t,uint64_t,size_t,struct slcli_source_unit12_window *);
int slcli_source_unit12_git_edge_begin(struct slcli_source_unit12_git_edge_state *,
 const struct slcli_source_unit12_record *);
/* 0 refusal (outputs unchanged), 1 progress, 2 one pending edge. Invalid
 * storage/disjointness seams refuse before mutation; valid-seam semantic,
 * cursor and pending failures absorb. Positive calls return exact consumed
 * suffix bytes; no hidden edge at Finish. */
int slcli_source_unit12_git_edge_feed(struct slcli_source_unit12_git_edge_state *,
 uint64_t,const unsigned char *,size_t,size_t *,struct slcli_source_unit12_git_edge *);
/* SAME retained address/value, fieldwise; structural progress only, no ACK. */
int slcli_source_unit12_git_edge_resume(struct slcli_source_unit12_git_edge_state *,
 const struct slcli_source_unit12_git_edge *);
int slcli_source_unit12_git_edge_finish(struct slcli_source_unit12_git_edge_state *);
int slcli_source_unit12_git_name_order_begin(struct slcli_source_unit12_git_name_order_state *,
 const struct slcli_source_unit12_record *,const struct slcli_source_unit12_git_edge *,
 const struct slcli_source_unit12_git_edge *);
int slcli_source_unit12_git_name_prefix_begin(struct slcli_source_unit12_git_name_order_state *,
 const struct slcli_source_unit12_record *,const struct slcli_source_unit12_git_edge *,
 const struct slcli_source_unit12_git_edge *);
int slcli_source_unit12_git_name_order_windows(const struct slcli_source_unit12_git_name_order_state *,
 uint64_t,size_t,struct slcli_source_unit12_window *,struct slcli_source_unit12_window *);
int slcli_source_unit12_git_name_order_feed(struct slcli_source_unit12_git_name_order_state *,
 uint64_t,const unsigned char *,size_t,uint64_t,const unsigned char *,size_t,size_t *,size_t *);
int slcli_source_unit12_git_name_prefix_finish(struct slcli_source_unit12_git_name_order_state *);
int slcli_source_unit12_git_name_order_finish(struct slcli_source_unit12_git_name_order_state *);
/* Index is zero-based table routing, not a claimed paid sourceMember/actor. */
#ifdef __cplusplus
}
#endif
#endif
