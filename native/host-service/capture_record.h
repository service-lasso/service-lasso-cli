#ifndef SLCLI_CAPTURE_RECORD_H
#define SLCLI_CAPTURE_RECORD_H
#include "image_reader.h"
#include "peer_windows.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_capture_record {
  struct slcli_evidence_file *producer_file;
  struct slcli_native_clock producer_clock;
  unsigned char header[32];
  const unsigned char *producer_raw,*query_raw;
  size_t producer_bytes,query_bytes;
  const struct slcli_capture_record_reader *query_copy_parent;
  uint32_t used,failed,complete,query_detached;
};
/* Private raw native producer record, source/tool ABI bound by the immutable
 * original SOURCE inputs. This record has no principal/grant/admission field.
 * Actual source actor/ROOT/native ownership remains separately required. */
int slcli_capture_record_write(struct slcli_capture_record *,uint32_t,
  const void *,size_t,const void *,size_t);
struct slcli_capture_record_reader {
  /* Private finite native-row view. Its original handle, rows, workspace and
   * sink must exactly match this reader; it never runs an image hash reader or
   * supplies image/actor authority. All clock storage is precharged before birth. */
  struct slcli_image_reader deadline_rows;
  struct slcli_shared_workspace *workspace;
  slcli_input_handle original;
  const struct slcli_capture_record *producer;
  slcli_input_capture *captures;
  size_t capture_count,capture_capacity;
  slcli_store_capture_sink capture_sink;
  void *capture_original;
  const slcli_input_capture *before,*after;
  uint64_t offset,bytes;
  uint32_t used,failed,complete;
};
/* Actual independent readonly native reads compare exact original producer
 * and query bodies. Success is raw native association only, never admission,
 * physical sealing, source authenticity or allC closure/retirement authority. */
int slcli_capture_record_read_original(struct slcli_capture_record_reader *);
int slcli_capture_record_deadline_prepare_unborn(struct slcli_arena *,
  struct slcli_capture_record_reader *,const struct slcli_source_deadline_original *,
  const void *,size_t,void *,size_t,uint64_t,slcli_input_handle *,const void *,size_t);
int slcli_capture_record_deadline_born(struct slcli_capture_record_reader *,
  slcli_input_handle *,const struct slcli_native_clock *);
#if defined(_WIN32)
int slcli_peer_windows_record_sink(void *,const struct slcli_peer_windows_capture *,const WCHAR *,size_t);
#endif
#endif
#endif
