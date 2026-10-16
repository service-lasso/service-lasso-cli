# Source-only PE32+ TLS geometry. No native index/value or issuer is invented.
function Get-SlcliInitialWindowsTlsRva {
    param([Parameter(Mandatory)][object]$Geometry,[Parameter(Mandatory)][UInt64]$Va,[Int64]$Bytes)
    if ($Va -lt $Geometry.Preferred -or $Bytes -lt 0 -or $Va-$Geometry.Preferred -gt $Geometry.ImageBytes -or
        $Bytes -gt $Geometry.ImageBytes-($Va-$Geometry.Preferred)) { throw 'Original TLS VA outside source image.' }
    return [Int64]($Va-$Geometry.Preferred)
}

function Read-SlcliInitialWindowsTlsSource {
    param([Parameter(Mandatory)][object]$Held,[Parameter(Mandatory)][object]$Geometry,
        [Parameter(Mandatory)][Int32]$ByteCapacity,[Parameter(Mandatory)][Int32]$CallbackCapacity)
    if (-not $Held.Complete -or $ByteCapacity -le 0 -or $CallbackCapacity -le 0) { throw 'Finite original TLS source/profile absent.' }
    $file=$Held.ManagedBody
    $original=[pscustomobject]@{ Held=$Held; Geometry=$Geometry; Rva=[Int64]0; Bytes=[Int64]0;
        Fields=[UInt64[]]::new(4); ZeroFill=[UInt32]0; Characteristics=[UInt32]0;
        TemplateRva=[Int64]0; TemplateBytes=[Int64]0; TotalBytes=[Int64]0;
        IndexRva=[Int64]0; InitialIndex=[UInt32]0; CallbackRva=[Int64]0;
        Callbacks=[object[]]::new($CallbackCapacity); CallbacksUsed=0; Present=$false; Complete=$false; LayoutComplete=$false }
    if ($Geometry.Directories -gt 9) {
        $original.Rva=[BitConverter]::ToUInt32($file,$Geometry.Optional+184)
        $original.Bytes=[BitConverter]::ToUInt32($file,$Geometry.Optional+188)
    }
    if (($original.Rva -eq 0) -ne ($original.Bytes -eq 0)) { throw 'Original TLS directory RVA/size pair differs.' }
    if ($original.Bytes -eq 0) { $original.Complete=$true;return $original }
    if ($original.Bytes -ne 40) { throw 'Unsupported incomplete/extended original PE32+ TLS directory.' }
    $at=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $original.Rva -Bytes 40
    for ($index=0;$index -lt 4;$index++) { $original.Fields[$index]=[BitConverter]::ToUInt64($file,$at+$index*8) }
    $original.ZeroFill=[BitConverter]::ToUInt32($file,$at+32)
    $original.Characteristics=[BitConverter]::ToUInt32($file,$at+36)
    $reserved=[UInt32]::MaxValue -bxor [UInt32]0x00f00000
    if (($original.Characteristics -band $reserved) -ne 0 -or
        (($original.Characteristics -shr 20) -band 15) -gt 14 -or $original.Fields[1] -lt $original.Fields[0]) {
        throw 'Original TLS alignment/reserved bits/template order differ.'
    }
    $original.TemplateBytes=[Int64]($original.Fields[1]-$original.Fields[0])
    if ($original.TemplateBytes -gt $ByteCapacity -or $original.ZeroFill -gt $ByteCapacity-$original.TemplateBytes) {
        throw 'Original complete TLS template/zero-fill exceeds finite profile.'
    }
    $original.TotalBytes=$original.TemplateBytes+$original.ZeroFill
    $original.TemplateRva=Get-SlcliInitialWindowsTlsRva -Geometry $Geometry -Va $original.Fields[0] -Bytes $original.TemplateBytes
    $null=Get-SlcliInitialWindowsTlsRva -Geometry $Geometry -Va $original.Fields[1] -Bytes 0
    if ($original.TemplateBytes -ne 0) {
        $null=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $original.TemplateRva -Bytes $original.TemplateBytes
    }
    $original.IndexRva=Get-SlcliInitialWindowsTlsRva -Geometry $Geometry -Va $original.Fields[2] -Bytes 4
    if ($original.IndexRva%4 -ne 0 -or $original.IndexRva -lt $Geometry.HeadersBytes) { throw 'Original TLS index is not aligned ordinary source data.' }
    $indexAt=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $original.IndexRva -Bytes 4
    $original.InitialIndex=[BitConverter]::ToUInt32($file,$indexAt)
    $original.CallbackRva=Get-SlcliInitialWindowsTlsRva -Geometry $Geometry -Va $original.Fields[3] -Bytes 8
    if ($original.CallbackRva%8 -ne 0) { throw 'Original TLS callback array is not pointer aligned.' }
    $terminated=$false
    for ($index=0;$index -lt $CallbackCapacity;$index++) {
        $rva=$original.CallbackRva+[Int64]$index*8
        $callbackAt=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $rva -Bytes 8
        $value=[BitConverter]::ToUInt64($file,$callbackAt)
        $row=[pscustomobject]@{ SlotRva=$rva; Va=$value; TargetRva=[Int64]0; Terminator=($value -eq 0) }
        $original.Callbacks[$index]=$row;$original.CallbacksUsed++
        if ($row.Terminator) { $terminated=$true;break }
        $row.TargetRva=Get-SlcliInitialWindowsTlsRva -Geometry $Geometry -Va $value -Bytes 1
        $null=Get-SlcliInitialWindowsSourceRva -Geometry $Geometry -Rva $row.TargetRva -Bytes 1
        $executable=$false
        foreach ($section in $Geometry.Sections) {
            if ($row.TargetRva -ge $section.Va -and $row.TargetRva-$section.Va -lt $section.Span -and
                ($section.Flags -band 0x20000000) -ne 0) { $executable=$true;break }
        }
        if (-not $executable) { throw 'Original TLS callback target is not in executable source section.' }
    }
    if (-not $terminated) { throw 'Original TLS callback list lacks finite complete terminator.' }
    foreach ($section in $Geometry.Sections) {
        if ($original.IndexRva -ge $section.Va -and $original.IndexRva-$section.Va -le $section.Span-4 -and
            ($section.Flags -band 0x20000000) -ne 0) { throw 'TLS index output overlaps executable source.' }
    }
    for ($index=0;$index -lt $Geometry.Directories;$index++) {
        if ($index -eq 4) { continue }
        [Int64]$rva=[BitConverter]::ToUInt32($file,$Geometry.Optional+112+$index*8)
        [Int64]$bytes=[BitConverter]::ToUInt32($file,$Geometry.Optional+116+$index*8)
        if ($bytes -ne 0 -and $original.IndexRva -lt $rva+$bytes -and $rva -lt $original.IndexRva+4) {
            throw 'TLS index output overlaps original source directory metadata.'
        }
    }
    foreach ($span in @(@($original.TemplateRva,$original.TemplateBytes),@($original.CallbackRva,[Int64]$original.CallbacksUsed*8))) {
        if ($span[1] -ne 0 -and $original.IndexRva -lt $span[0]+$span[1] -and $span[0] -lt $original.IndexRva+4) {
            throw 'TLS index output overlaps its source template/callback metadata.'
        }
    }
    $original.Present=$true;$original.Complete=$true
    return $original
}

function Assert-SlcliInitialWindowsTlsIndexSourceSpan {
    param([Int64]$Index,[Int64]$Rva,[Int64]$Bytes)
    if ($Rva -lt 0 -or $Bytes -lt 0 -or $Rva -gt [Int64]::MaxValue-$Bytes) { throw 'TLS index/source span overflows.' }
    if ($Bytes -ne 0 -and $Index -lt $Rva+$Bytes -and $Rva -lt $Index+4) { throw 'TLS index overlaps source metadata/import/delay effect.' }
}

function Assert-SlcliInitialWindowsTlsSourceLayout {
    param([Parameter(Mandatory)][object]$Source)
    $tls=$Source.Tls
    if (-not $tls.Complete -or $tls.LayoutComplete) { throw 'TLS source layout missing/reused.' }
    if (-not $tls.Present) { $tls.LayoutComplete=$true;return }
    for ($index=0;$index -lt $Source.Imports.Used;$index++) {
        $row=$Source.Imports.Rows[$index]
        foreach ($rva in @($row.IatRva,$row.LookupRva)) { Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $rva -Bytes 8 }
        if (-not $row.ByOrdinal) { Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva ([Int64]$row.Lookup) -Bytes (3+$row.Name.Length) }
    }
    for ($index=0;$index -lt $Source.Imports.DescriptorsUsed;$index++) {
        $descriptor=$Source.Imports.Descriptors[$index]
        if ($null -eq $descriptor.Name) { continue }
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $descriptor.NameRva -Bytes (1+$descriptor.Name.Length)
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $descriptor.Terminator.LookupRva -Bytes 8
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $descriptor.Terminator.IatRva -Bytes 8
    }
    if ($Source.Exports.Bytes -ne 0) {
        $exports=$Source.Exports;$geometry=$Source.Geometry;$file=$Source.Held.ManagedBody
        $at=Get-SlcliInitialWindowsSourceRva -Geometry $geometry -Rva $exports.Rva -Bytes 40
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva ([Int64][BitConverter]::ToUInt32($file,$at+12)) -Bytes (1+$exports.DllName.Length)
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $exports.AddressRva -Bytes ([Int64]$exports.FunctionCount*4)
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva ([Int64][BitConverter]::ToUInt32($file,$at+32)) -Bytes ([Int64]$exports.NameCount*4)
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva ([Int64][BitConverter]::ToUInt32($file,$at+36)) -Bytes ([Int64]$exports.NameCount*2)
        for ($index=0;$index -lt $exports.NameCount;$index++) {
            Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $exports.Names[$index].Rva -Bytes (1+$exports.Names[$index].Name.Length)
        }
    }
    for ($index=0;$index -lt $Source.DelayImports.DescriptorsUsed;$index++) {
        $descriptor=$Source.DelayImports.Descriptors[$index]
        if ($null -eq $descriptor.Name) { continue }
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $descriptor.Fields[1] -Bytes (1+$descriptor.Name.Length)
        Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $descriptor.Fields[2] -Bytes 8
        foreach ($field in @(3,4,5,6)) {
            if ($descriptor.Fields[$field] -ne 0) {
                Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva $descriptor.Fields[$field] -Bytes (([Int64]$descriptor.Thunks+1)*8)
            }
        }
    }
    for ($index=0;$index -lt $Source.DelayImports.Used;$index++) {
        $row=$Source.DelayImports.Rows[$index]
        if (-not $row.ByOrdinal) { Assert-SlcliInitialWindowsTlsIndexSourceSpan -Index $tls.IndexRva -Rva ([Int64]$row.Lookup) -Bytes (3+$row.Name.Length) }
    }
    $tls.LayoutComplete=$true
}
