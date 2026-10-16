#include "discovery_linux.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#include <linux/fcntl.h>
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int same(const struct stat *a,const struct stat *b) {
  return a->st_dev==b->st_dev&&a->st_ino==b->st_ino&&a->st_mode==b->st_mode&&
    a->st_uid==b->st_uid&&a->st_gid==b->st_gid&&a->st_nlink==b->st_nlink&&
    a->st_size==b->st_size&&a->st_mtime==b->st_mtime&&
    a->st_mtime_nsec==b->st_mtime_nsec&&a->st_ctime==b->st_ctime&&
    a->st_ctime_nsec==b->st_ctime_nsec;
}
static struct slcli_writer_path_effect *call(struct slcli_writer_discovery *d,
  long number,long a,long b,long c,long e) {
  struct slcli_writer_path_effect *r;
  if(d->capture_count>=d->capture_capacity)return 0;
  r=&d->captures[d->capture_count++];if(r->observed)return 0;
  r->number=number;r->args[0]=a;r->args[1]=b;r->args[2]=c;r->args[3]=e;
  r->result=(uint64_t)slcli_kernel_call(number,a,b,c,e,0,0);r->observed=1;return r;
}
static int retain(struct slcli_writer_discovery *d,struct slcli_writer_path_effect *r,
  const unsigned char *raw,size_t bytes) {
  if(!r)return 0;slcli_writer_path_clock(r);
  if(!d->sink(d->archive_original,r,raw,bytes))return 0;r->retained=1;return r->clock_complete;
}
static struct slcli_writer_path_effect *stat(struct slcli_writer_discovery *d,int fd) {
  struct slcli_writer_path_effect *r;
  if(d->capture_count>=d->capture_capacity)return 0;r=&d->captures[d->capture_count];
  r=call(d,__NR_fstat,fd,(long)&r->raw,0,0);
  return retain(d,r,0,0)&&!r->result?r:0;
}
int slcli_writer_discover_linux_original(struct slcli_writer_discovery *d) {
  struct slcli_writer_path_effect *before,*after,*r,*held,*namespace;
  size_t at,n,i,j,name_end,length;unsigned matches;long got;
  if(!d||d->used||d->original<0||!d->children||d->child_count>128||
    !d->captures||d->capture_count||!d->capture_capacity||
    d->capture_capacity>SIZE_MAX/sizeof(*d->captures)||!d->scratch||d->scratch_bytes!=1024||
    !d->sink||!d->archive_original||
    !separate(d,sizeof(*d),d->captures,d->capture_capacity*sizeof(*d->captures))||
    !separate(d,sizeof(*d),d->scratch,1024)||
    !separate(d->captures,d->capture_capacity*sizeof(*d->captures),d->scratch,1024)||
    !separate(d->children,d->child_count*sizeof(*d->children),d->scratch,1024)||
    !separate(d->children,d->child_count*sizeof(*d->children),d,sizeof(*d))||
    !separate(d->children,d->child_count*sizeof(*d->children),d->captures,
      d->capture_capacity*sizeof(*d->captures)))return 0;
  for(i=0;i<d->child_count;i++) {
    struct slcli_writer_discovery_child *c=&d->children[i];
    if(c->seen||c->original<0||!c->leaf||!c->bytes||c->bytes>255||c->leaf[c->bytes]||
      (c->bytes==1&&c->leaf[0]=='.')||(c->bytes==2&&c->leaf[0]=='.'&&c->leaf[1]=='.'))return 0;
    for(j=0;j<c->bytes;j++)if(!c->leaf[j]||c->leaf[j]=='/')return 0;
    if(!separate(c->leaf,c->bytes+1,d->scratch,1024)||
      !separate(c->leaf,c->bytes+1,d->captures,d->capture_capacity*sizeof(*d->captures)))return 0;
    for(j=0;j<i;j++)if(d->children[j].original==c->original||
      (d->children[j].bytes==c->bytes&&slcli_equal((void *)d->children[j].leaf,(void *)c->leaf,c->bytes)))return 0;
  }
  d->used=1;d->failed=1;before=stat(d,d->original);
  if(!before||(before->raw.st_mode&0170000)!=0040000)return 0;
  for(i=0;i<d->child_count;i++) {
    struct slcli_writer_discovery_child *c=&d->children[i];
    held=stat(d,c->original);if(!held||c->baseline||
      held->raw.st_dev!=c->admitted_identity.st_dev||held->raw.st_ino!=c->admitted_identity.st_ino||
      held->raw.st_uid!=c->admitted_identity.st_uid||held->raw.st_gid!=c->admitted_identity.st_gid||
      held->raw.st_mode!=c->admitted_identity.st_mode)return 0;
    c->baseline=held;
  }
  r=call(d,__NR_lseek,d->original,0,0,0);if(!retain(d,r,0,0)||r->result)return 0;
  for(;;) {
    r=call(d,__NR_getdents64,d->original,(long)d->scratch,1024,0);if(!r)return 0;
    got=(long)r->result;
    if(!retain(d,r,d->scratch,got>=0&&got<=1024?(size_t)got:0)||got<0||got>1024)return 0;
    if(!got){d->eof=1;break;}n=(size_t)got;if(n>65536-d->bytes)return 0;d->bytes+=n;
    for(at=0;at<n;at+=length) {
      if(n-at<20)return 0;length=(size_t)d->scratch[at+16]|(size_t)d->scratch[at+17]<<8;
      if(length<20||length>n-at||length%8||++d->records>130)return 0;
      name_end=at+19;while(name_end<at+length&&d->scratch[name_end])name_end++;
      if(name_end==at+length||name_end==at+19)return 0;
      i=name_end-(at+19);
      if((i==1&&d->scratch[at+19]=='.')||
        (i==2&&d->scratch[at+19]=='.'&&d->scratch[at+20]=='.'))continue;
      matches=0;
      for(j=0;j<d->child_count;j++) {
        struct slcli_writer_discovery_child *c=&d->children[j];
        if(c->bytes!=i||!slcli_equal((void *)c->leaf,d->scratch+at+19,i))continue;
        if(c->seen||++matches!=1)return 0;
        held=stat(d,c->original);if(!held||!c->baseline||!same(&held->raw,&c->baseline->raw))return 0;
        if(d->capture_count>=d->capture_capacity)return 0;namespace=&d->captures[d->capture_count];
        namespace=call(d,__NR_newfstatat,d->original,(long)c->leaf,(long)&namespace->raw,AT_SYMLINK_NOFOLLOW);
        if(!retain(d,namespace,0,0)||namespace->result||!same(&held->raw,&namespace->raw)||
          ((held->raw.st_mode&0170000)!=0040000&&
           ((held->raw.st_mode&0170000)!=0100000||held->raw.st_nlink!=1)))return 0;
        c->seen=1;
      }
      if(matches!=1)return 0;
    }
  }
  for(i=0;i<d->child_count;i++)if(!d->children[i].seen)return 0;
  after=stat(d,d->original);if(!after||!same(&before->raw,&after->raw))return 0;
  d->complete=1;d->failed=0;return 1;
}
#endif
