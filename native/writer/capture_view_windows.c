#include "capture_view_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int zero(const void *p,size_t n){size_t i;const unsigned char *b=p;for(i=0;i<n;i++)if(b[i])return 0;return 1;}
static int same(const struct slcli_writer_capture_extent_windows *c){
 const struct slcli_writer_capture_backing_windows *b;
 if(!c||!c->started||!c->arena||!slcli_arena_live_block(c->arena,c,sizeof(*c))||
  !(b=c->backing_original)||!slcli_arena_live_block(c->arena,b,sizeof(*b))||b->arena!=c->arena||
  !slcli_writer_capture_backing_windows_matches_original(b)||!b->views_started||
  b->view_next>b->count||c->plan_index>=b->view_next||c->plan_index>=b->count||
  b->plan[c->plan_index].capture!=c||c->readback!=b->readback||c->owner!=b->owner||
  c->request.index!=b->request.index||c->request.sequence!=b->request.sequence||
  c->file_bytes!=b->file_bytes||c->file_offset!=b->plan[c->plan_index].file_offset||
  c->bytes!=b->plan[c->plan_index].bytes||c->sections[0]!=b->sections[0]||c->sections[1]!=b->sections[1]||
  c->backing_registered||c->view_registered||c->live_registered)return 0;
 return 1;
}
static int tick(struct slcli_writer_capture_extent_windows *c,unsigned i,unsigned post){
 if(post){c->after_result[i]=slcli_native_clock_read(&c->clocks[i][post]);c->after_returned[i]=1;}
 else{c->before_result[i]=slcli_native_clock_read(&c->clocks[i][post]);c->before_returned[i]=1;}
 if(!(post?c->after_result[i]:c->before_result[i])||!same(c)||
  c->clocks[i][post].tick<c->previous_tick||c->clocks[i][post].tick>=c->readback->deadline->expires||
  !slcli_evidence_deadline_precede(c->readback->creator->evidence_file,&c->clocks[i][post]))return 0;
 c->previous_tick=c->clocks[i][post].tick;return 1;
}
int slcli_writer_capture_view_windows_prepare_original(struct slcli_writer_capture_extent_windows *c,
 struct slcli_writer_capture_backing_windows *b,size_t index){
 unsigned i;size_t j;uint64_t previous;
 if(!c||!b||!b->arena||!slcli_arena_live_block(b->arena,b,sizeof(*b))||
  !slcli_writer_capture_backing_windows_matches_original(b)||index>=b->count||
  !b->views_started||b->views_failed||b->views_complete||b->view_next!=index||
  b->plan[index].capture!=c||!slcli_arena_live_block(b->arena,c,sizeof(*c))||!zero(c,sizeof(*c))||
  !apart(c,sizeof(*c),b,sizeof(*b))||!slcli_source_deadline_away(b->readback->deadline,c,sizeof(*c)))return 0;
 previous=b->previous_tick;
 if(index){const struct slcli_writer_capture_extent_windows *p=b->plan[index-1].capture;
  if(p->backing_original!=b||p->plan_index!=index-1||!slcli_writer_capture_view_windows_matches_original(p))return 0;
  previous=p->previous_tick;
 }
 b->view_next=index+1;
 c->started=1;c->failed=1;c->arena=b->arena;c->backing_original=b;c->plan_index=index;
 c->readback=b->readback;c->owner=b->owner;c->request=b->request;c->file_bytes=b->file_bytes;
 c->file_offset=b->plan[index].file_offset;c->bytes=b->plan[index].bytes;c->previous_tick=previous;
 c->sections[0]=b->sections[0];c->sections[1]=b->sections[1];
 for(i=0;i<2;i++){
  unsigned call=i?5:1;
  c->map_args[i][0]=(uintptr_t)c->sections[i];c->map_args[i][1]=i?FILE_MAP_READ:FILE_MAP_WRITE;
  c->map_args[i][2]=(DWORD)(c->file_offset>>32);c->map_args[i][3]=(DWORD)c->file_offset;c->map_args[i][4]=c->bytes;
  if(!tick(c,call,0))return 0;c->called[call]=1;
  c->views[i]=MapViewOfFile(c->sections[i],(DWORD)c->map_args[i][1],(DWORD)c->map_args[i][2],
   (DWORD)c->map_args[i][3],(SIZE_T)c->bytes);c->returned[call]=1;
  if(!c->views[i])c->map_error[i]=GetLastError();
  if(!tick(c,call,1)||!c->views[i]||(uintptr_t)c->views[i]>UINTPTR_MAX-(uintptr_t)c->bytes)return 0;
  for(j=0;j<index;j++){unsigned k;for(k=0;k<2;k++)
   if(!apart(c->views[i],(size_t)c->bytes,b->plan[j].capture->views[k],(size_t)b->plan[j].bytes))return 0;}
  if(i&&!apart(c->views[0],(size_t)c->bytes,c->views[1],(size_t)c->bytes))return 0;
  c->query_args[i][0]=(uintptr_t)c->views[i];c->query_args[i][1]=(uintptr_t)&c->geometry[i];c->query_args[i][2]=sizeof(c->geometry[i]);
  if(!tick(c,call+1,0))return 0;c->called[call+1]=1;
  c->query_result[i]=VirtualQuery(c->views[i],&c->geometry[i],sizeof(c->geometry[i]));c->returned[call+1]=1;
  if(!c->query_result[i])c->query_error[i]=GetLastError();
  if(!tick(c,call+1,1)||c->query_result[i]!=sizeof(c->geometry[i])||
   c->geometry[i].BaseAddress!=c->views[i]||c->geometry[i].AllocationBase!=c->views[i]||
   c->geometry[i].RegionSize!=c->bytes||c->geometry[i].State!=MEM_COMMIT||
   c->geometry[i].Type!=MEM_MAPPED||c->geometry[i].Protect!=(i?PAGE_READONLY:PAGE_READWRITE))return 0;
  for(j=0;j<(size_t)c->bytes;j++){if(((volatile const unsigned char *)c->views[i])[j])return 0;c->zero_scanned[i]=j+1;}
  c->scan_after_result[i]=slcli_native_clock_read(&c->scan_after[i]);c->scan_after_returned[i]=1;
  if(!c->scan_after_result[i]||c->scan_after[i].tick<c->previous_tick||
   c->scan_after[i].tick>=c->readback->deadline->expires||!same(c)||
   !slcli_evidence_deadline_precede(c->readback->creator->evidence_file,&c->scan_after[i]))return 0;
  c->previous_tick=c->scan_after[i].tick;
 }
 c->complete=1;c->failed=0;return 1;
}
static int match_one(const struct slcli_writer_capture_extent_windows *c){
 unsigned i;uint64_t previous;
 if(!same(c)||!c->complete||c->failed)return 0;
 previous=c->backing_original->previous_tick;
 if(c->plan_index){const struct slcli_writer_capture_extent_windows *p=c->backing_original->plan[c->plan_index-1].capture;
  if(p->backing_original!=c->backing_original||p->plan_index!=c->plan_index-1||!p->complete||p->failed)return 0;
  previous=p->previous_tick;
 }
 for(i=0;i<2;i++){
  unsigned call=i?5:1,j;
  for(j=call;j<=call+1;j++){
   if(!c->called[j]||!c->returned[j]||!c->before_returned[j]||!c->after_returned[j]||
    !c->before_result[j]||!c->after_result[j]||!c->clocks[j][0].complete||!c->clocks[j][1].complete||
    c->clocks[j][0].tick<previous||c->clocks[j][1].tick<c->clocks[j][0].tick||
    c->clocks[j][1].tick>=c->readback->deadline->expires)return 0;
   previous=c->clocks[j][1].tick;
  }
  if(!c->scan_after_returned[i]||!c->scan_after_result[i]||!c->scan_after[i].complete||
   c->scan_after[i].tick<previous||c->scan_after[i].tick>=c->readback->deadline->expires||
   c->zero_scanned[i]!=c->bytes||!c->views[i]||(uintptr_t)c->views[i]>UINTPTR_MAX-(uintptr_t)c->bytes||
   c->map_args[i][0]!=(uintptr_t)c->sections[i]||c->map_args[i][1]!=(i?FILE_MAP_READ:FILE_MAP_WRITE)||
   c->map_args[i][2]!=(DWORD)(c->file_offset>>32)||c->map_args[i][3]!=(DWORD)c->file_offset||c->map_args[i][4]!=c->bytes||
   c->query_args[i][0]!=(uintptr_t)c->views[i]||c->query_args[i][1]!=(uintptr_t)&c->geometry[i]||
   c->query_args[i][2]!=sizeof(c->geometry[i])||c->query_result[i]!=sizeof(c->geometry[i])||
   c->geometry[i].BaseAddress!=c->views[i]||c->geometry[i].AllocationBase!=c->views[i]||
   c->geometry[i].RegionSize!=c->bytes||c->geometry[i].State!=MEM_COMMIT||
   c->geometry[i].Type!=MEM_MAPPED||c->geometry[i].Protect!=(i?PAGE_READONLY:PAGE_READWRITE))return 0;
  previous=c->scan_after[i].tick;
 }
 return c->previous_tick==previous&&apart(c->views[0],(size_t)c->bytes,c->views[1],(size_t)c->bytes);
}
int slcli_writer_capture_view_windows_matches_original(const struct slcli_writer_capture_extent_windows *c){
 size_t i,j;
 if(!same(c))return 0;
 for(i=0;i<=c->plan_index;i++){
  const struct slcli_writer_capture_extent_windows *p=c->backing_original->plan[i].capture;
  if(p->backing_original!=c->backing_original||p->plan_index!=i||!match_one(p))return 0;
  for(j=0;j<i;j++){unsigned x,y;const struct slcli_writer_capture_extent_windows *q=c->backing_original->plan[j].capture;
   for(x=0;x<2;x++)for(y=0;y<2;y++)if(!apart(p->views[x],(size_t)p->bytes,q->views[y],(size_t)q->bytes))return 0;
  }
 }
 return 1;
}
int slcli_writer_capture_view_windows_prepare_batch_original(struct slcli_writer_capture_backing_windows *b){
 size_t i;uint64_t previous;
 if(!b||!b->arena||!slcli_arena_live_block(b->arena,b,sizeof(*b))||
  !slcli_writer_capture_backing_windows_matches_original(b)||b->views_started||b->view_next||
  b->views_failed||b->views_complete||b->views_finish_returned)return 0;
 /* The source-owned one-use batch is consumed before its first mapping.
  * Any failure leaves every preceding alias and the uncertain row retained. */
 b->views_started=1;
 for(i=0;i<b->count;i++)if(!slcli_writer_capture_view_windows_prepare_original(b->plan[i].capture,b,i)){
  b->views_failed=1;return 0;
 }
 previous=b->plan[b->count-1].capture->previous_tick;
 if(!slcli_writer_capture_view_windows_matches_original(b->plan[b->count-1].capture)){
  b->views_failed=1;return 0;
 }
 b->views_finish_result=slcli_native_clock_read(&b->views_finish);b->views_finish_returned=1;
 if(!b->views_finish_result||b->views_finish.tick<previous||
  b->views_finish.tick>=b->readback->deadline->expires||
  !slcli_evidence_deadline_precede(b->readback->creator->evidence_file,&b->views_finish)){
  b->views_failed=1;return 0;
 }
 b->views_complete=1;return 1;
}
int slcli_writer_capture_view_windows_batch_matches_original(const struct slcli_writer_capture_backing_windows *b){
 if(!b||!b->arena||!slcli_arena_live_block(b->arena,b,sizeof(*b))||
  !slcli_writer_capture_backing_windows_matches_original(b)||!b->views_started||b->views_failed||
  !b->views_complete||b->view_next!=b->count||!b->views_finish_returned||!b->views_finish_result||
  !b->views_finish.complete||b->views_finish.tick>=b->readback->deadline->expires||
  !slcli_writer_capture_view_windows_matches_original(b->plan[b->count-1].capture)||
  b->views_finish.tick<b->plan[b->count-1].capture->previous_tick)return 0;
 return 1;
}
#endif
