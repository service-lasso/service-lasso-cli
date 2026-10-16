#include "service_channel.h"
#if defined(_WIN32) || defined(__linux__)
#include "source_transition.h"
#endif
#if defined(_WIN32) || defined(__linux__)
struct slcli_service_request *slcli_service_request_allocate(struct slcli_arena *a) {
  struct slcli_service_request *s;
  if(!a)return 0;s=slcli_arena_calloc(a,1,sizeof(*s));if(!s)return 0;
#if defined(__linux__)
  s->original=-1;s->output_original=-1;
#endif
  return s;
}
#if defined(_WIN32) || defined(__linux__)
static struct slcli_source_owner *service_enter(struct slcli_service_request *s,const void *pending) {
  struct slcli_source_owner *owner;
  if(!s||!s->owning_primary||!(owner=slcli_source_enter(s->owning_primary,SLCLI_SOURCE_SERVICE,s,pending)))return 0;
  if(!slcli_arena_live_block(owner->arena,s,sizeof(*s))||
     !slcli_arena_live_range(owner->arena,s->input_file,sizeof(*s->input_file))||
     !slcli_arena_live_range(owner->arena,s->response_file,sizeof(*s->response_file))||
     s->input_file->workspace!=owner->workspace||s->response_file->workspace!=owner->workspace) {
    slcli_source_leave(owner,0);return 0;
  }
  return owner;
}
#endif
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static struct slcli_source_owner *reference_owner(const struct slcli_native_reference *r) {
  const struct slcli_service_request *s;struct slcli_source_owner *o;
  const struct slcli_binding_originals *b;uintptr_t at,first;size_t bytes;
  if(!r||!r->version||!(s=r->service_request_original)||!s->owning_primary)return 0;
#if defined(_WIN32)
  if(!s->owning_primary->capture_store)return 0;
  o=s->owning_primary->capture_store->source_owner;
#else
  o=s->owning_primary->source_linux;
#endif
  if(!o||!o->arena||!slcli_arena_live_block(o->arena,s,sizeof(*s))||
     !(b=s->owning_primary->binding)||!slcli_arena_live_range(o->arena,b,sizeof(*b))||
     !b->references||!b->count||b->count>SIZE_MAX/sizeof(*r))return 0;
  bytes=b->count*sizeof(*r);first=(uintptr_t)b->references;at=(uintptr_t)r;
  if(!slcli_arena_live_range(o->arena,b->references,bytes)||at<first||
     at-first>=bytes||(at-first)%sizeof(*r)||
     !slcli_source_deadline_original_matches(o->arena,s->deadline_original,s))return 0;
  return o;
}
static int reference_fresh(const struct slcli_native_reference *r,unsigned kind) {
  return r&&r->kind==kind&&r->effect_flags==1&&r->version&&!r->handle&&
    !r->native_original&&!r->service_request_original&&
    r->native_class==SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED;
}
static int reference_zero(const void *p,size_t bytes) {
  size_t i;const unsigned char *raw=p;if(!p||!bytes)return 0;
  for(i=0;i<bytes;i++)if(raw[i])return 0;return 1;
}
int slcli_service_request_bind_references_original(struct slcli_service_request *s,
  struct slcli_stream_capture *reads,uint32_t nr,uint32_t first_read,
  struct slcli_channel_write_capture *writes,uint32_t nw,uint32_t first_write,
  uint32_t read_event,uint32_t write_event) {
  struct slcli_source_owner *o;const struct slcli_binding_originals *b;
  uint32_t i;size_t k,rb,wb;
  if(!s||s->reference_bind_attempted||!s->owning_primary)return 0;
#if defined(_WIN32)
  if(!s->owning_primary->capture_store)return 0;o=s->owning_primary->capture_store->source_owner;
#else
  o=s->owning_primary->source_linux;
#endif
  if(!o||!o->arena||!slcli_arena_live_block(o->arena,s,sizeof(*s)))return 0;
  s->reference_bind_attempted=1;
  if(s->started||s->exchange.started||s->failed||!nr||!nw||
     nr>SIZE_MAX/sizeof(*reads)||nw>SIZE_MAX/sizeof(*writes)||
     !(b=s->owning_primary->binding)||!slcli_arena_live_range(o->arena,b,sizeof(*b))||
     !b->references||!b->count||b->count>SIZE_MAX/sizeof(*b->references)||
     !slcli_arena_live_range(o->arena,b->references,b->count*sizeof(*b->references))||
     first_read>=b->count||nr>b->count-first_read||first_write>=b->count||nw>b->count-first_write||
     (first_read<first_write+nw&&first_write<first_read+nr)||
     !slcli_source_deadline_original_matches(o->arena,s->deadline_original,s))return 0;
  rb=nr*sizeof(*reads);wb=nw*sizeof(*writes);
  if(!slcli_arena_live_block(o->arena,reads,rb)||!slcli_arena_live_block(o->arena,writes,wb)||
     !separate(reads,rb,writes,wb)||!separate(reads,rb,s,sizeof(*s))||!separate(writes,wb,s,sizeof(*s))||
     !separate(b->references,b->count*sizeof(*b->references),s,sizeof(*s))||
     !separate(b->references,b->count*sizeof(*b->references),reads,rb)||
     !separate(b->references,b->count*sizeof(*b->references),writes,wb))return 0;
  for(k=0;k<rb;k++)if(((const unsigned char *)reads)[k])return 0;
  for(k=0;k<wb;k++)if(((const unsigned char *)writes)[k])return 0;
  for(i=0;i<nr;i++)if(!reference_fresh(&b->references[first_read+i],5))return 0;
  for(i=0;i<nw;i++)if(!reference_fresh(&b->references[first_write+i],5))return 0;
#if defined(_WIN32)
  if(read_event>=b->count||write_event>=b->count||read_event==write_event||
     (read_event>=first_read&&read_event-first_read<nr)||(read_event>=first_write&&read_event-first_write<nw)||
     (write_event>=first_read&&write_event-first_read<nr)||(write_event>=first_write&&write_event-first_write<nw)||
     !reference_fresh(&b->references[read_event],20)||!reference_fresh(&b->references[write_event],20))return 0;
  for(k=0;k<sizeof(s->read_event);k++)if(((const unsigned char *)&s->read_event)[k])return 0;
  for(k=0;k<sizeof(s->write_event);k++)if(((const unsigned char *)&s->write_event)[k])return 0;
  b->references[read_event].native_original=&s->read_event;
  b->references[read_event].service_request_original=s;
  b->references[read_event].native_class=SLCLI_NATIVE_ORIGINAL_CHANNEL_EVENT;
  b->references[write_event].native_original=&s->write_event;
  b->references[write_event].service_request_original=s;
  b->references[write_event].native_class=SLCLI_NATIVE_ORIGINAL_CHANNEL_EVENT;
#else
  if(read_event!=UINT32_MAX||write_event!=UINT32_MAX)return 0;
#endif
  s->original_read_rows=reads;s->original_read_capacity=nr;
  s->original_write_rows=writes;s->original_write_capacity=nw;
  for(i=0;i<nr;i++) {
    struct slcli_native_reference *r=&b->references[first_read+i];
    r->native_original=&reads[i];r->service_request_original=s;r->native_class=SLCLI_NATIVE_ORIGINAL_CHANNEL_READ;
  }
  for(i=0;i<nw;i++) {
    struct slcli_native_reference *r=&b->references[first_write+i];
    r->native_original=&writes[i];r->service_request_original=s;r->native_class=SLCLI_NATIVE_ORIGINAL_CHANNEL_WRITE;
  }
  s->references_bound=1;return 1;
}
/* The fixed readers inspect actual retained source cells under the owning
 * engine's serialization. They perform no native effect, callback, copied
 * success-receipt admission, or row retirement. Full containing constructor
 * and identity/capture digest production remain separate required work. */
int slcli_native_reference_recheck_original(const struct slcli_native_reference *r,
  const struct slcli_runtime_row_claim *q) {
  const struct slcli_service_request *s;struct slcli_source_owner *o;
  uintptr_t p,first;size_t bytes,index;
  if(!q||!r||!r->native_original||q->kind!=r->kind||!(o=reference_owner(r)))return 0;
  s=r->service_request_original;
  switch(r->native_class) {
#if defined(_WIN32)
  case SLCLI_NATIVE_ORIGINAL_CHANNEL_EVENT: {
    const struct slcli_channel_event_original *e=r->native_original;
    if(r->kind!=20||!s->references_bound||(e!=&s->read_event&&e!=&s->write_event))return 0;
    if(!!(q->effect_flags&2)!=!!e->birth_called||
       !!(q->effect_flags&4)!=!!e->birth_returned||
       !!(q->effect_flags&8)!=!!(e->birth_returned&&e->handle))return 0;
    if(!e->birth_started)return q->effect_flags==1&&r->handle==0&&reference_zero(e,sizeof(*e));
    if(e->reserved_reference!=r||e->reserved_version!=r->version||
       e->deadline!=s->deadline_original||e->origin!=e->deadline->origin||
       e->origin_tick!=e->origin->tick||e->expires!=e->deadline->expires||e->caller!=s)return 0;
    if(e->birth_called&&(e->birth_args[0]||e->birth_args[1]!=TRUE||e->birth_args[2]||e->birth_args[3]))return 0;
    if(r->handle!=(uint64_t)(uintptr_t)e->handle)return 0;
    if(q->state==0)return e->birth_qualified&&!e->close_started;
    if(q->state==1) {
      if(!e->close_called||!e->close_returned||!e->close_status||
         e->close_args[0]!=(uintptr_t)e->handle||s->pending_original||s->pending_write_original||
         !s->complete||!s->delivered||!s->input_file||!s->response_file||
         !s->input_file->sealed||!s->response_file->sealed)return 0;
      return e->close_qualified!=0;
    }
    return q->state==2||q->state==3;
  }
#endif
  case SLCLI_NATIVE_ORIGINAL_CHANNEL_READ: {
    const struct slcli_stream_capture *c=r->native_original;
    if(r->kind!=5||!s->references_bound||!s->original_read_rows||!s->original_read_capacity||
       s->original_read_capacity>SIZE_MAX/sizeof(*c))return 0;
    bytes=s->original_read_capacity*sizeof(*c);first=(uintptr_t)s->original_read_rows;p=(uintptr_t)c;
    if(!slcli_arena_live_block(o->arena,s->original_read_rows,bytes)||p<first||p-first>=bytes||
       (p-first)%sizeof(*c))return 0;
    index=(p-first)/sizeof(*c);if(index>=s->original_read_capacity)return 0;
    if(!c->started)return q->effect_flags==1&&reference_zero(c,sizeof(*c));
    if(c->original!=s->original||c->buffer!=s->read_buffer||!c->requested||c->requested>sizeof(s->read_buffer)||
       (c->count_observed&&c->returned>c->requested))return 0;
    if(q->state==1) {
      unsigned phase=0;
#if defined(_WIN32)
      phase=(c->event_wait.started||c->completion_observed)?1u:0u;
#endif
      return c->complete&&!c->pending&&!c->failed&&c->observed&&c->count_observed&&
        s->pending_original!=c&&c->transfer_returned[phase]&&c->transfer_result[phase]&&
        c->transfer_owner[phase]==s->capture_original&&c->transfer_sink[phase]==s->capture_sink&&
        c->transfer_bytes[phase]==s->read_buffer&&c->transfer_lengths[phase]==c->returned;
    }
    return q->state!=0||(c->pending&&s->pending_original==c);
  }
  case SLCLI_NATIVE_ORIGINAL_CHANNEL_WRITE: {
    const struct slcli_channel_write_capture *c=r->native_original;
    if(r->kind!=5||!s->references_bound||!s->original_write_rows||!s->original_write_capacity||
       s->original_write_capacity>SIZE_MAX/sizeof(*c))return 0;
    bytes=s->original_write_capacity*sizeof(*c);first=(uintptr_t)s->original_write_rows;p=(uintptr_t)c;
    if(!slcli_arena_live_block(o->arena,s->original_write_rows,bytes)||p<first||p-first>=bytes||
       (p-first)%sizeof(*c))return 0;
    if(!c->started)return q->effect_flags==1&&reference_zero(c,sizeof(*c));
    if(c->original!=s->output_original||!c->body||!c->requested||
       (c->count_observed&&c->returned>c->requested))return 0;
    if(q->state==1) {
      unsigned phase=0;
#if defined(_WIN32)
      phase=(c->event_wait.started||c->completion_observed)?1u:0u;
#endif
      return c->complete&&!c->pending&&!c->failed&&c->observed&&c->count_observed&&
        s->pending_write_original!=c&&c->transfer_returned[phase]&&c->transfer_result[phase]&&
        c->transfer_owner[phase]==s->write_original&&c->transfer_sink[phase]==s->write_sink&&
        c->transfer_bytes[phase]==c->body&&c->transfer_lengths[phase]==c->requested;
    }
    return q->state!=0||(c->pending&&s->pending_write_original==c);
  }
  default:return 0;
  }
}
static int capture_reserved(const struct slcli_service_request *s,const void *capture,
  enum slcli_native_original_class native_class) {
  const struct slcli_binding_originals *b;const struct slcli_native_reference *found=0;
  struct slcli_source_owner *o;uint32_t i,count;size_t stride,bytes;const void *rows;
  uintptr_t first,at=(uintptr_t)capture;
  if(!s||!s->references_bound||!s->owning_primary||!(b=s->owning_primary->binding)||
     !b->references||!b->count)return 0;
#if defined(_WIN32)
  if(!s->owning_primary->capture_store)return 0;o=s->owning_primary->capture_store->source_owner;
#else
  o=s->owning_primary->source_linux;
#endif
  if(!o||!o->arena||!slcli_arena_live_range(o->arena,b,sizeof(*b))||
     b->count>SIZE_MAX/sizeof(*b->references)||
     !slcli_arena_live_range(o->arena,b->references,b->count*sizeof(*b->references)))return 0;
  if(native_class==SLCLI_NATIVE_ORIGINAL_CHANNEL_READ) {
    rows=s->original_read_rows;count=s->original_read_capacity;stride=sizeof(struct slcli_stream_capture);
  } else if(native_class==SLCLI_NATIVE_ORIGINAL_CHANNEL_WRITE) {
    rows=s->original_write_rows;count=s->original_write_capacity;stride=sizeof(struct slcli_channel_write_capture);
  } else return 0;
  if(!rows||!count||count>SIZE_MAX/stride)return 0;bytes=count*stride;first=(uintptr_t)rows;
  if(!slcli_arena_live_block(o->arena,rows,bytes)||at<first||at-first>=bytes||(at-first)%stride)return 0;
  for(i=0;i<b->count;i++)if(b->references[i].native_original==capture) {
    if(found)return 0;found=&b->references[i];
  }
  return found&&found->kind==5&&found->version&&found->native_class==native_class&&
    found->service_request_original==s&&reference_owner(found)!=0;
}
#if defined(_WIN32)
static const struct slcli_native_reference *event_reservation(struct slcli_source_owner *o,struct slcli_service_request *s,
  struct slcli_channel_event_original *e) {
  const struct slcli_binding_originals *b=s->owning_primary->binding;uint32_t i;
  const struct slcli_native_reference *found=0;
  if(!s->references_bound||!b||!slcli_arena_live_range(o->arena,b,sizeof(*b))||!b->references||
     !b->count||b->count>128||
     !slcli_arena_live_range(o->arena,b->references,b->count*sizeof(*b->references)))return 0;
  for(i=0;i<b->count;i++)if(b->references[i].native_original==e) {
    if(found)return 0;found=&b->references[i];
  }
  /* This joins an already reserved original table cell. It never reserves
   * an extra event or supplies source identity from a HANDLE/address. */
  if(!found||found->kind!=20||!(found->effect_flags&1)||!found->version||
     found->native_class!=SLCLI_NATIVE_ORIGINAL_CHANNEL_EVENT||found->service_request_original!=s)return 0;
  return found;
}
static int event_tuple(struct slcli_source_owner *o,struct slcli_service_request *s,
  struct slcli_channel_event_original *e) {
  const struct slcli_native_reference *r=event_reservation(o,s,e);
  return r&&r==e->reserved_reference&&r->version==e->reserved_version&&
    slcli_source_deadline_original_matches(o->arena,e->deadline,s)&&
    e->deadline==s->deadline_original&&e->deadline->origin==e->origin&&
    e->origin->tick==e->origin_tick&&e->deadline->expires==e->expires&&e->caller==s;
}
static HANDLE event_acquire(struct slcli_source_owner *o,struct slcli_service_request *s,
  struct slcli_channel_event_original *e) {
  const struct slcli_native_reference *r;const struct slcli_native_clock *prior;size_t i;
  if(e->birth_started)return event_tuple(o,s,e)&&e->birth_qualified&&!e->close_started?e->handle:0;
  for(i=0;i<sizeof(*e);i++)if(((const unsigned char *)e)[i])return 0;
  if(!o->lock_depth||o->lock_depth>3||!o->attempts[o->lock_depth-1]||
     !slcli_source_deadline_original_matches(o->arena,s->deadline_original,s)||
     !(r=event_reservation(o,s,e))||r->handle||(r->effect_flags&~1u))return 0;
  prior=&o->attempts[o->lock_depth-1]->entry;
  e->birth_started=1;e->deadline=s->deadline_original;e->origin=e->deadline->origin;
  e->caller=s;e->origin_tick=e->origin->tick;e->expires=e->deadline->expires;
  e->reserved_reference=r;e->reserved_version=r->version;
  if(!slcli_native_clock_read(&e->birth_before)||e->birth_before.tick<prior->tick||
     e->birth_before.tick>=e->expires||!event_tuple(o,s,e))return 0;
  e->birth_args[0]=0;e->birth_args[1]=TRUE;e->birth_args[2]=FALSE;e->birth_args[3]=0;
  e->birth_called=1;e->handle=CreateEventW(0,TRUE,FALSE,0);e->birth_returned=1;
  if(!e->handle)e->birth_error=GetLastError();
  /* Native output is retained before qualification. A late actual birth is
   * held, never retried or silently replaced with a borrowed event. */
  if(!slcli_native_clock_read(&e->birth_after)||e->birth_after.tick<e->birth_before.tick||
     e->birth_after.tick>=e->expires||!event_tuple(o,s,e))return 0;
  if(e->birth_after.tick>o->observed_clock_frontier)o->observed_clock_frontier=e->birth_after.tick;
  e->birth_qualified=e->handle!=0;return e->birth_qualified?e->handle:0;
}
#endif
static int ready(const struct slcli_service_request *s,const struct slcli_stream_capture *r) {
  return s&&r&&s->references_bound&&!s->failed&&!s->complete&&s->input_file&&s->response_file&&
    capture_reserved(s,r,SLCLI_NATIVE_ORIGINAL_CHANNEL_READ)&&
    s->input_file!=s->response_file&&s->input_file->original!=s->response_file->original&&
    s->input_file->original!=s->original&&s->response_file->original!=s->original&&
    s->input_file->started&&!s->input_file->failed&&!s->input_file->sealed&&
    s->response_file->started&&!s->response_file->failed&&!s->response_file->sealed&&
    s->capture_sink&&s->capture_original&&separate(s,sizeof(*s),r,sizeof(*r))&&
    separate(s,sizeof(*s),s->input_file,sizeof(*s->input_file))&&
    separate(s,sizeof(*s),s->response_file,sizeof(*s->response_file))&&
    separate(r,sizeof(*r),s->input_file,sizeof(*s->input_file))&&
    separate(r,sizeof(*r),s->response_file,sizeof(*s->response_file))&&
    separate(s->input_file,sizeof(*s->input_file),s->response_file,sizeof(*s->response_file));
}
static size_t needed(const struct slcli_service_request *s) {
  if(s->header_bytes<4)return 4-s->header_bytes;
  if(s->body_bytes<s->input.length)return s->input.length-s->body_bytes;
  return 32-s->mac_bytes;
}
static int retain_read(struct slcli_service_request *s,struct slcli_stream_capture *r,size_t n) {
  return slcli_stream_transfer_original(r,s->capture_sink,s->capture_original,s->read_buffer,n);
}
static int retain_write(struct slcli_service_request *s,struct slcli_channel_write_capture *r) {
  unsigned phase=0;
#if defined(_WIN32)
  phase=(r->event_wait.started||r->completion_observed)?1u:0u;
#endif
  if(r->transfer_attempted[phase])return r->transfer_owner[phase]==s->write_original&&
    r->transfer_sink[phase]==s->write_sink&&r->transfer_bytes[phase]==r->body&&
    r->transfer_lengths[phase]==r->requested&&r->transfer_returned[phase]&&r->transfer_result[phase];
  if(!s->write_sink||!s->write_original)return 0;
  r->transfer_owner[phase]=s->write_original;r->transfer_sink[phase]=s->write_sink;
  r->transfer_bytes[phase]=r->body;r->transfer_lengths[phase]=r->requested;
  r->transfer_attempted[phase]=1;
  r->transfer_result[phase]=s->write_sink(s->write_original,r,r->body,r->requested);
  r->transfer_returned[phase]=1;return r->transfer_result[phase];
}
static int accept(struct slcli_service_request *s,struct slcli_stream_capture *r) {
  size_t n;unsigned char *target;uint32_t *count;size_t i;
  if(!ready(s,r)||r->original!=s->original||r->buffer!=s->read_buffer)return 0;
  n=r->count_observed&&r->returned<=r->requested?r->returned:0;
  if(s->pending_original&&s->pending_original!=r)goto failed;
  if(r->pending)s->pending_original=r;
  if(!retain_read(s,r,n))goto failed;
  if(r->pending){s->pending_original=r;if(r->failed)goto failed;return 1;}
  if(s->pending_original&&s->pending_original!=r)goto failed;s->pending_original=0;
  if(!r->complete||r->failed||!r->count_observed||!n||n>needed(s))goto failed;
  /* The actual raw channel bytes reach native retention before assembler reuse
   * or any command effect. A claimed count never supplies bytes here. */
  if(!slcli_evidence_append(s->input_file,s->read_buffer,n))goto failed;
  if(s->header_bytes<4){target=s->header;count=&s->header_bytes;}
  else if(s->body_bytes<s->input.length){target=s->input.body;count=&s->body_bytes;}
  else {target=s->mac;count=&s->mac_bytes;}
  for(i=0;i<n;i++)target[*count+i]=s->read_buffer[i];*count+=(uint32_t)n;
  if(s->header_bytes==4&&!s->input.length) {
    s->input.length=((uint32_t)s->header[0]<<24)|((uint32_t)s->header[1]<<16)|
      ((uint32_t)s->header[2]<<8)|s->header[3];
    if(!s->input.length||s->input.length>SLCLI_FRAME_BYTES)goto failed;
  }
  if(s->mac_bytes==32) {
    if(!slcli_evidence_flush_readback(s->input_file))goto failed;
    s->complete=1;
  }
  return 1;
failed:s->failed=1;return 0;
}
static int request_read(struct slcli_service_request *s,struct slcli_stream_capture *r,
                               slcli_input_handle event) {
  int ok;size_t n;
  if(!ready(s,r)||r->started||s->pending_original)return 0;
  n=needed(s);if(!n||n>sizeof(s->read_buffer))return 0;s->started=1;
#if defined(_WIN32)
  if(s->read_event.birth_qualified&&event==s->read_event.handle) {
    ok=slcli_stream_read_owned_original(r,s->original,s->read_buffer,n,event,s->last_owned_read);
    if(r->started)s->last_owned_read=r;
  } else
#endif
#if defined(__linux__)
  {
    struct slcli_source_linux *o=s->owning_primary->source_linux;
    struct slcli_channel_ready_inputs in;
    if(!o||!o->lock_depth||o->lock_depth>3||!o->attempts[o->lock_depth-1])return 0;
    in.arena=o->arena;in.deadline=s->deadline_original;in.caller=s;in.caller_bytes=sizeof(*s);
    in.predecessor=&o->attempts[o->lock_depth-1]->entry;
    ok=slcli_stream_read_ready_original(r,s->original,s->read_buffer,n,&in);
    if(r->readiness.after.complete&&r->readiness.after.tick>o->observed_clock_frontier)
      o->observed_clock_frontier=r->readiness.after.tick;
    if(r->clock.complete&&r->clock.tick>o->observed_clock_frontier)o->observed_clock_frontier=r->clock.tick;
    (void)event;
  }
#else
  ok=slcli_stream_read_original(r,s->original,s->read_buffer,n,event);
#endif
  if(!r->started){s->failed=1;return 0;}
  if(!accept(s,r))return 0;return ok;
}
int slcli_service_request_read(struct slcli_service_request *s,struct slcli_stream_capture *r,
  slcli_input_handle event) {
  int ok;
#if defined(_WIN32) || defined(__linux__)
  struct slcli_source_owner *owner=service_enter(s,0);if(!owner)return 0;
  if(!slcli_arena_live_range(owner->arena,r,sizeof(*r))) {
    slcli_source_leave(owner,0);return 0;
  }
#endif
#if defined(_WIN32)
  if(!event||(s->read_event.birth_started&&event==s->read_event.handle)) {
    const struct slcli_stream_capture *last=s->last_owned_read;unsigned phase;
    if(!ready(s,r)||r->started||s->pending_original)goto read_done;
    if(last) {
      if(!slcli_arena_live_range(owner->arena,last,sizeof(*last)))goto read_done;
      phase=(last->event_wait.started||last->completion_observed)?1u:0u;
      if(last->pending||
         !last->complete||last->failed||!last->transfer_returned[phase]||
         !last->transfer_result[phase]||last->overlapped.hEvent!=s->read_event.handle)goto read_done;
    }
    event=event_acquire(owner,s,&s->read_event);if(!event)goto read_done;
    ok=request_read(s,r,event);
  } else
#endif
  ok=request_read(s,r,event);
#if defined(_WIN32)
  goto read_publish;
read_done:ok=0;s->failed=1;
read_publish:
#endif
#if defined(_WIN32) || defined(__linux__)
  if(!slcli_source_pending(owner,r->pending?r:0))ok=0;
  ok=slcli_source_leave(owner,ok);
#endif
  return ok;
}
#if defined(_WIN32)
static int wait_read_completion(struct slcli_source_owner *owner,struct slcli_service_request *s,
  struct slcli_stream_capture *r) {
  int waited,completed=0;const struct slcli_native_clock *prior;
  if(!owner->lock_depth||owner->lock_depth>3||!owner->attempts[owner->lock_depth-1])return 0;
  prior=&owner->attempts[owner->lock_depth-1]->entry;
  waited=slcli_channel_event_wait_original(&r->event_wait,owner->arena,s->deadline_original,
    s,sizeof(*s),prior,r->overlapped.hEvent);
  if(r->event_wait.returned&&r->event_wait.status==WAIT_OBJECT_0)
    completed=slcli_stream_read_complete(r);
  if(r->event_wait.after.complete&&r->event_wait.after.tick>owner->observed_clock_frontier)
    owner->observed_clock_frontier=r->event_wait.after.tick;
  if(r->completion_clock.complete&&r->completion_clock.tick>owner->observed_clock_frontier)
    owner->observed_clock_frontier=r->completion_clock.tick;
  if(!waited||!completed){r->failed=1;return 0;}return 1;
}
static int wait_write_completion(struct slcli_source_owner *owner,struct slcli_service_request *s,
  struct slcli_channel_write_capture *r) {
  int waited,completed=0;const struct slcli_native_clock *prior;
  if(!owner->lock_depth||owner->lock_depth>3||!owner->attempts[owner->lock_depth-1])return 0;
  prior=&owner->attempts[owner->lock_depth-1]->entry;
  waited=slcli_channel_event_wait_original(&r->event_wait,owner->arena,s->deadline_original,
    s,sizeof(*s),prior,r->overlapped.hEvent);
  if(r->event_wait.returned&&r->event_wait.status==WAIT_OBJECT_0)
    completed=slcli_channel_write_complete(r);
  if(r->event_wait.after.complete&&r->event_wait.after.tick>owner->observed_clock_frontier)
    owner->observed_clock_frontier=r->event_wait.after.tick;
  if(r->completion_clock.complete&&r->completion_clock.tick>owner->observed_clock_frontier)
    owner->observed_clock_frontier=r->completion_clock.tick;
  if(!waited||!completed){r->failed=1;return 0;}return 1;
}
int slcli_service_request_complete(struct slcli_service_request *s,struct slcli_stream_capture *r) {
  int ok=0,valid=0;size_t n;struct slcli_source_owner *owner=service_enter(s,r);
  if(!owner) {
    if(!s||!s->owning_primary||!s->owning_primary->capture_store||
       !(owner=s->owning_primary->capture_store->source_owner)||
       !slcli_source_windows_pending_custody(owner,s,r)||
       !slcli_arena_live_range(owner->arena,s,sizeof(*s))||
       !slcli_arena_live_range(owner->arena,r,sizeof(*r))||s->pending_original!=r||
       !r->pending||r->original!=s->original||r->buffer!=s->read_buffer)return 0;
    wait_read_completion(owner,s,r);
    n=r->count_observed&&r->returned<=r->requested?r->returned:0;
    if(s->capture_sink&&s->capture_original)
      retain_read(s,r,n);
    return 0; /* Original custody only: unresolved owner, pending and charges remain. */
  }
  if(!slcli_arena_live_range(owner->arena,r,sizeof(*r))||
     s->pending_original!=r||!r->pending||r->original!=s->original||
     r->buffer!=s->read_buffer)goto done;
  valid=1;ok=wait_read_completion(owner,s,r);
  if(s->failed||owner->failed) {
    /* Even an already failed request must retain genuine completion of its
     * already submitted native IO. This neither resumes dispatch nor reuse. */
    n=r->count_observed&&r->returned<=r->requested?r->returned:0;
    if(s->capture_sink&&s->capture_original)
      retain_read(s,r,n);
    ok=0;
  } else if(!accept(s,r))ok=0;
done:
  if(valid&&!slcli_source_pending(owner,r->pending?r:0))ok=0;
  ok=slcli_source_leave(owner,ok);return ok;
}
#endif
static int request_dispatch(struct slcli_service_request *s,struct slcli_primary_engine *e,
                                   struct slcli_native_clock *clock,
                                   const struct slcli_primary_commit_originals *o) {
  uint32_t sequence;
  if(!s||!e||!clock||!o||s->failed||!s->complete||s->dispatched||s->pending_original||
     !s->input_file||!s->input_file->sealed||!s->response_file||
     !separate(s,sizeof(*s),e,sizeof(*e))||!e->ledger||
     !separate(s,sizeof(*s),e->ledger,sizeof(*e->ledger))||
     !separate(s,sizeof(*s),clock,sizeof(*clock)))return 0;
  s->dispatched=1;sequence=e->next;
  if(!slcli_primary_engine_dispatch(e,&s->input,s->mac,clock,o,&s->response)||
     !slcli_frame_mac(SLCLI_HOST,1,e->nonce,e->capability,sequence,&s->response,s->response_mac))goto failed;
  slcli_put_u32(s->response_header,s->response.length);
  if(!slcli_evidence_append(s->response_file,s->response_header,4)||
     !slcli_evidence_append(s->response_file,s->response.body,s->response.length)||
     !slcli_evidence_append(s->response_file,s->response_mac,32)||
     !slcli_evidence_flush_readback(s->response_file))goto failed;
  s->response_retained=1;return 1;
failed:s->failed=1;e->failed=1;return 0;
}
int slcli_service_request_dispatch(struct slcli_service_request *s,struct slcli_primary_engine *e,
  struct slcli_native_clock *clock,const struct slcli_primary_commit_originals *o) {
  int ok;
#if defined(_WIN32) || defined(__linux__)
  struct slcli_source_owner *owner=service_enter(s,0);if(!owner)return 0;
  /* SAME original marker excludes conflicting operations through sequence
   * capture, nested commit and response retention. SRW is already released;
   * no native IO or readback executes while holding the transition lock. */
  if(s->owning_primary!=e||
     !slcli_arena_live_range(owner->arena,clock,sizeof(*clock))||
     !slcli_arena_live_range(owner->arena,o,sizeof(*o))) {
    slcli_source_leave(owner,0);return 0;
  }
#endif
  ok=request_dispatch(s,e,clock,o);
#if defined(_WIN32) || defined(__linux__)
  ok=slcli_source_leave(owner,ok);
#endif
  return ok;
}
static int output_ready(const struct slcli_service_request *s,const struct slcli_channel_write_capture *r) {
  return s&&r&&s->references_bound&&!s->failed&&!s->delivered&&s->response_retained&&s->write_sink&&s->write_original&&
    capture_reserved(s,r,SLCLI_NATIVE_ORIGINAL_CHANNEL_WRITE)&&
    s->write_phase<3&&separate(s,sizeof(*s),r,sizeof(*r));
}
static const unsigned char *output_part(const struct slcli_service_request *s,uint32_t *bytes) {
  if(!s->write_phase){*bytes=4;return s->response_header;}
  if(s->write_phase==1){*bytes=s->response.length;return s->response.body;}
  *bytes=32;return s->response_mac;
}
static int output_accept(struct slcli_service_request *s,struct slcli_channel_write_capture *r) {
  uint32_t bytes;const unsigned char *body=output_part(s,&bytes);
  if(s->write_offset>=bytes||r->original!=s->output_original||
     r->body!=body+s->write_offset||r->requested!=bytes-s->write_offset)goto failed;
  if(s->pending_write_original&&s->pending_write_original!=r)goto failed;
  if(r->pending)s->pending_write_original=r;
  /* Retain failed and short native write captures before a fresh attempt. */
  if(!retain_write(s,r))goto failed;
  if(r->pending){s->pending_write_original=r;if(r->failed)goto failed;return 1;}
  if(s->pending_write_original&&s->pending_write_original!=r)goto failed;s->pending_write_original=0;
  if(!r->complete||r->failed||!r->count_observed||!r->returned||r->returned>r->requested)goto failed;
  s->write_offset+=(uint32_t)r->returned;
  if(s->write_offset==bytes){s->write_offset=0;s->write_phase++;if(s->write_phase==3)s->delivered=1;}
  return 1;
failed:s->failed=1;return 0;
}
static int response_write(struct slcli_service_request *s,struct slcli_channel_write_capture *r,
                                  slcli_input_handle event) {
  uint32_t bytes;const unsigned char *body;int ok;
  if(!output_ready(s,r)||r->started||s->pending_write_original)return 0;
  body=output_part(s,&bytes);if(s->write_offset>=bytes)return 0;
#if defined(_WIN32)
  if(s->write_event.birth_qualified&&event==s->write_event.handle) {
    ok=slcli_channel_write_owned_original(r,s->output_original,body+s->write_offset,
      bytes-s->write_offset,event,s->last_owned_write);
    if(r->started)s->last_owned_write=r;
  } else
#endif
#if defined(__linux__)
  {
    struct slcli_source_linux *o=s->owning_primary->source_linux;
    struct slcli_channel_ready_inputs in;
    if(!o||!o->lock_depth||o->lock_depth>3||!o->attempts[o->lock_depth-1])return 0;
    in.arena=o->arena;in.deadline=s->deadline_original;in.caller=s;in.caller_bytes=sizeof(*s);
    in.predecessor=&o->attempts[o->lock_depth-1]->entry;
    ok=slcli_channel_write_ready_original(r,s->output_original,body+s->write_offset,bytes-s->write_offset,&in);
    if(r->readiness.after.complete&&r->readiness.after.tick>o->observed_clock_frontier)
      o->observed_clock_frontier=r->readiness.after.tick;
    if(r->clock.complete&&r->clock.tick>o->observed_clock_frontier)o->observed_clock_frontier=r->clock.tick;
    (void)event;
  }
#else
  ok=slcli_channel_write_original(r,s->output_original,body+s->write_offset,bytes-s->write_offset,event);
#endif
  if(!r->started){s->failed=1;return 0;}if(!output_accept(s,r))return 0;return ok;
}
int slcli_service_response_write(struct slcli_service_request *s,struct slcli_channel_write_capture *r,
  slcli_input_handle event) {
  int ok;
#if defined(_WIN32) || defined(__linux__)
  struct slcli_source_owner *owner=service_enter(s,0);if(!owner)return 0;
  if(!slcli_arena_live_range(owner->arena,r,sizeof(*r))) {
    slcli_source_leave(owner,0);return 0;
  }
#endif
#if defined(_WIN32)
  if(!event||(s->write_event.birth_started&&event==s->write_event.handle)) {
    const struct slcli_channel_write_capture *last=s->last_owned_write;unsigned phase;
    if(!output_ready(s,r)||r->started||s->pending_write_original)goto write_done;
    if(last) {
      if(!slcli_arena_live_range(owner->arena,last,sizeof(*last)))goto write_done;
      phase=(last->event_wait.started||last->completion_observed)?1u:0u;
      if(last->pending||
         !last->complete||last->failed||!last->transfer_returned[phase]||
         !last->transfer_result[phase]||last->overlapped.hEvent!=s->write_event.handle)goto write_done;
    }
    event=event_acquire(owner,s,&s->write_event);if(!event)goto write_done;
    ok=response_write(s,r,event);
  } else
#endif
  ok=response_write(s,r,event);
#if defined(_WIN32)
  goto write_publish;
write_done:ok=0;s->failed=1;
write_publish:
#endif
#if defined(_WIN32) || defined(__linux__)
  if(!slcli_source_pending(owner,r->pending?r:0))ok=0;
  ok=slcli_source_leave(owner,ok);
#endif
  return ok;
}
#if defined(_WIN32)
static int close_event(struct slcli_source_owner *o,struct slcli_service_request *s,
  struct slcli_channel_event_original *e) {
  uint64_t prior=o->attempts[o->lock_depth-1]->entry.tick;
  if(!e->birth_started)return 1;
  if(e->close_started||!e->birth_qualified||!event_tuple(o,s,e))return 0;
  e->close_started=1;
  if(o->observed_clock_frontier>prior)prior=o->observed_clock_frontier;
  if(!slcli_native_clock_read(&e->close_before)||e->close_before.tick<prior||
     e->close_before.tick>=e->expires||!event_tuple(o,s,e))return 0;
  e->close_args[0]=(uintptr_t)e->handle;e->close_called=1;
  e->close_status=CloseHandle(e->handle);e->close_returned=1;
  if(!e->close_status)e->close_error=GetLastError();
  /* Preserve the original handle and actual return even on late failure.
   * Successful disposal is not table/extent/archive retirement or FREE. */
  if(!slcli_native_clock_read(&e->close_after)||e->close_after.tick<e->close_before.tick||
     e->close_after.tick>=e->expires||!event_tuple(o,s,e))return 0;
  if(e->close_after.tick>o->observed_clock_frontier)o->observed_clock_frontier=e->close_after.tick;
  e->close_qualified=e->close_status!=0;return e->close_qualified;
}
int slcli_service_request_close_events(struct slcli_service_request *s) {
  struct slcli_source_owner *o=service_enter(s,0);int read_ok,write_ok,ok;
  if(!o)return 0;
  if(!o->lock_depth||o->lock_depth>3||!o->attempts[o->lock_depth-1]||
     !s->complete||!s->delivered||s->pending_original||s->pending_write_original||
     !s->input_file->sealed||!s->response_file->sealed)goto denied;
  if((s->last_owned_read&&(!slcli_arena_live_range(o->arena,s->last_owned_read,sizeof(*s->last_owned_read))||
       !s->last_owned_read->complete||s->last_owned_read->pending))||
     (s->last_owned_write&&(!slcli_arena_live_range(o->arena,s->last_owned_write,sizeof(*s->last_owned_write))||
       !s->last_owned_write->complete||s->last_owned_write->pending)))goto denied;
  /* Actual independent native readbacks precede disposition, rather than a
   * successful archive callback being treated as durable custody proof. */
  if(s->exchange.close_readback_attempted[0]||s->exchange.close_readback_attempted[1])goto denied;
  s->exchange.close_readback_attempted[0]=1;
  read_ok=slcli_evidence_recheck_readback(s->input_file);
  s->exchange.close_readback_result[0]=read_ok;s->exchange.close_readback_returned[0]=1;
  s->exchange.close_readback_attempted[1]=1;
  write_ok=slcli_evidence_recheck_readback(s->response_file);
  s->exchange.close_readback_result[1]=write_ok;s->exchange.close_readback_returned[1]=1;
  if(!read_ok||!write_ok)goto denied;
  read_ok=close_event(o,s,&s->read_event);
  write_ok=close_event(o,s,&s->write_event);
  ok=read_ok&&write_ok;return slcli_source_leave(o,ok);
denied:s->failed=1;return slcli_source_leave(o,0);
}
int slcli_service_response_complete(struct slcli_service_request *s,struct slcli_channel_write_capture *r) {
  int ok=0,valid=0;struct slcli_source_owner *owner=service_enter(s,r);
  if(!owner) {
    if(!s||!s->owning_primary||!s->owning_primary->capture_store||
       !(owner=s->owning_primary->capture_store->source_owner)||
       !slcli_source_windows_pending_custody(owner,s,r)||
       !slcli_arena_live_range(owner->arena,s,sizeof(*s))||
       !slcli_arena_live_range(owner->arena,r,sizeof(*r))||s->pending_write_original!=r||
       !r->pending||r->original!=s->output_original)return 0;
    wait_write_completion(owner,s,r);
    if(s->write_sink&&s->write_original)
      retain_write(s,r);
    return 0;
  }
  if(!slcli_arena_live_range(owner->arena,r,sizeof(*r))||
     s->pending_write_original!=r||!r->pending||r->original!=s->output_original)goto done;
  valid=1;ok=wait_write_completion(owner,s,r);
  if(s->failed||owner->failed) {
    if(s->write_sink&&s->write_original)
      retain_write(s,r);
    ok=0;
  } else if(!output_accept(s,r))ok=0;
done:
  if(valid&&!slcli_source_pending(owner,r->pending?r:0))ok=0;
  ok=slcli_source_leave(owner,ok);return ok;
}
#endif
static int exchange_rows(struct slcli_source_owner *o,struct slcli_service_request *s,
  const void *rows,uint32_t count,size_t stride,enum slcli_native_original_class native_class) {
  const struct slcli_binding_originals *b=s->owning_primary->binding;
  uint32_t i,j;size_t k;const unsigned char *raw=rows;
  if(!rows||!count||count>128||count>SIZE_MAX/stride||
     !slcli_arena_live_block(o->arena,rows,count*stride)||
     !separate(rows,count*stride,s,sizeof(*s))||
     !separate(rows,count*stride,o->workspace,sizeof(*o->workspace))||!b||
     !slcli_arena_live_range(o->arena,b,sizeof(*b))||!b->references||
     !b->count||b->count>128||!slcli_arena_live_range(o->arena,b->references,b->count*sizeof(*b->references)))return 0;
  for(k=0;k<count*stride;k++)if(raw[k])return 0;
  for(i=0;i<count;i++) {
    const struct slcli_native_reference *found=0;
    for(j=0;j<b->count;j++)if(b->references[j].native_original==raw+i*stride) {
      if(found)return 0;found=&b->references[j];
    }
    if(!found||found->kind!=5||found->effect_flags!=1||!found->version||
       found->native_class!=native_class||found->service_request_original!=s)return 0;
  }
  return 1;
}
int slcli_service_exchange_original(struct slcli_service_request *s,
  struct slcli_stream_capture *reads,uint32_t nr,struct slcli_channel_write_capture *writes,uint32_t nw,
  struct slcli_native_clock *clock,const struct slcli_primary_commit_originals *commit) {
  struct slcli_source_owner *o=service_enter(s,0);struct slcli_service_exchange *x;
  uint32_t i;int ok;size_t k,read_bytes,write_bytes,direct_bytes;
  if(!o)return 0;x=&s->exchange;
  for(k=0;k<sizeof(*x);k++)if(((const unsigned char *)x)[k])goto occupied_failed;
  /* Claim the fresh result owner before validation. Invalid original arguments
   * and their failed publication remain retained, not made into a fresh retry. */
  x->started=1;x->read_rows=reads;x->write_rows=writes;x->read_capacity=nr;x->write_capacity=nw;
  if(!s->references_bound||reads!=s->original_read_rows||writes!=s->original_write_rows||
     nr!=s->original_read_capacity||nw!=s->original_write_capacity||nr>128||nw>128||nr>128-nw||
     !exchange_rows(o,s,reads,nr,sizeof(*reads),SLCLI_NATIVE_ORIGINAL_CHANNEL_READ)||
     !exchange_rows(o,s,writes,nw,sizeof(*writes),SLCLI_NATIVE_ORIGINAL_CHANNEL_WRITE)||
     !separate(reads,nr*sizeof(*reads),writes,nw*sizeof(*writes))||s->started||s->failed||
     !slcli_arena_live_range(o->arena,clock,sizeof(*clock))||
     !slcli_arena_live_range(o->arena,commit,sizeof(*commit))||
     !separate(reads,nr*sizeof(*reads),clock,sizeof(*clock))||
     !separate(writes,nw*sizeof(*writes),clock,sizeof(*clock))||
     !separate(reads,nr*sizeof(*reads),commit,sizeof(*commit))||
     !separate(writes,nw*sizeof(*writes),commit,sizeof(*commit)))goto initial_failed;
  read_bytes=nr*sizeof(*reads);write_bytes=nw*sizeof(*writes);
  if(read_bytes>SIZE_MAX-sizeof(*s)||write_bytes>SIZE_MAX-sizeof(*s)-read_bytes)goto initial_failed;
  direct_bytes=sizeof(*s)+read_bytes+write_bytes;
  if(!o->arena->owner||direct_bytes>o->arena->owner->limit)goto initial_failed;
  x->request_storage_bytes=sizeof(*s);x->read_storage_bytes=read_bytes;
  x->write_storage_bytes=write_bytes;x->direct_storage_bytes=direct_bytes;
  x->preflight_result=1;x->preflight_returned=1;
  ok=slcli_source_continue(o,s,1);
  x->entry_continue_result=ok;x->entry_continue_returned=1;
  x->entry_publication_result=slcli_source_leave(o,ok);x->entry_publication_returned=1;
  if(!ok||!x->entry_publication_result)goto failed;
  while(!s->complete) {
    if(x->reads>=x->read_capacity)goto failed;i=x->reads++;
    x->read_result[i]=slcli_service_request_read(s,&reads[i],0);x->read_returned[i]=1;
#if defined(_WIN32)
    if(reads[i].pending) {
      x->read_completion_result[i]=slcli_service_request_complete(s,&reads[i]);
      x->read_completion_returned[i]=1;
      if(!x->read_completion_result[i])goto failed;
    }
#endif
    if(!x->read_result[i]||s->failed)goto failed;
  }
  x->dispatch_attempted=1;
  x->dispatch_result=slcli_service_request_dispatch(s,s->owning_primary,clock,commit);x->dispatch_returned=1;
  if(!x->dispatch_result)goto failed;
  while(!s->delivered) {
    if(x->writes>=x->write_capacity)goto failed;i=x->writes++;
    x->write_result[i]=slcli_service_response_write(s,&writes[i],0);x->write_returned[i]=1;
#if defined(_WIN32)
    if(writes[i].pending) {
      x->write_completion_result[i]=slcli_service_response_complete(s,&writes[i]);
      x->write_completion_returned[i]=1;
      if(!x->write_completion_result[i])goto failed;
    }
#endif
    if(!x->write_result[i]||s->failed)goto failed;
  }
#if defined(_WIN32)
  x->close_attempted=1;x->close_result=slcli_service_request_close_events(s);x->close_returned=1;
  if(!x->close_result)goto failed;
#endif
  o=service_enter(s,0);if(!o)goto failed;
  ok=slcli_source_continue(o,s,0);x->exit_continue_result=ok;x->exit_continue_returned=1;
  x->publication_result=slcli_source_leave(o,ok);x->publication_returned=1;
  if(!x->publication_result)goto failed;x->completed=1;return 1;
initial_failed:
  x->preflight_result=0;x->preflight_returned=1;
  x->preflight_publication_result=slcli_source_leave(o,0);x->preflight_publication_returned=1;
failed:s->failed=1;x->failed=1;return 0;
occupied_failed:
  /* The newly retained source attempt owns this denial's body/publication
   * results. Never overwrite the earlier exchange's original result cells. */
  slcli_source_leave(o,0);s->failed=1;return 0;
}
#endif
