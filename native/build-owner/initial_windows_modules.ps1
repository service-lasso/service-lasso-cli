# Complete finite source file roster and actual stopped-child mapping/path
# observations. These are not the still-required loaded/import/source proof.
function New-SlcliInitialWindowsModulePath {
    param([Parameter(Mandatory)][object]$Owner)
    $units=$Owner.Source.ModulePathUnits
    if ($units -isnot [Int32] -or $units -lt 4 -or $units -gt 32768) { throw 'Invalid finite actual module path capacity.' }
    return [pscustomobject]@{ Units=$units; Buffer=(Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ($units*2) -Alignment 2);
        Call=$null; Text=$null; Complete=$false }
}

function Complete-SlcliInitialWindowsModulePath {
    param([Parameter(Mandatory)][object]$Original)
    if ($null -eq $Original.Call -or $Original.Call.Raw -le 0 -or $Original.Call.Raw -ge $Original.Units) {
        throw 'Actual native module pathname absent/truncated.'
    }
    $Original.Text=[Runtime.InteropServices.Marshal]::PtrToStringUni($Original.Buffer.Pointer,[Int32]$Original.Call.Raw)
    if (-not $Original.Text.StartsWith('\Device\',[StringComparison]::Ordinal) -or
        $Original.Text.IndexOf([char]0) -ge 0 -or $Original.Text.Contains('/')) {
        throw 'Actual native module pathname is not the complete NT namespace form.'
    }
    # Selected source paths are strict UTF16; invalid code units cannot be
    # normalized/replaced to obtain equality with a different native name.
    $null=[Text.UnicodeEncoding]::new($false,$false,$true).GetByteCount($Original.Text)
    $Original.Complete=$true
}

function New-SlcliInitialWindowsModuleSources {
    param([Parameter(Mandatory)][object]$Owner)
    $members=$Owner.Source.ToolModuleMembers
    if ($null -ne $Owner.ModuleSources -or $members -isnot [object[]] -or $members.Length -le 0 -or
        $members.Length -gt $Owner.Rows.Length -or -not [Object]::ReferenceEquals($members[0],$Owner.Source.Compiler) -or
        -not $Owner.Compiler.Complete -or $Owner.Source.ImportCapacity -isnot [Int32] -or
        $Owner.Source.ImportNameCapacity -isnot [Int32] -or $Owner.Source.ExportCapacity -isnot [Int32]) {
        throw 'Initial closed module source roster/actual compiler original absent.'
    }
    $original=[pscustomobject]@{ Members=$members; Rows=[object[]]::new($members.Length); Complete=$false }
    $Owner.ModuleSources=$original
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    for ($index=0;$index -lt $members.Length;$index++) {
        $row=[pscustomobject]@{ Member=$members[$index]; Held=$null; NativePath=$null; Geometry=$null;
            Imports=$null; Exports=$null; BoundImports=$null; DelayImports=$null }
        $original.Rows[$index]=$row
        $row.Held=if ($index -eq 0) {$Owner.Compiler} else {Read-SlcliInitialWindowsHeldFile -Owner $Owner -Member $row.Member}
        $limit=if ($index -eq 0) {$Owner.Source.CompilerImageLimit} else {$Owner.Source.ToolModuleImageLimit}
        $row.Geometry=Get-SlcliInitialWindowsPeGeometry -Bytes $row.Held.ManagedBody -PageBytes $Owner.InitialGeometry.PageBytes -ImageLimit $limit
        $row.Imports=Read-SlcliInitialWindowsOrdinaryImports -Held $row.Held -Geometry $row.Geometry -Capacity $Owner.Source.ImportCapacity -NameCapacity $Owner.Source.ImportNameCapacity
        $row.Exports=Read-SlcliInitialWindowsExports -Held $row.Held -Geometry $row.Geometry -Capacity $Owner.Source.ExportCapacity -NameCapacity $Owner.Source.ImportNameCapacity
        $row.BoundImports=Read-SlcliInitialWindowsBoundImports -Imports $row.Imports
        $row.DelayImports=Read-SlcliInitialWindowsDelayImports -Imports $row.Imports
        $row.NativePath=New-SlcliInitialWindowsModulePath -Owner $Owner
        $row.NativePath.Call=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetFinalPathNameByHandleW' -Arguments @(
            $row.Held.Leaf.Handle,$row.NativePath.Buffer.Pointer,[UInt32]$row.NativePath.Units,[UInt32]2)
        Complete-SlcliInitialWindowsModulePath -Original $row.NativePath
        for ($prior=0;$prior -lt $index;$prior++) {
            if ([String]::Equals($original.Rows[$prior].NativePath.Text,$row.NativePath.Text,[StringComparison]::Ordinal)) {
                throw 'Initial closed module source roster duplicates an actual native pathname.'
            }
        }
    }
    $original.Complete=$true
    return $original
}

function New-SlcliInitialWindowsModuleWalk {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $capacity=$Owner.Source.ModuleScanCapacity
    if ($null -ne $Birth.Birth -or $null -ne $Birth.Modules -or -not $Owner.ModuleSources.Complete -or
        -not $Owner.InitialGeometry.Complete -or $capacity -isnot [Int32] -or $capacity -le 0 -or
        $capacity -gt [Int32]::MaxValue/48 -or $Owner.Source.ToolModuleImageLimit -isnot [Int32]) {
        throw 'Initial complete module walk backing must precede birth.'
    }
    $original=[pscustomobject]@{ Birth=$Birth; Sources=$Owner.ModuleSources; Thread=$null; Controls=$null;
        Rows=[object[]]::new($capacity); Used=0; Cursor=$Owner.InitialGeometry.Minimum;
        Images=[object[]]::new($Owner.ModuleSources.Rows.Length); ImagesUsed=0; Started=$false; Complete=$false;
        SourceMappings=[object[]]::new($Owner.ModuleSources.Rows.Length); ImageBytesComplete=$false;
        ImportBytesObserved=$false; ExportBytesObserved=$false; ImportBefore=$null; ImportAfter=$null;
        ExportBefore=$null; ExportAfter=$null; BoundBefore=$null; BoundAfter=$null; BoundBytesObserved=$false;
        DelayBefore=$null; DelayAfter=$null; DelayBytesObserved=$false;
        RouteBefore=$null; RouteAfter=$null; RouteBytesObserved=$false;
        RouteMatches=[Int64]0; RouteUnresolved=[Int64]0 }
    $Birth.Modules=$original
    Register-SlcliInitialWindowsCapture -Owner $Owner -Original $original
    $original.Controls=Get-SlcliInitialWindowsBuffer -Owner $Owner -Bytes ($capacity*48) -Alignment 8
    for ($index=0;$index -lt $original.Images.Length;$index++) {
        $original.Images[$index]=[pscustomobject]@{ Region=$null; Base=[Int64]0; Path=(New-SlcliInitialWindowsModulePath -Owner $Owner);
            Source=$null; Mapping=$null; ImportRows=[object[]]::new($Owner.Source.ImportCapacity);
            ExportRows=[object[]]::new($Owner.Source.ExportCapacity); ImportRoutes=[object[]]::new($Owner.Source.ImportCapacity);
            BoundRows=[object[]]::new($Owner.Source.ImportCapacity);
            BoundReferenceRows=[object[]]::new($Owner.Source.ImportCapacity); DelayRows=[object[]]::new($Owner.Source.ImportCapacity);
            DelayDescriptors=[object[]]::new($Owner.Source.ImportCapacity+1); Complete=$false }
        $source=$Owner.ModuleSources.Rows[$index]
        $mapping=if ([Object]::ReferenceEquals($source.Held,$Owner.Compiler)) {$Birth.RemoteImage} else {
            New-SlcliInitialWindowsImageBacking -Owner $Owner -Birth $Birth -Held $source.Held -ImageLimit $Owner.Source.ToolModuleImageLimit
        }
        $original.SourceMappings[$index]=[pscustomobject]@{ Source=$source; Mapping=$mapping }
    }
    New-SlcliInitialWindowsImportRoutes -Owner $Owner -Walk $original
    return $original
}

function Read-SlcliInitialWindowsModules {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $original=$Birth.Modules
    if ($null -eq $original -or $original.Started -or -not $Birth.Child.Complete -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event) -or $null -ne $Birth.PendingEvent.Continue) {
        throw 'Initial actual module walk requires SAME stopped root occurrence.'
    }
    $original.Started=$true
    $original.Thread=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'GetCurrentThreadId' -Arguments @()
    if ($original.Thread.Raw -ne $Owner.Primary.ThreadId.Raw) { throw 'Initial module walk left native creator thread.' }
    while ($original.Cursor -le $Owner.InitialGeometry.Maximum) {
        if ($original.Used -ge $original.Rows.Length) { throw 'Initial complete user-address module scan budget exhausted.' }
        $at=[IntPtr]::Add($original.Controls.Pointer,$original.Used*48)
        $region=[pscustomobject]@{ Address=$original.Cursor; Control=$at; Query=$null; Bytes=[Int64]0;
            Allocation=[Int64]0; Type=0; State=0; Image=$null }
        $original.Rows[$original.Used]=$region;$original.Used++
        $region.Query=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'VirtualQueryEx' -Arguments @(
            $Birth.ProcessReference.Handle,[IntPtr]::new($region.Address),$at,[UIntPtr]::new(48))
        $region.Bytes=[Runtime.InteropServices.Marshal]::ReadInt64($at,24)
        $region.Allocation=[Runtime.InteropServices.Marshal]::ReadIntPtr($at,8).ToInt64()
        $region.Type=[Runtime.InteropServices.Marshal]::ReadInt32($at,40)
        $region.State=[Runtime.InteropServices.Marshal]::ReadInt32($at,32)
        if ($region.Query.Raw -ne 48 -or [Runtime.InteropServices.Marshal]::ReadIntPtr($at).ToInt64() -ne $region.Address -or
            $region.Bytes -le 0 -or $region.Bytes -gt $Owner.InitialGeometry.Maximum+1-$region.Address -or
            $region.Bytes%$Owner.InitialGeometry.PageBytes -ne 0 -or $region.State -notin @(0x1000,0x2000,0x10000)) {
            throw 'Initial native user-address scan failed/changed/nonprogressing.'
        }
        if ($region.Type -eq 0x1000000) {
            if ($region.Allocation -lt $Owner.InitialGeometry.Minimum -or $region.Allocation -gt $region.Address -or
                $region.State -eq 0x10000) { throw 'Initial actual image allocation relation invalid.' }
            $seen=$null
            for ($index=0;$index -lt $original.ImagesUsed;$index++) {
                if ($original.Images[$index].Base -eq $region.Allocation) { $seen=$original.Images[$index];break }
            }
            if ($null -eq $seen) {
                if ($region.Allocation -ne $region.Address -or $original.ImagesUsed -ge $original.Images.Length) {
                    throw 'Initial actual image allocation starts outside scan or exceeds closed roster.'
                }
                $seen=$original.Images[$original.ImagesUsed];$original.ImagesUsed++
                $seen.Region=$region;$seen.Base=$region.Allocation;$region.Image=$seen
                $seen.Path.Call=Invoke-SlcliInitialWindowsCall -Owner $Owner -Name 'K32GetMappedFileNameW' -Arguments @(
                    $Birth.ProcessReference.Handle,[IntPtr]::new($seen.Base),$seen.Path.Buffer.Pointer,[UInt32]$seen.Path.Units)
                Complete-SlcliInitialWindowsModulePath -Original $seen.Path
                foreach ($source in $original.Sources.Rows) {
                    if ([String]::Equals($source.NativePath.Text,$seen.Path.Text,[StringComparison]::Ordinal)) {
                        if ($null -ne $seen.Source) { throw 'Initial actual mapped pathname matched multiple held sources.' }
                        $seen.Source=$source
                    }
                }
                if ($null -eq $seen.Source) { throw 'Initial actual mapped image missing from complete selected source roster.' }
                foreach ($sourceMapping in $original.SourceMappings) {
                    if ([Object]::ReferenceEquals($sourceMapping.Source,$seen.Source)) { $seen.Mapping=$sourceMapping.Mapping;break }
                }
                if ($null -eq $seen.Mapping) { throw 'Actual selected module has no prebirth image backing.' }
                for ($index=0;$index -lt $original.ImagesUsed-1;$index++) {
                    if ([Object]::ReferenceEquals($original.Images[$index].Source,$seen.Source)) {
                        throw 'Initial duplicate loaded occurrence exceeds selected one-image source schedule.'
                    }
                }
                $seen.Complete=$true
            } else { $region.Image=$seen }
        }
        $original.Cursor+=$region.Bytes
    }
    $rootBase=[Runtime.InteropServices.Marshal]::ReadIntPtr($Birth.PendingEvent.Pointer,40).ToInt64()
    $root=$null
    for ($index=0;$index -lt $original.ImagesUsed;$index++) {
        if ($original.Images[$index].Base -eq $rootBase) { $root=$original.Images[$index] }
    }
    if ($null -eq $root -or -not [Object]::ReferenceEquals($root.Source.Held,$Owner.Compiler)) {
        throw 'Actual initial root image absent/different in complete native address scan.'
    }
    # Complete refers only to finite native address/path observations. Actual
    # full bytes/imports/loader source and every later load event remain required.
    $original.Complete=$true
    return $original
}

function Read-SlcliInitialWindowsModuleImages {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $walk=$Birth.Modules
    if ($null -eq $walk -or -not $walk.Complete -or $walk.ImageBytesComplete -or
        -not $Birth.RemoteImage.Complete -or $null -ne $Birth.PendingEvent.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event)) {
        throw 'Actual loaded image bytes require SAME stopped root and complete finite native scan.'
    }
    for ($index=0;$index -lt $walk.ImagesUsed;$index++) {
        $module=$walk.Images[$index]
        if ([Object]::ReferenceEquals($module.Source.Held,$Owner.Compiler)) {
            if (-not [Object]::ReferenceEquals($module.Mapping,$Birth.RemoteImage) -or
                $module.Base -ne $Birth.RemoteImage.Base) { throw 'Root scan/event mapping original changed.' }
            continue
        }
        if ($null -ne $module.Mapping.Module -or $null -ne $module.Mapping.EventImage -or $module.Mapping.Complete) {
            throw 'Initial module byte backing already consumed.'
        }
        $module.Mapping.Module=$module
        $null=Read-SlcliInitialWindowsImageRegions -Owner $Owner -Original $module.Mapping
    }
    # This marker means strict source-relocated bytes only, never complete
    # native loader/import/state authority or permission to continue an event.
    $walk.ImageBytesComplete=$true
    return $walk
}
