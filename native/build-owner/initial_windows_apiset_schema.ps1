# Fixed v6 byte layout only. Parsing a candidate or matching source/native bytes
# cannot qualify an installed OS/private ABI, source membership or initial ROOT.
function Assert-SlcliInitialWindowsApiSetSpan {
    param([Int32]$Length,[Int64]$Offset,[Int64]$Count,[Int32]$Width)
    if ($Offset -lt 0 -or $Count -lt 0 -or $Width -le 0 -or $Offset -gt $Length -or
        $Count -gt [Int64][Math]::Floor(($Length-$Offset)/$Width)) { throw 'Complete API-set span outside original body.' }
}

function Read-SlcliInitialWindowsApiSetText {
    param([Parameter(Mandatory)][byte[]]$Bytes,[Int64]$Offset,[Int64]$Length,[bool]$AllowEmpty=$false)
    Assert-SlcliInitialWindowsApiSetSpan -Length $Bytes.Length -Offset $Offset -Count $Length -Width 1
    if ($Length%2 -ne 0 -or $Offset%2 -ne 0 -or ($Length -eq 0 -and -not $AllowEmpty)) {
        throw 'API-set complete UTF16 extent is empty/misaligned.'
    }
    if ($Length -eq 0) { return [String]::Empty }
    $text=[Text.UnicodeEncoding]::new($false,$false,$true).GetString($Bytes,[Int32]$Offset,[Int32]$Length)
    if ($text.IndexOf([char]0) -ge 0) { throw 'API-set counted UTF16 contains an embedded terminator.' }
    return $text
}

function Read-SlcliInitialWindowsApiSetSchema {
    param([Parameter(Mandatory)][byte[]]$Bytes,[Parameter(Mandatory)][Int32]$EntryCapacity,
        [Parameter(Mandatory)][Int32]$ValueCapacity)
    if ($Bytes.Length -lt 28 -or $EntryCapacity -le 0 -or $ValueCapacity -le 0 -or
        [BitConverter]::ToUInt32($Bytes,0) -ne 6 -or [BitConverter]::ToUInt32($Bytes,4) -ne $Bytes.Length) {
        throw 'Not the complete selected API-set v6 source/native body.'
    }
    $original=[pscustomobject]@{ Bytes=$Bytes; Version=[UInt32]6; Flags=[BitConverter]::ToUInt32($Bytes,8);
        Count=[Int64][BitConverter]::ToUInt32($Bytes,12); EntryOffset=[Int64][BitConverter]::ToUInt32($Bytes,16);
        HashOffset=[Int64][BitConverter]::ToUInt32($Bytes,20); HashFactor=[BitConverter]::ToUInt32($Bytes,24);
        Entries=[object[]]::new($EntryCapacity); Values=[object[]]::new($ValueCapacity); ValuesUsed=0;
        Hashes=[object[]]::new($EntryCapacity); HashSeen=[bool[]]::new($EntryCapacity); Complete=$false }
    if ($original.Count -gt $EntryCapacity -or $original.Flags -gt 3) { throw 'API-set entry/flag selection exceeds original capacity.' }
    Assert-SlcliInitialWindowsApiSetSpan -Length $Bytes.Length -Offset $original.EntryOffset -Count $original.Count -Width 24
    Assert-SlcliInitialWindowsApiSetSpan -Length $Bytes.Length -Offset $original.HashOffset -Count $original.Count -Width 8
    for ($index=0;$index -lt $original.Count;$index++) {
        $at=$original.EntryOffset+$index*24
        $entry=[pscustomobject]@{ Index=$index; Offset=$at; Flags=[BitConverter]::ToUInt32($Bytes,$at);
            NameOffset=[Int64][BitConverter]::ToUInt32($Bytes,$at+4); NameBytes=[Int64][BitConverter]::ToUInt32($Bytes,$at+8);
            HashedBytes=[Int64][BitConverter]::ToUInt32($Bytes,$at+12); ValueOffset=[Int64][BitConverter]::ToUInt32($Bytes,$at+16);
            ValueCount=[Int64][BitConverter]::ToUInt32($Bytes,$at+20); Name=$null; FirstValue=$original.ValuesUsed }
        $original.Entries[$index]=$entry
        if ($entry.Flags -gt 3 -or $entry.HashedBytes -gt $entry.NameBytes -or $entry.HashedBytes%2 -ne 0 -or
            $entry.ValueCount -gt $original.Values.Length-$original.ValuesUsed) { throw 'API-set entry/value/hashed extent invalid.' }
        $entry.Name=Read-SlcliInitialWindowsApiSetText -Bytes $Bytes -Offset $entry.NameOffset -Length $entry.NameBytes
        for ($prior=0;$prior -lt $index;$prior++) {
            if ([String]::Equals($entry.Name,$original.Entries[$prior].Name,[StringComparison]::OrdinalIgnoreCase)) {
                throw 'API-set namespace has duplicate case-folded contract name.'
            }
        }
        Assert-SlcliInitialWindowsApiSetSpan -Length $Bytes.Length -Offset $entry.ValueOffset -Count $entry.ValueCount -Width 20
        for ($valueIndex=0;$valueIndex -lt $entry.ValueCount;$valueIndex++) {
            $valueAt=$entry.ValueOffset+$valueIndex*20
            $value=[pscustomobject]@{ Entry=$entry; Index=$valueIndex; Offset=$valueAt; Flags=[BitConverter]::ToUInt32($Bytes,$valueAt);
                AliasOffset=[Int64][BitConverter]::ToUInt32($Bytes,$valueAt+4); AliasBytes=[Int64][BitConverter]::ToUInt32($Bytes,$valueAt+8);
                HostOffset=[Int64][BitConverter]::ToUInt32($Bytes,$valueAt+12); HostBytes=[Int64][BitConverter]::ToUInt32($Bytes,$valueAt+16);
                Alias=$null; Host=$null }
            $original.Values[$original.ValuesUsed]=$value;$original.ValuesUsed++
            if ($value.Flags -ne 0) { throw 'API-set value flags are not the selected known form.' }
            $value.Alias=Read-SlcliInitialWindowsApiSetText -Bytes $Bytes -Offset $value.AliasOffset -Length $value.AliasBytes -AllowEmpty $true
            $value.Host=Read-SlcliInitialWindowsApiSetText -Bytes $Bytes -Offset $value.HostOffset -Length $value.HostBytes -AllowEmpty $true
            for ($prior=$entry.FirstValue;$prior -lt $original.ValuesUsed-1;$prior++) {
                if ([String]::Equals($value.Alias,$original.Values[$prior].Alias,[StringComparison]::OrdinalIgnoreCase)) {
                    throw 'API-set namespace has ambiguous duplicate value alias.'
                }
            }
        }
        $hashAt=$original.HashOffset+$index*8
        $hash=[pscustomobject]@{ Offset=$hashAt; Hash=[BitConverter]::ToUInt32($Bytes,$hashAt);
            Index=[Int64][BitConverter]::ToUInt32($Bytes,$hashAt+4) }
        $original.Hashes[$index]=$hash
        if ($hash.Index -ge $original.Count -or $original.HashSeen[$hash.Index] -or
            ($index -ne 0 -and $hash.Hash -lt $original.Hashes[$index-1].Hash)) {
            throw 'API-set complete hash/index roster invalid/duplicate/unsorted.'
        }
        $original.HashSeen[$hash.Index]=$true
    }
    $original.Complete=$true
    return $original
}

function New-SlcliInitialWindowsApiSetSource {
    param([Parameter(Mandatory)][object]$Owner)
    if ($null -ne $Owner.ApiSetSource -or -not $Owner.ModuleSources.Complete -or
        $null -eq $Owner.Source.ApiSetSchemaMember -or $Owner.Source.ApiSetByteCapacity -isnot [Int32] -or
        $Owner.Source.ApiSetByteCapacity -lt 28 -or $Owner.Source.ApiSetEntryCapacity -isnot [Int32] -or
        $Owner.Source.ApiSetValueCapacity -isnot [Int32]) { throw 'Initial selected API-set source/capacity absent.' }
    $original=[pscustomobject]@{ Source=$null; Section=$null; Body=$null; Schema=$null; Before=$null; After=$null; Complete=$false }
    $Owner.ApiSetSource=$original
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    $original.Before=Read-SlcliInitialWindowsClock -Owner $Owner
    foreach ($source in $Owner.ModuleSources.Rows) {
        if ([Object]::ReferenceEquals($source.Member,$Owner.Source.ApiSetSchemaMember)) {
            if ($null -ne $original.Source) { throw 'Initial API-set source member is not unique.' }
            $original.Source=$source
        }
    }
    if ($null -eq $original.Source) { throw 'API-set schema is not the SAME held selected source member.' }
    $expected=[byte[]]@(46,97,112,105,115,101,116,0)
    foreach ($section in $original.Source.Geometry.Sections) {
        $equal=$true
        for ($index=0;$index -lt 8;$index++) { if ($section.Name[$index] -ne $expected[$index]) { $equal=$false;break } }
        if ($equal) {
            if ($null -ne $original.Section) { throw 'Multiple original .apiset sections are ambiguous.' }
            $original.Section=$section
        }
    }
    if ($null -eq $original.Section -or $original.Section.Raw -lt 28) { throw 'Complete original .apiset source section absent.' }
    $file=$original.Source.Held.ManagedBody;$offset=$original.Section.File
    [Int64]$size=[BitConverter]::ToUInt32($file,$offset+4)
    if ([BitConverter]::ToUInt32($file,$offset) -ne 6 -or $size -lt 28 -or $size -gt $original.Section.Raw -or
        $size -gt $Owner.Source.ApiSetByteCapacity) { throw 'Original complete schema extent exceeds source/profile.' }
    $original.Body=[byte[]]::new([Int32]$size)
    [Array]::Copy($file,$offset,$original.Body,0,$size)
    $original.Schema=Read-SlcliInitialWindowsApiSetSchema -Bytes $original.Body -EntryCapacity $Owner.Source.ApiSetEntryCapacity -ValueCapacity $Owner.Source.ApiSetValueCapacity
    $original.After=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($original.After.Tick -lt $original.Before.Tick) { throw 'API-set source processing clock regressed.' }
    $Owner.PriorTick=$original.After.Tick;$original.Complete=$true
    return $original
}
