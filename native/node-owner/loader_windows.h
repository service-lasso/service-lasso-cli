#ifndef SLCLI_NODE_LOADER_WINDOWS_H
#define SLCLI_NODE_LOADER_WINDOWS_H
#include "prefix_windows.h"
#if defined(_WIN32)
#include <winternl.h>
struct slcli_node_loader_windows_module {
  const LDR_DATA_TABLE_ENTRY *original;
  LDR_DATA_TABLE_ENTRY before,after;
  IMAGE_DOS_HEADER dos;
  IMAGE_NT_HEADERS64 nt;
  IMAGE_TLS_DIRECTORY64 tls;
  WCHAR *name;
  size_t name_units;
  DWORD tls_index;
  uintptr_t tls_template_begin,tls_template_end;
  uint64_t tls_template_bytes,tls_storage_bytes;
  HMODULE native_reference;
  uint64_t reference_args[3];
  BOOL reference_result;
  DWORD reference_error;
  struct slcli_native_clock reference_before,reference_after;
  uint32_t reference_called,reference_returned,reference_error_observed;
  uint64_t decrement_args[1];
  BOOL decrement_result;
  DWORD decrement_error;
  uint32_t decrement_called,decrement_returned,decrement_error_observed,reference_decremented;
  struct slcli_native_clock begin,end;
  uintptr_t image_end;
  uint32_t started,header_returned,tls_present,tls_index_observed;
  uint32_t name_returned,rechecked,returned;
  uint32_t tls_template_observed;
};
struct slcli_node_loader_windows {
  const struct slcli_node_prefix_windows *prefix_original;
  struct slcli_node_loader_windows_module *modules;
  WCHAR *names;
  size_t capacity,name_capacity,per_name_units,used;
  PROCESS_BASIC_INFORMATION basic;
  NTSTATUS basic_status;
  ULONG basic_bytes;
  uint64_t basic_args[5];
  const PEB *peb_original;
  const PEB_LDR_DATA *ldr_original;
  PEB peb_before,peb_after;
  PEB_LDR_DATA ldr_before,ldr_after;
  const LIST_ENTRY *head_original,*next_original,*previous_original;
  struct slcli_native_clock before,after,finish;
  uint64_t expires,previous_tick;
  uint32_t started,basic_called,basic_returned,terminal,complete_list;
  uint32_t capture_active,capture_returned;
  size_t static_tls_modules;
  ULONG lock_disposition;
  ULONG_PTR lock_cookie;
  NTSTATUS lock_status,unlock_status;
  uint64_t lock_args[3],unlock_args[2];
  struct slcli_native_clock lock_before,lock_after;
  uint32_t lock_called,lock_returned,lock_held;
  uint32_t unlock_called,unlock_returned;
  const IMAGE_TLS_DIRECTORY64 *own_tls_directory_original;
  const ULONG *own_tls_index_original;
  size_t own_tls_module;
  ULONG own_tls_index;
  struct slcli_native_clock own_tls_before,own_tls_after;
  int own_tls_before_result,own_tls_after_result;
  const void *own_tls_slot_original;
  DWORD own_thread;
  MEMORY_BASIC_INFORMATION own_tls_geometry;
  SIZE_T own_tls_query_result;
  DWORD own_tls_query_error;
  uint64_t own_tls_query_args[3];
  uint32_t own_thread_called,own_thread_returned,own_tls_slot_returned;
  uint32_t own_tls_query_called,own_tls_query_returned,own_tls_query_error_observed;
  struct slcli_native_clock own_thread_before,own_thread_after,own_tls_query_before,own_tls_query_after;
  int own_thread_before_result,own_thread_after_result,own_tls_query_before_result,own_tls_query_after_result;
  uint32_t own_tls_started,own_tls_returned,own_tls_symbols_match;
  int own_tls_result;
  uint32_t own_tls_call_returned;
};
/* Storage and complete source-selection basis are genuine entry inputs, never
 * allocated or authenticated by these raw observations. Full module/source/
 * native namespace and staticTLS joins still precede owner import. */
int slcli_node_loader_windows_observe(struct slcli_node_loader_windows *,
  struct slcli_node_loader_windows_module *,size_t,WCHAR *,size_t,size_t,
  const struct slcli_node_prefix_windows *);
/* Invoke only after every original borrower has returned. Release is independent
 * of useful-work deadline; a failed native release remains retained and cannot
 * be retried or represented as successful closure. */
int slcli_node_loader_windows_release(struct slcli_node_loader_windows *);
/* Terminal original native cleanup only, after every borrower has returned and
 * loader serialization was independently released. No useful deadline grants
 * cleanup. A decrement is not an image unload or mapping/TLS FREE observation. */
int slcli_node_loader_windows_decrement_references(struct slcli_node_loader_windows *);
#endif
#endif
