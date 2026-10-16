#ifndef SLCLI_HA2_ENROLLMENT_CODEC_H
#define SLCLI_HA2_ENROLLMENT_CODEC_H
#include "json.h"
#include "crypto.h"
enum slcli_ha2_enrollment_association {SLCLI_HE_REALM=0,SLCLI_HE_HOST,SLCLI_HE_PROVIDER,
  SLCLI_HE_STARTUP,SLCLI_HE_INSTALLER,SLCLI_HE_CALLER,SLCLI_HE_STORE,SLCLI_HE_PROFILE,
  SLCLI_HE_GRANT,SLCLI_HE_CREDENTIAL,SLCLI_HE_MANAGEMENT_ENTRY,SLCLI_HE_ANCHOR,
  SLCLI_HE_ASSOCIATIONS};
#define SLCLI_HA2_ENROLLMENT_LIMIT 16384
#define SLCLI_HA2_ENROLLMENT_DOMAIN_BYTES (sizeof("ServiceLassoHA2RealmEnrollment1"))
#define SLCLI_HA2_ENROLLMENT_PREIMAGE_LIMIT (SLCLI_HA2_ENROLLMENT_DOMAIN_BYTES+4+SLCLI_HA2_ENROLLMENT_LIMIT)
struct slcli_ha2_enrollment {
  const unsigned char *raw;
  size_t bytes,prefix_bytes,key_version_bytes;
  unsigned char associations[SLCLI_HE_ASSOCIATIONS][32],signature[64];
  unsigned char key_version[64],raw_digest[32];
  uint32_t purpose;
};
/* Exact raw private representation, never native compound/source/root grant.
 * Prefix ends immediately after the original flags:0 value, before comma and
 * signature field. UTF8 keyVersion uses literal bytes and minimal quote/backslash
 * escaping, with no normalization or escaped Unicode alternative. */
int slcli_ha2_enrollment_parse(const unsigned char *,size_t,struct slcli_ha2_enrollment *);
size_t slcli_ha2_enrollment_preimage(unsigned char *,size_t,const unsigned char *,size_t);
#endif
