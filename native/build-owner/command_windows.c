#include "command_windows.h"
#if defined(_WIN32)
static int apart(const void *a,size_t n,const void *b,size_t m) {
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return a&&b&&n&&m&&x<=UINTPTR_MAX-n&&y<=UINTPTR_MAX-m&&(x+n<=y||y+m<=x);
}
static int zero(const void *p,size_t n){const unsigned char *s=p;size_t i;for(i=0;i<n;i++)if(s[i])return 0;return 1;}
static int span(const WCHAR *s,size_t n,int environment) {
 size_t i;
 if(!s||n<(environment?2:1)||n>SIZE_MAX/sizeof(WCHAR)||
 (uintptr_t)s>UINTPTR_MAX-n*sizeof(WCHAR)||s[n-1]||(environment&&s[n-2]))return 0;
 if(!environment)for(i=0;i+1<n;i++)if(!s[i])return 0;
 if(environment)for(i=0;i+2<n;i++)if(!s[i]&&!s[i+1])return 0;
 return 1;
}
static int away(const struct slcli_build_windows_command_source *s,const void *p,size_t n) {
 return slcli_build_held_file_windows_output_away_original(s->executable_original,p,n)&&
 slcli_build_principal_windows_output_away_original(s->process_primary_original,p,n)&&
 apart(p,n,s,sizeof(*s))&&apart(p,n,s->command_original,s->command_units*sizeof(WCHAR))&&
 apart(p,n,s->environment_original,s->environment_units*sizeof(WCHAR))&&
 apart(p,n,s->cwd_original,s->cwd_units*sizeof(WCHAR));
}
static int shape(const struct slcli_build_windows_command *c,
 const struct slcli_build_windows_command_source *s,WCHAR *command,void *attributes) {
 const void *outputs[3];size_t sizes[3],i,j;
 if(!c||!s||!s->executable_original||!s->process_primary_original||
 !s->process_primary_original->process_primary_only||
 s->executable_original->principal_original!=s->process_primary_original->issuing_effective_original||
 !span(s->command_original,s->command_units,0)||s->command_units>32767||s->command_units<2||
 !span(s->environment_original,s->environment_units,1)||!span(s->cwd_original,s->cwd_units,0)||
 s->cwd_units<4||!((s->cwd_original[0]>='A'&&s->cwd_original[0]<='Z')||
 (s->cwd_original[0]>='a'&&s->cwd_original[0]<='z'))||s->cwd_original[1]!=':'||
 s->cwd_original[2]!='\\'||!s->attribute_capacity||!s->pipe_capacity||
 (uintptr_t)attributes%sizeof(void *)||(uintptr_t)command%sizeof(WCHAR))return 0;
 outputs[0]=c;sizes[0]=sizeof(*c);outputs[1]=command;sizes[1]=s->command_units*sizeof(WCHAR);
 outputs[2]=attributes;sizes[2]=s->attribute_capacity;
 for(i=0;i<3;i++) {
 if(!away(s,outputs[i],sizes[i]))return 0;
 for(j=0;j<i;j++)if(!apart(outputs[i],sizes[i],outputs[j],sizes[j]))return 0;
 }
 return s->process_primary_original->expires==s->executable_original->expires;
}
static int clocks(const struct slcli_build_windows_call *r,uint64_t *prior,uint64_t expires) {
 if(r->called!=1||r->returned!=1||!slcli_native_clock_windows_matches_original(&r->before)||
 !slcli_native_clock_windows_matches_original(&r->after)||r->before.tick<*prior||
 r->before.tick>=expires||r->after.tick<r->before.tick||r->after.tick>=expires)return 0;
 *prior=r->after.tick;return 1;
}
static int begin(struct slcli_build_windows_call *r,uint64_t prior,uint64_t expires) {
 return !r->called&&!r->returned&&slcli_native_clock_read(&r->before)&&
 slcli_native_clock_windows_matches_original(&r->before)&&r->before.tick>=prior&&r->before.tick<expires;
}
static int end(struct slcli_build_windows_call *r,uint64_t *prior,uint64_t expires) {
 return slcli_native_clock_read(&r->after)&&clocks(r,prior,expires);
}
static int row(const struct slcli_build_windows_call *r,const uintptr_t *a,size_t n,
 uint64_t *prior,uint64_t expires) {
 size_t i;
 if(n>12||r->error_observed||r->error||!r->raw||!clocks(r,prior,expires))return 0;
 for(i=0;i<12;i++)if(r->args[i]!=(i<n?a[i]:0))return 0;return 1;
}
static int pipe_matches(const struct slcli_build_windows_pipe *p,int private_reader,
 DWORD capacity,uint64_t *prior,uint64_t expires) {
 uintptr_t a[4];
 if(p->security_original.nLength!=sizeof(p->security_original)||p->security_original.lpSecurityDescriptor||
 p->security_original.bInheritHandle!=TRUE||!p->read_original||!p->write_original||
 p->read_original==INVALID_HANDLE_VALUE||p->write_original==INVALID_HANDLE_VALUE||p->read_original==p->write_original)return 0;
 a[0]=(uintptr_t)&p->read_original;a[1]=(uintptr_t)&p->write_original;
 a[2]=(uintptr_t)&p->security_original;a[3]=capacity;
 if(!row(&p->create,a,4,prior,expires))return 0;
 if(private_reader) {
 a[0]=(uintptr_t)p->read_original;a[1]=HANDLE_FLAG_INHERIT;a[2]=0;
 if(!row(&p->reader_private,a,3,prior,expires))return 0;
 }else if(!zero(&p->reader_private,sizeof(p->reader_private)))return 0;
 a[0]=(uintptr_t)p->read_original;a[1]=(uintptr_t)&p->read_flags_original;
 if(!row(&p->read_flags,a,2,prior,expires)||
 !!(p->read_flags_original&HANDLE_FLAG_INHERIT)!=!private_reader)return 0;
 a[0]=(uintptr_t)p->write_original;a[1]=(uintptr_t)&p->write_flags_original;
 return row(&p->write_flags,a,2,prior,expires)&&(p->write_flags_original&HANDLE_FLAG_INHERIT);
}
#define CALL(r,expression) do { (r)->called=1;(r)->raw=(uintptr_t)(expression);(r)->returned=1; \
 if(!(r)->raw){(r)->error=GetLastError();(r)->error_observed=1;} } while(0)
static int pipe_capture(struct slcli_build_windows_pipe *p,int private_reader,DWORD capacity,
 uint64_t *prior,uint64_t expires) {
 struct slcli_build_windows_call *r=&p->create;uint64_t initial=*prior,replay;
 p->security_original.nLength=sizeof(p->security_original);p->security_original.bInheritHandle=TRUE;
 r->args[0]=(uintptr_t)&p->read_original;r->args[1]=(uintptr_t)&p->write_original;
 r->args[2]=(uintptr_t)&p->security_original;r->args[3]=capacity;
 if(!begin(r,*prior,expires))return 0;
 CALL(r,CreatePipe(&p->read_original,&p->write_original,&p->security_original,capacity));
 if(!end(r,prior,expires)||!r->raw)return 0;
 if(private_reader) {
 r=&p->reader_private;r->args[0]=(uintptr_t)p->read_original;r->args[1]=HANDLE_FLAG_INHERIT;
 if(!begin(r,*prior,expires))return 0;CALL(r,SetHandleInformation(p->read_original,HANDLE_FLAG_INHERIT,0));
 if(!end(r,prior,expires)||!r->raw)return 0;
 }
 r=&p->read_flags;r->args[0]=(uintptr_t)p->read_original;r->args[1]=(uintptr_t)&p->read_flags_original;
 if(!begin(r,*prior,expires))return 0;CALL(r,GetHandleInformation(p->read_original,&p->read_flags_original));
 if(!end(r,prior,expires)||!r->raw)return 0;
 r=&p->write_flags;r->args[0]=(uintptr_t)p->write_original;r->args[1]=(uintptr_t)&p->write_flags_original;
 if(!begin(r,*prior,expires))return 0;CALL(r,GetHandleInformation(p->write_original,&p->write_flags_original));
 if(!end(r,prior,expires)||!r->raw)return 0;
 replay=initial;return pipe_matches(p,private_reader,capacity,&replay,expires)&&replay==*prior;
}
static int contents(const struct slcli_build_windows_command *c,uint64_t *last) {
 const struct slcli_build_windows_command_source *s;
 const struct slcli_build_windows_call *r;const STARTUPINFOW *startup;
 uint64_t prior;uintptr_t a[10];size_t i;
 if(!c||!shape(c,c->source_original,c->command_mutable_original,c->attribute_original)||
 c->started!=1||c->failed||c->completed!=1)return 0;s=c->source_original;
 prior=s->executable_original->finish.tick;
 if(prior<s->process_primary_original->finish.tick)prior=s->process_primary_original->finish.tick;
 if(c->prior_tick!=prior||c->expires!=s->process_primary_original->expires)return 0;
 if(!row(&c->issuing_process,NULL,0,&prior,c->expires)||
 c->issuing_process.raw!=c->issuing_process_original||
 c->issuing_process_original!=s->process_primary_original->process_original||
 !row(&c->issuing_thread,NULL,0,&prior,c->expires)||c->issuing_thread.raw!=c->issuing_thread_original||
 c->issuing_thread_original!=s->process_primary_original->thread_original)return 0;
 if(!pipe_matches(&c->stdin_original,0,s->pipe_capacity,&prior,c->expires)||
 !pipe_matches(&c->stdout_original,1,s->pipe_capacity,&prior,c->expires)||
 !pipe_matches(&c->stderr_original,1,s->pipe_capacity,&prior,c->expires))return 0;
 a[0]=(uintptr_t)c->stdin_original.write_original;
 if(!row(&c->stdin_writer_close,a,1,&prior,c->expires))return 0;
 r=&c->attributes_size;
 if(!clocks(r,&prior,c->expires)||r->raw||r->error_observed!=1||r->error!=ERROR_INSUFFICIENT_BUFFER||
 r->args[0]||r->args[1]!=1||r->args[2]||r->args[3]!=(uintptr_t)&c->attribute_bytes_original||
 !c->attribute_bytes_original||c->attribute_bytes_original>s->attribute_capacity||
 c->attribute_bytes_before!=c->attribute_bytes_original)return 0;
 for(i=4;i<12;i++)if(r->args[i])return 0;
 a[0]=(uintptr_t)c->attribute_original;a[1]=1;a[2]=0;a[3]=(uintptr_t)&c->attribute_bytes_original;
 if(!row(&c->attributes_init,a,4,&prior,c->expires))return 0;
 if(c->child_handles_original[0]!=c->stdin_original.read_original||
 c->child_handles_original[1]!=c->stdout_original.write_original||
 c->child_handles_original[2]!=c->stderr_original.write_original)return 0;
 a[0]=(uintptr_t)c->attribute_original;a[1]=0;a[2]=PROC_THREAD_ATTRIBUTE_HANDLE_LIST;
 a[3]=(uintptr_t)c->child_handles_original;a[4]=sizeof(c->child_handles_original);a[5]=0;a[6]=0;
 if(!row(&c->attributes_handles,a,7,&prior,c->expires))return 0;
 startup=&c->startup_original.StartupInfo;
 if(startup->cb!=sizeof(c->startup_original)||startup->dwFlags!=(STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW)||
 startup->wShowWindow!=SW_HIDE||startup->hStdInput!=c->stdin_original.read_original||
 startup->hStdOutput!=c->stdout_original.write_original||startup->hStdError!=c->stderr_original.write_original||
 c->startup_original.lpAttributeList!=c->attribute_original||startup->lpReserved||startup->lpDesktop||
 startup->lpTitle||startup->dwX||startup->dwY||startup->dwXSize||startup->dwYSize||
 startup->dwXCountChars||startup->dwYCountChars||startup->dwFillAttribute||startup->cbReserved2||startup->lpReserved2)return 0;
 a[0]=(uintptr_t)s->executable_original->member_original->leaf.original;
 a[1]=(uintptr_t)c->command_mutable_original;a[2]=0;a[3]=0;a[4]=TRUE;
 a[5]=DEBUG_PROCESS|CREATE_NO_WINDOW|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT;
 a[6]=(uintptr_t)s->environment_original;a[7]=(uintptr_t)s->cwd_original;
 a[8]=(uintptr_t)&c->startup_original.StartupInfo;a[9]=(uintptr_t)&c->process_original;
 if(!row(&c->birth,a,10,&prior,c->expires)||!c->process_original.hProcess||!c->process_original.hThread||
 c->process_original.hProcess==INVALID_HANDLE_VALUE||c->process_original.hThread==INVALID_HANDLE_VALUE||
 !c->process_original.dwProcessId||!c->process_original.dwThreadId)return 0;
 a[0]=(uintptr_t)&c->first_event_original;a[1]=c->first_wait_milliseconds;
 if(c->first_event.before.tick>=c->expires||
 c->first_wait_milliseconds!=(c->expires-c->first_event.before.tick)/1000000+
 ((c->expires-c->first_event.before.tick)%1000000!=0)||
 !c->first_wait_milliseconds||c->first_wait_milliseconds==INFINITE||
 !row(&c->first_event,a,2,&prior,c->expires)||
 c->first_event_original.dwDebugEventCode!=CREATE_PROCESS_DEBUG_EVENT||
 c->first_event_original.dwProcessId!=c->process_original.dwProcessId||
 c->first_event_original.dwThreadId!=c->process_original.dwThreadId||
 !c->first_event_original.u.CreateProcessInfo.hFile||
 !c->first_event_original.u.CreateProcessInfo.hProcess||!c->first_event_original.u.CreateProcessInfo.hThread||
 c->first_event_original.u.CreateProcessInfo.hFile==INVALID_HANDLE_VALUE||
 c->first_event_original.u.CreateProcessInfo.hProcess==INVALID_HANDLE_VALUE||
 c->first_event_original.u.CreateProcessInfo.hThread==INVALID_HANDLE_VALUE)return 0;
 *last=prior;return 1;
}
int slcli_build_windows_command_birth_matches_original(const struct slcli_build_windows_command *c) {
 uint64_t prior;return contents(c,&prior)&&slcli_native_clock_windows_matches_original(&c->finish)&&
 c->finish.tick>=prior&&c->finish.tick<c->expires;
}
int slcli_build_windows_command_output_away_original(
 const struct slcli_build_windows_command *c,const void *out,size_t bytes) {
 return slcli_build_windows_command_birth_matches_original(c)&&apart(out,bytes,c,sizeof(*c))&&
 away(c->source_original,out,bytes)&&
 apart(out,bytes,c->command_mutable_original,c->source_original->command_units*sizeof(WCHAR))&&
 apart(out,bytes,c->attribute_original,c->source_original->attribute_capacity);
}
int slcli_build_windows_command_birth_original(struct slcli_build_windows_command *c,
 const struct slcli_build_windows_command_source *s,WCHAR *command,void *attributes) {
 struct slcli_build_windows_call *r;uint64_t prior,remaining,ms;size_t i;
 if(!shape(c,s,command,attributes)||!zero(c,sizeof(*c))||
 !zero(command,s->command_units*sizeof(WCHAR))||!zero(attributes,s->attribute_capacity))return 0;
 c->source_original=s;c->command_mutable_original=command;c->attribute_original=attributes;
 c->expires=s->process_primary_original->expires;prior=s->executable_original->finish.tick;
 if(prior<s->process_primary_original->finish.tick)prior=s->process_primary_original->finish.tick;
 c->prior_tick=prior;c->started=1;c->failed=1;
 for(i=0;i<s->command_units;i++)command[i]=s->command_original[i];
 r=&c->issuing_process;if(!begin(r,prior,c->expires))return 0;
 r->called=1;c->issuing_process_original=GetCurrentProcessId();r->raw=c->issuing_process_original;r->returned=1;
 if(!end(r,&prior,c->expires)||c->issuing_process_original!=s->process_primary_original->process_original)return 0;
 r=&c->issuing_thread;if(!begin(r,prior,c->expires))return 0;
 r->called=1;c->issuing_thread_original=GetCurrentThreadId();r->raw=c->issuing_thread_original;r->returned=1;
 if(!end(r,&prior,c->expires)||c->issuing_thread_original!=s->process_primary_original->thread_original)return 0;
 if(!pipe_capture(&c->stdin_original,0,s->pipe_capacity,&prior,c->expires)||
 !pipe_capture(&c->stdout_original,1,s->pipe_capacity,&prior,c->expires)||
 !pipe_capture(&c->stderr_original,1,s->pipe_capacity,&prior,c->expires))return 0;
 r=&c->stdin_writer_close;r->args[0]=(uintptr_t)c->stdin_original.write_original;
 if(!begin(r,prior,c->expires))return 0;CALL(r,CloseHandle(c->stdin_original.write_original));
 if(!end(r,&prior,c->expires)||!r->raw)return 0;
 r=&c->attributes_size;r->args[1]=1;r->args[3]=(uintptr_t)&c->attribute_bytes_original;
 if(!begin(r,prior,c->expires))return 0;
 CALL(r,InitializeProcThreadAttributeList(NULL,1,0,&c->attribute_bytes_original));
 if(!end(r,&prior,c->expires)||r->raw||r->error!=ERROR_INSUFFICIENT_BUFFER||
 !c->attribute_bytes_original||c->attribute_bytes_original>s->attribute_capacity)return 0;
 c->attribute_bytes_before=c->attribute_bytes_original;
 r=&c->attributes_init;r->args[0]=(uintptr_t)attributes;r->args[1]=1;r->args[3]=(uintptr_t)&c->attribute_bytes_original;
 if(!begin(r,prior,c->expires))return 0;
 CALL(r,InitializeProcThreadAttributeList((LPPROC_THREAD_ATTRIBUTE_LIST)attributes,1,0,&c->attribute_bytes_original));
 if(!end(r,&prior,c->expires)||!r->raw)return 0;
 c->child_handles_original[0]=c->stdin_original.read_original;
 c->child_handles_original[1]=c->stdout_original.write_original;c->child_handles_original[2]=c->stderr_original.write_original;
 r=&c->attributes_handles;r->args[0]=(uintptr_t)attributes;r->args[2]=PROC_THREAD_ATTRIBUTE_HANDLE_LIST;
 r->args[3]=(uintptr_t)c->child_handles_original;r->args[4]=sizeof(c->child_handles_original);
 if(!begin(r,prior,c->expires))return 0;
 CALL(r,UpdateProcThreadAttribute((LPPROC_THREAD_ATTRIBUTE_LIST)attributes,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
 c->child_handles_original,sizeof(c->child_handles_original),NULL,NULL));
 if(!end(r,&prior,c->expires)||!r->raw)return 0;
 c->startup_original.StartupInfo.cb=sizeof(c->startup_original);
 c->startup_original.StartupInfo.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
 c->startup_original.StartupInfo.wShowWindow=SW_HIDE;
 c->startup_original.StartupInfo.hStdInput=c->child_handles_original[0];
 c->startup_original.StartupInfo.hStdOutput=c->child_handles_original[1];
 c->startup_original.StartupInfo.hStdError=c->child_handles_original[2];
 c->startup_original.lpAttributeList=(LPPROC_THREAD_ATTRIBUTE_LIST)attributes;
 r=&c->birth;r->args[0]=(uintptr_t)s->executable_original->member_original->leaf.original;
 r->args[1]=(uintptr_t)command;r->args[4]=TRUE;
 r->args[5]=DEBUG_PROCESS|CREATE_NO_WINDOW|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT;
 r->args[6]=(uintptr_t)s->environment_original;r->args[7]=(uintptr_t)s->cwd_original;
 r->args[8]=(uintptr_t)&c->startup_original.StartupInfo;r->args[9]=(uintptr_t)&c->process_original;
 if(!begin(r,prior,c->expires))return 0;
 CALL(r,CreateProcessW(s->executable_original->member_original->leaf.original,command,NULL,NULL,TRUE,
 DEBUG_PROCESS|CREATE_NO_WINDOW|CREATE_UNICODE_ENVIRONMENT|EXTENDED_STARTUPINFO_PRESENT,
 (void *)s->environment_original,s->cwd_original,&c->startup_original.StartupInfo,&c->process_original));
 if(!end(r,&prior,c->expires)||!r->raw)return 0;
 r=&c->first_event;if(!begin(r,prior,c->expires))return 0;
 remaining=c->expires-r->before.tick;ms=remaining/1000000+(remaining%1000000!=0);
 if(!ms||ms>=INFINITE)return 0;c->first_wait_milliseconds=(DWORD)ms;
 r->args[0]=(uintptr_t)&c->first_event_original;r->args[1]=c->first_wait_milliseconds;
 CALL(r,WaitForDebugEvent(&c->first_event_original,c->first_wait_milliseconds));
 if(!end(r,&prior,c->expires)||!r->raw)return 0;
 c->completed=1;c->failed=0;if(!contents(c,&prior)){c->failed=1;return 0;}c->failed=1;
 if(!slcli_native_clock_read(&c->finish)||!slcli_native_clock_windows_matches_original(&c->finish)||
 c->finish.tick<prior||c->finish.tick>=c->expires)return 0;c->failed=0;return 1;
}
#undef CALL
#endif
