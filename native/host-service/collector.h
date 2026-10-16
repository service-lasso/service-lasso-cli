#ifndef SLCLI_COLLECTOR_H
#define SLCLI_COLLECTOR_H
#include "stream_native.h"
#include "evidence_native.h"
#include "primitive.h"
#if defined(_WIN32) || defined(__linux__)
typedef int (*slcli_collector_native_sink)(void *,const struct slcli_stream_capture *,
                                           const unsigned char *,size_t);
typedef int (*slcli_collector_forward)(void *,const unsigned char *,size_t);
struct slcli_collector {
  unsigned char buffer[1024],primitive[2048];
  unsigned char identity[32],source[32],eof_hash[32];
  slcli_input_handle original;
  struct slcli_evidence_file *data_file,*native_file;
  slcli_collector_native_sink native_sink;
  slcli_collector_forward forward;
  void *native_original,*forward_original;
  const struct slcli_stream_capture *eof_original;
  struct slcli_stream_capture *pending_original;
  uint64_t maximum,count,deadline;
  uint32_t reads,failed,eof,primitive_bytes;
};
/* Caller is the original service collector, not an observer or public API.
 * Independently admitted stream/native-capture/retention/forwarding source
 * parameters and genuine arena/parent custody remain required. No constructor
 * or registry admission is supplied. EOF borrows the actual original buffer. */
int slcli_collector_read(struct slcli_collector *,struct slcli_stream_capture *,slcli_input_handle);
int slcli_collector_recheck_eof(struct slcli_collector *);
#if defined(_WIN32)
int slcli_collector_complete(struct slcli_collector *,struct slcli_stream_capture *);
#endif
#endif
#endif
