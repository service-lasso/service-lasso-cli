# Source export declarations and actual complete captured EAT bytes only.
# No GetProcAddress, LoadLibrary, source issuer or import-resolution grant.
function Read-SlcliInitialWindowsExports {
    param([Parameter(Mandatory)][object]$Held,[Parameter(Mandatory)][object]$Geometry,
        [Parameter(Mandatory)][Int32]$Capacity,[Parameter(Mandatory)][Int32]$NameCapacity)
    if (-not $Held.Complete -or $Capacity -le 0 -or $NameCapacity -le 0 -or $NameCapacity -gt 32768) {
        throw 'Invalid finite original export source geometry.'
    }
    $file=$Held.ManagedBody
    $original=[pscustomobject]@{ Held=$Held; Geometry=$Geometry; Capacity=$Capacity; NameCapacity=$NameCapacity;
        Rva=[Int64]0; Bytes=[Int64]0; DllName=$null; OrdinalBase=[UInt32]0; AddressRva=[Int64]0;
        Functions=[object[]]::new($Capacity); FunctionCount=0; Names=[object[]]::new($Capacity);
        NameCount=0; SourceComplete=$false }
    if ($Geometry.Directories -gt 0) {
        $original.Rva=[BitConverter]::ToUInt32($file,$Geometry.Optional+112)
        $original.Bytes=[BitConverter]::ToUInt32($file,$Geometry.Optional+116)
    }
    if (($original.Rva -eq 0) -ne ($original.Bytes -eq 0)) { throw 'Original export directory RVA/size pair differs.' }
    if ($original.Bytes -eq 0) { $original.SourceComplete=$true;return $original }
    if ($original.Bytes -lt 40) { throw 'Original export directory header truncated.' }
    $at=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $original.Rva -Bytes $original.Bytes
    [Int64]$name=[BitConverter]::ToUInt32($file,$at+12)
    $original.OrdinalBase=[BitConverter]::ToUInt32($file,$at+16)
    [Int64]$functions=[BitConverter]::ToUInt32($file,$at+20)
    [Int64]$names=[BitConverter]::ToUInt32($file,$at+24)
    $original.AddressRva=[BitConverter]::ToUInt32($file,$at+28)
    [Int64]$namePointers=[BitConverter]::ToUInt32($file,$at+32)
    [Int64]$ordinals=[BitConverter]::ToUInt32($file,$at+36)
    if ($functions -gt $Capacity -or $names -gt $Capacity -or ($functions -eq 0 -and $names -ne 0) -or
        ($functions -ne 0 -and [Int64]$original.OrdinalBase+$functions-1 -gt [UInt32]::MaxValue)) {
        throw 'Original complete export count/ordinal capacity differs.'
    }
    $original.DllName=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $Geometry -Rva $name -Capacity $NameCapacity
    $addressOffset=if ($functions -ne 0) {Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $original.AddressRva -Bytes ($functions*4)} else {0}
    $namesOffset=if ($names -ne 0) {Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $namePointers -Bytes ($names*4)} else {0}
    $ordinalOffset=if ($names -ne 0) {Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $ordinals -Bytes ($names*2)} else {0}
    $original.FunctionCount=[Int32]$functions;$original.NameCount=[Int32]$names
    for ($index=0;$index -lt $functions;$index++) {
        [Int64]$rva=[BitConverter]::ToUInt32($file,$addressOffset+$index*4)
        $row=[pscustomobject]@{ Index=$index; Ordinal=[UInt32]([Int64]$original.OrdinalBase+$index);
            EatRva=($original.AddressRva+$index*4); Rva=$rva; Hole=($rva -eq 0); Forwarder=$null }
        $original.Functions[$index]=$row
        if ($row.Hole) { continue }
        if ($rva -ge $Geometry.ImageBytes) { throw 'Original export target outside complete image.' }
        if ($rva -ge $original.Rva -and $rva-$original.Rva -lt $original.Bytes) {
            $remaining=$original.Bytes-($rva-$original.Rva)
            $bound=[Int32][Math]::Min([Int64]$NameCapacity,$remaining)
            $row.Forwarder=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $Geometry -Rva $rva -Capacity $bound
            $separator=$row.Forwarder.LastIndexOf('.')
            if ($separator -le 0 -or $separator -eq $row.Forwarder.Length-1) { throw 'Original export forwarder lacks module/symbol.' }
        }
    }
    for ($index=0;$index -lt $names;$index++) {
        [Int64]$rva=[BitConverter]::ToUInt32($file,$namesOffset+$index*4)
        $ordinal=[Int32][BitConverter]::ToUInt16($file,$ordinalOffset+$index*2)
        if ($ordinal -ge $functions) { throw 'Original export name ordinal index outside complete EAT.' }
        $text=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $Geometry -Rva $rva -Capacity $NameCapacity
        if ($index -ne 0 -and [String]::CompareOrdinal($original.Names[$index-1].Name,$text) -ge 0) {
            throw 'Original export name pointers are not strictly sorted/unique.'
        }
        $original.Names[$index]=[pscustomobject]@{ Name=$text; Rva=$rva; Index=$ordinal; Function=$original.Functions[$ordinal] }
    }
    $original.SourceComplete=$true
    return $original
}

function Read-SlcliInitialWindowsCapturedAscii {
    param([Parameter(Mandatory)][byte[]]$Image,[Parameter(Mandatory)][Int64]$Rva,
        [Parameter(Mandatory)][Int32]$Capacity)
    if ($Rva -lt 0 -or $Capacity -le 0 -or $Rva -gt $Image.Length-$Capacity) { throw 'Actual captured ASCII extent outside image.' }
    $characters=[char[]]::new($Capacity)
    for ($index=0;$index -lt $Capacity;$index++) {
        $value=$Image[$Rva+$index]
        if ($value -eq 0) {
            if ($index -eq 0) { throw 'Actual captured export string empty.' }
            return [String]::new($characters,0,$index)
        }
        if ($value -lt 33 -or $value -gt 126) { throw 'Actual captured export string has invalid/nonselected byte.' }
        $characters[$index]=[char]$value
    }
    throw 'Actual captured export string lacks finite terminator.'
}

function Read-SlcliInitialWindowsMappedExports {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $walk=$Birth.Modules
    if (-not $walk.ImagesCaptured -or $walk.ExportBytesObserved -or $null -ne $Birth.PendingEvent.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event)) { throw 'Actual export bytes require SAME stopped mapped images.' }
    $walk.ExportBefore=Read-SlcliInitialWindowsClock -Owner $Owner
    for ($index=0;$index -lt $walk.ImagesUsed;$index++) {
        $module=$walk.Images[$index];$exports=$module.Source.Exports;$mapping=$module.Mapping
        if ((-not $mapping.CapturedComplete -or -not $mapping.ExpectedComplete) -or -not $exports.SourceComplete -or
            -not [Object]::ReferenceEquals($mapping.Held,$exports.Held)) { throw 'Actual export source/mapped occurrence mismatch.' }
        for ($entry=0;$entry -lt $exports.FunctionCount;$entry++) {
            $source=$exports.Functions[$entry]
            if ($source.EatRva -gt $mapping.Observed.Length-4) { throw 'Actual export EAT slot outside complete observed image.' }
            $row=[pscustomobject]@{ Source=$source; Mapping=$mapping; Slot=($mapping.Base+$source.EatRva);
                Rva=[Int64][BitConverter]::ToUInt32($mapping.Observed,$source.EatRva); Target=[Int64]0; Forwarder=$null }
            $module.ExportRows[$entry]=$row
            if ($row.Rva -ne $source.Rva) { throw 'Actual captured EAT RVA differs from original source.' }
            if ($source.Hole) { continue }
            if ($null -ne $source.Forwarder) {
                $bound=[Int32][Math]::Min([Int64]$exports.NameCapacity,$exports.Bytes-($source.Rva-$exports.Rva))
                $row.Forwarder=Read-SlcliInitialWindowsCapturedAscii -Image $mapping.Observed -Rva $row.Rva -Capacity $bound
                if ($row.Forwarder -cne $source.Forwarder) { throw 'Actual complete forwarder bytes differ from original source.' }
            } else { $row.Target=$mapping.Base+$row.Rva }
        }
    }
    $walk.ExportAfter=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($walk.ExportAfter.Tick -lt $walk.ExportBefore.Tick) { throw 'Actual export processing clock regressed.' }
    $Owner.PriorTick=$walk.ExportAfter.Tick
    $walk.ExportBytesObserved=$true
    return $walk
}
