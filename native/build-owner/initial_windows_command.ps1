# Fixed initial A seed compilation only. No supplied argv/executable callback.
function New-SlcliInitialWindowsCompilePlan {
    param([Parameter(Mandatory)][object]$Owner)
    $names=[string[]]@('build-owner/bootstrap_windows.c','build-owner/principal_windows.c',
        'build-owner/held_file_windows.c','build-owner/command_windows.c','build-owner/child_principal_windows.c',
        'build-owner/loaded_file_windows.c','build-owner/remote_image_windows.c','build-owner/node_abi_windows.c',
        'build-owner/node_loaded_windows.c','build-owner/seed_compile_windows.c','build-owner/streams_windows.c',
        'host-service/arena.c','host-service/owner_memory.c','host-service/platform_memory_windows.c',
        'host-service/native_clock.c','host-service/file_windows.c','host-service/crypto.c','build-owner/addon_windows.c')
    if ($null -eq $Owner.Compiler -or -not $Owner.Compiler.Complete -or
        -not (Test-SlcliInitialWindowsCanonicalPath -Path $Owner.Source.NativeRoot) -or
        -not (Test-SlcliInitialWindowsCanonicalPath -Path $Owner.Source.OutputDirectory) -or
        $Owner.Source.SeedMembers.Count -ne $names.Length -or $Owner.Source.IncludeDirectories.Count -ne 5 -or
        $null -ne $Owner.CompilePlan) { throw 'Incomplete fixed initial seed source/include roster.' }
    $plan=[pscustomobject]@{ Names=$names; Units=[object[]]::new($names.Length); Next=0; Active=$null; NaturalClosed=0 }
    $Owner.CompilePlan=$plan
    for ($index=0;$index -lt $names.Length;$index++) {
        $member=$Owner.Source.SeedMembers[$index]
        $expected=$Owner.Source.NativeRoot+'\'+$names[$index].Replace('/','\')
        if ($member.Path -cne $expected) { throw 'Changed fixed seed compilation order/source path.' }
        $output=$Owner.Source.OutputDirectory+'\'+$index.ToString('D2',[Globalization.CultureInfo]::InvariantCulture)+'-'+[IO.Path]::GetFileNameWithoutExtension($names[$index])+'.obj'
        $plan.Units[$index]=[pscustomobject]@{ Member=$member; Output=$output; Held=$null; Birth=$null; Complete=$false }
        $plan.Units[$index].Held=Read-SlcliInitialWindowsHeldFile -Owner $Owner -Member $member
    }
    return $plan
}

function New-SlcliInitialWindowsPipe {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][bool]$PrivateReader)
    $buffer=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 48 -Alignment 8
    # SECURITY_ATTRIBUTES24 + HANDLE[2] + DWORD flags[2], all fresh/disjoint.
    [Runtime.InteropServices.Marshal]::WriteInt32($buffer.Pointer,24)
    [Runtime.InteropServices.Marshal]::WriteInt32($buffer.Pointer,16,1)
    $read=[IntPtr]::Add($buffer.Pointer,24);$write=[IntPtr]::Add($buffer.Pointer,32)
    $original=[pscustomobject]@{ Buffer=$buffer; Create=$null; Reader=[IntPtr]::Zero; Writer=[IntPtr]::Zero;
        ReaderReference=$null; WriterReference=$null; Private=$null; ReadFlags=$null; WriteFlags=$null }
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    $original.Create=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'CreatePipe' -Arguments @($read,$write,$buffer.Pointer,[UInt32]$Owner.Source.PipeBytes)
    $original.Reader=[Runtime.InteropServices.Marshal]::ReadIntPtr($read)
    $original.Writer=[Runtime.InteropServices.Marshal]::ReadIntPtr($write)
    $original.ReaderReference=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $original.Create -Handle $original.Reader -Role 'PipeReader'
    $original.WriterReference=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $original.Create -Handle $original.Writer -Role 'PipeWriter'
    if ($original.Create.Raw -eq 0 -or $original.Reader.ToInt64() -in @(0,-1) -or $original.Writer.ToInt64() -in @(0,-1) -or
        $original.Reader -eq $original.Writer) { throw 'Initial native pipe acquisition failed.' }
    if ($PrivateReader) {
        $original.Private=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'SetHandleInformation' -Arguments @($original.Reader,[UInt32]1,[UInt32]0)
        if ($original.Private.Raw -eq 0) { throw 'Initial parent stream reader inheritance not removed.' }
    }
    $original.ReadFlags=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetHandleInformation' -Arguments @($original.Reader,[IntPtr]::Add($buffer.Pointer,40))
    $original.WriteFlags=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetHandleInformation' -Arguments @($original.Writer,[IntPtr]::Add($buffer.Pointer,44))
    $rf=[Runtime.InteropServices.Marshal]::ReadInt32($buffer.Pointer,40)
    $wf=[Runtime.InteropServices.Marshal]::ReadInt32($buffer.Pointer,44)
    if ($original.ReadFlags.Raw -eq 0 -or $original.WriteFlags.Raw -eq 0 -or
        (($rf -band 1) -ne 0) -eq $PrivateReader -or ($wf -band 1) -eq 0) { throw 'Initial pipe native flags differ.' }
    return $original
}

function Start-SlcliInitialWindowsFixedCompile {
    param([Parameter(Mandatory)][object]$Owner)
    $plan=$Owner.CompilePlan
    if ($Owner.Failed -or $null -eq $plan -or $null -ne $plan.Active -or $plan.Next -ge $plan.Units.Length -or
        $plan.NaturalClosed -ne $plan.Next -or -not $Owner.Compiler.Complete -or
        -not $Owner.Primary.Complete -or -not [Object]::ReferenceEquals($Owner.Compiler.Principal,$Owner.Effective) -or
        -not [Object]::ReferenceEquals($Owner.Primary.IssuingEffective,$Owner.Effective)) { throw 'Initial fixed compiler source/principal/order mismatch.' }
    $unit=$plan.Units[$plan.Next]
    if (-not $unit.Held.Complete -or $unit.Member.Path.Contains('"') -or $unit.Output.Contains('"') -or
        $Owner.Compiler.Source.Path.Contains('"') -or $Owner.Source.EventCapacity -isnot [Int32] -or
        $Owner.Source.EventCapacity -le 0 -or $Owner.Source.EventCapacity -gt [Int32]::MaxValue/176 -or
        $Owner.Source.AttributeBytes -isnot [Int32] -or $Owner.Source.AttributeBytes -le 0 -or
        $Owner.Source.WaitQuantumMilliseconds -isnot [Int32] -or $Owner.Source.WaitQuantumMilliseconds -le 0 -or
        $Owner.Source.WaitQuantumMilliseconds -gt 60000) { throw 'Invalid fixed initial native command geometry.' }
    $command='"'+$Owner.Compiler.Source.Path+'" /nologo /TC /std:c11 /O2 /GS /MT /W4 /DBUILDING_NODE_EXTENSION /DNAPI_VERSION=8 /c "'+$unit.Member.Path+'" /Fo"'+$unit.Output+'"'
    foreach ($include in $Owner.Source.IncludeDirectories) {
        if ($include -isnot [string] -or -not (Test-SlcliInitialWindowsCanonicalPath -Path $include)) { throw 'Invalid fixed initial include path.' }
        $command+=' /I"'+$include+'"'
    }
    $text=Get-SlcliInitialWindowsText -Owner $Owner -Text $command
    $exe=Get-SlcliInitialWindowsText -Owner $Owner -Text $Owner.Compiler.Source.Path
    $cwd=Get-SlcliInitialWindowsText -Owner $Owner -Text $Owner.Source.OutputDirectory
    $startup=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 112 -Alignment 8
    $process=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 24 -Alignment 8
    $attributes=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes $Owner.Source.AttributeBytes -Alignment 8
    $attributeSize=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 8 -Alignment 8
    $handles=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 24 -Alignment 8
    $events=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ($Owner.Source.EventCapacity*176) -Alignment 8
    $original=[pscustomobject]@{ Unit=$unit; Command=$text; Exe=$exe; Cwd=$cwd; Startup=$startup; Process=$process;
        Attributes=$attributes; AttributeSize=$attributeSize; Handles=$handles; EventBody=$events;
        Events=[object[]]::new($Owner.Source.EventCapacity); EventsUsed=0; PendingEvent=$null;
        Stdin=$null; Stdout=$null; Stderr=$null; StdinWriterClose=$null; AttributeProbe=$null; AttributeInit=$null;
        AttributeHandles=$null; IssuingThread=$null; Birth=$null; ProcessReference=$null; ThreadReference=$null;
        Environment=$Owner.Environment; Streams=$null; Child=$null; EventImage=$null; RemoteImage=$null; Modules=$null;
        Pid=[UInt32]0; Tid=[UInt32]0; NaturalExit=$null; Complete=$false }
    $unit.Birth=$original;$plan.Active=$original
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    $null=New-SlcliInitialWindowsStreams -Owner $Owner -Birth $original
    $null=New-SlcliInitialWindowsChild -Owner $Owner -Birth $original
    $null=New-SlcliInitialWindowsEventFile -Owner $Owner -Birth $original
    $null=New-SlcliInitialWindowsRemoteImage -Owner $Owner -Birth $original
    $null=New-SlcliInitialWindowsModuleWalk -Owner $Owner -Birth $original
    $original.Stdin=New-SlcliInitialWindowsPipe -Owner $Owner -PrivateReader $false
    $original.Stdout=New-SlcliInitialWindowsPipe -Owner $Owner -PrivateReader $true
    $original.Stderr=New-SlcliInitialWindowsPipe -Owner $Owner -PrivateReader $true
    $original.StdinWriterClose=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'CloseHandle' -Arguments @($original.Stdin.Writer)
    $original.Stdin.WriterReference.Close=$original.StdinWriterClose
    if ($original.StdinWriterClose.Raw -eq 0) { throw 'Initial empty-stdin writer close failed.' }
    $original.AttributeProbe=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'InitializeProcThreadAttributeList' -Arguments @([IntPtr]::Zero,[UInt32]1,[UInt32]0,$attributeSize.Pointer)
    $actualBytes=[Runtime.InteropServices.Marshal]::ReadInt64($attributeSize.Pointer)
    if ($original.AttributeProbe.Raw -ne 0 -or $original.AttributeProbe.CacheObserved -ne 1 -or $original.AttributeProbe.CachedError -ne 122 -or
        $actualBytes -le 0 -or $actualBytes -gt $attributes.Bytes) { throw 'Initial native attribute-size probe differs.' }
    $original.AttributeInit=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'InitializeProcThreadAttributeList' -Arguments @($attributes.Pointer,[UInt32]1,[UInt32]0,$attributeSize.Pointer)
    if ($original.AttributeInit.Raw -eq 0) { throw 'Initial native attribute initialization failed.' }
    [Runtime.InteropServices.Marshal]::WriteIntPtr($handles.Pointer,$original.Stdin.Reader)
    [Runtime.InteropServices.Marshal]::WriteIntPtr($handles.Pointer,8,$original.Stdout.Writer)
    [Runtime.InteropServices.Marshal]::WriteIntPtr($handles.Pointer,16,$original.Stderr.Writer)
    $original.AttributeHandles=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'UpdateProcThreadAttribute' -Arguments @(
        $attributes.Pointer,[UInt32]0,[UIntPtr]::new(0x20002),$handles.Pointer,[UIntPtr]::new(24),[IntPtr]::Zero,[IntPtr]::Zero)
    if ($original.AttributeHandles.Raw -eq 0) { throw 'Initial exact three-handle inheritance failed.' }
    [Runtime.InteropServices.Marshal]::WriteInt32($startup.Pointer,112)
    [Runtime.InteropServices.Marshal]::WriteInt32($startup.Pointer,60,0x101)
    [Runtime.InteropServices.Marshal]::WriteIntPtr($startup.Pointer,80,$original.Stdin.Reader)
    [Runtime.InteropServices.Marshal]::WriteIntPtr($startup.Pointer,88,$original.Stdout.Writer)
    [Runtime.InteropServices.Marshal]::WriteIntPtr($startup.Pointer,96,$original.Stderr.Writer)
    [Runtime.InteropServices.Marshal]::WriteIntPtr($startup.Pointer,104,$attributes.Pointer)
    $original.IssuingThread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($original.IssuingThread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'Initial compiler/debug owner thread changed.' }
    if ($null -eq $Owner.Environment) { throw 'Initial explicit fixed environment original absent.' }
    $original.Birth=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'CreateProcessW' -Arguments @(
        $exe.Native.Pointer,$text.Native.Pointer,[IntPtr]::Zero,[IntPtr]::Zero,[Int32]1,[UInt32]0x08080401,
        $Owner.Environment.Native.Pointer,$cwd.Native.Pointer,$startup.Pointer,$process.Pointer)
    $ph=[Runtime.InteropServices.Marshal]::ReadIntPtr($process.Pointer)
    $th=[Runtime.InteropServices.Marshal]::ReadIntPtr($process.Pointer,8)
    $original.ProcessReference=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $original.Birth -Handle $ph -Role 'ReturnedProcess'
    $original.ThreadReference=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $original.Birth -Handle $th -Role 'ReturnedThread'
    $original.Pid=[UInt32]([Int64][Runtime.InteropServices.Marshal]::ReadInt32($process.Pointer,16) -band [Int64]0xffffffff)
    $original.Tid=[UInt32]([Int64][Runtime.InteropServices.Marshal]::ReadInt32($process.Pointer,20) -band [Int64]0xffffffff)
    if ($original.Birth.Raw -eq 0 -or $ph.ToInt64() -in @(0,-1) -or $th.ToInt64() -in @(0,-1) -or
        $original.Pid -eq 0 -or $original.Tid -eq 0) { $Owner.Pending=$true;throw 'Initial native compiler birth failed/unresolved.' }
    Start-SlcliInitialWindowsStreams -Owner $Owner -Birth $original
    # First real event remains STOPPED until full native image/principal/source
    # joins; no callback or always-continue path is supplied by this body.
    $null=Wait-SlcliInitialWindowsEvent -Owner $Owner -Birth $original
    if ($original.PendingEvent.Code -ne 3 -or $original.PendingEvent.Pid -ne $original.Pid -or
        $original.PendingEvent.Tid -ne $original.Tid) { throw 'Initial compiler first native event mismatch.' }
    $null=Read-SlcliInitialWindowsChild -Owner $Owner -Birth $original
    $null=Read-SlcliInitialWindowsModules -Owner $Owner -Birth $original
    $null=Read-SlcliInitialWindowsEventFile -Owner $Owner -Birth $original
    $null=Read-SlcliInitialWindowsRemoteImage -Owner $Owner -Birth $original
    return $original
}

function Wait-SlcliInitialWindowsEvent {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    if ($null -ne $Birth.PendingEvent) { throw 'Initial native event still pending.' }
    while ($Birth.EventsUsed -lt $Birth.Events.Length) {
        if (-not $Birth.Streams.Complete) { $null=Read-SlcliInitialWindowsStreams -Owner $Owner -Birth $Birth }
        $thread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
        if ($thread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'Initial debug wait left its creator thread.' }
        $remaining=$Owner.Expires-$Owner.PriorTick
        $whole=[Numerics.BigInteger]::Divide(([Numerics.BigInteger]$remaining+999999),[Numerics.BigInteger]1000000)
        $milliseconds=[UInt32][Math]::Min([Int64]$whole,[Int64]$Owner.Source.WaitQuantumMilliseconds)
        if ($milliseconds -eq 0) { throw 'Initial debug wait has no original deadline remaining.' }
        $pointer=[IntPtr]::Add($Birth.EventBody.Pointer,$Birth.EventsUsed*176)
        $event=[pscustomobject]@{ Thread=$thread; Pointer=$pointer; Wait=$null; Code=0; Pid=0; Tid=0; Continue=$null; TerminalAliasesReleased=$false }
        $Birth.Events[$Birth.EventsUsed]=$event;$Birth.EventsUsed++
        $event.Wait=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'WaitForDebugEvent' -Arguments @($pointer,$milliseconds)
        if ($event.Wait.Raw -eq 0) {
            if ($event.Wait.CacheObserved -ne 1 -or $event.Wait.CachedError -ne 121) { $Owner.Pending=$true;throw 'Initial native debug wait unresolved.' }
            continue
        }
        $event.Code=[Runtime.InteropServices.Marshal]::ReadInt32($pointer)
        $event.Pid=[UInt32]([Int64][Runtime.InteropServices.Marshal]::ReadInt32($pointer,4) -band [Int64]0xffffffff)
        $event.Tid=[UInt32]([Int64][Runtime.InteropServices.Marshal]::ReadInt32($pointer,8) -band [Int64]0xffffffff)
        $Birth.PendingEvent=$event
        return $event
    }
    $Owner.Pending=$true;throw 'Initial full native event roster exhausted.'
}
