#include "ledger_state.h"
#include "primary_wire.h"
struct encoder {
  slcli_ledger_sink sink; void *context;
  struct slcli_sha256 hash; struct slcli_hmac256 *mac;
  uint32_t bytes,failed;
};
static int nonzero(const unsigned char *p) {
  unsigned char n=0;unsigned i;for(i=0;i<32;i++)n|=p[i];return n!=0;
}
static int separate(const void *a,size_t n,const void *b,size_t m) {
  uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
  return a&&b&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
static int original_output(const struct slcli_ledger_state *l,const void *out,size_t n) {
  unsigned i;
  if(!l||!out||!separate(l,sizeof(*l),out,n)||
     !separate(l->key_version.raw,l->key_version.bytes,out,n))return 0;
  for(i=0;i<SLCLI_BINDINGS;i++)if(l->slots[i].state!=SLCLI_FREE&&
     !separate(l->slots[i].owner.raw,l->slots[i].owner.bytes,out,n))return 0;
  return 1;
}
static int emit(struct encoder *e,const unsigned char *p,size_t n) {
  if(e->failed||n>SLCLI_LEDGER_LIMIT-e->bytes||
     !slcli_sha256_update(&e->hash,p,n)||
     (e->mac&&!slcli_hmac256_update(e->mac,p,n))||
     (e->sink&&!e->sink(e->context,p,n))) {e->failed=1;return 0;}
  e->bytes+=(uint32_t)n;return 1;
}
static int literal(struct encoder *e,const char *s) {
  size_t n=0;while(s[n])n++;return emit(e,(const unsigned char *)s,n);
}
static int number(struct encoder *e,uint64_t n,int quoted) {
  unsigned char digits[20];size_t i=20;do{digits[--i]=(unsigned char)('0'+n%10);n/=10;}while(n);
  return (!quoted||literal(e,"\""))&&emit(e,digits+i,20-i)&&
    (!quoted||literal(e,"\""));
}
static int digest(struct encoder *e,const unsigned char *raw,int present) {
  static const unsigned char hex[]="0123456789abcdef";unsigned char bytes[64];unsigned i;
  if(!present)return literal(e,"null");
  for(i=0;i<32;i++){bytes[2*i]=hex[raw[i]>>4];bytes[2*i+1]=hex[raw[i]&15];}
  return literal(e,"\"")&&emit(e,bytes,64)&&literal(e,"\"");
}
static int text(struct encoder *e,struct slcli_slice s,int present) {
  static const unsigned char hex[]="0123456789abcdef";
  unsigned char esc[6]={'\\','u','0','0',0,0};size_t i;
  if(!present)return literal(e,"null");
  if(!literal(e,"\""))return 0;
  for(i=0;i<s.bytes;i++) {
    unsigned char c=s.raw[i];
    if(c=='"'||c=='\\') {unsigned char two[2]={'\\',c};if(!emit(e,two,2))return 0;}
    else if(c=='\b'||c=='\f'||c=='\n'||c=='\r'||c=='\t') {
      unsigned char two[2]={'\\',c=='\b'?'b':c=='\f'?'f':c=='\n'?'n':c=='\r'?'r':'t'};
      if(!emit(e,two,2))return 0;
    } else if(c<32||c=='<'||c=='>'||c=='&') {
      esc[4]=hex[c>>4];esc[5]=hex[c&15];if(!emit(e,esc,6))return 0;
    } else if(c==0xe2&&i+2<s.bytes&&s.raw[i+1]==0x80&&
                (s.raw[i+2]==0xa8||s.raw[i+2]==0xa9)) {
      if(!literal(e,s.raw[i+2]==0xa8?"\\u2028":"\\u2029"))return 0;i+=2;
    } else if(!emit(e,&s.raw[i],1))return 0;
  }
  return literal(e,"\"");
}
static int decimal_ref(struct encoder *e,uint64_t n,int present) {
  return present?number(e,n,1):literal(e,"null");
}
static const char *state_name(uint32_t s) {
  static const char *names[]={"\"FREE\"","\"RESERVED\"","\"LIVE\"",
    "\"CLOSE_PENDING\"","\"RETAINED\"","\"QUARANTINED\""};return names[s];
}
int slcli_ledger_state_valid(const struct slcli_ledger_state *l) {
  unsigned i;
  if(!l||!l->generation||!nonzero(l->host)||!nonzero(l->epoch)||
     !nonzero(l->profile)||!l->key_version.raw||!l->key_version.bytes||
     l->key_version.bytes>80||!slcli_utf8(l->key_version.raw,l->key_version.bytes,1)||
     l->previous_present>1||(l->previous_present&&!nonzero(l->previous)))return 0;
  for(i=0;i<SLCLI_BINDINGS;i++) {
    const struct slcli_ledger_slot *s=&l->slots[i];
    const struct slcli_terminal_cell *t=&l->terminal[i];
    if(s->state>SLCLI_QUARANTINED||s->retained>8388608||
       s->closure_present>1||s->intent_present>1||s->observation_present>1||
       t->occupied>1||t->intent_present>1||t->ready>1||t->ack_present>1)return 0;
    if(s->state==SLCLI_FREE) {
      if(s->retained||s->observation_present)return 0;
    } else {
      if(!nonzero(s->reservation)||!nonzero(s->primary)||!nonzero(s->resource)||
         !nonzero(s->observer)||!s->generation||s->generation>l->generation||
         !s->created||s->created>s->generation||!s->deadline||!s->owner.raw||
         !s->owner.bytes||s->owner.bytes>184||!slcli_utf8(s->owner.raw,s->owner.bytes,1))return 0;
      if(s->closure_present&&!nonzero(s->closure))return 0;
      if(s->intent_present&&(!nonzero(s->intent)||!s->intent_generation||
         s->intent_generation>s->generation||!s->intent_tick||s->intent_tick>=s->deadline))return 0;
      if(s->state==SLCLI_CLOSE_PENDING&&!s->intent_present)return 0;
      if(s->observation_present&&(!nonzero(s->observation)||s->observation_bytes<6||
         s->observation_bytes>SLCLI_RUNTIME_SNAPSHOT_BYTES))return 0;
    }
    if(!t->occupied) {
      if(t->ready||t->ack_present||t->intent_present)return 0;
    } else {
      if(!nonzero(t->reservation)||!nonzero(t->closure)||!t->generation||
         t->generation>l->generation||t->disposition>2||
         (t->intent_present&&!nonzero(t->intent))||
         (!t->disposition&&!t->intent_present))return 0;
      if(t->ready&&(!nonzero(t->ledger)||!nonzero(t->receipt)||!nonzero(t->receipt_ref)))return 0;
      if(t->ack_present&&(!t->ready||!nonzero(t->ack)))return 0;
    }
  }
  return 1;
}
static int payload(struct encoder *e,const struct slcli_ledger_state *l) {
  unsigned i;int comma=0;
  if(!literal(e,"{\"schema\":\"service-lasso.cli-host-ledger.v2\",\"hostIdentity\":")||
     !digest(e,l->host,1)||!literal(e,",\"hostEpoch\":")||!digest(e,l->epoch,1)||
     !literal(e,",\"profileSha256\":")||!digest(e,l->profile,1)||
     !literal(e,",\"keyVersion\":")||!text(e,l->key_version,1)||
     !literal(e,",\"generation\":")||!number(e,l->generation,1)||
     !literal(e,",\"slots\":["))return 0;
  for(i=0;i<SLCLI_BINDINGS;i++) {
    const struct slcli_ledger_slot *s=&l->slots[i];int used=s->state!=SLCLI_FREE;
    if((i&&!literal(e,","))||!literal(e,"{\"schema\":\"service-lasso.cli-host-slot.v2\",\"slotId\":")||
       !number(e,i,0)||!literal(e,",\"reservationId\":")||!digest(e,s->reservation,used)||
       !literal(e,",\"generation\":")||!decimal_ref(e,s->generation,used)||
       !literal(e,",\"state\":")||!literal(e,state_name(s->state))||
       !literal(e,",\"hostEpoch\":")||!digest(e,l->epoch,used)||
       !literal(e,",\"serviceProfileSha256\":")||!digest(e,l->profile,used)||
       !literal(e,",\"ownerNativeId\":")||!text(e,s->owner,used)||
       !literal(e,",\"primaryIdentityDigest\":")||!digest(e,s->primary,used)||
       !literal(e,",\"createdSequence\":")||!decimal_ref(e,s->created,used)||
       !literal(e,",\"resourceDigest\":")||!digest(e,s->resource,used)||
       !literal(e,",\"maximumBytes\":")||!literal(e,used?"8388608":"null")||
       !literal(e,",\"retainedBytes\":")||!number(e,s->retained,0)||
       !literal(e,",\"closureEvidenceDigest\":")||!digest(e,s->closure,used&&s->closure_present)||
       !literal(e,",\"keyVersion\":")||!text(e,l->key_version,used)||
       !literal(e,",\"observerBindingDigest\":")||!digest(e,s->observer,used)||
       !literal(e,",\"closeIntentDigest\":")||!digest(e,s->intent,used&&s->intent_present)||
       !literal(e,",\"closeIntentGeneration\":")||!decimal_ref(e,s->intent_generation,used&&s->intent_present)||
       !literal(e,",\"closeIntentTick\":")||!decimal_ref(e,s->intent_tick,used&&s->intent_present)||
       !literal(e,",\"absoluteDeadlineTick\":")||!decimal_ref(e,s->deadline,used)||!literal(e,"}"))return 0;
  }
  if(!literal(e,"],\"observations\":["))return 0;
  for(i=0;i<SLCLI_BINDINGS;i++) {
    const struct slcli_ledger_slot *s=&l->slots[i];if(!s->observation_present)continue;
    if((comma&&!literal(e,","))||!literal(e,"{\"slotId\":")||!number(e,i,0)||
       !literal(e,",\"sha256\":")||!digest(e,s->observation,1)||
       !literal(e,",\"size\":")||!number(e,s->observation_bytes,0)||!literal(e,"}"))return 0;
    comma=1;
  }
  if(!literal(e,"],\"terminalCells\":["))return 0;
  for(i=0;i<SLCLI_BINDINGS;i++) {
    const struct slcli_terminal_cell *t=&l->terminal[i];int used=t->occupied!=0;
    if((i&&!literal(e,","))||!literal(e,"{\"slotId\":")||!number(e,i,0)||
       !literal(e,",\"reservationId\":")||!digest(e,t->reservation,used)||
       !literal(e,",\"intentDigest\":")||!digest(e,t->intent,used&&t->intent_present)||
       !literal(e,",\"closureEvidenceDigest\":")||!digest(e,t->closure,used)||
       !literal(e,",\"terminalGeneration\":")||!decimal_ref(e,t->generation,used)||
       !literal(e,",\"closureLedgerRef\":")||!digest(e,t->ledger,used&&t->ready)||
       !literal(e,",\"terminalReceiptDigest\":")||!digest(e,t->receipt,used&&t->ready)||
       !literal(e,",\"terminalReceiptRef\":")||!digest(e,t->receipt_ref,used&&t->ready)||
       !literal(e,",\"disposition\":")||!literal(e,used?state_name(t->disposition==0?SLCLI_FREE:t->disposition==1?SLCLI_RETAINED:SLCLI_QUARANTINED):"null")||
       !literal(e,",\"ackDigest\":")||!digest(e,t->ack,used&&t->ack_present)||!literal(e,"}"))return 0;
  }
  return literal(e,"],\"previousPayloadSha256\":")&&digest(e,l->previous,l->previous_present)&&literal(e,"}\n");
}
int slcli_ledger_payload(const struct slcli_ledger_state *l,slcli_ledger_sink sink,
                          void *context,uint32_t *bytes,unsigned char hash[32]) {
  struct encoder e={0};
  if(!bytes||!hash||!slcli_ledger_state_valid(l)||
     !original_output(l,bytes,sizeof(*bytes))||!original_output(l,hash,32)||
     !separate(bytes,sizeof(*bytes),hash,32))return 0;
  e.sink=sink;e.context=context;slcli_sha256_init(&e.hash);
  if(!payload(&e,l)||!slcli_sha256_finish(&e.hash,hash))return 0;
  *bytes=e.bytes;return 1;
}
int slcli_ledger_frame(const struct slcli_ledger_state *l,const unsigned char key[32],
                        slcli_ledger_sink sink,void *context,uint32_t *bytes,unsigned char hash[32]) {
  static const unsigned char domain[]="SLCLI-HOST-LEDGER-2";
  struct encoder e={0};struct slcli_hmac256 mac;unsigned char length[4],first[32],tag[32];uint32_t n;
  if(!key||!sink||!bytes||!hash||!slcli_ledger_state_valid(l)||
     !original_output(l,bytes,sizeof(*bytes))||!original_output(l,hash,32)||
     !separate(bytes,sizeof(*bytes),hash,32)||!separate(bytes,sizeof(*bytes),key,32)||
     !separate(hash,32,key,32)||!slcli_ledger_payload(l,0,0,&n,first))return 0;
  slcli_put_u32(length,n);
  if(!slcli_hmac256_init(&mac,key)||!slcli_hmac256_update(&mac,domain,sizeof(domain))||
     !slcli_hmac256_update(&mac,length,4)||!sink(context,length,4))return 0;
  e.sink=sink;e.context=context;e.mac=&mac;slcli_sha256_init(&e.hash);
  if(!payload(&e,l)||e.bytes!=n||!slcli_sha256_finish(&e.hash,hash)||
     !slcli_equal(first,hash,32)||!slcli_hmac256_finish(&mac,tag)||!sink(context,tag,32))return 0;
  *bytes=n+36;return 1;
}
int slcli_ledger_selector(unsigned char out[74],uint8_t slot,uint64_t generation,
                           const unsigned char hash[32],const unsigned char key[32]) {
  static const unsigned char domain[]="SLCLI-HOST-SELECTOR-2";struct slcli_hmac256 mac;unsigned i;
  if(!out||!hash||!key||slot>1||!generation||!nonzero(hash)||
     !separate(out,74,hash,32)||!separate(out,74,key,32))return 0;
  out[0]=2;out[1]=slot;slcli_put_u64(out+2,generation);for(i=0;i<32;i++)out[10+i]=hash[i];
  return slcli_hmac256_init(&mac,key)&&slcli_hmac256_update(&mac,domain,sizeof(domain))&&
    slcli_hmac256_update(&mac,out,42)&&slcli_hmac256_finish(&mac,out+42);
}
int slcli_ledger_selector_read(const unsigned char raw[74],const unsigned char key[32],
                                uint8_t *slot,uint64_t *generation,unsigned char hash[32]) {
  unsigned char expected[74];struct slcli_cursor c;uint64_t n;unsigned i;
  if(!raw||!key||!slot||!generation||!hash||raw[0]!=2||raw[1]>1||
     !separate(raw,74,slot,sizeof(*slot))||!separate(raw,74,generation,sizeof(*generation))||
     !separate(raw,74,hash,32)||!separate(key,32,slot,sizeof(*slot))||
     !separate(key,32,generation,sizeof(*generation))||!separate(key,32,hash,32)||
     !separate(slot,sizeof(*slot),generation,sizeof(*generation))||
     !separate(slot,sizeof(*slot),hash,32)||!separate(generation,sizeof(*generation),hash,32))return 0;
  c.raw=raw+2;c.bytes=8;c.offset=0;c.failed=0;
  if(!slcli_u64(&c,&n)||!n||!slcli_ledger_selector(expected,raw[1],n,raw+10,key)||
     !slcli_equal(raw,expected,74))return 0;
  *slot=raw[1];*generation=n;for(i=0;i<32;i++)hash[i]=raw[10+i];return 1;
}
