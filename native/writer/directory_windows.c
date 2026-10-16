#include "directory_windows.h"
#if defined(_WIN32)
#include <stddef.h>
NTSYSAPI NTSTATUS NTAPI NtQueryDirectoryFile(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,
  PIO_STATUS_BLOCK,PVOID,ULONG,FILE_INFORMATION_CLASS,BOOLEAN,PUNICODE_STRING,BOOLEAN);
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
int slcli_writer_windows_same_identity(const FILE_ID_INFO *a,const FILE_ID_INFO *b) {
  return a->VolumeSerialNumber==b->VolumeSerialNumber&&
    slcli_equal(a->FileId.Identifier,b->FileId.Identifier,16);
}
int slcli_writer_windows_same_file(const struct slcli_windows_file_capture *a,
  const struct slcli_windows_file_capture *b) {
  return a->completed&&b->completed&&slcli_writer_windows_same_identity(&a->identity,&b->identity)&&
    a->owner_sid_bytes==b->owner_sid_bytes&&slcli_equal(a->owner_sid,b->owner_sid,a->owner_sid_bytes)&&
    a->standard.Directory==b->standard.Directory&&a->standard.NumberOfLinks==b->standard.NumberOfLinks&&
    a->standard.EndOfFile.QuadPart==b->standard.EndOfFile.QuadPart&&
    a->standard.AllocationSize.QuadPart==b->standard.AllocationSize.QuadPart&&
    a->attributes.FileAttributes==b->attributes.FileAttributes&&
    a->attributes.ReparseTag==b->attributes.ReparseTag&&
    a->basic.LastWriteTime.QuadPart==b->basic.LastWriteTime.QuadPart&&
    a->basic.ChangeTime.QuadPart==b->basic.ChangeTime.QuadPart;
}
static int clock(struct slcli_writer_windows_effect *r) {
  uint64_t seconds,remainder,frequency,whole,fraction;
  r->counter_ok=QueryPerformanceCounter(&r->counter);
  r->frequency_ok=QueryPerformanceFrequency(&r->frequency);
  if(!r->counter_ok||!r->frequency_ok||r->counter.QuadPart<=0||r->frequency.QuadPart<=0)return 0;
  frequency=(uint64_t)r->frequency.QuadPart;
  seconds=(uint64_t)r->counter.QuadPart/frequency;
  remainder=(uint64_t)r->counter.QuadPart%frequency;
  if(seconds>UINT64_MAX/1000000000||remainder>UINT64_MAX/1000000000)return 0;
  whole=seconds*1000000000;fraction=remainder*1000000000/frequency;
  if(fraction>UINT64_MAX-whole)return 0;r->tick=whole+fraction;return r->tick!=0;
}
static struct slcli_writer_windows_effect *next(struct slcli_writer_windows_effect *rows,
  size_t *count,size_t capacity,unsigned operation,HANDLE original) {
  struct slcli_writer_windows_effect *r;
  if(*count>=capacity)return 0;r=&rows[(*count)++];if(r->used||r->pending)return 0;
  r->used=1;r->operation=operation;r->original=original;return r;
}
static int retain(struct slcli_writer_windows_effect *r,slcli_writer_windows_effect_sink sink,
  void *archive,const unsigned char *raw,size_t bytes) {
  int tick;if(!r)return 0;tick=clock(r);
  if(!sink(archive,r,raw,bytes))return 0;r->retained=1;return tick;
}
static int observe(struct slcli_writer_windows_effect *r,int directory,
  slcli_writer_windows_effect_sink sink,void *archive) {
  int ok;if(!r)return 0;
  ok=directory?slcli_windows_directory_observe(r->original,&r->file):
    slcli_windows_file_observe(r->original,&r->file);
  r->pending=r->file.retained_native_io;r->complete=(uint32_t)ok;
  return retain(r,sink,archive,0,0)&&ok;
}
static int open(struct slcli_writer_windows_effect *r,HANDLE parent,
  WCHAR *leaf,size_t units,int directory,slcli_writer_windows_effect_sink sink,void *archive) {
  if(!r||!units||units>255)return 0;
  r->name.Length=(USHORT)(2*units);r->name.MaximumLength=r->name.Length;r->name.Buffer=leaf;
  r->attributes.Length=sizeof(r->attributes);r->attributes.RootDirectory=parent;
  r->attributes.ObjectName=&r->name;r->attributes.Attributes=0x40|0x1000;
  r->args[0]=(uintptr_t)&r->opened;r->args[1]=FILE_READ_DATA|FILE_READ_ATTRIBUTES|READ_CONTROL|SYNCHRONIZE;
  r->args[2]=(uintptr_t)&r->attributes;r->args[3]=(uintptr_t)&r->io;
  r->args[6]=FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;r->args[7]=1;
  r->args[8]=0x20|0x200000|(directory?0x1:0x40);r->pending=1;
  r->status=NtCreateFile(&r->opened,(ACCESS_MASK)r->args[1],&r->attributes,&r->io,
    0,0,(ULONG)r->args[6],1,(ULONG)r->args[8],0,0);
  if(r->status==0&&r->io.Status==0&&r->io.Information==1){r->pending=0;r->complete=1;}
  return retain(r,sink,archive,0,0)&&r->complete;
}
struct extd_entry {
  ULONG next,index;
  LARGE_INTEGER creation,access,write,change,eof,allocation;
  ULONG attributes,name_bytes,ea,reparse;
  FILE_ID_128 identity;
  WCHAR name[1];
};
_Static_assert(offsetof(struct extd_entry,name)==88,"native FileIdExtdDirectoryInformation layout");
int slcli_writer_discover_windows_original(struct slcli_writer_windows_discovery *d) {
  struct slcli_writer_windows_effect *before,*after,*r,*held,*route,*namespace;
  size_t i,j,at,n,step,units;ULONG got;unsigned matches;int first=1;
  if(!d||d->used||!d->original||d->original==INVALID_HANDLE_VALUE||!d->children||
    d->child_count>128||!d->captures||d->capture_count||
    d->capture_capacity<134+4*d->child_count||d->capture_capacity>SIZE_MAX/sizeof(*d->captures)||
    !d->scratch||d->scratch_bytes!=1024||(uintptr_t)d->scratch%8||!d->sink||!d->archive_original||
    !separate(d,sizeof(*d),d->captures,d->capture_capacity*sizeof(*d->captures))||
    !separate(d,sizeof(*d),d->scratch,1024)||
    !separate(d->captures,d->capture_capacity*sizeof(*d->captures),d->scratch,1024)||
    !separate(d->children,d->child_count*sizeof(*d->children),d->scratch,1024)||
    !separate(d->children,d->child_count*sizeof(*d->children),d,sizeof(*d))||
    !separate(d->children,d->child_count*sizeof(*d->children),d->captures,
      d->capture_capacity*sizeof(*d->captures)))return 0;
  for(i=0;i<d->child_count;i++) {
    struct slcli_writer_windows_child *c=&d->children[i];
    if(c->seen||c->baseline||!c->created||!c->created->complete||c->created->failed||
      c->created->retained_native_io||!c->created->name.Length||c->created->name.Length%2)return 0;
    if(!separate(c->created,sizeof(*c->created),d->scratch,1024)||
      !separate(c->created,sizeof(*c->created),d->captures,d->capture_capacity*sizeof(*d->captures)))return 0;
    for(j=0;j<i;j++)if(c->created==d->children[j].created||
      slcli_writer_windows_same_identity(&c->created->mode.file.identity,
        &d->children[j].created->mode.file.identity)||
      (c->created->name.Length==d->children[j].created->name.Length&&
        slcli_equal((void *)c->created->leaf,(void *)d->children[j].created->leaf,c->created->name.Length)))return 0;
  }
  d->used=1;d->failed=1;
  before=next(d->captures,&d->capture_count,d->capture_capacity,1,d->original);
  if(!observe(before,1,d->sink,d->archive_original))return 0;
  for(i=0;i<d->child_count;i++) {
    struct slcli_writer_windows_child *c=&d->children[i];
    held=next(d->captures,&d->capture_count,d->capture_capacity,1,c->created->original);
    if(!observe(held,c->created->directory,d->sink,d->archive_original)||
      !slcli_writer_windows_same_identity(&held->file.identity,&c->created->mode.file.identity)||
      held->file.owner_sid_bytes!=c->created->mode.file.owner_sid_bytes||
      !slcli_equal(held->file.owner_sid,c->created->mode.file.owner_sid,held->file.owner_sid_bytes)||
      (!c->created->directory&&held->file.standard.NumberOfLinks!=1))return 0;
    c->baseline=&held->file;
  }
  for(;;) {
    r=next(d->captures,&d->capture_count,d->capture_capacity,2,d->original);if(!r)return 0;
    r->args[0]=(uintptr_t)d->original;r->args[4]=(uintptr_t)&r->io;
    r->args[5]=(uintptr_t)d->scratch;r->args[6]=1024;r->args[7]=60;r->args[10]=first;
    r->pending=1;
    r->status=NtQueryDirectoryFile(d->original,0,0,0,&r->io,d->scratch,1024,
      (FILE_INFORMATION_CLASS)60,FALSE,0,(BOOLEAN)first);first=0;
    /* STATUS_NO_MORE_FILES is the only accepted terminal empty native result.
     * Overflow, partial, pending and unknown remain held; no synthetic EOF. */
    if((r->status==0&&r->io.Status==0)||
       ((ULONG)r->status==0x80000006&&(ULONG)r->io.Status==0x80000006&&!r->io.Information)) {
      r->pending=0;r->complete=1;
    }
    n=r->io.Information<=1024?(size_t)r->io.Information:0;
    if(!retain(r,d->sink,d->archive_original,d->scratch,n)||!r->complete||r->io.Information>1024)return 0;
    if((ULONG)r->status==0x80000006){d->eof=1;break;}
    got=(ULONG)r->io.Information;if(!got||d->bytes>131072||got>131072-d->bytes)return 0;d->bytes+=got;
    for(at=0;at<got;at+=step) {
      const struct extd_entry *e;struct slcli_writer_windows_child *c=0;
      if(got-at<88||at%8)return 0;e=(const void *)(d->scratch+at);
      if(!e->name_bytes||e->name_bytes%2||e->name_bytes>510||e->name_bytes>got-at-88||
        ++d->records>130)return 0;
      units=e->name_bytes/2;step=e->next?e->next:got-at;
      if(step<88+e->name_bytes||step>got-at||(e->next&&step%8))return 0;
      if((units==1&&e->name[0]=='.')||(units==2&&e->name[0]=='.'&&e->name[1]=='.'))continue;
      matches=0;
      for(i=0;i<d->child_count;i++) {
        struct slcli_writer_windows_child *candidate=&d->children[i];
        if(candidate->created->name.Length==e->name_bytes&&
          slcli_equal((void *)candidate->created->leaf,(void *)e->name,e->name_bytes)) {
          if(candidate->seen||++matches!=1)return 0;c=candidate;
        }
      }
      if(matches!=1||!c||e->attributes&FILE_ATTRIBUTE_REPARSE_POINT||e->reparse||
        !!(e->attributes&FILE_ATTRIBUTE_DIRECTORY)!=!!c->created->directory||
        !slcli_equal(e->identity.Identifier,c->baseline->identity.FileId.Identifier,16))return 0;
      held=next(d->captures,&d->capture_count,d->capture_capacity,1,c->created->original);
      if(!observe(held,c->created->directory,d->sink,d->archive_original)||
        !slcli_writer_windows_same_file(&held->file,c->baseline))return 0;
      route=next(d->captures,&d->capture_count,d->capture_capacity,3,d->original);
      if(!open(route,d->original,c->created->leaf,units,c->created->directory,d->sink,d->archive_original))return 0;
      namespace=next(d->captures,&d->capture_count,d->capture_capacity,1,route->opened);
      if(!observe(namespace,c->created->directory,d->sink,d->archive_original)||
        !slcli_writer_windows_same_file(&namespace->file,c->baseline))return 0;c->seen=1;
    }
  }
  for(i=0;i<d->child_count;i++)if(!d->children[i].seen)return 0;
  after=next(d->captures,&d->capture_count,d->capture_capacity,1,d->original);
  if(!observe(after,1,d->sink,d->archive_original)||
    !slcli_writer_windows_same_file(&before->file,&after->file))return 0;
  d->complete=1;d->failed=0;return 1;
}
int slcli_writer_publish_windows_directory(struct slcli_writer_windows_publish *p) {
  struct slcli_writer_windows_path *c;struct slcli_writer_windows_effect *r,*before,*parent,*after;
  struct rename_information { BOOLEAN replace;HANDLE root;ULONG bytes;WCHAR name[1]; };
  struct rename_information *rename;size_t i,offset=offsetof(struct rename_information,name);
  if(!p||p->used||!(c=p->candidate)||!c->directory||!c->complete||c->failed||c->retained_native_io||
    !p->admitted_parent_identity||!p->final_units||p->final_units>255||!p->sink||!p->archive_original||
    !separate(p,sizeof(*p),c,sizeof(*c))||
    !separate(p,sizeof(*p),p->admitted_parent_identity,sizeof(*p->admitted_parent_identity)))return 0;
  for(i=0;i<p->final_units;i++)if(!p->final_leaf[i]||p->final_leaf[i]=='/'||
    p->final_leaf[i]=='\\'||p->final_leaf[i]==':')return 0;
  for(i=0;i<p->final_units;i++) {
    if(p->final_leaf[i]>=0xdc00&&p->final_leaf[i]<=0xdfff)return 0;
    if(p->final_leaf[i]>=0xd800&&p->final_leaf[i]<=0xdbff) {
      if(i+1==p->final_units||p->final_leaf[i+1]<0xdc00||p->final_leaf[i+1]>0xdfff)return 0;i++;
    }
  }
  if((p->final_units==1&&p->final_leaf[0]=='.')||
    (p->final_units==2&&p->final_leaf[0]=='.'&&p->final_leaf[1]=='.'))return 0;
  if(c->name.Length==2*p->final_units&&slcli_equal((void *)c->leaf,(void *)p->final_leaf,c->name.Length))return 0;
  p->used=1;p->failed=1;
  parent=next(p->effects,&p->count,10,1,c->parent);
  if(!observe(parent,1,p->sink,p->archive_original)||
    !slcli_writer_windows_same_identity(&parent->file.identity,p->admitted_parent_identity))return 0;
  before=next(p->effects,&p->count,10,1,c->original);
  if(!observe(before,1,p->sink,p->archive_original)||
    !slcli_writer_windows_same_identity(&before->file.identity,&c->mode.file.identity))return 0;
  r=next(p->effects,&p->count,10,3,c->parent);
  if(!open(r,c->parent,c->leaf,c->name.Length/2,1,p->sink,p->archive_original))return 0;
  p->route_original=r->opened;r=next(p->effects,&p->count,10,1,p->route_original);
  if(!observe(r,1,p->sink,p->archive_original)||!slcli_writer_windows_same_file(&before->file,&r->file))return 0;
  rename=(void *)p->rename_raw;p->rename_bytes=offset+2*p->final_units;
  if(p->rename_bytes>sizeof(p->rename_raw))return 0;
  rename->replace=FALSE;rename->root=c->parent;rename->bytes=(ULONG)(2*p->final_units);
  for(i=0;i<p->final_units;i++)rename->name[i]=p->final_leaf[i];
  r=next(p->effects,&p->count,10,4,c->original);if(!r)return 0;
  r->args[0]=(uintptr_t)c->original;r->args[1]=(uintptr_t)&r->io;
  r->args[2]=(uintptr_t)p->rename_raw;r->args[3]=p->rename_bytes;r->args[4]=10;r->pending=1;
  r->status=NtSetInformationFile(c->original,&r->io,p->rename_raw,(ULONG)p->rename_bytes,(FILE_INFORMATION_CLASS)10);
  if(r->status==0&&r->io.Status==0){r->pending=0;r->complete=1;}
  if(!retain(r,p->sink,p->archive_original,(void *)p->rename_raw,p->rename_bytes)||!r->complete)return 0;
  r=next(p->effects,&p->count,10,5,c->parent);if(!r)return 0;
  r->args[0]=(uintptr_t)c->parent;r->result=FlushFileBuffers(c->parent);
  if(!r->result)r->error=GetLastError();else r->complete=1;
  if(!retain(r,p->sink,p->archive_original,0,0)||!r->complete)return 0;
  r=next(p->effects,&p->count,10,3,c->parent);
  if(!open(r,c->parent,p->final_leaf,p->final_units,1,p->sink,p->archive_original))return 0;p->reopened=r->opened;
  after=next(p->effects,&p->count,10,1,p->reopened);
  /* Rename changes ChangeTime: retain both original observations and compare
   * stable identity/owner/type here; repeated final discovery follows in build. */
  if(!observe(after,1,p->sink,p->archive_original)||
    !slcli_writer_windows_same_identity(&before->file.identity,&after->file.identity)||
    before->file.owner_sid_bytes!=after->file.owner_sid_bytes||
    !slcli_equal(before->file.owner_sid,after->file.owner_sid,before->file.owner_sid_bytes)||
    before->file.attributes.FileAttributes!=after->file.attributes.FileAttributes)return 0;
  r=next(p->effects,&p->count,10,1,c->original);
  if(!observe(r,1,p->sink,p->archive_original)||
    !slcli_writer_windows_same_file(&after->file,&r->file))return 0;
  r=next(p->effects,&p->count,10,1,c->parent);
  if(!observe(r,1,p->sink,p->archive_original)||
    !slcli_writer_windows_same_identity(&r->file.identity,p->admitted_parent_identity)||
    parent->file.owner_sid_bytes!=r->file.owner_sid_bytes||
    !slcli_equal(parent->file.owner_sid,r->file.owner_sid,parent->file.owner_sid_bytes))return 0;
  p->complete=1;p->failed=0;return 1;
}
#endif
