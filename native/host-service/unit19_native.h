#ifndef SLCLI_UNIT19_NATIVE_H
#define SLCLI_UNIT19_NATIVE_H
#include "store_body.h"
#include "store_deadline.h"
#if defined(_WIN32) || defined(__linux__)
/* Full original member bytes remain in their own charged source ranges. A
 * new-unit stream owns16384-byte write/16385-byte readback scratch and all captures;
 * no whole-unit read-all or additional owner/pool is introduced. This performs
 * new-unit preparation IO, never birth, seal, provider or unit admission. */
struct slcli_unit19_native {
  struct slcli_arena *arena;
  struct slcli_store_deadline_owner *deadline_owner;
  struct slcli_unit19_reader parsed;
  struct slcli_sha256 member_hash;
  struct slcli_store_stream *write,*held;
  uint64_t retained_store_bytes;
  uint32_t started,failed,prepared,seal_attempted,creator_closed;
};
struct slcli_unit19_native *slcli_unit19_native_allocate(struct slcli_arena *,size_t,size_t);
/* Fresh source-owned write/readback clocks before native output birth/effects.
 * Caller/source/provider/seal authority remains independently required. */
int slcli_unit19_native_deadline_bind(struct slcli_unit19_native *,
  const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,slcli_store_capture_sink,void *,size_t);
int slcli_unit19_native_framing_storage(struct slcli_unit19_native *,
  const struct slcli_unit19_framing *,const unsigned char *const *,
  const struct slcli_unit19_record *,size_t,const uint64_t *,size_t);
int slcli_unit19_native_prepare(struct slcli_unit19_native *,enum slcli_unit19_kind,uint32_t,
  const unsigned char[64],const unsigned char *,size_t,const unsigned char *const *,
  const struct slcli_unit19_record *,const uint64_t *,size_t,
  slcli_input_handle,slcli_store_capture_sink,void *,uint64_t);
#endif
#endif
