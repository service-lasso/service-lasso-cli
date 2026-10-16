#ifndef SLCLI_BUILD_LOADED_FILE_WINDOWS_H
#define SLCLI_BUILD_LOADED_FILE_WINDOWS_H
#include "child_principal_windows.h"
#if defined(_WIN32)
struct slcli_build_loaded_file_windows {
 const struct slcli_build_child_principal_windows *child_original;
 HANDLE event_file_original;
 struct slcli_build_windows_call issuing_thread;
 struct slcli_native_clock before_observe[2];
 struct slcli_native_clock *before_reads_original;
 struct slcli_windows_file_capture before_original,after_original;
 struct slcli_windows_file_capture *reads_original;
 unsigned char *body_original;
 struct slcli_sha256 hash_original;
 unsigned char digest_original[32],eof_original;
 struct slcli_native_clock finish;
 uint64_t bytes_original,used_bytes,prior_tick,expires;
 size_t reads_used;
 uint32_t started,failed,completed;
};
int slcli_build_loaded_file_windows_capture_original(struct slcli_build_loaded_file_windows *,
 const struct slcli_build_child_principal_windows *,struct slcli_windows_file_capture *,
 struct slcli_native_clock *,unsigned char *);
int slcli_build_loaded_file_windows_matches_original(const struct slcli_build_loaded_file_windows *);
#endif
#endif
