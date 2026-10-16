#ifndef SLCLI_WRITER_ATTEMPT_H
#define SLCLI_WRITER_ATTEMPT_H
#include "../host-service/input_body.h"
#include "../host-service/wire.h"
#if defined(_WIN32) || defined(__linux__)
struct slcli_writer_entry { struct slcli_slice path,body;uint8_t mode; };
struct slcli_writer_attempt {
  const unsigned char *nonce,*read_receipt,*plan_digest;
  struct slcli_slice destination;
  struct slcli_writer_entry entries[128];
  struct slcli_input_body *original;
  uint32_t count,complete,failed;
};
/* SAME existing protocol3/non-capability contexts. No template semantics,
 * profile/key/input authority or destination actor is created by decoding. */
int slcli_writer_attempt_consume(struct slcli_input_body *,struct slcli_writer_attempt *);
#endif
#endif
