# Private fixed seed A implementation. SOURCE-UNRUN: dot-sourcing/emission is a
# target action and needs fresh ENTIRE review + NEW exact initial admission.
# No caller-controlled library, symbol, signature, instruction or callback table.
function New-SlcliInitialWindowsNative {
    [CmdletBinding()]
    param([Parameter(Mandatory)][object]$Owner)
    $ErrorActionPreference = 'Stop'
    $pointer = [IntPtr]; $u = [UInt32]; $q = [UIntPtr]; $b = [Int32]; $v = [void]
    if ($null -ne $Owner.EmissionOriginal) { throw 'Initial emitter original already exists.' }
    # Install the original before the first metadata effect. Every partial
    # assembly/type/IL prefix remains reachable from this SAME owner on failure.
    $emission = [pscustomobject]@{ Assembly=$null; Module=$null; TypeBuilder=$null;
        Type=$null; Metadata=$null; Methods=$null; Started=$true; Returned=$false }
    $Owner.EmissionOriginal = $emission
    # Fixed source metadata: return, args, cached-last-error convention. All
    # strings/structures/outputs are explicit native pointers, not marshalled
    # managed objects. NTSTATUS is signed32 and never BOOL/last-error.
    $entries = @(
        @('kernel32.dll','GetCurrentProcessId',$u,[Type[]]@(),$false),
        @('kernel32.dll','GetCurrentThreadId',$u,[Type[]]@(),$false),
        @('kernel32.dll','GetCurrentProcess',$pointer,[Type[]]@(),$false),
        @('kernel32.dll','GetCurrentThread',$pointer,[Type[]]@(),$false),
        @('kernel32.dll','QueryPerformanceCounter',$b,[Type[]]@($pointer),$false),
        @('kernel32.dll','QueryPerformanceFrequency',$b,[Type[]]@($pointer),$false),
        @('kernel32.dll','GetSystemInfo',$v,[Type[]]@($pointer),$false),
        @('kernel32.dll','GetProcessTimes',$b,[Type[]]@($pointer,$pointer,$pointer,$pointer,$pointer),$true),
        @('kernel32.dll','GetThreadTimes',$b,[Type[]]@($pointer,$pointer,$pointer,$pointer,$pointer),$true),
        @('advapi32.dll','OpenProcessToken',$b,[Type[]]@($pointer,$u,$pointer),$true),
        @('advapi32.dll','OpenThreadToken',$b,[Type[]]@($pointer,$u,$b,$pointer),$true),
        @('advapi32.dll','GetTokenInformation',$b,[Type[]]@($pointer,$u,$pointer,$u,$pointer),$true),
        @('advapi32.dll','IsValidSid',$b,[Type[]]@($pointer),$false),
        @('advapi32.dll','GetLengthSid',$u,[Type[]]@($pointer),$false),
        @('advapi32.dll','GetKernelObjectSecurity',$b,[Type[]]@($pointer,$u,$pointer,$u,$pointer),$true),
        @('advapi32.dll','GetSecurityDescriptorOwner',$b,[Type[]]@($pointer,$pointer,$pointer),$true),
        @('kernel32.dll','CreateFileW',$pointer,[Type[]]@($pointer,$u,$u,$pointer,$u,$u,$pointer),$true),
        @('kernel32.dll','ReadFile',$b,[Type[]]@($pointer,$pointer,$u,$pointer,$pointer),$true),
        @('kernel32.dll','WriteFile',$b,[Type[]]@($pointer,$pointer,$u,$pointer,$pointer),$true),
        @('kernel32.dll','FlushFileBuffers',$b,[Type[]]@($pointer),$true),
        @('kernel32.dll','SetFilePointerEx',$b,[Type[]]@($pointer,[Int64],$pointer,$u),$true),
        @('kernel32.dll','GetFileInformationByHandleEx',$b,[Type[]]@($pointer,$u,$pointer,$u),$true),
        @('kernel32.dll','GetFileType',$u,[Type[]]@($pointer),$true),
        @('kernel32.dll','GetHandleInformation',$b,[Type[]]@($pointer,$pointer),$true),
        @('kernel32.dll','SetHandleInformation',$b,[Type[]]@($pointer,$u,$u),$true),
        @('kernel32.dll','GetFinalPathNameByHandleW',$u,[Type[]]@($pointer,$pointer,$u,$u),$true),
        @('kernel32.dll','VirtualAlloc',$pointer,[Type[]]@($pointer,$q,$u,$u),$true),
        @('kernel32.dll','VirtualQuery',$q,[Type[]]@($pointer,$pointer,$q),$false),
        @('kernel32.dll','VirtualQueryEx',$q,[Type[]]@($pointer,$pointer,$pointer,$q),$true),
        @('kernel32.dll','ReadProcessMemory',$b,[Type[]]@($pointer,$pointer,$pointer,$q,$pointer),$true),
        @('kernel32.dll','VirtualFree',$b,[Type[]]@($pointer,$q,$u),$true),
        @('kernel32.dll','CreatePipe',$b,[Type[]]@($pointer,$pointer,$pointer,$u),$true),
        @('kernel32.dll','PeekNamedPipe',$b,[Type[]]@($pointer,$pointer,$u,$pointer,$pointer,$pointer),$true),
        @('kernel32.dll','CloseHandle',$b,[Type[]]@($pointer),$true),
        @('kernel32.dll','InitializeProcThreadAttributeList',$b,[Type[]]@($pointer,$u,$u,$pointer),$true),
        @('kernel32.dll','UpdateProcThreadAttribute',$b,[Type[]]@($pointer,$u,$q,$pointer,$q,$pointer,$pointer),$true),
        @('kernel32.dll','DeleteProcThreadAttributeList',$v,[Type[]]@($pointer),$false),
        @('kernel32.dll','CreateProcessW',$b,[Type[]]@($pointer,$pointer,$pointer,$pointer,$b,$u,$pointer,$pointer,$pointer,$pointer),$true),
        @('kernel32.dll','WaitForDebugEvent',$b,[Type[]]@($pointer,$u),$true),
        @('kernel32.dll','ContinueDebugEvent',$b,[Type[]]@($u,$u,$u),$true),
        @('kernel32.dll','GetProcessId',$u,[Type[]]@($pointer),$true),
        @('kernel32.dll','GetThreadId',$u,[Type[]]@($pointer),$true),
        @('kernel32.dll','GetProcessIdOfThread',$u,[Type[]]@($pointer),$true),
        @('kernel32.dll','DuplicateHandle',$b,[Type[]]@($pointer,$pointer,$pointer,$pointer,$u,$b,$u),$true),
        @('kernel32.dll','GetExitCodeProcess',$b,[Type[]]@($pointer,$pointer),$true),
        @('kernel32.dll','GetExitCodeThread',$b,[Type[]]@($pointer,$pointer),$true),
        @('kernel32.dll','GetModuleHandleExW',$b,[Type[]]@($u,$pointer,$pointer),$true),
        @('kernel32.dll','GetModuleFileNameW',$u,[Type[]]@($pointer,$pointer,$u),$true),
        @('kernel32.dll','K32GetMappedFileNameW',$u,[Type[]]@($pointer,$pointer,$pointer,$u),$true),
        @('kernel32.dll','GetThreadContext',$b,[Type[]]@($pointer,$pointer),$true),
        @('ntdll.dll','NtQueryInformationFile',$b,[Type[]]@($pointer,$pointer,$pointer,$u,$u),$false),
        @('ntdll.dll','NtQueryObject',$b,[Type[]]@($pointer,$u,$pointer,$u,$pointer),$false)
    )
    $assemblyName = [Reflection.AssemblyName]::new('ServiceLasso.InitialSeedA.FixedWindows')
    $assembly = [Reflection.Emit.AssemblyBuilder]::DefineDynamicAssembly($assemblyName,[Reflection.Emit.AssemblyBuilderAccess]::Run)
    $emission.Assembly = $assembly
    $module = $assembly.DefineDynamicModule('FixedWindows')
    $emission.Module = $module
    $type = $module.DefineType('ServiceLasso.InitialSeedA.FixedWindows',
        [Reflection.TypeAttributes]::NotPublic -bor [Reflection.TypeAttributes]::Abstract -bor [Reflection.TypeAttributes]::Sealed)
    $emission.TypeBuilder = $type
    $dllCtor = [Runtime.InteropServices.DllImportAttribute].GetConstructor([Type[]]@([string]))
    $fields = [Reflection.FieldInfo[]]@(
        [Runtime.InteropServices.DllImportAttribute].GetField('EntryPoint'),
        [Runtime.InteropServices.DllImportAttribute].GetField('ExactSpelling'),
        [Runtime.InteropServices.DllImportAttribute].GetField('SetLastError'),
        [Runtime.InteropServices.DllImportAttribute].GetField('CallingConvention'),
        [Runtime.InteropServices.DllImportAttribute].GetField('CharSet'),
        [Runtime.InteropServices.DllImportAttribute].GetField('PreserveSig'))
    $cachedError = [Runtime.InteropServices.Marshal].GetMethod('GetLastPInvokeError',[Type[]]@())
    if ($null -eq $dllCtor -or $null -eq $cachedError -or $fields -contains $null) {
        throw 'Selected initial CLR lacks the fixed interop metadata surface.'
    }
    # The complete closed metadata roster is retained; Run is intentionally not
    # collectible. No finalizer may retire a module with live native borrowers.
    $metadata = [Collections.Generic.List[object]]::new($entries.Count)
    $emission.Metadata = $metadata
    foreach ($entry in $entries) {
        $library,$name,$result,$arguments,$lastError = $entry
        $original=[pscustomobject]@{ Name=$name; Library=$library; Result=$result; Arguments=$arguments;
            CachedError=$lastError; Wrapper=$null; Native=$null; ActualMethod=$null; ActualIL=$null }
        $metadata.Add($original)
        $native = $type.DefinePInvokeMethod(('Native_' + $name),$library,$name,
            [Reflection.MethodAttributes]::Private -bor [Reflection.MethodAttributes]::Static -bor [Reflection.MethodAttributes]::PinvokeImpl,
            [Reflection.CallingConventions]::Standard,$result,$arguments,
            [Runtime.InteropServices.CallingConvention]::Winapi,[Runtime.InteropServices.CharSet]::Unicode)
        $original.Native=$native
        $values = [object[]]@($name,$true,$lastError,[Runtime.InteropServices.CallingConvention]::Winapi,
            [Runtime.InteropServices.CharSet]::Unicode,$true)
        $native.SetCustomAttribute([Reflection.Emit.CustomAttributeBuilder]::new($dllCtor,[object[]]@($library),$fields,$values))
        $native.SetImplementationFlags($native.GetMethodImplementationFlags() -bor [Reflection.MethodImplAttributes]::PreserveSig)
        $wrapperArguments = [Type[]]::new($arguments.Length + 1)
        $wrapperArguments[0] = [IntPtr]
        [Array]::Copy($arguments,0,$wrapperArguments,1,$arguments.Length)
        $wrapper = $type.DefineMethod(('Capture_' + $name),
            [Reflection.MethodAttributes]::Assembly -bor [Reflection.MethodAttributes]::Static,
            [Reflection.CallingConventions]::Standard,[void],$wrapperArguments)
        $original.Wrapper=$wrapper
        $wrapper.SetImplementationFlags([Reflection.MethodImplAttributes]::IL -bor [Reflection.MethodImplAttributes]::Managed -bor [Reflection.MethodImplAttributes]::NoInlining)
        $il = $wrapper.GetILGenerator()
        # Native row: called:u32@0, returned:u32@4, raw:i64@8,
        # cached_error:i32@16, cache_observed:u32@20. Remaining source args and
        # clocks are owned by the fixed controller. No buffer erase/reuse here.
        $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0)
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
        $il.Emit([Reflection.Emit.OpCodes]::Stind_I4)
        for ($index=0;$index -lt $arguments.Length;$index++) {
            $il.Emit([Reflection.Emit.OpCodes]::Ldarg,[Int16]($index+1))
        }
        $il.Emit([Reflection.Emit.OpCodes]::Call,$native)
        if ($result -ne [void]) {
            $rawLocal = $il.DeclareLocal($result)
            $il.Emit([Reflection.Emit.OpCodes]::Stloc,$rawLocal)
            $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0)
            $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_8)
            $il.Emit([Reflection.Emit.OpCodes]::Add)
            $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$rawLocal)
            if ($result -eq [UInt32]) { $il.Emit([Reflection.Emit.OpCodes]::Conv_U8) }
            else { $il.Emit([Reflection.Emit.OpCodes]::Conv_I8) }
            $il.Emit([Reflection.Emit.OpCodes]::Stind_I8)
        }
        $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0)
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4)
        $il.Emit([Reflection.Emit.OpCodes]::Add)
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
        $il.Emit([Reflection.Emit.OpCodes]::Stind_I4)
        if ($lastError) {
            $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0)
            $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[Int32]16)
            $il.Emit([Reflection.Emit.OpCodes]::Add)
            $il.Emit([Reflection.Emit.OpCodes]::Call,$cachedError)
            $il.Emit([Reflection.Emit.OpCodes]::Stind_I4)
            $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0)
            $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[Int32]20)
            $il.Emit([Reflection.Emit.OpCodes]::Add)
            $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
            $il.Emit([Reflection.Emit.OpCodes]::Stind_I4)
        }
        $il.Emit([Reflection.Emit.OpCodes]::Ret)
    }
    $actualType = $type.CreateType()
    $emission.Type = $actualType
    $methods = [Collections.Generic.Dictionary[string,Reflection.MethodInfo]]::new($entries.Count,[StringComparer]::Ordinal)
    foreach ($entry in $metadata) {
        $method = $actualType.GetMethod(('Capture_' + $entry.Name),[Reflection.BindingFlags]::NonPublic -bor [Reflection.BindingFlags]::Static)
        if ($null -eq $method -or $methods.ContainsKey($entry.Name)) { throw 'Incomplete fixed native wrapper roster.' }
        $methods.Add($entry.Name,$method)
        $entry.ActualMethod=$method
        # Retain exact emitted IL for whole source/byte comparison before use.
        $entry.ActualIL=$method.GetMethodBody().GetILAsByteArray()
    }
    $emission.Methods = [Collections.ObjectModel.ReadOnlyDictionary[string,Reflection.MethodInfo]]::new($methods)
    $emission.Returned = $true
    return $emission
}
