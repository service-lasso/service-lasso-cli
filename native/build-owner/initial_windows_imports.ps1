# Ordinary PE64 import declarations and actual captured slot bytes only.
# Delay/bound/API-set/forwarder/loaded-state policy is not inferred here.
function Read-SlcliInitialWindowsImportAscii {
    param([Parameter(Mandatory)][byte[]]$File,[Parameter(Mandatory)][object]$Geometry,
        [Parameter(Mandatory)][Int64]$Rva,[Parameter(Mandatory)][Int32]$Capacity)
    if ($Rva -le 0 -or $Capacity -le 0) { throw 'Invalid original import ASCII span.' }
    $characters=[char[]]::new($Capacity)
    for ($index=0;$index -lt $Capacity;$index++) {
        $at=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva ($Rva+$index) -Bytes 1
        $value=$File[$at]
        if ($value -eq 0) {
            if ($index -eq 0) { throw 'Empty original import name.' }
            return [String]::new($characters,0,$index)
        }
        if ($value -lt 33 -or $value -gt 126) { throw 'Invalid/nonselected import name byte.' }
        $characters[$index]=[char]$value
    }
    throw 'Original import name exceeds selected finite span or lacks terminator.'
}

function Read-SlcliInitialWindowsOrdinaryImports {
    param([Parameter(Mandatory)][object]$Held,[Parameter(Mandatory)][object]$Geometry,
        [Parameter(Mandatory)][Int32]$Capacity,[Parameter(Mandatory)][Int32]$NameCapacity)
    if (-not $Held.Complete -or $Capacity -le 0 -or $Capacity -eq [Int32]::MaxValue -or
        $NameCapacity -le 0 -or $NameCapacity -gt 32768) { throw 'Invalid finite ordinary import source geometry.' }
    $file=$Held.ManagedBody
    $original=[pscustomobject]@{ Held=$Held; Geometry=$Geometry; Capacity=$Capacity; NameCapacity=$NameCapacity;
        Descriptors=[object[]]::new($Capacity+1); DescriptorsUsed=0; Rows=[object[]]::new($Capacity);
        Used=0; Rva=[Int64]0; Bytes=[Int64]0; DelayRva=[Int64]0; DelayBytes=[Int64]0;
        BoundRva=[Int64]0; BoundBytes=[Int64]0; OrdinaryComplete=$false }
    if ($Geometry.Directories -gt 1) {
        $original.Rva=[BitConverter]::ToUInt32($file,$Geometry.Optional+120)
        $original.Bytes=[BitConverter]::ToUInt32($file,$Geometry.Optional+124)
    }
    if ($Geometry.Directories -gt 11) {
        $original.BoundRva=[BitConverter]::ToUInt32($file,$Geometry.Optional+200)
        $original.BoundBytes=[BitConverter]::ToUInt32($file,$Geometry.Optional+204)
    }
    if ($Geometry.Directories -gt 13) {
        $original.DelayRva=[BitConverter]::ToUInt32($file,$Geometry.Optional+216)
        $original.DelayBytes=[BitConverter]::ToUInt32($file,$Geometry.Optional+220)
    }
    foreach ($pair in @(@($original.Rva,$original.Bytes),@($original.BoundRva,$original.BoundBytes),@($original.DelayRva,$original.DelayBytes))) {
        if (($pair[0] -eq 0) -ne ($pair[1] -eq 0)) { throw 'Original import directory RVA/size pair differs.' }
        if ($pair[1] -ne 0) { $null=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $pair[0] -Bytes $pair[1] }
    }
    if ($original.Bytes -eq 0) { $original.OrdinaryComplete=$true;return $original }
    $directory=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $original.Rva -Bytes $original.Bytes
    $high=[UInt64]::Parse('9223372036854775808',[Globalization.CultureInfo]::InvariantCulture)
    [Int64]$cursor=0;$terminated=$false
    while ($cursor -lt $original.Bytes) {
        if ($original.Bytes-$cursor -lt 20 -or $original.DescriptorsUsed -ge $original.Descriptors.Length) {
            throw 'Original import descriptor extent/capacity exhausted before terminator.'
        }
        $at=$directory+$cursor
        $descriptor=[pscustomobject]@{ Offset=$cursor; Lookup=[Int64][BitConverter]::ToUInt32($file,$at);
            Stamp=[BitConverter]::ToUInt32($file,$at+4); Forwarder=[BitConverter]::ToUInt32($file,$at+8);
            NameRva=[Int64][BitConverter]::ToUInt32($file,$at+12); Iat=[Int64][BitConverter]::ToUInt32($file,$at+16);
            Name=$null; Thunks=0; Terminator=$null }
        $original.Descriptors[$original.DescriptorsUsed]=$descriptor;$original.DescriptorsUsed++
        if ($descriptor.Lookup -eq 0 -and $descriptor.Stamp -eq 0 -and $descriptor.Forwarder -eq 0 -and
            $descriptor.NameRva -eq 0 -and $descriptor.Iat -eq 0) {
            for ($tail=$cursor+20;$tail -lt $original.Bytes;$tail++) {
                if ($file[$directory+$tail] -ne 0) { throw 'Nonzero source import directory tail after null descriptor.' }
            }
            $terminated=$true;break
        }
        if ($descriptor.NameRva -eq 0 -or $descriptor.Iat -eq 0) { throw 'Original import descriptor name/IAT absent.' }
        $descriptor.Name=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $Geometry -Rva $descriptor.NameRva -Capacity $NameCapacity
        $lookup=if ($descriptor.Lookup -ne 0) {$descriptor.Lookup} else {$descriptor.Iat}
        while ($true) {
            [Int64]$delta=[Int64]$descriptor.Thunks*8
            $lookupRva=$lookup+$delta;$iatRva=$descriptor.Iat+$delta
            $lookupAt=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $lookupRva -Bytes 8
            $iatAt=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $iatRva -Bytes 8
            $value=[BitConverter]::ToUInt64($file,$lookupAt);$sourceIat=[BitConverter]::ToUInt64($file,$iatAt)
            if ($value -eq 0) {
                if ($sourceIat -ne 0) { throw 'Original lookup/IAT terminators differ.' }
                $descriptor.Terminator=[pscustomobject]@{ LookupRva=$lookupRva; IatRva=$iatRva; Lookup=$value; Iat=$sourceIat }
                break
            }
            if ($original.Used -ge $original.Rows.Length) { throw 'Original full import entry capacity exhausted.' }
            $row=[pscustomobject]@{ Descriptor=$descriptor; Index=$descriptor.Thunks; LookupRva=$lookupRva;
                IatRva=$iatRva; Lookup=$value; SourceIat=$sourceIat; ByOrdinal=($value -ge $high);
                Ordinal=[UInt16]0; Hint=[UInt16]0; Name=$null }
            $original.Rows[$original.Used]=$row;$original.Used++;$descriptor.Thunks++
            if ($row.ByOrdinal) {
                $ordinal=[Numerics.BigInteger]$value-$high
                if ($ordinal -gt [UInt16]::MaxValue) { throw 'Reserved PE64 ordinal bits are nonzero.' }
                $row.Ordinal=[UInt16]$ordinal
            } else {
                if ($value -gt [Int32]::MaxValue) { throw 'Reserved PE64 hint/name RVA bits are nonzero.' }
                $hint=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva ([Int64]$value) -Bytes 2
                $row.Hint=[BitConverter]::ToUInt16($file,$hint)
                $row.Name=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $Geometry -Rva ([Int64]$value+2) -Capacity $NameCapacity
            }
        }
        $cursor+=20
    }
    if (-not $terminated) { throw 'Original complete ordinary import directory lacks a null descriptor.' }
    $original.OrdinaryComplete=$true
    return $original
}

function Read-SlcliInitialWindowsMappedImports {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $walk=$Birth.Modules
    if (-not $walk.ImageBytesComplete -or $walk.ImportBytesObserved -or $null -ne $Birth.PendingEvent.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event)) { throw 'Actual import bytes require SAME stopped mapped images.' }
    $walk.ImportBefore=Read-SlcliInitialWindowsClock -Owner $Owner
    for ($index=0;$index -lt $walk.ImagesUsed;$index++) {
        $module=$walk.Images[$index];$imports=$module.Source.Imports;$mapping=$module.Mapping
        if (-not $mapping.Complete -or -not $imports.OrdinaryComplete -or
            -not [Object]::ReferenceEquals($mapping.Held,$imports.Held)) { throw 'Actual import source/mapped occurrence mismatch.' }
        for ($entry=0;$entry -lt $imports.Used;$entry++) {
            $source=$imports.Rows[$entry]
            if ($source.IatRva -gt $mapping.Observed.Length-8) { throw 'Actual source IAT slot outside complete observed image.' }
            $module.ImportRows[$entry]=[pscustomobject]@{ Source=$source; Mapping=$mapping;
                Address=($mapping.Base+$source.IatRva); Actual=[BitConverter]::ToUInt64($mapping.Observed,$source.IatRva);
                ExpectedInitial=[BitConverter]::ToUInt64($mapping.Expected,$source.IatRva) }
            if ($module.ImportRows[$entry].Actual -ne $module.ImportRows[$entry].ExpectedInitial) {
                throw 'Actual initial mapped import slot differs from strict source-relocated image.'
            }
        }
    }
    $walk.ImportAfter=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($walk.ImportAfter.Tick -lt $walk.ImportBefore.Tick) { throw 'Actual import processing clock regressed.' }
    $Owner.PriorTick=$walk.ImportAfter.Tick
    $walk.ImportBytesObserved=$true
    return $walk
}
