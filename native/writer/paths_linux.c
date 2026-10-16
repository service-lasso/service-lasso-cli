#include "paths_linux.h"
#include "../host-service/evidence_deadline.h"
#if defined(__linux__)
#include "../host-service/native_syscall_linux.h"
#include <linux/fcntl.h>
#include <linux/time.h>
int slcli_writer_path_clock(struct slcli_writer_path_effect *r) {
  long status;uint64_t seconds,nanoseconds;
  if(!r||r->clock_observed)return 0;r->clock_args[0]=CLOCK_MONOTONIC;
  r->clock_args[1]=(uintptr_t)&r->clock_raw;
  status=slcli_kernel_call(__NR_clock_gettime,CLOCK_MONOTONIC,(long)&r->clock_raw,0,0,0,0);
  r->clock_result=(uint64_t)status;r->clock_observed=1;
  if(status||r->clock_raw.tv_sec<0||r->clock_raw.tv_nsec<0||r->clock_raw.tv_nsec>=1000000000)return 0;
  seconds=(uint64_t)r->clock_raw.tv_sec;nanoseconds=(uint64_t)r->clock_raw.tv_nsec;
  if(seconds>(UINT64_MAX-nanoseconds)/1000000000)return 0;
  r->tick=seconds*1000000000+nanoseconds;r->clock_complete=r->tick!=0;return r->clock_complete;
}
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int evidence_plan_same(const struct slcli_writer_linux_evidence_originals *x,
  const struct slcli_writer_linux_evidence_originals *y) {
  return x->deadline==y->deadline&&x->caller==y->caller&&x->caller_bytes==y->caller_bytes&&
    x->capture_bytes==y->capture_bytes&&x->parent_identity==y->parent_identity&&
    x->name==y->name&&x->name_bytes==y->name_bytes&&x->uid==y->uid&&x->gid==y->gid&&
    x->maximum==y->maximum&&x->predecessor==y->predecessor&&x->parent==y->parent&&
    x->mode==y->mode&&x->path_sink==y->path_sink&&x->capture_sink==y->capture_sink&&
    x->capture_owner==y->capture_owner;
}
int slcli_writer_create_linux_evidence_original(struct slcli_arena *a,
  struct slcli_writer_path_original *p,struct slcli_writer_output_original *o,
  const struct slcli_writer_linux_evidence_originals *in) {
  const void *sources[4];size_t sizes[4],i,j;struct slcli_evidence_file *f;int clock_ok,joined,created_ok;
  const struct slcli_writer_linux_evidence_originals *original=in;
  struct slcli_writer_linux_evidence_originals *plan;
  if(!a||!p||!o||!in||!slcli_arena_live_range(a,p,sizeof(*p))||
    !slcli_arena_live_range(a,o,sizeof(*o))||!slcli_arena_live_range(a,in,sizeof(*in))||
    !(f=o->file)||!slcli_arena_live_range(a,f,sizeof(*f))||o->used||o->failed||o->complete||
    !f->captures||!f->capture_capacity||f->capture_capacity>SIZE_MAX/sizeof(*f->captures)||
    !f->workspace||!slcli_arena_live_range(a,f->workspace,sizeof(*f->workspace))||
    !slcli_arena_live_range(a,f->captures,f->capture_capacity*sizeof(*f->captures))||
    !in->name_bytes||in->name_bytes>255||!in->parent_identity||
    (in->mode!=0644&&in->mode!=0755)||!in->path_sink||!in->capture_sink||
    !in->caller_bytes||!in->capture_bytes||!in->maximum||
    !slcli_source_deadline_original_matches(a,in->deadline,in->caller)||
    !slcli_arena_live_range(a,in->caller,in->caller_bytes)||
    !slcli_arena_live_range(a,in->capture_owner,in->capture_bytes)||
    !separate(p,sizeof(*p),o,sizeof(*o))||!separate(p,sizeof(*p),f,sizeof(*f))||
    !separate(o,sizeof(*o),f,sizeof(*f))||
    !separate(o,sizeof(*o),in->capture_owner,in->capture_bytes)||
    !slcli_source_deadline_away(in->deadline,o,sizeof(*o))||
    !separate(o,sizeof(*o),f->workspace,sizeof(*f->workspace))||
    !separate(o,sizeof(*o),f->captures,f->capture_capacity*sizeof(*f->captures)))return 0;
  sources[0]=in;sizes[0]=sizeof(*in);sources[1]=in->parent_identity;sizes[1]=sizeof(*in->parent_identity);
  sources[2]=in->name;sizes[2]=in->name_bytes;sources[3]=in->caller;sizes[3]=in->caller_bytes;
  for(i=0;i<4;i++) {
    if(!slcli_arena_live_range(a,sources[i],sizes[i])||
      !separate(p,sizeof(*p),sources[i],sizes[i])||!separate(o,sizeof(*o),sources[i],sizes[i])||
      !separate(f,sizeof(*f),sources[i],sizes[i])||
      !separate(f->workspace,sizeof(*f->workspace),sources[i],sizes[i])||
      !separate(f->captures,f->capture_capacity*sizeof(*f->captures),sources[i],sizes[i])||
      !separate(in->capture_owner,in->capture_bytes,sources[i],sizes[i]))return 0;
    for(j=i+1;j<4;j++)if(i!=0||j!=3)
      if(!separate(sources[i],sizes[i],sources[j],sizes[j]))return 0;
  }
  /* Wholly byte-zeroed canonical producer storage is consumed once. The fd
   * sentinel is installed before preparation, not after a native failure. */
  for(i=0;i<sizeof(*p);i++)if(((const unsigned char *)p)[i])return 0;
  for(i=0;i<sizeof(o->before);i++)if(((const unsigned char *)&o->before)[i]||
    ((const unsigned char *)&o->after)[i])return 0;
  plan=slcli_arena_calloc(a,1,sizeof(*plan));if(!plan)return 0;
  if(!slcli_arena_live_block(a,plan,sizeof(*plan)))return 0;*plan=*in;
  p->evidence_original=original;p->evidence_plan=plan;p->evidence_file=f;in=plan;p->original=-1;
  if(!evidence_plan_same(original,plan))return 0;
  if(!slcli_evidence_deadline_prepare_unborn(a,f,in->deadline,in->caller,in->caller_bytes,
    in->capture_owner,in->capture_bytes,in->predecessor,in->parent,in->maximum,
    in->capture_sink,&p->original,p,sizeof(*p)))return 0;
  if(!slcli_native_clock_read(&p->evidence_entry_clock)||
    !slcli_evidence_deadline_precede(f,&p->evidence_entry_clock))return 0;
  p->evidence_previous_tick=p->evidence_entry_clock.tick;
  created_ok=slcli_writer_create_original(p,in->parent,in->parent_identity,in->name,in->name_bytes,
    in->uid,in->gid,in->mode,in->path_sink,in->capture_owner);
  if(!p->created||p->original<0){f->deadline_owner->failed=1;return 0;}
  clock_ok=slcli_native_clock_read(&p->evidence_birth_clock);
  joined=slcli_evidence_deadline_born(f,&p->original,&p->evidence_birth_clock);
  if(!clock_ok||!joined||!created_ok||!evidence_plan_same(original,plan)||!p->count||
    p->count>sizeof(p->effects)/sizeof(p->effects[0])||!p->effects[p->count-1].clock_complete||
    p->evidence_birth_clock.tick<p->effects[p->count-1].tick){f->deadline_owner->failed=1;return 0;}
  return slcli_writer_begin_created_file(p,o,in->maximum,in->capture_sink,in->capture_owner);
}
static int name_copy(char out[256],const char *in,size_t n) {
  size_t i;if(!in||!n||n>255||(n==1&&in[0]=='.')||
    (n==2&&in[0]=='.'&&in[1]=='.'))return 0;
  for(i=0;i<n;i++)if(!in[i]||in[i]=='/')return 0;
  for(i=0;i<n;i++)out[i]=in[i];out[n]=0;return 1;
}
static int same(const struct stat *a,const struct stat *b) {
  return a->st_dev==b->st_dev&&a->st_ino==b->st_ino&&
    a->st_uid==b->st_uid&&a->st_gid==b->st_gid&&a->st_mode==b->st_mode;
}
static struct slcli_writer_path_effect *effect(struct slcli_writer_path_original *p,
  long number,long a,long b,long c,long d,long e,long f) {
  struct slcli_writer_path_effect *r;size_t i;
  if(p->count>=20)return 0;r=&p->effects[p->count++];
  for(i=0;i<sizeof(*r);i++)if(((const unsigned char *)r)[i])return 0;
  r->number=number;r->args[0]=a;r->args[1]=b;r->args[2]=c;
  r->args[3]=d;r->args[4]=e;r->args[5]=f;
  if(p->evidence_plan) {
    struct slcli_evidence_deadline_owner *owner=p->evidence_file?p->evidence_file->deadline_owner:0;
    struct slcli_image_deadline_owner *deadline=owner?owner->rows.deadline_owner:0;
    if(p->evidence_unresolved||!deadline||!slcli_evidence_deadline_ready(p->evidence_file)||
      !evidence_plan_same(p->evidence_original,p->evidence_plan)||
      !slcli_native_clock_read(&r->before_clock)||r->before_clock.tick<p->evidence_previous_tick||
      r->before_clock.tick>=deadline->expires||
      !slcli_evidence_deadline_precede(p->evidence_file,&r->before_clock)) {
      p->evidence_unresolved=1;
      /* The requested effect was never called. Preserve this prefix/clock;
       * return no result row, so default result0 cannot manufacture fd0. */
      if(p->sink&&p->sink(p->archive_original,r))r->retained=1;
      return 0;
    }
    p->evidence_previous_tick=r->before_clock.tick;
  }
  r->called=1;r->result=(uint64_t)slcli_kernel_call(number,a,b,c,d,e,f);
  r->returned=1;r->observed=1;
  slcli_writer_path_clock(r);
  return r;
}
static int retain(struct slcli_writer_path_original *p,struct slcli_writer_path_effect *r) {
  struct slcli_image_deadline_owner *deadline;
  if(!r||!p->sink(p->archive_original,r))return 0;r->retained=1;
  if(!p->evidence_plan)return r->clock_complete;
  deadline=p->evidence_file->deadline_owner->rows.deadline_owner;
  if(!r->called||!r->returned||!r->observed||!r->before_clock.complete||
    !r->clock_complete||r->tick<p->evidence_previous_tick) {
    p->evidence_unresolved=1;return 0;
  }
  p->evidence_previous_tick=r->tick;
  if(r->tick>=deadline->expires){p->evidence_unresolved=1;return 0;}
  return !p->evidence_unresolved;
}
static struct slcli_writer_path_effect *observe(struct slcli_writer_path_original *p,int fd) {
  struct slcli_writer_path_effect *r;
  if(p->count>=20)return 0;r=&p->effects[p->count];
  r=effect(p,__NR_fstat,fd,(long)&r->raw,0,0,0,0);
  return retain(p,r)&&r->result==0?r:0;
}
int slcli_writer_create_original(struct slcli_writer_path_original *p,int parent,
  const struct stat *admitted,const char *name,size_t bytes,uint64_t uid,uint64_t gid,
  unsigned mode,slcli_writer_path_sink sink,void *archive) {
  struct slcli_writer_path_effect *r;long fd;
  if(!p||p->used||p->count||parent<0||!admitted||!sink||!archive||
    (p->evidence_plan&&(!p->evidence_file||!p->evidence_original||
      !p->evidence_entry_clock.complete||!slcli_evidence_deadline_ready(p->evidence_file)||
      p->evidence_unresolved||!evidence_plan_same(p->evidence_original,p->evidence_plan)||
      parent!=p->evidence_plan->parent||admitted!=p->evidence_plan->parent_identity||
      name!=p->evidence_plan->name||bytes!=p->evidence_plan->name_bytes||
      uid!=p->evidence_plan->uid||gid!=p->evidence_plan->gid||mode!=p->evidence_plan->mode||
      sink!=p->evidence_plan->path_sink||archive!=p->evidence_plan->capture_owner))||
    (mode!=0700&&mode!=0644&&mode!=0755)||
    !separate(p,sizeof(*p),admitted,sizeof(*admitted))||
    !separate(p,sizeof(*p),name,bytes)||!name_copy(p->name,name,bytes))return 0;
  p->used=1;p->failed=1;p->parent=parent;p->original=-1;p->route_original=-1;p->reopened=-1;
  p->sink=sink;p->archive_original=archive;p->parent_identity=*admitted;
  p->admitted_uid=uid;p->admitted_gid=gid;
  r=observe(p,parent);if(!r||(r->raw.st_mode&0170000)!=0040000||
    !same(&r->raw,admitted))return 0;
  if(mode==0700) {
    r=effect(p,__NR_mkdirat,parent,(long)p->name,0700,0,0,0);
    if(r&&!r->result)p->created=1;
    if(!retain(p,r)||r->result)return 0;
    r=effect(p,__NR_openat,parent,(long)p->name,
      O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC,0,0,0);
  } else r=effect(p,__NR_openat,parent,(long)p->name,
      O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC|O_NONBLOCK,mode,0,0);
  if(!r)return 0;fd=(long)r->result;
  /* Record a successful partial original even if its archive transfer fails. */
  if(fd>=0&&fd<=INT32_MAX){p->original=(int)fd;p->created=1;}
  if(!retain(p,r)||fd<0||fd>INT32_MAX)return 0;
  r=effect(p,__NR_fchmod,p->original,mode,0,0,0,0);
  if(!retain(p,r)||r->result)return 0;
  r=observe(p,p->original);if(!r||r->raw.st_uid!=uid||r->raw.st_gid!=gid||
    (r->raw.st_mode&0177777)!=((mode==0700?0040000:0100000)|mode)||
    (mode!=0700&&(r->raw.st_nlink!=1||r->raw.st_size!=0)))return 0;
  r=observe(p,parent);if(!r||!same(&r->raw,admitted))return 0;
  p->complete=1;p->failed=0;return 1;
}
int slcli_writer_begin_created_file(struct slcli_writer_path_original *p,
  struct slcli_writer_output_original *o,uint64_t maximum,
  slcli_store_capture_sink sink,void *archive) {
  if(!p||!p->complete||p->failed||!o||!o->file||o->used||
    !separate(p,sizeof(*p),o,sizeof(*o)))return 0;
  o->admitted_uid=p->admitted_uid;o->admitted_gid=p->admitted_gid;
  return slcli_evidence_begin(o->file,p->original,p->parent,maximum,sink,archive);
}
int slcli_writer_publish_directory(struct slcli_writer_path_original *p,const char *name,size_t n) {
  struct slcli_writer_path_effect *before,*route,*r;long fd;
  if(!p||!p->complete||p->failed||p->publish_used||p->count>6||
    !separate(p,sizeof(*p),name,n)||!name_copy(p->published,name,n))return 0;
  p->publish_used=1;p->failed=1;
  before=observe(p,p->original);if(!before||(before->raw.st_mode&0177777)!=0040700)return 0;
  r=effect(p,__NR_openat,p->parent,(long)p->name,
    O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC,0,0,0);
  if(!r)return 0;fd=(long)r->result;
  /* The pre-rename reopened route is retained as a separate original. */
  if(fd>=0&&fd<=INT32_MAX)p->route_original=(int)fd;
  if(!retain(p,r)||fd<0||fd>INT32_MAX)return 0;
  route=observe(p,(int)fd);if(!route||!same(&before->raw,&route->raw))return 0;
  r=effect(p,__NR_fsync,p->original,0,0,0,0,0);if(!retain(p,r)||r->result)return 0;
  r=effect(p,__NR_renameat2,p->parent,(long)p->name,p->parent,(long)p->published,1,0);
  if(!retain(p,r)||r->result)return 0;
  r=effect(p,__NR_fsync,p->parent,0,0,0,0,0);if(!retain(p,r)||r->result)return 0;
  r=effect(p,__NR_openat,p->parent,(long)p->published,
    O_RDONLY|O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC,0,0,0);
  if(!r)return 0;fd=(long)r->result;
  if(fd>=0&&fd<=INT32_MAX)p->reopened=(int)fd;
  if(!retain(p,r)||fd<0||fd>INT32_MAX)return 0;
  route=observe(p,p->reopened);if(!route||!same(&before->raw,&route->raw))return 0;
  r=observe(p,p->original);if(!r||!same(&before->raw,&r->raw))return 0;
  r=observe(p,p->parent);if(!r||!same(&p->parent_identity,&r->raw))return 0;
  /* Full final discovery/readback and genuine closure are separately owning
   * steps. Namespace identity is never admission or product acceptance. */
  p->published_complete=1;p->failed=0;return 1;
}
#endif
