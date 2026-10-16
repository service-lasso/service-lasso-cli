#include "bq_receive_linux.h"
#if defined(__linux__)
static int separate(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&!(x<y+m&&y<x+n);
}
/* SOURCE-UNRUN controlled non-native fixture, NOT a mutation API for genuine
 * retained native originals. Caller supplies synthetically successful receive
 * and claimed local fixture storage; no syscall or clock is invoked here.
 * Positive control must satisfy the actual matcher before the sole final-clock
 * field is moved to the original expiry boundary. The constructor uses that
 * SAME validation predicate before publishing raw complete. */
int slcli_bq_receive_linux_late_validation_vector(
 struct slcli_bq_receive_linux_original *fixture,unsigned char *before,size_t bytes) {
 const unsigned char *parts[3];size_t sizes[3],total,i,j,k;struct slcli_arena *a;
 struct slcli_binding_storage_original *s;struct slcli_binding_storage_geometry g;
 const void *protected[12];size_t protected_bytes[12];
 if(!fixture||!slcli_bq_receive_linux_matches_original(fixture))return 0;
 s=fixture->storage;a=s->arena;
 if(!slcli_binding_storage_geometry_derive(s->capacity,&g))return 0;
 parts[0]=fixture->payload;sizes[0]=sizeof(fixture->payload);
 parts[1]=fixture->control.bytes;sizes[1]=sizeof(fixture->control.bytes);
 parts[2]=(const void *)fixture->received_fds;sizes[2]=sizeof(fixture->received_fds);
 total=sizes[0]+sizes[1]+sizes[2];
 if(bytes!=total||!slcli_arena_live_block(a,before,bytes)||
  (uintptr_t)before>UINTPTR_MAX-bytes||
  (uintptr_t)fixture>UINTPTR_MAX-sizeof(*fixture)||
  ((uintptr_t)before<(uintptr_t)fixture+sizeof(*fixture)&&
   (uintptr_t)fixture<(uintptr_t)before+bytes))return 0;
 protected[0]=s;protected_bytes[0]=sizeof(*s);protected[1]=s->book;protected_bytes[1]=sizeof(*s->book);
 protected[2]=s->references;protected_bytes[2]=g.references_bytes;
 protected[3]=s->ids;protected_bytes[3]=g.ids_bytes;protected[4]=s->indices;protected_bytes[4]=g.indices_bytes;
 protected[5]=s->bitmap;protected_bytes[5]=g.bitmap_bytes;protected[6]=s->snapshot;protected_bytes[6]=g.snapshot_bytes;
 protected[7]=s->claims;protected_bytes[7]=g.claims_bytes;
 protected[8]=fixture->caller;protected_bytes[8]=fixture->caller_bytes;
 protected[9]=fixture->deadline;protected_bytes[9]=sizeof(*fixture->deadline);
 protected[10]=fixture->prior;protected_bytes[10]=sizeof(*fixture->prior);
 protected[11]=fixture->origin;protected_bytes[11]=sizeof(*fixture->origin);
 for(i=0;i<12;i++)if(!separate(before,bytes,protected[i],protected_bytes[i]))return 0;
 for(i=0,k=0;i<3;i++)for(j=0;j<sizes[i];j++)before[k++]=parts[i][j];
 /* Immediate post-recv clock remains genuinely in-D in this source vector. */
 if(fixture->after.tick>=fixture->expires)return 0;
 fixture->validation_after.tick=fixture->expires;
 if(slcli_bq_receive_linux_matches_original(fixture))return 0;
 for(i=0,k=0;i<3;i++)for(j=0;j<sizes[i];j++)if(parts[i][j]!=before[k++])return 0;
 return fixture->validation_after.tick==fixture->expires;
}
#endif
