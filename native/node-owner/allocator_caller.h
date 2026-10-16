#ifndef SLCLI_NODE_ALLOCATOR_CALLER_H
#define SLCLI_NODE_ALLOCATOR_CALLER_H
#include <stdatomic.h>
#include "../host-service/source_deadline.h"
/* Private original constructor supplies this storage and its native thread
 * lineage before thread use. It is not a public budget/admission interface.
 * Rows remain charged and one-use through the caller's original lifetime.
 * Current native thread ID equality alone is not original thread birth/source
 * authority; the genuine constructor and retained lifetime join remain required. */
struct slcli_node_allocator_thread_row {
  uint64_t args[1],result;
  uint32_t started,observed;
};
struct slcli_node_allocator_wait_row {
  struct slcli_node_allocator_thread_row thread;
  struct slcli_native_clock clock;
  uint32_t lock_called,lock_was_set,marker_released;
  uint32_t allocator_operation,old_retired,result_ready;
  uintptr_t argument,new_raw,old_raw,completed_result,caller_result;
  size_t bytes,alignment,count;
  void (*handler_original)(void);
  int (*crt_handler_original)(size_t);
  size_t handler_bytes;
  int handler_result;
  uint32_t handler_kind;
  uint32_t handler_called,handler_returned,handler_exception;
  uintptr_t native_address;
  size_t native_bytes;
  uint64_t native_result;
  uint64_t native_raw_result;
  uint64_t native_arguments[2];
  uint32_t native_operation,native_called,native_returned;
  uint32_t native_raw_returned;
};
struct slcli_node_allocator_caller {
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller_original;
  struct slcli_node_allocator_wait_row *rows;
  atomic_flag *owning_lock;
  uint64_t expires,origin_tick,previous_tick,native_thread;
  struct slcli_node_allocator_thread_row thread;
  size_t capacity,used,caller_bytes,acquisition_row;
  uint32_t prepared,unresolved,acquired;
  struct slcli_node_allocator_caller *creator_original;
  uint64_t creator_native_thread;
  uint32_t unborn_thread;
  const atomic_int *child_phase;
};
/* Exact lexical BSS marker of the full image owner. Native/source/image/BSS
 * provenance still requires the genuine constructor's admitted originals. */
atomic_flag *slcli_node_allocator_marker_original(void);
int slcli_node_allocator_caller_prepare(struct slcli_arena *,
  struct slcli_node_allocator_caller *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  struct slcli_node_allocator_wait_row *,size_t,atomic_flag *);
int slcli_node_allocator_caller_prepare_unborn(struct slcli_arena *,
  struct slcli_node_allocator_caller *,struct slcli_node_allocator_caller *,
  struct slcli_node_allocator_wait_row *,size_t,atomic_flag *);
int slcli_node_allocator_caller_take(struct slcli_node_allocator_caller *,atomic_flag *);
int slcli_node_allocator_caller_release(struct slcli_node_allocator_caller *,atomic_flag *);
int slcli_node_allocator_caller_handler_before(struct slcli_node_allocator_caller *,
  void (*)(void),size_t *);
int slcli_node_allocator_caller_handler_after(struct slcli_node_allocator_caller *,
  void (*)(void),size_t,int);
int slcli_node_allocator_caller_crt_handler_before(struct slcli_node_allocator_caller *,
  int (*)(size_t),size_t,size_t *);
int slcli_node_allocator_caller_crt_handler_after(struct slcli_node_allocator_caller *,
  int (*)(size_t),size_t,size_t,int,int);
/* Private actual source-native operation boundary. The address is retained
 * custody, never actor/lock/source authority. Lexical native constructor and
 * storage birth/lifetime admission remain independently required. */
int slcli_node_allocator_caller_native_before(struct slcli_node_allocator_caller *,
  uint32_t,const void *,size_t,uint64_t,uint64_t,size_t *);
int slcli_node_allocator_caller_native_after(struct slcli_node_allocator_caller *,
  uint32_t,const void *,size_t,size_t,uint64_t);
/* Plain metadata only: retain an actual raw return before GetLastError or a
 * fallible postclock. SAME existing paid row; no native call or extra roster. */
int slcli_node_allocator_caller_native_raw_return(struct slcli_node_allocator_caller *,
  uint32_t,const void *,size_t,size_t,uint64_t);
/* Actual Windows join wait argument from its fresh SAME pre-clock. No renewed
 * deadline, INFINITE sentinel or new wait/capture quota is supplied. */
int slcli_node_allocator_caller_wait_before(struct slcli_node_allocator_caller *,
  const void *,size_t,uint64_t,uint32_t *,size_t *);
#endif
