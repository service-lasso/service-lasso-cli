#include "build_linux.h"
#if defined(__linux__)
static int same_path(struct slcli_slice a,struct slcli_slice b) {
  return a.bytes==b.bytes&&slcli_equal(a.raw,b.raw,a.bytes);
}
static int route(struct slcli_writer_linux_build *b,struct slcli_writer_linux_route *r,size_t bound) {
  struct slcli_slice parent;size_t i,at=0;
  if(!r->created||r->created->used||!r->path.raw||!r->path.bytes||
    !r->leaf.raw||!r->leaf.bytes||r->leaf.bytes>255||r->parent_directory>bound)return 0;
  for(i=0;i<r->path.bytes;i++)if(r->path.raw[i]=='/')at=i+1;
  if(r->path.bytes-at!=r->leaf.bytes||!slcli_equal(r->path.raw+at,r->leaf.raw,r->leaf.bytes))return 0;
  parent.raw=r->path.raw;parent.bytes=at?at-1:0;
  if(!r->parent_directory)return !parent.bytes;
  return same_path(parent,b->directories[r->parent_directory-1].path);
}
static int discover(struct slcli_writer_linux_build *b,size_t index,size_t directory,int fd) {
  struct slcli_writer_discovery *d=&b->discovery[index];size_t i,count=0;
  if(d->used||d->capture_count||!d->children)return 0;
  for(i=0;i<b->directory_count+b->file_count;i++) {
    struct slcli_writer_linux_route *r=i<b->directory_count?&b->directories[i]:&b->files[i-b->directory_count];
    struct slcli_writer_discovery_child *c;
    if(r->parent_directory!=directory)continue;if(count>=d->child_count)return 0;
    c=&d->children[count++];if(c->seen||c->baseline)return 0;
    c->original=r->created->original;c->leaf=r->created->name;c->bytes=r->leaf.bytes;
    c->admitted_identity=r->created->effects[r->created->count-2].raw;
  }
  if(count!=d->child_count)return 0;d->original=fd;
  return slcli_writer_discover_linux_original(d);
}
int slcli_writer_build_linux_original(struct slcli_writer_linux_build *b) {
  struct slcli_writer_attempt *a;size_t i,j,start,n;int parent,required;const struct stat *identity;
  if(!b||b->used||!(a=b->attempt)||!a->complete||a->failed||!a->original||
    a->original->consumed!=1||b->admitted_parent<0||!b->admitted_parent_identity||
    !b->candidate||b->candidate->used||!b->candidate_leaf||!b->final_leaf||
    !b->directories||!b->files||!b->materialize||!b->materialize->outputs||
    b->directory_count>127||b->file_count!=a->count||!b->file_count||b->file_count>128||
    b->directory_count+b->file_count+4>128||!b->discovery||
    b->discovery_count!=2*(b->directory_count+1)||!b->path_sink||!b->file_sink||
    !b->archive_original||b->materialize->used||b->materialize->count!=b->file_count||
    b->materialize->attempt!=a)return 0;
  if(!same_path(b->admitted_destination,a->destination)||a->destination.bytes<2||
    a->destination.raw[0]!='/')return 0;
  for(i=1,start=1;i<=a->destination.bytes;i++) {
    if(i<a->destination.bytes&&a->destination.raw[i]!='/')continue;
    n=i-start;
    if(!n||n>255||(n==1&&a->destination.raw[start]=='.')||
      (n==2&&a->destination.raw[start]=='.'&&a->destination.raw[start+1]=='.'))return 0;
    if(i==a->destination.bytes) {
      if(n!=b->final_bytes||!slcli_equal(a->destination.raw+start,(void *)b->final_leaf,n))return 0;
    }
    start=i+1;
  }
  /* Source maps every path before creating anything, including distinct
   * physical output records. A path join checks routing, never authority. */
  for(i=0;i<b->directory_count;i++) {
    if(!route(b,&b->directories[i],i)||b->directories[i].created==b->candidate)return 0;
    required=0;
    for(j=0;j<a->count;j++)if(a->entries[j].path.bytes>b->directories[i].path.bytes&&
      a->entries[j].path.raw[b->directories[i].path.bytes]=='/'&&
      slcli_equal(a->entries[j].path.raw,b->directories[i].path.raw,b->directories[i].path.bytes))required=1;
    if(!required)return 0;
    for(j=0;j<i;j++)if(same_path(b->directories[i].path,b->directories[j].path)||
      b->directories[i].created==b->directories[j].created)return 0;
  }
  for(i=0;i<b->file_count;i++) {
    struct slcli_writer_linux_route *r=&b->files[i];
    if(!route(b,r,b->directory_count)||r->created==b->candidate||r->output!=&b->materialize->outputs[i]||
      !same_path(r->path,a->entries[i].path)||!same_path(r->path,r->output->admitted_path))return 0;
    for(j=0;j<i;j++)if(r->created==b->files[j].created)return 0;
    for(j=0;j<b->directory_count;j++)if(r->created==b->directories[j].created)return 0;
  }
  for(i=0;i<b->discovery_count;i++) {
    size_t directory=i%(b->directory_count+1),children=0;
    struct slcli_writer_discovery *d=&b->discovery[i];
    for(j=0;j<b->directory_count;j++)if(b->directories[j].parent_directory==directory)children++;
    for(j=0;j<b->file_count;j++)if(b->files[j].parent_directory==directory)children++;
    if(d->used||d->capture_count||!d->children||d->child_count!=children||
      !d->captures||d->capture_capacity<134+3*children||!d->scratch||d->scratch_bytes!=1024||
      !d->sink||!d->archive_original)return 0;
  }
  b->used=1;b->failed=1;
  if(!slcli_writer_create_original(b->candidate,b->admitted_parent,b->admitted_parent_identity,
    b->candidate_leaf,b->candidate_bytes,b->admitted_uid,b->admitted_gid,0700,
    b->path_sink,b->archive_original))return 0;
  for(i=0;i<b->directory_count+b->file_count;i++) {
    struct slcli_writer_linux_route *r=i<b->directory_count?&b->directories[i]:&b->files[i-b->directory_count];
    struct slcli_writer_path_original *p=r->parent_directory?
      b->directories[r->parent_directory-1].created:b->candidate;
    if(!p->complete||p->failed||!p->count)return 0;
    parent=p->original;identity=&p->effects[p->count-2].raw;
    /* The held parent is freshly fstat'ed inside the concrete constructor;
     * original creation's own stat is the stable identity mapping. */
    if(!slcli_writer_create_original(r->created,parent,identity,(void *)r->leaf.raw,r->leaf.bytes,
      b->admitted_uid,b->admitted_gid,i<b->directory_count?0700:
        (a->entries[i-b->directory_count].mode?0755:0644),b->path_sink,b->archive_original))return 0;
    if(i>=b->directory_count&&!slcli_writer_begin_created_file(r->created,r->output,
      a->entries[i-b->directory_count].body.bytes?a->entries[i-b->directory_count].body.bytes:1,
      b->file_sink,b->archive_original))return 0;
  }
  if(!slcli_writer_materialize_originals(b->materialize))return 0;
  /* Each caller-owned discovery set maps the SAME held children, including
   * original raw stat identities acquired after complete writes/readbacks. */
  for(i=0;i<b->directory_count+1;i++) {
    parent=i?b->directories[i-1].created->original:b->candidate->original;
    if(!discover(b,i,i,parent))return 0;
  }
  if(!slcli_writer_publish_directory(b->candidate,b->final_leaf,b->final_bytes))return 0;
  for(i=0;i<b->directory_count+1;i++) {
    size_t at=b->directory_count+1+i;
    parent=i?b->directories[i-1].created->original:b->candidate->reopened;
    if(!discover(b,at,i,parent))return 0;
  }
  b->complete=1;b->failed=0;return 1;
}
#endif
