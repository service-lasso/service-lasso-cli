#ifndef SLCLI_SERVICE_CHANNEL_H
#define SLCLI_SERVICE_CHANNEL_H
#include "primary_engine.h"
#include "stream_native.h"
#include "evidence_native.h"
#include "channel_write_native.h"
#if defined(_WIN32) || defined(__linux__)
typedef int (*slcli_channel_capture_sink)(void *,const struct slcli_stream_capture *,
                                          const unsigned char *,size_t);
typedef int (*slcli_channel_write_sink)(void *,const struct slcli_channel_write_capture *,
                                        const unsigned char *,size_t);
struct slcli_service_exchange {
  struct slcli_stream_capture *read_rows;
  struct slcli_channel_write_capture *write_rows;
  uint32_t read_capacity,write_capacity,reads,writes,started,completed,failed;
  /* Actual logical resident geometry within the already fully charged native
   * arena extent; never a second physical budget or whole-runtime fit claim. */
  uint64_t request_storage_bytes,read_storage_bytes,write_storage_bytes,direct_storage_bytes;
  int read_result[128],read_completion_result[128],write_result[128],write_completion_result[128];
  uint8_t read_returned[128],read_completion_returned[128],write_returned[128],write_completion_returned[128];
  int dispatch_result,close_result,publication_result;
  int entry_continue_result,entry_publication_result,exit_continue_result;
  uint32_t entry_continue_returned,entry_publication_returned,exit_continue_returned,publication_returned;
  int preflight_result,preflight_publication_result;
  uint32_t preflight_returned,preflight_publication_returned;
  int close_readback_result[2];
  uint8_t close_readback_attempted[2],close_readback_returned[2];
  uint32_t dispatch_attempted,dispatch_returned,close_attempted,close_returned;
};
/* One request, one original input retention file, one response retention file.
 * Storage is part of the service owner; it cannot be shared with another live
 * request or be recycled after failure. Original native peer/source/capability
 * admission and the output channel writer remain external unqualified owners. */
struct slcli_service_request {
  /* Retained original SAME primary/source owner, independently constructed.
   * No incoming frame or dispatch call manufactures this association. */
  struct slcli_primary_engine *owning_primary;
  const struct slcli_source_deadline_original *deadline_original;
  struct slcli_frame input,response;
  unsigned char read_buffer[16384],header[4],mac[32],response_header[4],response_mac[32];
  slcli_input_handle original;
  struct slcli_evidence_file *input_file,*response_file;
  slcli_channel_capture_sink capture_sink;
  void *capture_original;
  struct slcli_stream_capture *pending_original;
  struct slcli_channel_write_capture *pending_write_original;
  slcli_channel_write_sink write_sink;
  void *write_original;
  slcli_input_handle output_original;
  uint32_t header_bytes,body_bytes,mac_bytes,started,complete,dispatched,failed;
  uint32_t response_retained,write_phase,write_offset,delivered;
  struct slcli_service_exchange exchange;
#if defined(_WIN32)
  struct slcli_channel_event_original read_event,write_event;
  const struct slcli_stream_capture *last_owned_read;
  const struct slcli_channel_write_capture *last_owned_write;
#endif
};
/* Charged zeroed request storage only; actual source/actor/file admission is
 * still required before any ordinary native effect. */
struct slcli_service_request *slcli_service_request_allocate(struct slcli_arena *);
/* One real ordinary exchange over already reserved original capture rows.
 * Authentic source/binding/files/keys remain inputs; no constructor grant is
 * inferred. Failure retains request, rows, native pending effects and root. */
int slcli_service_exchange_original(struct slcli_service_request *,
  struct slcli_stream_capture *,uint32_t,struct slcli_channel_write_capture *,uint32_t,
  struct slcli_native_clock *,const struct slcli_primary_commit_originals *);
int slcli_service_request_read(struct slcli_service_request *,struct slcli_stream_capture *,
                               slcli_input_handle);
#if defined(_WIN32)
int slcli_service_request_complete(struct slcli_service_request *,struct slcli_stream_capture *);
/* NULL event selects this request's real once-created native event. Passing a
 * borrowed event does not enroll it in this owner. */
int slcli_service_request_close_events(struct slcli_service_request *);
#endif
/* Dispatch follows the SAME retained native input's full independent file
 * readback. Success retains a durable response body, not delivery/primary ACK. */
int slcli_service_request_dispatch(struct slcli_service_request *,struct slcli_primary_engine *,
                                   struct slcli_native_clock *,
                                   const struct slcli_primary_commit_originals *);
/* The SAME original output channel and native write capture owner are supplied
 * by the independent authenticated launcher. A completed actual write proves
 * delivery only, never primary INTENT_ACK receipt or physical resource closure. */
int slcli_service_response_write(struct slcli_service_request *,struct slcli_channel_write_capture *,
                                  slcli_input_handle);
#if defined(_WIN32)
int slcli_service_response_complete(struct slcli_service_request *,struct slcli_channel_write_capture *);
#endif
#endif
#endif
