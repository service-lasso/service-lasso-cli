#include "json.h"
#include "wire.h"
struct reader { const unsigned char *raw; size_t bytes, at; uint32_t members,tokens; };
static void space(struct reader *r) {
  while(r->at<r->bytes&&(r->raw[r->at]==' '||r->raw[r->at]=='\t'||
        r->raw[r->at]=='\r'||r->raw[r->at]=='\n')) ++r->at;
}
static int hex4(struct reader *r,uint32_t *out) {
  uint32_t value=0,i,n; unsigned char c;
  if(r->bytes-r->at<4) return 0;
  for(i=0;i<4;++i) {
    c=r->raw[r->at++];
    if(c>='0'&&c<='9') n=c-'0'; else if(c>='a'&&c<='f') n=c-'a'+10;
    else if(c>='A'&&c<='F') n=c-'A'+10; else return 0;
    value=(value<<4)|n;
  }
  *out=value; return 1;
}
/* Whole-input UTF8 validation precedes this scalar reader. Escaped strings use
 * the same decoded scalar identity as literal UTF8, including paired surrogates. */
static int scalar(struct reader *r,uint32_t *out) {
  unsigned char c,first; uint32_t low,n,value;
  if(r->at==r->bytes) return 0;
  c=r->raw[r->at++];
  if(c=='\\') {
    if(r->at==r->bytes) return 0;
    c=r->raw[r->at++];
    switch(c) {
    case '"': case '\\': case '/': *out=c; return 1;
    case 'b': *out=8; return 1; case 'f': *out=12; return 1;
    case 'n': *out=10; return 1; case 'r': *out=13; return 1;
    case 't': *out=9; return 1;
    case 'u':
      if(!hex4(r,&value)||(value>=0xdc00&&value<=0xdfff)) return 0;
      if(value>=0xd800&&value<=0xdbff) {
        if(r->bytes-r->at<6||r->raw[r->at++]!='\\'||r->raw[r->at++]!='u'||
           !hex4(r,&low)||low<0xdc00||low>0xdfff) return 0;
        value=0x10000+((value-0xd800)<<10)+(low-0xdc00);
      }
      *out=value; return 1;
    default: return 0;
    }
  }
  if(c<0x20||c=='"') return 0;
  if(c<0x80) { *out=c; return 1; }
  first=c;
  if(c>=0xc2&&c<=0xdf) { n=1; value=c&31; }
  else if(c>=0xe0&&c<=0xef) { n=2; value=c&15; }
  else if(c>=0xf0&&c<=0xf4) { n=3; value=c&7; }
  else return 0;
  if(n>r->bytes-r->at) return 0;
  while(n--) { c=r->raw[r->at++]; if((c&0xc0)!=0x80) return 0; value=(value<<6)|(c&63); }
  if((first<=0xdf&&value<0x80)||(first>=0xe0&&first<=0xef&&value<0x800)||
     (first>=0xf0&&value<0x10000)||value>0x10ffff||
     (value>=0xd800&&value<=0xdfff)) return 0;
  *out=value; return 1;
}
static int string(struct reader *r,struct slcli_json_span *span) {
  uint32_t value;
  if(r->at==r->bytes||r->raw[r->at]!='"') return 0;
  span->start=r->at++;
  while(r->at<r->bytes&&r->raw[r->at]!='"') if(!scalar(r,&value)) return 0;
  if(r->at==r->bytes) return 0;
  span->end=++r->at; return 1;
}
static int equal_key(const struct reader *root,struct slcli_json_span a,
                       struct slcli_json_span b,const char *literal) {
  struct reader left=*root,right=*root; uint32_t x,y; size_t i=0;
  left.at=a.start+1; left.bytes=a.end-1;
  right.at=b.start+1; right.bytes=b.end?b.end-1:0;
  while(left.at<left.bytes) {
    if(!scalar(&left,&x)) return 0;
    if(literal) { if(!literal[i]||x!=(unsigned char)literal[i++]) return 0; }
    else if(right.at==right.bytes||!scalar(&right,&y)||x!=y) return 0;
  }
  return literal?!literal[i]:right.at==right.bytes;
}
static int value(struct reader *,uint32_t,int,struct slcli_json_span *);
static int duplicate(const struct reader *root,size_t content,
                       struct slcli_json_span key,uint32_t depth) {
  struct reader prior=*root; struct slcli_json_span name,body;
  prior.at=content;
  while(prior.at<key.start) {
    space(&prior); if(prior.at==key.start) return 0;
    if(!string(&prior,&name)||equal_key(root,key,name,0)) return 1;
    space(&prior); if(prior.at==prior.bytes||prior.raw[prior.at++]!=':') return 1;
    if(!value(&prior,depth+1,0,&body)) return 1;
    space(&prior); if(prior.at>=key.start||prior.raw[prior.at++]!=',') return 1;
  }
  return prior.at!=key.start;
}
static int literal(struct reader *r,const char *text) {
  while(*text) if(r->at==r->bytes||r->raw[r->at++]!=(unsigned char)*text++) return 0;
  return 1;
}
static int digits(struct reader *r) {
  size_t before=r->at;
  while(r->at<r->bytes&&r->raw[r->at]>='0'&&r->raw[r->at]<='9') ++r->at;
  return r->at>before;
}
static int value(struct reader *r,uint32_t depth,int check,struct slcli_json_span *span) {
  unsigned char first,closing; size_t content; uint32_t count=0;
  struct slcli_json_span key,item;
  space(r); if(depth>16||r->at==r->bytes||(check&&++r->tokens>65536)) return 0;
  span->start=r->at; first=r->raw[r->at];
  if(first=='"') return string(r,span);
  if(first=='{'||first=='[') {
    closing=first=='{'?'}':']'; content=++r->at; space(r);
    if(r->at<r->bytes&&r->raw[r->at]==closing) { span->end=++r->at; return 1; }
    for(;;) {
      if(first=='{') {
        if((check&&++r->members>4096)||!string(r,&key)||
           (check&&duplicate(r,content,key,depth))) return 0;
        space(r); if(r->at==r->bytes||r->raw[r->at++]!=':') return 0;
      } else if(++count>4096) return 0;
      if(!value(r,depth+1,check,&item)) return 0;
      space(r); if(r->at==r->bytes) return 0;
      if(r->raw[r->at]==closing) { span->end=++r->at; return 1; }
      if(r->raw[r->at++]!=',') return 0;
      space(r);
    }
  }
  if(first=='t') { if(!literal(r,"true")) return 0; }
  else if(first=='f') { if(!literal(r,"false")) return 0; }
  else if(first=='n') { if(!literal(r,"null")) return 0; }
  else {
    if(first=='-') { if(++r->at==r->bytes) return 0; }
    if(r->raw[r->at]=='0') ++r->at;
    else if(r->raw[r->at]>='1'&&r->raw[r->at]<='9') { if(!digits(r)) return 0; }
    else return 0;
    if(r->at<r->bytes&&r->raw[r->at]=='.') { ++r->at; if(!digits(r)) return 0; }
    if(r->at<r->bytes&&(r->raw[r->at]=='e'||r->raw[r->at]=='E')) {
      ++r->at; if(r->at<r->bytes&&(r->raw[r->at]=='+'||r->raw[r->at]=='-')) ++r->at;
      if(!digits(r)) return 0;
    }
  }
  span->end=r->at; return 1;
}
int slcli_json_validate(const unsigned char *raw,size_t bytes,size_t maximum,
                         struct slcli_json_span *out) {
  struct reader r; struct slcli_json_span root={0,0};
  if(out) *out=root;
  if(!raw||!out||!bytes||bytes>maximum||
     (maximum!=16384&&maximum!=262144)||!slcli_utf8(raw,bytes,1)) return 0;
  r.raw=raw; r.bytes=bytes; r.at=0; r.members=0; r.tokens=0;
  if(!value(&r,0,1,&root)) return 0;
  space(&r); if(r.at!=bytes) return 0;
  *out=root; return 1;
}
int slcli_json_object(const unsigned char *raw,size_t bytes,struct slcli_json_span object,
                       const char *const *keys,size_t count,struct slcli_json_span *out) {
  struct reader r; struct slcli_json_span key,body,empty={0,0};
  uint64_t seen=0; size_t i,j;
  if(!raw||!keys||!out||!count||count>64||object.start>=object.end||object.end>bytes||
     raw[object.start]!='{'||raw[object.end-1]!='}') return 0;
  for(i=0;i<count;++i) out[i]=empty;
  r.raw=raw; r.bytes=object.end; r.at=object.start+1; r.members=0; r.tokens=0;
  space(&r);
  while(r.at<r.bytes&&r.raw[r.at]!='}') {
    if(!string(&r,&key)) return 0;
    for(j=0;j<count;++j) if(equal_key(&r,key,empty,keys[j])) break;
    if(j==count||(seen&(UINT64_C(1)<<j))) return 0;
    seen|=UINT64_C(1)<<j;
    space(&r); if(r.at==r.bytes||r.raw[r.at++]!=':'||!value(&r,1,0,&body)) return 0;
    out[j]=body; space(&r);
    if(r.at<r.bytes&&r.raw[r.at]=='}') break;
    if(r.at==r.bytes||r.raw[r.at++]!=',') return 0;
    space(&r);
    if(r.at==r.bytes||r.raw[r.at]=='}') return 0;
  }
  if(r.at+1!=object.end||r.raw[r.at]!='}'||
     seen!=(count==64?UINT64_MAX:(UINT64_C(1)<<count)-1)) return 0;
  return 1;
}
int slcli_json_text(const unsigned char *raw,size_t bytes,struct slcli_json_span text,
                     unsigned char *destination,size_t capacity,int controls,size_t *written) {
  struct reader r; uint32_t s; unsigned char encoded[4]; size_t n,i,at=0;
  if(written) *written=0;
  if(!raw||!destination||!written||text.start>=text.end||text.end>bytes||
     raw[text.start]!='"'||raw[text.end-1]!='"'||(controls!=0&&controls!=1)) return 0;
  r.raw=raw; r.bytes=text.end-1; r.at=text.start+1; r.members=0; r.tokens=0;
  while(r.at<r.bytes) {
    if(!scalar(&r,&s)||(!controls&&(s<32||s==127))) return 0;
    if(s<0x80) { n=1; encoded[0]=(unsigned char)s; }
    else if(s<0x800) { n=2; encoded[0]=0xc0|(s>>6); encoded[1]=0x80|(s&63); }
    else if(s<0x10000) { n=3; encoded[0]=0xe0|(s>>12); encoded[1]=0x80|((s>>6)&63); encoded[2]=0x80|(s&63); }
    else { n=4; encoded[0]=0xf0|(s>>18); encoded[1]=0x80|((s>>12)&63); encoded[2]=0x80|((s>>6)&63); encoded[3]=0x80|(s&63); }
    if(n>capacity-at) return 0;
    for(i=0;i<n;++i) destination[at++]=encoded[i];
  }
  *written=at; return 1;
}
int slcli_json_unsigned(const unsigned char *raw,size_t bytes,struct slcli_json_span number,
                         uint64_t maximum,int zero,uint64_t *out) {
  uint64_t value=0,digit; size_t i;
  if(out) *out=0;
  if(!raw||!out||number.start>=number.end||number.end>bytes||
     (number.end-number.start>1&&raw[number.start]=='0')) return 0;
  for(i=number.start;i<number.end;++i) {
    if(raw[i]<'0'||raw[i]>'9') return 0;
    digit=raw[i]-'0'; if(digit>maximum||value>(maximum-digit)/10) return 0;
    value=value*10+digit;
  }
  if(!zero&&!value) return 0;
  *out=value; return 1;
}
