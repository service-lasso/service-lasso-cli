#ifndef SLCLI_HA2_LARGE_VECTORS_H
#define SLCLI_HA2_LARGE_VECTORS_H
#include "ha2_large_members.h"
/* Representation-only source drivers; authentic source/native ownership and
 * full semantics are not created by supplied body, length or work plan. */
int slcli_ha2_large_positive_vector(const unsigned char *,size_t,const unsigned char *,size_t,
  const struct slcli_json_ha2_plan *,struct slcli_ha2_large_members_view *);
int slcli_ha2_large_failure_vector(const unsigned char *,size_t,const unsigned char *,size_t,
  const struct slcli_json_ha2_plan *,struct slcli_ha2_large_members_view *,
  const struct slcli_ha2_large_members_view *);
#if defined(_WIN32) || defined(__linux__)
#include "ha2_large_consume.h"
int slcli_ha2_large_positive_original(struct slcli_ha2_large_consumer *,slcli_input_handle);
#endif
#endif
