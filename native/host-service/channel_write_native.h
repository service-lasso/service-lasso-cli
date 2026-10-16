#ifndef SLCLI_CHANNEL_WRITE_NATIVE_H
#define SLCLI_CHANNEL_WRITE_NATIVE_H
#include "stream_native.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_channel_write_capture;
typedef int (*slcli_channel_write_transfer_sink)(void *,const struct slcli_channel_write_capture *,const unsigned char *,size_t);
/* First-call capture is precharged wholly byte-zeroed constructor storage,
 * including padding. Pending/completed originals are never reset for reuse. */
struct slcli_channel_write_capture {
  slcli_input_handle original;
  const unsigned char *body;
  size_t requested,returned;
  uint64_t args[5],status,error;
  uint32_t started,observed,count_observed,pending,complete,failed;
  struct slcli_native_clock clock;
  uint32_t transfer_attempted[2],transfer_returned[2];
  int transfer_result[2];
  void *transfer_owner[2];
  slcli_channel_write_transfer_sink transfer_sink[2];
  const unsigned char *transfer_bytes[2];
  size_t transfer_lengths[2];
#if defined(_WIN32)
  struct slcli_channel_event_wait event_wait;
  HANDLE reset_original;
  const struct slcli_channel_write_capture *event_predecessor;
  BOOL reset_status;
  DWORD reset_error;
  uint32_t reset_returned;
  OVERLAPPED overlapped;
  DWORD initial_count,initial_error,completion_count,completion_error;
  BOOL initial_status,completion_status;
  uint32_t completion_observed;
  uint64_t completion_args[4];
  struct slcli_native_clock completion_clock;
#else
  struct slcli_channel_readiness readiness;
  uint64_t flags_args[3],flags_status;
  uint32_t flags_observed;
#endif
};
int slcli_channel_write_original(struct slcli_channel_write_capture *,slcli_input_handle,
                                  const unsigned char *,size_t,slcli_input_handle);
#if defined(_WIN32)
int slcli_channel_write_complete(struct slcli_channel_write_capture *);
int slcli_channel_write_owned_original(struct slcli_channel_write_capture *,slcli_input_handle,
  const unsigned char *,size_t,HANDLE,const struct slcli_channel_write_capture *);
#endif
#if defined(__linux__)
int slcli_channel_write_ready_original(struct slcli_channel_write_capture *,slcli_input_handle,
  const unsigned char *,size_t,const struct slcli_channel_ready_inputs *);
#endif
#endif
#endif
