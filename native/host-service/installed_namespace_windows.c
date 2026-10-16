#include "installed_namespace_windows.h"
#if defined(_WIN32)
/* Direct imported native ABI; import eligibility/code generation remain whole
 * build qualification inputs. No lazy loader or caller callback is used. */
NTSYSAPI NTSTATUS NTAPI NtCreateFile(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,
 PIO_STATUS_BLOCK,PLARGE_INTEGER,ULONG,ULONG,ULONG,ULONG,PVOID,ULONG);
static int away(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int canonical(const WCHAR *p,size_t bytes,size_t *count){
 size_t i,start,n,total=1;
 if(!p||bytes%sizeof(WCHAR)||bytes<5*sizeof(WCHAR)||bytes/sizeof(WCHAR)>32767)return 0;
 n=bytes/sizeof(WCHAR);
 if(!((p[0]>=L'A'&&p[0]<=L'Z')||(p[0]>=L'a'&&p[0]<=L'z'))||p[1]!=L':'||p[2]!=L'\\'||p[n-1])return 0;
 start=3;
 for(i=3;i<n;i++){
  if(p[i]==L'/'||p[i]==L':'||p[i]==L'"'||p[i]==L'<'||p[i]==L'>'||
   p[i]==L'|'||p[i]==L'*'||p[i]==L'?'||(p[i]&&p[i]<32)||(!p[i]&&i!=n-1))return 0;
  if(p[i]==L'\\'||i==n-1){
   size_t length=i-start;
   if(!length||p[i-1]==L'.'||p[i-1]==L' '||(length==1&&p[start]==L'.')||
    (length==2&&p[start]==L'.'&&p[start+1]==L'.'))return 0;
   total++;start=i+1;
  }
 }
 *count=total;return 1;
}
static int clock_original(struct slcli_installed_namespace_windows *c,
 struct slcli_native_clock *out,const struct slcli_native_clock *prior){
 return prior&&prior->complete&&slcli_native_clock_read(out)&&
  out->tick>=prior->tick&&out->tick<c->deadline->expires;
}
static int claimed_original(const struct slcli_installed_namespace_windows *c,struct slcli_arena *a){
 const struct slcli_binding_storage_original *s=c->storage_original;
 struct slcli_binding_storage_geometry g;
 size_t i;uint32_t ordinal;const struct slcli_binding_row_claim_original *claim;
 if(!s)return !c->plans_original&&!c->ids_original;
 if(!slcli_arena_live_block(a,s,sizeof(*s))||!s->prepared||s->arena!=a||
  s->book!=c->book||c->book->storage_original!=s||s->attempted!=c->book->count||
  !slcli_binding_storage_geometry_derive(s->capacity,&g)||
  s->references!=c->book->references||s->ids!=c->book->original_ids||s->indices!=c->book->original_indices||
  !slcli_arena_live_block(a,s->references,g.references_bytes)||
  !slcli_arena_live_block(a,s->ids,g.ids_bytes)||!slcli_arena_live_block(a,s->indices,g.indices_bytes)||
  s->attempted>s->capacity||s->capacity>SIZE_MAX/sizeof(*claim)||
  !slcli_arena_live_block(a,s->claims,s->capacity*sizeof(*claim))||
  c->count>SIZE_MAX/sizeof(*c->plans_original)||c->count>SIZE_MAX/sizeof(*c->ids_original)||
  !slcli_arena_live_block(a,c->plans_original,c->count*sizeof(*c->plans_original))||
  !slcli_arena_live_block(a,c->ids_original,c->count*sizeof(*c->ids_original)))return 0;
 for(i=0;i<c->count;i++){
  ordinal=c->ordinals_original[i];if(ordinal>=s->attempted)return 0;claim=&s->claims[ordinal];
  if(!claim->claimed||claim->ordinal!=ordinal||claim->capture!=&c->components[i]||
   claim->capture_bytes!=sizeof(*c->components)||claim->containing_block!=c->components||
   claim->containing_bytes!=c->count*sizeof(*c->components)||
   claim->capture_offset!=i*sizeof(*c->components)||claim->native_class!=SLCLI_NATIVE_ORIGINAL_INSTALLED_NAMESPACE||
   c->book->original_ids[ordinal]!=c->ids_original[i]||
   c->book->references[ordinal].version!=c->plans_original[i].version)return 0;
 }
 return 1;
}
static int plan_original(const struct slcli_installed_namespace_windows *c,struct slcli_arena *a){
 size_t i;const struct slcli_installed_namespace_component_windows *p;
 const struct slcli_native_reference *r;uint16_t effects;uint64_t handle;
 if(!slcli_source_deadline_original_matches(a,c->deadline,c->caller)||
  c->book->deadline_original!=c->deadline||c->prepared!=c->count||!claimed_original(c,a))return 0;
 for(i=0;i<c->path_bytes;i++)if(((const unsigned char *)c->path)[i]!=((const unsigned char *)c->path_original)[i])return 0;
 for(i=0;i<c->count;i++){
  p=&c->components[i];
  if(p->ordinal!=c->ordinals_original[i]||p->ordinal>=c->book->count)return 0;
  r=&c->book->references[p->ordinal];
  handle=p->returned&&p->status>=0&&p->status!=0x103&&p->handle&&p->handle!=INVALID_HANDLE_VALUE?(uintptr_t)p->handle:0;
  effects=p->called?(p->returned?(handle?15:23):3):1;
  if(r->version!=p->version||!p->version||r->kind!=(i+1==c->count?4:21)||
   r->native_original!=p||r->native_class!=SLCLI_NATIVE_ORIGINAL_INSTALLED_NAMESPACE||
   r->effect_flags!=effects||r->state!=(handle?0:2)||r->handle!=handle||
   r->service_request_original||r->physical_extent!=UINT16_MAX||r->alias_of!=UINT16_MAX)return 0;
 }
 return 1;
}
static int acquire_original(
 struct slcli_installed_namespace_windows *c,struct slcli_arena *a,
 const struct slcli_binding_originals *b,const struct slcli_source_deadline_original *d,
 const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,
 const WCHAR *path,size_t path_bytes,const uint32_t *ordinals,size_t count,
 struct slcli_binding_storage_original *storage,const struct slcli_native_reference *plans,const uint32_t *ids){
 size_t expected,i,j,start,end,n;struct slcli_native_reference *r;
 struct slcli_installed_namespace_component_windows *p;
 const struct slcli_native_clock *previous=prior;const struct slcli_native_reference *planned;
 if(!c||!a||!slcli_arena_live_block(a,c,sizeof(*c))||!b||
  !slcli_arena_live_range(a,b,sizeof(*b))||(!storage&&!b->count)||!b->references||
  b->count>SIZE_MAX/sizeof(*r)||(!storage&&!slcli_arena_live_range(a,b->references,b->count*sizeof(*r)))||
  b->deadline_original!=d||!slcli_source_deadline_original_matches(a,d,caller)||
  !caller_bytes||!slcli_arena_live_block(a,caller,caller_bytes)||!prior||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||
  prior->tick<d->origin->tick||prior->tick>=d->expires||!path||
  !slcli_arena_live_range(a,path,path_bytes)||!canonical(path,path_bytes,&expected)||
  count!=expected||count>SIZE_MAX/sizeof(*p)||count>SIZE_MAX/sizeof(*ordinals)||
  !ordinals||!slcli_arena_live_range(a,ordinals,count*sizeof(*ordinals))||
  !away(c,sizeof(*c),path,path_bytes)||!away(c,sizeof(*c),ordinals,count*sizeof(*ordinals))||
  !away(c,sizeof(*c),b,sizeof(*b))||(!storage&&!away(c,sizeof(*c),b->references,b->count*sizeof(*r)))||
  !away(c,sizeof(*c),caller,caller_bytes)||!away(c,sizeof(*c),prior,sizeof(*prior))||
  !slcli_source_deadline_away(d,c,sizeof(*c)))return 0;
 for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
 if(storage&&(!slcli_arena_live_block(a,storage,sizeof(*storage))||!storage->prepared||
  storage->arena!=a||storage->book!=b||b->storage_original!=storage||b->count!=storage->attempted||
  storage->attempted>storage->capacity||count>storage->capacity-storage->attempted||
  count>SIZE_MAX/sizeof(*plans)||!slcli_arena_live_block(a,plans,count*sizeof(*plans))||
  !slcli_arena_live_block(a,ids,count*sizeof(*ids))||
  !away(c,sizeof(*c),plans,count*sizeof(*plans))||!away(c,sizeof(*c),ids,count*sizeof(*ids))))return 0;
 for(i=0;i<count;i++){
  if(storage){if(ordinals[i]!=storage->attempted+i)return 0;}
  else if(ordinals[i]>=b->count)return 0;
  for(j=0;j<i;j++)if(ordinals[i]==ordinals[j])return 0;
  planned=storage?&plans[i]:&b->references[ordinals[i]];
  if(planned->kind!=(i+1==count?4:21)||!planned->version||planned->state!=2||planned->effect_flags!=1||
   planned->handle||planned->native_original||planned->service_request_original||
   planned->native_class!=SLCLI_NATIVE_ORIGINAL_UNCONSTRUCTED||
   planned->physical_extent!=UINT16_MAX||planned->alias_of!=UINT16_MAX)return 0;
 }
 c->started=1;c->book=b;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;
 c->storage_original=storage;c->plans_original=plans;c->ids_original=ids;
 c->path_original=path;c->path_bytes=path_bytes;c->ordinals_original=ordinals;c->count=count;c->prior_original=prior;
 c->path=slcli_arena_alloc(a,path_bytes);if(!c->path)return 0;
 for(i=0;i<path_bytes;i++)((unsigned char *)c->path)[i]=((const unsigned char *)path)[i];
 c->root=slcli_arena_calloc(a,8,sizeof(WCHAR));if(!c->root)return 0;
 c->root[0]=L'\\';c->root[1]=L'?';c->root[2]=L'?';c->root[3]=L'\\';
 c->root[4]=path[0];c->root[5]=L':';c->root[6]=L'\\';
 c->components=slcli_arena_calloc(a,count,sizeof(*p));if(!c->components)return 0;
 if(storage)for(i=0;i<count;i++){
  if(!slcli_binding_storage_claim_pre_effect_original(storage,&plans[i],ids[i],
   c->components,count*sizeof(*p),&c->components[i],sizeof(*p),SLCLI_NATIVE_ORIGINAL_INSTALLED_NAMESPACE))return 0;
 }
 n=path_bytes/sizeof(WCHAR);start=3;
 for(i=0;i<count;i++){
  p=&c->components[i];p->ordinal=ordinals[i];r=&b->references[p->ordinal];p->version=r->version;
  if(!i){p->name.Buffer=c->root;p->name.Length=14;p->name.MaximumLength=16;}
  else{
   end=start;while(end<n-1&&c->path[end]!=L'\\')end++;
   p->name.Buffer=c->path+start;p->name.Length=(USHORT)((end-start)*sizeof(WCHAR));
   /* Component backing includes the actual separator or terminal NUL. */
   p->name.MaximumLength=(USHORT)((end-start+1)*sizeof(WCHAR));start=end+1;
  }
  p->attributes.Length=sizeof(p->attributes);p->attributes.ObjectName=&p->name;
  p->attributes.Attributes=0x40u|0x1000u;
  r->native_original=p;r->native_class=SLCLI_NATIVE_ORIGINAL_INSTALLED_NAMESPACE;c->prepared=i+1;
 }
 for(i=0;i<count;i++){
  p=&c->components[i];r=&b->references[p->ordinal];
  p->attributes.RootDirectory=i?c->components[i-1].handle:NULL;
  if(!plan_original(c,a)||!clock_original(c,&p->before,previous)||!plan_original(c,a))return 0;
  p->args[0]=(uintptr_t)&p->handle;p->args[1]=0x100000u|0x80u|0x20000u|1u;
  p->args[2]=(uintptr_t)&p->attributes;p->args[3]=(uintptr_t)&p->io;
  p->args[6]=1;p->args[7]=1;p->args[8]=0x20u|0x200000u|(i+1<count?1u:0x40u);
  r->effect_flags=3;p->called=1;c->attempted=i+1;
  p->status=NtCreateFile(&p->handle,(ACCESS_MASK)p->args[1],&p->attributes,&p->io,
   NULL,0,1,1,(ULONG)p->args[8],NULL,0);p->returned=1;
  r->handle=p->status>=0&&p->status!=0x103&&p->handle&&p->handle!=INVALID_HANDLE_VALUE?(uintptr_t)p->handle:0;
  r->effect_flags=(p->status>=0&&r->handle)?15:23;r->state=r->effect_flags==15?0:2;
  if(!clock_original(c,&p->after,&p->before)||p->status<0||p->status==0x103||
   (NTSTATUS)p->io.Status<0||(NTSTATUS)p->io.Status==0x103||p->io.Information!=1||!r->handle||!plan_original(c,a))return 0;
  if(!clock_original(c,&p->query_before,&p->after)||!plan_original(c,a))return 0;
  p->query_args[0]=(uintptr_t)p->handle;p->query_args[1]=FileAttributeTagInfo;
  p->query_args[2]=(uintptr_t)&p->tag;p->query_args[3]=sizeof(p->tag);
  p->query_called=1;p->query_status=GetFileInformationByHandleEx(p->handle,FileAttributeTagInfo,&p->tag,sizeof(p->tag));p->query_returned=1;
  if(!p->query_status)p->query_error=GetLastError();
  if(!clock_original(c,&p->query_after,&p->query_before)||!p->query_status||
   (p->tag.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)||p->tag.ReparseTag||
   !!(p->tag.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=(i+1<count)||!plan_original(c,a))return 0;
  p->complete=1;c->completed=i+1;previous=&p->query_after;
 }
 c->acquired=1;return 1;
}
int slcli_installed_namespace_windows_acquire_original(
 struct slcli_installed_namespace_windows *c,struct slcli_arena *a,
 const struct slcli_binding_originals *b,const struct slcli_source_deadline_original *d,
 const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,
 const WCHAR *path,size_t path_bytes,const uint32_t *ordinals,size_t count){
 return acquire_original(c,a,b,d,caller,caller_bytes,prior,path,path_bytes,ordinals,count,NULL,NULL,NULL);
}
int slcli_installed_namespace_windows_acquire_claimed_original(
 struct slcli_installed_namespace_windows *c,struct slcli_arena *a,
 struct slcli_binding_storage_original *storage,const struct slcli_source_deadline_original *d,
 const void *caller,size_t caller_bytes,const struct slcli_native_clock *prior,
 const WCHAR *path,size_t path_bytes,const uint32_t *ordinals,size_t count,
 const struct slcli_native_reference *plans,const uint32_t *ids){
 if(!storage||!a||!slcli_arena_live_block(a,storage,sizeof(*storage)))return 0;
 return acquire_original(c,a,storage->book,d,caller,caller_bytes,prior,path,path_bytes,ordinals,count,storage,plans,ids);
}
int slcli_installed_namespace_windows_matches_original(
 const struct slcli_installed_namespace_windows *c,struct slcli_arena *a){
 size_t i,j,expected,start,end,n;const struct slcli_native_reference *r;
 const struct slcli_installed_namespace_component_windows *p;
 const struct slcli_native_clock *previous;
 if(!c||!a||!slcli_arena_live_block(a,c,sizeof(*c))||!c->started||!c->acquired||
  !slcli_source_deadline_original_matches(a,c->deadline,c->caller)||
  !c->caller_bytes||!slcli_arena_live_block(a,c->caller,c->caller_bytes)||
  !c->book||!slcli_arena_live_range(a,c->book,sizeof(*c->book))||
  c->book->deadline_original!=c->deadline||!c->book->count||!c->book->references||
  c->book->count>SIZE_MAX/sizeof(*r)||
  !slcli_arena_live_range(a,c->book->references,c->book->count*sizeof(*r))||
  !slcli_arena_live_block(a,c->path,c->path_bytes)||
  !slcli_arena_live_range(a,c->path_original,c->path_bytes)||
  !canonical(c->path,c->path_bytes,&expected)||expected!=c->count||
  c->count>SIZE_MAX/sizeof(*p)||c->count>SIZE_MAX/sizeof(uint32_t)||
  !slcli_arena_live_range(a,c->ordinals_original,c->count*sizeof(uint32_t))||
  !slcli_arena_live_block(a,c->root,8*sizeof(WCHAR))||
  !slcli_arena_live_block(a,c->components,c->count*sizeof(*p))||
  c->prepared!=c->count||c->attempted!=c->count||c->completed!=c->count||
  !c->prior_original||!slcli_arena_live_range(a,c->prior_original,sizeof(*previous))||
  !c->prior_original->complete||c->prior_original->tick<c->deadline->origin->tick)return 0;
 if(c->leaf_borrow_attempted>1||
  (c->leaf_borrow_attempted?(!c->leaf_borrower_original||!c->leaf_borrower_bytes||
   !slcli_arena_live_block(a,c->leaf_borrower_original,c->leaf_borrower_bytes)):
   (c->leaf_borrower_original||c->leaf_borrower_bytes)))return 0;
 if(!claimed_original(c,a))return 0;
 for(i=0;i<c->path_bytes;i++)if(((const unsigned char *)c->path)[i]!=((const unsigned char *)c->path_original)[i])return 0;
 if(c->root[0]!=L'\\'||c->root[1]!=L'?'||c->root[2]!=L'?'||c->root[3]!=L'\\'||
  c->root[4]!=c->path[0]||c->root[5]!=L':'||c->root[6]!=L'\\'||c->root[7])return 0;
 previous=c->prior_original;start=3;n=c->path_bytes/sizeof(WCHAR);
 for(i=0;i<c->count;i++){
  p=&c->components[i];
  if(p->ordinal!=c->ordinals_original[i]||p->ordinal>=c->book->count)return 0;
  for(j=0;j<i;j++)if(p->ordinal==c->components[j].ordinal)return 0;
  r=&c->book->references[p->ordinal];
  if(r->version!=p->version||!p->version||r->native_original!=p||
   r->native_class!=SLCLI_NATIVE_ORIGINAL_INSTALLED_NAMESPACE||r->kind!=(i+1==c->count?4:21)||
   r->state||r->effect_flags!=15||r->handle!=(uintptr_t)p->handle||r->service_request_original||
   r->physical_extent!=UINT16_MAX||r->alias_of!=UINT16_MAX||
   !p->handle||p->handle==INVALID_HANDLE_VALUE||!p->called||!p->returned||p->status<0||p->status==0x103||
   (NTSTATUS)p->io.Status<0||(NTSTATUS)p->io.Status==0x103||p->io.Information!=1||!p->complete||
   !p->query_called||!p->query_returned||!p->query_status||
   (p->tag.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)||p->tag.ReparseTag||
   !!(p->tag.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=(i+1<c->count))return 0;
  if(!i){if(p->name.Buffer!=c->root||p->name.Length!=14||p->name.MaximumLength!=16)return 0;}
  else{
   end=start;while(end<n-1&&c->path[end]!=L'\\')end++;
   if(p->name.Buffer!=c->path+start||p->name.Length!=(end-start)*sizeof(WCHAR)||
    p->name.MaximumLength!=(end-start+1)*sizeof(WCHAR))return 0;
   start=end+1;
  }
  if(p->attributes.Length!=sizeof(p->attributes)||p->attributes.RootDirectory!=(i?c->components[i-1].handle:NULL)||
   p->attributes.ObjectName!=&p->name||p->attributes.Attributes!=(0x40u|0x1000u)||
   p->attributes.SecurityDescriptor||p->attributes.SecurityQualityOfService||
   p->args[0]!=(uintptr_t)&p->handle||p->args[1]!=(0x100000u|0x80u|0x20000u|1u)||
   p->args[2]!=(uintptr_t)&p->attributes||p->args[3]!=(uintptr_t)&p->io||p->args[4]||p->args[5]||
   p->args[6]!=1||p->args[7]!=1||p->args[8]!=(0x20u|0x200000u|(i+1<c->count?1u:0x40u))||p->args[9]||p->args[10]||
   p->query_args[0]!=(uintptr_t)p->handle||p->query_args[1]!=FileAttributeTagInfo||
   p->query_args[2]!=(uintptr_t)&p->tag||p->query_args[3]!=sizeof(p->tag)||
   !p->before.complete||!p->after.complete||!p->query_before.complete||!p->query_after.complete||
   p->before.tick<previous->tick||p->after.tick<p->before.tick||p->query_before.tick<p->after.tick||
   p->query_after.tick<p->query_before.tick||p->query_after.tick>=c->deadline->expires)return 0;
  previous=&p->query_after;
 }
 return 1;
}
#endif
