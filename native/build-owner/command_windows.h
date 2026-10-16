#ifndef SLCLI_BUILD_COMMAND_WINDOWS_H
#define SLCLI_BUILD_COMMAND_WINDOWS_H
#include "held_file_windows.h"
#if defined(_WIN32)
struct slcli_build_windows_command_source {
 const struct slcli_build_held_file_windows *executable_original;
 const struct slcli_build_principal_windows *process_primary_original;
 const WCHAR *command_original,*environment_original,*cwd_original;
 size_t command_units,environment_units,cwd_units,attribute_capacity;
 DWORD pipe_capacity;
};
struct slcli_build_windows_pipe {
 SECURITY_ATTRIBUTES security_original;
 HANDLE read_original,write_original;
 DWORD read_flags_original,write_flags_original;
 struct slcli_build_windows_call create,reader_private,read_flags,write_flags;
};
struct slcli_build_windows_command {
 const struct slcli_build_windows_command_source *source_original;
 WCHAR *command_mutable_original;
 void *attribute_original;
 SIZE_T attribute_bytes_original,attribute_bytes_before;
 struct slcli_build_windows_pipe stdin_original,stdout_original,stderr_original;
 HANDLE child_handles_original[3];
 STARTUPINFOEXW startup_original;
 PROCESS_INFORMATION process_original;
 DEBUG_EVENT first_event_original;
 DWORD issuing_process_original,issuing_thread_original;
 struct slcli_build_windows_call issuing_process,issuing_thread;
 struct slcli_build_windows_call attributes_size,attributes_init,attributes_handles;
 struct slcli_build_windows_call stdin_writer_close,birth,first_event;
 struct slcli_native_clock finish;
 uint64_t prior_tick,expires;
 DWORD first_wait_milliseconds;
 uint32_t started,failed,completed;
};
/* Fixed native root only. No continuation/retirement/source or reserve issuer.
 * All real handles and stopped debug-event backing survive failed/late results.
 * Whole owner and external exact admission precede invoking this private body. */
int slcli_build_windows_command_birth_original(struct slcli_build_windows_command *,
 const struct slcli_build_windows_command_source *,WCHAR *,void *);
int slcli_build_windows_command_output_away_original(
 const struct slcli_build_windows_command *,const void *,size_t);
int slcli_build_windows_command_birth_matches_original(const struct slcli_build_windows_command *);
#endif
#endif
