#include "held_file_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int text(const struct slcli_build_windows_path *p) {
 size_t i;
 if(!p||!p->original||p->units<2||p->units>SIZE_MAX/sizeof(WCHAR)||
 (uintptr_t)p->original>UINTPTR_MAX-p->units*sizeof(WCHAR)||p->original[p->units-1])return 0;
 for(i=0;i<p->units-1;i++)if(!p->original[i])return 0;return 1;
}
static int paths(const struct slcli_build_windows_member *m) {
 const WCHAR *s;size_t i,j,index=0,prefix=3,n;
 if(!m||!text(&m->leaf)||m->leaf.units<5||!m->ancestors_original||!m->ancestor_count||
 m->ancestor_count>SIZE_MAX/sizeof(*m->ancestors_original))return 0;s=m->leaf.original;n=m->leaf.units-1;
 if(!((s[0]>='A'&&s[0]<='Z')||(s[0]>='a'&&s[0]<='z'))||s[1]!=':'||s[2]!='\\')return 0;
 for(i=3;i<=n;i++) {
 if(i<n&&(s[i]=='/'||s[i]==':'||s[i]=='<'||s[i]=='>'||s[i]=='"'||s[i]=='|'||s[i]=='?'||s[i]=='*'))return 0;
 if(i==n||s[i]=='\\') {
 if(i==prefix||s[i-1]=='.'||s[i-1]==' ')return 0;prefix=i+1;
 }
 }
 for(i=2;i<n;i++)if(s[i]=='\\') {
 const struct slcli_build_windows_path *p;
 size_t length=i==2?3:i;
 if(index>=m->ancestor_count)return 0;p=&m->ancestors_original[index++];
 if(!text(p)||p->units!=length+1)return 0;
 for(j=0;j<length;j++)if(p->original[j]!=s[j])return 0;
 }
 return index==m->ancestor_count;
}
static int borrowed_away(const void *out,size_t bytes,const struct slcli_build_principal_windows *p,
 const struct slcli_build_windows_member *m) {
 size_t i;
 if(!slcli_build_principal_windows_output_away_original(p,out,bytes)||
 !apart(out,bytes,m,sizeof(*m))||!apart(out,bytes,m->leaf.original,m->leaf.units*sizeof(WCHAR))||
 !apart(out,bytes,m->ancestors_original,m->ancestor_count*sizeof(*m->ancestors_original)))return 0;
 for(i=0;i<m->ancestor_count;i++)if(!apart(out,bytes,m->ancestors_original[i].original,
 m->ancestors_original[i].units*sizeof(WCHAR)))return 0;return 1;
}
static int shape(const struct slcli_build_held_file_windows *c,const struct slcli_build_principal_windows *p,
 const struct slcli_build_windows_member *m,struct slcli_build_windows_open *ancestors,
 struct slcli_windows_file_capture *reads,unsigned char *body) {
 const void *outputs[4];size_t sizes[4],i,j;uint64_t minimum;
 if(!c||!paths(m)||m->bytes>SIZE_MAX||m->bytes>UINT64_MAX/8||!m->read_capacity||
 m->ancestor_count>SIZE_MAX/sizeof(*ancestors)||(m->bytes?!body:body!=NULL)||
 m->read_capacity>SIZE_MAX/sizeof(*reads)||m->bytes==UINT64_MAX||m->read_capacity>m->bytes+1)return 0;
 minimum=m->bytes/16384+(m->bytes%16384!=0)+1;if(m->read_capacity<minimum)return 0;
 outputs[0]=c;sizes[0]=sizeof(*c);outputs[1]=ancestors;sizes[1]=m->ancestor_count*sizeof(*ancestors);
 outputs[2]=reads;sizes[2]=m->read_capacity*sizeof(*reads);outputs[3]=body;sizes[3]=(size_t)m->bytes;
 for(i=0;i<4;i++) {
 if(!sizes[i])continue;
 if(!borrowed_away(outputs[i],sizes[i],p,m))return 0;
 for(j=0;j<i;j++)if(!apart(outputs[i],sizes[i],outputs[j],sizes[j]))return 0;
 }
 return 1;
}
static int open_matches(const struct slcli_build_windows_open *o,
 const struct slcli_build_windows_path *path,int directory,uint64_t *prior,uint64_t expiry) {
 const struct slcli_build_windows_call *r=&o->call;
 size_t i;
 DWORD flags=FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OPEN_REPARSE_POINT|(directory?FILE_FLAG_BACKUP_SEMANTICS:0);
 for(i=6;i<12;i++)if(r->args[i])return 0;
 if(r->args[0]!=(uintptr_t)path->original||r->args[1]!=GENERIC_READ||r->args[2]!=FILE_SHARE_READ||
 r->args[3]||r->args[4]!=OPEN_EXISTING||r->args[5]!=flags||r->called!=1||r->returned!=1||
 !r->raw||r->raw==(uintptr_t)INVALID_HANDLE_VALUE||r->raw!=(uintptr_t)o->handle_original||
 r->error_observed||r->error||!slcli_native_clock_windows_matches_original(&r->before)||
 !slcli_native_clock_windows_matches_original(&r->after)||r->before.tick<*prior||
 r->before.tick>=expiry||r->after.tick<r->before.tick||r->after.tick>=expiry||
 (directory?!slcli_windows_directory_source_observation_matches_original(o->handle_original,&o->observation):
 !slcli_windows_source_observation_matches_original(o->handle_original,&o->observation))||
 o->observation.tick<r->after.tick||o->observation.tick>=expiry)return 0;
 *prior=o->observation.tick;return 1;
}
static int open_original(struct slcli_build_windows_open *o,
 const struct slcli_build_windows_path *path,int directory,uint64_t *prior,uint64_t expiry) {
 struct slcli_build_windows_call *r=&o->call;
 DWORD flags=FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OPEN_REPARSE_POINT|(directory?FILE_FLAG_BACKUP_SEMANTICS:0);
 r->args[0]=(uintptr_t)path->original;r->args[1]=GENERIC_READ;r->args[2]=FILE_SHARE_READ;
 r->args[3]=0;r->args[4]=OPEN_EXISTING;r->args[5]=flags;r->args[6]=0;
 if(!slcli_native_clock_read(&r->before)||!slcli_native_clock_windows_matches_original(&r->before)||
 r->before.tick<*prior||r->before.tick>=expiry)return 0;
 r->called=1;o->handle_original=CreateFileW(path->original,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,flags,NULL);
 r->raw=(uintptr_t)o->handle_original;r->returned=1;
 if(o->handle_original==INVALID_HANDLE_VALUE){r->error=GetLastError();r->error_observed=1;}
 if(!slcli_native_clock_read(&r->after)||!slcli_native_clock_windows_matches_original(&r->after)||
 r->after.tick<r->before.tick||r->after.tick>=expiry||!o->handle_original||o->handle_original==INVALID_HANDLE_VALUE)return 0;
 if(directory?!slcli_windows_directory_source_observe(o->handle_original,&o->observation):
 !slcli_windows_source_observe(o->handle_original,&o->observation))return 0;
 return open_matches(o,path,directory,prior,expiry);
}
static int metadata_equal(const struct slcli_windows_file_capture *a,const struct slcli_windows_file_capture *b) {
 size_t i;
 if(a->identity.VolumeSerialNumber!=b->identity.VolumeSerialNumber||
 a->standard.EndOfFile.QuadPart!=b->standard.EndOfFile.QuadPart||
 a->basic.CreationTime.QuadPart!=b->basic.CreationTime.QuadPart||
 a->basic.LastWriteTime.QuadPart!=b->basic.LastWriteTime.QuadPart||
 a->basic.ChangeTime.QuadPart!=b->basic.ChangeTime.QuadPart||
 a->attributes.FileAttributes!=b->attributes.FileAttributes||a->attributes.ReparseTag!=b->attributes.ReparseTag||
 a->owner_sid_bytes!=b->owner_sid_bytes)return 0;
 for(i=0;i<sizeof(a->identity.FileId.Identifier);i++)if(a->identity.FileId.Identifier[i]!=b->identity.FileId.Identifier[i])return 0;
 for(i=0;i<a->owner_sid_bytes;i++)if(a->owner_sid[i]!=b->owner_sid[i])return 0;return 1;
}
static int contents(const struct slcli_build_held_file_windows *c,uint64_t *last) {
 const struct slcli_build_windows_member *m;const struct slcli_windows_file_capture *r;
 uint64_t prior,offset=0,remaining;DWORD requested;size_t i;struct slcli_sha256 hash;unsigned char digest[32];
 if(!c||!shape(c,c->principal_original,c->member_original,c->ancestors_original,c->reads_original,c->body_original)||
 c->started!=1||c->failed||c->completed!=1||c->expires!=c->principal_original->expires||
 c->prior_tick!=c->principal_original->finish.tick)return 0;
 m=c->member_original;prior=c->prior_tick;
 if(c->ancestors_used!=m->ancestor_count||!c->reads_used||c->reads_used>m->read_capacity)return 0;
 for(i=0;i<c->ancestors_used;i++)if(!open_matches(&c->ancestors_original[i],&m->ancestors_original[i],1,&prior,c->expires))return 0;
 if(!open_matches(&c->leaf,&m->leaf,0,&prior,c->expires)||c->leaf.observation.standard.EndOfFile.QuadPart!=(int64_t)m->bytes)return 0;
 for(i=0;i<c->reads_used;i++) {
 r=&c->reads_original[i];remaining=m->bytes-offset;requested=(DWORD)(remaining>16384?16384:remaining);
 if(!remaining) {
 if(i+1!=c->reads_used||!slcli_windows_file_read_matches_original(c->leaf.handle_original,&c->eof_original,1,
 offset,r)||r->returned_bytes||r->tick<prior||r->tick>=c->expires)return 0;
 }else if(!slcli_windows_file_read_matches_original(c->leaf.handle_original,c->body_original+offset,requested,
 offset,r)||!r->returned_bytes||r->returned_bytes>requested||r->tick<prior||r->tick>=c->expires)return 0;
 offset+=r->returned_bytes;prior=r->tick;
 }
 if(offset!=m->bytes||c->used_bytes!=offset||
 c->reads_original[c->reads_used-1].requested!=1||c->reads_original[c->reads_used-1].offset!=m->bytes||
 !slcli_windows_source_observation_matches_original(c->leaf.handle_original,&c->final_original)||
 c->final_original.tick<prior||c->final_original.tick>=c->expires||
 !metadata_equal(&c->leaf.observation,&c->final_original))return 0;
 slcli_sha256_init(&hash);if(!slcli_sha256_update(&hash,c->body_original,(size_t)m->bytes)||
 !slcli_sha256_finish(&hash,digest)||!slcli_equal(digest,m->digest,32)||
 !slcli_equal(digest,c->digest_original,32))return 0;
 if(c->hash_original.bytes!=hash.bytes||c->hash_original.used!=hash.used||
 c->hash_original.finished!=hash.finished||c->hash_original.failed!=hash.failed)return 0;
 for(i=0;i<8;i++)if(c->hash_original.state[i]!=hash.state[i])return 0;
 for(i=0;i<64;i++)if(c->hash_original.block[i]!=hash.block[i])return 0;
 *last=c->final_original.tick;return 1;
}
int slcli_build_held_file_windows_matches_original(const struct slcli_build_held_file_windows *c) {
 uint64_t prior;return contents(c,&prior)&&slcli_native_clock_windows_matches_original(&c->finish)&&
 c->finish.tick>=prior&&c->finish.tick<c->expires;
}
int slcli_build_held_file_windows_capture_original(struct slcli_build_held_file_windows *c,
 const struct slcli_build_principal_windows *p,const struct slcli_build_windows_member *m,
 struct slcli_build_windows_open *ancestors,struct slcli_windows_file_capture *reads,unsigned char *body) {
 size_t i;uint64_t prior,remaining;DWORD requested;struct slcli_windows_file_capture *r;
 if(!shape(c,p,m,ancestors,reads,body)||!zero(c,sizeof(*c))||!zero(ancestors,m->ancestor_count*sizeof(*ancestors))||
 !zero(reads,m->read_capacity*sizeof(*reads))||!zero(body,(size_t)m->bytes))return 0;
 c->principal_original=p;c->member_original=m;c->ancestors_original=ancestors;c->reads_original=reads;
 c->body_original=body;c->prior_tick=p->finish.tick;c->expires=p->expires;c->started=1;c->failed=1;prior=c->prior_tick;
 for(i=0;i<m->ancestor_count;i++) {
 c->ancestors_used=i+1;if(!open_original(&ancestors[i],&m->ancestors_original[i],1,&prior,c->expires))return 0;
 }
 if(!open_original(&c->leaf,&m->leaf,0,&prior,c->expires)||c->leaf.observation.standard.EndOfFile.QuadPart!=(int64_t)m->bytes)return 0;
 for(;;) {
 if(c->reads_used==m->read_capacity)return 0;r=&reads[c->reads_used++];remaining=m->bytes-c->used_bytes;
 requested=(DWORD)(remaining>16384?16384:remaining);
 if(!remaining) {
 if(!slcli_windows_file_read(c->leaf.handle_original,&c->eof_original,1,c->used_bytes,r)||
 !slcli_windows_file_read_matches_original(c->leaf.handle_original,&c->eof_original,1,c->used_bytes,r)||
 r->returned_bytes||r->tick<prior||r->tick>=c->expires)return 0;prior=r->tick;break;
 }
 if(!slcli_windows_file_read(c->leaf.handle_original,body+c->used_bytes,requested,c->used_bytes,r)||
 !slcli_windows_file_read_matches_original(c->leaf.handle_original,body+c->used_bytes,requested,c->used_bytes,r)||
 !r->returned_bytes||r->returned_bytes>requested||r->tick<prior||r->tick>=c->expires)return 0;
 c->used_bytes+=r->returned_bytes;prior=r->tick;
 }
 if(!slcli_windows_source_observe(c->leaf.handle_original,&c->final_original)||
 !slcli_windows_source_observation_matches_original(c->leaf.handle_original,&c->final_original)||
 c->final_original.tick<prior||c->final_original.tick>=c->expires||!metadata_equal(&c->leaf.observation,&c->final_original))return 0;
 slcli_sha256_init(&c->hash_original);
 if(!slcli_sha256_update(&c->hash_original,body,(size_t)m->bytes)||
 !slcli_sha256_finish(&c->hash_original,c->digest_original)||!slcli_equal(c->digest_original,m->digest,32))return 0;
 c->completed=1;c->failed=0;if(!contents(c,&prior)){c->failed=1;return 0;}c->failed=1;
 if(!slcli_native_clock_read(&c->finish)||!slcli_native_clock_windows_matches_original(&c->finish)||
 c->finish.tick<prior||c->finish.tick>=c->expires)return 0;c->failed=0;return 1;
}
#endif
