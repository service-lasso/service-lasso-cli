#ifndef SLCLI_HA2_JOURNAL_CODEC_H
#define SLCLI_HA2_JOURNAL_CODEC_H
#include "crypto.h"
#include "wire.h"
#define SLCLI_HA2_JOURNAL_BYTES 772u
#define SLCLI_HA2_JOURNAL_SIGNED_BYTES 708u
#define SLCLI_HA2_JOURNAL_ASSOCIATIONS 18u
#define SLCLI_HA2_JOURNAL_RECORD_LIMIT 1024u
#define SLCLI_HA2_JOURNAL_FILE_BYTES (SLCLI_HA2_JOURNAL_RECORD_LIMIT*SLCLI_HA2_JOURNAL_BYTES)
_Static_assert(SLCLI_HA2_JOURNAL_FILE_BYTES==790528u,"Exact 1024-record journal extent");
#define SLCLI_HA2_JOURNAL_SIGNATURE_DOMAIN "ServiceLassoHA2RealmJournal1"
#define SLCLI_HA2_JOURNAL_PREIMAGE_BYTES \
  (sizeof(SLCLI_HA2_JOURNAL_SIGNATURE_DOMAIN)+4u+SLCLI_HA2_JOURNAL_SIGNED_BYTES)
_Static_assert(SLCLI_HA2_JOURNAL_PREIMAGE_BYTES==741u,"Exact journal domain/NUL/length/prefix extent");
enum slcli_ha2_journal_state { SLCLI_HA2_PREPARED=1,SLCLI_HA2_ACTIVE,
  SLCLI_HA2_REVOKING,SLCLI_HA2_REVOKED,SLCLI_HA2_QUARANTINED };
struct slcli_ha2_journal_record {
  const unsigned char *raw,*associations[18],*key_version,*signature;
  uint64_t sequence,generation,prior_offset,prior_bytes;
  uint32_t state,key_version_bytes;
};
/* Byte representation only. Native SAME BQ/high-water, source-owned request
 * descriptor/frontier/event, independent anchor/signature and pair durability
 * must all be checked by the installed owner before any authority transition. */
int slcli_ha2_journal_record_parse(const unsigned char *,size_t,
                                    struct slcli_ha2_journal_record *);
struct slcli_ha2_journal_prefix_originals {
  uint64_t sequence,generation;
  uint32_t state;
  const unsigned char (*associations)[32];
  const unsigned char *key_version;
  size_t key_version_bytes;
};
/* Render from the separately retained BEFORE-effect eighteen-field native
 * frontier in source-owned storage. Shape validation supplies no phase/source
 * authority; caller retains unsuccessful output and never reuses the range. */
int slcli_ha2_journal_prefix_original(unsigned char *,size_t,
  const struct slcli_ha2_journal_prefix_originals *);
/* Signing prefix exists BEFORE detached signature. This deliberately validates
 * only exact first708, avoiding a signature/signature-preimage birth cycle. */
size_t slcli_ha2_journal_signature_preimage(unsigned char *,size_t,
                                            const unsigned char *,size_t);
#endif
