/* SPDX-License-Identifier: Apache-2.0 */
#ifndef SLCLI_BQ_PROVIDER_INGRESS_WINDOWS_H
#define SLCLI_BQ_PROVIDER_INGRESS_WINDOWS_H
#if defined(_WIN32)
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Fixed private qualification-module export, not a public product API.
 * Both operands are routing geometry ONLY. The actual acquisition owner stays
 * module-local; neither this return nor a managed projection transports it.
 * The complete genuine prebirth financing/source/native acquisition body and
 * module build/export association are still implementation obligations.
 * A resident DLL/owner does not preserve non-IOCP IO when its issuing thread
 * exits. This synchronous export must keep the SAME issuing thread live and
 * may return only after genuine settlement of every native IO borrower. A
 * managed Thread.Join or diagnostic return is not native settlement. No IOCP
 * or alternate completion/settlement export is selected by this declaration.
 * No body/default instance or positive stub is provided by this declaration. */
DWORD WINAPI SlcliInitialProviderAcquireOriginal(
 HANDLE endpoint_original,ULONGLONG complete_offer_count);
#ifdef __cplusplus
}
#endif
#endif
#endif
