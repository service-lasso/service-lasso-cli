#include "owner_handoff_codec.h"
static int apart(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static void put(unsigned char **p,uint64_t v,size_t n) {
  size_t i;for(i=0;i<n;i++){*(*p)++=(unsigned char)v;v>>=8;}
}
static uint64_t get(const unsigned char **p,size_t n) {
  uint64_t v=0;size_t i;for(i=0;i<n;i++)v|=(uint64_t)*(*p)++<<(8*i);return v;
}
int slcli_owner_handoff_encode(unsigned char *out,size_t bytes,
                                const struct slcli_memory_owner *o) {
  static const unsigned char magic[8]={'S','L','O','W','v','1',0,0};
  unsigned char *p=out;size_t i;
  if(bytes!=SLCLI_OWNER_HANDOFF_BYTES||!apart(out,bytes,o,sizeof(*o))||
     !slcli_memory_owner_frozen(o))return 0;
  for(i=0;i<bytes;i++)if(out[i])return 0;
  for(i=0;i<8;i++)*p++=magic[i];put(&p,1,4);put(&p,256,4);
  put(&p,o->limit,8);put(&p,o->charged,8);put(&p,o->next_sequence,8);
  for(i=0;i<3;i++)put(&p,o->category_peak[i],8);
  for(i=0;i<3;i++)put(&p,o->category_live[i],8);
  put(&p,o->peak,8);
  for(i=0;i<32;i++)*p++=o->original_birth[i];
  for(i=0;i<32;i++)*p++=o->original_image[i];
  for(i=0;i<32;i++)*p++=o->original_owner[i];
  for(i=0;i<32;i++)*p++=o->original_source[i];
  put(&p,o->failed,4);put(&p,o->count,4);put(&p,o->native_page_size,8);
  for(i=0;i<2;i++)put(&p,o->native_page_geometry[i],8);
  put(&p,o->handoff_state,4);
  for(i=0;i<256;i++) {
    const struct slcli_extent *e=&o->extents[i];
    put(&p,e->base,8);put(&p,e->reserved,8);put(&p,e->backed,8);
    put(&p,e->acquisition,8);put(&p,e->latest,8);put(&p,e->retirement,8);
    put(&p,e->category,4);put(&p,e->state,4);
    put(&p,e->backed_observed,4);put(&p,e->arena_initialized,4);
  }
  return (size_t)(p-out)==bytes;
}
int slcli_owner_handoff_decode(struct slcli_owner_handoff_decode *c,
                                const unsigned char *raw,size_t bytes) {
  static const unsigned char magic[8]={'S','L','O','W','v','1',0,0};
  const unsigned char *p=raw;struct slcli_memory_owner *o;size_t i;uint64_t value;
  if(!c||!apart(c,sizeof(*c),raw,bytes))return 0;
  for(i=0;i<sizeof(*c);i++)if(((const unsigned char *)c)[i])return 0;
  c->started=1;c->raw_original=raw;c->bytes_original=bytes;
  if(bytes!=SLCLI_OWNER_HANDOFF_BYTES)goto done;
  for(i=0;i<8;i++)if(*p++!=magic[i])goto done;
  if(get(&p,4)!=1||get(&p,4)!=256)goto done;o=&c->owner;
  o->limit=get(&p,8);o->charged=get(&p,8);o->next_sequence=get(&p,8);
  for(i=0;i<3;i++)o->category_peak[i]=get(&p,8);
  for(i=0;i<3;i++)o->category_live[i]=get(&p,8);
  o->peak=get(&p,8);
  for(i=0;i<32;i++)o->original_birth[i]=*p++;
  for(i=0;i<32;i++)o->original_image[i]=*p++;
  for(i=0;i<32;i++)o->original_owner[i]=*p++;
  for(i=0;i<32;i++)o->original_source[i]=*p++;
  o->failed=(uint32_t)get(&p,4);o->count=(uint32_t)get(&p,4);o->native_page_size=get(&p,8);
  for(i=0;i<2;i++) {
    value=get(&p,8);if(value>UINTPTR_MAX)goto done;
    o->native_page_geometry[i]=(uintptr_t)value;
  }
  o->handoff_state=(uint32_t)get(&p,4);
  for(i=0;i<256;i++) {
    struct slcli_extent *e=&o->extents[i];
    value=get(&p,8);if(value>UINTPTR_MAX)goto done;e->base=(uintptr_t)value;
    e->reserved=get(&p,8);e->backed=get(&p,8);e->acquisition=get(&p,8);
    e->latest=get(&p,8);e->retirement=get(&p,8);e->category=(uint32_t)get(&p,4);
    e->state=(uint32_t)get(&p,4);e->backed_observed=(uint32_t)get(&p,4);
    e->arena_initialized=(uint32_t)get(&p,4);
  }
  c->qualified=(size_t)(p-raw)==bytes&&slcli_memory_owner_frozen(o);
done:c->returned=1;return c->qualified;
}
int slcli_owner_handoff_matches(const unsigned char *raw,size_t bytes,
                                 const struct slcli_memory_owner *o) {
  static const unsigned char magic[8]={'S','L','O','W','v','1',0,0};
  const unsigned char *p=raw;size_t i;
  if(bytes!=SLCLI_OWNER_HANDOFF_BYTES||!apart(raw,bytes,o,sizeof(*o))||
     !slcli_memory_owner_frozen(o))return 0;
  for(i=0;i<8;i++)if(*p++!=magic[i])return 0;
  if(get(&p,4)!=1||get(&p,4)!=256||get(&p,8)!=o->limit||
     get(&p,8)!=o->charged||get(&p,8)!=o->next_sequence)return 0;
  for(i=0;i<3;i++)if(get(&p,8)!=o->category_peak[i])return 0;
  for(i=0;i<3;i++)if(get(&p,8)!=o->category_live[i])return 0;
  if(get(&p,8)!=o->peak)return 0;
  for(i=0;i<32;i++)if(*p++!=o->original_birth[i])return 0;
  for(i=0;i<32;i++)if(*p++!=o->original_image[i])return 0;
  for(i=0;i<32;i++)if(*p++!=o->original_owner[i])return 0;
  for(i=0;i<32;i++)if(*p++!=o->original_source[i])return 0;
  if(get(&p,4)!=o->failed||get(&p,4)!=o->count||get(&p,8)!=o->native_page_size)return 0;
  for(i=0;i<2;i++)if(get(&p,8)!=o->native_page_geometry[i])return 0;
  if(get(&p,4)!=o->handoff_state)return 0;
  for(i=0;i<256;i++) {
    const struct slcli_extent *e=&o->extents[i];
    if(get(&p,8)!=e->base||get(&p,8)!=e->reserved||get(&p,8)!=e->backed||
       get(&p,8)!=e->acquisition||get(&p,8)!=e->latest||get(&p,8)!=e->retirement||
       get(&p,4)!=e->category||get(&p,4)!=e->state||
       get(&p,4)!=e->backed_observed||get(&p,4)!=e->arena_initialized)return 0;
  }
  return (size_t)(p-raw)==bytes;
}
