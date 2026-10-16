# Actual debug event file, never a reopen by event PID/path. Native byte and
# FileId correspondence are narrower than full loader/source qualification.
function New-SlcliInitialWindowsEventFile {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $held=$Owner.Compiler
    if ($null -ne $Birth.Birth -or $null -ne $Birth.EventImage -or -not $held.Complete -or
        $held.Source.ReadRows -gt [Int32]::MaxValue/16) { throw 'Initial event image backing must precede birth.' }
    $image=[pscustomobject]@{ Birth=$Birth; Held=$held; Child=$null; Event=$null; Reference=$null;
        Body=$null; Eof=$null; Outputs=$null; Rows=[object[]]::new($held.Source.ReadRows);
        Used=0; BytesUsed=0; Before=$null; After=$null; Digest=$null;
        ManagedBody=[byte[]]::new($held.Source.Bytes); Complete=$false }
    $Birth.EventImage=$image
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $image
    $image.Body=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ([Math]::Max(1,$held.Source.Bytes))
    $image.Eof=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 1
    $image.Outputs=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ($held.Source.ReadRows*16) -Alignment 8
    return $image
}

function Assert-SlcliInitialWindowsFileCorrespondence {
    param([Parameter(Mandatory)][object]$Held,[Parameter(Mandatory)][object]$Observed)
    if (-not $Held.Complete -or -not $Observed.Complete -or $Observed.Size -ne $Held.Source.Bytes) {
        throw 'Initial actual event file length/observation incomplete.'
    }
    foreach ($field in @('Identity','Stable','OwnerSid')) {
        $expected=$Held.Leaf.Metadata.$field;$actual=$Observed.$field
        if ($null -eq $expected -or $null -eq $actual -or $expected.Length -ne $actual.Length) {
            throw 'Initial event/held file metadata extent differs.'
        }
        for ($index=0;$index -lt $expected.Length;$index++) {
            if ($expected[$index] -ne $actual[$index]) { throw 'Initial actual event/held file metadata differs.' }
        }
    }
}

function Read-SlcliInitialWindowsEventFile {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $image=$Birth.EventImage;$child=$Birth.Child
    if ($null -eq $image -or $null -ne $image.Event -or $image.Complete -or
        $null -eq $child -or -not $child.Complete -or $null -ne $child.Event.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$child.Event) -or
        -not [Object]::ReferenceEquals($image.Held,$Owner.Compiler)) {
        throw 'Initial event image child/held/stopped occurrence mismatch.'
    }
    $image.Child=$child;$image.Event=$child.Event;$image.Reference=$child.EventFile
    $image.Before=Read-SlcliInitialWindowsFileMetadata -Owner $Owner -Handle $image.Reference.Handle -Directory $false
    Assert-SlcliInitialWindowsFileCorrespondence -Held $image.Held -Observed $image.Before
    for ($index=0;$index -lt $image.Rows.Length;$index++) {
        $remaining=$image.Held.Source.Bytes-$image.BytesUsed
        $requested=if ($remaining -eq 0) {1} else {[Math]::Min(16384,$remaining)}
        $destination=if ($remaining -eq 0) {$image.Eof.Pointer} else {[IntPtr]::Add($image.Body.Pointer,$image.BytesUsed)}
        $at=[IntPtr]::Add($image.Outputs.Pointer,$index*16)
        $row=[pscustomobject]@{ Offset=$image.BytesUsed; Requested=$requested; Destination=$destination;
            Outputs=$at; Seek=$null; Read=$null; Received=0 }
        $image.Rows[$index]=$row;$image.Used++
        $row.Seek=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'SetFilePointerEx' -Arguments @(
            $image.Reference.Handle,[Int64]$row.Offset,$at,[UInt32]0)
        if ($row.Seek.Raw -eq 0 -or [Runtime.InteropServices.Marshal]::ReadInt64($at) -ne $row.Offset) {
            throw 'Initial actual event image seek failed/changed.'
        }
        $row.Read=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'ReadFile' -Arguments @(
            $image.Reference.Handle,$destination,[UInt32]$requested,[IntPtr]::Add($at,8),[IntPtr]::Zero)
        $row.Received=[Runtime.InteropServices.Marshal]::ReadInt32($at,8)
        if ($row.Read.Raw -eq 0 -or $row.Received -lt 0 -or $row.Received -gt $requested -or
            ($remaining -ne 0 -and $row.Received -eq 0) -or ($remaining -eq 0 -and $row.Received -ne 0)) {
            throw 'Initial actual complete event image read/EOF failed.'
        }
        if ($remaining -eq 0) { break }
        $image.BytesUsed+=$row.Received
    }
    $last=$image.Rows[$image.Used-1]
    if ($image.BytesUsed -ne $image.Held.Source.Bytes -or $last.Offset -ne $image.Held.Source.Bytes -or
        $last.Received -ne 0) { throw 'Initial actual image roster exhausted before independent EOF.' }
    $image.After=Read-SlcliInitialWindowsFileMetadata -Owner $Owner -Handle $image.Reference.Handle -Directory $false
    Assert-SlcliInitialWindowsFileCorrespondence -Held $image.Held -Observed $image.After
    [Runtime.InteropServices.Marshal]::Copy($image.Body.Pointer,$image.ManagedBody,0,$image.BytesUsed)
    for ($index=0;$index -lt $image.ManagedBody.Length;$index++) {
        if ($image.ManagedBody[$index] -ne $image.Held.ManagedBody[$index]) { throw 'Initial event image full source bytes differ.' }
    }
    $image.Digest=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($image.ManagedBody)).ToLowerInvariant()
    if ($image.Digest -cne $image.Held.Digest) { throw 'Initial event image full digest correspondence differs.' }
    $image.Complete=$true
    return $image
}
