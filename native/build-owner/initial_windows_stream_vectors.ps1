# Pure extent regression cases used by the actual initial A pipe reader.
# They create no pipe/native success/ROOT. Source only, UNRUN.
function Test-SlcliInitialWindowsStreamVectors {
    foreach ($case in @(@(0,1,[UInt32]1,1),@(0,20000,[UInt32]20000,16384),
        @(10,20,[UInt32]3,3),@(1,16385,[UInt32]::MaxValue,16384))) {
        $actual=Get-SlcliInitialWindowsStreamExtent -Used $case[0] -Capacity $case[1] -Available $case[2]
        if ($actual -ne $case[3]) { throw 'Actual stream extent calculation differs.' }
    }
    foreach ($case in @(@(-1,10,[UInt32]1),@(0,0,[UInt32]1),@(11,10,[UInt32]1),
        @(0,10,[UInt32]0),@(10,10,[UInt32]1),@(1,16384,[UInt32]16384))) {
        $denied=$false
        try { $null=Get-SlcliInitialWindowsStreamExtent -Used $case[0] -Capacity $case[1] -Available $case[2] }
        catch { $denied=$true }
        if (-not $denied) { throw 'Invalid/exhausted actual stream extent accepted.' }
    }
}
