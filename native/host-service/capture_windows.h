#ifndef SLCLI_CAPTURE_WINDOWS_H
#define SLCLI_CAPTURE_WINDOWS_H
#include "capture_record.h"
#include "close_native.h"
#include "primary_engine.h"
#include "../writer/paths_windows.h"
#if defined(_WIN32)
struct slcli_capture_store_owner;
struct slcli_capture_store_loan {
  struct slcli_capture_store_loan *next;
  struct slcli_capture_store_owner *owner;
  struct slcli_capture_windows *graph;
  uint64_t maximum,actual,ordinal;
  HANDLE writer,readonly;
  uint32_t native_started,copy_held;
};
/* One original root and SAME primary retained byte counter, serialized by its
 * native owner. Counts and bytes remain charged on every outcome; no release
 * API fabricates terminal/archive closure. Admission is separately required. */
struct slcli_capture_store_owner {
  struct slcli_arena *arena;
  struct slcli_primary_engine *primary;
  struct slcli_source_windows *source_owner;
  struct slcli_capture_store_loan *first;
  HANDLE root;
  FILE_ID_INFO root_identity;
  uint64_t count,maximum_files;
};
struct slcli_capture_windows {
  struct slcli_arena *arena;
  struct slcli_capture_store_loan *store_loan;
  struct slcli_writer_windows_path *created;
  struct slcli_evidence_file *file;
  struct slcli_capture_record *record;
  struct slcli_peer_windows_capture *peer;
  struct slcli_capture_record_reader *reader;
  struct slcli_close_capture writer_close;
  struct slcli_windows_file_capture root_before,root_after,readonly;
  struct slcli_native_clock writer_birth_clock,peer_entry_clock,close_entry_clock;
  struct slcli_native_clock reopen_entry_clock,reopen_birth_clock,readback_entry_clock;
  HANDLE reopened;
  OBJECT_ATTRIBUTES attributes;
  UNICODE_STRING name;
  IO_STATUS_BLOCK io;
  NTSTATUS status;
  uint64_t open_args[11];
  uint32_t used,failed,complete,pending;
};
struct slcli_capture_windows_originals {
  const struct slcli_source_deadline_original *deadline_original;
  HANDLE root,effective_token;
  const FILE_ID_INFO *root_identity;
  const TOKEN_STATISTICS *file_token;
  const unsigned char *file_owner,*descriptor;
  size_t file_owner_bytes,descriptor_bytes;
  const WCHAR *leaf;
  size_t leaf_units;
  const struct slcli_peer_windows_originals *peer;
  slcli_writer_windows_path_sink path_sink;
  slcli_writer_mode_sink mode_sink;
  slcli_store_capture_sink file_sink;
  void *archive_original;
  size_t archive_original_bytes;
};
typedef int (*slcli_capture_windows_sink)(void *,const struct slcli_capture_windows *);
struct slcli_capture_store_owner *slcli_capture_store_allocate(struct slcli_arena *,
  struct slcli_primary_engine *,HANDLE,const FILE_ID_INFO *,uint64_t);
/* One serialized full path/workspace original, shared across retained graphs. */
struct slcli_peer_path_workspace *slcli_capture_windows_workspaces_allocate(
  struct slcli_arena *,struct slcli_shared_workspace **);
/* Complete resident graph allocated in one actual calloc block. Workspaces
 * are SAME arena charged originals shared under serialized native ownership.
 * Row capacities bound retained attempts; exhaustion retains failed originals. */
struct slcli_capture_windows *slcli_capture_windows_allocate(
  struct slcli_capture_store_owner *,struct slcli_shared_workspace *,
  struct slcli_peer_path_workspace *,size_t,size_t,size_t,size_t);
/* Concrete private file/capture/raw-reader chain. Every native original and
 * reservation must originate in the independently admitted original producer/
 * reader source owners; this constructor grants NONE of those authorities.
 * No actor/source grant, FREE, archive ACK or Q32-to-budget conversion occurs. */
int slcli_capture_windows_original(struct slcli_capture_windows *,
  const struct slcli_capture_windows_originals *,slcli_capture_windows_sink,void *);
#endif
#endif
