#include "credential_acquire_vectors.h"
#if defined(_WIN32) || defined(__linux__)
int slcli_management_credential_native_positive(
  struct slcli_management_credential_acquisition *c,
  struct slcli_management_credential_persistence *p) {
  uint64_t result,before_tick,after_tick;size_t w,r1,r2;
  if(!c||!p||p->acquisition!=c||!p->prepared||p->started||c->started||
     !slcli_management_credential_acquire(c)||
     !slcli_management_credential_persist(p))return 0;
  if(!c->complete||c->unresolved||!c->native_called||!c->result_observed||
     !c->count_observed||c->returned_bytes!=64||!c->destination_retained||
     !p->complete||p->unresolved||!p->write_verified||!p->first_verified||!p->second_verified||
     !p->write->held_verified||!p->first->held_verified||!p->second->held_verified||
     p->write->offset!=64||p->first->offset!=64||p->second->offset!=64||
     p->first->mode!=1||p->second->mode!=1||
     !slcli_equal(p->write->digest,p->first->digest,32)||
     !slcli_equal(p->write->digest,p->second->digest,32))return 0;
  result=c->status;before_tick=c->before.tick;after_tick=c->after.tick;
  w=p->write->capture_count;r1=p->first->capture_count;r2=p->second->capture_count;
  /* Repeated calls must reject before clock/RNG/IO effects, preserving the
   * already completed real raw outputs and their exact retained row counts. */
  if(slcli_management_credential_acquire(c)||slcli_management_credential_persist(p))return 0;
  return c->complete&&!c->unresolved&&c->status==result&&
    c->before.tick==before_tick&&c->after.tick==after_tick&&p->complete&&!p->unresolved&&
    p->write->capture_count==w&&p->first->capture_count==r1&&p->second->capture_count==r2;
}
#endif
