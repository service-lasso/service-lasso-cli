#ifndef SLCLI_BQ_SOURCE_CONTROL_SINK_WINDOWS_H
#define SLCLI_BQ_SOURCE_CONTROL_SINK_WINDOWS_H
#include "source_unit12_native.h"
#include "../writer/capture_ranges_windows.h"
#include "../writer/capture_backing_windows.h"
#if defined(_WIN32)
struct slcli_writer_bq_record_windows;
struct slcli_bq_source_control_target_windows {
 const slcli_input_capture *source;
 const unsigned char *raw_original;
 struct slcli_windows_file_capture *copy;
 unsigned char *payload;
 const unsigned char *readonly_payload;
 size_t ordinal,bytes,payload_maximum;
 uint32_t attempted,copied;
};
struct slcli_bq_source_control_sink_windows {
 const struct slcli_writer_bq_record_windows *record_borrower;
 struct slcli_arena *arena;
 const struct slcli_writer_capture_factory_windows *factory;
 struct slcli_writer_capture_ranges_windows *ranges;
 struct slcli_bq_source_control_target_windows *targets;
 struct slcli_input_body *const *bodies;
 size_t body_count,count,first_ordinal,claimed,used;
 uint32_t started,prepared,failed;
};
/* All reachable mutable views must be disjoint from the entire actual arena
 * containing source/namespace/control blocks. This is geometry, not authority. */
int slcli_bq_source_control_sink_windows_view_original(
 struct slcli_arena *,const struct slcli_writer_capture_factory_windows *);
int slcli_bq_source_control_sink_windows_prepare_original(
 struct slcli_bq_source_control_sink_windows *,struct slcli_arena *,
 const struct slcli_writer_capture_factory_windows *,struct slcli_bq_source_control_target_windows *,
 size_t,struct slcli_input_body *const *,size_t);
int slcli_bq_source_control_sink_windows_matches_original(const struct slcli_bq_source_control_sink_windows *);
int slcli_bq_source_control_sink_windows_write(void *,const slcli_input_capture *,const unsigned char *,size_t);
/* Private exact deadline/body lane; no supplied ordinal or generic callback. */
int slcli_bq_source_control_sink_windows_write_owned(
 struct slcli_input_deadline_owner *,const slcli_input_capture *,const unsigned char *,size_t);
#endif
#endif
