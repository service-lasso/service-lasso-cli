#ifndef SLCLI_WRITER_MATERIALIZE_LINUX_H
#define SLCLI_WRITER_MATERIALIZE_LINUX_H
#include "attempt.h"
#include "../host-service/evidence_native.h"
#if defined(__linux__)
/* These are independently admitted, already exclusively created originals.
 * This body does not infer destination custody from a path or an effective UID.
 * Directory construction, original discovery and no-replace publication remain
 * necessary caller work before an output can be accepted as a product result. */
struct slcli_writer_output_original {
  struct slcli_slice admitted_path;
  struct slcli_evidence_file *file;
  struct slcli_linux_file_capture before,after;
  uint64_t admitted_uid,admitted_gid;
  uint32_t used,complete,failed;
};
struct slcli_writer_materialization {
  struct slcli_writer_attempt *attempt;
  struct slcli_writer_output_original *outputs;
  size_t count;
  uint32_t used,complete,failed;
};
int slcli_writer_materialize_originals(struct slcli_writer_materialization *);
#endif
#endif
