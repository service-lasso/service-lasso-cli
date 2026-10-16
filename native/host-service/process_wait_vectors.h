#ifndef SLCLI_PROCESS_WAIT_VECTORS_H
#define SLCLI_PROCESS_WAIT_VECTORS_H
#include "process_native.h"
#if defined(_WIN32) || defined(__linux__)
/* Genuine original process, lifetime/pending owner, caller/deadline/capture
 * storage and independently controlled natural exit are fixture prerequisites.
 * These bodies call real methods; no return/status/actor is mocked or enrolled.
 * UNRUN until different whole source review and exact native input admission. */
int slcli_process_wait_positive_vector(struct slcli_process_exit_capture *,struct slcli_arena *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,slcli_input_handle);
/* repeat_copy is a distinct precharged retained native fixture block. The
 * timeout's genuine cause and original process remaining alive are established
 * independently; a false method return alone cannot establish timeout. */
int slcli_process_wait_timeout_vector(struct slcli_process_exit_capture *,
  struct slcli_process_exit_capture *,struct slcli_arena *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,slcli_input_handle);
#endif
#endif
