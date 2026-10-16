#ifndef SLCLI_NODE_EARLY_ALLOCATOR_H
#define SLCLI_NODE_EARLY_ALLOCATOR_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
struct slcli_arena;
struct slcli_node_allocator_caller;
struct slcli_source_deadline_original;
struct slcli_node_allocator_wait_row;
struct slcli_node_child_reservation {
  void *storage;
  struct slcli_node_allocator_caller *child;
  struct slcli_node_allocator_wait_row *rows;
  size_t storage_bytes,capacity;
  size_t storage_offset,child_offset,rows_offset;
};
/* Pre-attachment, same-arena capture schedule. Slots are lifetime attempts,
 * never concurrent-thread permits and never recycled after failure or exit.
 * The containing source constructor must supply its complete finite T/E;
 * these inputs are not source authority or a numeric budget selection. */
struct slcli_node_child_capture_schedule {
  struct slcli_node_child_reservation *slots;
  size_t count,used,birth_bytes,event_capacity;
  size_t slots_offset,capture_begin,capture_end,capture_bytes;
  uint32_t started,ready;
  size_t prepared_slot,prepared_block,zero_scanned,nonzero_offset;
  unsigned char nonzero_value;
};
struct slcli_node_child_capture_schedule *
slcli_node_allocator_prepare_child_capture_original(struct slcli_arena *,size_t,size_t,size_t);
#if defined(_WIN32)
struct slcli_node_route_windows;
struct slcli_node_child_capture_schedule *
slcli_node_allocator_adopt_child_capture_windows_original(struct slcli_arena *,
 const struct slcli_node_route_windows *);
#endif
/* Includes a partial failed constructor; caller must separately require ready.
 * Retaining this address does not provide outside-process custody. */
struct slcli_node_child_capture_schedule *slcli_node_allocator_child_capture_original(void);
/* Exact existing arena header/alignment sum for this typed contribution only;
 * zero on overflow. Does not include other prefix bodies or page padding. */
size_t slcli_node_allocator_child_capture_bytes_original(size_t,size_t,size_t);
/* Exact consumed one-use typed slot, checked under the SAME allocator marker.
 * Native/source lifetime and outside-process custody remain independent. */
int slcli_node_allocator_child_storage_original(const void *,size_t);
/* Preentry source owner supplies the actual already charged arena. This binds
 * storage once; it authenticates no issuer, native actor, measured ceiling or
 * loader/CRT extent. No late initialization, ambient allocation or quota is
 * available. Complete genuine preentry constructor/integration remains required. */
int slcli_node_allocator_bind_original(struct slcli_arena *);
int slcli_node_allocator_bound(void);
/* Actual current-thread address of the existing static C TLS caller slot.
 * No value read, initialization, allocation or TLS/source admission occurs. */
const void *slcli_node_allocator_tls_slot_original(void);
/* Entire original bootstrap pipeline result, separately from the allocator
 * attachment intermediate effect; this predicate grants no native admission. */
int slcli_node_bootstrap_entry_complete_original(void);
/* Private genuine serialized constructor only. TLS/capture storage and the
 * actual original native caller/thread lifetime must already be charged. */
int slcli_node_allocator_prepare_caller_original(struct slcli_arena *,
  struct slcli_node_allocator_caller *,const struct slcli_source_deadline_original *,
  const void *,size_t,struct slcli_node_allocator_wait_row *,size_t);
int slcli_node_allocator_attach_caller_original(struct slcli_node_allocator_caller *);
/* Under the SAME actual allocator marker, before native child birth. This
 * prepares custody only: real native creator/birth and pre-PTD attachment
 * remain required, with no parent-ID substitution or new deadline. */
int slcli_node_allocator_prepare_child_original(struct slcli_arena *,
  struct slcli_node_allocator_caller *,struct slcli_node_allocator_wait_row *,size_t);
/* Actual private source constructor. Consumes one prepared lifetime-attempt
 * slot; no late capture allocation or reuse. The containing preentry must
 * prepare the complete source-derived schedule before attaching its caller.
 * Event capacity matches the creator's original finite capacity, never a new
 * native/runtime quota or fit claim. */
struct slcli_node_child_reservation *slcli_node_allocator_reserve_child_original(size_t);
/* Exact canonical SAME allocator payload and requested size, under its actual
 * owning marker. Containment alone never admits a CRT parameter object. */
int slcli_node_allocator_allocation_original(void *,size_t);
int slcli_node_allocator_is_current_original(const struct slcli_node_allocator_caller *);
/* Private actual C++ new-handler boundary. Output ordinal is a retained row,
 * never caller/native authority. Completion records normal return or unwind. */
int slcli_node_allocator_handler_before_original(void (*)(void),size_t *);
int slcli_node_allocator_handler_after_original(void (*)(void),size_t,int);
int slcli_node_allocator_crt_handler_before_original(int (*)(size_t),size_t,size_t *);
int slcli_node_allocator_crt_handler_after_original(int (*)(size_t),size_t,size_t,int,int);
int slcli_node_allocator_native_before_original(uint32_t,const void *,size_t,uint64_t,uint64_t,size_t *);
int slcli_node_allocator_native_after_original(uint32_t,const void *,size_t,size_t,uint64_t);
int slcli_node_allocator_native_raw_return_original(uint32_t,const void *,size_t,size_t,uint64_t);
int slcli_node_allocator_wait_before_original(const void *,size_t,uint64_t,uint32_t *,size_t *);
void *slcli_node_malloc(size_t);
void *slcli_node_calloc(size_t,size_t);
void *slcli_node_realloc(void *,size_t);
void slcli_node_free(void *);
/* Pure SAME-row disposition after an original void free. No native effect,
 * lock, clock, storage reuse, outside custody or global retirement grant. */
size_t slcli_node_allocator_operation_frontier_original(void);
int slcli_node_allocator_free_retired_original(const void *,size_t);
void *slcli_node_aligned_alloc(size_t,size_t);
size_t slcli_node_msize(void *);
void *slcli_node_expand(void *,size_t);
void *slcli_node_recalloc(void *,size_t,size_t);
#ifdef __cplusplus
}
#endif
#endif
