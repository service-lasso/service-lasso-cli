#include "node_loaded_windows.h"
#if defined(_WIN32)
#if defined(NAPI_VERSION) && NAPI_VERSION != 8
#error Selected seed uses the original ordinary Node-API8 header surface
#endif
#ifndef NAPI_VERSION
#define NAPI_VERSION 8
#endif
#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif
#include <node_api.h>
_Static_assert(sizeof(uintptr_t)==8,"Selected Windows Node seed is x64");
static int apart(const void *a,size_t n,const void *b,size_t m){uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
int slcli_build_node_windows_rva_correspond_original(uintptr_t base,uintptr_t actual,uint32_t rva,size_t bytes) {
 return base&&rva&&bytes&&base<=UINTPTR_MAX-bytes&&rva<bytes&&actual==base+rva;
}
static int begin(struct slcli_build_node_loaded_windows *c,struct slcli_build_windows_call *r) {
 return slcli_native_clock_read(&r->before)&&r->before.tick>=c->prior_tick&&r->before.tick<c->expires;
}
static int end(struct slcli_build_node_loaded_windows *c,struct slcli_build_windows_call *r) {
 if(!slcli_native_clock_read(&r->after)||r->after.tick<r->before.tick||r->after.tick>=c->expires)return 0;
 c->prior_tick=r->after.tick;return 1;
}
static int deny(struct slcli_build_node_loaded_windows *c,struct slcli_build_bootstrap_windows *b) {
 c->failed=1;b->failed=1;b->memory_original.failed=1;return 0;
}
int slcli_build_node_loaded_windows_capture_original(struct slcli_build_node_loaded_windows *c,
 struct slcli_build_bootstrap_windows *b,const struct slcli_build_node_abi_windows *t) {
 const struct slcli_build_windows_path *leaf;struct slcli_build_windows_call *r;size_t i;
 if(!c||!b||!t||!b->completed||b->failed||b->memory_original.failed||t->bootstrap_original!=b||
 !t->started||t->failed||!t->byte_tuple_corresponds||!t->image_geometry.headers_bytes||b->prior_tick<t->after_parse.tick||
 !slcli_arena_live_block(&b->arena_original,c,sizeof(*c))||!zero(c,sizeof(*c))||
 !apart(c,sizeof(*c),b,sizeof(*b))||!apart(c,sizeof(*c),t,sizeof(*t))||
 !slcli_build_node_abi_windows_source_output_away_original(t->source_original,c,sizeof(*c))||
 !slcli_build_principal_windows_output_away_original(b->primary_original,c,sizeof(*c))||
 !slcli_build_principal_windows_output_away_original(b->effective_original,c,sizeof(*c)))return 0;
 for(i=0;i<SLCLI_BUILD_NODE_INPUTS;i++)if(!slcli_build_held_file_windows_matches_original(&t->inputs[i].held_original)||
 !slcli_build_held_file_windows_output_away_original(&t->inputs[i].held_original,c,sizeof(*c)))return 0;
 c->tuple_original=t;c->started=1;c->failed=1;c->prior_tick=b->prior_tick;c->expires=b->source_original->expires;
 c->path_original=slcli_arena_calloc(&b->arena_original,32768,sizeof(WCHAR));
 c->headers_original=slcli_arena_calloc(&b->arena_original,t->image_geometry.headers_bytes,1);
 if(!c->path_original||!c->headers_original)return deny(c,b);
 /* Actual compiled imported function addresses, never source-supplied pointers.
  * The selected MSVC/x64 implementation defines these pointer conversions. */
 c->addresses_original[0]=(uintptr_t)&napi_get_cb_info;
 c->addresses_original[1]=(uintptr_t)&napi_is_buffer;
 c->addresses_original[2]=(uintptr_t)&napi_get_buffer_info;
 c->addresses_original[3]=(uintptr_t)&napi_create_buffer_copy;
 c->addresses_original[4]=(uintptr_t)&napi_create_object;
 c->addresses_original[5]=(uintptr_t)&napi_set_named_property;
 c->addresses_original[6]=(uintptr_t)&napi_create_string_utf8;
 c->addresses_original[7]=(uintptr_t)&napi_throw_error;
 c->addresses_original[8]=(uintptr_t)&napi_get_undefined;
 c->addresses_original[9]=(uintptr_t)&napi_define_properties;
 c->addresses_original[10]=(uintptr_t)&napi_get_version;
 r=&c->process;if(!begin(c,r))return deny(c,b);r->called=1;r->raw=GetCurrentProcessId();r->returned=1;
 if(!end(c,r)||r->raw!=b->primary_original->process_original)return deny(c,b);
 r=&c->thread;if(!begin(c,r))return deny(c,b);r->called=1;r->raw=GetCurrentThreadId();r->returned=1;
 if(!end(c,r)||r->raw!=b->primary_original->thread_original)return deny(c,b);
 r=&c->process_handle;if(!begin(c,r))return deny(c,b);r->called=1;
 r->raw=(uintptr_t)GetCurrentProcess();r->returned=1;if(!end(c,r)||!r->raw)return deny(c,b);
 r=&c->module;if(!begin(c,r))return deny(c,b);
 r->args[0]=0;r->args[1]=0;r->args[2]=(uintptr_t)&c->module_original;r->called=1;
 r->raw=GetModuleHandleExW(0,NULL,&c->module_original);r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!end(c,r)||!r->raw||!c->module_original)return deny(c,b);
 r=&c->path;if(!begin(c,r))return deny(c,b);
 r->args[0]=(uintptr_t)c->module_original;r->args[1]=(uintptr_t)c->path_original;r->args[2]=32768;r->called=1;
 r->raw=GetModuleFileNameW(c->module_original,c->path_original,32768);r->returned=1;
 if(!r->raw||r->raw==32768){r->error=GetLastError();r->error_observed=1;}
 if(!end(c,r)||!r->raw||r->raw>=32768||c->path_original[r->raw])return deny(c,b);
 leaf=&t->source_original->members[SLCLI_BUILD_NODE_IMAGE].leaf;
 if(r->raw+1!=leaf->units)return deny(c,b);
 for(i=0;i<leaf->units;i++)if(c->path_original[i]!=leaf->original[i])return deny(c,b);
 r=&c->headers;if(!begin(c,r))return deny(c,b);
 r->args[0]=c->process_handle.raw;r->args[1]=(uintptr_t)c->module_original;
 r->args[2]=(uintptr_t)c->headers_original;r->args[3]=t->image_geometry.headers_bytes;
 r->args[4]=(uintptr_t)&c->header_bytes_original;r->called=1;
 r->raw=ReadProcessMemory((HANDLE)r->args[0],c->module_original,c->headers_original,
 t->image_geometry.headers_bytes,&c->header_bytes_original);r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!end(c,r)||!r->raw||c->header_bytes_original!=t->image_geometry.headers_bytes)return deny(c,b);
 for(i=0;i<c->header_bytes_original;i++)if(c->headers_original[i]!=t->inputs[0].body_original[i])return deny(c,b);
 for(i=0;i<SLCLI_BUILD_NODE_API_IMPORTS;i++) {
 MEMORY_BASIC_INFORMATION *g=&c->geometry_original[i];DWORD protection;uintptr_t p=c->addresses_original[i];
 if(!slcli_build_node_windows_rva_correspond_original((uintptr_t)c->module_original,p,
 t->archive_original.symbols[i].export_rva,t->image_geometry.image_bytes))return deny(c,b);
 r=&c->queries[i];if(!begin(c,r))return deny(c,b);
 r->args[0]=p;r->args[1]=(uintptr_t)g;r->args[2]=sizeof(*g);r->called=1;
 r->raw=VirtualQuery((const void *)p,g,sizeof(*g));r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!end(c,r)||r->raw!=sizeof(*g))return deny(c,b);protection=g->Protect&0xff;
 if(g->Type!=MEM_IMAGE||g->State!=MEM_COMMIT||g->AllocationBase!=(void *)c->module_original||
 (g->Protect&PAGE_GUARD)||protection!=PAGE_EXECUTE_READ||!g->RegionSize||
 (uintptr_t)g->BaseAddress>UINTPTR_MAX-g->RegionSize||p<(uintptr_t)g->BaseAddress||
 p-(uintptr_t)g->BaseAddress>=g->RegionSize)return deny(c,b);
 c->symbols_observed++;
 }
 if(!slcli_native_clock_read(&c->finish)||c->finish.tick<c->prior_tick||c->finish.tick>=c->expires)return deny(c,b);
 c->addresses_correspond=1;c->failed=0;b->prior_tick=c->finish.tick;return 1;
}
#endif
