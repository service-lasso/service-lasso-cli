#ifndef SLCLI_WRITER_MODE_WINDOWS_H
#define SLCLI_WRITER_MODE_WINDOWS_H
#include "../host-service/file_windows.h"
#include "../host-service/evidence_deadline.h"
#if defined(_WIN32)
struct slcli_writer_mode_capture;
typedef int (*slcli_writer_mode_sink)(void *,const struct slcli_writer_mode_capture *);
/* Original query/impersonation tokens and their issuer/actor admission come
 * from the original private owner. These native observations only correlate
 * their current raw state. No process token/path/UID selects a new principal. */
struct slcli_writer_mode_capture {
  HANDLE original,effective_token;
  TOKEN_STATISTICS before,after;
  uint64_t token_args[2][5],security_args[5],access_args[8];
  uint64_t user_args[2][5],user_raw[2][64];
  BOOL user_ok[2];
  DWORD user_bytes[2];
  BOOL token_ok[2],security_ok,owner_ok,dacl_ok,control_ok,access_ok;
  DWORD token_bytes[2],security_bytes,privilege_bytes,granted;
  BOOL access_status,owner_defaulted,dacl_defaulted,dacl_present;
  SECURITY_DESCRIPTOR_CONTROL control;
  DWORD revision,error;
  PSID owner;
  PACL dacl;
  GENERIC_MAPPING mapping;
  uint64_t security[64],privileges[64];
  struct slcli_windows_file_capture file;
  struct slcli_evidence_file *evidence_file;
  HANDLE *birth_cell,bound_effective;
  const TOKEN_STATISTICS *bound_token;
  const unsigned char *bound_owner;
  size_t bound_owner_bytes;
  slcli_writer_mode_sink bound_sink;
  void *bound_archive;
  struct slcli_native_clock call_clocks[20][2];
  uint32_t call_count,call_called[20],call_returned[20],call_kind[20],unresolved;
  BOOL sid_valid[3],security_valid,acl_valid,file_observe_ok;
  DWORD sid_length[3];
  uint32_t used,failed,complete,mode;
};
int slcli_writer_mode_prepare_unborn(struct slcli_arena *,struct slcli_writer_mode_capture *,
  struct slcli_evidence_file *,HANDLE *,HANDLE,const TOKEN_STATISTICS *,
  const unsigned char *,size_t,slcli_writer_mode_sink,void *);
int slcli_writer_mode_bound(const struct slcli_writer_mode_capture *,struct slcli_evidence_file *);
int slcli_writer_observe_windows_mode(struct slcli_writer_mode_capture *,HANDLE,HANDLE,
  const TOKEN_STATISTICS *,const unsigned char *,size_t,slcli_writer_mode_sink,void *);
int slcli_writer_observe_windows_directory(struct slcli_writer_mode_capture *,HANDLE,HANDLE,
  const TOKEN_STATISTICS *,const unsigned char *,size_t,slcli_writer_mode_sink,void *);
#endif
#endif
