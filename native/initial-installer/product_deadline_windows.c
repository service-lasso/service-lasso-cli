#include "product_deadline_windows.h"
#if defined(_WIN32)
static const unsigned char tag[8]={'S','L','P','D','L','N','1',0};
static int apart(const void *x,size_t n,const void *y,size_t m) {
 uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
 return x&&y&&n&&m&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&(a+n<=b||b+m<=a);
}
static uint32_t u32(const unsigned char *p) {
 return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];
}
static uint64_t u64(const unsigned char *p) {return ((uint64_t)u32(p)<<32)|u32(p+4);}
static void p32(unsigned char *p,uint32_t v) {
 p[0]=(unsigned char)(v>>24);p[1]=(unsigned char)(v>>16);
 p[2]=(unsigned char)(v>>8);p[3]=(unsigned char)v;
}
static void p64(unsigned char *p,uint64_t v) {p32(p,(uint32_t)(v>>32));p32(p+4,(uint32_t)v);}
static int values(const struct slcli_product_windows_deadline_values *v) {
 return v&&v->enclosing_expires&&v->active_phase_expires&&
 v->active_phase_expires<=v->enclosing_expires&&v->active_phase>=1&&v->active_phase<=4&&
 v->parent_pid&&(v->parent_creation[0]||v->parent_creation[1])&&
 v->qpc_frequency&&v->qpc_frequency<=INT64_MAX;
}
int slcli_product_windows_deadline_encode(unsigned char *out,size_t bytes,
 const struct slcli_product_windows_deadline_values *v) {
 size_t i;
 if(bytes!=SLCLI_PRODUCT_DEADLINE_BYTES||!apart(out,bytes,v,sizeof(*v))||!values(v))return 0;
 /* Refusal above is zero-write. Every successful byte, including reserved,
  * is assigned from the fixed source grammar; no host struct is copied. */
 for(i=0;i<8;i++)out[i]=tag[i];p32(out+8,1);p32(out+12,SLCLI_PRODUCT_DEADLINE_BYTES);
 p64(out+16,v->enclosing_expires);p64(out+24,v->active_phase_expires);
 p32(out+32,v->active_phase);p32(out+36,0);p32(out+40,v->parent_pid);
 p32(out+44,v->parent_creation[0]);p32(out+48,v->parent_creation[1]);p32(out+52,0);
 p64(out+56,v->qpc_frequency);return 1;
}
int slcli_product_windows_deadline_decode(struct slcli_product_windows_deadline_values *out,
 const unsigned char *raw,size_t bytes) {
 struct slcli_product_windows_deadline_values v;size_t i;
 if(bytes!=SLCLI_PRODUCT_DEADLINE_BYTES||!apart(out,sizeof(*out),raw,bytes))return 0;
 for(i=0;i<8;i++)if(raw[i]!=tag[i])return 0;
 if(u32(raw+8)!=1||u32(raw+12)!=bytes||u32(raw+36)||u32(raw+52))return 0;
 v.enclosing_expires=u64(raw+16);v.active_phase_expires=u64(raw+24);
 v.active_phase=u32(raw+32);v.parent_pid=u32(raw+40);
 v.parent_creation[0]=u32(raw+44);v.parent_creation[1]=u32(raw+48);
 v.qpc_frequency=u64(raw+56);if(!values(&v))return 0;
 /* Field publication only after the complete fixed body validates. These
  * scalars remain untrusted routing constraints until native acquisition. */
 out->enclosing_expires=v.enclosing_expires;out->active_phase_expires=v.active_phase_expires;
 out->active_phase=v.active_phase;out->parent_pid=v.parent_pid;
 out->parent_creation[0]=v.parent_creation[0];out->parent_creation[1]=v.parent_creation[1];
 out->qpc_frequency=v.qpc_frequency;return 1;
}
static int publication_same(const struct slcli_product_windows_deadline_publication *p) {
 struct slcli_product_storage_windows *s=p->storage_original;
 const struct slcli_node_transport_windows *t=s?s->transport_original:0;
 return s&&!s->failed&&s->storage_constructed==1&&s->deadline_publication_original==p&&
 slcli_arena_live_block(&s->arena_original,p,sizeof(*p))&&
 slcli_source_deadline_original_matches(&s->arena_original,s->deadline_original,s->caller_original)&&
 slcli_source_deadline_original_matches(&s->arena_original,p->phase_original,s->caller_original)&&
 s->deadline_original->origin==s->operation_origin_original&&
 slcli_native_clock_windows_matches_original(s->operation_origin_original)&&
 p->phase_original->origin==p->phase_origin_original&&
 p->phase_origin_original->tick==p->phase_origin_tick&&
 slcli_native_clock_windows_matches_original(p->phase_origin_original)&&
 p->source_original->active_phase_original==p->phase_original&&p->source_original->active_phase==p->phase&&
 p->values.active_phase==p->phase&&p->values.active_phase_expires==p->phase_original->expires&&
 p->values.enclosing_expires==s->deadline_original->expires&&
 p->values.qpc_frequency==(uint64_t)s->operation_origin_original->native_windows_frequency.QuadPart&&
 t&&t->deadline==s->deadline_original&&t->origin==s->operation_origin_original&&
 t->origin_tick==s->operation_origin_original->tick&&t->expires==p->values.enclosing_expires&&
 t->parent_pid==p->values.parent_pid&&t->parent_creation.dwLowDateTime==p->values.parent_creation[0]&&
 t->parent_creation.dwHighDateTime==p->values.parent_creation[1]&&values(&p->values);
}
static int publication_clock(struct slcli_product_windows_deadline_publication *p,
 struct slcli_native_clock *out,uint64_t prior) {
 return slcli_native_clock_read(out)&&publication_same(p)&&out->tick>=prior&&
 out->tick<p->values.active_phase_expires;
}
static int publication_stop(struct slcli_product_windows_deadline_publication *p) {
 struct slcli_product_storage_windows *s=p->storage_original;
 struct slcli_node_transport_windows *t=s->transport_original;
 p->failed=1;
 /* Known separate writer references may retire after a useful deadline fails;
  * the existing helper keeps unknown/pending calls and never retries a row. */
 t->terminal=1;(void)slcli_node_transport_windows_dispose_writer(t,&s->arena_original);
 s->failed=1;return 0;
}
static int publication_outputs_apart(struct slcli_product_storage_windows *s,
 const struct slcli_product_windows_deadline_source *source,const void *out,size_t n) {
 struct slcli_product_section_windows *sec=s->section_original;
 return slcli_product_storage_windows_output_away_original(s,out,n)&&
 apart(out,n,source,sizeof(*source))&&
 slcli_source_deadline_away(source->active_phase_original,out,n)&&
 apart(out,n,sec,sizeof(*sec))&&apart(out,n,sec->acl_original,sec->acl_bytes)&&
 apart(out,n,sec->relative_original,sec->relative_capacity);
}
int slcli_product_windows_deadline_publish_original(struct slcli_product_storage_windows *s,
 const struct slcli_product_windows_deadline_source *source) {
 struct slcli_product_windows_deadline_publication *p;
 struct slcli_node_transport_windows *t;struct slcli_arena *a;size_t i;
 if(!s||s->failed||s->storage_constructed!=1||s->deadline_publication_original||
 s->deadline_publication_entry_original.started||!source||
 !slcli_arena_live_block(&s->arena_original,source,sizeof(*source))||
 source->active_phase<1||source->active_phase>4||
 !s->section_original||!s->section_original->descriptor_constructed||!s->section_original->transport_created||
 !slcli_memory_owner_mutable(&s->memory_original)||s->arena_original.owner!=&s->memory_original||
 !slcli_source_deadline_original_matches(&s->arena_original,s->deadline_original,s->caller_original)||
 !slcli_source_deadline_original_matches(&s->arena_original,source->active_phase_original,s->caller_original)||
 source->active_phase_original->expires>s->deadline_original->expires||
 !slcli_native_clock_windows_matches_original(s->operation_origin_original)||
 !slcli_native_clock_windows_matches_original(source->active_phase_original->origin)||
 source->active_phase_original->origin->native_windows_frequency.QuadPart!=
 s->operation_origin_original->native_windows_frequency.QuadPart||
 !apart(s,sizeof(*s),source,sizeof(*source))||
 !slcli_source_deadline_away(source->active_phase_original,s,sizeof(*s)))return 0;
 a=&s->arena_original;t=s->transport_original;
 if(!t||!slcli_arena_live_block(a,t,sizeof(*t))||!t->object_prepared||t->terminal||
 t->publication_attempted||t->transport_original||t->writer_disposal_attempted||
 !t->parent_token_closed||t->parent_token_disposal_pending||
 t->deadline!=s->deadline_original||t->origin!=s->operation_origin_original||
 t->expires!=s->deadline_original->expires||t->origin_tick!=s->operation_origin_original->tick||
 !t->calls[1].returned||!t->calls[1].result||t->calls[1].result!=(uintptr_t)t->writable||
 !t->calls[2].returned||!t->calls[2].result||t->calls[2].result!=(uintptr_t)t->writer_view||
 !t->calls[4].returned||!t->calls[4].result||t->readonly!=t->readonly_raw||
 !slcli_arena_live_block(a,t->encoded,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
 !slcli_arena_live_block(a,t->readback,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
 !publication_outputs_apart(s,source,t->encoded,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
 !publication_outputs_apart(s,source,t->readback,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
 !apart(t->encoded,SLCLI_BOOTSTRAP_TRANSPORT_BYTES,t->readback,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
 !slcli_arena_allocation_bytes(sizeof(*p)))return 0;
 if(!slcli_native_clock_read(&s->deadline_publication_entry_original)||
 s->deadline_publication_entry_original.tick<s->prior_tick||
 s->deadline_publication_entry_original.tick>=source->active_phase_original->expires){s->failed=1;return 0;}
 p=slcli_arena_alloc(a,sizeof(*p));
 if(!p){s->failed=1;return 0;}
 /* Exclusions precede every write to the newly allocated control. The owning
  * pointer is installed only after those checks; allocation remains charged. */
 if(!publication_outputs_apart(s,source,p,sizeof(*p))||
 !apart(p,sizeof(*p),t->encoded,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)||
 !apart(p,sizeof(*p),t->readback,SLCLI_BOOTSTRAP_TRANSPORT_BYTES)){s->failed=1;return 0;}
 for(i=0;i<sizeof(*p);i++)((unsigned char *)p)[i]=0;
 s->deadline_publication_original=p;p->storage_original=s;p->source_original=source;
 p->phase_original=source->active_phase_original;p->phase_origin_original=p->phase_original->origin;
 p->phase_origin_tick=p->phase_origin_original->tick;p->phase=source->active_phase;p->started=1;
 p->values.enclosing_expires=s->deadline_original->expires;
 p->values.active_phase_expires=p->phase_original->expires;p->values.active_phase=p->phase;
 p->values.parent_pid=t->parent_pid;p->values.parent_creation[0]=t->parent_creation.dwLowDateTime;
 p->values.parent_creation[1]=t->parent_creation.dwHighDateTime;
 p->values.qpc_frequency=(uint64_t)s->operation_origin_original->native_windows_frequency.QuadPart;
 if(!publication_clock(p,&p->entry,s->deadline_publication_entry_original.tick))return publication_stop(p);
 t->publication_attempted=1;
 if(!publication_clock(p,&t->publication[0],p->entry.tick))return publication_stop(p);
 if(!slcli_product_windows_deadline_encode(t->encoded,SLCLI_PRODUCT_DEADLINE_BYTES,&p->values))return publication_stop(p);
 for(i=SLCLI_PRODUCT_DEADLINE_BYTES;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)t->encoded[i]=0;
 if(!publication_clock(p,&t->publication[1],t->publication[0].tick))return publication_stop(p);
 for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)((volatile unsigned char *)t->writer_view)[i]=t->encoded[i];
 for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++)t->readback[i]=((const volatile unsigned char *)t->writer_view)[i];
 for(i=0;i<SLCLI_BOOTSTRAP_TRANSPORT_BYTES;i++){
 t->compared=i+1;if(t->readback[i]!=t->encoded[i])return publication_stop(p);}
 t->bytes_equal=1;
 if(!publication_clock(p,&t->publication[2],t->publication[1].tick))return publication_stop(p);
 t->terminal=1;
 if(!slcli_node_transport_windows_dispose_writer(t,a)||!t->writer_unmapped||!t->writable_closed||
 !t->view_transition_returned||!t->view_transition_result||t->writer_disposal_pending)return publication_stop(p);
 for(i=5;i<=6;i++)if(!t->calls[i].called||!t->calls[i].returned||!t->calls[i].result||
 !t->calls[i].before.complete||!t->calls[i].after.complete||
 t->calls[i].before.tick<(i==5?t->publication[2].tick:t->calls[5].after.tick)||
 t->calls[i].after.tick<t->calls[i].before.tick||
 t->calls[i].after.tick>=p->values.active_phase_expires)return publication_stop(p);
 if(!publication_clock(p,&p->finish,t->calls[6].after.tick))return publication_stop(p);
 s->prior_tick=p->finish.tick;t->returned=1;p->publication_completed=1;return 1;
}
#endif
