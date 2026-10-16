/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_SOURCE_ASSOCIATION_UNIT_H
#define SLCLI_SOURCE_ASSOCIATION_UNIT_H
#include "source_association_native.h"
#include "source_unit12_native.h"
#if defined(_WIN32) || defined(__linux__)
/* Returning correspondence of every actually observed row, not authentic
 * original source/capture/provider/publication membership. No success token,
 * reusable owner, allocation, path lookup or native effect is produced. The
 * genuine installer must bracket this scan with its original deadline and
 * independently join every source owner/member/Git edge before publication. */
int slcli_source_association_unit_matches_original(
 const struct slcli_source_association_native *,const struct slcli_source_unit12_native *);
#endif
#endif
