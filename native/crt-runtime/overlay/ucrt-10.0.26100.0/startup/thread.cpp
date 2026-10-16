//
// thread.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The _beginthread(), _beginthreadex(), _endthread(), and _endthreadex().
//
// There are several key differences in behavior between _beginthread() and
// _beginthreadex():
//
//  * _beginthreadex() takes three additional parameters, which are passed on to
//    CreateThread():  a security descriptor for the new thread, the initial
//    thread state (running or asleep), and an optional out parameter to which
//    the thread id of the newly created thread will be stored.
//
//  * The procedure passed to _beginthread() must be __cdecl and has no return
//    code.  The routine passed to _beginthreadex() must be __stdcall and must
//    return a return code, which will be used as the thread exit code.
//    Likewise, _endthread() takes no parameter and always returns a thread exit
//    code of 0 if the thread exits without error, whereas _endthreadex() takes
//    an exit code.
//
//  * _endthread() calls CloseHandle() on the handle returned from CreateThread().
//    Note that this means that a caller should not use this handle, since it is
//    possible that the thread will have terminated and the handle will have been
//    closed by the time that _beginthread() returns.
//
//    _endthreadex() does not call CloseHandle() to close the handle:  the caller
//    of _beginthreadex() is required to close the handle.
//
//  * _beginthread() returns -1 on failure.  _beginthreadex() returns zero on
//    failure (just as CreateThread() does).
//
#include <corecrt_internal.h>
#include <process.h>
#include <roapi.h>
#include "../../../../node-owner/thread_windows.h"

// In some compilation models, the compiler is able to detect that the return
// statement at the end of thread_start is unreachable.  We cannot suppress the
// warning locally because it is a backend warning.
#pragma warning(disable: 4702) // unreachable code
#pragma warning(disable: 4984) // 'if constexpr' is a C++17 language extension


namespace
{
    struct thread_parameter_free_policy
    {
        void operator()(__acrt_thread_parameter* const parameter) throw()
        {
            if (!parameter)
            {
                return;
            }

            if (parameter->_thread_handle)
            {
                CloseHandle(parameter->_thread_handle);
            }

            if (parameter->_module_handle)
            {
                FreeLibrary(parameter->_module_handle);
            }

            _free_crt(parameter);
        }
    };

    using unique_thread_parameter = __crt_unique_heap_ptr<
        __acrt_thread_parameter,
        thread_parameter_free_policy>;
}

template <typename ThreadProcedure, bool Ex>
static unsigned long WINAPI thread_start(void* const parameter) throw()
{
    if (!parameter)
    {
        DWORD const error = GetLastError();
        slcli_node_thread_exit_original(nullptr, error);
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    __acrt_thread_parameter* const context = static_cast<__acrt_thread_parameter*>(parameter);

    __acrt_getptd()->_beginthread_context = context;

    if (__acrt_get_begin_thread_init_policy() == begin_thread_init_policy_ro_initialize)
    {
        context->_initialized_apartment = __acrt_RoInitialize(RO_INIT_MULTITHREADED) == S_OK;
    }

    __try
    {
        ThreadProcedure const procedure = reinterpret_cast<ThreadProcedure>(context->_procedure);
        if constexpr (Ex)
        {
            _endthreadex(procedure(context->_context));
        }
        else
        {
            procedure(context->_context);
            _endthreadex(0);
        }
    }
    __except (_seh_filter_exe(GetExceptionCode(), GetExceptionInformation()))
    {
        // Execution should never reach here:
        _exit(GetExceptionCode());
    }

    // This return statement will never be reached.  All execution paths result
    // in the thread or process exiting.
    return 0;
}



static __acrt_thread_parameter* __cdecl create_thread_parameter(
    void* const procedure,
    void* const context
    ) throw()
{
    unique_thread_parameter parameter(_calloc_crt_t(__acrt_thread_parameter, 1).detach());
    if (!parameter)
    {
        return nullptr;
    }

    parameter.get()->_procedure = procedure;
    parameter.get()->_context   = context;

    // Attempt to bump the reference count of the module in which the user's
    // thread procedure is defined, to ensure that the module will stay loaded
    // as long as the thread is executing.  We will release this HMDOULE when
    // the thread procedure returns or _endthreadex is called.
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(procedure),
        &parameter.get()->_module_handle);

    return parameter.detach();
}

extern "C" uintptr_t __cdecl _beginthread(
    _beginthread_proc_type const procedure,
    unsigned int           const stack_size,
    void*                  const context
    )
{
    _VALIDATE_RETURN(procedure != nullptr, EINVAL, reinterpret_cast<uintptr_t>(INVALID_HANDLE_VALUE));

    unique_thread_parameter parameter(create_thread_parameter(procedure, context));
    if (!parameter)
    {
        return reinterpret_cast<uintptr_t>(INVALID_HANDLE_VALUE);
    }

    // We create the new thread in a suspended state so that we can update
    // the parameter structure with the thread handle.  The newly created
    // thread is responsible for closing this handle.
    slcli_node_thread_birth* const birth = slcli_node_thread_create_original(
        nullptr,
        stack_size,
        thread_start<_beginthread_proc_type, false>,
        parameter.get(),
        sizeof(*parameter.get()),
        context,
        0);
    HANDLE const thread_handle = slcli_node_thread_handle_original(birth);

    if (!thread_handle)
    {
        DWORD const error = slcli_node_thread_error_original(birth);
        if (error) __acrt_errno_map_os_error(error);
        else _set_errno(EAGAIN); // unavailable original custody, no invented native error
        return reinterpret_cast<uintptr_t>(INVALID_HANDLE_VALUE);
    }

    parameter.get()->_thread_handle = thread_handle;
    // Native birth already exists. Keep its parameter/module alive even when
    // a later original deadline/resume check denies caller success. The SAME
    // retained birth/owner must resolve the suspended/live native child.
    parameter.detach();
    if (!slcli_node_thread_birth_ready_original(birth))
    {
        _set_errno(EAGAIN);
        return reinterpret_cast<uintptr_t>(INVALID_HANDLE_VALUE);
    }

    // Now we can start the thread...
    if (!slcli_node_thread_resume_original(birth))
    {
        DWORD const error = slcli_node_thread_resume_error_original(birth);
        if (error) __acrt_errno_map_os_error(error);
        else _set_errno(EAGAIN);
        return reinterpret_cast<uintptr_t>(INVALID_HANDLE_VALUE);
    }

    // If we successfully created the thread, the thread now owns its parameter:
    // Parameter ownership was transferred before actual native resume.

    return reinterpret_cast<uintptr_t>(thread_handle);
}

extern "C" uintptr_t __cdecl _beginthreadex(
    void*                    const security_descriptor,
    unsigned int             const stack_size,
    _beginthreadex_proc_type const procedure,
    void*                    const context,
    unsigned int             const creation_flags,
    unsigned int*            const thread_id_result
    )
{
    _VALIDATE_RETURN(procedure != nullptr, EINVAL, 0);

    unique_thread_parameter parameter(create_thread_parameter(procedure, context));
    if (!parameter)
    {
        return 0;
    }

    slcli_node_thread_birth* const birth = slcli_node_thread_create_original(
        reinterpret_cast<LPSECURITY_ATTRIBUTES>(security_descriptor),
        stack_size,
        thread_start<_beginthreadex_proc_type, true>,
        parameter.get(),
        sizeof(*parameter.get()),
        context,
        creation_flags);
    HANDLE const thread_handle = slcli_node_thread_handle_original(birth);

    if (!thread_handle)
    {
        DWORD const error = slcli_node_thread_error_original(birth);
        if (error) __acrt_errno_map_os_error(error);
        else _set_errno(EAGAIN);
        return 0;
    }

    // Retain the actual native child parameter before any diagnostic can fail
    // or resume can run its original CRT pre-PTD entry.
    parameter.detach();
    if (!slcli_node_thread_birth_ready_original(birth))
    {
        _set_errno(EAGAIN);
        return 0;
    }
    if (thread_id_result)
    {
        *thread_id_result = slcli_node_thread_id_original(birth);
    }
    if (!(creation_flags & CREATE_SUSPENDED) && !slcli_node_thread_resume_original(birth))
    {
        DWORD const error = slcli_node_thread_resume_error_original(birth);
        if (error) __acrt_errno_map_os_error(error);
        else _set_errno(EAGAIN);
        return 0;
    }

    // If we successfully created the thread, the thread now owns its parameter:
    // Parameter ownership was transferred before actual native resume.

    return reinterpret_cast<uintptr_t>(thread_handle);
}



static void __cdecl common_end_thread(unsigned int const return_code) throw()
{
    __acrt_ptd* const ptd = __acrt_getptd_noexit();
    if (!ptd)
    {
        slcli_node_thread_exit_original(nullptr, return_code);
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    __acrt_thread_parameter* const parameter = ptd->_beginthread_context;
    if (!parameter)
    {
        slcli_node_thread_exit_original(nullptr, return_code);
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    if (parameter->_initialized_apartment)
    {
        __acrt_RoUninitialize();
    }

    if (parameter->_thread_handle != INVALID_HANDLE_VALUE && parameter->_thread_handle != nullptr)
    {
        auto const completion = slcli_node_thread_detach_original(&parameter->_thread_handle);
        if (!completion.complete)
            __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }

    if (parameter->_module_handle != INVALID_HANDLE_VALUE && parameter->_module_handle != nullptr)
    {
        slcli_node_thread_exit_original(parameter->_module_handle, return_code);
    }
    else
    {
        slcli_node_thread_exit_original(nullptr, return_code);
    }
    __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}

extern "C" void __cdecl _endthread()
{
    return common_end_thread(0);
}

extern "C" void __cdecl _endthreadex(unsigned int const return_code)
{
    return common_end_thread(return_code);
}
