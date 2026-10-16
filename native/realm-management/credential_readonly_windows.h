#ifndef SLCLI_MANAGEMENT_CREDENTIAL_READONLY_WINDOWS_H
#define SLCLI_MANAGEMENT_CREDENTIAL_READONLY_WINDOWS_H
#include "credential_factory_windows.h"
#include "credential_original.h"
#include "../host-service/input_deadline.h"
#if defined(_WIN32)
/* Private SAME-file preparation and consumption, not an association producer.
 * Authentic BQ/native caller/realm/permission/namespace/all-map-writer-alias
 * admission preexists. A readonly handle does not prove whole alias closure.
 * Both native aliases and every failed output remain charged and retained. */
struct slcli_management_credential_windows_readonly {
  struct slcli_arena *arena;
  struct slcli_management_credential_windows_factory *factory;
  struct slcli_input_body *body;
  struct slcli_input_deadline_owner *input_owner;
  struct slcli_management_credential *credential;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller_original;
  size_t caller_bytes;
  uint64_t expires,origin_tick,previous_tick;
  struct slcli_native_clock clocks[8];
  struct slcli_windows_file_capture readonly_capture;
  HANDLE process,readonly_original,writer_original;
  uint64_t duplicate_args[7],close_args[1];
  BOOL duplicate_status,close_status;
  DWORD duplicate_error,close_error;
  uint32_t prepared,started,unresolved,process_called,process_observed;
  uint32_t duplicate_called,duplicate_observed,duplicate_error_observed;
  uint32_t alias_acquired,close_called,close_observed,close_error_observed,writer_closed;
  uint32_t body_read,credential_consumed,complete;
};
/* Fresh unstarted factory only. Full134-row input body/copy/captures, matching
 *134 pre-clock rows and private credential view are allocated BEFORE factory
 * file birth/RNG/write/duplication/close. Failure retains all allocations. */
struct slcli_management_credential_windows_readonly *slcli_management_credential_windows_readonly_allocate(
  struct slcli_arena *,struct slcli_management_credential_windows_factory *);
int slcli_management_credential_windows_readonly_run(struct slcli_management_credential_windows_readonly *);
int slcli_management_credential_windows_readonly_native_positive(struct slcli_management_credential_windows_readonly *);
#endif
#endif