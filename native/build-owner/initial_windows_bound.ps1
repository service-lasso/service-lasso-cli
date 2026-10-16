# Original PE bound import metadata. A timestamp/name is never accepted as
# actual loader/source resolution, and no library is opened by these names.
function Read-SlcliInitialWindowsBoundImports {
    param([Parameter(Mandatory)][object]$Imports)
    if (-not $Imports.OrdinaryComplete) { throw 'Complete held import source required before bound metadata.' }
    $original=[pscustomobject]@{ Imports=$Imports; Rows=[object[]]::new($Imports.Capacity);
        References=[object[]]::new($Imports.Capacity); Used=0; ReferencesUsed=0; Complete=$false }
    if ($Imports.BoundBytes -eq 0) { $original.Complete=$true;return $original }
    $g=$Imports.Geometry;$file=$Imports.Held.ManagedBody
    $directory=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva $Imports.BoundRva -Bytes $Imports.BoundBytes
    [Int64]$cursor=0;$terminated=$false
    while ($cursor -lt $Imports.BoundBytes) {
        if ($Imports.BoundBytes-$cursor -lt 8 -or $original.Used -ge $original.Rows.Length) {
            throw 'Original complete bound descriptor extent/capacity exhausted.'
        }
        $at=$directory+$cursor
        $row=[pscustomobject]@{ Offset=$cursor; Stamp=[BitConverter]::ToUInt32($file,$at);
            NameOffset=[Int32][BitConverter]::ToUInt16($file,$at+4);
            ForwarderCount=[Int32][BitConverter]::ToUInt16($file,$at+6); Name=$null; FirstReference=$original.ReferencesUsed }
        $original.Rows[$original.Used]=$row;$original.Used++
        if ($row.Stamp -eq 0 -and $row.NameOffset -eq 0 -and $row.ForwarderCount -eq 0) { $terminated=$true;break }
        if ($row.NameOffset -lt 8 -or $row.NameOffset -ge $Imports.BoundBytes -or
            $row.ForwarderCount -gt $original.References.Length-$original.ReferencesUsed -or
            [Int64]$row.ForwarderCount*8 -gt $Imports.BoundBytes-$cursor-8) {
            throw 'Original bound name/reference extent invalid.'
        }
        $bound=[Int32][Math]::Min([Int64]$Imports.NameCapacity,$Imports.BoundBytes-$row.NameOffset)
        $row.Name=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $g -Rva ($Imports.BoundRva+$row.NameOffset) -Capacity $bound
        for ($index=0;$index -lt $row.ForwarderCount;$index++) {
            $refAt=$at+8+$index*8
            $reference=[pscustomobject]@{ Descriptor=$row; Offset=($cursor+8+$index*8);
                Stamp=[BitConverter]::ToUInt32($file,$refAt); NameOffset=[Int32][BitConverter]::ToUInt16($file,$refAt+4);
                Reserved=[BitConverter]::ToUInt16($file,$refAt+6); Name=$null }
            $original.References[$original.ReferencesUsed]=$reference;$original.ReferencesUsed++
            if ($reference.Reserved -ne 0 -or $reference.NameOffset -lt 8 -or $reference.NameOffset -ge $Imports.BoundBytes) {
                throw 'Original bound forwarder name/reserved field invalid.'
            }
            $bound=[Int32][Math]::Min([Int64]$Imports.NameCapacity,$Imports.BoundBytes-$reference.NameOffset)
            $reference.Name=Read-SlcliInitialWindowsImportAscii -File $file -Geometry $g -Rva ($Imports.BoundRva+$reference.NameOffset) -Capacity $bound
        }
        $cursor+=8+[Int64]$row.ForwarderCount*8
    }
    if (-not $terminated) { throw 'Original complete bound directory lacks null descriptor.' }
    $original.Complete=$true
    return $original
}

function Read-SlcliInitialWindowsMappedBoundImports {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $walk=$Birth.Modules
    if (-not $walk.ImageBytesComplete -or $walk.BoundBytesObserved -or $null -ne $Birth.PendingEvent.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event)) { throw 'Actual bound bytes require SAME stopped mapped images.' }
    $walk.BoundBefore=Read-SlcliInitialWindowsClock -Owner $Owner
    for ($index=0;$index -lt $walk.ImagesUsed;$index++) {
        $module=$walk.Images[$index];$bound=$module.Source.BoundImports;$imports=$bound.Imports;$mapping=$module.Mapping
        if (-not $mapping.Complete -or -not $bound.Complete -or
            -not [Object]::ReferenceEquals($mapping.Held,$imports.Held)) { throw 'Actual bound source/mapped occurrence mismatch.' }
        # Include the actual null descriptor as a retained source occurrence.
        for ($entry=0;$entry -lt $bound.Used;$entry++) {
            $source=$bound.Rows[$entry];$at=$imports.BoundRva+$source.Offset
            if ($at -gt $mapping.Observed.Length-8) { throw 'Actual bound descriptor outside complete image.' }
            $row=[pscustomobject]@{ Source=$source; Slot=($mapping.Base+$at);
                Stamp=[BitConverter]::ToUInt32($mapping.Observed,$at); NameOffset=[BitConverter]::ToUInt16($mapping.Observed,$at+4);
                ForwarderCount=[BitConverter]::ToUInt16($mapping.Observed,$at+6); Name=$null }
            $module.BoundRows[$entry]=$row
            if ($row.Stamp -ne $source.Stamp -or $row.NameOffset -ne $source.NameOffset -or $row.ForwarderCount -ne $source.ForwarderCount) {
                throw 'Actual complete bound descriptor bytes differ from original source.'
            }
            if ($null -ne $source.Name) {
                $capacity=[Int32][Math]::Min([Int64]$imports.NameCapacity,$imports.BoundBytes-$source.NameOffset)
                $row.Name=Read-SlcliInitialWindowsCapturedAscii -Image $mapping.Observed -Rva ($imports.BoundRva+$source.NameOffset) -Capacity $capacity
                if ($row.Name -cne $source.Name) { throw 'Actual complete bound name differs from original source.' }
            }
        }
        for ($entry=0;$entry -lt $bound.ReferencesUsed;$entry++) {
            $source=$bound.References[$entry];$at=$imports.BoundRva+$source.Offset
            if ($at -gt $mapping.Observed.Length-8) { throw 'Actual bound reference outside complete image.' }
            $row=[pscustomobject]@{ Source=$source; Slot=($mapping.Base+$at);
                Stamp=[BitConverter]::ToUInt32($mapping.Observed,$at); NameOffset=[BitConverter]::ToUInt16($mapping.Observed,$at+4);
                Reserved=[BitConverter]::ToUInt16($mapping.Observed,$at+6); Name=$null }
            $module.BoundReferenceRows[$entry]=$row
            if ($row.Stamp -ne $source.Stamp -or $row.NameOffset -ne $source.NameOffset -or $row.Reserved -ne 0) {
                throw 'Actual complete bound reference bytes differ from original source.'
            }
            $capacity=[Int32][Math]::Min([Int64]$imports.NameCapacity,$imports.BoundBytes-$source.NameOffset)
            $row.Name=Read-SlcliInitialWindowsCapturedAscii -Image $mapping.Observed -Rva ($imports.BoundRva+$source.NameOffset) -Capacity $capacity
            if ($row.Name -cne $source.Name) { throw 'Actual complete bound reference name differs from original source.' }
        }
    }
    $walk.BoundAfter=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($walk.BoundAfter.Tick -lt $walk.BoundBefore.Tick) { throw 'Actual bound metadata processing clock regressed.' }
    $Owner.PriorTick=$walk.BoundAfter.Tick;$walk.BoundBytesObserved=$true
    return $walk
}
