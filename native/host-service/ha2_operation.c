#include "ha2_operation.h"
#include "crypto.h"
static int away(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int equal_text(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  const char *expected) {
  unsigned char text[96];size_t count,i;
  if(!slcli_json_text(raw,bytes,span,text,sizeof(text),0,&count))return 0;
  for(i=0;i<count;i++)if(!expected[i]||text[i]!=(unsigned char)expected[i])return 0;
  return !expected[count];
}
static int decimal(const unsigned char *raw,size_t bytes,struct slcli_json_span span,uint64_t *out) {
  unsigned char text[20];size_t n,i;uint64_t value=0;
  if(!slcli_json_text(raw,bytes,span,text,sizeof(text),0,&n)||!n||text[0]=='0')return 0;
  for(i=0;i<n;i++) {
    uint64_t d;if(text[i]<'0'||text[i]>'9')return 0;d=text[i]-'0';
    if(value>(UINT64_MAX-d)/10)return 0;value=value*10+d;
  }
  *out=value;return 1;
}
static int digest(const unsigned char *raw,size_t bytes,struct slcli_json_span span,
  unsigned char out[32]) {
  unsigned char text[64];size_t n,i;unsigned char a,b;
  if(!slcli_json_text(raw,bytes,span,text,sizeof(text),0,&n)||n!=64)return 0;
  for(i=0;i<32;i++) {
    a=text[2*i];b=text[2*i+1];
    if(a>='0'&&a<='9')a-='0';else if(a>='a'&&a<='f')a=a-'a'+10;else return 0;
    if(b>='0'&&b<='9')b-='0';else if(b>='a'&&b<='f')b=b-'a'+10;else return 0;
    out[i]=(unsigned char)(a*16+b);
  }
  return 1;
}
static int nonzero(const unsigned char p[32]) {
  unsigned char n=0;unsigned i;for(i=0;i<32;i++)n|=p[i];return n!=0;
}
static int canonical(const unsigned char *raw,size_t bytes,const char *const keys[12],
  const struct slcli_json_span fields[12]) {
  size_t at=0,i,j;
  if(bytes<3||raw[at++]!='{')return 0;
  for(i=0;i<12;i++) {
    if(at>=bytes||raw[at++]!='"')return 0;
    for(j=0;keys[i][j];j++)if(at>=bytes||raw[at++]!=(unsigned char)keys[i][j])return 0;
    if(at+2>bytes||raw[at++]!='"'||raw[at++]!=':'||fields[i].start!=at||
       fields[i].end<=at||fields[i].end>bytes)return 0;
    /* Every value in this closed ABI is ASCII text/digits. Alternate escaped
     * spellings and whitespace are not equivalent original descriptor bytes. */
    for(j=at;j<fields[i].end;j++)if(raw[j]=='\\'||raw[j]<33||raw[j]>126)return 0;
    at=fields[i].end;
    if(i<11&&(at>=bytes||raw[at++]!=','))return 0;
  }
  return at+2==bytes&&raw[at]=='}'&&raw[at+1]=='\n';
}
int slcli_ha2_operation_parse(const unsigned char *raw,size_t bytes,struct slcli_ha2_operation *out) {
  static const char *const keys[]={"schema","version","event","sequence","generation",
    "callerAssociation","sourceAssociation","processBirthAssociation","frontierAssociation",
    "priorRequestAssociation","priorRecordRawSha256","flags"};
  static const char *const events[]={"INSTALL_PREPARE","ACTIVATE","LEASE_BIRTH","LEDGER_BIRTH",
    "BORROW_OPEN","BORROW_CLOSE","ARCHIVE","NEXT_START_PREPARE","REVOKE_BEGIN",
    "REVOKE_COMPLETE","QUARANTINE"};
  struct slcli_json_span root,fields[12];struct slcli_ha2_operation result={0};
  uint64_t version,flags;unsigned i;
  if(!out||!away(out,sizeof(*out),raw,bytes)||
     !slcli_json_validate(raw,bytes,16384,&root)||
     !slcli_json_object(raw,bytes,root,keys,12,fields)||
     !canonical(raw,bytes,keys,fields)||
     !equal_text(raw,bytes,fields[0],"service-lasso.ha2-journal-operation.v1")||
     !slcli_json_unsigned(raw,bytes,fields[1],1,0,&version)||version!=1||
     !slcli_json_unsigned(raw,bytes,fields[11],0,1,&flags)||flags||
     !decimal(raw,bytes,fields[3],&result.sequence)||result.sequence>1024||
     !decimal(raw,bytes,fields[4],&result.generation))return 0;
  for(i=0;i<11;i++)if(equal_text(raw,bytes,fields[2],events[i])){result.event=i+1;break;}
  if(!result.event||!digest(raw,bytes,fields[5],result.caller)||!nonzero(result.caller)||
     !digest(raw,bytes,fields[6],result.source)||!nonzero(result.source)||
     !digest(raw,bytes,fields[7],result.birth)||!nonzero(result.birth)||
     !digest(raw,bytes,fields[8],result.frontier)||!nonzero(result.frontier)||
     !digest(raw,bytes,fields[9],result.prior_request)||
     !digest(raw,bytes,fields[10],result.prior_record))return 0;
  if(result.sequence==1) {
    if(result.generation!=1||result.event!=SLCLI_HA2_INSTALL_PREPARE||
       nonzero(result.prior_request)||nonzero(result.prior_record))return 0;
  } else if(result.event==SLCLI_HA2_INSTALL_PREPARE||
      !nonzero(result.prior_request)||!nonzero(result.prior_record))return 0;
  result.raw=raw;result.bytes=bytes;*out=result;return 1;
}
