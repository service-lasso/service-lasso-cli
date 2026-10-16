# Fixed initial A PE64 source/mapping consumer. No compiled B helper is used.
function Read-SlcliInitialWindowsGeometry {
    param([Parameter(Mandatory)][object]$Owner)
    $original=[pscustomobject]@{ Buffer=$null; Call=$null; PageBytes=0; AllocationBytes=0;
        Minimum=[Int64]0; Maximum=[Int64]0; Complete=$false }
    $Owner.InitialGeometry=$original
    $original.Buffer=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 48 -Alignment 8
    $original.Call=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetSystemInfo' -Arguments @($original.Buffer.Pointer)
    $original.PageBytes=[Runtime.InteropServices.Marshal]::ReadInt32($original.Buffer.Pointer,4)
    $original.AllocationBytes=[Runtime.InteropServices.Marshal]::ReadInt32($original.Buffer.Pointer,40)
    $original.Minimum=[Runtime.InteropServices.Marshal]::ReadIntPtr($original.Buffer.Pointer,8).ToInt64()
    $original.Maximum=[Runtime.InteropServices.Marshal]::ReadIntPtr($original.Buffer.Pointer,16).ToInt64()
    if ($original.PageBytes -le 0 -or ($original.PageBytes -band ($original.PageBytes-1)) -ne 0 -or
        $original.AllocationBytes -lt $original.PageBytes -or $original.AllocationBytes%$original.PageBytes -ne 0 -or
        [Runtime.InteropServices.Marshal]::ReadInt16($original.Buffer.Pointer) -ne 9 -or
        $original.Minimum -le 0 -or $original.Maximum -lt $original.Minimum -or $original.Maximum -eq [Int64]::MaxValue -or
        $original.Minimum%$original.PageBytes -ne 0 -or ($original.Maximum+1)%$original.PageBytes -ne 0) {
        throw 'Initial actual native system geometry is not selected AMD64.'
    }
    $original.Complete=$true
    return $original
}

function Get-SlcliInitialWindowsPeGeometry {
    param([Parameter(Mandatory)][byte[]]$Bytes,[Parameter(Mandatory)][Int32]$PageBytes,
        [Parameter(Mandatory)][Int32]$ImageLimit)
    if ($Bytes.Length -lt 64 -or $Bytes[0] -ne 77 -or $Bytes[1] -ne 90 -or $PageBytes -le 0 -or
        ($PageBytes -band ($PageBytes-1)) -ne 0 -or $ImageLimit -le 0) { throw 'Invalid initial PE source geometry.' }
    [Int64]$pe=[BitConverter]::ToUInt32($Bytes,60)
    if ($pe -lt 64 -or $pe -gt $Bytes.Length-24 -or [BitConverter]::ToUInt32($Bytes,$pe) -ne 0x4550 -or
        [BitConverter]::ToUInt16($Bytes,$pe+4) -ne 0x8664) { throw 'Initial source is not PE AMD64.' }
    $count=[Int32][BitConverter]::ToUInt16($Bytes,$pe+6)
    $optional=[Int32][BitConverter]::ToUInt16($Bytes,$pe+20);$at=$pe+24
    if ($count -le 0 -or $count -gt 96 -or $optional -lt 112 -or $optional -gt $Bytes.Length-$at -or
        [BitConverter]::ToUInt16($Bytes,$at) -ne 0x20b) { throw 'Initial complete PE64 optional header missing.' }
    [Int64]$image=[BitConverter]::ToUInt32($Bytes,$at+56)
    [Int64]$headers=[BitConverter]::ToUInt32($Bytes,$at+60)
    [Int64]$sectionAlign=[BitConverter]::ToUInt32($Bytes,$at+32)
    [Int64]$fileAlign=[BitConverter]::ToUInt32($Bytes,$at+36)
    [Int64]$directories=[BitConverter]::ToUInt32($Bytes,$at+108)
    $table=$at+$optional
    if ($image -le 0 -or $image -gt $ImageLimit -or $image%$PageBytes -ne 0 -or
        $sectionAlign -le 0 -or ($sectionAlign -band ($sectionAlign-1)) -ne 0 -or $sectionAlign%$PageBytes -ne 0 -or
        $image%$sectionAlign -ne 0 -or $fileAlign -lt 512 -or $fileAlign -gt 65536 -or
        ($fileAlign -band ($fileAlign-1)) -ne 0 -or $fileAlign -gt $sectionAlign -or
        $directories -gt 16 -or $optional -lt 112+$directories*8 -or $headers -le 0 -or
        $headers -gt $Bytes.Length -or $headers -gt $image -or $headers%$fileAlign -ne 0 -or
        $count -gt ($headers-$table)/40) { throw 'Initial PE alignment/header/image extent invalid.' }
    $sections=[object[]]::new($count)
    for ($index=0;$index -lt $count;$index++) {
        $row=$table+$index*40
        [Int64]$virtual=[BitConverter]::ToUInt32($Bytes,$row+8)
        [Int64]$va=[BitConverter]::ToUInt32($Bytes,$row+12)
        [Int64]$raw=[BitConverter]::ToUInt32($Bytes,$row+16)
        [Int64]$file=[BitConverter]::ToUInt32($Bytes,$row+20)
        $span=[Math]::Max($virtual,$raw)
        if ($va -lt $sectionAlign -or $va%$sectionAlign -ne 0 -or $va -lt $headers -or $va -gt $image -or
            $span -gt $image-$va -or $file -gt $Bytes.Length -or $raw -gt $Bytes.Length-$file -or
            ($raw -ne 0 -and ($file -lt $headers -or $file%$fileAlign -ne 0 -or $raw%$fileAlign -ne 0))) {
            throw 'Initial PE section file/virtual extent invalid.'
        }
        for ($prior=0;$prior -lt $index;$prior++) {
            $old=$sections[$prior]
            if (($span -ne 0 -and $old.Span -ne 0 -and $va -lt $old.Va+$old.Span -and $old.Va -lt $va+$span) -or
                ($raw -ne 0 -and $old.Raw -ne 0 -and $file -lt $old.File+$old.Raw -and $old.File -lt $file+$raw)) {
                throw 'Initial PE overlapping source sections.'
            }
        }
        $name=[byte[]]::new(8);[Array]::Copy($Bytes,$row,$name,0,8)
        $sections[$index]=[pscustomobject]@{ Va=[Int32]$va; Raw=[Int32]$raw; File=[Int32]$file; Span=[Int32]$span;
            Name=$name; Flags=[BitConverter]::ToUInt32($Bytes,$row+36) }
    }
    return [pscustomobject]@{ ImageBytes=[Int32]$image; HeadersBytes=[Int32]$headers; Sections=$sections;
        Optional=[Int32]$at; Directories=[Int32]$directories; Preferred=[BitConverter]::ToUInt64($Bytes,$at+24);
        PageBytes=$PageBytes; Capacity=[Int32]($image/$PageBytes) }
}

function Get-SlcliInitialWindowsSourceRva {
    param([Parameter(Mandatory)][object]$Geometry,[Parameter(Mandatory)][Int64]$Rva,
        [Parameter(Mandatory)][Int64]$Bytes)
    if ($Rva -lt 0 -or $Bytes -le 0 -or $Rva -gt $Geometry.ImageBytes -or $Bytes -gt $Geometry.ImageBytes-$Rva) {
        throw 'Initial source RVA extent outside actual image.'
    }
    if ($Rva -lt $Geometry.HeadersBytes -and $Bytes -le $Geometry.HeadersBytes-$Rva) { return [Int32]$Rva }
    foreach ($section in $Geometry.Sections) {
        if ($Rva -ge $section.Va -and $Rva-$section.Va -le $section.Raw -and
            $Bytes -le $section.Raw-($Rva-$section.Va)) { return [Int32]($section.File+$Rva-$section.Va) }
    }
    throw 'Initial source RVA has no complete original file span.'
}

function New-SlcliInitialWindowsRemoteImage {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    if ($null -ne $Birth.Birth -or $null -ne $Birth.RemoteImage -or -not $Owner.InitialGeometry.Complete -or
        $Owner.Source.CompilerImageLimit -isnot [Int32]) { throw 'Initial remote image preparation must precede birth.' }
    $Birth.RemoteImage=New-SlcliInitialWindowsImageBacking -Owner $Owner -Birth $Birth -Held $Owner.Compiler -ImageLimit $Owner.Source.CompilerImageLimit
    return $Birth.RemoteImage
}

function New-SlcliInitialWindowsImageBacking {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth,
        [Parameter(Mandatory)][object]$Held,[Parameter(Mandatory)][Int32]$ImageLimit)
    if ($null -ne $Birth.Birth -or -not $Held.Complete -or -not $Owner.InitialGeometry.Complete) {
        throw 'Actual selected source image backing must precede birth.'
    }
    $geometry=Get-SlcliInitialWindowsPeGeometry -Bytes $Held.ManagedBody -PageBytes $Owner.InitialGeometry.PageBytes -ImageLimit $ImageLimit
    if ($geometry.Capacity -gt [Int32]::MaxValue/64) { throw 'Initial remote row budget overflows.' }
    $original=[pscustomobject]@{ Birth=$Birth; Held=$Held; Geometry=$geometry; EventImage=$null; Module=$null;
        Body=$null; Controls=$null; Rows=[object[]]::new($geometry.Capacity); Used=0; Base=[Int64]0; Cursor=0;
        Expected=[byte[]]::new($geometry.ImageBytes); Observed=[byte[]]::new($geometry.ImageBytes);
        RelocationSeen=[byte[]]::new([Int32][Math]::Floor(([Int64]$geometry.ImageBytes+7)/8));
        BindingSeen=[byte[]]::new([Int32][Math]::Floor(([Int64]$geometry.ImageBytes+7)/8));
        BindingEffects=[object[]]::new($Owner.Source.ImportCapacity); BindingEffectsUsed=0;
        RelocationCursor=0; Relocations=0; Compared=0; Thread=$null; BeforeCompare=$null; AfterCompare=$null;
        BeforeExpected=$null; AfterExpected=$null; CookieOutput=$null; TlsIndexOutput=$null;
        CapturedComplete=$false; ExpectedComplete=$false; Complete=$false }
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    $original.Body=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes $geometry.ImageBytes
    $original.Controls=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ($geometry.Capacity*64) -Alignment 8
    return $original
}

function Read-SlcliInitialWindowsRemoteImage {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $original=$Birth.RemoteImage;$image=$Birth.EventImage
    if ($null -eq $original -or $null -ne $original.EventImage -or $original.Complete -or -not $image.Complete -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$image.Event) -or $null -ne $image.Event.Continue) {
        throw 'Initial remote image must consume SAME stopped event-file occurrence.'
    }
    $original.EventImage=$image
    return (Read-SlcliInitialWindowsImageRegions -Owner $Owner -Original $original)
}

function Read-SlcliInitialWindowsImageRegions {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Original)
    $original=$Original;$Birth=$original.Birth
    if ($original.CapturedComplete -or $original.Complete -or $original.Used -ne 0 -or $original.Cursor -ne 0 -or
        $null -eq $Birth.PendingEvent -or $null -ne $Birth.PendingEvent.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event) -or -not $Birth.Child.Complete) {
        throw 'Actual selected mapping must precede any native event continuation.'
    }
    if ($null -ne $original.EventImage) {
        if ($null -ne $original.Module -or -not $original.EventImage.Complete -or
            -not [Object]::ReferenceEquals($original.EventImage.Event,$Birth.PendingEvent) -or
            -not [Object]::ReferenceEquals($original.Held,$Owner.Compiler)) { throw 'Actual root image occurrence mismatch.' }
        $original.Base=[Runtime.InteropServices.Marshal]::ReadIntPtr($Birth.PendingEvent.Pointer,40).ToInt64()
    } else {
        $module=$original.Module;$walk=$Birth.Modules;$found=$false
        if ($null -eq $module -or -not $walk.Complete -or -not $module.Path.Complete -or
            -not [Object]::ReferenceEquals($module.Source.Held,$original.Held) -or
            -not [Object]::ReferenceEquals($module.Mapping,$original)) { throw 'Actual module/source mapping occurrence mismatch.' }
        for ($index=0;$index -lt $walk.ImagesUsed;$index++) {
            if ([Object]::ReferenceEquals($walk.Images[$index],$module)) { $found=$true;break }
        }
        if (-not $found -or $module.Region.Query.Raw -ne 48 -or $module.Region.Allocation -ne $module.Base) {
            throw 'Mapped base is not an actual original of this native child walk.'
        }
        $original.Base=$module.Base
    }
    $geometry=$original.Geometry
    if ($original.Base -le 0 -or $original.Base%$geometry.PageBytes -ne 0 -or
        $original.Base -gt [Int64]::MaxValue-$geometry.ImageBytes) { throw 'Initial actual remote image base invalid.' }
    $original.Thread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($original.Thread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'Initial remote capture left native creator thread.' }
    while ($original.Cursor -lt $geometry.ImageBytes) {
        if ($original.Used -ge $original.Rows.Length) { throw 'Initial actual image region roster exhausted.' }
        $control=[IntPtr]::Add($original.Controls.Pointer,$original.Used*64)
        $row=[pscustomobject]@{ Offset=$original.Cursor; Control=$control; Query=$null; Bytes=[Int64]0;
            State=0; Protection=0; Read=$null; Received=[Int64]0 }
        $original.Rows[$original.Used]=$row;$original.Used++
        $address=[IntPtr]::new($original.Base+$row.Offset)
        $row.Query=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'VirtualQueryEx' -Arguments @(
            $Birth.ProcessReference.Handle,$address,$control,[UIntPtr]::new(48))
        $row.Bytes=[Runtime.InteropServices.Marshal]::ReadInt64($control,24)
        $row.State=[Runtime.InteropServices.Marshal]::ReadInt32($control,32)
        $row.Protection=[Runtime.InteropServices.Marshal]::ReadInt32($control,36)
        if ($row.Query.Raw -ne 48 -or [Runtime.InteropServices.Marshal]::ReadIntPtr($control) -ne $address -or
            [Runtime.InteropServices.Marshal]::ReadIntPtr($control,8).ToInt64() -ne $original.Base -or
            $row.Bytes -le 0 -or $row.Bytes -gt $geometry.ImageBytes-$row.Offset -or $row.Bytes%$geometry.PageBytes -ne 0 -or
            [Runtime.InteropServices.Marshal]::ReadInt32($control,40) -ne 0x1000000 -or $row.State -ne 0x1000 -or
            ($row.Protection -band 0x100) -ne 0 -or ($row.Protection -band 0xff) -notin @(2,4,8,32,64,128)) {
            throw 'Initial actual image region incomplete/unreadable/different mapping; no fabricated gap bytes.'
        }
        $row.Read=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'ReadProcessMemory' -Arguments @(
            $Birth.ProcessReference.Handle,$address,[IntPtr]::Add($original.Body.Pointer,$row.Offset),
            [UIntPtr]::new([UInt64]$row.Bytes),[IntPtr]::Add($control,48))
        $row.Received=[Runtime.InteropServices.Marshal]::ReadInt64($control,48)
        if ($row.Read.Raw -eq 0 -or $row.Received -ne $row.Bytes) { throw 'Initial actual complete mapped image read failed/short.' }
        $original.Cursor+=[Int32]$row.Bytes
    }
    [Runtime.InteropServices.Marshal]::Copy($original.Body.Pointer,$original.Observed,0,$geometry.ImageBytes)
    $original.CapturedComplete=$true
    $null=Prepare-SlcliInitialWindowsExpectedImage -Owner $Owner -Original $original
    return $original
}

function Prepare-SlcliInitialWindowsExpectedImage {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Original)
    $g=$Original.Geometry;$file=$Original.Held.ManagedBody
    if (-not $Original.CapturedComplete -or $Original.ExpectedComplete) { throw 'Expected image requires fresh complete capture.' }
    $Original.BeforeExpected=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($g.Preferred -eq 0 -or [Numerics.BigInteger]$g.Preferred+$g.ImageBytes -gt [UInt64]::MaxValue) {
        throw 'Initial preferred image base overflows.'
    }
    [Array]::Copy($file,0,$Original.Expected,0,$g.HeadersBytes)
    foreach ($section in $g.Sections) { [Array]::Copy($file,$section.File,$Original.Expected,$section.Va,$section.Raw) }
    [Int64]$rva=0;[Int64]$bytes=0
    if ($g.Directories -gt 5) {
        $rva=[BitConverter]::ToUInt32($file,$g.Optional+152)
        $bytes=[BitConverter]::ToUInt32($file,$g.Optional+156)
    }
    if (($rva -eq 0) -ne ($bytes -eq 0) -or ($bytes -eq 0 -and $g.Preferred -ne [UInt64]$Original.Base)) {
        throw 'Initial relocation table/base mismatch.'
    }
    $table=if ($bytes -ne 0) { Get-SlcliInitialWindowsSourceRva -Geometry $g -Rva $rva -Bytes $bytes } else {0}
    while ($Original.RelocationCursor -lt $bytes) {
        $cursor=$Original.RelocationCursor
        if ($bytes-$cursor -lt 8) { throw 'Initial relocation block header truncated.' }
        [Int64]$page=[BitConverter]::ToUInt32($file,$table+$cursor)
        [Int64]$block=[BitConverter]::ToUInt32($file,$table+$cursor+4)
        if ($page%4096 -ne 0 -or $block -lt 8 -or $block%2 -ne 0 -or $block -gt $bytes-$cursor) {
            throw 'Initial relocation block extent invalid.'
        }
        for ($at=8;$at -lt $block;$at+=2) {
            $entry=[Int32][BitConverter]::ToUInt16($file,$table+$cursor+$at);$kind=$entry -shr 12
            $Original.Relocations++
            if ($kind -eq 0) { continue }
            $target=$page+($entry -band 4095)
            if ($kind -ne 10 -or $target -gt $g.ImageBytes-8) { throw 'Initial unsupported/outside relocation.' }
            for ($byte=0;$byte -lt 8;$byte++) {
                $bit=$target+$byte;$cell=[Int32][Math]::Floor($bit/8);$mask=1 -shl ($bit%8)
                if (($Original.RelocationSeen[$cell] -band $mask) -ne 0) { throw 'Initial overlapping relocation targets.' }
                $Original.RelocationSeen[$cell]=[byte]($Original.RelocationSeen[$cell] -bor $mask)
            }
            $value=[BitConverter]::ToUInt64($Original.Expected,$target)
            if ($value -lt $g.Preferred -or $value-$g.Preferred -gt $g.ImageBytes) { throw 'Initial relocation source outside image.' }
            $replacement=[UInt64]([Numerics.BigInteger]$Original.Base+([Numerics.BigInteger]$value-$g.Preferred))
            [Array]::Copy([BitConverter]::GetBytes($replacement),0,$Original.Expected,$target,8)
        }
        $Original.RelocationCursor+=[Int32]$block
    }
    $Original.AfterExpected=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($Original.AfterExpected.Tick -lt $Original.BeforeExpected.Tick) { throw 'Initial image expectation clock regressed.' }
    $Owner.PriorTick=$Original.AfterExpected.Tick;$Original.ExpectedComplete=$true
}

function Compare-SlcliInitialWindowsRemoteImage {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Original)
    if (-not $Original.CapturedComplete -or -not $Original.ExpectedComplete -or $Original.Complete) {
        throw 'Final image correspondence requires SAME complete capture and source expectation.'
    }
    $Original.BeforeCompare=Read-SlcliInitialWindowsClock -Owner $Owner
    $null=Find-SlcliInitialWindowsImageDifference -Original $Original
    if ($Original.Compared -ne $Original.Geometry.ImageBytes) {
        throw 'Initial complete source/effect image correspondence differs; unexplained loader bytes are not waived.'
    }
    $Original.AfterCompare=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($Original.AfterCompare.Tick -lt $Original.BeforeCompare.Tick) { throw 'Initial image comparison clock regressed.' }
    $Owner.PriorTick=$Original.AfterCompare.Tick;$Original.Complete=$true
}

function Find-SlcliInitialWindowsImageDifference {
    param([Parameter(Mandatory)][object]$Original)
    if ($Original.Expected.Length -ne $Original.Geometry.ImageBytes -or $Original.Observed.Length -ne $Original.Geometry.ImageBytes) {
        throw 'Full image byte vectors differ from selected geometry.'
    }
    if ($null -ne $Original.CookieOutput) { Assert-SlcliInitialWindowsCookieOutput -Mapping $Original }
    if ($null -ne $Original.TlsIndexOutput) { Assert-SlcliInitialWindowsTlsIndexOutput -Mapping $Original }
    for ($Original.Compared=0;$Original.Compared -lt $Original.Geometry.ImageBytes;$Original.Compared++) {
        if ($null -ne $Original.CookieOutput -and $Original.Compared -ge $Original.CookieOutput.Source.Rva -and
            $Original.Compared-$Original.CookieOutput.Source.Rva -lt 8) {
            continue
        }
        if ($null -ne $Original.TlsIndexOutput -and $Original.Compared -ge $Original.TlsIndexOutput.Source.Tls.IndexRva -and
            $Original.Compared-$Original.TlsIndexOutput.Source.Tls.IndexRva -lt 4) { continue }
        if ($Original.Expected[$Original.Compared] -ne $Original.Observed[$Original.Compared]) {
            return $Original.Compared
        }
    }
    return $Original.Compared
}
