# Private initial seed A native owner; SOURCE-UNRUN, not a public module/issuer.
# The independently admitted exact initial engine/source/principal/case/profile
# occurrence precedes ALL statements, including allocations/emission/clock setup.
# Source operands below are constraints under that occurrence, not a JSON grant.
function New-SlcliInitialWindowsOwner {
    [CmdletBinding()]
    param([Parameter(Mandatory)][object]$Source,[Parameter(Mandatory)][object[]]$Registry)
    if ([IntPtr]::Size -ne 8 -or $Source.Platform -cne 'win32-x64' -or
        $Source.NativeRows -isnot [Int32] -or $Source.NativeRows -le 0 -or
        $Source.BufferBytes -isnot [Int32] -or $Source.BufferBytes -le 0 -or
        $Source.Expires -isnot [UInt64] -or $Source.PriorTick -isnot [UInt64] -or
        $Source.Expires -le $Source.PriorTick -or
        $Source.NativeRows -gt [Int32]::MaxValue / 128 -or
        $null -eq $Source.Effective -or $null -eq $Source.Primary -or $null -eq $Source.Compiler -or
        $Registry.Length -ne 1 -or $null -ne $Registry[0]) {
        throw 'Invalid fixed initial source constraints.'
    }
    # Managed allocation/pin/metadata/JIT costs belong to the initial full
    # physical profile. These source counts are NOT reservation authority.
    $owner = [pscustomobject]@{
        Source=$Source; EmissionOriginal=$null; PinOriginals=[Collections.Generic.List[object]]::new(2)
        RowBody=[byte[]]::new($Source.NativeRows * 128); RowBase=[IntPtr]::Zero
        BufferBody=[byte[]]::new($Source.BufferBytes); BufferBase=[IntPtr]::Zero
        Rows=[object[]]::new($Source.NativeRows); RowsUsed=0; BufferUsed=0
        NativeHandles=[object[]]::new($Source.NativeRows); HandlesUsed=0
        Captures=[object[]]::new($Source.NativeRows); CapturesUsed=0
        Files=[Collections.Generic.List[object]]::new($Source.NativeRows); Compiler=$null
        Environment=$null; CompilePlan=$null
        PriorTick=$Source.PriorTick; Expires=$Source.Expires; FirstCause=$null
        Failed=$false; Pending=$false; Native=$null; Process=$null; Thread=$null
        Effective=$null; Primary=$null; InitialClock=$null; InitialGeometry=$null
        Started=$true; Returned=$false
    }
    # The fixed enclosing caller owns this one-slot registry before entry.
    # Install SAME owner before pin/emitter/native effects; it survives throws.
    $Registry[0]=$owner
    foreach ($pair in @(@('RowBody','RowBase'),@('BufferBody','BufferBase'))) {
        $record = [pscustomobject]@{ Body=$owner.($pair[0]); Attempted=$true; Pin=$null; Returned=$false; Address=[IntPtr]::Zero }
        $owner.PinOriginals.Add($record)
        try {
            $record.Pin = [Runtime.InteropServices.GCHandle]::Alloc($record.Body,[Runtime.InteropServices.GCHandleType]::Pinned)
            $record.Returned = $true
            $record.Address = $record.Pin.AddrOfPinnedObject()
            $owner.($pair[1]) = $record.Address
        } catch { $owner.FirstCause=$_; $owner.Failed=$true; throw }
    }
    try {
        $owner.Native = New-SlcliInitialWindowsNative -Owner $owner
        $owner.InitialClock = Read-SlcliInitialWindowsClock -Owner $owner
        $owner.PriorTick = $owner.InitialClock.Tick
        $owner.Process = Invoke-SlcliInitialWindowsCall -Owner $owner -Name 'GetCurrentProcess' -Arguments @()
        $owner.Thread = Invoke-SlcliInitialWindowsCall -Owner $owner -Name 'GetCurrentThread' -Arguments @()
        $null=Read-SlcliInitialWindowsGeometry -Owner $owner
        $null=Read-SlcliInitialWindowsPrincipal -Owner $owner -Expected $Source.Effective -Plane Effective
        $null=Read-SlcliInitialWindowsPrincipal -Owner $owner -Expected $Source.Primary -Plane Primary
        $owner.Compiler=Read-SlcliInitialWindowsHeldFile -Owner $owner -Member $Source.Compiler
        $owner.Environment=New-SlcliInitialWindowsEnvironment -Owner $owner
        $null=New-SlcliInitialWindowsCompilePlan -Owner $owner
        $owner.Returned=$true
        return $owner
    } catch { if ($null -eq $owner.FirstCause) { $owner.FirstCause=$_ }; $owner.Failed=$true; throw }
}

function Get-SlcliInitialWindowsBuffer {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][Int32]$Bytes,[Int32]$Alignment=16)
    if ($Owner.Failed -or $Bytes -le 0 -or $Alignment -le 0 -or
        ($Alignment -band ($Alignment-1)) -ne 0) { throw 'Invalid initial native buffer request.' }
    [Int64]$start = ([Int64]$Owner.BufferUsed + $Alignment - 1) -band (-[Int64]$Alignment)
    if ($start -gt $Owner.BufferBody.Length - $Bytes) { $Owner.Failed=$true; throw 'Initial native buffer profile exhausted.' }
    $Owner.BufferUsed = [Int32]($start+$Bytes)
    return [pscustomobject]@{ Pointer=[IntPtr]::Add($Owner.BufferBase,[Int32]$start); Bytes=$Bytes; Offset=$start }
}

function Invoke-SlcliInitialWindowsRaw {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][string]$Name,
        [Parameter(Mandatory)][AllowEmptyCollection()][object[]]$Arguments,[object]$BeforeOriginal=$null)
    if ($Owner.Failed -or $Owner.RowsUsed -ge $Owner.Rows.Length -or
        -not $Owner.Native.Returned -or -not $Owner.Native.Methods.ContainsKey($Name)) {
        throw 'Initial native operation unavailable or profile exhausted.'
    }
    $method = $Owner.Native.Methods[$Name]
    $parameters = $method.GetParameters()
    if ($parameters.Length -ne $Arguments.Length+1) { throw 'Fixed native operand shape changed.' }
    $ordinal = $Owner.RowsUsed; $Owner.RowsUsed++
    $pointer = [IntPtr]::Add($Owner.RowBase,$ordinal*128)
    # Install before any reflection/native entry. A thrown Invoke can leave
    # entry uncertain; absent marker never establishes an absent native effect.
    $row = [pscustomobject]@{ Ordinal=$ordinal; Name=$Name; Pointer=$pointer; Arguments=$Arguments;
        InvocationArguments=[object[]]::new($parameters.Length); Attempted=$true; Returned=$false;
        Cause=$null; Called=0; NativeReturned=0; Raw=[Int64]0; CachedError=0; CacheObserved=0;
        Before=$BeforeOriginal; After=$null }
    $Owner.Rows[$ordinal]=$row
    $row.InvocationArguments[0]=$pointer
    for ($index=0;$index -lt $Arguments.Length;$index++) {
        if ($null -eq $Arguments[$index] -or $Arguments[$index].GetType() -ne $parameters[$index+1].ParameterType) {
            $row.Cause='Wrong exact native operand type'; $Owner.Failed=$true; throw $row.Cause
        }
        $row.InvocationArguments[$index+1]=$Arguments[$index]
    }
    try { $null=$method.Invoke($null,$row.InvocationArguments); $row.Returned=$true }
    catch { $row.Cause=$_; if ($null -eq $Owner.FirstCause) {$Owner.FirstCause=$_}; $Owner.Failed=$true; $Owner.Pending=$true }
    # Native stub stores these before returning, before postclock or any error
    # interpretation. The pinned original backing stays reachable after failure.
    $row.Called=[Runtime.InteropServices.Marshal]::ReadInt32($pointer,0)
    $row.NativeReturned=[Runtime.InteropServices.Marshal]::ReadInt32($pointer,4)
    $row.Raw=[Runtime.InteropServices.Marshal]::ReadInt64($pointer,8)
    $row.CachedError=[Runtime.InteropServices.Marshal]::ReadInt32($pointer,16)
    $row.CacheObserved=[Runtime.InteropServices.Marshal]::ReadInt32($pointer,20)
    if (-not $row.Returned -or $row.Called -ne 1 -or $row.NativeReturned -ne 1) {
        $Owner.Failed=$true; $Owner.Pending=$true; throw 'Initial native entry unresolved.'
    }
    return $row
}

function Read-SlcliInitialWindowsClock {
    param([Parameter(Mandatory)][object]$Owner)
    $counter=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 8 -Alignment 8
    $frequency=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 8 -Alignment 8
    $original=[pscustomobject]@{ Counter=$counter; Frequency=$frequency; CounterCall=$null; FrequencyCall=$null; Tick=[UInt64]0 }
    $original.CounterCall=Invoke-SlcliInitialWindowsRaw -Owner $Owner -Name 'QueryPerformanceCounter' -Arguments @($counter.Pointer)
    $original.FrequencyCall=Invoke-SlcliInitialWindowsRaw -Owner $Owner -Name 'QueryPerformanceFrequency' -Arguments @($frequency.Pointer)
    $raw=[Runtime.InteropServices.Marshal]::ReadInt64($counter.Pointer)
    $rate=[Runtime.InteropServices.Marshal]::ReadInt64($frequency.Pointer)
    if ($original.CounterCall.Raw -eq 0 -or $original.FrequencyCall.Raw -eq 0 -or $raw -lt 0 -or $rate -le 0) {
        $Owner.Failed=$true; throw 'Initial native clock failed.'
    }
    $tick=[Numerics.BigInteger]::Divide(([Numerics.BigInteger]$raw * 1000000000),[Numerics.BigInteger]$rate)
    if ($tick -le 0 -or $tick -gt [UInt64]::MaxValue -or $tick -lt $Owner.PriorTick -or $tick -ge $Owner.Expires) {
        $Owner.Failed=$true; throw 'Initial native clock outside original deadline.'
    }
    $original.Tick=[UInt64]$tick
    return $original
}

function Invoke-SlcliInitialWindowsCall {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][string]$Name,
        [Parameter(Mandatory)][AllowEmptyCollection()][object[]]$Arguments)
    $before=Read-SlcliInitialWindowsClock -Owner $Owner
    $row=Invoke-SlcliInitialWindowsRaw -Owner $Owner -Name $Name -Arguments $Arguments -BeforeOriginal $before
    # Raw native result/backing above survives even a thrown/late postclock.
    try {
        $row.After=Read-SlcliInitialWindowsClock -Owner $Owner
        if ($row.After.Tick -lt $before.Tick) { throw 'Initial native clock regressed.' }
        $Owner.PriorTick=$row.After.Tick
    } catch { if ($null -eq $Owner.FirstCause) {$Owner.FirstCause=$_}; $Owner.Failed=$true; throw }
    return $row
}

function Get-SlcliInitialWindowsText {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][string]$Text)
    if ($Text.Contains([char]0) -or $Text.Length -gt 32766) { throw 'Invalid fixed native text span.' }
    $source=[Text.UnicodeEncoding]::new($false,$false,$true).GetBytes($Text+[char]0)
    $buffer=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes $source.Length -Alignment 2
    [Runtime.InteropServices.Marshal]::Copy($source,0,$buffer.Pointer,$source.Length)
    return [pscustomobject]@{ Source=$source; Native=$buffer; Units=$Text.Length+1; Text=$Text }
}

function Register-SlcliInitialWindowsHandle {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Row,
        [Parameter(Mandatory)][IntPtr]$Handle,[Parameter(Mandatory)][string]$Role)
    if ($Owner.HandlesUsed -ge $Owner.NativeHandles.Length) { $Owner.Failed=$true; throw 'Initial handle roster exhausted.' }
    # Store returned raw reference even when invalid/late; no inferred close or
    # numeric-handle alias coalescing. Each actual native occurrence is distinct.
    $entry=[pscustomobject]@{ Row=$Row; Handle=$Handle; Role=$Role; Close=$null; LastBorrower=$null }
    $Owner.NativeHandles[$Owner.HandlesUsed]=$entry;$Owner.HandlesUsed++
    return $entry
}

function Register-SlcliInitialWindowsCapture {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Original)
    if ($Owner.CapturesUsed -ge $Owner.Captures.Length) { $Owner.Failed=$true;throw 'Initial source capture roster exhausted.' }
    $Owner.Captures[$Owner.CapturesUsed]=$Original;$Owner.CapturesUsed++
}

function New-SlcliInitialWindowsEnvironment {
    param([Parameter(Mandatory)][object]$Owner)
    $entries=$Owner.Source.EnvironmentEntries
    if ($entries -isnot [string[]] -or $entries.Length -eq 0 -or $entries.Length -gt $Owner.Rows.Length) {
        throw 'Initial explicit finite environment absent.'
    }
    $seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($entry in $entries) {
        $equals=$entry.IndexOf('=')
        if ($equals -le 0 -or $entry.Contains([char]0) -or
            $entry.Substring(0,$equals) -cnotmatch '^[A-Za-z_][A-Za-z0-9_]*$' -or
            -not $seen.Add($entry.Substring(0,$equals))) { throw 'Initial fixed environment key invalid/duplicated.' }
    }
    foreach ($name in @('SystemRoot','TEMP','TMP','PATH','INCLUDE','LIB')) {
        if (-not $seen.Contains($name)) { throw 'Initial explicit tool environment incomplete.' }
    }
    $text=[string]::Join([string][char]0,$entries)+[char]0+[char]0
    if ($text.Length -gt 32767) { throw 'Initial complete environment native span overflow.' }
    $bytes=[Text.UnicodeEncoding]::new($false,$false,$true).GetBytes($text)
    $buffer=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes $bytes.Length -Alignment 2
    [Runtime.InteropServices.Marshal]::Copy($bytes,0,$buffer.Pointer,$bytes.Length)
    return [pscustomobject]@{ Entries=$entries; Names=$seen; Body=$bytes; Native=$buffer }
}
