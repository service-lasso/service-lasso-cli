#ifndef SLCLI_PRODUCT_BIRTH_WINDOWS_H
#define SLCLI_PRODUCT_BIRTH_WINDOWS_H
#include "product_deadline_windows.h"
#include "../build-owner/held_file_windows.h"
#if defined(_WIN32)
/* Private source-owned operands, not a nativecase/reservation issuer. */
struct slcli_product_windows_birth_source {
 const struct slcli_build_held_file_windows *image_original;
 const WCHAR *command_original,*environment_original,*directory_original;
 size_t command_units,environment_units,directory_units,attribute_capacity;
 const void *process_descriptor_original,*thread_descriptor_original;
 size_t process_descriptor_bytes,thread_descriptor_bytes;
 HANDLE standard_original[3];
};
enum {SLCLI_PRODUCT_BIRTH_FLAGS=0,SLCLI_PRODUCT_BIRTH_SD_VALID=5,
 SLCLI_PRODUCT_BIRTH_SD_LENGTH=7,SLCLI_PRODUCT_BIRTH_CREATOR_PID=9,
 SLCLI_PRODUCT_BIRTH_CREATOR_TID,SLCLI_PRODUCT_BIRTH_ATTRIBUTE_SIZE,
 SLCLI_PRODUCT_BIRTH_ATTRIBUTE_INIT,SLCLI_PRODUCT_BIRTH_ATTRIBUTE_HANDLES,
 SLCLI_PRODUCT_BIRTH_CREATE,SLCLI_PRODUCT_BIRTH_TIMES,
 SLCLI_PRODUCT_BIRTH_CHILD_PID,SLCLI_PRODUCT_BIRTH_CHILD_TID,
 SLCLI_PRODUCT_BIRTH_CALLS};
struct slcli_product_windows_birth {
 struct slcli_product_storage_windows *storage_original;
 const struct slcli_product_windows_birth_source *source_original;
 struct slcli_product_windows_birth_source source_snapshot;
 const struct slcli_source_deadline_original *deadline_original;
 const struct slcli_source_deadline_original *phase_original;
 const struct slcli_native_clock *origin_original;
 uint64_t origin_tick,enclosing_expires,expires,prior_tick;
 struct slcli_build_windows_call calls[SLCLI_PRODUCT_BIRTH_CALLS];
 struct slcli_native_clock entry,finish;
 WCHAR *command_original;
 void *attributes_original;
 SIZE_T attribute_required,attribute_returned;
 HANDLE inherited_original[5];DWORD flags_original[5];
 SECURITY_ATTRIBUTES process_security,thread_security;
 STARTUPINFOEXW startup_original;
 PROCESS_INFORMATION process_original;
 FILETIME creation_original,exit_original,kernel_original,user_original;
 uint32_t started,failed,child_effects_possible,birth_captured;
};
/* Actual fixed suspended PUBLIC birth. No resume, attach, route write, child
 * permission or retirement. Full source/native/prebirth admission precedes it. */
int slcli_product_windows_birth_original(struct slcli_product_storage_windows *,
 const struct slcli_product_windows_birth_source *);
int slcli_product_windows_birth_output_away_original(
 const struct slcli_product_windows_birth *,const void *,size_t);
#endif
#endif
