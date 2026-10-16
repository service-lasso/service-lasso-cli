# Fixed initial A token observations. No current-user/default principal issuer.
function Read-SlcliInitialWindowsPrincipal {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Expected,
        [Parameter(Mandatory)][ValidateSet('Effective','Primary')][string]$Plane)
    if ($Expected.Sid -isnot [byte[]] -or $Expected.Sid.Length -lt 8 -or $Expected.Sid.Length -gt 68 -or
        $Expected.TokenId -isnot [UInt32[]] -or $Expected.TokenId.Length -ne 2 -or
        $Expected.AuthenticationId -isnot [UInt32[]] -or $Expected.AuthenticationId.Length -ne 2 -or
        ($Plane -ceq 'Primary' -and $null -eq $Owner.Effective)) {
        throw 'Missing independently selected initial token constraints.'
    }
    $token=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 8 -Alignment 8
    $user=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 88 -Alignment 8
    $stats=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 56 -Alignment 8
    $recheck=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 56 -Alignment 8
    $lengths=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 12 -Alignment 4
    $times=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 64 -Alignment 8
    $original=[pscustomobject]@{ Expected=$Expected; Plane=$Plane; TokenBuffer=$token; Token=[IntPtr]::Zero;
        User=$user; Statistics=$stats; Recheck=$recheck; Lengths=$lengths; Times=$times;
        ProcessId=$null; ThreadId=$null; ProcessTimes=$null; ThreadTimes=$null;
        ThreadToken=$null; ProcessToken=$null; UserCall=$null; StatisticsCall=$null;
        SidValid=$null; SidLength=$null; RecheckCall=$null; Sid=$null; TokenReference=$null;
        IssuingEffective=$Owner.Effective; Complete=$false }
    $Owner.$Plane=$original
    $original.ProcessId=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentProcessId' -Arguments @()
    $original.ThreadId=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($original.ProcessId.Raw -le 0 -or $original.ThreadId.Raw -le 0 -or
        ($Plane -ceq 'Primary' -and ($original.ProcessId.Raw -ne $Owner.Effective.ProcessId.Raw -or
        $original.ThreadId.Raw -ne $Owner.Effective.ThreadId.Raw))) { throw 'Initial token issuing process/thread changed.' }
    $p=[IntPtr]::new($Owner.Process.Raw);$t=[IntPtr]::new($Owner.Thread.Raw)
    $original.ProcessTimes=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetProcessTimes' -Arguments @(
        $p,$times.Pointer,[IntPtr]::Add($times.Pointer,8),[IntPtr]::Add($times.Pointer,16),[IntPtr]::Add($times.Pointer,24))
    $original.ThreadTimes=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetThreadTimes' -Arguments @(
        $t,[IntPtr]::Add($times.Pointer,32),[IntPtr]::Add($times.Pointer,40),[IntPtr]::Add($times.Pointer,48),[IntPtr]::Add($times.Pointer,56))
    if ($original.ProcessTimes.Raw -eq 0 -or $original.ThreadTimes.Raw -eq 0) { throw 'Initial issuing lifetime observation failed.' }
    if ($Plane -ceq 'Effective') {
        $original.ThreadToken=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'OpenThreadToken' -Arguments @($t,[UInt32]8,[Int32]1,$token.Pointer)
        if ($original.ThreadToken.Raw -eq 0 -and ($original.ThreadToken.CacheObserved -ne 1 -or $original.ThreadToken.CachedError -ne 1008)) {
            throw 'Initial effective thread token acquisition failed.'
        }
    }
    if ($Plane -ceq 'Primary' -or $original.ThreadToken.Raw -eq 0) {
        $original.ProcessToken=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'OpenProcessToken' -Arguments @($p,[UInt32]8,$token.Pointer)
        if ($original.ProcessToken.Raw -eq 0) { throw 'Initial process token acquisition failed.' }
    }
    $original.Token=[Runtime.InteropServices.Marshal]::ReadIntPtr($token.Pointer)
    $referenceRow=if ($null -ne $original.ProcessToken) { $original.ProcessToken } else { $original.ThreadToken }
    $original.TokenReference=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $referenceRow -Handle $original.Token -Role ($Plane+'Token')
    if ($original.Token -eq [IntPtr]::Zero -or $original.Token.ToInt64() -eq -1) { throw 'Initial token native reference invalid.' }
    $original.UserCall=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetTokenInformation' -Arguments @(
        $original.Token,[UInt32]1,$user.Pointer,[UInt32]88,$lengths.Pointer)
    $userBytes=[Runtime.InteropServices.Marshal]::ReadInt32($lengths.Pointer)
    $sid=[Runtime.InteropServices.Marshal]::ReadIntPtr($user.Pointer)
    [Int64]$offset=$sid.ToInt64()-$user.Pointer.ToInt64()
    if ($original.UserCall.Raw -eq 0 -or $userBytes -lt 16 -or $userBytes -gt 88 -or $offset -lt 16 -or $offset -gt $userBytes-8) {
        throw 'Initial token user/SID extent invalid.'
    }
    $sidBytes=8+4*[Int32][Runtime.InteropServices.Marshal]::ReadByte($sid,1)
    if ($sidBytes -gt 68 -or $sidBytes -gt $userBytes-$offset -or $sidBytes -ne $Expected.Sid.Length) {
        throw 'Initial token SID source extent differs.'
    }
    $original.SidValid=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'IsValidSid' -Arguments @($sid)
    if ($original.SidValid.Raw -eq 0) { throw 'Initial token SID invalid.' }
    $original.SidLength=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetLengthSid' -Arguments @($sid)
    if ($original.SidLength.Raw -ne $sidBytes) { throw 'Initial token SID native length changed.' }
    $original.Sid=[byte[]]::new($sidBytes)
    [Runtime.InteropServices.Marshal]::Copy($sid,$original.Sid,0,$sidBytes)
    for ($index=0;$index -lt $sidBytes;$index++) { if ($original.Sid[$index] -ne $Expected.Sid[$index]) { throw 'Initial designated SID differs.' } }
    $original.StatisticsCall=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetTokenInformation' -Arguments @(
        $original.Token,[UInt32]10,$stats.Pointer,[UInt32]56,[IntPtr]::Add($lengths.Pointer,4))
    $original.RecheckCall=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetTokenInformation' -Arguments @(
        $original.Token,[UInt32]10,$recheck.Pointer,[UInt32]56,[IntPtr]::Add($lengths.Pointer,8))
    if ($original.StatisticsCall.Raw -eq 0 -or $original.RecheckCall.Raw -eq 0 -or
        [Runtime.InteropServices.Marshal]::ReadInt32($lengths.Pointer,4) -ne 56 -or
        [Runtime.InteropServices.Marshal]::ReadInt32($lengths.Pointer,8) -ne 56) { throw 'Initial token statistics incomplete.' }
    foreach ($offset in @(0,4,8,12,24,40,44,48,52)) {
        if ([Runtime.InteropServices.Marshal]::ReadInt32($stats.Pointer,$offset) -ne
            [Runtime.InteropServices.Marshal]::ReadInt32($recheck.Pointer,$offset)) { throw 'Initial token statistics changed.' }
    }
    for ($index=0;$index -lt 2;$index++) {
        $tokenPart=[BitConverter]::ToUInt32([BitConverter]::GetBytes([Runtime.InteropServices.Marshal]::ReadInt32($stats.Pointer,$index*4)),0)
        $authPart=[BitConverter]::ToUInt32([BitConverter]::GetBytes([Runtime.InteropServices.Marshal]::ReadInt32($stats.Pointer,8+$index*4)),0)
        if ($tokenPart -ne $Expected.TokenId[$index] -or $authPart -ne $Expected.AuthenticationId[$index]) {
            throw 'Initial selected native token identity differs.'
        }
    }
    if ($Plane -ceq 'Primary' -and [Runtime.InteropServices.Marshal]::ReadInt32($stats.Pointer,24) -ne 1) {
        throw 'CreateProcessW principal is not the separately selected primary token.'
    }
    $original.Complete=$true
    return $original
}
