#include "bootstrap.h"
#if defined(_WIN32)
#include "entry_windows.h"
#include "../host-service/arena_layout.h"
int slcli_node_bootstrap_adopt_tail_windows_original(void) {
 struct slcli_node_bootstrap *c=slcli_node_bootstrap_image_original();
 const struct slcli_node_entry_windows_capture *entry;
 const struct slcli_node_bootstrap_transport *t;
 struct slcli_node_capture_layout_windows shape;struct slcli_extent *e;
 struct slcli_arena_block_header *tail;size_t floor,offset;
 if(!c||!c->storage_bound||c->tail_adopt_started||!c->storage_entry_original||
  !c->transport_original||
  c->owner.handoff_state!=SLCLI_OWNER_CHILD||!slcli_memory_owner_mutable(&c->owner)||
  c->owner.failed||c->owner.count>256)return 0;
 entry=c->storage_entry_original;
 if(!slcli_node_capture_layout_windows_live_headers_original(&entry->route,&shape)||
  (void *)entry!=(void *)((unsigned char *)entry->route.base+shape.stages[SLCLI_CAPTURE_ENTRY].offset)||
  entry->receiver!=(struct slcli_node_receive_windows *)((unsigned char *)entry->route.base+shape.stages[SLCLI_CAPTURE_RECEIVE].offset)||
  entry->loader!=(struct slcli_node_loader_windows *)((unsigned char *)entry->route.base+shape.stages[SLCLI_CAPTURE_LOADER].offset)||
  c->transport_original!=&entry->receiver->decode||!c->transport_original->qualified||
  entry->bootstrap_original!=c||
  (void *)c!=(void *)((unsigned char *)entry->route.base+shape.stages[SLCLI_CAPTURE_BOOTSTRAP].offset))return 0;
 t=&c->transport_original->transport;
 if(!entry->loader_returned||!entry->loader_result||
  !entry->loader->own_tls_result||!entry->loader->own_tls_call_returned||
  !entry->loader->own_tls_returned||!entry->loader->own_tls_after_result||
  !entry->loader->own_tls_after.complete||!entry->loader->own_tls_after.tick||
  entry->loader->expires!=t->expires||
  t->runtime_request.index>=c->owner.count||t->runtime_request.index>=256)return 0;
 e=&c->owner.extents[t->runtime_request.index];floor=shape.capture_bytes;
 /* This requires an independently acquired original; a matching route or
  * copied frozen owner cannot change BACKING_ACQUIRED into LIVE here. */
 if(e->state!=SLCLI_EXTENT_LIVE||e->category!=SLCLI_RUNTIME||e->arena_initialized||
  !e->acquisition||e->acquisition!=t->runtime_request.sequence||
  !e->latest||e->retirement||e->backed_observed>1||
  (!e->backed_observed&&e->backed)||e->backed>e->reserved||
  e->base!=(uintptr_t)entry->route.base||e->reserved!=entry->route.bytes||
  e->reserved!=t->runtime_bytes||e->reserved>SIZE_MAX||e->reserved%16||
  !slcli_memory_owner_mapped_runtime_prefix_matches(&c->owner,&t->frozen_owner,
   &t->runtime_request,e->base,e->reserved)||
  floor%16||floor/16>UINT32_MAX-1u||floor>e->reserved||e->reserved-floor<48||
  (uintptr_t)c<e->base||(uintptr_t)c-e->base>floor||
  sizeof(*c)>floor-((uintptr_t)c-e->base))return 0;
 /* Fresh original descriptor only. Do not overwrite a bound arena by choosing
  * this otherwise unattached child ledger as a different supplied owner. */
 for(offset=0;offset<sizeof(c->arena);offset++)
  if(((const unsigned char *)&c->arena)[offset])return 0;
 c->tail_adopt_started=1;
 c->tail_before_result=slcli_native_clock_read(&c->tail_before);
 if(!c->tail_before_result||c->tail_before.tick<entry->loader->own_tls_after.tick||
  (c->before.complete&&c->tail_before.tick<c->before.tick)||
  c->tail_before.tick>=t->expires)return 0;
 /* Prefix headers/payloads are retained untouched; only fresh tail bytes are
  * examined. A failed scan is terminal for this original constructor. */
 for(offset=floor;offset<(size_t)e->reserved;offset++) {
  unsigned char value=((const unsigned char *)e->base)[offset];
  c->tail_zero_scanned=offset-floor+1;
  if(value){c->tail_nonzero_offset=offset;c->tail_nonzero_value=value;return 0;}
 }
 /* Recheck the existing headers after the scan, before the first write. */
 if(!slcli_node_capture_layout_windows_live_headers_original(&entry->route,&shape)||
  shape.capture_bytes!=floor||e->state!=SLCLI_EXTENT_LIVE||e->arena_initialized||
  e->base!=(uintptr_t)entry->route.base||e->reserved!=entry->route.bytes||
  e->acquisition!=t->runtime_request.sequence||
  !slcli_memory_owner_mapped_runtime_prefix_matches(&c->owner,&t->frozen_owner,
   &t->runtime_request,e->base,e->reserved))return 0;
 c->tail_scanned_result=slcli_native_clock_read(&c->tail_scanned);
 if(!c->tail_scanned_result||c->tail_scanned.tick<c->tail_before.tick||
  c->tail_scanned.tick>=t->expires)return 0;
 tail=(struct slcli_arena_block_header *)(e->base+floor);
 c->tail_header_attempted=1;
 tail->bytes=e->reserved-floor;tail->requested=0;tail->next=0;
 tail->state=SLCLI_ARENA_BLOCK_FREE;c->tail_header_written=1;
 c->arena.owner=&c->owner;c->arena.extent=t->runtime_request.index;
 c->arena.acquisition=e->acquisition;c->arena.allocation_floor=floor;
 e->arena_initialized=(uint32_t)(floor/16)+1u;
 c->tail_after_result=slcli_native_clock_read(&c->tail_after);
 if(!c->tail_after_result||c->tail_after.tick<c->tail_scanned.tick||
  c->tail_after.tick>=t->expires)return 0;
 c->tail_adopted=1;
 /* No allocation, charge, sequence or acquisition is invented. Native clock
  * failures retain the scan/header/publication frontier above. */
 return 1;
}
/* Routing cell only. All owning Windows constructor/decode bytes are retained
 * in the parent's exact preplanned shared capture pages. Image cell/codegen
 * accounting and native source authority remain independent obligations. */
static struct slcli_node_bootstrap *image_bootstrap_original;
int slcli_node_bootstrap_route_storage_windows_original(struct slcli_node_entry_windows_capture *entry) {
 struct slcli_node_capture_layout_windows shape;struct slcli_node_bootstrap *c;
 struct slcli_arena_block_header expected;const unsigned char *header;size_t i,offset;
 if(!entry||image_bootstrap_original||entry->bootstrap_storage_attempted||
  !slcli_node_capture_layout_windows_route_shape_original(&entry->route,&shape)||
  (void *)entry!=(void *)((unsigned char *)entry->route.base+shape.stages[SLCLI_CAPTURE_ENTRY].offset)||
  entry->entry_original!=(const void *)&slcli_node_entry_windows||
  !entry->receive_returned||!entry->receive_result||
  entry->receiver!=(struct slcli_node_receive_windows *)((unsigned char *)entry->route.base+shape.stages[SLCLI_CAPTURE_RECEIVE].offset))return 0;
 entry->bootstrap_storage_attempted=1;
 if(!entry->receiver->decode_returned||!entry->receiver->decode_result||
  !entry->receiver->decode.qualified||!entry->receiver->parent_returned||!entry->receiver->parent_result||
  entry->receiver->entry_original!=entry->entry_original||
  entry->receiver->transport_original!=entry->route.transport||entry->receiver->parent_original!=entry->route.parent)return 0;
 offset=shape.stages[SLCLI_CAPTURE_BOOTSTRAP].offset;
 if(offset<sizeof(expected)||shape.stages[SLCLI_CAPTURE_BOOTSTRAP].requested!=sizeof(*c))return 0;
 expected.bytes=shape.stages[SLCLI_CAPTURE_BOOTSTRAP].charge;expected.requested=sizeof(*c);
 expected.next=offset-sizeof(expected)+expected.bytes;expected.state=SLCLI_ARENA_BLOCK_LIVE;
 header=(const unsigned char *)entry->route.base+offset-sizeof(expected);
 for(i=0;i<sizeof(expected);i++)if(header[i]!=((const unsigned char *)&expected)[i])return 0;
 c=(struct slcli_node_bootstrap *)((unsigned char *)entry->route.base+offset);
 for(i=0;i<sizeof(*c);i++){
  unsigned char value=((const unsigned char *)c)[i];entry->bootstrap_zero_scanned=i+1;
  if(value){entry->bootstrap_nonzero_offset=i;entry->bootstrap_nonzero_value=value;return 0;}
 }
 entry->bootstrap_original=c;c->storage_entry_original=entry;
 c->transport_original=&entry->receiver->decode;c->storage_bound=1;
 image_bootstrap_original=c;return 1;
}
#else
/* Linux's owning initial-stack/shared-stage integration remains unfinished. */
static struct slcli_node_bootstrap image_bootstrap;
static struct slcli_node_bootstrap_decode image_transport;
#endif
struct slcli_node_bootstrap *slcli_node_bootstrap_image_original(void) {
#if defined(_WIN32)
  return image_bootstrap_original;
#else
  return &image_bootstrap;
#endif
}
static int image_charged(struct slcli_node_bootstrap *c) {
  size_t i;uintptr_t base=(uintptr_t)c;
  if(!c||c->owner.failed||!c->owner.limit||!c->owner.next_sequence||
     !c->owner.native_page_size||c->owner.count>256||
     base>UINTPTR_MAX-sizeof(*c))return 0;
  for(i=0;i<c->owner.count;i++) {
    const struct slcli_extent *e=&c->owner.extents[i];
    if(e->state==SLCLI_EXTENT_LIVE&&e->category==SLCLI_RUNTIME&&e->acquisition&&
       e->base&&e->reserved<=UINTPTR_MAX&&e->base<=UINTPTR_MAX-e->reserved&&
       base>=e->base&&base+sizeof(*c)<=e->base+e->reserved)return 1;
  }
  return 0;
}
/* Original transport storage must itself remain charged and separate from the
 * mutable constructor. Containment is storage custody, never admission. */
static int input_charged(struct slcli_node_bootstrap *c,const void *input,size_t bytes) {
  uintptr_t p=(uintptr_t)input,base=(uintptr_t)c;size_t i;
  if(!input||!bytes||p>UINTPTR_MAX-bytes||
     (p<base+sizeof(*c)&&base<p+bytes))return 0;
  for(i=0;i<c->owner.count&&i<256;i++) {
    const struct slcli_extent *e=&c->owner.extents[i];
    if(e->state==SLCLI_EXTENT_LIVE&&e->base&&e->reserved<=UINTPTR_MAX&&
       e->base<=UINTPTR_MAX-e->reserved&&p>=e->base&&p+bytes<=e->base+e->reserved)return 1;
  }
  return 0;
}
int slcli_node_bootstrap_acquire_original(const void *entry,
  const struct slcli_native_clock *origin,uint64_t expires,uint64_t bytes) {
  struct slcli_node_bootstrap *c=slcli_node_bootstrap_image_original();size_t i;uintptr_t origin_address=(uintptr_t)origin;
  /* Entry identity and charged geometry are predicates, not source/native
   * authority. The actual native entry owns this entire one-use constructor. */
  if(!c||c->started||!entry||!origin||!image_charged(c)||
     !input_charged(c,entry,1)||!input_charged(c,origin,sizeof(*origin))||
     !origin->complete||!origin->tick||expires<=origin->tick||!bytes||
     origin_address>UINTPTR_MAX-sizeof(*origin)||
     (origin_address<(uintptr_t)c+sizeof(*c)&&(uintptr_t)c<origin_address+sizeof(*origin)))return 0;
  c->started=1;c->entry_original=entry;c->expires_original=expires;
  c->bootstrap_bytes_original=bytes;
  for(i=0;i<sizeof(*origin);i++)
    ((unsigned char *)&c->origin_original)[i]=((const unsigned char *)origin)[i];
  if(!slcli_native_clock_read(&c->before)||c->before.tick<origin->tick||
     c->before.tick>=expires)return 0;
#if defined(_WIN32)
  {
    struct slcli_node_capture_layout_windows shape;size_t stage,j;
    const struct slcli_node_entry_windows_capture *storage=c->storage_entry_original;
    if(!storage||!c->transport_original||c->transport_original->transport.expires!=expires||
      !storage->loader||!storage->loader_returned||!storage->loader_result||
      !storage->loader->own_tls_call_returned||!storage->loader->own_tls_result||
      !storage->loader->own_tls_returned||!storage->loader->own_tls_after_result||
      !storage->loader->own_tls_after.complete||
      c->before.tick<storage->loader->own_tls_after.tick||
      c->transport_original->transport.bootstrap_bytes!=bytes||
      !slcli_node_bootstrap_adopt_tail_windows_original()||
      !slcli_node_capture_layout_windows_live_headers_original(&storage->route,&shape))return 0;
    /* These exact controls were already physically charged and laid out before
     * birth. No tail allocation or fresh native acquisition is performed. */
    for(stage=SLCLI_CAPTURE_BOOTSTRAP_CALLER;stage<=SLCLI_CAPTURE_BOOTSTRAP_DEADLINE;stage++) {
      const unsigned char *payload=(const unsigned char *)storage->route.base+shape.stages[stage].offset;
      c->control_stage=(uint32_t)stage;c->control_zero_scanned=0;
      for(j=0;j<shape.stages[stage].requested;j++) {
        unsigned char value=payload[j];c->control_zero_scanned=j+1;
        if(value){c->control_nonzero_stage=(uint32_t)stage;c->control_nonzero_offset=j;
          c->control_nonzero_value=value;return 0;}
      }
    }
    c->caller=(struct slcli_node_bootstrap_caller *)((unsigned char *)storage->route.base+shape.stages[SLCLI_CAPTURE_BOOTSTRAP_CALLER].offset);
    c->local_origin=(struct slcli_native_clock *)((unsigned char *)storage->route.base+shape.stages[SLCLI_CAPTURE_BOOTSTRAP_ORIGIN].offset);
    c->local_predecessor=(struct slcli_native_clock *)((unsigned char *)storage->route.base+shape.stages[SLCLI_CAPTURE_BOOTSTRAP_PREDECESSOR].offset);
    c->deadline=(struct slcli_source_deadline_original *)((unsigned char *)storage->route.base+shape.stages[SLCLI_CAPTURE_BOOTSTRAP_DEADLINE].offset);
    if(!slcli_arena_live_block(&c->arena,c->caller,sizeof(*c->caller))||
      !slcli_arena_live_block(&c->arena,c->local_origin,sizeof(*c->local_origin))||
      !slcli_arena_live_block(&c->arena,c->local_predecessor,sizeof(*c->local_predecessor))||
      !slcli_arena_live_block(&c->arena,c->deadline,sizeof(*c->deadline)))return 0;
    if(!slcli_native_clock_read(&c->after)||c->after.tick<c->tail_after.tick||c->after.tick>=expires)return 0;
  }
#else
  c->acquire_result=slcli_platform_acquire(&c->owner,SLCLI_RUNTIME,bytes,&c->acquisition);
  c->acquire_returned=1;
  /* Actual native acquisition and helper return precede this fallible clock.
   * Unknown/late native outputs keep their original extent/capture custody. */
  c->postclock_result=slcli_native_clock_read(&c->after)&&
    c->after.tick>=c->before.tick&&c->after.tick<expires;
  if(!c->acquire_result||!c->postclock_result) {
    if(c->acquisition.observed&&c->acquisition.request.sequence) {
      c->retained_result=slcli_memory_unobserved(&c->owner,&c->acquisition.request);
      c->retained_returned=1;
    }
    return 0;
  }
  c->arena_result=slcli_arena_init(&c->arena,&c->owner,c->acquisition.request.index);
  c->arena_returned=1;if(!c->arena_result)return 0;
  c->caller=(struct slcli_node_bootstrap_caller *)slcli_arena_calloc(&c->arena,1,sizeof(*c->caller));
  c->local_origin=(struct slcli_native_clock *)slcli_arena_alloc(&c->arena,sizeof(*c->local_origin));
  c->local_predecessor=(struct slcli_native_clock *)slcli_arena_alloc(&c->arena,sizeof(*c->local_predecessor));
  c->deadline=(struct slcli_source_deadline_original *)slcli_arena_calloc(&c->arena,1,sizeof(*c->deadline));
#endif
  if(!c->caller||!c->local_origin||!c->local_predecessor||!c->deadline)return 0;
  for(i=0;i<sizeof(*c->local_predecessor);i++)
    ((unsigned char *)c->local_predecessor)[i]=((const unsigned char *)&c->after)[i];
#if defined(__linux__)
  if(!slcli_native_clock_linux_representation_matches_original(c->local_predecessor,&c->after))return 0;
#endif
  for(i=0;i<sizeof(*c->local_origin);i++)
    ((unsigned char *)c->local_origin)[i]=((const unsigned char *)&c->origin_original)[i];
  c->caller->entry_original=entry;c->caller->transported_origin_original=&c->origin_original;
  c->caller->transported_expires_original=expires;
  c->deadline->origin=c->local_origin;c->deadline->expires=expires;c->deadline->caller_original=c->caller;
  c->constructed=1;return 1;
}
#if defined(_WIN32)
int slcli_node_bootstrap_windows_bind_original(HANDLE backing,HANDLE expected_backing,
  uint64_t bytes,const FILETIME *creation,size_t capacity,size_t thread_capture_attempts,const struct slcli_extent_request *request_original) {
  struct slcli_node_bootstrap *c=slcli_node_bootstrap_image_original();
  struct slcli_node_capture_layout_windows shape;size_t i,stage,j;
  if(!c||!c->constructed||!c->tail_adopted||!c->storage_entry_original||c->bind_attempted||!input_charged(c,creation,sizeof(*creation))||
    !input_charged(c,request_original,sizeof(*request_original)))return 0;
  if(!slcli_node_capture_layout_windows_live_headers_original(&c->storage_entry_original->route,&shape)||
    capacity!=shape.events||thread_capture_attempts!=shape.threads||
    bytes!=c->storage_entry_original->route.bytes||request_original->index!=c->arena.extent||
    request_original->sequence!=c->arena.acquisition)return 0;
  c->bind_attempted=1;
  c->bind_storage_before_result=slcli_native_clock_read(&c->bind_storage_before);
  if(!c->bind_storage_before_result||c->bind_storage_before.tick<c->after.tick||
    c->bind_storage_before.tick>=c->expires_original)return 0;
  for(stage=SLCLI_CAPTURE_RUNTIME_REQUEST;stage<=SLCLI_CAPTURE_PREENTRY;stage++) {
    unsigned char *payload=(unsigned char *)c->storage_entry_original->route.base+shape.stages[stage].offset;
    if(!slcli_arena_live_block(&c->arena,payload,shape.stages[stage].requested))return 0;
    c->control_stage=(uint32_t)stage;c->control_zero_scanned=0;
    for(j=0;j<shape.stages[stage].requested;j++) {
      unsigned char value=payload[j];c->control_zero_scanned=j+1;
      if(value){c->control_nonzero_stage=(uint32_t)stage;c->control_nonzero_offset=j;
        c->control_nonzero_value=value;return 0;}
    }
  }
  c->local_runtime_request=(void *)((unsigned char *)c->storage_entry_original->route.base+shape.stages[SLCLI_CAPTURE_RUNTIME_REQUEST].offset);
  c->local_runtime_request->index=request_original->index;
  c->local_runtime_request->sequence=request_original->sequence;
  for(i=0;i<sizeof(*creation);i++)((unsigned char *)&c->creation_original)[i]=((const unsigned char *)creation)[i];
  c->local_creation=(void *)((unsigned char *)c->storage_entry_original->route.base+shape.stages[SLCLI_CAPTURE_CREATION].offset);
  for(i=0;i<sizeof(*c->local_creation);i++)((unsigned char *)c->local_creation)[i]=((const unsigned char *)&c->creation_original)[i];
  c->node=(void *)((unsigned char *)c->storage_entry_original->route.base+shape.stages[SLCLI_CAPTURE_PREENTRY].offset);
  c->bind_storage_after_result=slcli_native_clock_read(&c->bind_storage_after);
  if(!c->bind_storage_after_result||c->bind_storage_after.tick<c->bind_storage_before.tick||
    c->bind_storage_after.tick>=c->expires_original)return 0;
  c->bind_result=slcli_node_preentry_windows_bind_storage(c->node,&c->arena,c->deadline,
    c->caller,sizeof(*c->caller),&c->bind_storage_after,backing,expected_backing,bytes,c->local_creation,capacity,thread_capture_attempts,c->local_runtime_request);
  c->bind_returned=1;return c->bind_result;
}
#elif defined(__linux__)
int slcli_node_bootstrap_linux_bind_original(int descriptor,uint64_t bytes,
  const struct stat *expected,size_t capacity,const struct slcli_extent_request *request_original) {
  struct slcli_node_bootstrap *c=slcli_node_bootstrap_image_original();
  size_t i;if(!c||!c->constructed||c->bind_attempted||!input_charged(c,expected,sizeof(*expected))||
    !input_charged(c,request_original,sizeof(*request_original)))return 0;
  c->bind_attempted=1;
  c->local_runtime_request=(struct slcli_extent_request *)slcli_arena_calloc(&c->arena,1,sizeof(*c->local_runtime_request));
  if(!c->local_runtime_request)return 0;
  c->local_runtime_request->index=request_original->index;
  c->local_runtime_request->sequence=request_original->sequence;
  for(i=0;i<sizeof(*expected);i++)((unsigned char *)&c->backing_original)[i]=((const unsigned char *)expected)[i];
  c->local_backing=(struct stat *)slcli_arena_alloc(&c->arena,sizeof(*c->local_backing));
  if(!c->local_backing)return 0;
  for(i=0;i<sizeof(*c->local_backing);i++)((unsigned char *)c->local_backing)[i]=((const unsigned char *)&c->backing_original)[i];
  c->node=(struct slcli_node_preentry_linux *)slcli_arena_calloc(&c->arena,1,sizeof(*c->node));
  if(!c->node)return 0;
  c->bind_result=slcli_node_preentry_linux_bind_storage(c->node,&c->arena,c->deadline,
    c->caller,sizeof(*c->caller),c->local_predecessor,descriptor,bytes,c->local_backing,capacity,c->local_runtime_request);
  c->bind_returned=1;return c->bind_result;
}
#endif
const struct slcli_node_bootstrap_decode *slcli_node_bootstrap_decode_original(
  const unsigned char *raw,size_t bytes) {
  struct slcli_node_bootstrap *c=slcli_node_bootstrap_image_original();
#if defined(_WIN32)
  struct slcli_node_bootstrap_decode *decoded=c?c->transport_original:0;
  /* The actual receiver already decoded once into its paid stage. Never
   * decode again or overwrite that retained native receive frontier. */
  if(!c||!c->storage_bound||!decoded||!image_charged(c)||
     !input_charged(c,raw,bytes)||!input_charged(c,decoded,sizeof(*decoded))||
     !decoded->qualified||decoded->raw_original!=raw||decoded->raw_bytes_original!=bytes)return 0;
  return decoded;
#else
  if(!image_charged(c)||!input_charged(c,raw,bytes)||
     !input_charged(c,&image_transport,sizeof(image_transport)))return 0;
  if(!slcli_node_bootstrap_transport_decode(&image_transport,raw,bytes))return 0;
  return &image_transport;
#endif
}
int slcli_node_bootstrap_from_transport_original(const void *entry) {
  struct slcli_node_bootstrap *c=slcli_node_bootstrap_image_original();
  const struct slcli_node_bootstrap_decode *decoded;
  const struct slcli_node_bootstrap_transport *t;
  size_t i;int result;
#if defined(_WIN32)
  decoded=c&&c->storage_bound?c->transport_original:0;
#else
  decoded=&image_transport;
#endif
  if(!c||!decoded||c->transport_attempted||!decoded->qualified||!entry)return 0;
  t=&decoded->transport;
  c->transport_attempted=1;
  if(c->owner.limit!=t->owner_limit||t->capacity>SIZE_MAX||t->thread_capture_attempts>SIZE_MAX||
     !slcli_memory_owner_prefix_matches(&c->owner,&t->frozen_owner))goto failed;
  /* Equality retains association with the independently initialized original
   * owner; it is not an issuer/native/source admission check or new quota. */
  for(i=0;i<32;i++)if(c->owner.original_birth[i]!=t->associations[0][i]||
    c->owner.original_image[i]!=t->associations[1][i]||
    c->owner.original_owner[i]!=t->associations[2][i]||
    c->owner.original_source[i]!=t->associations[3][i])goto failed;
  result=slcli_node_bootstrap_acquire_original(entry,&t->origin,t->expires,t->bootstrap_bytes);
  c->transport_acquire_result=result;c->transport_acquire_returned=1;
  if(!result)goto failed;
#if defined(_WIN32)
  if(t->capabilities[0]>UINTPTR_MAX||t->capabilities[1]>UINTPTR_MAX)goto failed;
  result=slcli_node_bootstrap_windows_bind_original((HANDLE)(uintptr_t)t->capabilities[0],
    (HANDLE)(uintptr_t)t->capabilities[1],t->runtime_bytes,
    (const FILETIME *)(const void *)t->expected_native,(size_t)t->capacity,(size_t)t->thread_capture_attempts,&t->runtime_request);
  c->transport_bind_result=result;c->transport_bind_returned=1;
  if(!result)goto failed;
  c->transport_identity_result=c->node->child_caller->process_result==t->process_original&&
    c->node->child_caller->thread_result==t->thread_original;
  if(!c->transport_identity_result)goto failed;
#elif defined(__linux__)
  if(t->capabilities[0]>INT32_MAX)goto failed;
  result=slcli_node_bootstrap_linux_bind_original((int)t->capabilities[0],t->runtime_bytes,
    (const struct stat *)(const void *)t->expected_native,(size_t)t->capacity,&t->runtime_request);
  c->transport_bind_result=result;c->transport_bind_returned=1;
  if(!result)goto failed;
  c->transport_identity_result=c->node->child_caller->process_result==t->process_original&&
    c->node->child_caller->thread_result==t->thread_original;
  if(!c->transport_identity_result)goto failed;
#endif
  c->transport_result=1;c->transport_returned=1;return 1;
failed:c->transport_returned=1;return 0;
}
int slcli_node_bootstrap_entry_complete_original(void) {
  const struct slcli_node_bootstrap *c=slcli_node_bootstrap_image_original();
  /* Allocator attachment is an intermediate actual effect. It cannot turn a
   * failed final transport/child-identity qualification into CRT continuation. */
  return c&&c->started&&c->constructed&&!c->owner.failed&&c->acquire_returned&&
    c->acquire_result&&c->postclock_result&&c->arena_returned&&c->arena_result&&
    c->bind_attempted&&c->bind_returned&&c->bind_result&&c->transport_attempted&&
    c->transport_returned&&c->transport_result&&c->transport_acquire_returned&&
    c->transport_acquire_result&&c->transport_bind_returned&&c->transport_bind_result&&
    c->transport_identity_result;
}
