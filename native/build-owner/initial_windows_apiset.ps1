# Fixed actual child namespace read. Native addresses derive only from the
# retained SAME child PBI; no caller PID/pointer/read callback is accepted.
function New-SlcliInitialWindowsApiSetCapture {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $capacity=$Owner.Source.ApiSetRegionCapacity
    if ($null -ne $Birth.Birth -or $null -ne $Birth.ApiSet -or -not $Owner.ApiSetSource.Complete -or
        $capacity -isnot [Int32] -or $capacity -le 0 -or $capacity -gt [Int32]::MaxValue/64) {
        throw 'Complete finite API-set capture backing must precede child birth.'
    }
    $original=[pscustomobject]@{ Birth=$Birth; Source=$Owner.ApiSetSource; Thread=$null; Map=[Int64]0;
        Allocation=[Int64]0; Body=$null; ManagedBody=[byte[]]::new($Owner.ApiSetSource.Body.Length);
        Regions=[object[]]::new($capacity); Used=0; Cursor=0; PointerBefore=$null; PointerAfter=$null;
        HeaderBefore=$null; HeaderAfter=$null; Path=$null; Schema=$null; BeforeCompare=$null; AfterCompare=$null;
        Started=$false; Complete=$false }
    $Birth.ApiSet=$original
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    $original.Body=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes $original.ManagedBody.Length
    $original.Path=New-SlcliInitialWindowsModulePath -Owner $Owner
    foreach ($name in @('PointerBefore','PointerAfter','HeaderBefore','HeaderAfter')) {
        $bytes=if ($name.StartsWith('Pointer',[StringComparison]::Ordinal)) {8} else {28}
        $record=[pscustomobject]@{ Buffer=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes $bytes -Alignment 8);
            Control=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 64 -Alignment 8); Offset=0;
            Address=[Int64]0; Bytes=$bytes; Query=$null; Read=$null; Received=[Int64]0; Allocation=[Int64]0 }
        $original.$name=$record
    }
    for ($index=0;$index -lt $capacity;$index++) {
        $original.Regions[$index]=[pscustomobject]@{ Buffer=$null;
            Control=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 64 -Alignment 8); Offset=0;
            Address=[Int64]0; Bytes=0; Query=$null; Read=$null; Received=[Int64]0; Allocation=[Int64]0 }
    }
    return $original
}

function Read-SlcliInitialWindowsApiSetSlice {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Capture,
        [Parameter(Mandatory)][object]$Record)
    $pointer=([Object]::ReferenceEquals($Record,$Capture.PointerBefore) -or [Object]::ReferenceEquals($Record,$Capture.PointerAfter))
    $header=([Object]::ReferenceEquals($Record,$Capture.HeaderBefore) -or [Object]::ReferenceEquals($Record,$Capture.HeaderAfter))
    $body=$false
    for ($index=0;$index -lt $Capture.Used;$index++) {
        if ([Object]::ReferenceEquals($Record,$Capture.Regions[$index])) { $body=$true;break }
    }
    if (($pointer -and $header) -or ($body -and ($pointer -or $header)) -or (-not $pointer -and -not $header -and -not $body) -or
        $null -ne $Record.Query -or $null -ne $Record.Read) { throw 'API-set read is not a fresh fixed owned slice.' }
    $birth=$Capture.Birth
    if ($pointer) {
        $peb=[Runtime.InteropServices.Marshal]::ReadIntPtr($birth.Child.Basic.Pointer,8).ToInt64()
        if ($peb -le 0 -or $peb -gt [Int64]::MaxValue-0x68) { throw 'Actual child PBI PEB pointer invalid.' }
        $Record.Address=$peb+0x68
    } else {
        if ($Capture.Map -le 0) { throw 'Actual API-set native map pointer absent.' }
        $Record.Address=$Capture.Map
        if ($body) { $Record.Offset=$Capture.Cursor;$Record.Address+=$Record.Offset }
    }
    if ($Record.Address -lt $Owner.InitialGeometry.Minimum -or $Record.Address -gt $Owner.InitialGeometry.Maximum) {
        throw 'Actual PBI-derived namespace slice outside native user address range.'
    }
    $at=$Record.Control.Pointer
    $Record.Query=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'VirtualQueryEx' -Arguments @(
        $birth.ProcessReference.Handle,[IntPtr]::new($Record.Address),$at,[UIntPtr]::new(48))
    $regionBase=[Runtime.InteropServices.Marshal]::ReadIntPtr($at).ToInt64()
    $regionBytes=[Runtime.InteropServices.Marshal]::ReadInt64($at,24)
    $Record.Allocation=[Runtime.InteropServices.Marshal]::ReadIntPtr($at,8).ToInt64()
    $protection=[Runtime.InteropServices.Marshal]::ReadInt32($at,36)
    if ($Record.Query.Raw -ne 48 -or $regionBase -le 0 -or $regionBase -gt $Record.Address -or
        $regionBytes -le 0 -or $regionBytes -gt $Owner.InitialGeometry.Maximum+1-$regionBase -or
        $regionBytes -le $Record.Address-$regionBase -or [Runtime.InteropServices.Marshal]::ReadInt32($at,32) -ne 0x1000 -or
        ($protection -band 0x100) -ne 0 -or ($protection -band 0xff) -notin @(2,4,8,32,64,128)) {
        throw 'Actual PBI-derived namespace slice unobserved/unreadable/guarded.'
    }
    if (-not $pointer) {
        $type=[Runtime.InteropServices.Marshal]::ReadInt32($at,40)
        if (($protection -band 0xff) -ne 2 -or $type -notin @(0x40000,0x1000000) -or
            $Record.Allocation -le 0 -or $Record.Allocation -gt $Record.Address) {
            throw 'Actual API-set map is not readonly attributed mapped/image storage.'
        }
        if ($Capture.Allocation -eq 0) { $Capture.Allocation=$Record.Allocation }
        elseif ($Capture.Allocation -ne $Record.Allocation) { throw 'Actual API-set body crossed its original allocation.' }
    }
    $available=$regionBytes-($Record.Address-$regionBase)
    if ($body) {
        $Record.Bytes=[Int32][Math]::Min($available,[Int64]$Capture.ManagedBody.Length-$Record.Offset)
        $destination=[IntPtr]::Add($Capture.Body.Pointer,$Record.Offset)
    } else {
        if ($Record.Bytes -gt $available) { throw 'Actual fixed namespace pointer/header crosses unobserved region.' }
        $destination=$Record.Buffer.Pointer
    }
    if ($Record.Bytes -le 0) { throw 'Actual namespace slice did not make positive bounded progress.' }
    $Record.Read=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'ReadProcessMemory' -Arguments @(
        $birth.ProcessReference.Handle,[IntPtr]::new($Record.Address),$destination,[UIntPtr]::new([UInt64]$Record.Bytes),[IntPtr]::Add($at,48))
    $Record.Received=[Runtime.InteropServices.Marshal]::ReadInt64($at,48)
    if ($Record.Read.Raw -eq 0 -or $Record.Received -ne $Record.Bytes) { throw 'Actual namespace native read failed/short; prefix retained.' }
    return $Record
}

function Read-SlcliInitialWindowsApiSet {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $original=$Birth.ApiSet
    if ($null -eq $original -or $original.Started -or -not $Birth.Child.Complete -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event) -or $null -ne $Birth.PendingEvent.Continue) {
        throw 'Actual API-set namespace requires SAME initial stopped root.'
    }
    $original.Started=$true
    $original.Thread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($original.Thread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'Actual namespace capture left native creator thread.' }
    $null=Read-SlcliInitialWindowsApiSetSlice -Owner $Owner -Capture $original -Record $original.PointerBefore
    $original.Map=[Runtime.InteropServices.Marshal]::ReadIntPtr($original.PointerBefore.Buffer.Pointer).ToInt64()
    if ($original.Map -le 0 -or $original.Map%4 -ne 0 -or
        $original.Map -gt [Int64]::MaxValue-$original.ManagedBody.Length) { throw 'Actual namespace map pointer invalid/overflowing.' }
    $null=Read-SlcliInitialWindowsApiSetSlice -Owner $Owner -Capture $original -Record $original.HeaderBefore
    for ($index=0;$index -lt 28;$index++) {
        if ([Runtime.InteropServices.Marshal]::ReadByte($original.HeaderBefore.Buffer.Pointer,$index) -ne $original.Source.Body[$index]) {
            throw 'Actual complete API-set header differs from held original source.'
        }
    }
    $original.Path.Call=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'K32GetMappedFileNameW' -Arguments @(
        $Birth.ProcessReference.Handle,[IntPtr]::new($original.Map),$original.Path.Buffer.Pointer,[UInt32]$original.Path.Units)
    Complete-SlcliInitialWindowsModulePath -Original $original.Path
    if (-not [String]::Equals($original.Path.Text,$original.Source.Source.NativePath.Text,[StringComparison]::Ordinal)) {
        throw 'Actual native schema map pathname differs from SAME held source.'
    }
    while ($original.Cursor -lt $original.ManagedBody.Length) {
        if ($original.Used -ge $original.Regions.Length) { throw 'Actual complete schema native region budget exhausted.' }
        $record=$original.Regions[$original.Used];$original.Used++
        $null=Read-SlcliInitialWindowsApiSetSlice -Owner $Owner -Capture $original -Record $record
        $original.Cursor+=$record.Bytes
    }
    $null=Read-SlcliInitialWindowsApiSetSlice -Owner $Owner -Capture $original -Record $original.PointerAfter
    $null=Read-SlcliInitialWindowsApiSetSlice -Owner $Owner -Capture $original -Record $original.HeaderAfter
    if ([Runtime.InteropServices.Marshal]::ReadIntPtr($original.PointerAfter.Buffer.Pointer).ToInt64() -ne $original.Map) {
        throw 'Actual child namespace pointer changed during complete observation.'
    }
    [Runtime.InteropServices.Marshal]::Copy($original.Body.Pointer,$original.ManagedBody,0,$original.ManagedBody.Length)
    $original.BeforeCompare=Read-SlcliInitialWindowsClock -Owner $Owner
    for ($index=0;$index -lt $original.ManagedBody.Length;$index++) {
        if ($original.ManagedBody[$index] -ne $original.Source.Body[$index]) { throw 'Complete actual/source API-set body bytes differ.' }
        if ($index -lt 28 -and [Runtime.InteropServices.Marshal]::ReadByte($original.HeaderAfter.Buffer.Pointer,$index) -ne $original.ManagedBody[$index]) {
            throw 'Actual independent namespace header changed during observation.'
        }
    }
    $original.Schema=Read-SlcliInitialWindowsApiSetSchema -Bytes $original.ManagedBody -EntryCapacity $Owner.Source.ApiSetEntryCapacity -ValueCapacity $Owner.Source.ApiSetValueCapacity
    $original.AfterCompare=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($original.AfterCompare.Tick -lt $original.BeforeCompare.Tick) { throw 'Actual namespace processing clock regressed.' }
    $Owner.PriorTick=$original.AfterCompare.Tick;$original.Complete=$true
    return $original
}
