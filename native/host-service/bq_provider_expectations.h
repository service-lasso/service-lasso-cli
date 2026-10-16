/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_PROVIDER_EXPECTATIONS_H
#define SLCLI_BQ_PROVIDER_EXPECTATIONS_H
#include <stddef.h>
#include <stdint.h>
#include "bq_subject_transfer.h"
/* Private immutable source expectations, NEVER a serialized capability or an
 * admitted original. No instance/default principal is supplied by this header.
 * An independently reviewed installer/destination occurrence must acquire the
 * native inputs and verify them with fixed source code. Policy bytes, linkage,
 * readonly storage, a nonnull pointer or zero-initialized UID cannot select that
 * occurrence/principal, grant financing, or establish authentic membership. */
/* Constraints-only native installed policy representation. No default instance,
 * locator, digest authority or reservation is defined here. */
/* Complete source resource recipe constraints, not acquired backing, finance,
 * source authentication or an OS allocation/scheduling-success guarantee.
 * User/kernel/cache RAM are disjoint charged categories; VM/storage separate. */
struct slcli_bq_resource_cost {
 uint64_t user_ram_bytes,user_virtual_bytes,kernel_ram_bytes,cache_ram_bytes;
 uint64_t retained_storage_bytes,native_references,physical_extent_events;
 uint64_t native_attempts,pure_attempts,cpu_nanoseconds;
};
struct slcli_bq_owner_resource_profile {
 struct slcli_bq_resource_cost simultaneous;
 uint64_t combined_memory_bytes;
};
enum { SLCLI_BQ_RETENTION_POLICY_BYTES=168,SLCLI_BQ_RETENTION_POLICY_VERSION=1 };
struct slcli_bq_retention_policy_values {
 uint64_t maximum_bytes,maximum_record_bytes,partial_attempts,failure_tail_bytes;
 uint64_t whole_lineage_bytes,whole_lineage_attempts,whole_lineage_nodes;
 uint64_t records_offset,records_bytes,index_offset,index_bytes;
 uint64_t terminal_offset,terminal_bytes,native_cell_bytes;
 uint64_t source_window_bytes,write_window_bytes,readback_window_bytes;
 uint64_t pair_window_bytes,active_index_bytes;
};
struct slcli_bq_destination_retention_constraints {
 const unsigned char *parent_path;size_t parent_path_bytes;
 const unsigned char *policy_name;size_t policy_name_bytes;
 uint64_t policy_body_bytes,policy_read_rows;
 unsigned char policy_body_digest[32],policy_source_project[32],policy_source_association[32];
 uint64_t maximum_bytes,maximum_record_bytes,partial_attempts,failure_tail_bytes;
 uint64_t whole_lineage_bytes,whole_lineage_attempts,whole_lineage_nodes;
 uint64_t records_offset,records_bytes,index_offset,index_bytes;
 uint64_t terminal_offset,terminal_bytes,native_cell_bytes;
 uint64_t source_window_bytes,write_window_bytes,readback_window_bytes;
 uint64_t pair_window_bytes,active_index_bytes;
};
enum slcli_bq_expected_platform {
 SLCLI_BQ_EXPECTED_WINDOWS=1,SLCLI_BQ_EXPECTED_LINUX=2
};
struct slcli_bq_expected_principal {
 uint32_t platform;
 /* Windows native SID bytes and LUID low/high DWORD pairs. All are compared
  * to the actual designated effective-token occurrence; no optional mask. */
 const unsigned char *windows_sid;
 size_t windows_sid_bytes;
 uint32_t windows_token_id[2],windows_authentication_id[2];
 /* Linux actual real/effective identities, kept distinct from raw message
  * credentials and never inferred from another actor's current UID/GID. */
 uint32_t linux_real_uid,linux_effective_uid,linux_real_gid,linux_effective_gid;
};
struct slcli_bq_expected_image_entry {
 uint64_t member;
 uint32_t source_project,image_format;
 uint64_t image_bytes,entry_file_offset,entry_file_bytes;
 uint64_t entry_virtual_locator,entry_virtual_bytes;
 /* Format1 PE: virtual locator is RVA relative to actual image base.
  * Format2 ELF64-LE: locator is link-time VA, joined through actual PT_LOAD
  * file/virtual geometry and independently observed load bias. Neither value
  * is a child pointer, an actual mapping or independently acquired entry. */
 const unsigned char *entry_symbol;
 size_t entry_symbol_bytes;
 /* entry_source_digest is T2 entrySourceSha256. It does NOT digest executable
  * bytes. entry_image_digest covers exactly the distinct selected file span.
  * Qualified source-built output derives complete file/virtual/symbol geometry;
  * never guess function size or treat a supplied RVA as source correspondence. */
 unsigned char source_association[32],image_digest[32],entry_source_digest[32],entry_image_digest[32];
};
/* Private source-selected acquisition geometry, not a wire/native authority
 * class. Never infer one of these from reader_role or subject family. */
enum slcli_bq_expected_source_reader {
 SLCLI_BQ_EXPECTED_LOOSE_NATIVE_SUBJECT=1,
 SLCLI_BQ_EXPECTED_A_PREREQUISITE=2,
 SLCLI_BQ_EXPECTED_UNIT_PREREQUISITE=3,
 SLCLI_BQ_EXPECTED_SP1_MEMBER_RANGE=4,
 SLCLI_BQ_EXPECTED_ARCHIVE_FOUNDATION_PREREQUISITE=5,
 SLCLI_BQ_EXPECTED_ARCHIVE_READBACK_PREREQUISITE=6
};
struct slcli_bq_expected_occurrence_selector {
 uint32_t phase,command,requirement;
};
struct slcli_bq_expected_source_occurrence {
 uint32_t reader_class;
 uint64_t native_file_bytes,read_offset,read_bytes;
 /* Exact actual earlier ordinary requirement occurrences, not new offers.
  * A itself has no self-indexing A/unit relation. Its two unused selectors
  * use UINT32_MAX in ALL fields; this exception does not select an owner.
  * Unit needs its independently held A; unit's unit selector is unused.
  * In this selected full-provider route, LOOSE_NATIVE_SUBJECT and
  * SP1_MEMBER_RANGE require both actual earlier A and unit occurrences.
  * No implicit independently authenticated no-SP1 fallback is represented.
  * Every real selector is independently joined to its actual native owner. */
 struct slcli_bq_expected_occurrence_selector association_index,source_unit;
 /* Private native pair selectors; ordinary reader classes use ALL UINT32_MAX.
  * These are local source geometry, never transported owner/capability pointers. */
 struct slcli_bq_expected_occurrence_selector archive_foundation,archive_readback;
};
struct slcli_bq_expected_subject {
 /* This selector constrains an actually acquired native membership. Neither
  * member nor the zero-based source table index below creates that membership. */
 struct slcli_bq_provider_subject_selector selector;
 uint32_t phase,command,requirement;
 uint32_t source_project,source_record_kind;
 uint64_t source_record_index,native_rights;
 struct slcli_bq_expected_source_occurrence source_occurrence;
 unsigned char source_association[32],source_capture[32],body_digest[32];
 /* Exact local reader role, separate from native family. Preserve all actual
  * reader occurrences, including compiler inputs/config/Git/root controls.
  * Bytes exclude any terminator; the source owner retains the full span. */
 const unsigned char *reader_role;
 size_t reader_role_bytes;
};
struct slcli_bq_provider_expectations {
 struct slcli_bq_expected_principal principal;
 struct slcli_bq_expected_image_entry producer;
 unsigned char source_root_association[32],source_unit_association[32],source_unit_digest[32];
 uint64_t source_unit_bytes,source_unit_record_count;
 /* Exact source-produced count, not floor((bytes-64)/128). The consumer
  * checks 64+128*count without overflow and against actual header/table/file
  * geometry before traversal; equality does not authenticate the unit or A. */
 uint64_t association_index_bytes,association_index_record_count,association_index_names_bytes;
 /* Independently source-produced A geometry before preownership, NEVER
  * derived from received A/header/file length. Positive count equals the
  * complete unit record count; namesBytes>=count; checked128+320*count+
  * namesBytes equals association_index_bytes. Class2's actual native extent
  * and complete read span must match. Exact bounded name-window backing is
  * preowned independently before IO; these constraints create no authority. */
 /* Actual complete command/requirement order. These arrays and every nested
  * role/SID span belong to the same source expectation occurrence. Independent
  * destination decoder association is checked before receiving each candidate.
  * Raw candidate member/family remains unauthenticated until native/source joins. */
 const uint32_t *requirements_per_command;
 size_t command_count;
 const struct slcli_bq_expected_subject *subjects;
 size_t subject_count;
};
/* No transfer encoding, success field, public registration, constructor or
 * callback verifier exists here. Sender-local native owners are not accepted
 * across processes. Existing64-byte OFFER is routing only; destination owns its
 * pidfd/token/held image/entry/source/readback originals and all partial outputs.
 * Exact reviewed source export/build closure is required before consumers link.
 * This header does not implement the still-missing authentic acquisition body. */
/* Pure Linux bootstrap correspondence only. No writes, native effects, authority
 * or output count; caller independently owns exact decoder/source occurrence. */
static inline int slcli_bq_expected_selector_unused(
 const struct slcli_bq_expected_occurrence_selector *s){
 return s->phase==UINT32_MAX&&s->command==UINT32_MAX&&s->requirement==UINT32_MAX;
}
static inline int slcli_bq_expected_digest_unused(const unsigned char *p){
 size_t i;for(i=0;i<32;i++)if(p[i])return 0;return 1;
}
static inline int slcli_bq_provider_expectations_bootstrap_shape(
 const struct slcli_bq_provider_expectations *e,uint32_t actual_phase){
 size_t command,index=0;uint32_t requirement;uint64_t bytes=0;
 if(!e||e->principal.platform!=SLCLI_BQ_EXPECTED_LINUX||!e->requirements_per_command||
  !e->subjects||!e->command_count||e->subject_count<2||e->subject_count==SIZE_MAX||
  e->requirements_per_command[0]<2)return 0;
 for(command=0;command<e->command_count;command++){
  if(command>UINT32_MAX)return 0;
  for(requirement=0;requirement<e->requirements_per_command[command];requirement++){
   const struct slcli_bq_expected_subject *s;
   const struct slcli_bq_expected_source_occurrence *o;
   if(index>=e->subject_count)return 0;s=&e->subjects[index];o=&s->source_occurrence;
   if(s->phase!=actual_phase||s->command!=(uint32_t)command||s->requirement!=requirement)return 0;
   if(index<2){
    uint32_t expected=index==0?SLCLI_BQ_EXPECTED_ARCHIVE_FOUNDATION_PREREQUISITE:
     SLCLI_BQ_EXPECTED_ARCHIVE_READBACK_PREREQUISITE;
    if(command||requirement!=(uint32_t)index||o->reader_class!=expected||
     s->selector.family!=SLCLI_BQ_SUBJECT_SOURCE||!s->selector.member||!s->selector.member_bytes||
     o->native_file_bytes!=s->selector.member_bytes||o->read_offset||o->read_bytes||
     !slcli_bq_expected_selector_unused(&o->association_index)||
     !slcli_bq_expected_selector_unused(&o->source_unit)||
     o->archive_foundation.phase!=actual_phase||o->archive_foundation.command||o->archive_foundation.requirement||
     o->archive_readback.phase!=actual_phase||o->archive_readback.command||o->archive_readback.requirement!=1||
     s->source_record_kind||s->source_record_index||s->reader_role||s->reader_role_bytes||
     !slcli_bq_expected_digest_unused(s->source_association)||
     !slcli_bq_expected_digest_unused(s->source_capture)||
     !slcli_bq_expected_digest_unused(s->body_digest))return 0;
    if(index&&bytes!=o->native_file_bytes)return 0;bytes=o->native_file_bytes;
   }else if(o->reader_class<SLCLI_BQ_EXPECTED_LOOSE_NATIVE_SUBJECT||
    o->reader_class>SLCLI_BQ_EXPECTED_SP1_MEMBER_RANGE||
    !slcli_bq_expected_selector_unused(&o->archive_foundation)||
    !slcli_bq_expected_selector_unused(&o->archive_readback))return 0;
   index++;
  }
 }
 return index==e->subject_count;
}
#endif
