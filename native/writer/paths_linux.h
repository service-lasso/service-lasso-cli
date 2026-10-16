#ifndef SLCLI_WRITER_PATHS_LINUX_H
#define SLCLI_WRITER_PATHS_LINUX_H
#include "materialize_linux.h"
#include "../host-service/source_deadline.h"
#if defined(__linux__)
#include <linux/time_types.h>
/* Fresh precharged rows retain actual syscall arguments/returns and structures.
 * The independently admitted original archive owner must retain each complete
 * row before later effects. This sink neither admits a principal nor supplies
 * source authority. Partial objects are kept; no close/delete/retry exists. */
struct slcli_writer_path_effect {
  struct slcli_native_clock before_clock;
  uint64_t number,args[6],result;
  struct stat raw;
  struct __kernel_timespec clock_raw;
  uint64_t clock_args[2],clock_result,tick;
  uint32_t clock_observed,clock_complete;
  uint32_t observed,retained;
  uint32_t called,returned;
};
typedef int (*slcli_writer_path_sink)(void *,const struct slcli_writer_path_effect *);
struct slcli_writer_path_original {
  int parent,original,route_original,reopened;
  char name[256],published[256];
  uint64_t admitted_uid,admitted_gid;
  struct stat parent_identity;
  struct slcli_writer_path_effect effects[20];
  size_t count;
  slcli_writer_path_sink sink;
  void *archive_original;
  uint32_t used,created,complete,failed,publish_used,published_complete;
  struct slcli_native_clock evidence_entry_clock,evidence_birth_clock;
  const struct slcli_writer_linux_evidence_originals *evidence_original;
  struct slcli_writer_linux_evidence_originals *evidence_plan;
  struct slcli_evidence_file *evidence_file;
  uint64_t evidence_previous_tick;
  uint32_t evidence_unresolved;
};
/* PRIVATE already admitted native entry plan, never registration/authority.
 * Original parent/owner/source/caller and full store reservation preexist.
 * Name/UID/stat/native success cannot designate those originals. */
struct slcli_writer_linux_evidence_originals {
  const struct slcli_source_deadline_original *deadline;
  const void *caller;
  size_t caller_bytes,capture_bytes;
  const struct stat *parent_identity;
  const char *name;
  size_t name_bytes;
  uint64_t uid,gid,maximum,predecessor;
  int parent;
  unsigned mode;
  slcli_writer_path_sink path_sink;
  slcli_store_capture_sink capture_sink;
  void *capture_owner;
};
int slcli_writer_create_linux_evidence_original(struct slcli_arena *,
  struct slcli_writer_path_original *,struct slcli_writer_output_original *,
  const struct slcli_writer_linux_evidence_originals *);
/* Caller supplies a genuinely admitted held parent and original native owner.
 * Name/UID/GID/stat comparison do not manufacture that admission. The file
 * result connects directly to the same existing evidence/materializer bodies. */
int slcli_writer_create_original(struct slcli_writer_path_original *,int,
  const struct stat *,const char *,size_t,uint64_t,uint64_t,unsigned,
  slcli_writer_path_sink,void *);
int slcli_writer_begin_created_file(struct slcli_writer_path_original *,
  struct slcli_writer_output_original *,uint64_t,slcli_store_capture_sink,void *);
int slcli_writer_publish_directory(struct slcli_writer_path_original *,const char *,size_t);
int slcli_writer_path_clock(struct slcli_writer_path_effect *);
#endif
#endif
