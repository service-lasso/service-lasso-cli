# Intent-approved bounded creation output. No entropy prediction or issuer.
function New-SlcliInitialWindowsCookieCapture {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    if ($null -ne $Birth.Birth -or $null -ne $Birth.Cookies -or -not $Owner.ModuleSources.Complete) {
        throw 'Cookie backing/case recipe must precede native child birth.'
    }
    $capture=[pscustomobject]@{ Owner=$Owner; Birth=$Birth; Recipe=$Owner.CompilePlan;
        Sources=$Owner.ModuleSources; Rows=[object[]]::new($Owner.ModuleSources.Rows.Length);
        BeforeStarted=$false; AfterStarted=$false; BeforeComplete=$false; Complete=$false;
        BeforeThread=$null; AfterThread=$null; BeforeClock=$null; BeforeFinish=$null; AfterClock=$null; AfterFinish=$null }
    $Birth.Cookies=$capture
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $capture
    for ($index=0;$index -lt $capture.Rows.Length;$index++) {
        $source=$capture.Sources.Rows[$index]
        if (-not $source.Cookie.Complete) { throw 'Cookie source geometry incomplete before backing.' }
        $row=[pscustomobject]@{ Source=$source.Cookie; Module=$null; Case=$null; CaseBacking=$null; Before=$null; After=$null;
            Mapping=$null; Embedded=[UInt64]0; Complete=$false }
        $capture.Rows[$index]=$row
        if (-not $source.Cookie.Present) { continue }
        foreach ($phase in @('Before','After')) {
            $row.$phase=[pscustomobject]@{ Body=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 8 -Alignment 8);
                Control=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 64 -Alignment 8);
                Managed=[byte[]]::new(8); Address=[Int64]0; Query=$null; Read=$null; Received=[Int64]0; Raw=[UInt64]0; Complete=$false }
        }
        $row.CaseBacking=[pscustomobject]@{ Owner=$Owner; Capture=$capture; Birth=$Birth; Event=$null;
            NativeBirth=$null; Child=$null; Recipe=$capture.Recipe; Module=$null;
            Source=$row.Source; Held=$source.Held; Before=$row.Before; After=$row.After; State='Prebirth' }
    }
    return $capture
}

function Assert-SlcliInitialWindowsCreatedCapture {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Capture)
    $birth=$Capture.Birth;$event=$birth.PendingEvent
    if ($Owner.Failed -or -not [Object]::ReferenceEquals($Capture.Owner,$Owner) -or
        -not [Object]::ReferenceEquals($Capture.Recipe,$Owner.CompilePlan) -or
        -not [Object]::ReferenceEquals($Capture.Sources,$Owner.ModuleSources) -or
        -not [Object]::ReferenceEquals($Owner.CompilePlan.Active,$birth) -or
        $null -eq $birth.Birth -or $birth.Birth.Name -cne 'CreateProcessW' -or $birth.Birth.Raw -eq 0 -or
        -not $birth.Child.Complete -or $null -eq $event -or $event.Code -ne 3 -or
        $event.Pid -ne $birth.Pid -or $event.Tid -ne $birth.Tid -or $null -ne $event.Continue -or
        -not [Object]::ReferenceEquals($event,$birth.Child.Event) -or -not $birth.Modules.Complete) {
        throw 'Cookie case is not SAME fixed-created first stopped pre-user child.'
    }
    $native=$birth.Birth
    if ($native.Ordinal -lt 0 -or $native.Ordinal -ge $Owner.RowsUsed -or
        -not [Object]::ReferenceEquals($Owner.Rows[$native.Ordinal],$native) -or
        $event.Wait.Ordinal -lt 0 -or $event.Wait.Ordinal -ge $Owner.RowsUsed -or
        -not [Object]::ReferenceEquals($Owner.Rows[$event.Wait.Ordinal],$event.Wait) -or
        $event.Wait.Name -cne 'WaitForDebugEvent' -or -not $event.Wait.Returned -or
        $event.Wait.Called -ne 1 -or $event.Wait.NativeReturned -ne 1 -or $event.Wait.Raw -eq 0 -or
        -not $native.Returned -or $native.Called -ne 1 -or $native.NativeReturned -ne 1 -or
        $native.Arguments.Length -ne 10 -or $native.Arguments[5] -ne [UInt32]0x08080401 -or
        $native.Arguments[0] -ne $birth.Exe.Native.Pointer -or $native.Arguments[1] -ne $birth.Command.Native.Pointer -or
        $native.Arguments[6] -ne $birth.Environment.Native.Pointer -or $native.Arguments[7] -ne $birth.Cwd.Native.Pointer -or
        $native.Arguments[8] -ne $birth.Startup.Pointer -or $native.Arguments[9] -ne $birth.Process.Pointer -or
        $birth.Exe.Text -cne $Owner.Compiler.Source.Path) { throw 'Cookie case native birth operands/returned originals differ from fixed source recipe.' }
    foreach ($prior in $birth.Events) {
        if ($null -ne $prior -and ($null -ne $prior.Continue -or
            ($prior.Wait.Raw -ne 0 -and -not [Object]::ReferenceEquals($prior,$event)))) {
            throw 'Cookie case has prior continuation/native event or attach/replayed phase.'
        }
    }
}

function New-SlcliInitialWindowsCookieCase {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Capture,[Parameter(Mandatory)][object]$Row)
    Assert-SlcliInitialWindowsCreatedCapture -Owner $Owner -Capture $Capture
    $birth=$Capture.Birth;$event=$birth.PendingEvent
    $found=$false
    foreach ($owned in $Capture.Rows) { if ([Object]::ReferenceEquals($owned,$Row)) { $found=$true;break } }
    if (-not $found -or $null -ne $Row.Case -or $null -eq $Row.CaseBacking -or
        $Row.CaseBacking.State -cne 'Prebirth' -or -not $Row.Source.Present -or -not $Row.Source.Complete) {
        throw 'Cookie case does not consume fresh SAME prebirth source/output backing.'
    }
    $Row.CaseBacking.State='Constructing'
    for ($index=0;$index -lt $birth.Modules.ImagesUsed;$index++) {
        $module=$birth.Modules.Images[$index]
        if ([Object]::ReferenceEquals($module.Source,$Row.Source.Source)) {
            if ($null -ne $Row.Module) { throw 'Cookie initial module/source occurrence is ambiguous.' }
            $Row.Module=$module
        }
    }
    if ($null -eq $Row.Module) { $Row.CaseBacking.State='NotMapped';return $null } # No output case.
    # Actual initial source/OS creation case and ALL product-actor exclusion
    # remain in the independently admitted initial occurrence. This constructor
    # joins its fixed call/event/held/actual-map originals; equality cannot mint
    # that admission. No caller case/boolean or default current-user is consumed.
    $case=$Row.CaseBacking
    $case.Event=$event;$case.NativeBirth=$birth.Birth;$case.Child=$birth.Child;$case.Module=$Row.Module
    $case.State='Constructed'
    $Row.Case=$case
    return $case
}

function Read-SlcliInitialWindowsCookieWord {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Capture,
        [Parameter(Mandatory)][object]$Row,[Parameter(Mandatory)][object]$Record)
    if ($null -eq $Row.Case -or (-not [Object]::ReferenceEquals($Record,$Row.Before) -and
        -not [Object]::ReferenceEquals($Record,$Row.After)) -or $null -ne $Record.Query -or $null -ne $Record.Read -or
        -not [Object]::ReferenceEquals($Capture.Birth.PendingEvent,$Row.Case.Event) -or $null -ne $Row.Case.Event.Continue) {
        throw 'Cookie native read lost SAME fresh fixed creation-output slice.'
    }
    $Record.Address=$Row.Module.Base+$Row.Source.Rva
    if ($Record.Address -lt $Owner.InitialGeometry.Minimum -or $Record.Address -gt $Owner.InitialGeometry.Maximum-7) {
        throw 'Cookie native word outside original user geometry.'
    }
    $at=$Record.Control.Pointer
    $Record.Query=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'VirtualQueryEx' -Arguments @(
        $Capture.Birth.ProcessReference.Handle,[IntPtr]::new($Record.Address),$at,[UIntPtr]::new(48))
    $region=[Runtime.InteropServices.Marshal]::ReadIntPtr($at).ToInt64()
    $bytes=[Runtime.InteropServices.Marshal]::ReadInt64($at,24)
    $protection=[Runtime.InteropServices.Marshal]::ReadInt32($at,36)
    if ($Record.Query.Raw -ne 48 -or $region -le 0 -or $region -gt $Record.Address -or
        $bytes -le 0 -or $bytes -gt $Owner.InitialGeometry.Maximum+1-$region -or $Record.Address-$region -gt $bytes-8 -or
        [Runtime.InteropServices.Marshal]::ReadIntPtr($at,8).ToInt64() -ne $Row.Module.Base -or
        [Runtime.InteropServices.Marshal]::ReadInt32($at,32) -ne 0x1000 -or
        [Runtime.InteropServices.Marshal]::ReadInt32($at,40) -ne 0x1000000 -or
        ($protection -band 0x100) -ne 0 -or ($protection -band 0xff) -notin @(2,4,8,32,64,128)) {
        throw 'Cookie output native mapping differs/guarded/unreadable/incomplete.'
    }
    $Record.Read=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'ReadProcessMemory' -Arguments @(
        $Capture.Birth.ProcessReference.Handle,[IntPtr]::new($Record.Address),$Record.Body.Pointer,[UIntPtr]::new(8),[IntPtr]::Add($at,48))
    $Record.Received=[Runtime.InteropServices.Marshal]::ReadInt64($at,48)
    [Runtime.InteropServices.Marshal]::Copy($Record.Body.Pointer,$Record.Managed,0,8)
    $Record.Raw=[BitConverter]::ToUInt64($Record.Managed,0)
    if ($Record.Read.Raw -eq 0 -or $Record.Received -ne 8) { throw 'Cookie output native read failed/short; raw prefix retained.' }
    $Record.Complete=$true
}

function Read-SlcliInitialWindowsCookieBefore {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $capture=$Birth.Cookies
    if ($null -eq $capture -or $capture.BeforeStarted -or $Birth.Modules.ImagesCaptured) { throw 'Cookie first capture is not before full image reads.' }
    $capture.BeforeStarted=$true;$capture.BeforeClock=Read-SlcliInitialWindowsClock -Owner $Owner
    $capture.BeforeThread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($capture.BeforeThread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'Cookie capture left actual native creator thread.' }
    foreach ($row in $capture.Rows) {
        if (-not $row.Source.Present) { continue }
        $case=New-SlcliInitialWindowsCookieCase -Owner $Owner -Capture $capture -Row $row
        if ($null -ne $case) { Read-SlcliInitialWindowsCookieWord -Owner $Owner -Capture $capture -Row $row -Record $row.Before }
    }
    $capture.BeforeFinish=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($capture.BeforeFinish.Tick -lt $capture.BeforeClock.Tick) { throw 'Cookie first processing clock regressed.' }
    $Owner.PriorTick=$capture.BeforeFinish.Tick;$capture.BeforeComplete=$true
}

function Read-SlcliInitialWindowsCookieAfter {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $capture=$Birth.Cookies
    if ($null -eq $capture -or -not $capture.BeforeComplete -or $capture.AfterStarted -or
        -not $Birth.Modules.ImagesCaptured -or -not $Birth.Modules.RouteBytesObserved) { throw 'Cookie final capture lost complete prior source/image phase.' }
    $capture.AfterStarted=$true;$capture.AfterClock=Read-SlcliInitialWindowsClock -Owner $Owner
    $capture.AfterThread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($capture.AfterThread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'Cookie final capture left actual native creator thread.' }
    foreach ($row in $capture.Rows) {
        if ($null -eq $row.Case) { continue }
        Read-SlcliInitialWindowsCookieWord -Owner $Owner -Capture $capture -Row $row -Record $row.After
        $row.Mapping=$row.Module.Mapping
        if (-not $row.Mapping.CapturedComplete -or -not $row.Mapping.ExpectedComplete -or $null -ne $row.Mapping.CookieOutput -or
            -not [Object]::ReferenceEquals($row.Mapping.Held,$row.Case.Held)) { throw 'Cookie output lost SAME source/captured mapping.' }
        $row.Embedded=[BitConverter]::ToUInt64($row.Mapping.Observed,$row.Source.Rva)
        if ($row.Before.Raw -ne $row.After.Raw -or $row.Embedded -ne $row.Before.Raw) { throw 'Cookie native output changed across stopped full image capture.' }
        for ($offset=0;$offset -lt 8;$offset++) {
            $bit=$row.Source.Rva+$offset;$cell=[Int32][Math]::Floor($bit/8);$mask=1 -shl ($bit%8)
            if (($row.Mapping.RelocationSeen[$cell] -band $mask) -ne 0 -or ($row.Mapping.BindingSeen[$cell] -band $mask) -ne 0) {
                throw 'Cookie output overlaps relocation/import effect.'
            }
        }
        $pointer=[BitConverter]::ToUInt64($row.Mapping.Observed,$row.Source.DirectoryRva+88)
        if ($pointer -ne [UInt64]($row.Module.Base+$row.Source.Rva)) { throw 'Actual load-config pointer differs from source-derived cookie slot.' }
        $row.Complete=$true;$row.Mapping.CookieOutput=$row
    }
    $capture.AfterFinish=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($capture.AfterFinish.Tick -lt $capture.AfterClock.Tick) { throw 'Cookie final processing clock regressed.' }
    $Owner.PriorTick=$capture.AfterFinish.Tick;$capture.Complete=$true
}

function Assert-SlcliInitialWindowsCookieOutput {
    param([Parameter(Mandatory)][object]$Mapping)
    $row=$Mapping.CookieOutput
    if ($null -eq $row) { throw 'Missing exact cookie output relation.' }
    $case=$row.Case
    if ($null -eq $case -or $case.State -cne 'Constructed' -or -not $row.Complete -or -not $case.Capture.Complete -or
        $row.Source.Rva -lt 0 -or $row.Source.Rva -gt $Mapping.Observed.Length-8 -or
        -not [Object]::ReferenceEquals($row.Mapping,$Mapping) -or
        -not [Object]::ReferenceEquals($case.Module.Mapping,$Mapping) -or
        -not [Object]::ReferenceEquals($case.Held,$Mapping.Held) -or
        -not [Object]::ReferenceEquals($case.Birth.PendingEvent,$case.Event) -or $null -ne $case.Event.Continue -or
        $case.Event.Code -ne 3 -or $case.NativeBirth.Name -cne 'CreateProcessW' -or $case.NativeBirth.Raw -eq 0 -or
        -not $row.Before.Complete -or -not $row.After.Complete -or $row.Before.Raw -ne $row.After.Raw -or
        $row.Embedded -ne $row.Before.Raw -or [BitConverter]::ToUInt64($Mapping.Observed,$row.Source.Rva) -ne $row.Embedded) {
        throw 'Exact cookie span has no SAME completed native creation-output relation.'
    }
    if (-not [Object]::ReferenceEquals($row.Source,$case.Source) -or
        -not [Object]::ReferenceEquals($row.Before,$case.Before) -or -not [Object]::ReferenceEquals($row.After,$case.After) -or
        -not [Object]::ReferenceEquals($case.Birth.Birth,$case.NativeBirth) -or
        -not [Object]::ReferenceEquals($case.Owner.Rows[$case.NativeBirth.Ordinal],$case.NativeBirth) -or
        -not [Object]::ReferenceEquals($case.Owner.Rows[$case.Event.Wait.Ordinal],$case.Event.Wait) -or
        $case.Capture.BeforeThread.Raw -ne $case.Owner.Primary.ThreadId.Raw -or
        $case.Capture.AfterThread.Raw -ne $case.Owner.Primary.ThreadId.Raw) {
        throw 'Cookie span lost actual native birth/event/source/output/issuing-thread originals.'
    }
}
