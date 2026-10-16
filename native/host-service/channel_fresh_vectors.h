#ifndef SLCLI_CHANNEL_FRESH_VECTORS_H
#define SLCLI_CHANNEL_FRESH_VECTORS_H
#include "channel_write_native.h"
#if defined(_WIN32) || defined(__linux__)
/* Genuine native constructor supplies endpoints, event, original deadline,
 * source authority, wholly zeroed rows and precharged disjoint scratch.
 * SOURCE-UNRUN: these callers create no authority or native fixture. */
int slcli_channel_fresh_positive_original(struct slcli_stream_capture *,
  struct slcli_channel_write_capture *,slcli_input_handle,slcli_input_handle,
  slcli_input_handle,slcli_input_handle,unsigned char *,const unsigned char *,size_t,
  unsigned char *,size_t);
int slcli_channel_markerless_vectors_original(struct slcli_stream_capture *,
  struct slcli_channel_write_capture *,slcli_input_handle,slcli_input_handle,
  slcli_input_handle,slcli_input_handle,unsigned char *,const unsigned char *,size_t,
  unsigned char *,size_t);
#endif
#endif
