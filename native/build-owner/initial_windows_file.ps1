# Actual held initial A source/tool acquisition. Hash equality is byte
# correspondence only; native source membership/initial ROOT remains separate.
function Test-SlcliInitialWindowsCanonicalPath {
    param([AllowNull()][AllowEmptyString()][string]$Path)
    if ([string]::IsNullOrEmpty($Path) -or $Path.Length -gt 32766 -or $Path -cnotmatch '^[A-Za-z]:\\' -or
        $Path.Contains('/') -or $Path.Contains('"') -or $Path.Contains([char]0) -or $Path.Substring(3).Contains(':')) { return $false }
    foreach ($part in $Path.Substring(3).Split('\')) {
        if (($part.Length -eq 0 -and $Path.Length -ne 3) -or $part -ceq '.' -or $part -ceq '..' -or
            $part.EndsWith('.') -or $part.EndsWith(' ') -or $part.IndexOfAny([char[]]'<>|?*') -ge 0) { return $false }
        foreach ($character in $part.ToCharArray()) { if ([Int32]$character -lt 32) { return $false } }
    }
    return $true
}

function Read-SlcliInitialWindowsFileMetadata {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][IntPtr]$Handle,
        [Parameter(Mandatory)][bool]$Directory)
    $layout=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 744 -Alignment 8
    $base=$layout.Pointer
    $at=@{ Access=$base; AccessBytes=[IntPtr]::Add($base,56); Flags=[IntPtr]::Add($base,60);
        Mode=[IntPtr]::Add($base,64); Io=[IntPtr]::Add($base,72); Identity=[IntPtr]::Add($base,88);
        Standard=[IntPtr]::Add($base,112); Attributes=[IntPtr]::Add($base,136); Basic=[IntPtr]::Add($base,144);
        Security=[IntPtr]::Add($base,184); SecurityBytes=[IntPtr]::Add($base,696);
        Owner=[IntPtr]::Add($base,704); Defaulted=[IntPtr]::Add($base,712) }
    $original=[pscustomobject]@{ Handle=$Handle; Directory=$Directory; Layout=$layout; At=$at;
        Type=$null; Access=$null; Flags=$null; Mode=$null; Information=[object[]]::new(4);
        Security=$null; Owner=$null; SidValid=$null; SidLength=$null; OwnerSid=$null;
        Identity=$null; Stable=$null; Size=[Int64]0; Complete=$false }
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    $original.Type=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetFileType' -Arguments @($Handle)
    if ($original.Type.Raw -ne 1) { throw 'Initial held object is not a disk file/directory.' }
    $original.Access=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'NtQueryObject' -Arguments @($Handle,[UInt32]0,$at.Access,[UInt32]56,$at.AccessBytes)
    if ($original.Access.Raw -ne 0 -or [Runtime.InteropServices.Marshal]::ReadInt32($at.AccessBytes) -ne 56) {
        $Owner.Pending=$true;throw 'Initial native access query unresolved.'
    }
    $rights=[Runtime.InteropServices.Marshal]::ReadInt32($at.Access,4)
    if (($rights -band 0x00120081) -ne 0x00120081 -or ($rights -band 0x500D0116) -ne 0 -or
        ($Directory -and ($rights -band 0x40) -ne 0)) { throw 'Initial held object has missing/read-write mutable rights.' }
    $original.Flags=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetHandleInformation' -Arguments @($Handle,$at.Flags)
    if ($original.Flags.Raw -eq 0 -or ([Runtime.InteropServices.Marshal]::ReadInt32($at.Flags) -band 1) -ne 0) {
        throw 'Initial held source handle is inheritable or unobserved.'
    }
    $original.Mode=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'NtQueryInformationFile' -Arguments @($Handle,$at.Io,$at.Mode,[UInt32]4,[UInt32]16)
    if ($original.Mode.Raw -ne 0 -or [Runtime.InteropServices.Marshal]::ReadInt32($at.Io) -ne 0 -or
        [Runtime.InteropServices.Marshal]::ReadInt64($at.Io,8) -ne 4 -or
        ([Runtime.InteropServices.Marshal]::ReadInt32($at.Mode) -band 0x30) -eq 0) {
        $Owner.Pending=$true;throw 'Initial native file mode query unresolved.'
    }
    $definitions=@(@([UInt32]18,$at.Identity,[UInt32]24),@([UInt32]1,$at.Standard,[UInt32]24),
        @([UInt32]9,$at.Attributes,[UInt32]8),@([UInt32]0,$at.Basic,[UInt32]40))
    for ($index=0;$index -lt 4;$index++) {
        $part=$definitions[$index]
        $original.Information[$index]=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetFileInformationByHandleEx' -Arguments @($Handle,$part[0],$part[1],$part[2])
        if ($original.Information[$index].Raw -eq 0) { throw 'Initial native file metadata incomplete.' }
    }
    $original.Size=[Runtime.InteropServices.Marshal]::ReadInt64($at.Standard,8)
    $nativeDirectory=[Runtime.InteropServices.Marshal]::ReadByte($at.Standard,21) -ne 0
    $attributes=[Runtime.InteropServices.Marshal]::ReadInt32($at.Attributes)
    if ($original.Size -lt 0 -or [Runtime.InteropServices.Marshal]::ReadByte($at.Standard,20) -ne 0 -or
        $nativeDirectory -ne $Directory -or (($attributes -band 0x10) -ne 0) -ne $Directory -or
        ($attributes -band 0x400) -ne 0) { throw 'Initial source object is deleted/reparse/wrong native kind.' }
    $original.Security=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetKernelObjectSecurity' -Arguments @($Handle,[UInt32]1,$at.Security,[UInt32]512,$at.SecurityBytes)
    $securityBytes=[Runtime.InteropServices.Marshal]::ReadInt32($at.SecurityBytes)
    if ($original.Security.Raw -eq 0 -or $securityBytes -lt 20 -or $securityBytes -gt 512) { throw 'Initial native source owner descriptor incomplete.' }
    $original.Owner=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetSecurityDescriptorOwner' -Arguments @($at.Security,$at.Owner,$at.Defaulted)
    $sid=[Runtime.InteropServices.Marshal]::ReadIntPtr($at.Owner)
    $offset=$sid.ToInt64()-$at.Security.ToInt64()
    if ($original.Owner.Raw -eq 0 -or $offset -lt 0 -or $offset -gt $securityBytes-8 -or ($sid.ToInt64() -band 3) -ne 0) {
        throw 'Initial native source owner pointer outside descriptor.'
    }
    $sidBytes=8+4*[Int32][Runtime.InteropServices.Marshal]::ReadByte($sid,1)
    if ($sidBytes -gt 68 -or $sidBytes -gt $securityBytes-$offset) { throw 'Initial native source owner SID extent invalid.' }
    $original.SidValid=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'IsValidSid' -Arguments @($sid)
    if ($original.SidValid.Raw -eq 0) { throw 'Initial native source owner SID invalid.' }
    $original.SidLength=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetLengthSid' -Arguments @($sid)
    if ($original.SidLength.Raw -ne $sidBytes) { throw 'Initial native source owner SID length changed.' }
    $original.OwnerSid=[byte[]]::new($sidBytes)
    [Runtime.InteropServices.Marshal]::Copy($sid,$original.OwnerSid,0,$sidBytes)
    $original.Identity=[byte[]]::new(24)
    [Runtime.InteropServices.Marshal]::Copy($at.Identity,$original.Identity,0,24)
    # Match only the same documented stable cuts as held_file_windows.c;
    # LastAccessTime/padding are retained raw, not invented invariants.
    $original.Stable=[byte[]]::new(40)
    [Runtime.InteropServices.Marshal]::Copy([IntPtr]::Add($at.Standard,8),$original.Stable,0,8)
    [Runtime.InteropServices.Marshal]::Copy($at.Basic,$original.Stable,8,8)
    [Runtime.InteropServices.Marshal]::Copy([IntPtr]::Add($at.Basic,16),$original.Stable,16,16)
    [Runtime.InteropServices.Marshal]::Copy($at.Attributes,$original.Stable,32,8)
    $original.Complete=$true
    return $original
}

function Open-SlcliInitialWindowsSource {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][string]$Path,
        [Parameter(Mandatory)][bool]$Directory)
    if ($null -eq $Owner.Effective -or -not $Owner.Effective.Complete -or
        -not (Test-SlcliInitialWindowsCanonicalPath -Path $Path)) { throw 'Initial canonical source path invalid.' }
    $text=Get-SlcliInitialWindowsText -Owner $Owner -Text $Path
    [UInt32]$flags=0x00200080
    if ($Directory) { $flags=$flags -bor [UInt32]0x02000000 }
    $original=[pscustomobject]@{ Path=$text; Principal=$Owner.Effective; Open=$null; Handle=[IntPtr]::Zero;
        Reference=$null; Metadata=$null; Directory=$Directory }
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    $original.Open=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'CreateFileW' -Arguments @(
        $text.Native.Pointer,[UInt32]2147483648,[UInt32]1,[IntPtr]::Zero,[UInt32]3,$flags,[IntPtr]::Zero)
    $original.Handle=[IntPtr]::new($original.Open.Raw)
    $original.Reference=Register-SlcliInitialWindowsHandle -Owner $Owner -Row $original.Open -Handle $original.Handle -Role 'HeldSource'
    if ($original.Open.Raw -eq 0 -or $original.Open.Raw -eq -1) { throw 'Initial readonly source acquisition failed.' }
    $original.Metadata=Read-SlcliInitialWindowsFileMetadata -Owner $Owner -Handle $original.Handle -Directory $Directory
    return $original
}

function Read-SlcliInitialWindowsHeldFile {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Member)
    if ($Member.Bytes -isnot [Int32] -or $Member.Bytes -lt 0 -or $Member.ReadRows -isnot [Int32] -or
        $Member.ReadRows -le 0 -or $Member.ReadRows -gt [Int32]::MaxValue/16 -or $Member.Sha256 -cnotmatch '^[a-f0-9]{64}$' -or
        $Member.Path -isnot [string] -or $Member.Path.Length -lt 4) { throw 'Invalid initial held member source geometry.' }
    $minimum=[Int64][Math]::Ceiling($Member.Bytes / 16384.0)+1
    if ($Member.ReadRows -lt $minimum -or $Member.ReadRows -gt [Int64]$Member.Bytes+1) { throw 'Initial complete read/EOF roster insufficient.' }
    $body=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ([Math]::Max(1,$Member.Bytes))
    $eof=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 1
    $readOutputs=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ([Int32]($Member.ReadRows*16)) -Alignment 8
    $original=[pscustomobject]@{ Source=$Member; Principal=$Owner.Effective; Ancestors=[Collections.Generic.List[object]]::new();
        Leaf=$null; Body=$body; Eof=$eof; ReadOutputs=$readOutputs; Rows=[object[]]::new($Member.ReadRows);
        ReadRowsUsed=0; BytesUsed=0; ManagedBody=[byte[]]::new($Member.Bytes); After=$null; Digest=$null; Complete=$false }
    # Roots every acquired ancestor/read/window before the next source action.
    $Owner.Files.Add($original)
    $root=$Member.Path.Substring(0,3)
    $original.Ancestors.Add((Open-SlcliInitialWindowsSource -Owner $Owner -Path $root -Directory $true))
    for ($offset=3;$offset -lt $Member.Path.Length;$offset++) {
        if ($Member.Path[$offset] -eq '\') {
            $original.Ancestors.Add((Open-SlcliInitialWindowsSource -Owner $Owner -Path $Member.Path.Substring(0,$offset) -Directory $true))
        }
    }
    $original.Leaf=Open-SlcliInitialWindowsSource -Owner $Owner -Path $Member.Path -Directory $false
    if ($original.Leaf.Metadata.Size -ne $Member.Bytes) { throw 'Initial native member length differs.' }
    for ($index=0;$index -lt $Member.ReadRows;$index++) {
        $remaining=$Member.Bytes-$original.BytesUsed
        $requested=if ($remaining -eq 0) {1} else {[Math]::Min(16384,$remaining)}
        $destination=if ($remaining -eq 0) {$eof.Pointer} else {[IntPtr]::Add($body.Pointer,$original.BytesUsed)}
        $out=[IntPtr]::Add($readOutputs.Pointer,$index*16)
        $read=[pscustomobject]@{ Offset=$original.BytesUsed; Requested=$requested; Destination=$destination;
            Outputs=$out; Seek=$null; Read=$null; Received=0 }
        $original.Rows[$index]=$read;$original.ReadRowsUsed++
        $read.Seek=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'SetFilePointerEx' -Arguments @(
            $original.Leaf.Handle,[Int64]$read.Offset,$out,[UInt32]0)
        if ($read.Seek.Raw -eq 0 -or [Runtime.InteropServices.Marshal]::ReadInt64($out) -ne $read.Offset) { throw 'Initial source read seek failed.' }
        $read.Read=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'ReadFile' -Arguments @(
            $original.Leaf.Handle,$destination,[UInt32]$requested,[IntPtr]::Add($out,8),[IntPtr]::Zero)
        $read.Received=[Runtime.InteropServices.Marshal]::ReadInt32($out,8)
        if ($read.Read.Raw -eq 0 -or $read.Received -lt 0 -or $read.Received -gt $requested -or
            ($remaining -ne 0 -and $read.Received -eq 0) -or ($remaining -eq 0 -and $read.Received -ne 0)) {
            throw 'Initial complete source read/EOF failed.'
        }
        if ($remaining -eq 0) { break }
        $original.BytesUsed+=$read.Received
    }
    $last=$original.Rows[$original.ReadRowsUsed-1]
    if ($original.BytesUsed -ne $Member.Bytes -or $last.Offset -ne $Member.Bytes -or $last.Received -ne 0) { throw 'Initial source read roster exhausted before actual EOF.' }
    $original.After=Read-SlcliInitialWindowsFileMetadata -Owner $Owner -Handle $original.Leaf.Handle -Directory $false
    foreach ($field in @('Identity','Stable','OwnerSid')) {
        $before=$original.Leaf.Metadata.$field;$after=$original.After.$field
        if ($before.Length -ne $after.Length) { throw 'Initial source metadata identity changed.' }
        for ($index=0;$index -lt $before.Length;$index++) { if ($before[$index] -ne $after[$index]) { throw 'Initial source SAME-handle metadata changed.' } }
    }
    [Runtime.InteropServices.Marshal]::Copy($body.Pointer,$original.ManagedBody,0,$Member.Bytes)
    $original.Digest=[Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($original.ManagedBody)).ToLowerInvariant()
    if ($original.Digest -cne $Member.Sha256) { throw 'Initial complete member byte correspondence differs.' }
    $original.Complete=$true
    return $original
}
