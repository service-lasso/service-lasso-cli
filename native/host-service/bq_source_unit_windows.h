#ifndef SLCLI_BQ_SOURCE_UNIT_WINDOWS_H
#define SLCLI_BQ_SOURCE_UNIT_WINDOWS_H
#include "bootstrap_qualification_admission.h"
#include "bq_source_namespace_windows.h"
#include "source_unit12_native.h"
#include "bq_source_control_sink_windows.h"
#if defined(_WIN32)
struct slcli_bq_source_unit_windows_inputs {
 const struct slcli_bq_windows_identity_original *identity;
 const WCHAR *path;size_t path_bytes;
 struct slcli_bq_source_namespace_windows *namespace_original;
 WCHAR *path_copy,*root;
 struct slcli_installed_namespace_component_windows *components;
 struct slcli_bq_source_namespace_target_windows *targets;
 size_t component_count;
 struct slcli_source_unit12_native *table;
 struct slcli_input_body *const *bodies;
 struct slcli_source_unit12_native_range *ranges;
 size_t range_count;uint64_t file_bytes,record_count;
 const struct slcli_writer_capture_factory_windows *capture_factory;
 struct slcli_bq_source_control_sink_windows *capture_sink;
 struct slcli_bq_source_control_target_windows *capture_targets;
 size_t capture_target_count;
};
struct slcli_bq_source_unit_windows {
 struct slcli_arena *arena;
 const struct slcli_bq_source_unit_windows_inputs *inputs;
 const struct slcli_source_deadline_original *deadline;
 const void *caller;size_t caller_bytes;
 struct slcli_native_clock joint_after,finish;
 int sink_result,prepare_result,joint_result,namespace_result,read_result,finish_result;
 uint32_t sink_returned;
 uint32_t prepare_returned,joint_returned,namespace_returned,read_returned,finish_returned;
 uint32_t started,first_failure,observed;
};
/* Actual source-object/table observation composition. This is NOT the private
 * qualification admission entry: selected principal/source/Q/P/capacity/root
 * and prior source reservation must independently precede its invocation. */
int slcli_bq_source_unit_windows_observe_original(struct slcli_bq_source_unit_windows *,
 struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_bq_source_unit_windows_inputs *);
#endif
#endif
