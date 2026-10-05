#include "primitive.h"
#include "crypto.h"

struct operation { const char *name; uint32_t kind_mask, platform; };
#define K(value) (1u << (value))
/* Platform0 is the original ledger actor;1 Linux;2 Windows. Local numbering is
 * not emitted and does not change any public/private operation string. */
static const struct operation operations[] = {
  {"linux.execveat",K(SLCLI_BIRTH),1}, {"linux.pidfd_open",K(SLCLI_BIRTH),1},
  {"win32.create_process",K(SLCLI_BIRTH),2}, {"win32.process_birth",K(SLCLI_BIRTH),2},
  {"linux.so_peercred",K(SLCLI_IDENTITY),1}, {"linux.fstat",K(SLCLI_IDENTITY),1},
  {"linux.rename_noreplace",K(SLCLI_IDENTITY),1}, {"win32.token_user",K(SLCLI_IDENTITY),2},
  {"win32.pipe_peer",K(SLCLI_IDENTITY),2}, {"win32.file_identity",K(SLCLI_IDENTITY),2},
  {"win32.security_info",K(SLCLI_IDENTITY),2}, {"win32.rename_noreplace",K(SLCLI_IDENTITY),2},
  {"linux.openat_read",K(SLCLI_READ),1}, {"linux.read",K(SLCLI_READ)|K(SLCLI_EOF),1},
  {"win32.nt_open_read",K(SLCLI_READ),2}, {"win32.read",K(SLCLI_READ)|K(SLCLI_EOF),2},
  {"linux.write",K(SLCLI_WRITE),1}, {"win32.write",K(SLCLI_WRITE),2},
  {"linux.fsync",K(SLCLI_FLUSH),1}, {"win32.flush",K(SLCLI_FLUSH),2},
  {"linux.getdents",K(SLCLI_DISCOVERY),1}, {"win32.directory_query",K(SLCLI_DISCOVERY),2},
  {"linux.waitid",K(SLCLI_EXIT),1}, {"win32.process_wait",K(SLCLI_EXIT),2},
  {"linux.pidfd_poll",K(SLCLI_EXIT),1}, {"linux.close",K(SLCLI_CLOSE),1},
  {"win32.close",K(SLCLI_CLOSE),2}, {"ledger.generation_write",K(SLCLI_LEDGER),0},
  {"ledger.generation_readback",K(SLCLI_LEDGER),0}, {"ledger.selector_commit",K(SLCLI_LEDGER),0},
  {"ledger.selector_readback",K(SLCLI_LEDGER),0}
};
static const char *const kinds[] = {"birth","identity","read","write","eof",
  "discovery","ledger","flush","exit","close"};
static const char *const outcomes[] = {"success","failure","unresolved"};
struct output { unsigned char *raw; size_t capacity, used; uint32_t failed; };
static void put(struct output *o, const char *s) {
  while (*s) {
    if (o->failed || o->used == o->capacity) { o->failed = 1; return; }
    o->raw[o->used++] = (unsigned char)*s++;
  }
}
static void hex(struct output *o, const unsigned char digest[32]) {
  static const char alphabet[] = "0123456789abcdef";
  size_t i;
  for (i=0;i<32;++i) {
    char pair[3] = {alphabet[digest[i]>>4],alphabet[digest[i]&15],0}; put(o,pair);
  }
}
static void number(struct output *o, uint64_t value) {
  char raw[21]; size_t at=20; raw[20]=0;
  do { raw[--at]=(char)('0'+value%10); value/=10; } while(value);
  put(o,raw+at);
}
static int nonzero(const unsigned char value[32]) {
  unsigned char any=0; size_t i; for(i=0;i<32;++i) any|=value[i]; return any!=0;
}
size_t slcli_primitive_into(unsigned char *destination, size_t capacity,
                            const struct slcli_primitive *p) {
  struct output o;
  struct slcli_sha256 hash;
  unsigned char digest[32];
  uintptr_t d,s;
  uint32_t byte_kind, platform;
  const char *platform_name;
#if defined(__linux__)
  platform=1; platform_name="linux";
#elif defined(_WIN32)
  platform=2; platform_name="win32";
#else
  return 0;
#endif
  if(!destination||capacity<2048||!p||p->kind>=10||p->outcome>=3||p->mode>2||
     p->operation>=sizeof(operations)/sizeof(operations[0])||!p->tick||
     !nonzero(p->identity)||!nonzero(p->related)||!nonzero(p->source)) return 0;
  if(!(operations[p->operation].kind_mask & K(p->kind))||
     (operations[p->operation].platform && operations[p->operation].platform!=platform)) return 0;
  d=(uintptr_t)destination;
  if(d>UINTPTR_MAX-2048) return 0;
  /* Neither metadata nor retained input may alias the output being written. */
  s=(uintptr_t)p;
  if(s>UINTPTR_MAX-sizeof(*p)||(d<s+sizeof(*p)&&s<d+2048)) return 0;
  byte_kind=p->kind==SLCLI_READ||p->kind==SLCLI_WRITE||p->kind==SLCLI_EOF||
             p->kind==SLCLI_DISCOVERY||p->kind==SLCLI_LEDGER;
  if(byte_kind) {
    if(p->mode||(!p->data&&(p->bytes||p->outcome==0))||
       (p->kind==SLCLI_EOF&&(p->bytes||p->outcome))) return 0;
  } else if(p->data||p->bytes||
            (p->mode&&(p->kind!=SLCLI_IDENTITY||
             (p->operation!=SLCLI_LINUX_FSTAT&&p->operation!=SLCLI_WIN_SECURITY)))) return 0;
  if(p->data) {
    s=(uintptr_t)p->data;
    if(p->bytes>UINTPTR_MAX||s>UINTPTR_MAX-p->bytes||
       (p->bytes&&d<s+p->bytes&&s<d+2048)) return 0;
    slcli_sha256_init(&hash);
    if(!slcli_sha256_update(&hash,p->data,p->bytes)||!slcli_sha256_finish(&hash,digest)) return 0;
  }
  o.raw=destination; o.capacity=2048; o.used=0; o.failed=0;
  put(&o,"{\"kind\":\""); put(&o,kinds[p->kind]); put(&o,"\",\"platform\":\"");
  put(&o,platform_name); put(&o,"\",\"objectIdentityDigest\":\""); hex(&o,p->identity);
  put(&o,"\",\"monotonicTick\":\""); number(&o,p->tick);
  put(&o,"\",\"operation\":\""); put(&o,operations[p->operation].name);
  put(&o,"\",\"outcome\":\""); put(&o,outcomes[p->outcome]); put(&o,"\",\"bytesSha256\":");
  if(p->data) { put(&o,"\""); hex(&o,digest); put(&o,"\""); } else put(&o,"null");
  put(&o,",\"byteCount\":"); if(p->data) number(&o,p->bytes); else put(&o,"null");
  put(&o,",\"mode\":");
  if(!p->mode) put(&o,"null"); else put(&o,p->mode==1?"\"0644\"":"\"0755\"");
  put(&o,",\"relatedIdentityDigest\":\""); hex(&o,p->related);
  put(&o,"\",\"sourceImageSha256\":\""); hex(&o,p->source); put(&o,"\"}\n");
  slcli_erase(digest,sizeof(digest));
  if(o.failed) { slcli_erase(destination,o.used); return 0; }
  return o.used;
}
