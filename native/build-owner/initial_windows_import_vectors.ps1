# Synthetic source bytes only, not native held files/loaded images/ROOT.
# UNRUN. Complete here labels ordinary parser input, never native authority.
function New-SlcliInitialWindowsImportFixture {
    $base=New-SlcliInitialWindowsPeFixture;$bytes=[byte[]]::new(2048)
    [Array]::Copy($base,0,$bytes,0,$base.Length)
    foreach ($pair in @(@(336,[UInt32]1536),@(344,[UInt32]1536),@(208,[UInt32]4096),@(212,[UInt32]40),
        @(512,[UInt32]4160),@(524,[UInt32]4144),@(528,[UInt32]4192))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$bytes,$pair[0],4)
    }
    [Array]::Copy([Text.Encoding]::ASCII.GetBytes("KERNEL32.dll`0"),0,$bytes,560,13)
    [Array]::Copy([Text.Encoding]::ASCII.GetBytes("GetLastError`0"),0,$bytes,642,13)
    $ordinal=[UInt64]::Parse('9223372036854775815',[Globalization.CultureInfo]::InvariantCulture)
    foreach ($offset in @(576,608)) {
        [Array]::Copy([BitConverter]::GetBytes([UInt64]4224),0,$bytes,$offset,8)
        [Array]::Copy([BitConverter]::GetBytes($ordinal),0,$bytes,$offset+8,8)
    }
    return ,$bytes
}

function Test-SlcliInitialWindowsImportVectors {
    $bytes=New-SlcliInitialWindowsImportFixture
    $geometry=Get-SlcliInitialWindowsPeGeometry -Bytes $bytes -PageBytes 4096 -ImageLimit 8192
    $fixture=[pscustomobject]@{ Complete=$true; ManagedBody=$bytes }
    $imports=Read-SlcliInitialWindowsOrdinaryImports -Held $fixture -Geometry $geometry -Capacity 4 -NameCapacity 64
    if ($imports.Used -ne 2 -or -not $imports.OrdinaryComplete -or
        $imports.Rows[0].Name -cne 'GetLastError' -or $imports.Rows[0].IatRva -ne 4192 -or
        $imports.Rows[0].ByOrdinal -or -not $imports.Rows[1].ByOrdinal -or $imports.Rows[1].Ordinal -ne 7 -or
        $imports.Rows[0].Descriptor.Name -cne 'KERNEL32.dll') { throw 'Synthetic complete ordinary import parse differs.' }
    foreach ($mutation in @('lookup-early-null','reserved-ordinal','reserved-name','unterminated-directory',
        'outside-iat','empty-name','nonascii-name','short-name-capacity','short-row-capacity')) {
        $candidate=New-SlcliInitialWindowsImportFixture;$capacity=4;$names=64
        switch ($mutation) {
            'lookup-early-null' { [Array]::Clear($candidate,576,8) }
            'reserved-ordinal' {
                $value=[UInt64]::Parse('9223372036854841344',[Globalization.CultureInfo]::InvariantCulture)
                [Array]::Copy([BitConverter]::GetBytes($value),0,$candidate,584,8)
            }
            'reserved-name' { [Array]::Copy([BitConverter]::GetBytes([UInt64]2147483648),0,$candidate,576,8) }
            'unterminated-directory' { [Array]::Copy([BitConverter]::GetBytes([UInt32]20),0,$candidate,212,4) }
            'outside-iat' { [Array]::Copy([BitConverter]::GetBytes([UInt32]8191),0,$candidate,528,4) }
            'empty-name' { $candidate[560]=0 }
            'nonascii-name' { $candidate[560]=128 }
            'short-name-capacity' { $names=12 }
            'short-row-capacity' { $capacity=1 }
        }
        $g=Get-SlcliInitialWindowsPeGeometry -Bytes $candidate -PageBytes 4096 -ImageLimit 8192
        $synthetic=[pscustomobject]@{ Complete=$true; ManagedBody=$candidate };$denied=$false
        try { $null=Read-SlcliInitialWindowsOrdinaryImports -Held $synthetic -Geometry $g -Capacity $capacity -NameCapacity $names }
        catch { $denied=$true }
        if (-not $denied) { throw ('Malformed synthetic ordinary import accepted: '+$mutation) }
    }
}
