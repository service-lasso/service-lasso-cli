//
// locks.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Critical sections used for synchronization in the CoreCRT.
//
#include <corecrt_internal.h>
#include "../../../../node-owner/early_allocator.h"



// This table holds the locks used by the CoreCRT.  It is indexed using the
// enumerators of the __acrt_lock_id enumeration.
static CRITICAL_SECTION __acrt_lock_table[__acrt_lock_count];

// This variable stores the number of locks that have been successfully
// initialized.  Locks are initialized in order and are destroyed in reverse
// order.  The startup and exit code must ensure that initialization and
// destruction is synchronized:  these functions should never be executed
// concurrently.
static unsigned __acrt_locks_initialized;
static bool __acrt_lock_initialization_attempted;



extern "C" bool __cdecl __acrt_initialize_locks()
{
    if (__acrt_lock_initialization_attempted) return false;
    __acrt_lock_initialization_attempted = true;
    for (unsigned i = 0; i < __acrt_lock_count; ++i)
    {
        CRITICAL_SECTION* const original = &__acrt_lock_table[i];
        size_t ordinal;
        if (!slcli_node_allocator_native_before_original(3, original, sizeof(*original),
                _CORECRT_SPINCOUNT, 0, &ordinal)) return false;
        BOOL const initialized = __acrt_InitializeCriticalSectionEx(original, _CORECRT_SPINCOUNT, 0);
        if (!slcli_node_allocator_native_raw_return_original(3, original, sizeof(*original), ordinal,
                static_cast<uint64_t>(initialized)))
            __fastfail(FAST_FAIL_FATAL_APP_EXIT);
        DWORD const error = initialized ? ERROR_SUCCESS : GetLastError();
        uint64_t const result = initialized ? UINT64_C(1) : (static_cast<uint64_t>(error) << 32);
        // A successful native birth remains real even if the following clock
        // denies completion. No row/count reuse follows failed initialization.
        if (initialized) ++__acrt_locks_initialized;
        if (!slcli_node_allocator_native_after_original(3, original, sizeof(*original), ordinal,
                result)) return false;
        if (!initialized)
        {
            __acrt_uninitialize_locks(false);
            return false;
        }
    }

    return true;
}

extern "C" bool __cdecl __acrt_uninitialize_locks(bool const /* terminating */)
{
    for (unsigned i = __acrt_locks_initialized; i > 0; --i)
    {
        CRITICAL_SECTION* const original = &__acrt_lock_table[i - 1];
        size_t ordinal;
        if (!slcli_node_allocator_native_before_original(4, original, sizeof(*original), 0, 0, &ordinal))
            return false;
        DeleteCriticalSection(original);
        if (!slcli_node_allocator_native_raw_return_original(4, original, sizeof(*original), ordinal, 0))
            __fastfail(FAST_FAIL_FATAL_APP_EXIT);
        --__acrt_locks_initialized;
        // Actual void return/prefix is retained before diagnostic failure.
        // This does not retire image/backing/capture or prove archive/exit.
        if (!slcli_node_allocator_native_after_original(4, original, sizeof(*original), ordinal, 0))
            return false;
    }

    return true;
}

extern "C" void __cdecl __acrt_lock(_In_ __acrt_lock_id _Lock)
{
    // This original void API cannot report a denied acquisition by returning.
    // Fatal failure grants no success, unlock, retirement or deadline renewal.
    // The containing outside owning book still must preserve every capture and
    // observe the exact abnormal exit; this local path alone proves no custody.
    if (static_cast<unsigned>(_Lock) >= __acrt_locks_initialized)
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    CRITICAL_SECTION* const original = &__acrt_lock_table[_Lock];
    for (;;) {
        size_t ordinal;
        if (!slcli_node_allocator_native_before_original(1, original, sizeof(*original), 0, 0, &ordinal))
            __fastfail(FAST_FAIL_FATAL_APP_EXIT);
        BOOL const acquired = TryEnterCriticalSection(original);
        if (!slcli_node_allocator_native_raw_return_original(1, original, sizeof(*original), ordinal,
                static_cast<uint64_t>(acquired)))
            __fastfail(FAST_FAIL_FATAL_APP_EXIT);
        if (!slcli_node_allocator_native_after_original(1, original, sizeof(*original), ordinal,
                static_cast<uint64_t>(acquired)))
            __fastfail(FAST_FAIL_FATAL_APP_EXIT);
        if (acquired) return;
        // Each next attempt must reserve two new charged rows and observe the
        // SAME original deadline. Native call latency remains unqualified.
    }
}

extern "C" void __cdecl __acrt_unlock(_In_ __acrt_lock_id _Lock)
{
    if (static_cast<unsigned>(_Lock) >= __acrt_locks_initialized)
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    CRITICAL_SECTION* const original = &__acrt_lock_table[_Lock];
    size_t ordinal;
    if (!slcli_node_allocator_native_before_original(2, original, sizeof(*original), 0, 0, &ordinal))
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    LeaveCriticalSection(original);
    if (!slcli_node_allocator_native_raw_return_original(2, original, sizeof(*original), ordinal, 0))
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    // The API has no result value: 0 records its void return only, never a
    // fabricated independent observation of final object/borrow disposition.
    if (!slcli_node_allocator_native_after_original(2, original, sizeof(*original), ordinal, 0))
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

extern "C" void __cdecl _lock_locales()
{
	__acrt_eagerly_load_locale_apis();
    __acrt_lock(__acrt_locale_lock);
}

extern "C" void __cdecl _unlock_locales()
{
    __acrt_unlock(__acrt_locale_lock);
}
