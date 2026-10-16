#ifndef SLCLI_MANAGEMENT_CREDENTIAL_READONLY_LINUX_H
#define SLCLI_MANAGEMENT_CREDENTIAL_READONLY_LINUX_H
#include "credential_factory_linux.h"
#include "credential_original.h"
#include "../host-service/input_deadline.h"
#if defined(__linux__)
/* Private SAME-inode preparation/consumption; authentic independent BQ,
 * realm, namespace, caller, SDK and all-map/writer-alias admission preexists.
 * Readonly descriptor custody supplies neither designation nor association.
 * All actual failed effects, aliases and capture storage remain charged. */
struct slcli_management_credential_linux_readonly {
  struct slcli_arena *arena;
  struct slcli_management_credential_linux_factory *factory;
  struct slcli_input_body *body;
  struct slcli_input_deadline_owner *input_owner;
  struct slcli_management_credential *credential;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller_original;
  size_t caller_bytes;
  uint64_t expires,origin_tick,previous_tick;
  struct slcli_native_clock clocks[10];
  struct slcli_linux_file_capture parent_capture,readonly_before,close_capture,readonly_after;
  int readonly_original,writer_original;
  uint64_t open_args[6],open_status;
  uint32_t prepared,started,unresolved,open_called,open_observed,alias_acquired,writer_closed;
  uint32_t body_read,credential_consumed,complete;
};
/* Fresh unstarted factory: full134 native input captures,134 preclock rows,
 * two65-byte copies and private credential view precede ALL native births. */
struct slcli_management_credential_linux_readonly *slcli_management_credential_linux_readonly_allocate(
  struct slcli_arena *,struct slcli_management_credential_linux_factory *);
int slcli_management_credential_linux_readonly_run(struct slcli_management_credential_linux_readonly *);
int slcli_management_credential_linux_readonly_native_positive(struct slcli_management_credential_linux_readonly *);
#endif
#endif