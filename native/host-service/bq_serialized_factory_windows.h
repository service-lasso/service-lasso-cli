#ifndef SLCLI_BQ_SERIALIZED_FACTORY_WINDOWS_H
#define SLCLI_BQ_SERIALIZED_FACTORY_WINDOWS_H
#include "bq_source_association_windows.h"
#include "bq_source_unit_windows.h"
#if defined(_WIN32)
struct slcli_bq_factory_block_windows {
 void *value;size_t requested;uint64_t header_offset,bytes,next,state;
};
struct slcli_bq_factory_subject_windows {
 const void *value;size_t bytes,offset;
 const struct slcli_bq_factory_block_windows *block;
};
struct slcli_bq_factory_bootstrap_windows;
/* Retained original caller, never a generic allocator/lock token. No teardown,
 * detach, retry or unlock API is supplied. Whole last-borrower closure is owed. */
struct slcli_bq_serialized_factory_windows {
 struct slcli_arena *arena;
 struct slcli_memory_owner *owner_original;
 struct slcli_bq_factory_bootstrap_windows *bootstrap_original;
 uintptr_t extent_base_original;
 uint64_t extent_bytes_original,acquisition_original;
 uint32_t extent_original;
 const struct slcli_source_deadline_original *deadline;
 const void *installer_original;size_t installer_bytes;
 const struct slcli_native_clock *prior_original;
 const struct slcli_bq_source_association_windows_inputs *association_template;
 const struct slcli_bq_source_unit_windows_inputs *unit_template;
 SRWLOCK lock;
 uintptr_t init_operand,try_operand;
 DWORD issuing_thread;
 BOOLEAN try_raw;
 uint32_t init_called,init_returned,thread_called,thread_returned;
 uint32_t try_called,try_returned,acquired,failed;
 struct slcli_native_clock clocks[4][2];
 int clock_result[4][2];uint32_t clock_returned[4][2];
 uint64_t tail_offset,chain_headers,joined_headers;
 struct slcli_bq_factory_block_windows *blocks;
 const struct slcli_bq_factory_block_windows *caller_block,*roster_block;
 struct slcli_bq_factory_subject_windows subjects[7];
 size_t block_capacity,block_count;
 size_t association_count,unit_count,association_targets,unit_targets;
 struct slcli_source_association_native *association_table;
 struct slcli_source_unit12_native *unit_table;
 struct slcli_input_body **association_bodies,**unit_bodies;
 struct slcli_source_association_native_range *association_ranges;
 struct slcli_source_unit12_native_range *unit_ranges;
 struct slcli_bq_source_control_sink_windows *association_sink,*unit_sink;
 struct slcli_bq_source_control_target_windows *association_target_rows,*unit_target_rows;
 struct slcli_bq_source_association_windows_inputs *association_inputs;
 struct slcli_bq_source_unit_windows_inputs *unit_inputs;
 struct slcli_bq_source_association_windows *association_original;
 struct slcli_bq_source_unit_windows *unit_original;
 uint32_t source_borrow,storage_prepared,association_returned,unit_returned;
 int association_result,unit_result;
};
struct slcli_bq_factory_bootstrap_windows {
 struct slcli_arena *arena_original;
 struct slcli_memory_owner *owner_original;
 const struct slcli_source_deadline_original *deadline_original;
 const void *installer_original;size_t installer_bytes;
 uint64_t chain_headers,tail_offset,required_bytes;
 uintptr_t caller_address;
 struct slcli_bq_serialized_factory_windows *caller_original;
 uint32_t started,attached,caller_header_written,caller_zeroed,returned;
 int result;
};
/* Parameterized native source construction only. Genuine unpublished owner /
 * installer initialization-window/source admission precedes this constructor;
 * neither storage/lock success nor table observation supplies that admission. */
int slcli_bq_serialized_factory_windows_construct_original(
 struct slcli_bq_factory_bootstrap_windows *,struct slcli_arena *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,
 const struct slcli_bq_source_association_windows_inputs *,
 const struct slcli_bq_source_unit_windows_inputs *);
/* Fresh exact header/storage correspondence only. This does not check the
 * currently executing native thread or admit source IO/provider/caller. */
int slcli_bq_serialized_factory_windows_body_storage_matches(
 const struct slcli_input_deadline_owner *);
#endif
#endif
