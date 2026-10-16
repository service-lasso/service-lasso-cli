#include "ha2_transition.h"
static int hash(const unsigned char *raw,size_t bytes,unsigned char out[32]) {
  struct slcli_sha256 h;slcli_sha256_init(&h);
  return slcli_sha256_update(&h,raw,bytes)&&slcli_sha256_finish(&h,out);
}
static int nonzero(const unsigned char p[32]) {
  unsigned char n=0;unsigned i;for(i=0;i<32;i++)n|=p[i];return n!=0;
}
static int same(const struct slcli_ha2_journal_record *a,
  const struct slcli_ha2_journal_record *b,unsigned field) {
  return slcli_equal(a->associations[field],b->associations[field],32);
}
int slcli_ha2_transition_compare(const unsigned char *prior,size_t prior_bytes,
  const unsigned char *operation,size_t operation_bytes,const unsigned char *next,size_t next_bytes) {
  struct slcli_ha2_journal_record p,n;struct slcli_ha2_operation o;
  unsigned char raw_hash[32];unsigned i;uint32_t desired,changed=0;
  if(!slcli_ha2_operation_parse(operation,operation_bytes,&o)||
     !slcli_ha2_journal_record_parse(next,next_bytes,&n)||
     o.sequence!=n.sequence||o.generation!=n.generation||
     !hash(operation,operation_bytes,raw_hash)||!slcli_equal(raw_hash,n.associations[11],32)||
     !slcli_equal(o.birth,n.associations[12],32))return 0;
  if(n.sequence==1) {
    /* The actual native source must independently prove lineage UNBORN and
     * field15..17 preexisting applicability; absence of prior bytes cannot. */
    return !prior&&!prior_bytes&&o.event==SLCLI_HA2_INSTALL_PREPARE&&
      n.state==SLCLI_HA2_PREPARED&&!nonzero(n.associations[13]);
  }
  if(!slcli_ha2_journal_record_parse(prior,prior_bytes,&p)||p.sequence>=1024||
     n.sequence!=p.sequence+1||!hash(prior,prior_bytes,raw_hash)||
     !slcli_equal(raw_hash,n.associations[17],32)||
     !slcli_equal(raw_hash,o.prior_record,32)||
     !slcli_equal(o.prior_request,p.associations[11],32)||
     same(&p,&n,11)||p.key_version_bytes!=n.key_version_bytes||
     !slcli_equal(p.key_version,n.key_version,p.key_version_bytes))return 0;
  for(i=0;i<11;i++)if(!same(&p,&n,i))return 0;
  if(o.event!=SLCLI_HA2_NEXT_START_PREPARE&&!same(&p,&n,12))return 0;
  if(p.state==SLCLI_HA2_QUARANTINED)return 0;
  desired=p.state;
  switch(o.event) {
    case SLCLI_HA2_ACTIVATE:
      if(p.state!=SLCLI_HA2_PREPARED||nonzero(p.associations[13]))return 0;
      desired=SLCLI_HA2_ACTIVE;break;
    case SLCLI_HA2_LEASE_BIRTH:
      if(p.state!=SLCLI_HA2_ACTIVE||nonzero(p.associations[13])||
         !nonzero(n.associations[13]))return 0;
      changed=1u<<13;break;
    case SLCLI_HA2_LEDGER_BIRTH:
      if(p.state!=SLCLI_HA2_ACTIVE||!nonzero(p.associations[13])||
         !nonzero(n.associations[15]))return 0;
      changed=1u<<15;break;
    case SLCLI_HA2_BORROW_OPEN:
      if(p.state!=SLCLI_HA2_ACTIVE||!nonzero(p.associations[13])||
         !nonzero(p.associations[15])||!nonzero(n.associations[14]))return 0;
      changed=(1u<<14)|(1u<<15);break;
    case SLCLI_HA2_BORROW_CLOSE:
      if((p.state!=SLCLI_HA2_ACTIVE&&p.state!=SLCLI_HA2_REVOKING)||
         !nonzero(p.associations[14])||!nonzero(n.associations[14]))return 0;
      changed=(1u<<14)|(1u<<15);break;
    case SLCLI_HA2_ARCHIVE:
      if((p.state!=SLCLI_HA2_ACTIVE&&p.state!=SLCLI_HA2_REVOKING)||
         !nonzero(n.associations[16]))return 0;
      changed=(1u<<14)|(1u<<15)|(1u<<16);break;
    case SLCLI_HA2_NEXT_START_PREPARE:
      if(p.state!=SLCLI_HA2_ACTIVE||p.generation==UINT64_MAX||
         !nonzero(p.associations[16])||nonzero(n.associations[13]))return 0;
      desired=SLCLI_HA2_PREPARED;changed=1u<<13;break;
    case SLCLI_HA2_REVOKE_BEGIN:
      if(p.state!=SLCLI_HA2_PREPARED&&p.state!=SLCLI_HA2_ACTIVE)return 0;
      desired=SLCLI_HA2_REVOKING;break;
    case SLCLI_HA2_REVOKE_COMPLETE:
      if(p.state!=SLCLI_HA2_REVOKING||!nonzero(n.associations[16]))return 0;
      desired=SLCLI_HA2_REVOKED;changed=(1u<<14)|(1u<<15)|(1u<<16);break;
    case SLCLI_HA2_QUARANTINE:
      desired=SLCLI_HA2_QUARANTINED;changed=(1u<<13)|(1u<<14)|(1u<<15)|(1u<<16);break;
    default:return 0;
  }
  if(n.state!=desired||n.generation!=p.generation+
     (o.event==SLCLI_HA2_NEXT_START_PREPARE?1u:0u))return 0;
  for(i=13;i<17;i++) {
    if(!(changed&(1u<<i))&&!same(&p,&n,i))return 0;
    if(nonzero(p.associations[i])&&!nonzero(n.associations[i])&&
       !(i==13&&o.event==SLCLI_HA2_NEXT_START_PREPARE))return 0;
  }
  return 1;
}
