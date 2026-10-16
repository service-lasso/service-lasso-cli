#ifndef SLCLI_LAUNCHER_NATIVE_H
#define SLCLI_LAUNCHER_NATIVE_H
#include "source_deadline.h"
#include "owner_handoff_codec.h"
#include "preentry_backing_windows.h"
#include "runtime_containing_windows.h"
#include "../node-owner/transport_windows.h"
#include "../node-owner/selector_windows.h"
#include "../node-owner/route_windows.h"
#if defined(_WIN32)
#include <windows.h>
/* Explicit installed image/transport inputs. Paths, command text and inherited
 * handles route native creation; none of them grants actor/source authority. */
struct slcli_launcher_windows_inputs {
  const WCHAR *application,*command,*environment,*directory;
  size_t application_bytes,command_bytes,environment_bytes,directory_bytes;
  /* Original self-relative private descriptors. Native capability admission
   * is independent of copying. */
  const void *process_descriptor,*thread_descriptor;
  size_t process_descriptor_bytes,thread_descriptor_bytes;
  const HANDLE *inherited;
  size_t inherited_count;
  struct slcli_node_transport_windows *transport;
  struct slcli_runtime_containing_windows *containing;
  const struct slcli_node_prefix_original *prefix;
  uint64_t bootstrap_bytes,allocator_capacity,thread_capture_attempts;
};
struct slcli_launcher_windows_capture {
  struct slcli_launcher_windows_inputs original;
  /* Parent-owned before birth: missing/zero child outcomes never clear this
   * possible-effect frontier. Exit is not borrower/resource disposition. */
  uint64_t possible_thread_attempts,possible_event_capacity;
  uint32_t child_effects_possible;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller;
  size_t caller_bytes;
  uint64_t origin_tick,expires,predecessor_tick;
  struct slcli_native_clock before,birth_before,after,resume_before,resume_after;
  struct slcli_native_clock setup_clock[3];
  WCHAR *application,*command_before,*command_returned,*environment,*directory;
  WCHAR bootstrap_selector[SLCLI_NODE_SELECTOR_ENTRY_CHARS];
  size_t environment_constructed_bytes;
  uint32_t selector_encoded,selector_environment_returned;
  struct slcli_node_route_windows route_original;
  unsigned char route_bytes[SLCLI_NODE_ROUTE_CELL_BYTES];
  int route_encode_result;uint32_t route_encode_returned;
  HANDLE *inherited;
  void *process_descriptor,*thread_descriptor;
  SECURITY_ATTRIBUTES process_security,thread_security;
  STARTUPINFOEXW startup;
  PROCESS_INFORMATION process;
  DWORD creator_process,creator_thread;
  FILETIME creation_time,exit_time,kernel_time,user_time;
  BOOL times_status;
  DWORD times_error;
  uint64_t times_args[5];
  uint32_t creator_observed,times_called,times_returned;
  SIZE_T query_bytes,allocated_bytes,initialize_bytes_before,initialize_bytes_returned;
  LPPROC_THREAD_ATTRIBUTE_LIST attributes;
  BOOL query_status,initialize_status,update_status,create_status;
  DWORD query_error,initialize_error,update_error,create_error,resume_error;
  DWORD creation_flags,resume_status;
  uint32_t started,query_returned,initialize_returned,update_returned;
  uint32_t create_called,create_returned,created,qualified;
  int child_mapping_result;uint32_t child_mapping_returned;
  int route_write_result;uint32_t route_write_returned;
  const struct slcli_memory_owner *target_owner_original;
  const struct slcli_preentry_backing_windows *runtime_backing_original;
  struct slcli_runtime_containing_windows *containing_original;
  uint64_t target_charged_original,target_next_sequence_original;
  uint32_t target_count_original;
  unsigned char target_associations_original[4][32];
  const unsigned char *owner_export_original;
  size_t owner_export_bytes;
  uint32_t resume_attempted,resume_called,resume_returned,resume_qualified;
  struct slcli_node_transport_windows *transport_original;
  const struct slcli_node_prefix_original *prefix_original;
  HANDLE transport_handle_original;
  struct slcli_node_bootstrap_transport *transport_payload;
  struct slcli_native_clock transport_setup,transport_ready;
  int transport_publication_result;
  uint32_t transport_publication_returned;
};
/* Caller holds exclusive native lifetime/capture ownership across creation and
 * publication. All storage stays charged, including partial process handles.
 * Child is suspended; separately admitted preentry binding must precede resume. */
int slcli_launcher_windows_create(struct slcli_launcher_windows_capture *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,const struct slcli_launcher_windows_inputs *,const struct slcli_memory_owner *,
  const struct slcli_preentry_backing_windows *);
/* Release requires the retained complete encoded frozen target owner. These
 * equality/storage checks do not supply private transport writer exclusion,
 * actual child/source/native admission or complete preentry binding. Raw
 * late release outputs remain in this parent capture, never the frozen ledger. */
int slcli_launcher_windows_resume(struct slcli_launcher_windows_capture *,struct slcli_arena *,
  const struct slcli_native_clock *,const struct slcli_memory_owner *,
  const unsigned char *,size_t);
#endif
#endif
