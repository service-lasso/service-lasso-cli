#include "remote_image_windows.h"
#if defined(_WIN32)
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static uint32_t le16(const unsigned char *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8);}
static uint32_t le32(const unsigned char *p){return le16(p)|(le16(p+2)<<16);}
int slcli_build_pe64_geometry_windows_original(struct slcli_build_pe64_geometry_windows *c,
 const unsigned char *p,size_t n,size_t page) {
 size_t offset,optional,sections,i,j,section_align,file_align,directories;uintptr_t out=(uintptr_t)c,in=(uintptr_t)p;
 if(!c||!p||!zero(c,sizeof(*c))||out>UINTPTR_MAX-sizeof(*c)||in>UINTPTR_MAX-n||
 (out<in+n&&in<out+sizeof(*c))||n<64||p[0]!='M'||p[1]!='Z'||!page||(page&(page-1)))return 0;
 offset=le32(p+60);if(offset<64||offset>n||n-offset<24||le32(p+offset)!=0x00004550||le16(p+offset+4)!=0x8664)return 0;
 sections=le16(p+offset+6);optional=le16(p+offset+20);
 if(!sections||sections>96||optional<112||optional>n-offset-24||le16(p+offset+24)!=0x20b)return 0;
 c->section_table_offset=offset+24+optional;c->section_count=sections;
 if(sections>(n-c->section_table_offset)/40)return 0;
 c->image_bytes=le32(p+offset+24+56);c->headers_bytes=le32(p+offset+24+60);section_align=le32(p+offset+24+32);
 file_align=le32(p+offset+24+36);directories=le32(p+offset+24+108);
 if(!c->image_bytes||c->image_bytes%page||!section_align||(section_align&(section_align-1))||section_align%page||
 c->image_bytes%section_align||file_align<512||file_align>65536||(file_align&(file_align-1))||file_align>section_align||
 directories>16||optional<112+directories*8||c->headers_bytes%file_align||
 !c->headers_bytes||c->headers_bytes>n||c->headers_bytes>c->image_bytes||
 c->section_table_offset+sections*40>c->headers_bytes)return 0;
 for(i=0;i<sections;i++) {
 const unsigned char *s=p+c->section_table_offset+i*40;
 uint64_t va=le32(s+12),raw=le32(s+16),file=le32(s+20),virtual=le32(s+8),span=raw>virtual?raw:virtual;
 if(va<section_align||va%section_align||va<c->headers_bytes||va>c->image_bytes||span>c->image_bytes-va||
 file>n||raw>n-file||(raw&&(file<c->headers_bytes||file%file_align||raw%file_align)))return 0;
 for(j=0;j<i;j++) {
 const unsigned char *old=p+c->section_table_offset+j*40;
 uint64_t ov=le32(old+12),oraw=le32(old+16),ofile=le32(old+20),ovirtual=le32(old+8),ospan=oraw>ovirtual?oraw:ovirtual;
 if(span&&ospan&&va<ov+ospan&&ov<va+span)return 0;
 if(raw&&oraw&&file<ofile+oraw&&ofile<file+raw)return 0;
 }
 }
 c->page_bytes=page;c->capacity=c->image_bytes/page;
 return c->capacity&&c->capacity<=SIZE_MAX/sizeof(struct slcli_build_remote_image_windows_row);
}
int slcli_build_remote_image_windows_prepare_original(struct slcli_build_remote_image_windows *c,
 struct slcli_arena *a,const struct slcli_build_held_file_windows *f,size_t page) {
 struct slcli_build_pe64_geometry_windows geometry={0};
 if(!c||!a||!f||!slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c))||
 !slcli_build_held_file_windows_output_away_original(f,c,sizeof(*c)))return 0;
 c->file_original=f;c->failed=1;
 if(!slcli_build_pe64_geometry_windows_original(&geometry,f->body_original,(size_t)f->member_original->bytes,page))return 0;
 c->image_bytes=geometry.image_bytes;c->headers_bytes=geometry.headers_bytes;
 c->section_table_offset=geometry.section_table_offset;c->section_count=geometry.section_count;
 c->page_bytes=geometry.page_bytes;c->capacity=geometry.capacity;
 c->rows_original=slcli_arena_calloc(a,c->capacity,sizeof(*c->rows_original));
 c->body_original=slcli_arena_calloc(a,c->image_bytes,1);
 if(!c->rows_original||!c->body_original)return 0;
 c->prepared=1;return 1;
}
static int begin(struct slcli_build_remote_image_windows *c,struct slcli_build_windows_call *r) {
 return slcli_native_clock_read(&r->before)&&r->before.tick>=c->prior_tick&&r->before.tick<c->expires;
}
static int end(struct slcli_build_remote_image_windows *c,struct slcli_build_windows_call *r) {
 if(!slcli_native_clock_read(&r->after)||r->after.tick<r->before.tick||r->after.tick>=c->expires)return 0;
 c->prior_tick=r->after.tick;return 1;
}
int slcli_build_remote_image_windows_observe_original(struct slcli_build_remote_image_windows *c,
 const struct slcli_build_loaded_file_windows *image) {
 const struct slcli_build_windows_command *birth;HANDLE process;uintptr_t limit;int complete=1;size_t i,covered=0;
 if(!c||!c->prepared||c->started||!image||!slcli_build_loaded_file_windows_matches_original(image)||
 image->child_original->birth_original->source_original->executable_original!=c->file_original)return 0;
 birth=image->child_original->birth_original;process=birth->process_original.hProcess;
 c->event_file_original=image;c->base_original=(uintptr_t)birth->first_event_original.u.CreateProcessInfo.lpBaseOfImage;
 c->started=1;c->prior_tick=image->finish.tick;c->expires=birth->expires;
 if(!c->base_original||c->base_original%c->page_bytes||c->image_bytes>UINTPTR_MAX-c->base_original)return 0;
 limit=c->base_original+c->image_bytes;c->cursor_original=c->base_original;
 if(!begin(c,&c->issuing_thread))return 0;c->issuing_thread.called=1;
 c->issuing_thread.raw=GetCurrentThreadId();c->issuing_thread.returned=1;
 if(!end(c,&c->issuing_thread)||c->issuing_thread.raw!=birth->issuing_thread_original)return 0;
 while(c->cursor_original<limit) {
 struct slcli_build_remote_image_windows_row *r;MEMORY_BASIC_INFORMATION *g;DWORD protection;size_t offset;
 if(c->used>=c->capacity)return 0;r=&c->rows_original[c->used++];g=&r->geometry_original;
 r->address_original=c->cursor_original;
 if(!begin(c,&r->query))return 0;
 r->query.args[0]=(uintptr_t)process;r->query.args[1]=r->address_original;
 r->query.args[2]=(uintptr_t)g;r->query.args[3]=sizeof(*g);r->query.called=1;
 r->query.raw=VirtualQueryEx(process,(const void *)r->address_original,g,sizeof(*g));r->query.returned=1;
 if(!r->query.raw){r->query.error=GetLastError();r->query.error_observed=1;}
 if(!end(c,&r->query)||r->query.raw!=sizeof(*g)||g->BaseAddress!=(void *)r->address_original||
 g->AllocationBase!=(void *)c->base_original||!g->RegionSize||g->RegionSize>limit-r->address_original||
 g->RegionSize%c->page_bytes||g->Type!=MEM_IMAGE||(g->State!=MEM_COMMIT&&g->State!=MEM_RESERVE))return 0;
 r->bytes_original=g->RegionSize;protection=g->Protect&0xff;
 if(g->State==MEM_COMMIT&&!(g->Protect&PAGE_GUARD)&&
 (protection==PAGE_READONLY||protection==PAGE_READWRITE||protection==PAGE_WRITECOPY||
 protection==PAGE_EXECUTE_READ||protection==PAGE_EXECUTE_READWRITE||protection==PAGE_EXECUTE_WRITECOPY)) {
 offset=(size_t)(r->address_original-c->base_original);
 if(!begin(c,&r->read))return 0;
 r->read.args[0]=(uintptr_t)process;r->read.args[1]=r->address_original;
 r->read.args[2]=(uintptr_t)(c->body_original+offset);r->read.args[3]=r->bytes_original;
 r->read.args[4]=(uintptr_t)&r->received_original;r->read.called=1;
 r->read.raw=ReadProcessMemory(process,(const void *)r->address_original,c->body_original+offset,r->bytes_original,&r->received_original);
 r->read.returned=1;if(!r->read.raw){r->read.error=GetLastError();r->read.error_observed=1;}
 if(!end(c,&r->read)||!r->read.raw||r->received_original!=r->bytes_original)return 0;r->read_observed=1;
 }else complete=0;
 c->cursor_original+=r->bytes_original;
 }
 for(i=0;i<c->used&&covered<c->headers_bytes;i++) {
 const struct slcli_build_remote_image_windows_row *r=&c->rows_original[i];
 if(!r->read_observed||r->address_original-c->base_original!=covered||r->bytes_original>SIZE_MAX-covered)return 0;
 covered+=r->bytes_original;
 }
 if(covered<c->headers_bytes)return 0;
 for(i=0;i<c->headers_bytes;i++)if(c->body_original[i]!=c->file_original->body_original[i])return 0;
 c->headers_correspond=1;
 if(!slcli_native_clock_read(&c->finish)||c->finish.tick<c->prior_tick||c->finish.tick>=c->expires)return 0;
 c->geometry_complete=1;c->body_complete=complete?1:0;c->failed=0;return 1;
}
#endif
