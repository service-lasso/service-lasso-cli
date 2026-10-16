#include "materialize_windows.h"
#if defined(_WIN32)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_writer_materialize_windows_originals(struct slcli_writer_windows_materialization *m) {
  struct slcli_writer_attempt *a;size_t i,j;unsigned mode;
  if(!m||m->used||!(a=m->attempt)||!a->complete||a->failed||!a->original||
    a->original->consumed!=1||!m->outputs||m->count!=a->count||!m->count||m->count>128||
    !m->admitted_token||!m->admitted_owner||!m->owner_bytes||m->owner_bytes>184||
    !m->mode_sink||!m->archive_original||
    !separate(m,sizeof(*m),a->original->body,a->original->bytes)||
    !separate(m->outputs,m->count*sizeof(*m->outputs),a->original->body,a->original->bytes)||
    !separate(m->outputs,m->count*sizeof(*m->outputs),m->admitted_token,sizeof(*m->admitted_token))||
    !separate(m->outputs,m->count*sizeof(*m->outputs),m->admitted_owner,m->owner_bytes)||
    !separate(m,sizeof(*m),m->admitted_token,sizeof(*m->admitted_token))||
    !separate(m,sizeof(*m),m->admitted_owner,m->owner_bytes)||
    !separate(m,sizeof(*m),m->outputs,m->count*sizeof(*m->outputs)))return 0;
  for(i=0;i<m->count;i++) {
    struct slcli_writer_windows_output *o=&m->outputs[i];
    if(o->used||!o->file||!o->file->started||o->file->failed||o->file->sealed||
      !slcli_writer_mode_bound(&o->before,o->file)||!slcli_writer_mode_bound(&o->after,o->file)||
      o->before.used||o->after.used||
      o->before.bound_effective!=m->effective_original||o->after.bound_effective!=m->effective_original||
      o->before.bound_token!=m->admitted_token||o->after.bound_token!=m->admitted_token||
      o->before.bound_owner!=m->admitted_owner||o->after.bound_owner!=m->admitted_owner||
      o->before.bound_owner_bytes!=m->owner_bytes||o->after.bound_owner_bytes!=m->owner_bytes||
      o->before.bound_sink!=m->mode_sink||o->after.bound_sink!=m->mode_sink||
      o->before.bound_archive!=m->archive_original||o->after.bound_archive!=m->archive_original||
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
    struct slcli_writer_windows_output *o=&m->outputs[i];
    struct slcli_writer_entry *entry=&a->entries[i];o->used=1;o->failed=1;
    mode=entry->mode?0755:0644;
    if(!slcli_writer_observe_windows_mode(&o->before,o->file->original,m->effective_original,
      m->admitted_token,m->admitted_owner,m->owner_bytes,m->mode_sink,m->archive_original)||
      o->before.mode!=mode||o->before.file.standard.EndOfFile.QuadPart!=0)return 0;
    if(entry->body.bytes&&!slcli_evidence_append(o->file,entry->body.raw,entry->body.bytes))return 0;
    if(!slcli_evidence_flush_readback(o->file)||o->file->count!=entry->body.bytes||
      !slcli_writer_observe_windows_mode(&o->after,o->file->original,m->effective_original,
        m->admitted_token,m->admitted_owner,m->owner_bytes,m->mode_sink,m->archive_original)||
      o->after.mode!=mode||o->after.file.standard.EndOfFile.QuadPart<0||
      (uint64_t)o->after.file.standard.EndOfFile.QuadPart!=entry->body.bytes||
      o->before.file.identity.VolumeSerialNumber!=o->after.file.identity.VolumeSerialNumber||
      !slcli_equal(o->before.file.identity.FileId.Identifier,o->after.file.identity.FileId.Identifier,16)||
      o->before.security_bytes!=o->after.security_bytes||
      !slcli_equal((void *)o->before.security,(void *)o->after.security,o->before.security_bytes))return 0;
    o->complete=1;o->failed=0;
  }
  m->complete=1;m->failed=0;return 1;
}
#endif
