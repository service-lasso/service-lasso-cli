#ifndef SLCLI_MANAGEMENT_FOUNDATION_CREATE_LINUX_H
#define SLCLI_MANAGEMENT_FOUNDATION_CREATE_LINUX_H
#include "../host-service/source_deadline.h"
#include "../host-service/store_body.h"
#if defined(__linux__)
/* PRIVATE native creator for original role97/98/21/15/6. Authentic BQ
 * designation/parent/ancestor/actor admission preexist. Role/name/uid/native
 * success are custody facts only; they cannot issue a protected-root grant.
 * The authentic parent owning lease must exclude EVERY namespace mutation
 * between mkdir and held child observation. mkdir supplies no inode output;
 * these calls alone cannot prove that a later open named the created inode. */
struct slcli_management_foundation_linux_creation {
  struct slcli_arena *arena;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller_original;
  size_t caller_bytes;
  const char *name_original;
  size_t name_bytes;
  char name[65];
  int parent_original,created_original;
  uint32_t original_role,parent_link_delta;
  slcli_store_capture_sink sink;
  void *capture_original;
  size_t capture_original_bytes;
  uint64_t expires,origin_tick,previous_tick;
  uint64_t uid_args[6],uid_status,mkdir_args[6],mkdir_status,open_args[6],open_status;
  struct slcli_linux_file_capture parent_before,created,parent_after;
  struct slcli_native_clock clocks[12];
  uint32_t prepared,started,unresolved,uid_called,uid_observed;
  uint32_t mkdir_called,mkdir_observed,directory_created;
  uint32_t open_called,open_observed,descriptor_acquired,complete;
};
struct slcli_management_foundation_linux_creation *slcli_management_foundation_linux_creation_allocate(
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
  int,uint32_t,uint32_t,const char *,size_t,slcli_store_capture_sink,void *,size_t);
/* parent_link_delta is exactly0 or1 from the independently qualified original
 * filesystem/namespace-owning plan. It is never inferred after observing a
 * failure or selected from caller JSON. Unsupported semantics deny creation. */
/* No EEXIST recovery or retry; separate consumption is a separately admitted
 * operation. Created directory/open descriptor remain held on failed checks. */
int slcli_management_foundation_linux_create(struct slcli_management_foundation_linux_creation *);
#endif
#endif
