#ifndef SLCLI_INSTALLED_LAUNCH_WINDOWS_H
#define SLCLI_INSTALLED_LAUNCH_WINDOWS_H
#include "launcher_native.h"
#if defined(_WIN32)
/* Source-selected production composition inputs, all already owned by the
 * original Service arena. These routes are not an installation admission. */
struct slcli_installed_launch_windows_inputs {
 const struct slcli_runtime_source_windows_inputs *source;
 const struct slcli_runtime_source_windows_claim_inputs *claims;
 const struct slcli_node_prefix_plan *prefix;
 const void *transport_descriptor;size_t transport_descriptor_bytes;
 const struct slcli_launcher_windows_inputs *launcher;
};
struct slcli_installed_launch_windows {
 struct slcli_arena *arena;
 const struct slcli_installed_launch_windows_inputs *inputs_original;
 const struct slcli_binding_originals *book_original;
 struct slcli_memory_owner *runtime_original;
 const struct slcli_source_deadline_original *deadline;
 const struct slcli_native_clock *prior_original,*origin_original;
 const void *caller;size_t caller_bytes;
 uint64_t origin_tick,expires,prior_tick;
 struct slcli_runtime_source_windows *source;
 struct slcli_node_prefix_original *prefix;
 struct slcli_node_transport_windows *transport;
 struct slcli_launcher_windows_capture *launcher;
 struct slcli_launcher_windows_inputs *launch_inputs;
 HANDLE *inherited;size_t inherited_count;
 /* Seven actual outer allocations only. Nested source/transport/launcher,
  * native mappings, borrowed inputs and failure overlap are additional costs. */
 size_t own_control_bytes;
 struct slcli_native_clock allocated_after,source_after,prefix_after,transport_after,finish;
 int allocated_after_result,source_result,source_after_result,prefix_result,prefix_after_result;
 int transport_result,transport_after_result,launcher_result,finish_result;
 uint32_t source_returned,prefix_returned,transport_returned,launcher_returned;
 uint32_t first_failure,complete;
};
/* Returns the actual partial owner even on later failure. NULL means no owner
 * block was obtained; no native birth is attempted in that case. Success is a
 * suspended birth only. Independent installer/source/native admission and the
 * separately guarded resume/earliest-root path remain required. Caller owns
 * serialization and brackets the initial arena allocation in SAME deadline. */
struct slcli_installed_launch_windows *slcli_installed_launch_windows_create_original(
 struct slcli_arena *,const struct slcli_binding_originals *,struct slcli_memory_owner *,
 const struct slcli_source_deadline_original *,const void *,size_t,
 const struct slcli_native_clock *,const struct slcli_installed_launch_windows_inputs *);
#endif
#endif
