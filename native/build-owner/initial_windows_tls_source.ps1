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
        Callbacks=[object[]]::new($CallbackCapacity); CallbacksUsed=0; Present=$false; Complete=$false }
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
    if ($original.IndexRva%4 -ne 0) { throw 'Original TLS index is not DWORD aligned.' }
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
    $original.Present=$true;$original.Complete=$true
    return $original
}
