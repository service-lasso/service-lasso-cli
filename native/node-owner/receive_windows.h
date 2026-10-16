#ifndef SLCLI_NODE_RECEIVE_WINDOWS_H
#define SLCLI_NODE_RECEIVE_WINDOWS_H
#include "parent_windows.h"
#if defined(_WIN32)
/* Complete raw/decode/query overlap belongs to the source-selected containing
 * image; mapped ingress is its separate prebirth reserved extent. Neither
 * this object nor parsed bytes creates a ledger or authentic input grant. */
struct slcli_node_receive_windows {
  HANDLE transport_original,parent_original;
  const void *entry_original;
  void *mapping;
  MEMORY_BASIC_INFORMATION geometry;
  struct slcli_native_clock before[3],after[3],copy_before,copy_after;
  uint64_t args[3][5],results[3];
  DWORD errors[3],handle_flags;
  uint32_t called[3],returned[3],error_observed[3],before_result[3],after_result[3];
  unsigned char raw[SLCLI_BOOTSTRAP_TRANSPORT_BYTES];
  struct slcli_node_bootstrap_decode decode;
  struct slcli_node_parent_windows parent;
  size_t copied;
  int decode_result,parent_result;
  uint32_t started,copy_returned,decode_returned,parent_returned;
  uint32_t mapping_observed,terminal,custody_returned;
};
/* Actual earliest-entry owns c in zero image storage. The separate selector
 * routes these inherited operands, never source/namespace/rights authority.
 * Successful custody remains insufficient for owner import/CRT continuation. */
int slcli_node_receive_windows_original(struct slcli_node_receive_windows *,
  HANDLE,HANDLE,const void *);
#endif
#endif
