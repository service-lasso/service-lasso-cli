#ifndef SLCLI_NODE_PREENTRY_H
#define SLCLI_NODE_PREENTRY_H
#include "allocator_caller.h"
#if defined(__linux__)
#include "../host-service/preentry_backing_linux.h"
/* A genuine native pre-CRT constructor calls this after independent parent,
 * caller/source/image/private transport admission. Raw receiver identity and
 * arena containment are NOT that admission. No main-time fallback calls it. */
struct slcli_node_preentry_linux {
  struct slcli_preentry_backing_linux *receiver;
  struct slcli_native_clock zero_before,zero_after,origin_before;
  const struct slcli_source_deadline_original *deadline_original;
  const struct slcli_native_clock *origin_original;
  uint64_t expires_original;
  struct slcli_arena child_arena;
  struct slcli_node_allocator_caller *allocator;
  struct slcli_node_allocator_wait_row *rows;
  struct slcli_source_deadline_original *child_deadline;
  struct slcli_native_clock *retained_origin;
  struct slcli_node_preentry_caller *child_caller;
  size_t capacity,zero_scanned,zero_nonzero_offset;
  unsigned char zero_nonzero_value;
  uint32_t zero_scan_returned;
  int zero_result,zero_clock_result;
  int receive_result,arena_result,bind_result,prepare_result,attach_result;
  uint32_t started,receive_returned,arena_returned,bind_returned,prepare_returned,attach_returned;
};
struct slcli_node_preentry_caller {
  const struct slcli_source_deadline_original *transported_deadline_original;
  const void *transported_caller_original;
  const struct slcli_preentry_backing_linux *receiver_original;
  uint64_t process_result,thread_result;
  uint32_t process_returned,thread_returned;
  struct slcli_native_clock native_before[3],clock;
};
int slcli_node_preentry_linux_bind_storage(struct slcli_node_preentry_linux *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,int,uint64_t,const struct stat *,size_t,const struct slcli_extent_request *);
#endif
#if defined(_WIN32)
#include "../host-service/preentry_backing_windows.h"
struct slcli_node_preentry_windows_caller {
  const struct slcli_source_deadline_original *transported_deadline_original;
  const void *transported_caller_original;
  const struct slcli_preentry_backing_windows *receiver_original;
  DWORD process_result,thread_result;
  FILETIME creation_time,exit_time,kernel_time,user_time,expected_creation_original;
  BOOL times_status;
  DWORD times_error;
  uint64_t times_args[5];
  uint32_t process_returned,thread_returned,times_called,times_returned;
  struct slcli_native_clock native_before[3],clock;
};
struct slcli_node_preentry_windows {
  uint32_t storage_stage;
  size_t storage_scanned,storage_nonzero_offset;
  unsigned char storage_nonzero_value;
  struct slcli_native_clock constructor_before,schedule_before,schedule_after,constructor_after;
  int constructor_before_result,schedule_before_result,schedule_after_result,constructor_after_result;
  struct slcli_node_child_capture_schedule *thread_capture;
  size_t thread_capture_attempts;
  uint32_t thread_capture_returned;
  struct slcli_preentry_backing_windows *receiver;
  struct slcli_native_clock zero_before,zero_after,origin_before;
  const struct slcli_source_deadline_original *deadline_original;
  const struct slcli_native_clock *origin_original;
  uint64_t expires_original;
  struct slcli_arena child_arena;
  struct slcli_node_allocator_caller *allocator;
  struct slcli_node_allocator_wait_row *rows;
  struct slcli_source_deadline_original *child_deadline;
  struct slcli_native_clock *retained_origin;
  struct slcli_node_preentry_windows_caller *child_caller;
  size_t capacity,zero_scanned,zero_nonzero_offset;
  unsigned char zero_nonzero_value;
  uint32_t zero_scan_returned;
  int zero_result,zero_clock_result;
  int receive_result,arena_result,bind_result,prepare_result,attach_result;
  uint32_t started,receive_returned,arena_returned,bind_returned,prepare_returned,attach_returned;
};
int slcli_node_preentry_windows_bind_storage(struct slcli_node_preentry_windows *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,HANDLE,HANDLE,uint64_t,const FILETIME *,size_t,size_t,const struct slcli_extent_request *);
#endif
#endif
