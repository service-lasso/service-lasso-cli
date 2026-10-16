#ifndef SLCLI_WRITER_PATHS_WINDOWS_H
#define SLCLI_WRITER_PATHS_WINDOWS_H
#include "materialize_windows.h"
#include "../host-service/source_deadline.h"
#if defined(_WIN32)
struct slcli_writer_windows_path {
  HANDLE parent,original;
  FILE_ID_INFO parent_before,parent_after;
  FILE_STANDARD_INFO parent_standard;
  FILE_ATTRIBUTE_TAG_INFO parent_attributes;
  BOOL parent_id_ok[2],parent_standard_ok,parent_attributes_ok;
  uint64_t parent_args[4][4],create_args[11];
  NTSTATUS create_status;
  IO_STATUS_BLOCK create_io;
  OBJECT_ATTRIBUTES attributes;
  UNICODE_STRING name;
  WCHAR leaf[256];
  uint64_t descriptor[64];
  DWORD descriptor_bytes,error;
  struct slcli_writer_mode_capture mode;
  /* Nine finite outer native calls. Mode retains its own internal raw calls;
   * this outer row does not qualify those calls or bound native latency. */
  struct slcli_native_clock call_clocks[9][2];
  uint32_t call_count,call_called[9],call_returned[9],evidence_unresolved;
  uint64_t evidence_previous_tick;
  BOOL descriptor_valid,descriptor_control_ok;
  int mode_observe_ok;
  SECURITY_DESCRIPTOR_CONTROL descriptor_control;
  DWORD descriptor_revision,descriptor_length;
  uint32_t used,failed,complete,retained_native_io,directory;
  struct slcli_native_clock evidence_entry_clock,evidence_birth_clock;
  const struct slcli_writer_windows_evidence_originals *evidence_original;
  struct slcli_writer_windows_evidence_originals *evidence_plan;
  struct slcli_evidence_file *evidence_file;
  /* Actual sole readonly-handle borrower, consumed before DuplicateHandle.
   * No close/disposition permission follows from this association. */
  struct slcli_writer_windows_readback_original *readback_borrower;
  /* Sole first105 append borrower, consumed even on a failed native prefix. */
  struct slcli_writer_bq_record_windows *bq_record_borrower;
};
typedef int (*slcli_writer_windows_path_sink)(void *,const struct slcli_writer_windows_path *);
/* Private independently admitted native entry inputs; no designation/grant
 * follows from this plan's pointer, owner SID, root ID or successful creation. */
struct slcli_writer_windows_evidence_originals {
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_profile *bq_profile;
  const void *caller;
  size_t caller_bytes,capture_bytes;
  HANDLE parent,effective_token;
  const FILE_ID_INFO *parent_identity;
  const TOKEN_STATISTICS *token;
  const WCHAR *leaf;
  size_t leaf_units;
  const unsigned char *descriptor,*owner;
  size_t descriptor_bytes,owner_bytes;
  uint64_t maximum,predecessor;
  slcli_writer_windows_path_sink path_sink;
  slcli_writer_mode_sink mode_sink;
  slcli_store_capture_sink capture_sink;
  void *capture_owner;
};
int slcli_writer_create_windows_evidence_original(struct slcli_arena *,
  struct slcli_writer_windows_path *,struct slcli_writer_windows_output *,
  const struct slcli_writer_windows_evidence_originals *);
int slcli_writer_create_windows_bq_original(struct slcli_arena *,
  struct slcli_writer_windows_path *,struct slcli_writer_windows_output *,
  const struct slcli_writer_windows_evidence_originals *);
/* Raw exclusive original construction, never issuer/profile/source admission.
 * Original native parent identity and immutable self-relative security body
 * come from separately admitted owner custody. All rows are precharged and
 * retained through failures, pending NT calls and eventual actual closure. */
int slcli_writer_create_windows_file(struct slcli_writer_windows_path *,HANDLE,
  const FILE_ID_INFO *,const WCHAR *,size_t,const unsigned char *,size_t,
  HANDLE,const TOKEN_STATISTICS *,const unsigned char *,size_t,
  slcli_writer_windows_path_sink,slcli_writer_mode_sink,void *);
int slcli_writer_create_windows_directory(struct slcli_writer_windows_path *,HANDLE,
  const FILE_ID_INFO *,const WCHAR *,size_t,const unsigned char *,size_t,
  HANDLE,const TOKEN_STATISTICS *,const unsigned char *,size_t,
  slcli_writer_windows_path_sink,slcli_writer_mode_sink,void *);
int slcli_writer_begin_windows_created_file(struct slcli_writer_windows_path *,
  struct slcli_writer_windows_output *,uint64_t,slcli_store_capture_sink,void *);
#endif
#endif
