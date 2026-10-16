#include "allocator_caller_vectors.h"
#include "early_allocator.h"
int slcli_node_allocator_caller_positive_original(struct slcli_arena *a,
  struct slcli_node_allocator_caller *c,const struct slcli_source_deadline_original *d,
  const void *caller,size_t caller_bytes,struct slcli_node_allocator_wait_row *rows,size_t capacity) {
  unsigned char *first,*next,*zeroed,*aligned;
  size_t i;uint64_t charged;struct slcli_node_allocator_wait_row *r;
  /* Invocation is inside the original qualification constructor's serialized
   * initial-thread cut; independent before-state is the genuine retained owner.
   * Concurrent child binding/contention/expiry and original image/stack/TLS fit
   * need their actual native constructor/driver, not a fabricated row here. */
  if(!a||!a->owner||a->owner->failed||!d||capacity<SLCLI_NODE_ALLOCATOR_POSITIVE_MIN_ROWS)return 0;
  charged=a->owner->charged;
  if(!slcli_node_allocator_bind_original(a)||
    !slcli_node_allocator_prepare_caller_original(a,c,d,caller,caller_bytes,rows,capacity)||
    !slcli_node_allocator_attach_caller_original(c)||!slcli_node_allocator_bound())return 0;
  first=slcli_node_malloc(32);if(!first)return 0;
  for(i=0;i<32;i++)first[i]=(unsigned char)(i+1);
  next=slcli_node_realloc(first,64);if(!next)return 0;
  for(i=0;i<32;i++)if(next[i]!=(unsigned char)(i+1))return 0;
  r=&rows[c->acquisition_row];
  if(r->allocator_operation!=5||!r->old_retired||!r->old_raw||!r->new_raw||
    r->old_raw==r->new_raw||r->completed_result!=(uintptr_t)next||
    r->caller_result!=(uintptr_t)next||!r->result_ready)return 0;
  zeroed=slcli_node_calloc(4,8);if(!zeroed)return 0;
  for(i=0;i<32;i++)if(zeroed[i])return 0;
  aligned=slcli_node_aligned_alloc(64,33);if(!aligned||(uintptr_t)aligned%64)return 0;
  slcli_node_free(next);slcli_node_free(zeroed);slcli_node_free(aligned);
  /* Logical free does not prove native physical release or a FREE receipt.
   * The full original extent charge persists through all actual payload calls. */
  return !c->unresolved&&!c->acquired&&a->owner->charged==charged&&
    !a->owner->failed&&rows[c->acquisition_row].old_retired;
}
