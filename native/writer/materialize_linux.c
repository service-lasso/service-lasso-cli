#include "materialize_linux.h"
#if defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int identity(const struct slcli_linux_file_capture *a,
                    const struct slcli_linux_file_capture *b) {
  return a->original_stat.st_dev==b->original_stat.st_dev&&
    a->original_stat.st_ino==b->original_stat.st_ino&&
    a->original_stat.st_uid==b->original_stat.st_uid&&
    a->original_stat.st_gid==b->original_stat.st_gid&&
    a->original_stat.st_mode==b->original_stat.st_mode&&
    a->original_stat.st_nlink==b->original_stat.st_nlink;
}
static int observe(struct slcli_writer_output_original *o,
                   struct slcli_linux_file_capture *r,unsigned mode) {
  int ok=slcli_linux_file_observe(o->file->original,r);
  if(!o->file->capture_sink(o->file->capture_original,r,0,0)||!ok)return 0;
  return (r->original_stat.st_mode&0177777)==(0100000|mode)&&
    r->original_stat.st_uid==o->admitted_uid&&
    r->original_stat.st_gid==o->admitted_gid&&r->original_stat.st_nlink==1;
}
int slcli_writer_materialize_originals(struct slcli_writer_materialization *m) {
  size_t i,j;struct slcli_writer_attempt *a;
  if(!m||m->used||!m->outputs||!(a=m->attempt)||!a->complete||a->failed||
     !a->original||a->original->consumed!=1||m->count!=a->count||!m->count||m->count>128||
     !separate(m,sizeof(*m),a->original->body,a->original->bytes)||
     !separate(m->outputs,m->count*sizeof(*m->outputs),a->original->body,a->original->bytes)||
     !separate(m,sizeof(*m),m->outputs,m->count*sizeof(*m->outputs)))return 0;
  /* Validate every route before any file write. Path equality checks the
   * admitted mapping, never manufactures native directory authority. */
  for(i=0;i<m->count;i++) {
    struct slcli_writer_output_original *o=&m->outputs[i];
    if(o->used||!o->file||!o->file->started||o->file->failed||o->file->sealed||
       o->file->count||!o->file->capture_sink||!o->file->capture_original||
       !o->file->captures||o->file->capture_capacity>SIZE_MAX/sizeof(*o->file->captures)||
       !separate(o->file,sizeof(*o->file),a->original->body,a->original->bytes)||
       !separate(o->file->captures,o->file->capture_capacity*sizeof(*o->file->captures),
                 a->original->body,a->original->bytes)||
       o->admitted_path.bytes!=a->entries[i].path.bytes||
       !slcli_equal(o->admitted_path.raw,a->entries[i].path.raw,o->admitted_path.bytes))return 0;
    for(j=0;j<i;j++)if(m->outputs[j].file==o->file||
      m->outputs[j].file->original==o->file->original)return 0;
  }
  m->used=1;m->failed=1;
  for(i=0;i<m->count;i++) {
    struct slcli_writer_output_original *o=&m->outputs[i];
    struct slcli_writer_entry *entry=&a->entries[i];
    unsigned mode=entry->mode?0755:0644;
    o->used=1;o->failed=1;
    if(!observe(o,&o->before,mode)||o->before.original_stat.st_size)return 0;
    if(entry->body.bytes&&!slcli_evidence_append(o->file,entry->body.raw,entry->body.bytes))return 0;
    if(!slcli_evidence_flush_readback(o->file)||o->file->count!=entry->body.bytes||
       !observe(o,&o->after,mode)||!identity(&o->before,&o->after)||
       o->after.original_stat.st_size<0||
       (uint64_t)o->after.original_stat.st_size!=entry->body.bytes)return 0;
    o->complete=1;o->failed=0;
  }
  m->complete=1;m->failed=0;return 1;
}
#endif
