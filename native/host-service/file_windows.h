#ifndef SLCLI_FILE_WINDOWS_H
#define SLCLI_FILE_WINDOWS_H
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <stdint.h>
#include "native_clock.h"
/* Persistent, precharged native capture storage. Unknown NT/IO completion must
 * retain this storage and original handle; it may not become a stack temporary
 * or a reused row. Captures start in original zeroed owned storage; a retained
 * native IO guard forbids subsequent reuse, and completed captures still must
 * reach actual persistence/readback before their owner reuses storage. No
 * function here admits a provider, peer or source lease. Every row is one-use:
 * operation is set before native effects, and all later calls reject the row
 * without changing it, even after observed native completion. No reset API
 * grants evidence/body retirement or permits reuse of a pending output target. */
struct slcli_windows_file_capture {
  /* SAME one-use source row's direct native thread observation. Ordinary
   * captures leave these zero; no thread ID supplies source admission. */
  DWORD source_thread_raw;
  uint32_t source_thread_called,source_thread_returned;
  struct slcli_native_clock source_thread_after;
  int source_thread_clock_result;
  uint32_t source_thread_clock_returned;
  HANDLE original;
  uint32_t operation, observed, completed, bytes_observed, retained_native_io;
  uint64_t offset, requested, returned_bytes, status, error;
  uint64_t mode_args[5], information_args[4][4], security_args[5], read_args[5];
  uint64_t access_args[5], handle_args[2], type_args[1],position_args[4];
  PUBLIC_OBJECT_BASIC_INFORMATION access_raw;
  NTSTATUS access_status;
  ULONG access_bytes;
  DWORD handle_flags, file_type;
  DWORD type_error_seed;
  BOOL handle_status;
  uint32_t error_observed, mode;
  NTSTATUS mode_status;
  IO_STATUS_BLOCK mode_io;
  LARGE_INTEGER position_requested, position_returned;
  BOOL position_status, read_status;
  DWORD transferred_raw;
  FILE_ID_INFO identity;
  FILE_STANDARD_INFO standard;
  FILE_ATTRIBUTE_TAG_INFO attributes;
  FILE_BASIC_INFO basic;
  BOOL identity_status, standard_status, attributes_status, basic_status, security_status;
  BOOL owner_status, sid_status, owner_defaulted;
  PSID owner_pointer;
  unsigned char security[512], owner_sid[184];
  DWORD security_bytes, owner_sid_bytes;
  uint64_t tick;
  LARGE_INTEGER counter_raw, frequency_raw;
  BOOL counter_status, frequency_status;
  uintptr_t view_flush_address;
  SIZE_T view_flush_bytes;
  BOOL view_flush_status;
  uint32_t view_flush_called,view_flush_returned;
  uintptr_t view_unmap_address;
  BOOL view_unmap_status;
  uint32_t view_unmap_called,view_unmap_returned;
};
int slcli_windows_file_observe(HANDLE, struct slcli_windows_file_capture *);
int slcli_windows_directory_observe(HANDLE, struct slcli_windows_file_capture *);
int slcli_windows_directory_source_observe(HANDLE,struct slcli_windows_file_capture *);
int slcli_windows_source_observe(HANDLE, struct slcli_windows_file_capture *);
int slcli_windows_source_observation_matches_original(HANDLE,
 const struct slcli_windows_file_capture *);
int slcli_windows_file_read(HANDLE, unsigned char *, DWORD, uint64_t,
                            struct slcli_windows_file_capture *);
/* Reconstruct exact original synchronous seek/read/mode/raw clock operands.
 * This is native correspondence only, never source/provider admission. */
int slcli_windows_file_read_matches_original(HANDLE,const unsigned char *,DWORD,uint64_t,
 const struct slcli_windows_file_capture *);
int slcli_windows_file_write(HANDLE, const unsigned char *, DWORD, uint64_t,
                             struct slcli_windows_file_capture *);
int slcli_windows_file_flush(HANDLE, struct slcli_windows_file_capture *);
/* The owning source must establish the actual file/view relation first.
 * This primitive captures a flush, never creates that relation or admission. */
int slcli_windows_file_flush_mapped(HANDLE,const void *,SIZE_T,struct slcli_windows_file_capture *);
/* Native alias observation only; caller must prove original file/view/source
 * association and completed borrowers. Never a RANGE or retirement issuer. */
int slcli_windows_file_unmap_original(HANDLE,const void *,struct slcli_windows_file_capture *);
#endif
#endif
