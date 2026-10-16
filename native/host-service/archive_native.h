#ifndef SLCLI_ARCHIVE_NATIVE_H
#define SLCLI_ARCHIVE_NATIVE_H
#include "store_body.h"
#include "shared_workspace.h"
#if defined(_WIN32) || defined(__linux__)
/* Whole original held-file copy under pre-reserved authentic retention. Source,
 * output and containing-directory references are independently admitted owner
 * parameters. A handle, bytes/hash or this storage constructor grants none. */
struct slcli_archive_copy {
  struct slcli_shared_workspace *workspace;
  slcli_input_capture *captures;
  slcli_input_handle source,output,root;
  slcli_store_capture_sink capture_sink;
  void *capture_original;
  size_t capture_count,capture_capacity;
  uint64_t expected,offset;
  unsigned char digest[32];
  uint32_t started,failed,complete;
};
struct slcli_archive_copy *slcli_archive_copy_allocate(struct slcli_arena *,size_t);
int slcli_archive_copy_original(struct slcli_archive_copy *,slcli_input_handle,
                                 slcli_input_handle,slcli_input_handle,uint64_t,
                                 const unsigned char[32],slcli_store_capture_sink,void *);
#endif
#endif
