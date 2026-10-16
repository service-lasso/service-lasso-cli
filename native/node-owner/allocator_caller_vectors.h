#ifndef SLCLI_NODE_ALLOCATOR_CALLER_VECTORS_H
#define SLCLI_NODE_ALLOCATOR_CALLER_VECTORS_H
#include "allocator_caller.h"
#define SLCLI_NODE_ALLOCATOR_POSITIVE_MIN_ROWS (9u*4u)
/* Genuine native fixture constructor supplies all originals/storage before
 * this initial-thread driver. No fabricated address, clock, principal or quota. */
int slcli_node_allocator_caller_positive_original(struct slcli_arena *,
  struct slcli_node_allocator_caller *,const struct slcli_source_deadline_original *,
  const void *,size_t,struct slcli_node_allocator_wait_row *,size_t);
#endif
