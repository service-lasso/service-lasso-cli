# Synthetic complete PE export metadata only. No native held/loaded/source
# owner or ROOT; all fixtures remain UNRUN.
function New-SlcliInitialWindowsExportFixture {
    $bytes=New-SlcliInitialWindowsPeFixture
    foreach ($pair in @(@(200,[UInt32]4096),@(204,[UInt32]256),@(524,[UInt32]4200),@(528,[UInt32]1),
        @(532,[UInt32]2),@(536,[UInt32]2),@(540,[UInt32]4160),@(544,[UInt32]4176),@(548,[UInt32]4184),
        @(576,[UInt32]4500),@(580,[UInt32]4300),@(592,[UInt32]4224),@(596,[UInt32]4232))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$bytes,$pair[0],4)
    }
    [Array]::Copy([BitConverter]::GetBytes([UInt16]1),0,$bytes,602,2)
    foreach ($pair in @(@(616,"TEST.dll`0"),@(640,"Alpha`0"),@(648,"Beta`0"),@(716,"OTHER.Symbol`0"))) {
        $value=[Text.Encoding]::ASCII.GetBytes($pair[1]);[Array]::Copy($value,0,$bytes,$pair[0],$value.Length)
    }
    return ,$bytes
}

function Test-SlcliInitialWindowsExportVectors {
    $bytes=New-SlcliInitialWindowsExportFixture
    $g=Get-SlcliInitialWindowsPeGeometry -Bytes $bytes -PageBytes 4096 -ImageLimit 8192
    $fixture=[pscustomobject]@{ Complete=$true; ManagedBody=$bytes }
    $exports=Read-SlcliInitialWindowsExports -Held $fixture -Geometry $g -Capacity 4 -NameCapacity 64
    if (-not $exports.SourceComplete -or $exports.FunctionCount -ne 2 -or $exports.NameCount -ne 2 -or
        $exports.DllName -cne 'TEST.dll' -or $exports.Functions[0].Rva -ne 4500 -or
        $exports.Functions[1].Forwarder -cne 'OTHER.Symbol' -or $exports.Names[0].Name -cne 'Alpha' -or
        $exports.Names[1].Index -ne 1) { throw 'Synthetic full export source geometry differs.' }
    foreach ($change in @(@(204,[UInt32]39),@(532,[UInt32]5),@(540,[UInt32]8191),
        @(576,[UInt32]8192),@(592,[UInt32]4232),@(524,[UInt32]8192))) {
        $candidate=New-SlcliInitialWindowsExportFixture
        [Array]::Copy([BitConverter]::GetBytes($change[1]),0,$candidate,$change[0],4)
        $synthetic=[pscustomobject]@{ Complete=$true; ManagedBody=$candidate };$denied=$false
        try { $null=Read-SlcliInitialWindowsExports -Held $synthetic -Geometry $g -Capacity 4 -NameCapacity 64 }
        catch { $denied=$true }
        if (-not $denied) { throw 'Malformed/outside/unsorted synthetic export source accepted.' }
    }
    $candidate=New-SlcliInitialWindowsExportFixture
    [Array]::Copy([BitConverter]::GetBytes([UInt16]2),0,$candidate,602,2)
    $synthetic=[pscustomobject]@{ Complete=$true; ManagedBody=$candidate };$denied=$false
    try { $null=Read-SlcliInitialWindowsExports -Held $synthetic -Geometry $g -Capacity 4 -NameCapacity 64 }
    catch { $denied=$true }
    if (-not $denied) { throw 'Synthetic export ordinal index outside function table accepted.' }
    $candidate=New-SlcliInitialWindowsExportFixture
    [Array]::Clear($candidate,716,32)
    [Array]::Copy([Text.Encoding]::ASCII.GetBytes("NOSPLIT`0"),0,$candidate,716,8)
    $synthetic=[pscustomobject]@{ Complete=$true; ManagedBody=$candidate };$denied=$false
    try { $null=Read-SlcliInitialWindowsExports -Held $synthetic -Geometry $g -Capacity 4 -NameCapacity 64 }
    catch { $denied=$true }
    if (-not $denied) { throw 'Synthetic forwarder without module/symbol separator accepted.' }
}
