# Fixed actual root-thread TLS relation, SOURCE-UNRUN. Neither native-assigned
# indices nor a template match issue source/ROOT/principal/profile authority.
function New-SlcliInitialWindowsTlsCapture {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $capacity=$Owner.Source.TlsRegionCapacity
    if ($null -ne $Birth.Birth -or $null -ne $Birth.Tls -or -not $Owner.ModuleSources.Complete -or
        $capacity -isnot [Int32] -or $capacity -le 0 -or $capacity -gt [Int32]::MaxValue/64 -or
        $Owner.Source.TlsSlotCapacity -isnot [Int32] -or $Owner.Source.TlsSlotCapacity -le 0) {
        throw 'Complete finite TLS relation backing must precede actual birth.'
    }
    $capture=[pscustomobject]@{ Owner=$Owner; Birth=$Birth; Recipe=$Owner.CompilePlan; Sources=$Owner.ModuleSources;
        Thread=$null; Basic=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 52 -Alignment 8); BasicCall=$null;
        Teb=[Int64]0; VectorBefore=$null; VectorAfter=$null; Rows=[object[]]::new($Owner.ModuleSources.Rows.Length);
        Before=$null; After=$null; AfterThread=$null; Event=$null; NativeBirth=$null; Started=$false; Complete=$false }
    $Birth.Tls=$capture
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $capture
    foreach ($name in @('VectorBefore','VectorAfter')) {
        $capture.$name=[pscustomobject]@{ Kind=$name; Buffer=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 8 -Alignment 8);
            Control=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 64 -Alignment 8); Bytes=8; Managed=[byte[]]::new(8);
            Query=$null; Read=$null; Raw=[UInt64]0; Address=[Int64]0; Allocation=[Int64]0; Received=[Int64]0; Complete=$false }
    }
    for ($index=0;$index -lt $capture.Rows.Length;$index++) {
        $source=$capture.Sources.Rows[$index]
        if (-not $source.Tls.LayoutComplete) { throw 'Complete nonoverlapping source TLS index layout required before native backing.' }
        $row=[pscustomobject]@{ Capture=$capture; Source=$source; Module=$null; IndexBefore=$null; IndexAfter=$null; SlotBefore=$null; SlotAfter=$null;
            Data=$null; Managed=$null; Controls=$null; Regions=$null; Used=0; Cursor=0; Allocation=[Int64]0;
            State='Prebirth'; Complete=$false }
        $capture.Rows[$index]=$row
        if (-not $source.Tls.Present) { continue }
        foreach ($name in @('IndexBefore','IndexAfter','SlotBefore','SlotAfter')) {
            $bytes=if ($name.StartsWith('Index',[StringComparison]::Ordinal)) {4} else {8}
            $row.$name=[pscustomobject]@{ Kind=$name; Buffer=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes $bytes -Alignment 8);
                Control=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes 64 -Alignment 8); Bytes=$bytes; Managed=[byte[]]::new($bytes);
                Query=$null; Read=$null; Raw=[UInt64]0; Address=[Int64]0; Allocation=[Int64]0; Received=[Int64]0; Complete=$false }
        }
        $row.Data=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ([Int32][Math]::Max(1,$source.Tls.TotalBytes))
        $row.Managed=[byte[]]::new([Int32]$source.Tls.TotalBytes)
        $row.Controls=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ($capacity*64) -Alignment 8
        $row.Regions=[object[]]::new($capacity)
        for ($region=0;$region -lt $capacity;$region++) {
            $row.Regions[$region]=[pscustomobject]@{ Kind='Data'; Buffer=$null; Control=[pscustomobject]@{Pointer=[IntPtr]::Add($row.Controls.Pointer,$region*64)};
                Bytes=0; Managed=$null; Query=$null; Read=$null; Raw=[UInt64]0; Address=[Int64]0;
                Allocation=[Int64]0; Received=[Int64]0; Offset=0; Complete=$false }
        }
    }
    return $capture
}

function Read-SlcliInitialWindowsTlsSlice {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Capture,
        [Parameter(Mandatory)][object]$Record,[object]$Row=$null)
    if ($null -ne $Record.Query -or $null -ne $Record.Read -or $Record.Complete) { throw 'TLS native slice already consumed.' }
    $isVector=([Object]::ReferenceEquals($Record,$Capture.VectorBefore) -or [Object]::ReferenceEquals($Record,$Capture.VectorAfter))
    $isIndex=$false;$isSlot=$false;$isData=$false
    if ($null -ne $Row) {
        $owned=$false
        foreach ($original in $Capture.Rows) { if ([Object]::ReferenceEquals($original,$Row)) { $owned=$true;break } }
        if (-not $owned -or $null -eq $Row.Module) { throw 'TLS slice has no SAME actual module/source row.' }
        $isIndex=([Object]::ReferenceEquals($Record,$Row.IndexBefore) -or [Object]::ReferenceEquals($Record,$Row.IndexAfter))
        $isSlot=([Object]::ReferenceEquals($Record,$Row.SlotBefore) -or [Object]::ReferenceEquals($Record,$Row.SlotAfter))
        for ($index=0;$index -lt $Row.Used;$index++) { if ([Object]::ReferenceEquals($Record,$Row.Regions[$index])) { $isData=$true;break } }
    }
    if (([Int32]$isVector+[Int32]$isIndex+[Int32]$isSlot+[Int32]$isData) -ne 1) { throw 'TLS read is not a fresh fixed owned slice.' }
    if ($isVector) { $Record.Address=$Capture.Teb+0x58 }
    elseif ($isIndex) { $Record.Address=$Row.Module.Base+$Row.Source.Tls.IndexRva }
    elseif ($isSlot) {
        if (-not $Row.IndexBefore.Complete -or $Row.IndexBefore.Raw -ge [UInt64]$Owner.Source.TlsSlotCapacity -or
            $Capture.VectorBefore.Raw -gt [UInt64]$Owner.InitialGeometry.Maximum) { throw 'Actual opaque TLS index/vector exceeds finite original profile.' }
        $Record.Address=[Int64]$Capture.VectorBefore.Raw+[Int64]$Row.IndexBefore.Raw*8
    } else {
        if (-not $Row.SlotBefore.Complete -or $Row.SlotBefore.Raw -gt [UInt64]$Owner.InitialGeometry.Maximum) { throw 'Actual TLS data pointer outside native user geometry.' }
        $Record.Offset=$Row.Cursor;$Record.Address=[Int64]$Row.SlotBefore.Raw+$Record.Offset
    }
    if ($Record.Address -lt $Owner.InitialGeometry.Minimum -or $Record.Address -gt $Owner.InitialGeometry.Maximum) { throw 'Actual TLS slice outside native user geometry.' }
    $at=$Record.Control.Pointer
    $Record.Query=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'VirtualQueryEx' -Arguments @(
        $Capture.Birth.ProcessReference.Handle,[IntPtr]::new($Record.Address),$at,[UIntPtr]::new(48))
    $region=[Runtime.InteropServices.Marshal]::ReadIntPtr($at).ToInt64();$bytes=[Runtime.InteropServices.Marshal]::ReadInt64($at,24)
    $Record.Allocation=[Runtime.InteropServices.Marshal]::ReadIntPtr($at,8).ToInt64()
    $protection=[Runtime.InteropServices.Marshal]::ReadInt32($at,36);$type=[Runtime.InteropServices.Marshal]::ReadInt32($at,40)
    if ($Record.Query.Raw -ne 48 -or $region -le 0 -or $region -gt $Record.Address -or $bytes -le 0 -or
        $bytes -gt $Owner.InitialGeometry.Maximum+1-$region -or $Record.Address-$region -ge $bytes -or
        [Runtime.InteropServices.Marshal]::ReadInt32($at,32) -ne 0x1000 -or ($protection -band 0x100) -ne 0 -or
        ($protection -band 0xff) -notin @(2,4,8,32,64,128) -or $Record.Allocation -le 0 -or $Record.Allocation -gt $Record.Address) {
        throw 'Actual TLS slice unobserved/unreadable/guarded/invalid geometry.'
    }
    if ($isIndex) {
        if ($type -ne 0x1000000 -or $Record.Allocation -ne $Row.Module.Base) { throw 'TLS index is not in SAME actual source image allocation.' }
    } elseif ($type -ne 0x20000) { throw 'Actual TEB/TLS array/block is not selected private native storage.' }
    $available=$bytes-($Record.Address-$region)
    if ($isData) {
        if ($Row.Allocation -eq 0) { $Row.Allocation=$Record.Allocation }
        elseif ($Row.Allocation -ne $Record.Allocation) { throw 'Actual TLS block crossed its original allocation.' }
        $Record.Bytes=[Int32][Math]::Min($available,[Int64]$Row.Managed.Length-$Record.Offset)
        $destination=[IntPtr]::Add($Row.Data.Pointer,$Record.Offset)
    } else {
        if ($Record.Bytes -gt $available) { throw 'Fixed TLS slice crosses an unobserved region.' }
        $destination=$Record.Buffer.Pointer
    }
    if ($Record.Bytes -le 0) { throw 'Actual TLS read makes no bounded positive progress.' }
    $Record.Read=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'ReadProcessMemory' -Arguments @(
        $Capture.Birth.ProcessReference.Handle,[IntPtr]::new($Record.Address),$destination,[UIntPtr]::new([UInt64]$Record.Bytes),[IntPtr]::Add($at,48))
    $Record.Received=[Runtime.InteropServices.Marshal]::ReadInt64($at,48)
    if (-not $isData) {
        [Runtime.InteropServices.Marshal]::Copy($Record.Buffer.Pointer,$Record.Managed,0,$Record.Bytes)
        $Record.Raw=if ($isIndex) {[UInt64][BitConverter]::ToUInt32($Record.Managed,0)} else {[BitConverter]::ToUInt64($Record.Managed,0)}
    }
    if ($Record.Read.Raw -eq 0 -or $Record.Received -ne $Record.Bytes) { throw 'Actual TLS native read failed/short; raw prefix retained.' }
    $Record.Complete=$true
}

function Read-SlcliInitialWindowsTlsRelations {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $capture=$Birth.Tls
    if ($null -eq $capture -or $capture.Started -or -not $Birth.Modules.ImagesCaptured -or
        -not $Birth.Cookies.Complete) { throw 'TLS relation lost fresh prebirth owner/complete actual image/creation phase.' }
    $capture.Started=$true
    Assert-SlcliInitialWindowsCreatedCapture -Owner $Owner -Capture $capture
    $capture.Event=$Birth.PendingEvent;$capture.NativeBirth=$Birth.Birth
    $capture.Before=Read-SlcliInitialWindowsClock -Owner $Owner
    $capture.Thread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($capture.Thread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'TLS relation left actual creator thread.' }
    $capture.BasicCall=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'NtQueryInformationThread' -Arguments @(
        $Birth.ThreadReference.Handle,[UInt32]0,$capture.Basic.Pointer,[UInt32]48,[IntPtr]::Add($capture.Basic.Pointer,48))
    $capture.Teb=[Runtime.InteropServices.Marshal]::ReadIntPtr($capture.Basic.Pointer,8).ToInt64()
    if ($capture.BasicCall.Raw -ne 0 -or [Runtime.InteropServices.Marshal]::ReadInt32($capture.Basic.Pointer,48) -ne 48 -or
        [Runtime.InteropServices.Marshal]::ReadIntPtr($capture.Basic.Pointer,16).ToInt64() -ne $Birth.Pid -or
        [Runtime.InteropServices.Marshal]::ReadIntPtr($capture.Basic.Pointer,24).ToInt64() -ne $Birth.Tid -or
        $capture.Teb -ne [Runtime.InteropServices.Marshal]::ReadIntPtr($Birth.PendingEvent.Pointer,56).ToInt64() -or
        $capture.Teb -lt $Owner.InitialGeometry.Minimum -or $capture.Teb -gt $Owner.InitialGeometry.Maximum-0x5f) {
        throw 'Native thread-basic/TEB/event source-layout or actual child identity mismatch.'
    }
    Read-SlcliInitialWindowsTlsSlice -Owner $Owner -Capture $capture -Record $capture.VectorBefore
    if ($capture.VectorBefore.Raw -ne 0 -and ($capture.VectorBefore.Raw%8 -ne 0 -or
        $capture.VectorBefore.Raw -lt [UInt64]$Owner.InitialGeometry.Minimum -or
        $capture.VectorBefore.Raw -gt [UInt64]($Owner.InitialGeometry.Maximum-7))) { throw 'Actual thread TLS array pointer misaligned/outside native user geometry.' }
    foreach ($row in $capture.Rows) {
        if (-not $row.Source.Tls.Present) { $row.State='NoSourceTls';continue }
        for ($index=0;$index -lt $Birth.Modules.ImagesUsed;$index++) {
            $module=$Birth.Modules.Images[$index]
            if ([Object]::ReferenceEquals($module.Source,$row.Source)) {
                if ($null -ne $row.Module) { throw 'Actual TLS source module occurrence ambiguous.' }
                $row.Module=$module
            }
        }
        if ($null -eq $row.Module) { $row.State='NotMapped';continue }
        if ($capture.VectorBefore.Raw -eq 0) { $row.State='NoThreadVector';continue }
        Read-SlcliInitialWindowsTlsSlice -Owner $Owner -Capture $capture -Row $row -Record $row.IndexBefore
        if ($row.IndexBefore.Raw -ne [BitConverter]::ToUInt32($row.Module.Mapping.Observed,$row.Source.Tls.IndexRva)) {
            throw 'Actual TLS index differs from SAME stopped full image capture.'
        }
        Read-SlcliInitialWindowsTlsSlice -Owner $Owner -Capture $capture -Row $row -Record $row.SlotBefore
        if ($row.SlotBefore.Raw -eq 0 -or $row.Source.Tls.TotalBytes -eq 0) { $row.State='NoThreadTemplate';continue }
        $alignment=($row.Source.Tls.Characteristics -shr 20) -band 15
        if ($alignment -ne 0 -and $row.SlotBefore.Raw%([UInt64]1 -shl ($alignment-1)) -ne 0) {
            throw 'Actual TLS block violates its complete source alignment.'
        }
        while ($row.Cursor -lt $row.Managed.Length) {
            if ($row.Used -ge $row.Regions.Length) { throw 'Finite complete TLS block query/read roster exhausted.' }
            $record=$row.Regions[$row.Used];$row.Used++
            Read-SlcliInitialWindowsTlsSlice -Owner $Owner -Capture $capture -Row $row -Record $record
            $row.Cursor+=$record.Bytes
        }
        [Runtime.InteropServices.Marshal]::Copy($row.Data.Pointer,$row.Managed,0,$row.Managed.Length)
        Assert-SlcliInitialWindowsTlsTemplate -Row $row
        Read-SlcliInitialWindowsTlsSlice -Owner $Owner -Capture $capture -Row $row -Record $row.IndexAfter
        Read-SlcliInitialWindowsTlsSlice -Owner $Owner -Capture $capture -Row $row -Record $row.SlotAfter
        if ($row.IndexBefore.Raw -ne $row.IndexAfter.Raw -or $row.SlotBefore.Raw -ne $row.SlotAfter.Raw) {
            throw 'Actual root-thread TLS index/slot changed during independent template relation.'
        }
        $row.State='SourceTemplateMatches';$row.Complete=$true
    }
    Read-SlcliInitialWindowsTlsSlice -Owner $Owner -Capture $capture -Record $capture.VectorAfter
    if ($capture.VectorBefore.Raw -ne $capture.VectorAfter.Raw) { throw 'Actual root-thread TLS array changed during module relations.' }
    $capture.AfterThread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($capture.AfterThread.Raw -ne $capture.Thread.Raw) { throw 'TLS relation left its actual native issuing thread.' }
    Assert-SlcliInitialWindowsCreatedCapture -Owner $Owner -Capture $capture
    $capture.After=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($capture.After.Tick -lt $capture.Before.Tick) { throw 'TLS relation processing clock regressed.' }
    $Owner.PriorTick=$capture.After.Tick;$capture.Complete=$true
    # Missing vector/block is an explicit uninitialized/unknown relation. It
    # cannot explain a changed index or supply an initialized/ready assertion.
    return $capture
}

function Assert-SlcliInitialWindowsTlsTemplate {
    param([Parameter(Mandatory)][object]$Row)
    $tls=$Row.Source.Tls
    if ($Row.Cursor -ne $tls.TotalBytes -or $Row.Managed.Length -ne $tls.TotalBytes -or
        $tls.TemplateBytes -gt $tls.TotalBytes -or $tls.TemplateRva -lt 0 -or
        $tls.TemplateRva -gt $Row.Module.Mapping.Expected.Length-$tls.TemplateBytes) {
        throw 'Incomplete actual TLS block/source-template extent.'
    }
    for ($index=0;$index -lt $Row.Managed.Length;$index++) {
        $expected=if ($index -lt $tls.TemplateBytes) {$Row.Module.Mapping.Expected[$tls.TemplateRva+$index]} else {[byte]0}
        if ($Row.Managed[$index] -ne $expected) { throw 'Actual root-thread TLS block differs from complete source template/zero-fill.' }
    }
}

function Assert-SlcliInitialWindowsTlsIndexOutput {
    param([Parameter(Mandatory)][object]$Mapping)
    $row=$Mapping.TlsIndexOutput
    if ($null -eq $row -or -not $row.Complete -or $row.State -cne 'SourceTemplateMatches' -or
        -not $row.Capture.Complete -or -not [Object]::ReferenceEquals($row.Module.Mapping,$Mapping) -or
        -not [Object]::ReferenceEquals($row.Source.Held,$Mapping.Held) -or
        -not [Object]::ReferenceEquals($row.Capture.Birth.PendingEvent,$row.Capture.Event) -or
        -not [Object]::ReferenceEquals($row.Capture.Birth.Birth,$row.Capture.NativeBirth) -or
        $null -ne $row.Capture.Event.Continue -or $row.Capture.Event.Code -ne 3 -or
        $row.Source.Tls.IndexRva -lt 0 -or $row.Source.Tls.IndexRva -gt $Mapping.Observed.Length-4 -or
        $row.IndexBefore.Raw -ne $row.IndexAfter.Raw -or $row.SlotBefore.Raw -eq 0 -or
        $row.SlotBefore.Raw -ne $row.SlotAfter.Raw -or
        $row.Capture.VectorBefore.Raw -eq 0 -or $row.Capture.VectorBefore.Raw -ne $row.Capture.VectorAfter.Raw -or
        $row.Cursor -ne $row.Source.Tls.TotalBytes -or
        [BitConverter]::ToUInt32($Mapping.Observed,$row.Source.Tls.IndexRva) -ne $row.IndexBefore.Raw) {
        throw 'TLS index span lost SAME actual thread/vector/complete source-template output relation.'
    }
    Assert-SlcliInitialWindowsCreatedCapture -Owner $row.Capture.Owner -Capture $row.Capture
}

function Complete-SlcliInitialWindowsTlsIndexOutputs {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $capture=$Birth.Tls
    if (-not $capture.Complete -or -not [Object]::ReferenceEquals($capture.Owner,$Owner)) { throw 'TLS index outputs require SAME completed root-thread relation.' }
    foreach ($row in $capture.Rows) {
        if (-not $row.Complete) { continue }
        $mapping=$row.Module.Mapping;$slot=$row.Source.Tls.IndexRva
        # A matching unchanged value does not establish that assignment happened.
        # It needs no immutable-byte exception and supplies no TLS-ready claim.
        if ($row.IndexBefore.Raw -eq [BitConverter]::ToUInt32($mapping.Expected,$slot)) { continue }
        if ($null -ne $mapping.TlsIndexOutput -or $slot -lt 0 -or $slot -gt $mapping.Observed.Length-4) { throw 'TLS index output reused/outside actual image.' }
        for ($offset=0;$offset -lt 4;$offset++) {
            $bit=$slot+$offset;$cell=[Int32][Math]::Floor($bit/8);$mask=1 -shl ($bit%8)
            if (($mapping.RelocationSeen[$cell] -band $mask) -ne 0 -or ($mapping.BindingSeen[$cell] -band $mask) -ne 0) {
                throw 'TLS assigned index overlaps relocation/import binding effect.'
            }
            if ($null -ne $mapping.CookieOutput -and $bit -ge $mapping.CookieOutput.Source.Rva -and
                $bit-$mapping.CookieOutput.Source.Rva -lt 8) { throw 'TLS assigned index overlaps typed cookie output.' }
        }
        $mapping.TlsIndexOutput=$row
        Assert-SlcliInitialWindowsTlsIndexOutput -Mapping $mapping
    }
}
