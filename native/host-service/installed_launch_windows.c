#include "installed_launch_windows.h"
#if defined(_WIN32)
static struct slcli_installed_launch_windows *stop(struct slcli_installed_launch_windows *c,uint32_t stage) {
 if(!c->first_failure)c->first_failure=stage;return c;
}
static int same(const struct slcli_installed_launch_windows *c) {
 return c&&c->arena&&slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)&&
  c->deadline->origin==c->origin_original&&c->origin_original->tick==c->origin_tick&&
  c->deadline->expires==c->expires&&c->prior_original->complete&&
  c->prior_original->tick==c->prior_tick&&c->prior_tick>=c->origin_tick&&c->prior_tick<c->expires;
}
static int clock_after(struct slcli_installed_launch_windows *c,struct slcli_native_clock *out,
 const struct slcli_native_clock *prior) {
 int result=slcli_native_clock_read(out);
 return result&&same(c)&&prior->complete&&out->tick>=prior->tick&&out->tick<c->expires;
}
struct slcli_installed_launch_windows *slcli_installed_launch_windows_create_original(
 struct slcli_arena *a,const struct slcli_binding_originals *book,struct slcli_memory_owner *runtime,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const struct slcli_installed_launch_windows_inputs *in) {
 struct slcli_installed_launch_windows *c;size_t i,j,count,charge,total=0,sizes[7];
 if(!a||!book||!runtime||!in||!caller_bytes||
  !slcli_arena_live_block(a,in,sizeof(*in))||!slcli_arena_live_block(a,book,sizeof(*book))||
  !slcli_arena_live_block(a,runtime,sizeof(*runtime))||
  !slcli_arena_live_block(a,caller,caller_bytes)||
  !slcli_source_deadline_original_matches(a,d,caller)||book->deadline_original!=d||
  !prior||!slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||
  !slcli_arena_live_block(a,in->source,sizeof(*in->source))||
  !slcli_arena_live_block(a,in->claims,sizeof(*in->claims))||
  !in->claims->storage||!slcli_arena_live_block(a,in->claims->storage,sizeof(*in->claims->storage))||
  in->claims->storage->arena!=a||in->claims->storage->book!=book||
  !slcli_arena_live_block(a,in->prefix,sizeof(*in->prefix))||
  !in->transport_descriptor_bytes||
  !slcli_arena_live_range(a,in->transport_descriptor,in->transport_descriptor_bytes)||
  !slcli_arena_live_block(a,in->launcher,sizeof(*in->launcher))||
  in->launcher->containing||in->launcher->transport||in->launcher->prefix||
  in->launcher->inherited_count>SIZE_MAX/sizeof(HANDLE)-4||
  (in->launcher->inherited_count&&!slcli_arena_live_block(a,in->launcher->inherited,
    in->launcher->inherited_count*sizeof(HANDLE))))return NULL;
 count=in->launcher->inherited_count+4;
 sizes[0]=sizeof(*c);sizes[1]=sizeof(*c->source);sizes[2]=sizeof(*c->prefix);
 sizes[3]=sizeof(*c->transport);sizes[4]=sizeof(*c->launcher);
 sizes[5]=sizeof(*c->launch_inputs);sizes[6]=count*sizeof(HANDLE);
 for(i=0;i<7;i++){
  charge=slcli_arena_allocation_bytes(sizes[i]);
  if(!charge||charge>SIZE_MAX-total)return NULL;total+=charge;
 }
 /* Each allocation is a new exact block, never an incoming capture address.
  * Store every obtained pointer before the next fallible allocation. */
 c=slcli_arena_calloc(a,1,sizeof(*c));if(!c)return NULL;
 c->arena=a;c->inputs_original=in;c->book_original=book;c->runtime_original=runtime;
 c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;c->prior_original=prior;
 c->origin_original=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;
 c->prior_tick=prior->tick;
 c->own_control_bytes=total;
 c->source=slcli_arena_calloc(a,1,sizeof(*c->source));if(!c->source)return stop(c,1);
 c->prefix=slcli_arena_calloc(a,1,sizeof(*c->prefix));if(!c->prefix)return stop(c,1);
 c->transport=slcli_arena_calloc(a,1,sizeof(*c->transport));if(!c->transport)return stop(c,1);
 c->launcher=slcli_arena_calloc(a,1,sizeof(*c->launcher));if(!c->launcher)return stop(c,1);
 c->launch_inputs=slcli_arena_calloc(a,1,sizeof(*c->launch_inputs));if(!c->launch_inputs)return stop(c,1);
 c->inherited=slcli_arena_calloc(a,count,sizeof(*c->inherited));if(!c->inherited)return stop(c,1);
 c->inherited_count=count;
 c->allocated_after_result=clock_after(c,&c->allocated_after,prior);
 if(!c->allocated_after_result)return stop(c,2);
 c->source_result=slcli_runtime_source_windows_acquire_claimed_original(c->source,a,book,runtime,
  d,caller,caller_bytes,&c->allocated_after,in->source,in->claims);
 c->source_returned=1;if(!c->source_result)return stop(c,3);
 c->source_after_result=clock_after(c,&c->source_after,&c->source->containing->constructor_after);
 if(!c->source_after_result)return stop(c,4);
 c->prefix_result=slcli_node_prefix_prepare_original(c->prefix,a,d,caller,caller_bytes,
  &c->source_after,in->prefix,runtime,&c->source->containing->backing->request);
 c->prefix_returned=1;if(!c->prefix_result)return stop(c,5);
 c->prefix_after_result=clock_after(c,&c->prefix_after,&c->prefix->after);
 if(!c->prefix_after_result)return stop(c,6);
 c->transport_result=slcli_node_transport_windows_create(c->transport,a,d,caller,caller_bytes,
  &c->prefix_after,in->transport_descriptor,in->transport_descriptor_bytes);
 c->transport_returned=1;if(!c->transport_result)return stop(c,7);
 c->transport_after_result=clock_after(c,&c->transport_after,&c->transport->calls[13].after);
 if(!c->transport_after_result)return stop(c,8);
 *c->launch_inputs=*in->launcher;
 /* Constructor-born originals cannot be supplied in a pre-construction
  * handle list. Route precisely these four actual handles plus owned extras. */
 c->inherited[0]=c->source->containing->backing->backing;
 c->inherited[1]=c->source->containing->backing->duplicate;
 c->inherited[2]=c->transport->readonly;c->inherited[3]=c->transport->parent;
 for(i=4;i<count;i++)c->inherited[i]=in->launcher->inherited[i-4];
 for(i=0;i<count;i++){
  if(!c->inherited[i]||c->inherited[i]==INVALID_HANDLE_VALUE)return stop(c,9);
  for(j=0;j<i;j++)if(c->inherited[i]==c->inherited[j])return stop(c,9);
 }
 c->launch_inputs->inherited=c->inherited;c->launch_inputs->inherited_count=count;
 c->launch_inputs->containing=c->source->containing;
 c->launch_inputs->prefix=c->prefix;c->launch_inputs->transport=c->transport;
 c->launcher_result=slcli_launcher_windows_create(c->launcher,a,d,caller,caller_bytes,
  &c->transport_after,c->launch_inputs,runtime,c->source->containing->backing);
 c->launcher_returned=1;if(!c->launcher_result)return stop(c,9);
 /* Create's raw process after-clock precedes mapping and route readback.
  * The completed last native route observation is the actual frontier. */
 c->finish_result=clock_after(c,&c->finish,&c->source->containing->route_writer.readback.after);
 if(!c->finish_result)return stop(c,10);
 c->complete=1;return c;
}
#endif
