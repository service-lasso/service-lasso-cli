#ifndef SLCLI_NODE_BOOTSTRAP_TRANSPORT_H
#define SLCLI_NODE_BOOTSTRAP_TRANSPORT_H
#include "../host-service/native_clock.h"
#include "../host-service/owner_handoff_codec.h"
#include <stddef.h>
#if defined(_WIN32)
#include <windows.h>
#if !defined(_WIN64)
#error Original Windows bootstrap transport selects x64 source ABI
#endif
#define SLCLI_BOOTSTRAP_EXPECTED_BYTES sizeof(FILETIME)
#define SLCLI_BOOTSTRAP_ABI 1u
#define SLCLI_BOOTSTRAP_PARENT_BYTES (32u+SECURITY_MAX_SID_SIZE)
#define SLCLI_BOOTSTRAP_MAPPED_VIEW_BYTES 64u
#elif defined(__linux__)
#include <asm/stat.h>
#define SLCLI_BOOTSTRAP_EXPECTED_BYTES sizeof(struct stat)
#define SLCLI_BOOTSTRAP_PARENT_BYTES 0u
#define SLCLI_BOOTSTRAP_MAPPED_VIEW_BYTES 0u
#if defined(__x86_64__)
#define SLCLI_BOOTSTRAP_ABI 2u
#elif defined(__aarch64__)
#define SLCLI_BOOTSTRAP_ABI 3u
#else
#error Original bootstrap transport requires separately reviewed native ABI
#endif
#else
#error Original bootstrap transport requires selected Windows or Linux source
#endif
/* Exact private source representation, not public LAUNCH JSON. Native raw
 * clock argument numbers remain original observations; never dereference them
 * as child-local pointers. Scalar capabilities route actual inherited originals
 * and grant no parent/caller/source/private authority. */
#define SLCLI_BOOTSTRAP_TRANSPORT_BYTES (344u+4u+256u*24u+SLCLI_BOOTSTRAP_PARENT_BYTES+SLCLI_BOOTSTRAP_MAPPED_VIEW_BYTES+SLCLI_BOOTSTRAP_EXPECTED_BYTES+SLCLI_OWNER_HANDOFF_BYTES)
#if defined(_WIN32)
/* Copied native outcomes, never an acquired child owner or source grant. */
struct slcli_node_mapped_view_observation {
 uint64_t base,bytes,section,view_version,section_version,query_tick;
 uint32_t status,protection,state,type;
};
_Static_assert(sizeof(struct slcli_node_mapped_view_observation)==64,"Mapped view observation ABI");
#endif
enum slcli_node_prefix_purpose { SLCLI_PREFIX_IMAGE=1,SLCLI_PREFIX_STACK=2,
  SLCLI_PREFIX_TLS_LOADER=4,SLCLI_PREFIX_INGRESS=8 };
struct slcli_node_prefix_row {
  uint32_t purpose,index;
  uint64_t sequence,reserved;
};
/* Source-owned original plan; serialized rows are custody copies only.
 * Existing256 is ledger representation capacity, never a new allowance. */
struct slcli_node_prefix_plan {
  uint32_t count;
  struct slcli_node_prefix_row rows[256];
};
struct slcli_node_bootstrap_transport {
  struct slcli_native_clock origin;
  struct slcli_extent_request runtime_request;
  struct slcli_memory_owner frozen_owner;
  uint64_t expires,owner_limit,bootstrap_bytes,runtime_bytes,capacity;
  uint64_t thread_capture_attempts;
  uint64_t capabilities[4],process_original,thread_original,birth_raw[2];
  unsigned char associations[4][32];
  unsigned char expected_native[SLCLI_BOOTSTRAP_EXPECTED_BYTES];
  struct slcli_node_prefix_plan prefix;
#if defined(_WIN32)
  /* Copied observations, not authority; no native SID pointer crosses birth. */
  uint32_t parent_pid,parent_creation[2],parent_sid_bytes;
  unsigned char parent_sid[SECURITY_MAX_SID_SIZE];
  uint32_t parent_token_id[2],parent_authentication_id[2];
  struct slcli_node_mapped_view_observation mapped_view;
#endif
};
int slcli_node_prefix_plan_matches(const struct slcli_node_prefix_plan *,
  const struct slcli_memory_owner *,const struct slcli_extent_request *);
struct slcli_arena;
struct slcli_source_deadline_original;
struct slcli_node_prefix_original {
  const struct slcli_node_prefix_plan *plan_original;
  const struct slcli_memory_owner *target_original;
  const struct slcli_extent_request *runtime_original;
  const struct slcli_source_deadline_original *deadline_original;
  const struct slcli_native_clock *prior_original;
  const void *caller_original;
  size_t caller_bytes;
  uint64_t target_charged,target_next_sequence,expires,origin_tick;
  uint32_t target_count;
  unsigned char associations[4][32];
  struct slcli_node_prefix_plan snapshot;
  struct slcli_extent_request runtime_snapshot;
  const struct slcli_extent *extent_originals[256];
  struct slcli_native_clock rows[256],after;
  uint32_t started,copied,terminal,returned;
};
int slcli_node_prefix_prepare_original(struct slcli_node_prefix_original *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
  const struct slcli_native_clock *,const struct slcli_node_prefix_plan *,
  const struct slcli_memory_owner *,const struct slcli_extent_request *);
int slcli_node_prefix_original_matches(const struct slcli_node_prefix_original *,
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,
  const struct slcli_memory_owner *,const struct slcli_extent_request *);
struct slcli_node_bootstrap_decode {
  const unsigned char *raw_original;
  size_t raw_bytes_original;
  struct slcli_node_bootstrap_transport transport;
  struct slcli_owner_handoff_decode owner_decode;
  uint32_t started,returned,qualified;
};
/* Pure byte construction/interpretation in original already charged storage.
 * Actual native entry owns the raw transport exclusively through admission and
 * local reconstruction. These routines perform no native admission or IO. */
int slcli_node_bootstrap_transport_encode(unsigned char *,size_t,
  const struct slcli_node_bootstrap_transport *);
int slcli_node_bootstrap_transport_decode(struct slcli_node_bootstrap_decode *,
  const unsigned char *,size_t);
#endif
