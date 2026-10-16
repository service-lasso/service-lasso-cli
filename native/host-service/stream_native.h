#ifndef SLCLI_STREAM_NATIVE_H
#define SLCLI_STREAM_NATIVE_H
#include "native_clock.h"
#include "input_body.h"
#include "source_deadline.h"
#if defined(__linux__)
#include <linux/poll.h>
#include <linux/time_types.h>
#endif
#if defined(_WIN32) || defined(__linux__)
struct slcli_stream_capture;
struct slcli_channel_ready_inputs;
typedef int (*slcli_stream_transfer_sink)(void *,const struct slcli_stream_capture *,const unsigned char *,size_t);
#if defined(__linux__)
struct slcli_channel_readiness {
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller;
  size_t caller_bytes;
  uint64_t origin_tick,expires,predecessor_tick,args[5],status;
  struct pollfd descriptor;
  struct __kernel_timespec timeout_original,timeout_returned;
  struct slcli_native_clock before,after;
  uint32_t started,called,returned,qualified;
};
struct slcli_channel_ready_inputs {
  struct slcli_arena *arena;
  const struct slcli_source_deadline_original *deadline;
  const void *caller;
  size_t caller_bytes;
  const struct slcli_native_clock *predecessor;
};
int slcli_channel_readiness_original(struct slcli_channel_readiness *,
  const struct slcli_channel_ready_inputs *,int,short);
#endif
#if defined(_WIN32)
/* One physical manual-reset event, retained for the whole request. Native
 * construction/close facts are distinct from table admission and retirement. */
struct slcli_channel_event_original {
  HANDLE handle;
  struct slcli_native_clock birth_before,birth_after,close_before,close_after;
  DWORD birth_error,close_error;
  BOOL close_status;
  uint32_t birth_started,birth_called,birth_returned,birth_qualified;
  uint32_t close_started,close_called,close_returned,close_qualified;
  uintptr_t birth_args[4],close_args[1];
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller;
  const void *reserved_reference;
  uint64_t reserved_version;
  uint64_t origin_tick,expires;
};
struct slcli_channel_event_wait {
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin,*predecessor;
  const void *caller;
  size_t caller_bytes;
  HANDLE event;
  uint64_t expires,origin_tick,predecessor_tick,args[2];
  struct slcli_native_clock before,after;
  DWORD milliseconds,status,error;
  uint32_t started,called,returned,qualified;
};
int slcli_channel_event_wait_original(struct slcli_channel_event_wait *,struct slcli_arena *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,HANDLE);
#endif
/* Source-owned service collector only. An inherited descriptor or event is not
 * admitted primary/stream authority. Buffer and SAME row remain held until
 * actual completion and independent capture transfer/readback. No reset/free.
 * First-call row must be wholly byte-zeroed constructor storage, including
 * padding; a C aggregate initializer alone does not assert that raw contract. */
struct slcli_stream_capture {
  slcli_input_handle original;
  unsigned char *buffer;
  size_t requested,returned;
  uint64_t args[5],status,error;
  uint32_t started,observed,count_observed,pending,complete,eof,failed;
  struct slcli_native_clock clock;
  /* Initial submission and one completion have independent retained transfer
   * outcomes. Failure finalization must not discard the archive result or
   * repeat an uncertain transfer of the same actual native observation. */
  uint32_t transfer_attempted[2],transfer_returned[2];
  int transfer_result[2];
  void *transfer_owner[2];
  slcli_stream_transfer_sink transfer_sink[2];
  const unsigned char *transfer_bytes[2];
  size_t transfer_lengths[2];
#if defined(_WIN32)
  struct slcli_channel_event_wait event_wait;
  HANDLE reset_original;
  const struct slcli_stream_capture *event_predecessor;
  BOOL reset_status;
  DWORD reset_error;
  uint32_t reset_returned;
  OVERLAPPED overlapped;
  DWORD initial_count,completion_count,initial_error,completion_error;
  BOOL initial_status,completion_status;
  uint32_t completion_observed,cancel_started,cancel_observed;
  BOOL cancel_status;
  DWORD cancel_error;
  uint64_t completion_args[4],cancel_args[2];
  struct slcli_native_clock completion_clock;
#else
  struct slcli_channel_readiness readiness;
  uint64_t flags_args[3],flags_status;
  uint32_t flags_observed;
#endif
};
int slcli_stream_read_original(struct slcli_stream_capture *,slcli_input_handle,
                                unsigned char *,size_t,slcli_input_handle);
/* Retains the actual archive call outcome only. Source/native authority and
 * independent durable readback remain obligations of the genuine owner. */
int slcli_stream_transfer_original(struct slcli_stream_capture *,slcli_stream_transfer_sink,
                                    void *,const unsigned char *,size_t);
#if defined(_WIN32)
int slcli_stream_read_complete(struct slcli_stream_capture *);
int slcli_stream_read_owned_original(struct slcli_stream_capture *,slcli_input_handle,
  unsigned char *,size_t,HANDLE,const struct slcli_stream_capture *);
int slcli_stream_read_cancel(struct slcli_stream_capture *);
#endif
#if defined(__linux__)
int slcli_stream_read_ready_original(struct slcli_stream_capture *,slcli_input_handle,
  unsigned char *,size_t,const struct slcli_channel_ready_inputs *);
#endif
#endif
#endif
