#ifndef SLCLI_SOURCE_STREAM_WINDOWS_H
#define SLCLI_SOURCE_STREAM_WINDOWS_H
#include "source_file_windows.h"
#include "crypto.h"
#if defined(_WIN32)
struct slcli_source_stream_geometry_windows {
 size_t reads,references,read_bytes,ordinal_bytes;
 size_t stream_charge,reads_charge,workspace_charge,ordinals_charge,total_control_charge;
 uint64_t native_calls,clock_calls,stream_row_comparisons,prior_claim_multiplier;
};
/* Prospective exact source storage/work subtotal, never a full fit result. */
int slcli_source_stream_geometry_windows_derive(uint64_t,size_t,
 struct slcli_source_stream_geometry_windows *);
struct slcli_source_stream_read_windows {
 uint64_t offset,version,args[5];DWORD requested,received,error;BOOL status;
 uint32_t ordinal,called,returned,hash_returned,consumed;int hash_status;
 struct slcli_native_clock before,after,consumed_after;
};
struct slcli_source_stream_windows {
 struct slcli_binding_storage_original *storage_original;
 const struct slcli_native_reference *plans_original;
 const uint32_t *ids_original,*ordinals_original;
 struct slcli_source_stream_geometry_windows geometry;
 const struct slcli_source_file_windows *source;
 struct slcli_source_stream_read_windows *reads;
 struct slcli_source_stream_read_windows eof;
 unsigned char *workspace;size_t workspace_bytes,count,used;
 uint32_t buffer_ordinal;uint64_t buffer_version,file_bytes,consumed;
 struct slcli_sha256 hash;unsigned char digest[32];
 LARGE_INTEGER zero,position,final_bytes;
 BY_HANDLE_FILE_INFORMATION final_information;
 FILE_ID_INFO identity[2];
 struct slcli_native_clock identity_before[2],identity_after[2];
 uint64_t identity_args[2][4];BOOL identity_status[2];DWORD identity_error[2];
 uint32_t identity_called[2],identity_returned[2];
 struct slcli_native_clock seek_before,seek_after,final_before,size_after,information_before,final_after;
 uint64_t seek_args[4],size_args[2],information_args[2];
 BOOL seek_status,size_status,information_status;
 DWORD seek_error,size_error,information_error;
 uint32_t started,buffer_attempted,buffer_returned,seek_called,seek_returned;
 uint32_t size_called,size_returned,information_called,information_returned;
 int hash_finish_status;uint32_t hash_finish_returned,hash_finished,complete;
};
/* Sole synchronous borrower of the retained original held file. The caller
 * supplies already admitted complete finite row/storage capacity, never an
 * allowance. Successful digest custody is not source or namespace authority. */
int slcli_source_stream_windows_original(struct slcli_source_stream_windows *,
 struct slcli_arena *,struct slcli_source_file_windows *,
 const struct slcli_native_clock *,size_t,const uint32_t *,size_t);
int slcli_source_stream_windows_matches_original(const struct slcli_source_stream_windows *,
 struct slcli_arena *,const struct slcli_source_file_windows *);
int slcli_source_stream_windows_claimed_original(struct slcli_source_stream_windows *,
 struct slcli_arena *,struct slcli_source_file_windows *,
 const struct slcli_native_clock *,size_t,const uint32_t *,size_t,
 struct slcli_binding_storage_original *,const struct slcli_native_reference *,const uint32_t *);
#endif
#endif
