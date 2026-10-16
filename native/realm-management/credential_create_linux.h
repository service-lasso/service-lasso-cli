#ifndef SLCLI_MANAGEMENT_CREDENTIAL_CREATE_LINUX_H
#define SLCLI_MANAGEMENT_CREDENTIAL_CREATE_LINUX_H
#include "../host-service/source_deadline.h"
#include "../host-service/store_body.h"
#if defined(__linux__)
/* Private raw64 original7/16 birth only. Independent realm parent, BQ
 * designation, namespace/protection/actor/SDK admission preexist. Native
 * success cannot designate a credential association or authorize signing.
 * The returned O_RDWR handle remains charged through persistence and its
 * genuine alias/readonly transfer. No cleanup or reset is supplied here. */
struct slcli_management_credential_linux_creation {
  struct slcli_arena *arena;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller_original;
  size_t caller_bytes;
  const char *name_original;
  size_t name_bytes;
  char name[65];
  int parent_original,created_original;
  uint32_t original_role;
  slcli_store_capture_sink sink;
  void *capture_original;
  size_t capture_original_bytes;
  uint64_t expires,origin_tick,previous_tick;
  uint64_t uid_args[6],uid_status,open_args[6],open_status;
  struct slcli_linux_file_capture parent_before,created,parent_after;
  struct slcli_native_clock clocks[10];
  uint32_t prepared,started,unresolved,uid_called,uid_observed;
  uint32_t open_called,open_observed,file_created,descriptor_acquired,complete;
};
struct slcli_management_credential_linux_creation *slcli_management_credential_linux_creation_allocate(
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
  int,uint32_t,const char *,size_t,slcli_store_capture_sink,void *,size_t);
/* One O_CREAT|O_EXCL native call with mode0600. Filesystem/namespace plans
 * must qualify regular-file parent link-count preservation and exclude all
 * interfering namespace changes. Before/after clocks do not bound kernel
 * latency or prove cancellation. Partial native effects remain retained. */
int slcli_management_credential_linux_create(struct slcli_management_credential_linux_creation *);
#endif
#endif