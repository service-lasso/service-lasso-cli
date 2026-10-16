#include "loaded_file_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static const struct slcli_build_held_file_windows *tool(const struct slcli_build_child_principal_windows *child) {
 return child->birth_original->source_original->executable_original;
}
static int shape(const struct slcli_build_loaded_file_windows *c,
 const struct slcli_build_child_principal_windows *child,struct slcli_windows_file_capture *reads,struct slcli_native_clock *cuts,unsigned char *body) {
 const struct slcli_build_windows_member *m;const void *outputs[4];size_t sizes[4],i,j;
 if(!child||!slcli_build_child_principal_windows_matches_original(child))return 0;m=tool(child)->member_original;
 if(m->read_capacity>SIZE_MAX/sizeof(*cuts))return 0;
 outputs[0]=c;sizes[0]=sizeof(*c);outputs[1]=reads;sizes[1]=m->read_capacity*sizeof(*reads);
 outputs[2]=cuts;sizes[2]=m->read_capacity*sizeof(*cuts);
 outputs[3]=body;sizes[3]=(size_t)m->bytes;
 if(m->bytes?!body:body!=NULL)return 0;
 for(i=0;i<4;i++) {
 if(!sizes[i])continue;
 if(!slcli_build_child_principal_windows_output_away_original(child,outputs[i],sizes[i]))return 0;
 for(j=0;j<i;j++)if(!apart(outputs[i],sizes[i],outputs[j],sizes[j]))return 0;
 }
 return 1;
}
static int issuing_thread_matches(const struct slcli_build_loaded_file_windows *c,uint64_t *prior) {
 const struct slcli_build_windows_call *r=&c->issuing_thread;size_t i;
 if(r->called!=1||r->returned!=1||r->raw!=c->child_original->birth_original->issuing_thread_original||
 r->error_observed||r->error||!slcli_native_clock_windows_matches_original(&r->before)||
 !slcli_native_clock_windows_matches_original(&r->after)||r->before.tick<*prior||
 r->before.tick>=c->expires||r->after.tick<r->before.tick||r->after.tick>=c->expires)return 0;
 for(i=0;i<12;i++)if(r->args[i])return 0;*prior=r->after.tick;return 1;
}
static int contents(const struct slcli_build_loaded_file_windows *c,uint64_t *last) {
 const struct slcli_build_child_principal_windows *child;const struct slcli_build_held_file_windows *held;
 const struct slcli_build_windows_member *m;const struct slcli_windows_file_capture *r;
 struct slcli_sha256 hash;unsigned char digest[32];uint64_t prior,offset=0,remaining;
 DWORD requested;size_t i;
 if(!c||!shape(c,c->child_original,c->reads_original,c->before_reads_original,c->body_original)||
 c->started!=1||c->failed||c->completed!=1)return 0;child=c->child_original;held=tool(child);m=held->member_original;
 if(c->prior_tick!=child->finish.tick||c->expires!=child->expires||c->bytes_original!=m->bytes||
 c->event_file_original!=child->birth_original->first_event_original.u.CreateProcessInfo.hFile||
 !c->reads_used||c->reads_used>m->read_capacity)return 0;prior=c->prior_tick;
 if(!issuing_thread_matches(c,&prior))return 0;
 if(!slcli_build_held_file_windows_observation_matches_original(held,c->event_file_original,&c->before_original)||
 !slcli_native_clock_windows_matches_original(&c->before_observe[0])||
 c->before_observe[0].tick<prior||c->before_observe[0].tick>=c->expires||
 c->before_original.tick<c->before_observe[0].tick||c->before_original.tick>=c->expires)return 0;prior=c->before_original.tick;
 for(i=0;i<c->reads_used;i++) {
 if(!slcli_native_clock_windows_matches_original(&c->before_reads_original[i])||
 c->before_reads_original[i].tick<prior||c->before_reads_original[i].tick>=c->expires)return 0;
 prior=c->before_reads_original[i].tick;
 r=&c->reads_original[i];remaining=m->bytes-offset;requested=(DWORD)(remaining>16384?16384:remaining);
 if(!remaining) {
 if(i+1!=c->reads_used||!slcli_windows_file_read_matches_original(c->event_file_original,&c->eof_original,1,
 offset,r)||r->returned_bytes||r->tick<prior||r->tick>=c->expires)return 0;
 }else if(!slcli_windows_file_read_matches_original(c->event_file_original,c->body_original+offset,requested,
 offset,r)||!r->returned_bytes||r->returned_bytes>requested||r->tick<prior||r->tick>=c->expires)return 0;
 offset+=r->returned_bytes;prior=r->tick;
 }
 if(offset!=m->bytes||c->used_bytes!=offset||
 c->reads_original[c->reads_used-1].requested!=1||c->reads_original[c->reads_used-1].offset!=m->bytes||
 !slcli_build_held_file_windows_observation_matches_original(held,c->event_file_original,&c->after_original)||
 !slcli_native_clock_windows_matches_original(&c->before_observe[1])||
 c->before_observe[1].tick<prior||c->before_observe[1].tick>=c->expires||
 c->after_original.tick<c->before_observe[1].tick||c->after_original.tick>=c->expires)return 0;
 for(i=0;i<(size_t)m->bytes;i++)if(c->body_original[i]!=held->body_original[i])return 0;
 slcli_sha256_init(&hash);if(!slcli_sha256_update(&hash,c->body_original,(size_t)m->bytes)||
 !slcli_sha256_finish(&hash,digest)||!slcli_equal(digest,m->digest,32)||
 !slcli_equal(digest,c->digest_original,32)||c->hash_original.bytes!=hash.bytes||
 c->hash_original.used!=hash.used||c->hash_original.finished!=hash.finished||c->hash_original.failed!=hash.failed)return 0;
 for(i=0;i<8;i++)if(c->hash_original.state[i]!=hash.state[i])return 0;
 for(i=0;i<64;i++)if(c->hash_original.block[i]!=hash.block[i])return 0;
 *last=c->after_original.tick;return 1;
}
int slcli_build_loaded_file_windows_matches_original(const struct slcli_build_loaded_file_windows *c) {
 uint64_t prior;return contents(c,&prior)&&slcli_native_clock_windows_matches_original(&c->finish)&&
 c->finish.tick>=prior&&c->finish.tick<c->expires;
}
int slcli_build_loaded_file_windows_capture_original(struct slcli_build_loaded_file_windows *c,
 const struct slcli_build_child_principal_windows *child,struct slcli_windows_file_capture *reads,struct slcli_native_clock *cuts,unsigned char *body) {
 const struct slcli_build_held_file_windows *held;const struct slcli_build_windows_member *m;
 struct slcli_windows_file_capture *r;struct slcli_build_windows_call *thread_row;uint64_t prior,remaining;DWORD requested;size_t i;
 if(!shape(c,child,reads,cuts,body))return 0;held=tool(child);m=held->member_original;
 if(!zero(c,sizeof(*c))||!zero(reads,m->read_capacity*sizeof(*reads))||
 !zero(cuts,m->read_capacity*sizeof(*cuts))||!zero(body,(size_t)m->bytes))return 0;
 c->child_original=child;c->event_file_original=child->birth_original->first_event_original.u.CreateProcessInfo.hFile;
 c->reads_original=reads;c->before_reads_original=cuts;c->body_original=body;c->bytes_original=m->bytes;c->prior_tick=child->finish.tick;
 c->expires=child->expires;c->started=1;c->failed=1;prior=c->prior_tick;
 thread_row=&c->issuing_thread;
 if(!slcli_native_clock_read(&thread_row->before)||
 !slcli_native_clock_windows_matches_original(&thread_row->before)||
 thread_row->before.tick<prior||thread_row->before.tick>=c->expires)return 0;
 thread_row->called=1;thread_row->raw=GetCurrentThreadId();thread_row->returned=1;
 if(!slcli_native_clock_read(&thread_row->after)||!issuing_thread_matches(c,&prior))return 0;
 if(!slcli_native_clock_read(&c->before_observe[0])||
 !slcli_native_clock_windows_matches_original(&c->before_observe[0])||
 c->before_observe[0].tick<prior||c->before_observe[0].tick>=c->expires)return 0;
 if(!slcli_windows_source_observe(c->event_file_original,&c->before_original)||
 !slcli_build_held_file_windows_observation_matches_original(held,c->event_file_original,&c->before_original)||
 !slcli_native_clock_windows_matches_original(&c->before_observe[0])||
 c->before_observe[0].tick<prior||c->before_observe[0].tick>=c->expires||
 c->before_original.tick<c->before_observe[0].tick||c->before_original.tick>=c->expires)return 0;prior=c->before_original.tick;
 for(;;) {
 if(c->reads_used==m->read_capacity)return 0;
 if(!slcli_native_clock_read(&cuts[c->reads_used])||
 !slcli_native_clock_windows_matches_original(&cuts[c->reads_used])||
 cuts[c->reads_used].tick<prior||cuts[c->reads_used].tick>=c->expires)return 0;
 prior=cuts[c->reads_used].tick;r=&reads[c->reads_used++];remaining=m->bytes-c->used_bytes;
 requested=(DWORD)(remaining>16384?16384:remaining);
 if(!remaining) {
 if(!slcli_windows_file_read(c->event_file_original,&c->eof_original,1,c->used_bytes,r)||
 !slcli_windows_file_read_matches_original(c->event_file_original,&c->eof_original,1,c->used_bytes,r)||
 r->returned_bytes||r->tick<prior||r->tick>=c->expires)return 0;prior=r->tick;break;
 }
 if(!slcli_windows_file_read(c->event_file_original,body+c->used_bytes,requested,c->used_bytes,r)||
 !slcli_windows_file_read_matches_original(c->event_file_original,body+c->used_bytes,requested,c->used_bytes,r)||
 !r->returned_bytes||r->returned_bytes>requested||r->tick<prior||r->tick>=c->expires)return 0;
 c->used_bytes+=r->returned_bytes;prior=r->tick;
 }
 if(!slcli_native_clock_read(&c->before_observe[1])||
 !slcli_native_clock_windows_matches_original(&c->before_observe[1])||
 c->before_observe[1].tick<prior||c->before_observe[1].tick>=c->expires)return 0;
 if(!slcli_windows_source_observe(c->event_file_original,&c->after_original)||
 !slcli_build_held_file_windows_observation_matches_original(held,c->event_file_original,&c->after_original)||
 !slcli_native_clock_windows_matches_original(&c->before_observe[1])||
 c->before_observe[1].tick<prior||c->before_observe[1].tick>=c->expires||
 c->after_original.tick<c->before_observe[1].tick||c->after_original.tick>=c->expires)return 0;
 for(i=0;i<(size_t)m->bytes;i++)if(body[i]!=held->body_original[i])return 0;
 slcli_sha256_init(&c->hash_original);
 if(!slcli_sha256_update(&c->hash_original,body,(size_t)m->bytes)||
 !slcli_sha256_finish(&c->hash_original,c->digest_original)||!slcli_equal(c->digest_original,m->digest,32))return 0;
 c->completed=1;c->failed=0;if(!contents(c,&prior)){c->failed=1;return 0;}c->failed=1;
 if(!slcli_native_clock_read(&c->finish)||!slcli_native_clock_windows_matches_original(&c->finish)||
 c->finish.tick<prior||c->finish.tick>=c->expires)return 0;c->failed=0;return 1;
}
#endif
