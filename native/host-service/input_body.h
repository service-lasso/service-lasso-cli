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
struct slcli_input_deadline_owner;
struct slcli_input_body {
  struct slcli_input_deadline_owner *deadline_owner;
  unsigned char *body;
  size_t capacity, bytes, capture_capacity, capture_count;
  slcli_input_capture *captures;
  slcli_input_handle original;
  unsigned char digest[32];
  uint32_t started, complete, eof_observed, failed;
  uint32_t recheck_started, recheck_complete, consumed;
  unsigned char *readback;
  size_t readback_capacity;
  /* Fixed native source-table sink's actual per-call raw payload allowance.
   * Set from preowned source/capture/record geometry before table IO. Zero
   * does not grant a Windows source-table fallback or larger native read. */
  size_t source_native_read_maximum;
};
int slcli_input_body_read(struct slcli_input_body *,slcli_input_handle,
                          uint64_t,const unsigned char[32]);
/* One-use full same-original consumption check. All new captures are appended
 * inside the originally reserved table. Every short read consumes a fresh row;
 * exhaustion preserves the failed original and never reuses a prior record. */
int slcli_input_body_recheck(struct slcli_input_body *);
/* Exact same held readonly original metadata and whole-file EOF comparison;
 * borrowed unit ranges use full file length, never pretend member-end is EOF. */
int slcli_input_original_same(const slcli_input_capture *,const slcli_input_capture *,uint64_t);
/* Real allocation of original body AND separate full readback copy from one
 * already charged original native extent. The caller
 * retains this complete block, its arena/owner and handle through archive
 * readback and actual disposition, including every failed/partial attempt.
 * No release/reset API is supplied here. This constructs storage only, never
 * a provider/source capability or a new quota. Capture capacity bounds genuine
 * short reads in BOTH initial and consumption reads; exhaustion retains the
 * block and original rows. Exact minimum rows: 2*(ceil(expected/16384)+3).
 * No prior read output is reused as the second read's native output target. */
struct slcli_input_body *slcli_input_body_allocate(struct slcli_arena *,
                                                   uint64_t,size_t);
/* Dedicated selected unit26 member storage in its original charged BQ owner.
 * The fixed member geometry selects at most1MiB body plus separate full copy;
 * this never widens ordinary262144 reads/rechecks or creates a service pool.
 * The unit-range consumer marks this storage consumed before native effects. */
struct slcli_input_body *slcli_input_ha2_member_storage_allocate(struct slcli_arena *,
  uint32_t,size_t);
#endif
#endif
