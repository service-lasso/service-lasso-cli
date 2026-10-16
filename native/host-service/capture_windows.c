#include "capture_windows.h"
#if defined(_WIN32)
#include "source_windows.h"
#include "image_deadline.h"
#include "evidence_deadline.h"
#include <stddef.h>
/* Native owner serialization covers allocation, this chain and primary store
 * accounting. No reset/release operation is provided for a retained loan. */
struct slcli_capture_store_owner *slcli_capture_store_allocate(struct slcli_arena *a,
  struct slcli_primary_engine *primary,HANDLE root,const FILE_ID_INFO *identity,uint64_t files) {
  struct slcli_capture_store_owner *s;
  if(!a||!a->owner||a->owner->limit!=1048576||!primary||!identity||!files||
    !root||root==INVALID_HANDLE_VALUE||!slcli_arena_live_range(a,primary,sizeof(*primary))||
    primary->capture_store||primary->retained_store_bytes>SLCLI_STORE_LIMIT)return 0;
  s=slcli_arena_calloc(a,1,sizeof(*s));if(!s)return 0;
  s->arena=a;s->primary=primary;s->root=root;s->root_identity=*identity;s->maximum_files=files;
  primary->capture_store=s;return s;
}
struct slcli_peer_path_workspace *slcli_capture_windows_workspaces_allocate(
  struct slcli_arena *a,struct slcli_shared_workspace **out) {
  struct workspace_block {
    struct slcli_peer_path_workspace path;
    struct slcli_shared_workspace workspace;
    WCHAR raw[32772];
  } *b;
  /* Caller output control also belongs to the SAME charged native arena. */
  if(!a||!a->owner||a->owner->limit!=1048576||!out||
    !slcli_arena_live_range(a,out,sizeof(*out))||*out)return 0;
  b=slcli_arena_calloc(a,1,sizeof(*b));if(!b)return 0;
  b->path.raw=b->raw;b->path.units=32772;*out=&b->workspace;return &b->path;
}
static int slice(size_t *total,size_t bytes,size_t alignment,size_t *offset) {
  size_t n;
  if(!bytes||!alignment||*total>SIZE_MAX-(alignment-1))return 0;
  n=(*total+alignment-1)/alignment*alignment;
  if(bytes>SIZE_MAX-n)return 0;*offset=n;*total=n+bytes;return 1;
}
static int store_live(struct slcli_capture_windows *c) {
  struct slcli_capture_store_owner *s;struct slcli_capture_store_loan *r,*loan=c->store_loan;
  uint64_t seen=0,bytes=0;int found=0;
  if(!loan||!slcli_arena_live_range(c->arena,loan,sizeof(*loan))||!(s=loan->owner)||
    !slcli_arena_live_range(c->arena,s,sizeof(*s))||s->arena!=c->arena||
    !s->primary||!slcli_arena_live_range(c->arena,s->primary,sizeof(*s->primary))||
    s->primary->capture_store!=s||!s->maximum_files||!s->count||s->count>s->maximum_files||
    s->primary->retained_store_bytes>SLCLI_STORE_LIMIT)return 0;
  for(r=s->first;r;r=r->next) {
    if(++seen>s->count||!slcli_arena_live_range(c->arena,r,sizeof(*r))||
      r->owner!=s||!r->graph||!slcli_arena_live_range(c->arena,r->graph,sizeof(*r->graph))||
      r->graph->arena!=c->arena||r->graph->store_loan!=r||
      !r->maximum||r->maximum>SLCLI_STORE_LIMIT||r->actual>r->maximum||
      r->ordinal!=s->count-seen+1||bytes>SLCLI_STORE_LIMIT-r->maximum)return 0;
    bytes+=r->maximum;
    if(r==loan)found=1;
  }
  return found&&seen==s->count&&bytes<=s->primary->retained_store_bytes&&loan->graph==c&&
    loan->maximum==32+offsetof(struct slcli_peer_windows_capture,used)+65544;
}
struct slcli_capture_windows *slcli_capture_windows_allocate(
  struct slcli_capture_store_owner *s,struct slcli_shared_workspace *workspace,
  struct slcli_peer_path_workspace *path,size_t file_rows,size_t record_rows,
  size_t held_rows,size_t route_rows) {
  struct slcli_arena *a;struct slcli_capture_windows *c;unsigned char *raw;
  struct slcli_image_reader *held,*route;struct slcli_capture_store_loan *loan;
  size_t total=sizeof(*c),offset[11],i,counts[4];
  uint64_t maximum=32+offsetof(struct slcli_peer_windows_capture,used)+65544;
  if(!s||!(a=s->arena)||!slcli_arena_live_range(a,s,sizeof(*s))||!a->owner||
    a->owner->limit!=1048576||!s->primary||
    !slcli_arena_live_range(a,s->primary,sizeof(*s->primary))||
    s->primary->capture_store!=s||!slcli_source_windows_held(s->source_owner)||
    !s->maximum_files||s->count>=s->maximum_files||s->count==UINT64_MAX||
    s->primary->retained_store_bytes>SLCLI_STORE_LIMIT-maximum||
    !slcli_arena_live_range(a,workspace,sizeof(*workspace))||workspace->busy_original||
    !slcli_arena_live_range(a,path,sizeof(*path))||path->borrower||path->units!=32772||
    !slcli_arena_live_range(a,path->raw,32772*sizeof(WCHAR)))return 0;
  counts[0]=file_rows;counts[1]=record_rows;counts[2]=held_rows;counts[3]=route_rows;
  if(!slice(&total,sizeof(struct slcli_writer_windows_path),_Alignof(struct slcli_writer_windows_path),&offset[0])||
    !slice(&total,sizeof(struct slcli_evidence_file),_Alignof(struct slcli_evidence_file),&offset[1])||
    !slice(&total,sizeof(struct slcli_capture_record),_Alignof(struct slcli_capture_record),&offset[2])||
    !slice(&total,sizeof(struct slcli_peer_windows_capture),_Alignof(struct slcli_peer_windows_capture),&offset[3])||
    !slice(&total,sizeof(struct slcli_capture_record_reader),_Alignof(struct slcli_capture_record_reader),&offset[4])||
    !slice(&total,sizeof(struct slcli_image_reader),_Alignof(struct slcli_image_reader),&offset[5])||
    !slice(&total,sizeof(struct slcli_image_reader),_Alignof(struct slcli_image_reader),&offset[6]))return 0;
  for(i=0;i<4;i++)if(!counts[i]||counts[i]>SIZE_MAX/sizeof(slcli_input_capture)||
    !slice(&total,counts[i]*sizeof(slcli_input_capture),_Alignof(slcli_input_capture),&offset[7+i]))return 0;
  /* Loan is inside the same original graph block, so no allocation failure
   * loses a partially allocated resident graph or unattached store record. */
  {size_t loan_offset;
    if(!slice(&total,sizeof(*loan),_Alignof(struct slcli_capture_store_loan),&loan_offset))return 0;
    raw=slcli_arena_calloc(a,1,total);if(!raw)return 0;
    c=(void *)raw;loan=(void *)(raw+loan_offset);
  }
  c->arena=a;c->store_loan=loan;c->created=(void *)(raw+offset[0]);
  c->file=(void *)(raw+offset[1]);c->record=(void *)(raw+offset[2]);
  c->peer=(void *)(raw+offset[3]);c->reader=(void *)(raw+offset[4]);
  held=(void *)(raw+offset[5]);route=(void *)(raw+offset[6]);
  c->file->workspace=workspace;c->file->captures=(void *)(raw+offset[7]);c->file->capture_capacity=file_rows;
  c->reader->workspace=workspace;c->reader->captures=(void *)(raw+offset[8]);c->reader->capture_capacity=record_rows;
  held->workspace=workspace;held->captures=(void *)(raw+offset[9]);held->capture_capacity=held_rows;
  route->workspace=workspace;route->captures=(void *)(raw+offset[10]);route->capture_capacity=route_rows;
  c->peer->held_reader=held;c->peer->route_reader=route;c->peer->path=path;
  /* This SAME counter is also used by primary ledger/artifact/closure writers.
   * Reserve maximum, never successful byte count, including failed partial IO. */
  loan->owner=s;loan->graph=c;loan->maximum=maximum;loan->ordinal=++s->count;
  loan->next=s->first;s->first=loan;s->primary->retained_store_bytes+=maximum;
  return c;
}
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int root_same(const struct slcli_windows_file_capture *c,const FILE_ID_INFO *expected) {
  return c->completed&&c->standard.Directory&&
    c->identity.VolumeSerialNumber==expected->VolumeSerialNumber&&
    slcli_equal(c->identity.FileId.Identifier,expected->FileId.Identifier,16);
}
static int rows_live(struct slcli_arena *a,const slcli_input_capture *rows,size_t count) {
  return count&&count<=SIZE_MAX/sizeof(*rows)&&
    slcli_arena_live_range(a,rows,count*sizeof(*rows));
}
static int graph_live(struct slcli_capture_windows *c) {
  struct slcli_arena *a=c->arena;struct slcli_peer_windows_capture *p=c->peer;
  struct slcli_image_reader *h,*r;
  if(!a||!a->owner||a->owner->limit!=1048576||a->owner->charged>a->owner->limit||
    !p||!(h=p->held_reader)||!(r=p->route_reader)||!p->path||p->path->units!=32772||
    !slcli_arena_live_range(a,c,sizeof(*c))||
    !slcli_arena_live_range(a,c->created,sizeof(*c->created))||
    !slcli_arena_live_range(a,c->file,sizeof(*c->file))||
    !slcli_arena_live_range(a,c->record,sizeof(*c->record))||
    !slcli_arena_live_range(a,p,sizeof(*p))||
    !slcli_arena_live_range(a,c->reader,sizeof(*c->reader))||
    !slcli_arena_live_range(a,h,sizeof(*h))||!slcli_arena_live_range(a,r,sizeof(*r))||
    !slcli_arena_live_range(a,p->path,sizeof(*p->path))||
    !slcli_arena_live_range(a,p->path->raw,32772*sizeof(WCHAR))||!store_live(c))return 0;
  return c->file->workspace&&!c->file->workspace->busy_original&&!p->path->borrower&&
    c->reader->workspace==c->file->workspace&&h->workspace==c->file->workspace&&
    r->workspace==c->file->workspace&&
    slcli_arena_live_range(a,c->file->workspace,sizeof(*c->file->workspace))&&
    slcli_arena_live_range(a,c->reader->workspace,sizeof(*c->reader->workspace))&&
    slcli_arena_live_range(a,h->workspace,sizeof(*h->workspace))&&
    slcli_arena_live_range(a,r->workspace,sizeof(*r->workspace))&&
    rows_live(a,c->file->captures,c->file->capture_capacity)&&
    rows_live(a,c->reader->captures,c->reader->capture_capacity)&&
    rows_live(a,h->captures,h->capture_capacity)&&rows_live(a,r->captures,r->capture_capacity);
}
int slcli_capture_windows_original(struct slcli_capture_windows *c,
  const struct slcli_capture_windows_originals *o,slcli_capture_windows_sink sink,void *archive) {
  struct slcli_writer_windows_path *p;struct slcli_evidence_file *f;int ok=0;
  if(!c||c->used||!o||!sink||!archive||!(p=c->created)||p->used||!(f=c->file)||f->started||
    !c->record||c->record->used||!c->peer||c->peer->used||!c->reader||c->reader->used||
    !o->peer||!o->root_identity||!o->file_token||!o->file_owner||!o->file_owner_bytes||
    !o->descriptor||!o->descriptor_bytes||!o->leaf||!o->leaf_units||!o->path_sink||
    !o->mode_sink||!o->file_sink||!o->archive_original||!o->archive_original_bytes||
    !separate(c,sizeof(*c),o,sizeof(*o))||!separate(c,sizeof(*c),p,sizeof(*p))||
    !separate(c,sizeof(*c),f,sizeof(*f))||!separate(c,sizeof(*c),c->record,sizeof(*c->record))||
    !separate(c,sizeof(*c),c->peer,sizeof(*c->peer))||
    !separate(c,sizeof(*c),c->reader,sizeof(*c->reader))||
    !separate(f,sizeof(*f),p,sizeof(*p))||!separate(f,sizeof(*f),c->record,sizeof(*c->record))||
    !separate(c->record,sizeof(*c->record),c->peer,sizeof(*c->peer))||
    !separate(c->reader,sizeof(*c->reader),c->peer,sizeof(*c->peer))||!graph_live(c))return 0;
  if(c->store_loan->native_started||c->store_loan->copy_held||
    c->store_loan->owner->root!=o->root||
    c->store_loan->owner->root_identity.VolumeSerialNumber!=o->root_identity->VolumeSerialNumber||
    !slcli_equal(c->store_loan->owner->root_identity.FileId.Identifier,o->root_identity->FileId.Identifier,16)||
    !slcli_source_windows_graph_held(c->store_loan->owner->source_owner,c,o))return 0;
  c->reader->capture_sink=o->file_sink;c->reader->capture_original=o->archive_original;
  c->peer->held_reader->capture_sink=o->file_sink;c->peer->held_reader->capture_original=o->archive_original;
  c->peer->route_reader->capture_sink=o->file_sink;c->peer->route_reader->capture_original=o->archive_original;
  c->used=1;c->failed=1;c->store_loan->native_started=1;
  /* Full held/route image clocks are precharged before the first file birth.
   * Nested image IO keeps this SAME original transaction deadline and caller. */
  if(!slcli_peer_windows_deadline_prepare(c->arena,c->peer,o->peer,o->deadline_original,
    o,sizeof(*o),o->archive_original,o->archive_original_bytes,
    c->store_loan->owner->source_owner->observed_clock_frontier))goto done;
  if(!slcli_capture_record_deadline_prepare_unborn(c->arena,c->reader,o->deadline_original,
    o,sizeof(*o),o->archive_original,o->archive_original_bytes,
    c->store_loan->owner->source_owner->observed_clock_frontier,&c->reopened,c,sizeof(*c)))goto done;
  if(!slcli_evidence_deadline_prepare_unborn(c->arena,f,o->deadline_original,o,sizeof(*o),
    o->archive_original,o->archive_original_bytes,
    c->store_loan->owner->source_owner->observed_clock_frontier,o->root,c->store_loan->maximum,
    o->file_sink,&p->original,p,sizeof(*p)))goto done;
  if(!slcli_writer_create_windows_file(p,o->root,o->root_identity,o->leaf,o->leaf_units,
    o->descriptor,o->descriptor_bytes,o->effective_token,o->file_token,o->file_owner,
    o->file_owner_bytes,o->path_sink,o->mode_sink,o->archive_original)||p->mode.mode!=0644)goto done;
  c->store_loan->writer=p->original;
  {int clock_ok=slcli_native_clock_read(&c->writer_birth_clock);
    int joined=slcli_evidence_deadline_born(f,&p->original,&c->writer_birth_clock);
    if(!clock_ok||!joined||!p->mode.file.tick||c->writer_birth_clock.tick<p->mode.file.tick)goto done;}
  if(!slcli_evidence_begin(f,p->original,o->root,
    c->store_loan->maximum,o->file_sink,o->archive_original))goto done;
  c->record->producer_file=f;
  if(!slcli_native_clock_read(&c->peer_entry_clock)||
    c->peer_entry_clock.tick<f->deadline_owner->rows.deadline_owner->previous_tick||
    !slcli_image_deadline_precede(c->peer->held_reader,&c->peer_entry_clock)||
    !slcli_image_deadline_precede(c->peer->route_reader,&c->peer_entry_clock))goto done;
  if(!slcli_peer_capture_windows(c->peer,o->peer,slcli_peer_windows_record_sink,c->record))goto done;
  if(!c->record->complete||c->record->failed||!f->sealed||f->failed||!f->readback_original||
    !slcli_windows_directory_observe(o->root,&c->root_before)||!root_same(&c->root_before,o->root_identity))goto done;
  if(!c->root_before.tick||c->root_before.tick<f->deadline_owner->rows.deadline_owner->previous_tick||
    !slcli_native_clock_read(&c->close_entry_clock)||c->close_entry_clock.tick<c->root_before.tick||
    !slcli_evidence_deadline_precede(f,&c->close_entry_clock))goto done;
  /* Mark the producer control before the actual once-only original close.
   * Even uncertain native close prevents later IO against a recycled value. */
  f->close_started=1;
  if(!slcli_close_original(&c->writer_close,p->original)||
    !slcli_evidence_deadline_precede(f,&c->writer_close.clock))goto done;
  c->name.Length=p->name.Length;c->name.MaximumLength=c->name.Length;c->name.Buffer=p->leaf;
  c->attributes.Length=sizeof(c->attributes);c->attributes.RootDirectory=o->root;
  c->attributes.ObjectName=&c->name;c->attributes.Attributes=0x40|0x1000;
  c->open_args[0]=(uintptr_t)&c->reopened;c->open_args[1]=0x00120081;
  c->open_args[2]=(uintptr_t)&c->attributes;c->open_args[3]=(uintptr_t)&c->io;
  c->open_args[6]=FILE_SHARE_READ;c->open_args[7]=1;c->open_args[8]=0x20|0x40|0x200000;
  if(!slcli_native_clock_read(&c->reopen_entry_clock)||
    !c->peer->after_clock.complete||c->reopen_entry_clock.tick<c->peer->after_clock.tick||
    !c->root_before.tick||!c->writer_close.clock.complete||
    c->writer_close.clock.tick<c->root_before.tick||
    c->reopen_entry_clock.tick<c->writer_close.clock.tick||
    !slcli_image_deadline_precede(&c->reader->deadline_rows,&c->reopen_entry_clock))goto done;
  c->pending=1;
  c->status=NtCreateFile(&c->reopened,0x00120081,&c->attributes,&c->io,0,0,
    FILE_SHARE_READ,1,0x20|0x40|0x200000,0,0);
  if(c->status!=0||c->io.Status!=0||c->io.Information!=1)goto done;c->pending=0;
  {int clock_ok=slcli_native_clock_read(&c->reopen_birth_clock);
    int joined=slcli_capture_record_deadline_born(c->reader,&c->reopened,&c->reopen_birth_clock);
    if(!clock_ok||!joined)goto done;}
  if(!slcli_windows_source_observe(c->reopened,&c->readonly)||
    c->readonly.identity.VolumeSerialNumber!=f->readback_original->identity.VolumeSerialNumber||
    !slcli_equal(c->readonly.identity.FileId.Identifier,f->readback_original->identity.FileId.Identifier,16)||
    c->readonly.owner_sid_bytes!=f->readback_original->owner_sid_bytes||
    !slcli_equal(c->readonly.owner_sid,f->readback_original->owner_sid,c->readonly.owner_sid_bytes)||
    c->readonly.standard.EndOfFile.QuadPart<0||(uint64_t)c->readonly.standard.EndOfFile.QuadPart!=f->count||
    !slcli_windows_directory_observe(o->root,&c->root_after)||!root_same(&c->root_after,o->root_identity))goto done;
  if(!c->readonly.tick||!c->root_after.tick||
    c->readonly.tick<c->reopen_birth_clock.tick||c->root_after.tick<c->readonly.tick||
    !slcli_native_clock_read(&c->readback_entry_clock)||
    c->readback_entry_clock.tick<c->root_after.tick||
    !slcli_image_deadline_precede(&c->reader->deadline_rows,&c->readback_entry_clock))goto done;
  c->reader->producer=c->record;
  if(!slcli_capture_record_read_original(c->reader))goto done;
  /* This retirement follows actual native operations, not the capture sink's
   * return value. The immutable original raw file owns the query copy; source
   * authentication and aggregate fit still require authentic separate inputs.
   * Raw native arguments preserve the old numeric pointer as an original ABI
   * observation; subsequent byte readers must read the held copy parent. */
  if(!c->writer_close.started||!c->writer_close.observed||!c->writer_close.closed||
    c->writer_close.original!=p->original||c->pending||c->peer->pending||
    !c->reader->complete||c->reader->failed||!c->reader->before||!c->reader->after||
    c->record->query_raw!=(const unsigned char *)c->peer->path->raw||
    c->peer->path->borrower!=c->peer)goto done;
  ok=1;
done:
  /* Owning transition publication must follow real image/peer clocks, even
   * when a later source/retention check failed. Never renew its original D. */
  {struct slcli_source_windows *source=c->store_loan->owner->source_owner;
    struct slcli_image_deadline_owner *held=c->peer->held_reader->deadline_owner;
    struct slcli_image_deadline_owner *route=c->peer->route_reader->deadline_owner;
    struct slcli_image_deadline_owner *record=c->reader->deadline_rows.deadline_owner;
    struct slcli_image_deadline_owner *evidence=f->deadline_owner?f->deadline_owner->rows.deadline_owner:0;
    if(held&&held->previous_tick>source->observed_clock_frontier)
      source->observed_clock_frontier=held->previous_tick;
    if(route&&route->previous_tick>source->observed_clock_frontier)
      source->observed_clock_frontier=route->previous_tick;
    if(record&&record->previous_tick>source->observed_clock_frontier)
      source->observed_clock_frontier=record->previous_tick;
    if(evidence&&evidence->previous_tick>source->observed_clock_frontier)
      source->observed_clock_frontier=evidence->previous_tick;
    if(c->peer->after_clock.complete&&c->peer->after_clock.tick>source->observed_clock_frontier)
      source->observed_clock_frontier=c->peer->after_clock.tick;
    {const struct slcli_native_clock *clocks[]={&c->writer_birth_clock,&c->peer_entry_clock,
        &c->close_entry_clock,&c->writer_close.clock,&c->reopen_entry_clock,
        &c->reopen_birth_clock,&c->readback_entry_clock,&c->record->producer_clock};
      const slcli_input_capture *rows[]={&c->root_before,&c->readonly,&c->root_after};size_t i;
      for(i=0;i<sizeof(clocks)/sizeof(clocks[0]);i++)
        if(clocks[i]->complete&&clocks[i]->tick>source->observed_clock_frontier)
          source->observed_clock_frontier=clocks[i]->tick;
      for(i=0;i<sizeof(rows)/sizeof(rows[0]);i++)
        if(rows[i]->tick>source->observed_clock_frontier)source->observed_clock_frontier=rows[i]->tick;
    }
  }
  c->store_loan->writer=p->original;c->store_loan->readonly=c->reopened;
  c->store_loan->actual=f->count;
  /* Preserve every original, including successful close plus later failed
   * reopen/readback. No retry, deletion, actor authentication or source lease. */
  if(!sink(archive,c)||!ok)return 0;
  /* A failed transfer keeps the original query loan. Successful transfer is
   * necessary for custody, while the concrete native chain above supplies
   * the independent byte/EOF and known-close conditions for retirement. */
  c->record->query_copy_parent=c->reader;c->record->query_raw=0;c->record->query_detached=1;
  c->store_loan->readonly=c->reopened;c->store_loan->actual=f->count;c->store_loan->copy_held=1;
  c->peer->path->borrower=0;c->complete=1;c->failed=0;return 1;
}
#endif
