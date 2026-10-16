#include "seed_compile_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int path(const WCHAR *p,size_t n) {
 size_t i,start=3;
 if(!p||n<5||n>32767||n>SIZE_MAX/sizeof(*p)||(uintptr_t)p>UINTPTR_MAX-n*sizeof(*p)||p[n-1]||
 !((p[0]>='A'&&p[0]<='Z')||(p[0]>='a'&&p[0]<='z'))||p[1]!=':'||p[2]!='\\'||p[n-2]=='\\')return 0;
 for(i=0;i<n-1;i++)if(!p[i]||p[i]<32||p[i]=='"'||p[i]=='/'||p[i]=='<'||p[i]=='>'||p[i]=='|'||p[i]=='?'||p[i]=='*'||(i!=1&&p[i]==':'))return 0;
 for(i=3;i<n;i++)if(i==n-1||p[i]=='\\') {
 if(i==start||p[i-1]=='.'||p[i-1]==' ')return 0;start=i+1;
 }
 return 1;
}
static int tail(const struct slcli_build_windows_path *p,const WCHAR *name,size_t count) {
 size_t i;if(!path(p->original,p->units)||p->units<count+2||p->original[p->units-count-2]!='\\')return 0;
 for(i=0;i<count;i++)if(p->original[p->units-count-1+i]!=name[i])return 0;return 1;
}
static int add(WCHAR *p,size_t capacity,size_t *used,const WCHAR *text,size_t n) {
 size_t i;if(*used>=capacity||n>capacity-*used-1)return 0;
 for(i=0;i<n;i++)p[(*used)++]=text[i];p[*used]=0;return 1;
}
static int quote(WCHAR *p,size_t capacity,size_t *used,const WCHAR *text,size_t n) {
 return path(text,n)&&add(p,capacity,used,L"\"",1)&&add(p,capacity,used,text,n-1)&&add(p,capacity,used,L"\"",1);
}
static int deny(struct slcli_build_seed_compile_windows *c,struct slcli_build_bootstrap_windows *b) {
 c->failed=1;b->failed=1;b->memory_original.failed=1;return 0;
}
int slcli_build_seed_compile_windows_birth_original(struct slcli_build_seed_compile_windows *c,
 struct slcli_build_bootstrap_windows *b,const struct slcli_build_seed_compile_windows_source *s) {
 static const WCHAR flags[]=L" /nologo /TC /std:c11 /O2 /GS /MT /W4 /c ";
 struct slcli_arena *a;const struct slcli_build_windows_member *compiler;size_t i,used=0;
 static const WCHAR object_suffix[]=L"\\bootstrap_windows.obj";
 if(!c||!b||!s||!b->completed||b->failed||b->memory_original.failed||!b->compiler_original||
 !slcli_build_held_file_windows_matches_original(b->compiler_original)||
 !s->node_tuple_original||!s->translation_unit_original||!slcli_build_held_file_windows_matches_original(s->translation_unit_original)||
 !slcli_arena_live_block(&b->arena_original,c,sizeof(*c))||!zero(c,sizeof(*c))||
 !apart(c,sizeof(*c),s,sizeof(*s))||!apart(c,sizeof(*c),b,sizeof(*b))||
 !slcli_build_node_abi_windows_source_output_away_original(s->node_tuple_original,c,sizeof(*c))||
 !slcli_build_held_file_windows_output_away_original(s->translation_unit_original,c,sizeof(*c))||
 !slcli_build_held_file_windows_output_away_original(b->compiler_original,c,sizeof(*c))||
 !tail(&s->translation_unit_original->member_original->leaf,L"bootstrap_windows.c",19)||
 !tail(&b->compiler_original->member_original->leaf,L"cl.exe",6)||
 !path(s->object_original,s->object_units)||!path(s->cwd_original,s->cwd_units)||
 s->object_units!=s->cwd_units+sizeof(object_suffix)/sizeof(object_suffix[0])-1||
 !s->environment_original||s->environment_units<2||s->environment_units>SIZE_MAX/sizeof(WCHAR)||
 !s->includes_original||!s->include_count||s->include_count>SIZE_MAX/sizeof(*s->includes_original)||
 !s->command_capacity||s->command_capacity>32767||!s->attribute_capacity||!s->pipe_capacity||
 !s->stream_poll_capacity||!s->stream_byte_capacity||
 !apart(c,sizeof(*c),s->object_original,s->object_units*sizeof(WCHAR))||
 !apart(c,sizeof(*c),s->cwd_original,s->cwd_units*sizeof(WCHAR))||
 !apart(c,sizeof(*c),s->environment_original,s->environment_units*sizeof(WCHAR))||
 !apart(c,sizeof(*c),s->includes_original,s->include_count*sizeof(*s->includes_original)))return 0;
 for(i=0;i<s->cwd_units-1;i++)if(s->object_original[i]!=s->cwd_original[i])return 0;
 for(i=0;i<sizeof(object_suffix)/sizeof(object_suffix[0]);i++)
 if(s->object_original[s->cwd_units-1+i]!=object_suffix[i])return 0;
 for(i=0;i<s->include_count;i++)if(!path(s->includes_original[i].original,s->includes_original[i].units)||
 !apart(c,sizeof(*c),s->includes_original[i].original,s->includes_original[i].units*sizeof(WCHAR)))return 0;
 compiler=b->compiler_original->member_original;
 if(compiler->read_capacity>SIZE_MAX/sizeof(*c->image_reads_original)||
 compiler->read_capacity>SIZE_MAX/sizeof(*c->image_clocks_original)||compiler->bytes>SIZE_MAX)return 0;
 c->bootstrap_original=b;c->source_original=s;c->started=1;c->failed=1;a=&b->arena_original;
 c->command_original_bytes=slcli_arena_calloc(a,s->command_capacity,sizeof(WCHAR));
 c->command_mutable_original=slcli_arena_calloc(a,s->command_capacity,sizeof(WCHAR));
 c->attributes_original=slcli_arena_calloc(a,s->attribute_capacity,1);
 c->image_reads_original=slcli_arena_calloc(a,compiler->read_capacity,sizeof(*c->image_reads_original));
 c->image_clocks_original=slcli_arena_calloc(a,compiler->read_capacity,sizeof(*c->image_clocks_original));
 c->image_body_original=slcli_arena_calloc(a,(size_t)compiler->bytes,1);
 c->mapping_original=slcli_arena_calloc(a,1,sizeof(*c->mapping_original));
 c->node_tuple_original=slcli_arena_calloc(a,1,sizeof(*c->node_tuple_original));
 c->node_loaded_original=slcli_arena_calloc(a,1,sizeof(*c->node_loaded_original));
 c->streams_original=slcli_arena_calloc(a,1,sizeof(*c->streams_original));
 if(!c->command_original_bytes||!c->command_mutable_original||!c->attributes_original||
 !c->image_reads_original||!c->image_clocks_original||!c->image_body_original||!c->mapping_original||!c->node_tuple_original||!c->node_loaded_original||!c->streams_original||
 !slcli_build_streams_windows_prepare_original(c->streams_original,a,s->stream_poll_capacity,s->stream_byte_capacity)||
 !slcli_build_remote_image_windows_prepare_original(c->mapping_original,a,b->compiler_original,
 b->pages_original.dwPageSize))return deny(c,b);
 if(!quote(c->command_original_bytes,s->command_capacity,&used,compiler->leaf.original,compiler->leaf.units)||
 !add(c->command_original_bytes,s->command_capacity,&used,flags,sizeof(flags)/sizeof(flags[0])-1)||
 !quote(c->command_original_bytes,s->command_capacity,&used,s->translation_unit_original->member_original->leaf.original,
 s->translation_unit_original->member_original->leaf.units)||
 !add(c->command_original_bytes,s->command_capacity,&used,L" /Fo",4)||
 !quote(c->command_original_bytes,s->command_capacity,&used,s->object_original,s->object_units))return deny(c,b);
 for(i=0;i<s->include_count;i++)if(!add(c->command_original_bytes,s->command_capacity,&used,L" /I",3)||
 !quote(c->command_original_bytes,s->command_capacity,&used,s->includes_original[i].original,s->includes_original[i].units))return deny(c,b);
 c->command_source_original.executable_original=b->compiler_original;
 c->command_source_original.process_primary_original=b->primary_original;
 c->command_source_original.command_original=c->command_original_bytes;c->command_source_original.command_units=used+1;
 c->command_source_original.environment_original=s->environment_original;c->command_source_original.environment_units=s->environment_units;
 c->command_source_original.cwd_original=s->cwd_original;c->command_source_original.cwd_units=s->cwd_units;
 c->command_source_original.attribute_capacity=s->attribute_capacity;c->command_source_original.pipe_capacity=s->pipe_capacity;
 if(!slcli_build_node_abi_windows_capture_original(c->node_tuple_original,b,s->node_tuple_original)||
 !slcli_build_node_loaded_windows_capture_original(c->node_loaded_original,b,c->node_tuple_original))return deny(c,b);
 if(!slcli_build_windows_command_birth_original(&c->command_original,&c->command_source_original,
 c->command_mutable_original,c->attributes_original)||
 c->command_original.issuing_process.before.tick<c->node_loaded_original->finish.tick||
 !slcli_build_child_principal_windows_capture_original(&c->child_original,&c->command_original)||
 !slcli_build_loaded_file_windows_capture_original(&c->image_original,&c->child_original,
 c->image_reads_original,c->image_clocks_original,c->image_body_original)||
 !slcli_build_remote_image_windows_observe_original(c->mapping_original,&c->image_original)||
 !slcli_build_remote_image_windows_initial_bytes_original(c->mapping_original)||
 !slcli_build_streams_windows_begin_original(c->streams_original,&c->command_original,&c->mapping_original->bytes_after)||
 !slcli_build_streams_windows_poll_original(c->streams_original))return deny(c,b);
 b->prior_tick=c->streams_original->prior_tick;
 c->stopped_prefix_captured=1;c->failed=0;return 1;
}
#endif
