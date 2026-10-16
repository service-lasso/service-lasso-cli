#if defined(__linux__)
#include "verity_linux.h"
#include <linux/fcntl.h>
static int away(const void *x,size_t n,const void *y,size_t m) {
  uintptr_t a=(uintptr_t)x,b=(uintptr_t)y;
  return x&&y&&a<=UINTPTR_MAX-n&&b<=UINTPTR_MAX-m&&!(a<b+m&&b<a+n);
}
struct slcli_verity_linux *slcli_verity_linux_allocate(struct slcli_arena *a) {
  struct slcli_verity_linux *s;
  if(!a)return 0;s=slcli_arena_calloc(a,1,sizeof(*s));
  if(s)s->arena=a;return s;
}
static int readonly(const struct slcli_linux_file_capture *c,uint64_t bytes) {
  return c&&c->operation==1&&c->observed&&!c->status&&c->clock_observed&&
    !c->clock_status&&c->tick&&c->stat_observed&&!c->stat_status&&
    c->flags_observed&&c->descriptor_observed&&
    (c->flags_status&O_ACCMODE)==O_RDONLY&&!(c->flags_status&O_PATH)&&
    (c->descriptor_status&FD_CLOEXEC)&&c->original_stat.st_size>=0&&
    (uint64_t)c->original_stat.st_size==bytes;
}
static int retained(struct slcli_verity_linux *s,struct slcli_linux_file_capture *c,int ok) {
  /* Raw row includes actual ioctl output and persistent effect facts even
   * when the later clock fails. No aggregate-ok masking or scratch reuse. */
  if(!s->sink(s->capture_original,c,0,0))ok=0;
  if(c->verity_applied)s->seal_applied=1;
  if(!ok||!c->clock_observed||c->clock_status||!c->tick||
     c->tick<s->previous_tick||c->tick>=s->expires||
     s->deadline->expires!=s->expires||
     s->deadline->caller_original!=s->caller_original||
     s->deadline->origin!=s->original_origin||
     s->original_origin->tick!=s->origin_tick) {
    s->failed=1;return 0;
  }
  s->previous_tick=c->tick;return 1;
}
static struct slcli_linux_file_capture *row(struct slcli_verity_linux *s) {
  struct slcli_native_clock *c;size_t i;
  if(s->row_count>=6){s->failed=1;return 0;}
  i=s->row_count++;c=&s->entry[i];
  /* Capture transfer can itself consume remaining time. Take a fresh retained
   * original native clock before each next file effect; never renew expiry. */
  if(s->deadline->expires!=s->expires||
     s->deadline->caller_original!=s->caller_original||
     s->deadline->origin!=s->original_origin||
     s->original_origin->tick!=s->origin_tick||
     !slcli_native_clock_read(c)||c->tick<s->previous_tick||c->tick>=s->expires) {
    s->failed=1;return 0;
  }
  s->previous_tick=c->tick;return &s->rows[i];
}
int slcli_verity_linux_original(struct slcli_verity_linux *s,int fd,uint32_t mode,
  uint32_t block_size,const struct slcli_linux_file_capture *expected,
  const struct slcli_linux_file_capture *predecessor,uint64_t bytes,
  const struct slcli_source_deadline_original *d,const void *caller,
  slcli_store_capture_sink sink,void *capture_original) {
  struct slcli_linux_file_capture *before,*enable,*first,*after,*second,*final;
  int ok;
  if(!s||s->started||s->failed||!s->arena||fd<0||mode>1||!bytes||
     !sink||!capture_original||!expected||
     !slcli_arena_live_range(s->arena,s,sizeof(*s))||
     !slcli_arena_live_range(s->arena,expected,sizeof(*expected))||
     !away(s,sizeof(*s),expected,sizeof(*expected))||
     !predecessor||!slcli_arena_live_range(s->arena,predecessor,sizeof(*predecessor))||
     !away(s,sizeof(*s),predecessor,sizeof(*predecessor))||
     !away(s,sizeof(*s),caller,1)||!away(s,sizeof(*s),capture_original,1)||
     !slcli_source_deadline_original_matches(s->arena,d,caller)||
     !slcli_source_deadline_away(d,s,sizeof(*s))||!readonly(expected,bytes)||
     !predecessor->clock_observed||predecessor->clock_status||!predecessor->tick||
     predecessor->tick<expected->tick||predecessor->tick<d->origin->tick||
     predecessor->tick>=d->expires||
     (mode&&(predecessor->operation!=7||!predecessor->observed||predecessor->status||
       !predecessor->descriptor_closed||predecessor->args[0]==(uint64_t)fd))||
     (mode&&(!block_size||(block_size&(block_size-1))))||(!mode&&block_size))return 0;
  s->started=1;s->deadline=d;s->caller_original=caller;s->expected=expected;
  s->original_origin=d->origin;
  s->predecessor=predecessor;
  s->sink=sink;s->capture_original=capture_original;s->original=fd;s->bytes=bytes;
  s->expires=d->expires;s->origin_tick=d->origin->tick;s->previous_tick=predecessor->tick;
  before=row(s);if(!before)return 0;ok=slcli_linux_file_observe(fd,before);
  if(!retained(s,before,ok)||!readonly(before,bytes)||
     !slcli_input_original_same(expected,before,bytes))goto failed;
  if(mode) {
    enable=row(s);if(!enable)return 0;ok=slcli_linux_file_verity_enable(fd,block_size,enable);
    if(!retained(s,enable,ok))return 0;
  }
  first=row(s);if(!first)return 0;ok=slcli_linux_file_verity_measure(fd,first);
  if(!retained(s,first,ok))return 0;
  after=row(s);if(!after)return 0;ok=slcli_linux_file_observe(fd,after);
  if(!retained(s,after,ok)||!readonly(after,bytes)||
     !slcli_store_original_same(before,after)||
     !(after->identity.stx_attributes_mask&STATX_ATTR_VERITY)||
     !(after->identity.stx_attributes&STATX_ATTR_VERITY))goto failed;
  second=row(s);if(!second)return 0;ok=slcli_linux_file_verity_measure(fd,second);
  if(!retained(s,second,ok)||
     !slcli_equal(first->verity_digest.raw,second->verity_digest.raw,
                  sizeof(struct fsverity_digest)+32))goto failed;
  final=row(s);if(!final)return 0;ok=slcli_linux_file_observe(fd,final);
  if(!retained(s,final,ok)||!readonly(final,bytes)||
     !slcli_input_original_same(after,final,bytes)||
     !(final->identity.stx_attributes_mask&STATX_ATTR_VERITY)||
     !(final->identity.stx_attributes&STATX_ATTR_VERITY))goto failed;
  s->verified=1;return 1;
failed:
  s->failed=1;return 0;
}
#endif
