#ifndef SLCLI_INPUT_BODY_H
#define SLCLI_INPUT_BODY_H
#include "crypto.h"
#include "arena.h"
#if defined(_WIN32)
#include "file_windows.h"
typedef HANDLE slcli_input_handle;
typedef struct slcli_windows_file_capture slcli_input_capture;
#elif defined(__linux__)
#include "file_linux.h"
typedef int slcli_input_handle;
typedef struct slcli_linux_file_capture slcli_input_capture;
#endif
#if defined(_WIN32) || defined(__linux__)
/* All storage is precharged original owner storage, including every capture.
 * Control and capture rows originate in fresh zeroed, correctly aligned storage;
 * their real owning constructor must forbid aliasing prior original records.
 * A failed attempt is retained permanently until authentic disposition. This
 * reader authenticates neither the caller/provider nor the expected digest. */
struct slcli_input_body {
  unsigned char *body;
  size_t capacity, bytes, capture_capacity, capture_count;
  slcli_input_capture *captures;
  slcli_input_handle original;
  unsigned char digest[32];
  uint32_t started, complete, eof_observed, failed;
};
int slcli_input_body_read(struct slcli_input_body *,slcli_input_handle,
                          uint64_t,const unsigned char[32]);
/* Real allocation from one already charged original native extent. The caller
 * retains this complete block, its arena/owner and handle through archive
 * readback and actual disposition, including every failed/partial attempt.
 * No release/reset API is supplied here. This constructs storage only, never
 * a provider/source capability or a new quota. Capture capacity bounds genuine
 * short reads; exhaustion retains the block and original rows. */
struct slcli_input_body *slcli_input_body_allocate(struct slcli_arena *,
                                                   uint64_t,size_t);
#endif
#endif
