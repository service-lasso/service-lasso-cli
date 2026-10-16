#ifndef SLCLI_STORE_BODY_H
#define SLCLI_STORE_BODY_H
#include "input_body.h"
#include "ledger_state.h"
#include "unit19.h"
#if defined(_WIN32) || defined(__linux__)
/* Original capture transfer must persist/read back the actual raw ABI row and
 * actual native input/returned bytes BEFORE reusable scratch changes. This is
 * an internal original source-authority parameter, not request dispatch, an
 * authority constructor or a Boolean proof of archive qualification. The real
 * admitted observer/capture owner remains an external unqualified dependency. */
typedef int (*slcli_store_capture_sink)(void *,const slcli_input_capture *,
                                        const unsigned char *,size_t);
/* Private pre-effect callback, prepared by the genuine original owner. A null
 * pair is transitional and supplies no deadline/native admission. */
struct slcli_store_stream;
typedef int (*slcli_store_capture_before)(void *,struct slcli_store_stream *,slcli_input_capture *);
typedef int (*slcli_store_capture_after)(void *,const slcli_input_capture *,
  const unsigned char *,size_t,int);
struct slcli_store_stream {
  slcli_store_capture_before capture_before;
  slcli_store_capture_after capture_after;
  void *after_original;
  void *before_original;
  unsigned char chunk[1024],readback[1025];
  /* Genuine new-unit stream geometry is selected by its storage constructor
   * before effects; ordinary ledger streams retain their original1024 buffers. */
  unsigned char *unit_chunk,*unit_readback;
  uint32_t unit_geometry;
  slcli_input_capture *captures;
  slcli_input_handle original;
  slcli_store_capture_sink capture_sink;
  void *capture_original;
  size_t capture_count,capture_capacity,buffered;
  uint64_t offset,expected;
  uint32_t attempted,mode,failed,held_verified;
  struct slcli_sha256 bytes_hash;
  unsigned char digest[32];
};
/* Storage only. No fd/HANDLE/profile/key/callback supplies original admission.
 * All rows/control/scratch share the original charged arena and remain held. */
struct slcli_store_stream *slcli_store_stream_allocate(struct slcli_arena *,size_t);
struct slcli_store_stream *slcli_store_unit19_allocate(struct slcli_arena *,size_t);
int slcli_store_stream_begin(struct slcli_store_stream *,slcli_input_handle,
                              uint32_t,uint64_t,slcli_store_capture_sink,void *);
/* Append to the SAME held original only after owning source compared the
 * complete existing committed prefix. Native initial EOF must equal prior;
 * finish flushes and observes exact final EOF without overwriting old bytes.
 * Result digest covers only the newly appended bytes, never the old prefix. */
int slcli_store_stream_append_begin(struct slcli_store_stream *,slcli_input_handle,
  uint64_t,uint64_t,slcli_store_capture_sink,void *);
/* Separate source-selected journal geometry: exact multiples of772, at most
 *1024 records. Ordinary ledger stream cap262144+36 is unchanged. */
int slcli_store_journal_compare_begin(struct slcli_store_stream *,slcli_input_handle,
  uint64_t,slcli_store_capture_sink,void *);
int slcli_store_journal_append_begin(struct slcli_store_stream *,slcli_input_handle,
  uint64_t,uint64_t,slcli_store_capture_sink,void *);
/* Selected whole-unit geometry, separately checked before IO. No ledger limit
 * changes; whole file stays within the unchanged combined store4MiB bound. */
int slcli_store_unit19_begin(struct slcli_store_stream *,slcli_input_handle,uint32_t,
  const struct slcli_unit19_reader *,slcli_store_capture_sink,void *);
int slcli_store_stream_append(void *,const unsigned char *,size_t);
int slcli_store_stream_finish(struct slcli_store_stream *);
/* Identity/owner/rights/alias metadata comparison across separately observed
 * passes on a mutable original. Does not compare changing EOF/timestamps. */
int slcli_store_original_same(const slcli_input_capture *,const slcli_input_capture *);
/* Original decoded cells/key stay immutable and charged across all passes.
 * Writes include actual flush; replay compares every byte, original EOF and
 * before/after identity, including real short reads. Atomic namespace publish,
 * parent flush/reopened readback/selector commit/archive are later owning steps. */
int slcli_store_ledger_write(struct slcli_store_stream *,
                             const struct slcli_ledger_state *,const unsigned char[32],
                             uint32_t *,unsigned char[32]);
int slcli_store_ledger_compare(struct slcli_store_stream *,
                               const struct slcli_ledger_state *,const unsigned char[32],
                               uint32_t,unsigned char[32]);
#endif
#endif
