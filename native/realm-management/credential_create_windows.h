#ifndef SLCLI_MANAGEMENT_CREDENTIAL_CREATE_WINDOWS_H
#define SLCLI_MANAGEMENT_CREDENTIAL_CREATE_WINDOWS_H
#include "../host-service/source_deadline.h"
#include "../host-service/store_body.h"
#if defined(_WIN32)
/* PRIVATE raw64 original7/16 creator. Native actor/designation/parent
 * ancestor/namespace lease and the FULL independently admitted ACL policy
 * preexist. Bounded selfrelative shape/owner/DACL equality is custody only;
 * it never proves that an arbitrary protected ACL grants only permitted actors. */
struct slcli_management_credential_windows_creation {
  struct slcli_arena *arena;
  const struct slcli_source_deadline_original *deadline;
  const struct slcli_native_clock *origin;
  const void *caller_original;
  size_t caller_bytes;
  const WCHAR *name_original;
  size_t name_bytes;
  WCHAR name[65];
  const unsigned char *security_original;
  size_t security_bytes;
  union { uint64_t alignment;unsigned char raw[512]; } security,security_readback;
  HANDLE parent_original,created_original;
  uint32_t original_role;
  UNICODE_STRING unicode_name;
  OBJECT_ATTRIBUTES attributes;
  IO_STATUS_BLOCK create_io;
  NTSTATUS create_status;
  uint64_t create_args[11],security_args[5];
  BOOL security_status;
  DWORD security_returned_bytes,security_error;
  struct slcli_windows_file_capture parent_before,created,parent_after;
  struct slcli_native_clock clocks[10];
  slcli_store_capture_sink sink;
  void *capture_original;
  size_t capture_original_bytes;
  uint64_t expires,origin_tick,previous_tick;
  uint32_t prepared,started,unresolved,native_called,native_return_observed;
  uint32_t native_output_retained,native_pending_guard,file_created;
  uint32_t descriptor_acquired,security_called,security_observed,complete;
};
struct slcli_management_credential_windows_creation *slcli_management_credential_windows_creation_allocate(
  struct slcli_arena *,const struct slcli_source_deadline_original *,const void *,size_t,
  HANDLE,uint32_t,const WCHAR *,size_t,const unsigned char *,size_t,
  slcli_store_capture_sink,void *,size_t);
/* One FILE_CREATE operation binds actual created object and returned handle.
 * Every non-completed/failed output remains held; no cleanup or pending reuse.
 * Original qualified regular-file namespace/parent plan preexists. */
int slcli_management_credential_windows_create(struct slcli_management_credential_windows_creation *);
#endif
#endif
