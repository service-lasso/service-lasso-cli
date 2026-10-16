# Synthetic v6 byte-layout regressions only. No PEB, native file/map/source,
# selected installed OS, initial profile or ROOT. All remain UNRUN.
function New-SlcliInitialWindowsApiSetFixture {
    $bytes=[byte[]]::new(256)
    foreach ($pair in @(@(0,[UInt32]6),@(4,[UInt32]256),@(12,[UInt32]1),@(16,[UInt32]28),@(20,[UInt32]92),
        @(24,[UInt32]31),@(32,[UInt32]100),@(36,[UInt32]22),@(40,[UInt32]22),@(44,[UInt32]52),@(48,[UInt32]2),
        @(64,[UInt32]128),@(68,[UInt32]28),@(76,[UInt32]160),@(80,[UInt32]20),@(84,[UInt32]184),@(88,[UInt32]24),@(92,[UInt32]1234))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$bytes,$pair[0],4)
    }
    foreach ($pair in @(@(100,'api-ms-test'),@(128,'kernelbase.dll'),@(160,'client.exe'),@(184,'kernel32.dll'))) {
        $value=[Text.UnicodeEncoding]::new($false,$false,$true).GetBytes($pair[1])
        [Array]::Copy($value,0,$bytes,$pair[0],$value.Length)
    }
    return ,$bytes
}

function Test-SlcliInitialWindowsApiSetVectors {
    $bytes=New-SlcliInitialWindowsApiSetFixture
    $schema=Read-SlcliInitialWindowsApiSetSchema -Bytes $bytes -EntryCapacity 2 -ValueCapacity 2
    if (-not $schema.Complete -or $schema.Count -ne 1 -or $schema.ValuesUsed -ne 2 -or
        $schema.Entries[0].Name -cne 'api-ms-test' -or $schema.Values[0].Alias -cne '' -or
        $schema.Values[0].Host -cne 'kernelbase.dll' -or $schema.Values[1].Alias -cne 'client.exe' -or
        $schema.Values[1].Host -cne 'kernel32.dll') { throw 'Synthetic complete v6 namespace layout differs.' }
    foreach ($change in @(@(0,[UInt32]4),@(4,[UInt32]255),@(8,[UInt32]4),@(12,[UInt32]3),
        @(16,[UInt32]250),@(32,[UInt32]255),@(36,[UInt32]21),@(40,[UInt32]24),@(48,[UInt32]3),
        @(52,[UInt32]1),@(80,[UInt32]0),@(96,[UInt32]1))) {
        $candidate=New-SlcliInitialWindowsApiSetFixture
        [Array]::Copy([BitConverter]::GetBytes($change[1]),0,$candidate,$change[0],4)
        $denied=$false
        try { $null=Read-SlcliInitialWindowsApiSetSchema -Bytes $candidate -EntryCapacity 2 -ValueCapacity 2 }
        catch { $denied=$true }
        if (-not $denied) { throw 'Unknown/overflow/ambiguous synthetic v6 namespace accepted.' }
    }
    foreach ($kind in @('invalid-surrogate','embedded-null')) {
        $candidate=New-SlcliInitialWindowsApiSetFixture
        if ($kind -ceq 'invalid-surrogate') { $candidate[128]=0;$candidate[129]=216 }
        else { $candidate[128]=0;$candidate[129]=0 }
        $denied=$false
        try { $null=Read-SlcliInitialWindowsApiSetSchema -Bytes $candidate -EntryCapacity 2 -ValueCapacity 2 }
        catch { $denied=$true }
        if (-not $denied) { throw 'Malformed synthetic counted UTF16 namespace accepted.' }
    }
}
