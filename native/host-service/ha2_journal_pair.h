#ifndef SLCLI_HA2_JOURNAL_PAIR_H
#define SLCLI_HA2_JOURNAL_PAIR_H
#include "ha2_journal_codec.h"
#include "store_body.h"
#include "store_deadline.h"
#if defined(_WIN32) || defined(__linux__)
/* One-use actual append/flush/full-prefix readback to TWO distinct held native
 * journal originals. Private BQ marker/source admission, signing, frontier and
 * high-water publication are separate prerequisites, never supplied by IO. */
struct slcli_ha2_journal_pair {
  struct slcli_arena *arena;
  struct slcli_store_deadline_owner *deadline_owner;
  struct slcli_store_stream *before[2],*write[2],*after[2];
  const unsigned char *const *committed;
  const unsigned char *candidate;
  uint64_t prior_bytes,final_bytes,retained_store_bytes;
  uint32_t started,failed,complete,committed_count;
};
struct slcli_ha2_journal_pair *slcli_ha2_journal_pair_allocate(struct slcli_arena *,size_t);
/* Private fresh original caller preparation; no BQ or journal authority. */
int slcli_ha2_journal_pair_deadline_bind(struct slcli_ha2_journal_pair *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,slcli_store_capture_sink,void *,size_t);
int slcli_ha2_journal_pair_original(struct slcli_ha2_journal_pair *,
  const unsigned char *const *,uint32_t,const unsigned char[772],
  slcli_input_handle,slcli_input_handle,uint64_t,slcli_store_capture_sink,void *);
#endif
#endif
