#ifndef SLCLI_LAUNCHER_LINUX_H
#define SLCLI_LAUNCHER_LINUX_H
#include "source_deadline.h"
#include "owner_handoff_codec.h"
#include "preentry_backing_linux.h"
#include "platform_memory.h"
#include "../node-owner/transport_linux.h"
#if defined(__linux__)
#include <stdatomic.h>
#include <linux/sched.h>
#include <linux/time_types.h>
_Static_assert(sizeof(atomic_uint)==4,"Original Linux futex word width");
enum slcli_launcher_fd_action { SLCLI_LAUNCH_KEEP=1,SLCLI_LAUNCH_EXEC_ONLY=2,SLCLI_LAUNCH_CLOSE=3 };
struct slcli_launcher_linux_fd { int fd;uint32_t action; };
struct slcli_launcher_linux_fd_capture {
  struct slcli_launcher_linux_fd original;
  struct slcli_native_clock before,after_flags,after_action;
  uint64_t flags_result,set_result,close_result;
  uint32_t flags_returned,set_returned,close_returned;
};
struct slcli_launcher_linux_child {
  /* Independent release/acquire publication of the immutable getpid/gettid
   * and birth-clock subset, before the gate wait. No polling policy or native
   * image/source authority is supplied by this shared capture. */
  atomic_uint birth_publication;
  /* 1 publishes immutable birth/wait/directory/FD/pre-exec fields, 2 also
   * publishes an actual exec failure and exit intent, 3 an abnormal exit return.
   * Readers acquire this phase and read only the corresponding frozen subset. */
  atomic_uint publication;
  atomic_uint gate;
  struct slcli_native_clock birth,before_wait,after_wait,before_exec,after_directory,exec_clock;
  uint64_t process_result,thread_result,wait_args[6],wait_result,exec_args[5],exec_result;
  struct __kernel_timespec absolute_timeout;
  uint32_t started,process_returned,thread_returned,wait_called,wait_returned;
  uint64_t directory_result,exit_result;
  uint32_t directory_called,directory_returned;
  struct slcli_launcher_linux_fd_capture *descriptors;
  size_t descriptor_count;
  uint32_t exec_called,exec_returned,exit_called,exit_returned;
};
struct slcli_launcher_linux_birth_view {
  struct slcli_native_clock birth;
  uint64_t process_result,thread_result;
  uint32_t observed,publication,process_returned,thread_returned,qualified;
};
struct slcli_launcher_linux_failure_view {
  uint32_t phase,exec_called,exec_returned,directory_called,directory_returned,exit_called;
  uint32_t exit_returned;
  uint64_t process_result,thread_result,wait_result,directory_result,exec_result,exit_result;
};
struct slcli_launcher_linux_inputs {
  int executable,directory;
  const struct slcli_launcher_linux_fd *descriptors;
  size_t descriptor_count;
  const char *const *argv,*const *environment;
  size_t argc,environment_count;
  struct slcli_node_transport_linux *transport;
  const struct slcli_node_prefix_original *prefix;
  uint64_t bootstrap_bytes,allocator_capacity,thread_capture_attempts;
  size_t birth_observation_capacity;
};
enum slcli_launcher_release_result { SLCLI_RELEASE_FAILED=0,
  SLCLI_RELEASE_COMPLETE=1,SLCLI_RELEASE_PENDING=2 };
struct slcli_launcher_birth_observation {
  struct slcli_native_clock before,after;
  uint32_t publication,failure_publication,observed;
};
struct slcli_launcher_linux_capture {
  struct slcli_launcher_linux_inputs original;
  uint64_t possible_thread_attempts,possible_event_capacity;
  uint32_t child_effects_possible;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller;
  size_t caller_bytes;
  uint64_t origin_tick,expires,predecessor_tick;
  struct slcli_native_clock before,clone_before,after,release_before,release_after;
  struct slcli_memory_capture shared_memory;
  struct slcli_arena shared_arena;
  struct slcli_launcher_linux_child *child;
  struct clone_args clone_original;
  int pidfd;
  uint64_t clone_result,wake_args[6],wake_result;
  char **argv,**environment;
  struct slcli_launcher_linux_fd *descriptors;
  uint32_t started,clone_called,clone_returned,created,qualified;
  const struct slcli_memory_owner *target_owner_original;
  const struct slcli_preentry_backing_linux *runtime_backing_original;
  uint64_t target_charged_original,target_next_sequence_original;
  uint32_t target_count_original;
  unsigned char target_associations_original[4][32];
  const unsigned char *owner_export_original;
  struct slcli_node_transport_linux *transport_original;
  const struct slcli_node_prefix_original *prefix_original;
  struct slcli_node_bootstrap_transport *transport_payload;
  struct slcli_launcher_linux_birth_view *transport_birth;
  struct slcli_native_clock transport_constructor[3];
  struct slcli_native_clock transport_ready;
  struct slcli_launcher_birth_observation *birth_observations;
  size_t birth_observation_used;
  uint32_t birth_observation_failed;
  int transport_descriptor_original;
  uint32_t transport_birth_attempted,transport_birth_returned;
  int transport_birth_result,transport_publish_result;
  uint32_t transport_publish_attempted,transport_publish_returned;
  size_t owner_export_bytes;
  uint32_t release_attempted,wake_called,wake_returned,release_qualified;
};
/* Explicit held image and child argument originals, never path/PID lookup or
 * argv/environment provenance. Child is held by a real inherited futex gate;
 * parent/child outputs occupy distinct retained storage. Source lifetime and
 * inherited image/preentry authority must be joined by the installed caller. */
int slcli_launcher_linux_create(struct slcli_launcher_linux_capture *,struct slcli_arena *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,const struct slcli_launcher_linux_inputs *,const struct slcli_memory_owner *,
  const struct slcli_preentry_backing_linux *);
/* One observation into fresh charged parent storage; an unpublished/invalid
 * result remains retained. Repeated observations need their own finite source
 * geometry. This does not release the child or establish installed entry. */
int slcli_launcher_linux_birth_original(const struct slcli_launcher_linux_capture *,
  struct slcli_arena *,struct slcli_launcher_linux_birth_view *);
/* Acquire a frozen failure subset only. Publication 1 precedes exec and cannot
 * expose exec/exit returns. Successful exec needs separate genuine installed
 * preentry/settled-lifetime observation, not this failure accessor. */
int slcli_launcher_linux_failure_original(const struct slcli_launcher_linux_capture *,
  struct slcli_arena *,struct slcli_launcher_linux_failure_view *);
/* Release requires the retained complete encoded frozen target owner. These
 * equality/storage checks do not supply private transport writer exclusion,
 * actual child/source/native admission or complete preentry binding. Raw
 * late release outputs remain in this parent capture, never the frozen ledger.
 * PENDING retains a distinct preowned publication observation but consumes no
 * release/birth-output attempt. Caller must compare the exact enum result,
 * service existing capture/deadline while pending, and never retry FAILED. */
int slcli_launcher_linux_release(struct slcli_launcher_linux_capture *,struct slcli_arena *,
  const struct slcli_native_clock *,const struct slcli_memory_owner *,
  const unsigned char *,size_t);
#endif
#endif
