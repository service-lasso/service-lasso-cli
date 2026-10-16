#ifndef SLCLI_PRODUCT_SECTION_WINDOWS_H
#define SLCLI_PRODUCT_SECTION_WINDOWS_H
#include "product_storage_windows.h"
#if defined(_WIN32)
enum { SLCLI_PRODUCT_SD_INITIALIZE=0,SLCLI_PRODUCT_ACL_INITIALIZE,
 SLCLI_PRODUCT_ACE_EFFECTIVE,SLCLI_PRODUCT_ACE_PRIMARY,SLCLI_PRODUCT_SD_OWNER,
 SLCLI_PRODUCT_SD_DACL,SLCLI_PRODUCT_SD_CONTROL,SLCLI_PRODUCT_SD_RELATIVE,
 SLCLI_PRODUCT_SD_VALID,SLCLI_PRODUCT_SD_GET_OWNER,SLCLI_PRODUCT_SD_GET_DACL,
 SLCLI_PRODUCT_SD_GET_CONTROL,SLCLI_PRODUCT_SD_LENGTH,SLCLI_PRODUCT_SD_CALLS };
struct slcli_product_section_windows {
 struct slcli_product_storage_windows *storage_original;
 const struct slcli_source_deadline_original *deadline_original;
 const struct slcli_native_clock *origin_original;
 uint64_t origin_tick,expires,prior_tick;
 struct slcli_build_principal_windows effective_original,primary_original;
 struct slcli_build_windows_call calls[SLCLI_PRODUCT_SD_CALLS];
 SECURITY_DESCRIPTOR absolute_original;
 ACL *acl_original,*observed_acl_original;
 unsigned char *relative_original;
 DWORD acl_bytes,relative_capacity,relative_bytes,observed_length,revision;
 PSID observed_owner_original;
 SECURITY_DESCRIPTOR_CONTROL control;
 BOOL owner_defaulted,dacl_present,dacl_defaulted;
 struct slcli_native_clock start,finish;
 uint32_t started,failed,descriptor_constructed,transport_created;
};
/* Fixed source-generated private descriptor and actual unnamed transport section.
 * Initial/source/profile/loaded admission is independent; no caller ACL or SDDL,
 * capability callback, product permission, child birth or source grant exists. */
int slcli_product_windows_section_create_original(struct slcli_product_storage_windows *);
/* Pure bounded byte/typed-output correspondence only, also used by labelled
 * fixtures. It creates no native output, source/actor membership or permission. */
int slcli_product_windows_descriptor_bytes_correspond_original(
 const struct slcli_product_section_windows *);
#endif
#endif
