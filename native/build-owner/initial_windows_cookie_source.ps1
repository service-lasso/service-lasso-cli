# Source-defined single cookie output slot, not an expected entropy value.
function Assert-SlcliInitialWindowsCookieSourceSpan {
    param([Int64]$Cookie,[Int64]$Rva,[Int64]$Bytes)
    if ($Bytes -lt 0 -or $Rva -lt 0 -or $Rva -gt [Int64]::MaxValue-$Bytes) { throw 'Cookie/source metadata range overflows.' }
    if ($Bytes -ne 0 -and $Cookie -lt $Rva+$Bytes -and $Rva -lt $Cookie+8) {
        throw 'Cookie output overlaps source metadata/another selected effect.'
    }
}

function Read-SlcliInitialWindowsCookieSource {
    param([Parameter(Mandatory)][object]$Source)
    $geometry=$Source.Geometry;$file=$Source.Held.ManagedBody
    $original=[pscustomobject]@{ Source=$Source; DirectoryRva=[Int64]0; DirectoryBytes=[Int64]0;
        Size=[UInt32]0; PointerVa=[UInt64]0; Rva=[Int64]0; Initial=[UInt64]0; Present=$false; Complete=$false }
    if (-not $Source.Held.Complete -or -not $Source.Tls.Complete) { throw 'Complete held source/TLS geometry required before cookie selection.' }
    if ($geometry.Directories -gt 10) {
        $original.DirectoryRva=[BitConverter]::ToUInt32($file,$geometry.Optional+192)
        $original.DirectoryBytes=[BitConverter]::ToUInt32($file,$geometry.Optional+196)
    }
    if (($original.DirectoryRva -eq 0) -ne ($original.DirectoryBytes -eq 0)) { throw 'Source load-config RVA/size pair differs.' }
    if ($original.DirectoryBytes -eq 0) { $original.Complete=$true;return $original }
    $at=Get-SlcliInitialWindowsSourceRva -Geometry $geometry -Rva $original.DirectoryRva -Bytes $original.DirectoryBytes
    if ($original.DirectoryBytes -lt 4) { throw 'Truncated original load-config size.' }
    $original.Size=[BitConverter]::ToUInt32($file,$at)
    if ($original.Size -gt $original.DirectoryBytes -or $original.Size -lt 96) { throw 'Unsupported source load-config cookie extent.' }
    $original.PointerVa=[BitConverter]::ToUInt64($file,$at+88)
    if ($original.PointerVa -eq 0) { $original.Complete=$true;return $original }
    $original.Rva=Get-SlcliInitialWindowsTlsRva -Geometry $geometry -Va $original.PointerVa -Bytes 8
    if ($original.Rva%8 -ne 0 -or $original.Rva -lt $geometry.HeadersBytes) { throw 'Cookie output slot misaligned/inside source headers.' }
    $cookieAt=Get-SlcliInitialWindowsSourceRva -Geometry $geometry -Rva $original.Rva -Bytes 8
    $original.Initial=[BitConverter]::ToUInt64($file,$cookieAt)
    foreach ($section in $geometry.Sections) {
        if ($original.Rva -ge $section.Va -and $original.Rva-$section.Va -le $section.Span-8 -and
            ($section.Flags -band 0x20000000) -ne 0) { throw 'Cookie output overlaps executable source section.' }
    }
    # No cookie output may overwrite another source directory/known effect.
    for ($index=0;$index -lt $geometry.Directories;$index++) {
        if ($index -eq 4) { continue } # Certificate directory is a file offset.
        [Int64]$rva=[BitConverter]::ToUInt32($file,$geometry.Optional+112+$index*8)
        [Int64]$bytes=[BitConverter]::ToUInt32($file,$geometry.Optional+116+$index*8)
        if ($bytes -ne 0 -and $original.Rva -lt $rva+$bytes -and $rva -lt $original.Rva+8) {
            throw 'Cookie output overlaps original source directory metadata.'
        }
    }
    for ($index=0;$index -lt $Source.Imports.Used;$index++) {
        $row=$Source.Imports.Rows[$index]
        foreach ($rva in @($row.IatRva,$row.LookupRva)) {
            if ($original.Rva -lt $rva+8 -and $rva -lt $original.Rva+8) { throw 'Cookie output overlaps import/lookup effect.' }
        }
        if (-not $row.ByOrdinal) {
            Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva ([Int64]$row.Lookup) -Bytes (3+$row.Name.Length)
        }
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $row.Descriptor.NameRva -Bytes (1+$row.Descriptor.Name.Length)
    }
    if ($Source.Tls.Present -and $original.Rva -lt $Source.Tls.IndexRva+4 -and $Source.Tls.IndexRva -lt $original.Rva+8) {
        throw 'Cookie output overlaps TLS index output.'
    }
    if ($Source.Tls.Present) {
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $Source.Tls.TemplateRva -Bytes $Source.Tls.TemplateBytes
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $Source.Tls.CallbackRva -Bytes ([Int64]$Source.Tls.CallbacksUsed*8)
    }
    for ($index=0;$index -lt $Source.Exports.NameCount;$index++) {
        $name=$Source.Exports.Names[$index]
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $name.Rva -Bytes (1+$name.Name.Length)
    }
    if ($Source.Exports.Bytes -ne 0) {
        $exportAt=Get-SlcliInitialWindowsSourceRva -Geometry $geometry -Rva $Source.Exports.Rva -Bytes 40
        $dllName=[Int64][BitConverter]::ToUInt32($file,$exportAt+12)
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $dllName -Bytes (1+$Source.Exports.DllName.Length)
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $Source.Exports.AddressRva -Bytes ([Int64]$Source.Exports.FunctionCount*4)
        $namePointers=[Int64][BitConverter]::ToUInt32($file,$exportAt+32)
        $ordinals=[Int64][BitConverter]::ToUInt32($file,$exportAt+36)
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $namePointers -Bytes ([Int64]$Source.Exports.NameCount*4)
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $ordinals -Bytes ([Int64]$Source.Exports.NameCount*2)
    }
    for ($index=0;$index -lt $Source.DelayImports.DescriptorsUsed;$index++) {
        $descriptor=$Source.DelayImports.Descriptors[$index]
        if ($null -eq $descriptor.Name) { continue }
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $descriptor.Fields[1] -Bytes (1+$descriptor.Name.Length)
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $descriptor.Fields[2] -Bytes 8
    }
    for ($index=0;$index -lt $Source.DelayImports.Used;$index++) {
        $row=$Source.DelayImports.Rows[$index]
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $row.IatRva -Bytes 8
        Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva $row.LookupRva -Bytes 8
        if (-not $row.ByOrdinal) {
            Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva ([Int64]$row.Lookup) -Bytes (3+$row.Name.Length)
        }
        foreach ($field in @(5,6)) {
            if ($row.Descriptor.Fields[$field] -ne 0) {
                Assert-SlcliInitialWindowsCookieSourceSpan -Cookie $original.Rva -Rva ([Int64]$row.Descriptor.Fields[$field]+[Int64]$row.Index*8) -Bytes 8
            }
        }
    }
    $original.Present=$true;$original.Complete=$true
    return $original
}
