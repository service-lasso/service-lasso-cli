#ifndef SLCLI_SOURCE_TRANSITION_H
#define SLCLI_SOURCE_TRANSITION_H
/* Compile-time owning platform selection; no runtime caller-selected owner. */
#if defined(_WIN32)
#include "source_windows.h"
#define slcli_source_owner slcli_source_windows
#define slcli_source_enter slcli_source_windows_enter
#define slcli_source_pending slcli_source_windows_pending
#define slcli_source_continue slcli_source_windows_continue
#define slcli_source_leave slcli_source_windows_leave
#elif defined(__linux__)
#include "source_linux.h"
#define slcli_source_owner slcli_source_linux
#define slcli_source_enter slcli_source_linux_enter
#define slcli_source_pending slcli_source_linux_pending
#define slcli_source_continue slcli_source_linux_continue
#define slcli_source_leave slcli_source_linux_leave
#endif
#endif
