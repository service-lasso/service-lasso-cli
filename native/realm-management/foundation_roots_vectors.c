#include "foundation_roots_vectors.h"
#if defined(_WIN32) || defined(__linux__)
int slcli_management_foundation_roots_native_positive(struct slcli_management_foundation_roots *r) {
  uint64_t tick;size_t i;
  if(!r||r->started||!slcli_management_foundation_roots_observe(r)||
     !r->complete||r->unresolved||r->rows_attempted!=10||r->rows_preserved!=10)return 0;
  for(i=0;i<20;i++)if(!r->clocks[i].started||!r->clocks[i].observed||
    !r->clocks[i].complete)return 0;
  tick=r->previous_tick;
  if(slcli_management_foundation_roots_observe(r))return 0;
  return r->complete&&!r->unresolved&&r->rows_attempted==10&&r->rows_preserved==10&&
    r->previous_tick==tick;
}
#endif
