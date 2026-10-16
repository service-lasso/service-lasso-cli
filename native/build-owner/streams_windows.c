#include "streams_windows.h"
#if defined(_WIN32)
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int begin(struct slcli_build_streams_windows *c,struct slcli_build_windows_call *r) {
 return slcli_native_clock_read(&r->before)&&r->before.tick>=c->prior_tick&&r->before.tick<c->expires;
}
static int end(struct slcli_build_streams_windows *c,struct slcli_build_windows_call *r) {
 if(!slcli_native_clock_read(&r->after)||r->after.tick<r->before.tick||r->after.tick>=c->expires)return 0;
 c->prior_tick=r->after.tick;return 1;
}
static int deny(struct slcli_build_streams_windows *c){c->failed=1;return 0;}
int slcli_build_stream_windows_read_extent_original(size_t used,size_t capacity,DWORD available,size_t *out) {
 size_t n=available;if(!out||!available||used>capacity)return 0;
 if(n>16384)n=16384;if(n>capacity-used)return 0;*out=n;return 1;
}
int slcli_build_streams_windows_prepare_original(struct slcli_build_streams_windows *c,
 struct slcli_arena *a,size_t polls,size_t bytes) {
 size_t i;
 if(!c||!a||!polls||!bytes||polls>SIZE_MAX/sizeof(struct slcli_build_stream_windows_row)||
 polls>SIZE_MAX/sizeof(*c->poll_threads_original)||polls>SIZE_MAX/sizeof(*c->poll_finishes_original)||
 !slcli_arena_live_block(a,c,sizeof(*c))||!zero(c,sizeof(*c)))return 0;
 c->failed=1;c->poll_capacity=polls;
 c->poll_threads_original=slcli_arena_calloc(a,polls,sizeof(*c->poll_threads_original));
 c->poll_finishes_original=slcli_arena_calloc(a,polls,sizeof(*c->poll_finishes_original));
 if(!c->poll_threads_original||!c->poll_finishes_original)return 0;
 for(i=0;i<2;i++) {
 struct slcli_build_stream_windows *s=&c->streams_original[i];
 s->row_capacity=polls;s->byte_capacity=bytes;
 s->rows_original=slcli_arena_calloc(a,polls,sizeof(*s->rows_original));
 s->body_original=slcli_arena_calloc(a,bytes,1);
 if(!s->rows_original||!s->body_original)return 0;
 }
 c->prepared=1;c->failed=0;return 1;
}
int slcli_build_streams_windows_begin_original(struct slcli_build_streams_windows *c,
 const struct slcli_build_windows_command *b,const struct slcli_native_clock *prior) {
 struct slcli_build_windows_call *r;HANDLE refs[3];size_t i;
 if(!c||!c->prepared||c->started||c->failed||!b||!prior||
 !slcli_build_windows_command_birth_matches_original(b)||prior->tick<b->finish.tick||
 !slcli_build_windows_command_output_away_original(b,c,sizeof(*c)))return 0;
 c->birth_original=b;c->started=1;c->prior_tick=prior->tick;c->expires=b->expires;
 c->streams_original[0].reader_original=b->stdout_original.read_original;
 c->streams_original[1].reader_original=b->stderr_original.read_original;
 r=&c->issuing_thread;if(!begin(c,r))return deny(c);r->called=1;r->raw=GetCurrentThreadId();r->returned=1;
 if(!end(c,r)||r->raw!=b->issuing_thread_original)return deny(c);
 refs[0]=b->stdin_original.read_original;refs[1]=b->stdout_original.write_original;refs[2]=b->stderr_original.write_original;
 for(i=0;i<3;i++) {
 r=&c->parent_child_ends[i];if(!begin(c,r))return deny(c);
 r->args[0]=(uintptr_t)refs[i];r->called=1;r->raw=CloseHandle(refs[i]);r->returned=1;
 if(!r->raw){r->error=GetLastError();r->error_observed=1;}
 if(!end(c,r)||!r->raw)return deny(c);c->parent_ends_closed++;
 }
 if(!slcli_native_clock_read(&c->finish)||c->finish.tick<c->prior_tick||c->finish.tick>=c->expires)return deny(c);
 c->prior_tick=c->finish.tick;return 1;
}
int slcli_build_streams_windows_poll_original(struct slcli_build_streams_windows *c) {
 struct slcli_build_windows_call *call;struct slcli_native_clock *finish;size_t i;
 if(!c||!c->prepared||!c->started||c->failed||c->streams_complete||c->parent_ends_closed!=3)return 0;
 if(c->polls_used>=c->poll_capacity)return deny(c);
 call=&c->poll_threads_original[c->polls_used];finish=&c->poll_finishes_original[c->polls_used++];
 if(!begin(c,call))return deny(c);call->called=1;call->raw=GetCurrentThreadId();call->returned=1;
 if(!end(c,call)||call->raw!=c->birth_original->issuing_thread_original)return deny(c);
 for(i=0;i<2;i++) {
 struct slcli_build_stream_windows *s=&c->streams_original[i];struct slcli_build_stream_windows_row *r;size_t n;
 if(s->eof_observed)continue;
 if(s->rows_used>=s->row_capacity)return deny(c);r=&s->rows_original[s->rows_used++];r->offset_original=s->bytes_used;
 call=&r->peek;if(!begin(c,call))return deny(c);
 call->args[0]=(uintptr_t)s->reader_original;call->args[1]=0;call->args[2]=0;call->args[3]=0;
 call->args[4]=(uintptr_t)&r->available_original;call->args[5]=0;call->called=1;
 call->raw=PeekNamedPipe(s->reader_original,NULL,0,NULL,&r->available_original,NULL);call->returned=1;
 if(!call->raw){call->error=GetLastError();call->error_observed=1;}
 if(!end(c,call))return deny(c);
 if(!call->raw) {
 if(call->error!=ERROR_BROKEN_PIPE||r->available_original)return deny(c);
 r->eof_observed=1;s->eof_observed=1;continue;
 }
 if(!r->available_original){r->idle_observed=1;continue;}
 if(!slcli_build_stream_windows_read_extent_original(s->bytes_used,s->byte_capacity,r->available_original,&n))return deny(c);
 r->requested_original=n;call=&r->read;if(!begin(c,call))return deny(c);
 call->args[0]=(uintptr_t)s->reader_original;call->args[1]=(uintptr_t)(s->body_original+s->bytes_used);
 call->args[2]=n;call->args[3]=(uintptr_t)&r->received_original;call->args[4]=0;call->called=1;
 call->raw=ReadFile(s->reader_original,s->body_original+s->bytes_used,(DWORD)n,&r->received_original,NULL);call->returned=1;
 if(!call->raw){call->error=GetLastError();call->error_observed=1;}
 if(!end(c,call)||r->received_original>n)return deny(c);
 /* The sole reader cannot lose its observed available prefix. A broken read
  * after a positive peek is retained failure, never synthetic clean EOF. */
 if(!call->raw)return deny(c);
 if(!r->received_original)return deny(c);s->bytes_used+=r->received_original;r->data_observed=1;
 }
 if(!slcli_native_clock_read(finish)||finish->tick<c->prior_tick||finish->tick>=c->expires)return deny(c);
 c->prior_tick=finish->tick;
 if(c->streams_original[0].eof_observed&&c->streams_original[1].eof_observed)c->streams_complete=1;
 return 1;
}
#endif
