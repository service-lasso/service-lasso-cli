# Synthetic byte-only source cases. No native file/loaded state, resolver,
# source owner or ROOT is created. Fixtures remain UNRUN.
function New-SlcliInitialWindowsBoundDelayFixture {
    $bytes=New-SlcliInitialWindowsImportFixture
    foreach ($pair in @(@(288,[UInt32]4608),@(292,[UInt32]96),@(304,[UInt32]4736),@(308,[UInt32]64),
        @(1024,[UInt32]123),@(1032,[UInt32]456),@(1152,[UInt32]1),@(1156,[UInt32]4816),
        @(1160,[UInt32]4840),@(1164,[UInt32]4864),@(1168,[UInt32]4896),@(1172,[UInt32]4928),@(1176,[UInt32]4960))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$bytes,$pair[0],4)
    }
    foreach ($pair in @(@(1028,[UInt16]32),@(1030,[UInt16]1),@(1036,[UInt16]48))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$bytes,$pair[0],2)
    }
    foreach ($pair in @(@(1056,"KERNEL32.dll`0"),@(1072,"NTDLL.dll`0"),@(1232,"USER32.dll`0"),@(1410,"MessageBoxW`0"))) {
        $value=[Text.Encoding]::ASCII.GetBytes($pair[1]);[Array]::Copy($value,0,$bytes,$pair[0],$value.Length)
    }
    foreach ($pair in @(@(1280,[UInt64]5368713620),@(1312,[UInt64]4992),
        @(1344,[UInt64]6442467328),@(1376,[UInt64]5368713620))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$bytes,$pair[0],8)
    }
    return ,$bytes
}

function Test-SlcliInitialWindowsBoundDelayVectors {
    $bytes=New-SlcliInitialWindowsBoundDelayFixture
    $g=Get-SlcliInitialWindowsPeGeometry -Bytes $bytes -PageBytes 4096 -ImageLimit 8192
    $synthetic=[pscustomobject]@{ Complete=$true; ManagedBody=$bytes }
    $imports=Read-SlcliInitialWindowsOrdinaryImports -Held $synthetic -Geometry $g -Capacity 4 -NameCapacity 64
    $bound=Read-SlcliInitialWindowsBoundImports -Imports $imports
    $delay=Read-SlcliInitialWindowsDelayImports -Imports $imports
    if (-not $bound.Complete -or $bound.Used -ne 2 -or $bound.ReferencesUsed -ne 1 -or
        $bound.Rows[0].Name -cne 'KERNEL32.dll' -or $bound.References[0].Name -cne 'NTDLL.dll' -or
        -not $delay.Complete -or $delay.Used -ne 1 -or $delay.Rows[0].Name -cne 'MessageBoxW' -or
        $delay.Rows[0].SourceIat -ne [UInt64]5368713620 -or $delay.Rows[0].Bound -ne [UInt64]6442467328 -or
        $delay.Descriptors[0].Name -cne 'USER32.dll') { throw 'Synthetic bound/delay source correspondence differs.' }
    foreach ($case in @('bound-reserved','bound-outside-name','bound-capacity','delay-legacy','delay-outside-handle',
        'delay-outside-bound','delay-directory-terminator','delay-early-lookup-null','delay-iat-terminator')) {
        $candidate=New-SlcliInitialWindowsBoundDelayFixture;$capacity=4
        switch ($case) {
            'bound-reserved' { $candidate[1038]=1 }
            'bound-outside-name' { [Array]::Copy([BitConverter]::GetBytes([UInt16]95),0,$candidate,1028,2) }
            'bound-capacity' { $capacity=1 }
            'delay-legacy' { [Array]::Clear($candidate,1152,4) }
            'delay-outside-handle' { [Array]::Copy([BitConverter]::GetBytes([UInt32]8192),0,$candidate,1160,4) }
            'delay-outside-bound' { [Array]::Copy([BitConverter]::GetBytes([UInt32]8191),0,$candidate,1172,4) }
            'delay-directory-terminator' { [Array]::Copy([BitConverter]::GetBytes([UInt32]32),0,$candidate,308,4) }
            'delay-early-lookup-null' { [Array]::Clear($candidate,1312,8) }
            'delay-iat-terminator' { $candidate[1288]=1 }
        }
        $fixture=[pscustomobject]@{ Complete=$true; ManagedBody=$candidate };$denied=$false
        try {
            $declarations=Read-SlcliInitialWindowsOrdinaryImports -Held $fixture -Geometry $g -Capacity 4 -NameCapacity 64
            # The bounded ordinary two-import source remains intact; select a
            # separate bound-array capacity only for that targeted source case.
            if ($case -ceq 'bound-capacity') { $declarations.Capacity=$capacity }
            if ($case.StartsWith('bound-',[StringComparison]::Ordinal)) {
                $null=Read-SlcliInitialWindowsBoundImports -Imports $declarations
            } else { $null=Read-SlcliInitialWindowsDelayImports -Imports $declarations }
        } catch { $denied=$true }
        if (-not $denied) { throw ('Malformed synthetic bound/delay source accepted: '+$case) }
    }
}
