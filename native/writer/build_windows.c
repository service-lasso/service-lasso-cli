#include "build_windows.h"
#if defined(_WIN32)
static int same_path(struct slcli_slice a,struct slcli_slice b) {
  return a.bytes==b.bytes&&slcli_equal(a.raw,b.raw,a.bytes);
}
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int route(struct slcli_writer_windows_build *b,struct slcli_writer_windows_route *r,size_t bound) {
  size_t i,at=0;struct slcli_slice parent;
  if(!r->created||r->created->used||!r->path.raw||!r->path.bytes||!r->leaf.raw||
    !r->leaf.bytes||r->leaf.bytes>255||r->parent_directory>bound||
    !r->descriptor||!r->descriptor_bytes||r->descriptor_bytes>sizeof(r->created->descriptor))return 0;
  for(i=0;i<r->path.bytes;i++)if(r->path.raw[i]=='/')at=i+1;
  if(r->path.bytes-at!=r->leaf.bytes||!slcli_equal(r->path.raw+at,r->leaf.raw,r->leaf.bytes))return 0;
  /* Entry leaves already passed unchanged selected portable ASCII policy in
   * the SAME input decoder. Unicode destination is independently preserved. */
  for(i=0;i<r->leaf.bytes;i++)if(!r->leaf.raw[i]||r->leaf.raw[i]>127||
    r->leaf.raw[i]=='/'||r->leaf.raw[i]=='\\'||r->leaf.raw[i]==':')return 0;
  parent.raw=r->path.raw;parent.bytes=at?at-1:0;
  if(!r->parent_directory)return !parent.bytes;
  return same_path(parent,b->directories[r->parent_directory-1].path);
}
static int destination(struct slcli_writer_windows_build *b) {
  struct slcli_slice s=b->attempt->destination;size_t i,start=0,units=0;uint32_t scalar;unsigned extra;
  if(!same_path(s,b->admitted_destination)||!s.raw||!s.bytes||!b->publish||
    b->publish->used||!slcli_utf8(s.raw,s.bytes,0))return 0;
  for(i=0;i<s.bytes;i++)if(s.raw[i]=='/'||s.raw[i]=='\\')start=i+1;
  if(start==s.bytes)return 0;
  /* Strict scalar conversion of exact original destination leaf, no locale,
   * replacement characters, code-page cast, normalization or authority inference. */
  for(i=start;i<s.bytes;) {
    scalar=s.raw[i++];extra=0;
    if(scalar>=0xf0){scalar&=7;extra=3;}
    else if(scalar>=0xe0){scalar&=15;extra=2;}
    else if(scalar>=0xc0){scalar&=31;extra=1;}
    while(extra--){if(i==s.bytes)return 0;scalar=(scalar<<6)|(s.raw[i++]&63);}
    if(!scalar||scalar==':'||scalar=='/'||scalar=='\\')return 0;
    if(scalar<0x10000) { if(units>=255)return 0;b->publish->final_leaf[units++]=(WCHAR)scalar; }
    else {
      if(units>253)return 0;scalar-=0x10000;
      b->publish->final_leaf[units++]=(WCHAR)(0xd800+(scalar>>10));
      b->publish->final_leaf[units++]=(WCHAR)(0xdc00+(scalar&1023));
    }
  }
  b->publish->final_leaf[units]=0;b->publish->final_units=units;return units!=0;
}
static int discover(struct slcli_writer_windows_build *b,size_t index,size_t directory,HANDLE original) {
  struct slcli_writer_windows_discovery *d=&b->discovery[index];size_t i,count=0;
  for(i=0;i<b->directory_count+b->file_count;i++) {
    struct slcli_writer_windows_route *r=i<b->directory_count?&b->directories[i]:&b->files[i-b->directory_count];
    struct slcli_writer_windows_child *c;
    if(r->parent_directory!=directory)continue;if(count>=d->child_count)return 0;
    c=&d->children[count++];if(c->seen||c->baseline||c->created)return 0;c->created=r->created;
  }
  if(count!=d->child_count)return 0;d->original=original;
  return slcli_writer_discover_windows_original(d);
}
int slcli_writer_build_windows_original(struct slcli_writer_windows_build *b) {
  struct slcli_writer_attempt *a;size_t i,j;int required;
  if(!b||b->used||!(a=b->attempt)||!a->complete||a->failed||!a->original||a->original->consumed!=1||
    !b->admitted_parent||b->admitted_parent==INVALID_HANDLE_VALUE||!b->admitted_parent_identity||
    !b->effective_original||b->effective_original==INVALID_HANDLE_VALUE||!b->admitted_token||
    !b->admitted_owner||!b->owner_bytes||b->owner_bytes>184||!b->candidate_descriptor||
    !b->candidate_descriptor_bytes||b->candidate_descriptor_bytes>512||!b->candidate_leaf||
    !b->candidate_units||b->candidate_units>255||!b->candidate||b->candidate->used||
    !b->directories||!b->files||!b->materialize||!b->materialize->outputs||b->materialize->used||
    b->directory_count>127||b->file_count!=a->count||!b->file_count||b->file_count>128||
    b->directory_count+b->file_count+4>128||b->materialize->count!=b->file_count||
    b->materialize->attempt!=a||b->materialize->effective_original!=b->effective_original||
    b->materialize->admitted_token!=b->admitted_token||b->materialize->admitted_owner!=b->admitted_owner||
    b->materialize->owner_bytes!=b->owner_bytes||!b->discovery||
    b->discovery_count!=2*(b->directory_count+1)||!b->publish||b->publish->used||b->publish->count||
    !b->publish->sink||!b->publish->archive_original||!b->path_sink||!b->file_sink||
    !b->mode_sink||!b->archive_original)return 0;
  /* All routing, object distinctness and original input/control separation is
   * checked before the first exclusive native construction. */
  if(!separate(b,sizeof(*b),a->original->body,a->original->bytes)||
    !separate(b->directories,b->directory_count*sizeof(*b->directories),a->original->body,a->original->bytes)||
    !separate(b->files,b->file_count*sizeof(*b->files),a->original->body,a->original->bytes)||
    !separate(b->directories,b->directory_count*sizeof(*b->directories),b->files,b->file_count*sizeof(*b->files))||
    !separate(b->candidate,sizeof(*b->candidate),a->original->body,a->original->bytes)||
    !separate(b->publish,sizeof(*b->publish),a->original->body,a->original->bytes)||
    !separate(b->candidate,sizeof(*b->candidate),b->publish,sizeof(*b->publish)))return 0;
  for(i=0;i<b->directory_count;i++) {
    struct slcli_writer_windows_route *r=&b->directories[i];
    if(!route(b,r,i)||r->created==b->candidate)return 0;required=0;
    for(j=0;j<a->count;j++)if(a->entries[j].path.bytes>r->path.bytes&&
      a->entries[j].path.raw[r->path.bytes]=='/'&&
      slcli_equal(a->entries[j].path.raw,r->path.raw,r->path.bytes))required=1;
    if(!required)return 0;
    for(j=0;j<i;j++)if(same_path(r->path,b->directories[j].path)||r->created==b->directories[j].created)return 0;
  }
  for(i=0;i<b->file_count;i++) {
    struct slcli_writer_windows_route *r=&b->files[i];
    if(!route(b,r,b->directory_count)||r->created==b->candidate||r->output!=&b->materialize->outputs[i]||
      !r->output->file||r->output->used||r->output->file->started||
      !same_path(r->path,a->entries[i].path)||!same_path(r->path,r->output->admitted_path))return 0;
    for(j=0;j<i;j++)if(r->created==b->files[j].created||r->output->file==b->files[j].output->file)return 0;
    for(j=0;j<b->directory_count;j++)if(r->created==b->directories[j].created)return 0;
  }
  for(i=0;i<b->discovery_count;i++) {
    size_t directory=i%(b->directory_count+1),children=0;
    struct slcli_writer_windows_discovery *d=&b->discovery[i];
    for(j=0;j<b->directory_count;j++)if(b->directories[j].parent_directory==directory)children++;
    for(j=0;j<b->file_count;j++)if(b->files[j].parent_directory==directory)children++;
    if(d->used||d->capture_count||!d->children||d->child_count!=children||!d->captures||
      d->capture_capacity<134+4*children||d->capture_capacity>SIZE_MAX/sizeof(*d->captures)||
      !d->scratch||d->scratch_bytes!=1024||(uintptr_t)d->scratch%8||!d->sink||!d->archive_original)return 0;
    for(j=0;j<children;j++)if(d->children[j].created||d->children[j].seen||d->children[j].baseline)return 0;
    for(j=0;j<d->capture_capacity;j++)if(d->captures[j].used||d->captures[j].pending)return 0;
    for(j=0;j<i;j++) {
      struct slcli_writer_windows_discovery *prior=&b->discovery[j];
      if(!separate(d->captures,d->capture_capacity*sizeof(*d->captures),prior->captures,
        prior->capture_capacity*sizeof(*prior->captures))||
        !separate(d->children,children*sizeof(*d->children),prior->children,
          prior->child_count*sizeof(*prior->children)))return 0;
    }
  }
  for(i=0;i<b->directory_count+b->file_count;i++) {
    struct slcli_writer_windows_route *r=i<b->directory_count?&b->directories[i]:&b->files[i-b->directory_count];
    if(!separate(r->native_leaf,sizeof(r->native_leaf),r->descriptor,r->descriptor_bytes)||
      !separate(r->native_leaf,sizeof(r->native_leaf),r->leaf.raw,r->leaf.bytes)||
      !separate(r->created,sizeof(*r->created),a->original->body,a->original->bytes)||
      !separate(r->created,sizeof(*r->created),b->directories,b->directory_count*sizeof(*b->directories))||
      !separate(r->created,sizeof(*r->created),b->files,b->file_count*sizeof(*b->files))||
      !separate(r->created,sizeof(*r->created),b->candidate,sizeof(*b->candidate))||
      !separate(r->created,sizeof(*r->created),b->publish,sizeof(*b->publish)))return 0;
    for(j=0;j<i;j++) {
      struct slcli_writer_windows_route *prior=j<b->directory_count?&b->directories[j]:&b->files[j-b->directory_count];
      if(!separate(r->created,sizeof(*r->created),prior->created,sizeof(*prior->created)))return 0;
    }
  }
  for(i=0;i<b->directory_count+b->file_count;i++) {
    struct slcli_writer_windows_route *r=i<b->directory_count?&b->directories[i]:&b->files[i-b->directory_count];
    for(j=0;j<r->leaf.bytes;j++)r->native_leaf[j]=(WCHAR)r->leaf.raw[j];
    r->native_leaf[r->leaf.bytes]=0;
  }
  if(!destination(b))return 0;
  b->used=1;b->failed=1;
  if(!slcli_writer_create_windows_directory(b->candidate,b->admitted_parent,b->admitted_parent_identity,
    b->candidate_leaf,b->candidate_units,b->candidate_descriptor,b->candidate_descriptor_bytes,
    b->effective_original,b->admitted_token,b->admitted_owner,b->owner_bytes,b->path_sink,
    b->mode_sink,b->archive_original))return 0;
  for(i=0;i<b->directory_count+b->file_count;i++) {
    struct slcli_writer_windows_route *r=i<b->directory_count?&b->directories[i]:&b->files[i-b->directory_count];
    struct slcli_writer_windows_path *parent=r->parent_directory?
      b->directories[r->parent_directory-1].created:b->candidate;
    /* SAME precharged route leaf survives every original native result. */
    if(!(i<b->directory_count?
      slcli_writer_create_windows_directory(r->created,parent->original,&parent->mode.file.identity,
        r->native_leaf,r->leaf.bytes,r->descriptor,r->descriptor_bytes,b->effective_original,b->admitted_token,
        b->admitted_owner,b->owner_bytes,b->path_sink,b->mode_sink,b->archive_original):
      slcli_writer_create_windows_file(r->created,parent->original,&parent->mode.file.identity,
        r->native_leaf,r->leaf.bytes,r->descriptor,r->descriptor_bytes,b->effective_original,b->admitted_token,
        b->admitted_owner,b->owner_bytes,b->path_sink,b->mode_sink,b->archive_original)))return 0;
    if(i>=b->directory_count&&!slcli_writer_begin_windows_created_file(r->created,r->output,
      a->entries[i-b->directory_count].body.bytes?a->entries[i-b->directory_count].body.bytes:1,
      b->file_sink,b->archive_original))return 0;
  }
  if(!slcli_writer_materialize_windows_originals(b->materialize))return 0;
  for(i=0;i<b->directory_count+1;i++)if(!discover(b,i,i,
    i?b->directories[i-1].created->original:b->candidate->original))return 0;
  b->publish->candidate=b->candidate;b->publish->admitted_parent_identity=b->admitted_parent_identity;
  if(!slcli_writer_publish_windows_directory(b->publish))return 0;
  for(i=0;i<b->directory_count+1;i++)if(!discover(b,b->directory_count+1+i,i,
    i?b->directories[i-1].created->original:b->publish->reopened))return 0;
  b->complete=1;b->failed=0;return 1;
}
#endif
