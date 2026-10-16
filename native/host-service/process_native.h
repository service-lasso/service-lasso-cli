#ifndef SLCLI_PROCESS_NATIVE_H
#define SLCLI_PROCESS_NATIVE_H
#include "stream_native.h"
#include "source_deadline.h"
#if defined(__linux__)
#include <asm/siginfo.h>
#include <linux/poll.h>
#include <linux/time_types.h>
#endif
#if defined(_WIN32) || defined(__linux__)
/* One genuine observation of the SAME externally admitted retained process
 * handle/pidfd. No PID lookup, fabricated birth or namespace actor enrollment.
 * A wait result does not prove EOF/channel/object/archive retirement. */
struct slcli_process_exit_capture {
  slcli_input_handle original;
  uint64_t args[5],status,error;
  uint32_t started,wait_called,observed,ended,natural_zero,complete;
  const struct slcli_source_deadline_original *deadline_original;
  const struct slcli_native_clock *origin_original,*predecessor_original;
  const void *caller_original;
  size_t caller_bytes;
  uint64_t origin_tick,expires,predecessor_tick;
  struct slcli_native_clock before,clock;
#if defined(_WIN32)
  DWORD wait_milliseconds;
  DWORD wait_status,exit_code;
  BOOL exit_status;
  uint32_t exit_called,exit_returned;
  DWORD exit_error;
  uint64_t exit_args[2];
#else
  struct pollfd readiness;
  struct __kernel_timespec timeout_original,timeout_returned;
  uint64_t readiness_args[5],readiness_status;
  uint32_t readiness_called,readiness_returned;
  siginfo_t info;
#endif
};
/* One finite wait under the SAME original caller deadline/predecessor. The
 * actual source owner must hold the original process lifetime/pending pin
 * across this call. Arena storage and a pointer never provide that authority.
 * Exit observation still does not dispose any process/token/image handles. */
int slcli_process_exit_original(struct slcli_process_exit_capture *,struct slcli_arena *,
  const struct slcli_source_deadline_original *,const void *,
  size_t,const struct slcli_native_clock *,slcli_input_handle);
#endif
#endif
