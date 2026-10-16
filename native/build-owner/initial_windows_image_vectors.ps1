# Synthetic PE byte-parser regressions only. No process, native success, loaded
# occurrence, source issuer or ROOT is constructed. UNRUN.
function New-SlcliInitialWindowsPeFixture {
    $bytes=[byte[]]::new(1024);$bytes[0]=77;$bytes[1]=90
    foreach ($pair in @(@(60,[UInt32]64),@(64,[UInt32]0x4550),@(120,[UInt32]4096),
        @(124,[UInt32]512),@(144,[UInt32]8192),@(148,[UInt32]512),@(196,[UInt32]16),
        @(336,[UInt32]1),@(340,[UInt32]4096),@(344,[UInt32]512),@(348,[UInt32]512))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$bytes,$pair[0],4)
    }
    foreach ($pair in @(@(68,[UInt16]0x8664),@(70,[UInt16]1),@(84,[UInt16]240),@(88,[UInt16]0x20b))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$bytes,$pair[0],2)
    }
    [Array]::Copy([BitConverter]::GetBytes([UInt64]0x140000000),0,$bytes,112,8)
    return ,$bytes
}

function Test-SlcliInitialWindowsPeVectors {
    $bytes=New-SlcliInitialWindowsPeFixture
    $g=Get-SlcliInitialWindowsPeGeometry -Bytes $bytes -PageBytes 4096 -ImageLimit 8192
    if ($g.ImageBytes -ne 8192 -or $g.Sections.Length -ne 1 -or
        (Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva 4096 -Bytes 512) -ne 512 -or
        (Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva 0 -Bytes 512) -ne 0) {
        throw 'Complete synthetic PE source span differs.'
    }
    foreach ($span in @(@(4096,513),@(8192,1),@(512,1),@(-1,1),@(0,0))) {
        $denied=$false
        try { $null=Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva $span[0] -Bytes $span[1] }
        catch { $denied=$true }
        if (-not $denied) { throw 'Synthetic uncovered/outside RVA extent accepted.' }
    }
    # Distinct concrete header/section extent failures, not an authority fixture.
    foreach ($change in @(@(0,[UInt32]0),@(60,[UInt32]1024),@(120,[UInt32]3),@(124,[UInt32]256),
        @(144,[UInt32]8193),@(148,[UInt32]1024),@(196,[UInt32]17),@(340,[UInt32]8192),
        @(344,[UInt32]1024),@(348,[UInt32]513))) {
        $candidate=New-SlcliInitialWindowsPeFixture
        [Array]::Copy([BitConverter]::GetBytes($change[1]),0,$candidate,$change[0],4)
        $denied=$false
        try { $null=Get-SlcliInitialWindowsPeGeometry -Bytes $candidate -PageBytes 4096 -ImageLimit 8192 }
        catch { $denied=$true }
        if (-not $denied) { throw 'Synthetic malformed PE extent accepted.' }
    }
    $denied=$false
    try { $null=Get-SlcliInitialWindowsPeGeometry -Bytes $bytes -PageBytes 4096 -ImageLimit 4096 }
    catch { $denied=$true }
    if (-not $denied) { throw 'Synthetic image exceeded original finite capacity.' }
}

function Test-SlcliInitialWindowsFinalImageVectors {
    # Algorithm-only synthetic bytes. This creates neither native capture nor
    # a source/effect issuer; the actual final caller still owns all native cuts.
    $fixture=[pscustomobject]@{ Geometry=[pscustomobject]@{ImageBytes=32};
        Expected=[byte[]]::new(32); Observed=[byte[]]::new(32); Compared=0; CookieOutput=$null }
    if ((Find-SlcliInitialWindowsImageDifference -Original $fixture) -ne 32) { throw 'Synthetic equal full image differs.' }
    foreach ($offset in @(0,7,8,15,16,24,31)) {
        $fixture.Observed[$offset]=1
        if ((Find-SlcliInitialWindowsImageDifference -Original $fixture) -ne $offset) {
            throw 'Unexplained synthetic header/IAT-adjacent/data/tail byte was ignored.'
        }
        $fixture.Observed[$offset]=0
    }
    $fixture.Observed=[byte[]]::new(31)
    $denied=$false
    try { $null=Find-SlcliInitialWindowsImageDifference -Original $fixture } catch { $denied=$true }
    if (-not $denied) { throw 'Short synthetic captured vector accepted.' }
}
