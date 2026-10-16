#ifndef SLCLI_MANAGEMENT_ENROLLMENT_BUILDER_H
#define SLCLI_MANAGEMENT_ENROLLMENT_BUILDER_H
#include "enrollment_signature.h"
#include "../host-service/profile.h"
#include "../host-service/grant.h"
#include "../host-service/input_body.h"
#include "credential_original.h"
struct slcli_management_enrollment_originals {
  const unsigned char *associations[SLCLI_HE_ASSOCIATIONS];
  const unsigned char *key_version,*seed,*anchor,*profile,*grant;
  const struct slcli_management_credential *credential;
  size_t key_version_bytes,profile_bytes,grant_bytes;
  uint32_t purpose;
};
struct slcli_management_enrollment_builder {
  struct slcli_arena *arena;
  struct slcli_management_enrollment_signature *signer;
  const struct slcli_management_enrollment_originals *originals;
  struct slcli_profile profile;
  struct slcli_grant grant;
  struct slcli_ha2_enrollment decoded;
  struct slcli_sha256 hash;
  unsigned char raw[SLCLI_HA2_ENROLLMENT_LIMIT],anchor_hash[32];
  size_t bytes,prefix_bytes;
  uint32_t started,failed,complete;
};
struct slcli_management_enrollment_builder *slcli_management_enrollment_builder_allocate(struct slcli_arena *);
/* PRIVATE BQ source builder only. Native source/root/credential/caller and own
 * P/G leases preexist; raw bytes and signing cannot authenticate those leases
 * or create a compound birth. Entire immutable anchor+enrollment unit follows. */
int slcli_management_enrollment_build(struct slcli_management_enrollment_builder *,
  const struct slcli_management_enrollment_originals *);
int slcli_management_enrollment_positive_original(struct slcli_management_enrollment_builder *,
  const struct slcli_management_enrollment_originals *);
#endif
