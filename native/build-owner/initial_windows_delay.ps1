# Selected modern PE64 RVA-based delay declarations. No delay helper, library
# load, API-set resolver, binding acceptance or slot modification is performed.
function Read-SlcliInitialWindowsDelayImports {
    param([Parameter(Mandatory)][object]$Imports)
    if (-not $Imports.OrdinaryComplete) { throw 'Complete held import source required before delay metadata.' }
    $original=[pscustomobject]@{ Imports=$Imports; Descriptors=[object[]]::new($Imports.Capacity+1);
        Rows=[object[]]::new($Imports.Capacity); Used=0; DescriptorsUsed=0; Complete=$false }
    if ($Imports.DelayBytes -eq 0) { $original.Complete=$true;return $original }
    $g=$Imports.Geometry;$file=$Imports.Held.ManagedBody
    $directory=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva $Imports.DelayRva -Bytes $Imports.DelayBytes
    $high=[UInt64]::Parse('9223372036854775808',[Globalization.CultureInfo]::InvariantCulture)
    [Int64]$cursor=0;$terminated=$false
    while ($cursor -lt $Imports.DelayBytes) {
        if ($Imports.DelayBytes-$cursor -lt 32 -or $original.DescriptorsUsed -ge $original.Descriptors.Length) {
            throw 'Original complete delay descriptor extent/capacity exhausted.'
        }
        $at=$directory+$cursor
        $descriptor=[pscustomobject]@{ Offset=$cursor; Fields=[UInt32[]]::new(8); Name=$null;
            Handle=[UInt64]0; Thunks=0; Terminator=$null }
        $original.Descriptors[$original.DescriptorsUsed]=$descriptor;$original.DescriptorsUsed++
        $zero=$true
        for ($field=0;$field -lt 8;$field++) {
            $descriptor.Fields[$field]=[BitConverter]::ToUInt32($file,$at+$field*4)
            if ($descriptor.Fields[$field] -ne 0) { $zero=$false }
        }
        if ($zero) {
            for ($tail=$cursor+32;$tail -lt $Imports.DelayBytes;$tail++) {
                if ($file[$directory+$tail] -ne 0) { throw 'Nonzero delay directory tail after null descriptor.' }
            }
            $terminated=$true;break
        }
        if ($descriptor.Fields[0] -ne 1 -or $descriptor.Fields[1] -eq 0 -or $descriptor.Fields[2] -eq 0 -or
            $descriptor.Fields[3] -eq 0 -or $descriptor.Fields[4] -eq 0) {
            throw 'Delay descriptor is not the selected complete modern RVA form.'
        }
        $descriptor.Name=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $g -Rva $descriptor.Fields[1] -Capacity $Imports.NameCapacity
        $handle=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva $descriptor.Fields[2] -Bytes 8
        $descriptor.Handle=[BitConverter]::ToUInt64($file,$handle)
        while ($true) {
            [Int64]$delta=[Int64]$descriptor.Thunks*8
            [Int64]$lookupRva=[Int64]$descriptor.Fields[4]+$delta
            [Int64]$iatRva=[Int64]$descriptor.Fields[3]+$delta
            $lookupAt=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva $lookupRva -Bytes 8
            $iatAt=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva $iatRva -Bytes 8
            $lookup=[BitConverter]::ToUInt64($file,$lookupAt);$iat=[BitConverter]::ToUInt64($file,$iatAt)
            $bound=$null;$unload=$null
            if ($descriptor.Fields[5] -ne 0) {
                $offset=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva ([Int64]$descriptor.Fields[5]+$delta) -Bytes 8
                $bound=[BitConverter]::ToUInt64($file,$offset)
            }
            if ($descriptor.Fields[6] -ne 0) {
                $offset=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva ([Int64]$descriptor.Fields[6]+$delta) -Bytes 8
                $unload=[BitConverter]::ToUInt64($file,$offset)
            }
            if ($lookup -eq 0) {
                if ($iat -ne 0 -or ($null -ne $bound -and $bound -ne 0) -or ($null -ne $unload -and $unload -ne 0)) {
                    throw 'Original delay table terminators differ.'
                }
                $descriptor.Terminator=[pscustomobject]@{ Index=$descriptor.Thunks; LookupRva=$lookupRva; IatRva=$iatRva;
                    Lookup=$lookup; Iat=$iat; Bound=$bound; Unload=$unload }
                break
            }
            if ($original.Used -ge $original.Rows.Length) { throw 'Original complete delay import capacity exhausted.' }
            $row=[pscustomobject]@{ Descriptor=$descriptor; Index=$descriptor.Thunks; LookupRva=$lookupRva; IatRva=$iatRva;
                Lookup=$lookup; SourceIat=$iat; Bound=$bound; Unload=$unload; ByOrdinal=($lookup -ge $high);
                Ordinal=[UInt16]0; Hint=[UInt16]0; Name=$null }
            $original.Rows[$original.Used]=$row;$original.Used++;$descriptor.Thunks++
            if ($row.ByOrdinal) {
                $ordinal=[Numerics.BigInteger]$lookup-$high
                if ($ordinal -gt [UInt16]::MaxValue) { throw 'Original delay ordinal reserved bits nonzero.' }
                $row.Ordinal=[UInt16]$ordinal
            } else {
                if ($lookup -gt [Int32]::MaxValue) { throw 'Original delay hint/name reserved bits nonzero.' }
                $hint=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva ([Int64]$lookup) -Bytes 2
                $row.Hint=[BitConverter]::ToUInt16($file,$hint)
                $row.Name=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $g -Rva ([Int64]$lookup+2) -Capacity $Imports.NameCapacity
            }
        }
        $cursor+=32
    }
    if (-not $terminated) { throw 'Original complete delay directory lacks null descriptor.' }
    $original.Complete=$true
    return $original
}

function Read-SlcliInitialWindowsMappedDelayImports {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $walk=$Birth.Modules
    if (-not $walk.ImagesCaptured -or $walk.DelayBytesObserved -or $null -ne $Birth.PendingEvent.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event)) { throw 'Actual delay bytes require SAME stopped mapped images.' }
    $walk.DelayBefore=Read-SlcliInitialWindowsClock -Owner $Owner
    for ($index=0;$index -lt $walk.ImagesUsed;$index++) {
        $module=$walk.Images[$index];$delay=$module.Source.DelayImports;$imports=$delay.Imports;$mapping=$module.Mapping
        if ((-not $mapping.CapturedComplete -or -not $mapping.ExpectedComplete) -or -not $delay.Complete -or
            -not [Object]::ReferenceEquals($mapping.Held,$imports.Held)) { throw 'Actual delay source/mapped occurrence mismatch.' }
        for ($entry=0;$entry -lt $delay.DescriptorsUsed;$entry++) {
            $source=$delay.Descriptors[$entry];$at=$imports.DelayRva+$source.Offset
            if ($at -gt $mapping.Observed.Length-32) { throw 'Actual delay descriptor outside complete image.' }
            $row=[pscustomobject]@{ Source=$source; Slot=($mapping.Base+$at); Fields=[UInt32[]]::new(8);
                Handle=$null; ExpectedHandle=$null; Name=$null; Terminator=$null }
            $module.DelayDescriptors[$entry]=$row
            for ($field=0;$field -lt 8;$field++) {
                $row.Fields[$field]=[BitConverter]::ToUInt32($mapping.Observed,$at+$field*4)
                if ($row.Fields[$field] -ne $source.Fields[$field]) { throw 'Actual complete delay descriptor differs from original source.' }
            }
            if ($null -eq $source.Name) { continue }
            $nameRva=$source.Fields[1]
            $capacity=[Int32][Math]::Min([Int64]$imports.NameCapacity,[Int64]$mapping.Observed.Length-$nameRva)
            $row.Name=Read-SlcliInitialWindowsCapturedAscii -Image $mapping.Observed -Rva $nameRva -Capacity $capacity
            if ($row.Name -cne $source.Name) { throw 'Actual complete delay DLL name differs from original source.' }
            $handle=$source.Fields[2]
            if ($handle -gt $mapping.Observed.Length-8) { throw 'Actual delay module-handle slot outside image.' }
            $row.Handle=[BitConverter]::ToUInt64($mapping.Observed,$handle)
            $row.ExpectedHandle=[BitConverter]::ToUInt64($mapping.Expected,$handle)
            if ($row.Handle -ne $row.ExpectedHandle) { throw 'Actual initial delay handle differs from source-relocated image.' }
            $term=$source.Terminator
            $row.Terminator=[pscustomobject]@{ Source=$term;
                Lookup=[BitConverter]::ToUInt64($mapping.Observed,$term.LookupRva);
                Iat=[BitConverter]::ToUInt64($mapping.Observed,$term.IatRva); Bound=$null; Unload=$null }
            if ($source.Fields[5] -ne 0) { $row.Terminator.Bound=[BitConverter]::ToUInt64($mapping.Observed,[Int64]$source.Fields[5]+[Int64]$term.Index*8) }
            if ($source.Fields[6] -ne 0) { $row.Terminator.Unload=[BitConverter]::ToUInt64($mapping.Observed,[Int64]$source.Fields[6]+[Int64]$term.Index*8) }
            if ($row.Terminator.Lookup -ne 0 -or $row.Terminator.Iat -ne 0 -or
                ($null -ne $row.Terminator.Bound -and $row.Terminator.Bound -ne 0) -or
                ($null -ne $row.Terminator.Unload -and $row.Terminator.Unload -ne 0)) { throw 'Actual complete delay table terminators differ.' }
        }
        for ($entry=0;$entry -lt $delay.Used;$entry++) {
            $source=$delay.Rows[$entry]
            if ($source.IatRva -gt $mapping.Observed.Length-8) { throw 'Actual delay IAT slot outside complete image.' }
            $row=[pscustomobject]@{ Source=$source; Address=($mapping.Base+$source.IatRva);
                Actual=[BitConverter]::ToUInt64($mapping.Observed,$source.IatRva);
                ExpectedInitial=[BitConverter]::ToUInt64($mapping.Expected,$source.IatRva); Bound=$null; Unload=$null }
            $module.DelayRows[$entry]=$row
            if ($row.Actual -ne $row.ExpectedInitial) { throw 'Actual initial delay IAT slot differs from source-relocated image.' }
            foreach ($pair in @(@(5,'Bound'),@(6,'Unload'))) {
                $rva=$source.Descriptor.Fields[$pair[0]]
                if ($rva -ne 0) {
                    [Int64]$slot=[Int64]$rva+[Int64]$source.Index*8
                    if ($slot -gt $mapping.Observed.Length-8) { throw 'Actual optional delay slot outside complete image.' }
                    $row.($pair[1])=[BitConverter]::ToUInt64($mapping.Observed,$slot)
                    if ($row.($pair[1]) -ne [BitConverter]::ToUInt64($mapping.Expected,$slot)) { throw 'Actual optional delay slot differs from initial source-relocated image.' }
                }
            }
        }
    }
    $walk.DelayAfter=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($walk.DelayAfter.Tick -lt $walk.DelayBefore.Tick) { throw 'Actual delay processing clock regressed.' }
    $Owner.PriorTick=$walk.DelayAfter.Tick;$walk.DelayBytesObserved=$true
    return $walk
}
