#include "attempt.h"
#if defined(_WIN32) || defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static unsigned char lower(unsigned char c){return c>='A'&&c<='Z'?c+('a'-'A'):c;}
static int equal_fold(const unsigned char *a,size_t n,const unsigned char *b,size_t m) {
  size_t i;if(n!=m)return 0;for(i=0;i<n;i++)if(lower(a[i])!=lower(b[i]))return 0;return 1;
}
static int reserved(const unsigned char *p,size_t n) {
  size_t base=0;while(base<n&&p[base]!='.')base++;
  return equal_fold(p,base,(const unsigned char *)"con",3)||
    equal_fold(p,base,(const unsigned char *)"prn",3)||
    equal_fold(p,base,(const unsigned char *)"aux",3)||
    equal_fold(p,base,(const unsigned char *)"nul",3)||
    (base==4&&p[3]>='1'&&p[3]<='9'&&
     (equal_fold(p,3,(const unsigned char *)"com",3)||equal_fold(p,3,(const unsigned char *)"lpt",3)));
}
static int portable(struct slcli_slice s) {
  size_t i,start=0,n;unsigned depth=0;unsigned char b;
  if(!s.raw||!s.bytes||s.bytes>240)return 0;
  for(i=0;i<=s.bytes;i++) {
    if(i<s.bytes&&s.raw[i]!='/') {
      b=s.raw[i];if(!((b>='a'&&b<='z')||(b>='A'&&b<='Z')||(b>='0'&&b<='9')||
                      b=='_'||b=='.'||b=='@'||b=='-'))return 0;continue;
    }
    n=i-start;depth++;
    if(!n||depth>12||s.raw[i-1]=='.'||reserved(s.raw+start,n))return 0;start=i+1;
  }
  return 1;
}
static int order(struct slcli_slice a,struct slcli_slice b) {
  size_t i,n=a.bytes<b.bytes?a.bytes:b.bytes;
  for(i=0;i<n;i++)if(a.raw[i]!=b.raw[i])return a.raw[i]<b.raw[i];return a.bytes<b.bytes;
}
static int compatible(struct slcli_slice a,struct slcli_slice b) {
  size_t ai=0,bi=0,ae,be,i;
  for(;;) {
    ae=ai;be=bi;while(ae<a.bytes&&a.raw[ae]!='/')ae++;while(be<b.bytes&&b.raw[be]!='/')be++;
    if(!equal_fold(a.raw+ai,ae-ai,b.raw+bi,be-bi))return 1;
    for(i=0;i<ae-ai;i++)if(a.raw[ai+i]!=b.raw[bi+i])return 0;
    if(ae==a.bytes||be==b.bytes)return 0;ai=ae+1;bi=be+1;
  }
}
static int text(struct slcli_cursor *c,uint32_t max,struct slcli_slice *out) {
  uint32_t n;size_t i;
  if(!slcli_u32(c,&n)||!n||n>max||!slcli_take(c,n,&out->raw))return 0;out->bytes=n;
  if(!slcli_utf8(out->raw,out->bytes,0))return 0;
  for(i=0;i<n;i++)if(!out->raw[i])return 0;return 1;
}
static int nonzero(const unsigned char *p){unsigned i;unsigned char n=0;for(i=0;i<32;i++)n|=p[i];return n!=0;}
int slcli_writer_attempt_consume(struct slcli_input_body *input,struct slcli_writer_attempt *out) {
  struct slcli_cursor c;uint32_t version,n,i,j;size_t bytes;
  if(!input||!out||out->original||out->complete||out->failed||!input->body||
     input->bytes<109||input->bytes>768000||input->consumed||
     input->capture_capacity>SIZE_MAX/sizeof(*input->captures)||
     !separate(input,sizeof(*input),out,sizeof(*out))||
     !separate(input->body,input->capacity,out,sizeof(*out))||
     !separate(input->readback,input->readback_capacity,out,sizeof(*out))||
     !separate(input->captures,input->capture_capacity*sizeof(*input->captures),out,sizeof(*out)))return 0;
  if(!slcli_input_body_recheck(input))return 0;input->consumed=2;input->failed=1;
  slcli_erase(out,sizeof(*out));out->original=input;out->failed=1;
  c.raw=input->body;c.bytes=input->bytes;c.offset=0;c.failed=0;
  if(!slcli_u32(&c,&version)||version!=3||!slcli_take(&c,32,&out->nonce)||
     !slcli_take(&c,32,&out->read_receipt)||!slcli_take(&c,32,&out->plan_digest)||
     !nonzero(out->nonce)||!nonzero(out->read_receipt)||!nonzero(out->plan_digest)||
     !text(&c,4096,&out->destination)||!slcli_u32(&c,&out->count)||!out->count||out->count>128)return 0;
  bytes=4+96+4+out->destination.bytes+4;
  for(i=0;i<out->count;i++) {
    struct slcli_writer_entry *e=&out->entries[i];
    if(!text(&c,240,&e->path)||!portable(e->path)||!slcli_u8(&c,&e->mode)||e->mode>1||
       !slcli_u32(&c,&n)||n>768000||!slcli_take(&c,n,&e->body.raw))return 0;e->body.bytes=n;
    if(i&&!order(out->entries[i-1].path,e->path))return 0;
    for(j=0;j<i;j++)if(!compatible(out->entries[j].path,e->path))return 0;
    if(bytes>768000||e->path.bytes+9>768000-bytes||n>768000-bytes-e->path.bytes-9)return 0;
    bytes+=e->path.bytes+9+n;
  }
  if(!slcli_cursor_end(&c)||bytes!=input->bytes)return 0;
  out->complete=1;out->failed=0;input->consumed=1;input->failed=0;return 1;
}
#endif
