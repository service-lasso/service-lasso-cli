# Fixed stopped-root child identity under the SAME A owner. This is actual
# CreateProcessW correspondence, not a child-local source/ROOT issuer.
function Test-SlcliInitialWindowsChildParentIdentity {
    param([UInt32]$ExpectedChild,[UInt32]$ExpectedParent,[Int64]$ActualChild,[Int64]$ActualParent)
    return ($ExpectedChild -ne 0 -and $ExpectedParent -ne 0 -and
        $ActualChild -gt 0 -and $ActualParent -gt 0 -and $ActualChild -le [UInt32]::MaxValue -and
        $ActualParent -le [UInt32]::MaxValue -and $ActualChild -eq $ExpectedChild -and $ActualParent -eq $ExpectedParent)
}

function New-SlcliInitialWindowsChild {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    if ($null -ne $Birth.Birth -or $null -ne $Birth.Child -or -not $Owner.Primary.Complete) {
        throw 'Initial child identity backing must precede birth.'
    }
    $child=[pscustomobject]@{ Birth=$Birth; Primary=$Owner.Primary; Event=$null;
        EventFile=$null; EventProcess=$null; EventThread=$null;
        ProcessIds=[object[]]::new(2); ThreadIds=[object[]]::new(2); ThreadProcessIds=[object[]]::new(2);
        ProcessTimes=[object[]]::new(2); Times=$null; Basic=$null; BasicBytes=$null; BasicCall=$null;
        TokenBuffer=$null; Token=[IntPtr]::Zero;
        TokenOpen=$null; TokenReference=$null; User=$null; Statistics=$null; Recheck=$null; Lengths=$null;
        UserCall=$null; StatisticsCall=$null; RecheckCall=$null; SidValid=$null; SidLength=$null;
        Sid=[byte[]]::new($Owner.Primary.Sid.Length); Complete=$false }
    $Birth.Child=$child
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $child
    $child.Times=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 64 -Alignment 8
    $child.Basic=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 48 -Alignment 8
    $child.BasicBytes=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 4 -Alignment 4
    $child.TokenBuffer=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 8 -Alignment 8
    $child.User=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 88 -Alignment 8
    $child.Statistics=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 56 -Alignment 8
    $child.Recheck=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 56 -Alignment 8
    $child.Lengths=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 12 -Alignment 4
    return $child
}

function Read-SlcliInitialWindowsChild {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $child=$Birth.Child;$event=$Birth.PendingEvent
    if ($null -eq $child -or $child.Complete -or $null -ne $child.Event -or
        $null -eq $event -or $event.Code -ne 3 -or $event.Pid -ne $Birth.Pid -or $event.Tid -ne $Birth.Tid -or
        $null -ne $event.Continue -or -not [Object]::ReferenceEquals($child.Primary,$Owner.Primary)) {
        throw 'Initial stopped root/principal original changed.'
    }
    $child.Event=$event
    # SDK x64 DEBUG_EVENT union begins at16. CREATE_PROCESS_DEBUG_INFO has
    # hFile/hProcess/hThread at0/8/16. Retain all actual event references first;
    # a same numeric handle is never evidence of independent/closed ownership.
    $child.EventFile=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $event.Wait -Handle (
        [Runtime.InteropServices.Marshal]::ReadIntPtr($event.Pointer,16)) -Role 'DebugImageFile'
    $child.EventProcess=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $event.Wait -Handle (
        [Runtime.InteropServices.Marshal]::ReadIntPtr($event.Pointer,24)) -Role 'DebugProcess'
    $child.EventThread=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $event.Wait -Handle (
        [Runtime.InteropServices.Marshal]::ReadIntPtr($event.Pointer,32)) -Role 'DebugThread'
    foreach ($reference in @($child.EventFile,$child.EventProcess,$child.EventThread)) {
        if ($reference.Handle.ToInt64() -in @(0,-1)) { throw 'Initial native debug reference absent/invalid.' }
    }
    $processes=@($Birth.ProcessReference,$child.EventProcess)
    $threads=@($Birth.ThreadReference,$child.EventThread)
    for ($index=0;$index -lt 2;$index++) {
        $child.ProcessIds[$index]=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetProcessId' -Arguments @($processes[$index].Handle)
        $child.ThreadIds[$index]=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetThreadId' -Arguments @($threads[$index].Handle)
        $child.ThreadProcessIds[$index]=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetProcessIdOfThread' -Arguments @($threads[$index].Handle)
        if ($child.ProcessIds[$index].Raw -ne $Birth.Pid -or $child.ThreadIds[$index].Raw -ne $Birth.Tid -or
            $child.ThreadProcessIds[$index].Raw -ne $Birth.Pid) { throw 'Initial native returned/event identity mismatch.' }
        $at=[IntPtr]::Add($child.Times.Pointer,$index*32)
        $child.ProcessTimes[$index]=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetProcessTimes' -Arguments @(
            $processes[$index].Handle,$at,[IntPtr]::Add($at,8),[IntPtr]::Add($at,16),[IntPtr]::Add($at,24))
        if ($child.ProcessTimes[$index].Raw -eq 0) { throw 'Initial actual child process lifetime unavailable.' }
    }
    if ([Runtime.InteropServices.Marshal]::ReadInt64($child.Times.Pointer) -ne
        [Runtime.InteropServices.Marshal]::ReadInt64($child.Times.Pointer,32)) { throw 'Initial child creation lifetime differs.' }
    $child.BasicCall=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'NtQueryInformationProcess' -Arguments @(
        $Birth.ProcessReference.Handle,[UInt32]0,$child.Basic.Pointer,[UInt32]48,$child.BasicBytes.Pointer)
    if ($child.BasicCall.Raw -ne 0 -or [Runtime.InteropServices.Marshal]::ReadInt32($child.BasicBytes.Pointer) -ne 48 -or
        -not (Test-SlcliInitialWindowsChildParentIdentity -ExpectedChild $Birth.Pid -ExpectedParent ([UInt32]$Owner.Primary.ProcessId.Raw) -ActualChild (
            [Runtime.InteropServices.Marshal]::ReadIntPtr($child.Basic.Pointer,32).ToInt64()) -ActualParent (
            [Runtime.InteropServices.Marshal]::ReadIntPtr($child.Basic.Pointer,40).ToInt64()))) {
        throw 'Initial actual native child/issuing-parent process relation differs.'
    }
    $child.TokenOpen=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'OpenProcessToken' -Arguments @(
        $Birth.ProcessReference.Handle,[UInt32]8,$child.TokenBuffer.Pointer)
    $child.Token=[Runtime.InteropServices.Marshal]::ReadIntPtr($child.TokenBuffer.Pointer)
    $child.TokenReference=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $child.TokenOpen -Handle $child.Token -Role 'ActualChildPrimaryToken'
    if ($child.TokenOpen.Raw -eq 0 -or $child.Token.ToInt64() -in @(0,-1)) { throw 'Initial actual child primary token unavailable.' }
    $child.UserCall=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetTokenInformation' -Arguments @(
        $child.Token,[UInt32]1,$child.User.Pointer,[UInt32]88,$child.Lengths.Pointer)
    $length=[Runtime.InteropServices.Marshal]::ReadInt32($child.Lengths.Pointer)
    $sid=[Runtime.InteropServices.Marshal]::ReadIntPtr($child.User.Pointer)
    [Int64]$offset=$sid.ToInt64()-$child.User.Pointer.ToInt64()
    if ($child.UserCall.Raw -eq 0 -or $length -lt 16 -or $length -gt 88 -or
        $offset -lt 16 -or $offset -gt $length-8 -or ($sid.ToInt64() -band 3) -ne 0) { throw 'Initial child SID pointer/extent invalid.' }
    $sidBytes=8+4*[Int32][Runtime.InteropServices.Marshal]::ReadByte($sid,1)
    if ($sidBytes -gt 68 -or $sidBytes -gt $length-$offset -or $sidBytes -ne $Owner.Primary.Sid.Length) {
        throw 'Initial child SID differs from selected primary extent.'
    }
    $child.SidValid=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'IsValidSid' -Arguments @($sid)
    if ($child.SidValid.Raw -eq 0) { throw 'Initial child native SID invalid.' }
    $child.SidLength=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetLengthSid' -Arguments @($sid)
    if ($child.SidLength.Raw -ne $sidBytes) { throw 'Initial child native SID length changed.' }
    [Runtime.InteropServices.Marshal]::Copy($sid,$child.Sid,0,$sidBytes)
    for ($index=0;$index -lt $sidBytes;$index++) {
        if ($child.Sid[$index] -ne $Owner.Primary.Sid[$index]) { throw 'Initial actual child selected primary SID differs.' }
    }
    $child.StatisticsCall=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetTokenInformation' -Arguments @(
        $child.Token,[UInt32]10,$child.Statistics.Pointer,[UInt32]56,[IntPtr]::Add($child.Lengths.Pointer,4))
    $child.RecheckCall=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetTokenInformation' -Arguments @(
        $child.Token,[UInt32]10,$child.Recheck.Pointer,[UInt32]56,[IntPtr]::Add($child.Lengths.Pointer,8))
    if ($child.StatisticsCall.Raw -eq 0 -or $child.RecheckCall.Raw -eq 0 -or
        [Runtime.InteropServices.Marshal]::ReadInt32($child.Lengths.Pointer,4) -ne 56 -or
        [Runtime.InteropServices.Marshal]::ReadInt32($child.Lengths.Pointer,8) -ne 56 -or
        [Runtime.InteropServices.Marshal]::ReadInt32($child.Statistics.Pointer,24) -ne 1) {
        throw 'Initial child native primary statistics incomplete.'
    }
    foreach ($at in @(0,4,8,12,24,40,44,48,52)) {
        if ([Runtime.InteropServices.Marshal]::ReadInt32($child.Statistics.Pointer,$at) -ne
            [Runtime.InteropServices.Marshal]::ReadInt32($child.Recheck.Pointer,$at)) { throw 'Initial actual child token changed.' }
    }
    foreach ($at in @(8,12)) {
        if ([Runtime.InteropServices.Marshal]::ReadInt32($child.Statistics.Pointer,$at) -ne
            [Runtime.InteropServices.Marshal]::ReadInt32($Owner.Primary.Statistics.Pointer,$at)) {
            throw 'Initial actual child authentication identity differs from selected process primary.'
        }
    }
    # The child's TokenId is its own observed original. It is intentionally NOT
    # compared to the parent's TokenId or to the issuing-effective token.
    $child.Complete=$true
    return $child
}
