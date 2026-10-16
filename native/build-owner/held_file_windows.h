#ifndef SLCLI_BUILD_HELD_FILE_WINDOWS_H
#define SLCLI_BUILD_HELD_FILE_WINDOWS_H
#include "principal_windows.h"
#include "../host-service/file_windows.h"
#include "../host-service/crypto.h"
#if defined(_WIN32)
struct slcli_build_windows_path { const WCHAR *original;size_t units; };
struct slcli_build_windows_member {
 struct slcli_build_windows_path leaf;
 const struct slcli_build_windows_path *ancestors_original;
 size_t ancestor_count,read_capacity;
 uint64_t bytes;
 unsigned char digest[32];
};
struct slcli_build_windows_open {
 struct slcli_build_windows_call call;
 HANDLE handle_original;
 struct slcli_windows_file_capture observation;
};
struct slcli_build_held_file_windows {
 const struct slcli_build_principal_windows *principal_original;
 const struct slcli_build_windows_member *member_original;
 struct slcli_build_windows_open *ancestors_original;
 struct slcli_windows_file_capture *reads_original;
 unsigned char *body_original;
 struct slcli_build_windows_open leaf;
 struct slcli_windows_file_capture final_original;
 struct slcli_sha256 hash_original;
 unsigned char digest_original[32],eof_original;
 struct slcli_native_clock finish;
 uint64_t used_bytes,prior_tick,expires;
 size_t ancestors_used,reads_used;
 uint32_t started,failed,completed;
};
/* Actual held objects/readback correspondence under independently qualified
 * complete source/tool/namespace constraints. No source, signature, loaded
 * module or reserve authority is created by byte/digest/file-ID equality. */
int slcli_build_held_file_windows_capture_original(struct slcli_build_held_file_windows *,
 const struct slcli_build_principal_windows *,const struct slcli_build_windows_member *,
 struct slcli_build_windows_open *,struct slcli_windows_file_capture *,unsigned char *);
int slcli_build_held_file_windows_output_away_original(
 const struct slcli_build_held_file_windows *,const void *,size_t);
int slcli_build_held_file_windows_matches_original(const struct slcli_build_held_file_windows *);
#endif
#endif
