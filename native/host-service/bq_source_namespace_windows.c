#include "bq_source_namespace_windows.h"
#if defined(_WIN32)
NTSYSAPI NTSTATUS NTAPI NtCreateFile(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,
 PIO_STATUS_BLOCK,PLARGE_INTEGER,ULONG,ULONG,ULONG,ULONG,PVOID,ULONG);
static int zero(const void *p,size_t n){const unsigned char *b=p;size_t i;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);}
static int canonical(const WCHAR *p,size_t bytes,size_t *count){
 size_t i,start,n,total=1;
 if(!p||bytes%sizeof(WCHAR)||bytes<5*sizeof(WCHAR)||bytes/sizeof(WCHAR)>32767)return 0;
 n=bytes/sizeof(WCHAR);
 if(!((p[0]>=L'A'&&p[0]<=L'Z')||(p[0]>=L'a'&&p[0]<=L'z'))||p[1]!=L':'||p[2]!=L'\\'||p[n-1])return 0;
 start=3;
 for(i=3;i<n;i++){
  if(p[i]==L'/'||p[i]==L':'||p[i]==L'"'||p[i]==L'<'||p[i]==L'>'||p[i]==L'|'||
   p[i]==L'*'||p[i]==L'?'||(p[i]&&p[i]<32)||(!p[i]&&i!=n-1))return 0;
  if(p[i]==L'\\'||i==n-1){size_t len=i-start;
   if(!len||p[i-1]==L'.'||p[i-1]==L' '||(len==1&&p[start]==L'.')||
    (len==2&&p[start]==L'.'&&p[start+1]==L'.'))return 0;
   total++;start=i+1;
  }
 }
 *count=total;return 1;
}
static int same(const struct slcli_bq_source_namespace_windows *c){
 size_t i,n;
 if(!c||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||!c->started||
  !c->count||c->count>SIZE_MAX/sizeof(*c->components)||c->count>SIZE_MAX/sizeof(*c->targets)||
  !slcli_arena_live_block(c->arena,c->caller,c->caller_bytes)||
  !slcli_source_deadline_original_matches(c->arena,c->deadline,c->caller)||
  c->deadline->origin!=c->origin||c->origin->tick!=c->origin_tick||c->deadline->expires!=c->expires||
  !slcli_arena_live_range(c->arena,c->source_path,c->path_bytes)||
  !slcli_arena_live_block(c->arena,c->path,c->path_bytes)||!slcli_arena_live_block(c->arena,c->root,8*sizeof(WCHAR))||
  !slcli_arena_live_block(c->arena,c->components,c->count*sizeof(*c->components))||
  !slcli_arena_live_block(c->arena,c->targets,c->count*sizeof(*c->targets))||
  !slcli_arena_live_range(c->arena,c->prior,sizeof(*c->prior))||!c->prior->complete||
  c->prior->tick<c->origin_tick||c->prior->tick>=c->expires||
  !canonical(c->source_path,c->path_bytes,&n)||n!=c->count)return 0;
 for(i=0;i<c->path_bytes;i++)if(((const unsigned char *)c->path)[i]!=((const unsigned char *)c->source_path)[i])return 0;
 if(c->root[0]!=L'\\'||c->root[1]!=L'?'||c->root[2]!=L'?'||c->root[3]!=L'\\'||
  c->root[4]!=c->path[0]||c->root[5]!=L':'||c->root[6]!=L'\\'||c->root[7])return 0;
 for(i=0;i<c->count;i++)if(c->targets[i].ordinal!=i||c->targets[i].taken!=1||
  c->targets[i].component!=&c->components[i]||c->targets[i].output!=&c->components[i].handle)return 0;
 return 1;
}
static int tick(struct slcli_bq_source_namespace_windows *c,struct slcli_native_clock *out){
 if(!slcli_native_clock_read(out)||!same(c)||out->tick<c->previous_tick||out->tick>=c->expires)return 0;
 c->previous_tick=out->tick;return 1;
}
int slcli_bq_source_namespace_windows_matches_original(const struct slcli_bq_source_namespace_windows *c){
 size_t i,start=3,end,n;uint64_t prior;
 if(!same(c)||c->failed||c->attempted!=c->count||c->completed!=c->count)return 0;
 n=c->path_bytes/sizeof(WCHAR);prior=c->prior->tick;
 for(i=0;i<c->count;i++){
  const struct slcli_installed_namespace_component_windows *p=&c->components[i];
  const WCHAR *name;size_t length,maximum;ULONG options=0x20u|0x200000u|(i+1<c->count?1u:0x40u);
  if(!i){name=c->root;length=14;maximum=16;}
  else{end=start;while(end<n-1&&c->path[end]!=L'\\')end++;name=c->path+start;
   length=(end-start)*sizeof(WCHAR);maximum=length+sizeof(WCHAR);start=end+1;}
  if(p->name.Buffer!=name||p->name.Length!=length||p->name.MaximumLength!=maximum||
   p->attributes.Length!=sizeof(p->attributes)||p->attributes.ObjectName!=&p->name||
   p->attributes.Attributes!=(0x40u|0x1000u)||p->attributes.RootDirectory!=(i?c->components[i-1].handle:NULL)||
   p->attributes.SecurityDescriptor||p->attributes.SecurityQualityOfService||
   p->args[0]!=(uintptr_t)&p->handle||p->args[1]!=(0x100000u|0x80u|0x20000u|1u)||
   p->args[2]!=(uintptr_t)&p->attributes||p->args[3]!=(uintptr_t)&p->io||p->args[4]||p->args[5]||
   p->args[6]!=1||p->args[7]!=1||p->args[8]!=options||p->args[9]||p->args[10]||
   p->called!=1||p->returned!=1||p->status<0||p->status==0x103||!p->handle||p->handle==INVALID_HANDLE_VALUE||
   (NTSTATUS)p->io.Status<0||(NTSTATUS)p->io.Status==0x103||p->io.Information!=1||
   p->query_args[0]!=(uintptr_t)p->handle||p->query_args[1]!=FileAttributeTagInfo||
   p->query_args[2]!=(uintptr_t)&p->tag||p->query_args[3]!=sizeof(p->tag)||
   p->query_called!=1||p->query_returned!=1||!p->query_status||p->query_error||p->complete!=1||
   p->tag.ReparseTag||(p->tag.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)||
   !!(p->tag.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=(i+1<c->count)||
   !p->before.complete||p->before.tick<prior||!p->after.complete||p->after.tick<p->before.tick||
   !p->query_before.complete||p->query_before.tick<p->after.tick||!p->query_after.complete||
   p->query_after.tick<p->query_before.tick||p->query_after.tick>=c->expires)return 0;
  prior=p->query_after.tick;
 }
 if(c->previous_tick!=prior)return 0;
 return !c->observed||(c->observed==1&&c->finish_returned==1&&c->finish_result&&c->finish.complete&&
  c->finish.tick>=prior&&c->finish.tick<c->expires);
}
int slcli_bq_source_namespace_windows_acquire_original(
 struct slcli_bq_source_namespace_windows *c,struct slcli_arena *a,
 const struct slcli_source_deadline_original *d,const void *caller,size_t caller_bytes,
 const struct slcli_native_clock *prior,const WCHAR *source,size_t bytes,WCHAR *path,WCHAR *root,
 struct slcli_installed_namespace_component_windows *parts,
 struct slcli_bq_source_namespace_target_windows *targets,size_t count){
 const void *blocks[5],*inputs[4];size_t sizes[5],input_sizes[4],expected,i,j,start=3,end,n;
 if(!a||!c||!d||!source||!prior||!caller||!caller_bytes||!count||
  !slcli_arena_live_range(a,source,bytes)||!canonical(source,bytes,&expected)||count!=expected||
  count>SIZE_MAX/sizeof(*parts)||count>SIZE_MAX/sizeof(*targets)||
  !slcli_source_deadline_original_matches(a,d,caller)||
  !slcli_arena_live_range(a,prior,sizeof(*prior))||!prior->complete||prior->tick<d->origin->tick||prior->tick>=d->expires)return 0;
 blocks[0]=c;sizes[0]=sizeof(*c);blocks[1]=path;sizes[1]=bytes;blocks[2]=root;sizes[2]=8*sizeof(WCHAR);
 blocks[3]=parts;sizes[3]=count*sizeof(*parts);blocks[4]=targets;sizes[4]=count*sizeof(*targets);
 inputs[0]=source;input_sizes[0]=bytes;inputs[1]=caller;input_sizes[1]=caller_bytes;
 inputs[2]=prior;input_sizes[2]=sizeof(*prior);inputs[3]=d;input_sizes[3]=sizeof(*d);
 if(!slcli_arena_live_block(a,caller,caller_bytes))return 0;
 for(i=0;i<5;i++){
  if(!slcli_arena_live_block(a,blocks[i],sizes[i])||!zero(blocks[i],sizes[i])||
   !slcli_source_deadline_away(d,blocks[i],sizes[i]))return 0;
  for(j=0;j<i;j++)if(!apart(blocks[i],sizes[i],blocks[j],sizes[j]))return 0;
  for(j=0;j<4;j++)if(!apart(blocks[i],sizes[i],inputs[j],input_sizes[j]))return 0;
 }
 c->started=1;c->arena=a;c->deadline=d;c->caller=caller;c->caller_bytes=caller_bytes;c->prior=prior;
 c->origin=d->origin;c->origin_tick=d->origin->tick;c->expires=d->expires;c->previous_tick=prior->tick;
 c->source_path=source;c->path_bytes=bytes;c->path=path;c->root=root;c->components=parts;c->targets=targets;c->count=count;
 for(i=0;i<bytes;i++)((unsigned char *)path)[i]=((const unsigned char *)source)[i];
 root[0]=L'\\';root[1]=L'?';root[2]=L'?';root[3]=L'\\';root[4]=path[0];root[5]=L':';root[6]=L'\\';
 /* Every finite target is consumed before the first namespace effect. */
 for(i=0;i<count;i++){targets[i].component=&parts[i];targets[i].output=&parts[i].handle;targets[i].ordinal=i;targets[i].taken=1;}
 n=bytes/sizeof(WCHAR);
 for(i=0;i<count;i++){
  struct slcli_installed_namespace_component_windows *p=&parts[i];
  if(!i){p->name.Buffer=root;p->name.Length=14;p->name.MaximumLength=16;}
  else{end=start;while(end<n-1&&path[end]!=L'\\')end++;p->name.Buffer=path+start;
   p->name.Length=(USHORT)((end-start)*sizeof(WCHAR));p->name.MaximumLength=(USHORT)((end-start+1)*sizeof(WCHAR));start=end+1;}
  p->attributes.Length=sizeof(p->attributes);p->attributes.ObjectName=&p->name;p->attributes.Attributes=0x40u|0x1000u;
  p->attributes.RootDirectory=i?parts[i-1].handle:NULL;
  if(!tick(c,&p->before))goto failed;
  p->args[0]=(uintptr_t)&p->handle;p->args[1]=0x100000u|0x80u|0x20000u|1u;p->args[2]=(uintptr_t)&p->attributes;
  p->args[3]=(uintptr_t)&p->io;p->args[6]=1;p->args[7]=1;p->args[8]=0x20u|0x200000u|(i+1<count?1u:0x40u);
  p->called=1;c->attempted=i+1;p->status=NtCreateFile(&p->handle,(ACCESS_MASK)p->args[1],&p->attributes,&p->io,NULL,0,1,1,(ULONG)p->args[8],NULL,0);p->returned=1;
  if(!tick(c,&p->after)||p->status<0||p->status==0x103||!p->handle||p->handle==INVALID_HANDLE_VALUE||
   (NTSTATUS)p->io.Status<0||(NTSTATUS)p->io.Status==0x103||p->io.Information!=1)goto failed;
  if(!tick(c,&p->query_before))goto failed;
  p->query_args[0]=(uintptr_t)p->handle;p->query_args[1]=FileAttributeTagInfo;p->query_args[2]=(uintptr_t)&p->tag;p->query_args[3]=sizeof(p->tag);
  p->query_called=1;p->query_status=GetFileInformationByHandleEx(p->handle,FileAttributeTagInfo,&p->tag,sizeof(p->tag));p->query_returned=1;
  if(!p->query_status)p->query_error=GetLastError();
  if(!tick(c,&p->query_after)||!p->query_status||p->tag.ReparseTag||(p->tag.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)||
   !!(p->tag.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=(i+1<count))goto failed;
  p->complete=1;c->completed=i+1;
 }
 if(!slcli_bq_source_namespace_windows_matches_original(c))goto failed;
 c->finish_result=slcli_native_clock_read(&c->finish);c->finish_returned=1;
 if(!c->finish_result||!same(c)||c->finish.tick<c->previous_tick||c->finish.tick>=c->expires)goto failed;
 c->observed=1;return 1;
failed:c->failed=1;return 0;
}
#endif
