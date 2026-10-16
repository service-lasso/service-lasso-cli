# Private stopped-child ordinary-import correspondence. This never invokes a
# loader or export and never grants permission to continue a debugger event.
function Get-SlcliInitialWindowsModuleKey {
    param([Parameter(Mandatory)][string]$Name)
    if ($Name.Length -eq 0 -or $Name.Length -gt 32768) { throw 'Empty/oversized module name.' }
    foreach ($character in $Name.ToCharArray()) {
        if (-not (($character -ge 'a' -and $character -le 'z') -or ($character -ge 'A' -and $character -le 'Z') -or
            ($character -ge '0' -and $character -le '9') -or $character -in @('-','_','.'))) {
            throw 'Module name is not the selected path-free ASCII basename.'
        }
    }
    if ($Name.StartsWith('.') -or $Name.EndsWith('.') -or $Name.Contains('..')) { throw 'Ambiguous module basename.' }
    $key=$Name.ToLowerInvariant()
    if (-not $key.EndsWith('.dll') -and -not $key.EndsWith('.exe')) { $key+='.dll' }
    return $key
}

function Select-SlcliInitialWindowsApiSetHost {
    param([Parameter(Mandatory)][object]$Schema,[Parameter(Mandatory)][string]$Contract,
        [Parameter(Mandatory)][string]$Importer)
    if (-not $Schema.Complete) { throw 'Incomplete actual API-set namespace.' }
    $key=Get-SlcliInitialWindowsModuleKey -Name $Contract
    if (-not $key.StartsWith('api-') -and -not $key.StartsWith('ext-')) { return $null }
    if (-not $key.EndsWith('.dll')) { throw 'API-set contract is not the selected DLL form.' }
    $key=$key.Substring(0,$key.Length-4)
    $importerKey=Get-SlcliInitialWindowsModuleKey -Name $Importer
    $entry=$null
    for ($index=0;$index -lt $Schema.Count;$index++) {
        if ([String]::Equals($key,$Schema.Entries[$index].Name,[StringComparison]::OrdinalIgnoreCase)) {
            if ($null -ne $entry) { throw 'Ambiguous actual API-set contract.' }
            $entry=$Schema.Entries[$index]
        }
    }
    if ($null -eq $entry) { throw 'Actual API-set contract absent; no guessed host.' }
    $selected=$null;$fallback=$null
    for ($index=0;$index -lt $entry.ValueCount;$index++) {
        $value=$Schema.Values[$entry.FirstValue+$index]
        if ($value.Alias.Length -eq 0) { $fallback=$value }
        elseif ([String]::Equals((Get-SlcliInitialWindowsModuleKey -Name $value.Alias),$importerKey,[StringComparison]::Ordinal)) {
            if ($null -ne $selected) { throw 'Ambiguous actual API-set importer alias.' }
            $selected=$value
        }
    }
    if ($null -eq $selected) { $selected=$fallback }
    if ($null -eq $selected -or $selected.Host.Length -eq 0) { throw 'Actual API-set host absent/empty; no loader fallback.' }
    return $selected
}

function New-SlcliInitialWindowsImportRoutes {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Walk)
    $capacity=$Owner.Source.ImportForwarderCapacity
    if ($capacity -isnot [Int32] -or $capacity -le 0 -or $capacity -gt 64 -or $Walk.Started) {
        throw 'Finite import route backing must precede actual child birth.'
    }
    foreach ($module in $Walk.Images) {
        for ($index=0;$index -lt $module.ImportRoutes.Length;$index++) {
            $route=[pscustomobject]@{ Import=$null; Hops=[object[]]::new($capacity); Used=0;
                Actual=[UInt64]0; Target=[UInt64]0; State='Unobserved'; Started=$false }
            $module.ImportRoutes[$index]=$route
            for ($hop=0;$hop -lt $capacity;$hop++) {
                $route.Hops[$hop]=[pscustomobject]@{ Importer=$null; Requested=$null; Host=$null; ApiSet=$null;
                    Module=$null; Name=$null; ByOrdinal=$false; Ordinal=[UInt32]0; Export=$null; Captured=$null }
            }
        }
    }
}

function Read-SlcliInitialWindowsImportRoutes {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $walk=$Birth.Modules
    if (-not $walk.ImportBytesObserved -or -not $walk.ExportBytesObserved -or -not $Birth.ApiSet.Complete -or
        $walk.RouteBytesObserved -or $null -ne $Birth.PendingEvent.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event)) {
        throw 'Import routes require SAME stopped child/import/export/API-set occurrences.'
    }
    $walk.RouteBefore=Read-SlcliInitialWindowsClock -Owner $Owner
    for ($moduleIndex=0;$moduleIndex -lt $walk.ImagesUsed;$moduleIndex++) {
        $module=$walk.Images[$moduleIndex]
        for ($index=0;$index -lt $module.Source.Imports.Used;$index++) {
            $route=$module.ImportRoutes[$index];$route.Started=$true;$route.Import=$module.ImportRows[$index]
            $route.Actual=$route.Import.Actual
            $source=$route.Import.Source;$importer=$module;$requested=$source.Descriptor.Name
            $byOrdinal=$source.ByOrdinal;$ordinal=[UInt32]$source.Ordinal;$name=$source.Name
            while ($true) {
                if ($route.Used -ge $route.Hops.Length) { throw 'Finite actual import forwarder route capacity exhausted.' }
                $hop=$route.Hops[$route.Used];$route.Used++
                $hop.Importer=$importer;$hop.Requested=$requested;$hop.Name=$name;$hop.ByOrdinal=$byOrdinal;$hop.Ordinal=$ordinal
                $importerName=$importer.Path.Text.Substring($importer.Path.Text.LastIndexOf('\')+1)
                $hop.ApiSet=Select-SlcliInitialWindowsApiSetHost -Schema $Birth.ApiSet.Schema -Contract $requested -Importer $importerName
                $hop.Host=Get-SlcliInitialWindowsModuleKey -Name $(if ($null -eq $hop.ApiSet) {$requested} else {$hop.ApiSet.Host})
                for ($candidate=0;$candidate -lt $walk.ImagesUsed;$candidate++) {
                    $image=$walk.Images[$candidate]
                    $leaf=$image.Path.Text.Substring($image.Path.Text.LastIndexOf('\')+1)
                    if ((Get-SlcliInitialWindowsModuleKey -Name $leaf) -ceq $hop.Host) {
                        if ($null -ne $hop.Module) { throw 'Actual import host has ambiguous loaded basename.' }
                        $hop.Module=$image
                    }
                }
                if ($null -eq $hop.Module) { $route.State='TargetNotLoaded';break }
                $exports=$hop.Module.Source.Exports;$function=$null
                if ($byOrdinal) {
                    [Int64]$slot=[Int64]$ordinal-$exports.OrdinalBase
                    if ($slot -ge 0 -and $slot -lt $exports.FunctionCount) { $function=$exports.Functions[$slot] }
                } else {
                    for ($symbol=0;$symbol -lt $exports.NameCount;$symbol++) {
                        if ($exports.Names[$symbol].Name -ceq $name) { $function=$exports.Names[$symbol].Function;break }
                    }
                }
                if ($null -eq $function -or $function.Hole) { throw 'Actual selected export absent/hole; no guessed function.' }
                $hop.Export=$function;$hop.Captured=$hop.Module.ExportRows[$function.Index]
                if ($null -eq $hop.Captured -or -not [Object]::ReferenceEquals($hop.Captured.Source,$function)) {
                    throw 'Import route does not join SAME captured export original.'
                }
                for ($prior=0;$prior -lt $route.Used-1;$prior++) {
                    if ([Object]::ReferenceEquals($route.Hops[$prior].Module,$hop.Module) -and
                        [Object]::ReferenceEquals($route.Hops[$prior].Export,$function)) { throw 'Actual export forwarder cycle.' }
                }
                if ($null -eq $hop.Captured.Forwarder) {
                    $route.Target=[UInt64]($hop.Module.Base+$function.Rva)
                    if ($route.Target -ne [UInt64]$hop.Captured.Target) { throw 'Actual direct export address differs from source-derived RVA/base.' }
                    if ($route.Target -eq 0) { throw 'Actual direct export target is null.' }
                    $route.State=if ($route.Actual -eq $route.Target) {'AddressMatches'} else {'AddressUnresolved'}
                    break
                }
                $forwarder=$hop.Captured.Forwarder;$separator=$forwarder.LastIndexOf('.')
                if ($separator -le 0 -or $separator -eq $forwarder.Length-1) { throw 'Malformed captured forwarder.' }
                $requested=$forwarder.Substring(0,$separator);$symbolName=$forwarder.Substring($separator+1)
                $byOrdinal=$symbolName.StartsWith('#');$ordinal=[UInt32]0;$name=$null
                if ($byOrdinal) {
                    $digits=$symbolName.Substring(1)
                    if ($digits.Length -eq 0) { throw 'Empty forwarded ordinal.' }
                    foreach ($digit in $digits.ToCharArray()) { if ($digit -lt '0' -or $digit -gt '9') { throw 'Malformed forwarded ordinal.' } }
                    if (-not [UInt32]::TryParse($digits,[Globalization.NumberStyles]::None,[Globalization.CultureInfo]::InvariantCulture,[ref]$ordinal)) {
                        throw 'Forwarded ordinal exceeds complete UInt32 extent.'
                    }
                } else { $name=$symbolName }
                $importer=$hop.Module
            }
            if ($route.State -ceq 'AddressMatches') { $walk.RouteMatches++ } else { $walk.RouteUnresolved++ }
        }
    }
    $walk.RouteAfter=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($walk.RouteAfter.Tick -lt $walk.RouteBefore.Tick) { throw 'Actual import-route processing clock regressed.' }
    $Owner.PriorTick=$walk.RouteAfter.Tick;$walk.RouteBytesObserved=$true
    # AddressMatches is correspondence only, not source/loader/ROOT authority.
    # TargetNotLoaded/AddressUnresolved remain explicit unfinished loader states.
    return $walk
}

function Assert-SlcliInitialWindowsBindingSpan {
    param([Parameter(Mandatory)][object]$Mapping,[Parameter(Mandatory)][Int64]$Slot)
    if ($Slot -lt 0 -or $Slot -gt $Mapping.Expected.Length-8 -or
        $Mapping.BindingSeen.Length -ne [Math]::Floor(([Int64]$Mapping.Expected.Length+7)/8) -or
        $Mapping.RelocationSeen.Length -ne $Mapping.BindingSeen.Length) { throw 'Incomplete binding effect span/overlap backing.' }
    for ($offset=0;$offset -lt 8;$offset++) {
        $bit=$Slot+$offset;$cell=[Int32][Math]::Floor($bit/8);$mask=1 -shl ($bit%8)
        if (($Mapping.BindingSeen[$cell] -band $mask) -ne 0 -or ($Mapping.RelocationSeen[$cell] -band $mask) -ne 0) {
            throw 'Binding effect overlaps another binding/relocation; no ambiguous byte attribution.'
        }
        $Mapping.BindingSeen[$cell]=[byte]($Mapping.BindingSeen[$cell] -bor $mask)
    }
}

function Complete-SlcliInitialWindowsImageCorrespondence {
    param([Parameter(Mandatory)][object]$Owner,[Parameter(Mandatory)][object]$Birth)
    $walk=$Birth.Modules
    if (-not $walk.ImagesCaptured -or -not $walk.RouteBytesObserved -or -not $walk.BoundBytesObserved -or
        -not $walk.DelayBytesObserved -or $walk.ImageBytesComplete -or $null -ne $Birth.PendingEvent.Continue -or
        -not [Object]::ReferenceEquals($Birth.PendingEvent,$Birth.Child.Event)) {
        throw 'Final source/effect correspondence requires SAME stopped full graph.'
    }
    $walk.EffectBefore=Read-SlcliInitialWindowsClock -Owner $Owner
    for ($moduleIndex=0;$moduleIndex -lt $walk.ImagesUsed;$moduleIndex++) {
        $module=$walk.Images[$moduleIndex];$mapping=$module.Mapping
        if (-not $mapping.CapturedComplete -or -not $mapping.ExpectedComplete -or $mapping.Complete) {
            throw 'Final source/effect mapping is absent/already consumed.'
        }
        for ($index=0;$index -lt $module.Source.Imports.Used;$index++) {
            $route=$module.ImportRoutes[$index];$slot=$route.Import.Source.IatRva
            if (-not [Object]::ReferenceEquals($route.Import.Mapping,$mapping) -or
                -not [Object]::ReferenceEquals($route.Import.Source,$module.Source.Imports.Rows[$index]) -or
                $slot -lt 0 -or $slot -gt $mapping.Expected.Length-8) { throw 'Import binding effect lost SAME source/slot occurrence.' }
            # Unresolved/missing targets cannot explain a changed byte. Original
            # source-relocated values remain subject to the final full compare.
            if ($route.State -cne 'AddressMatches' -or $route.Actual -eq $route.Import.ExpectedInitial) { continue }
            if ($mapping.BindingEffectsUsed -ge $mapping.BindingEffects.Length) { throw 'Finite binding-effect roster exhausted.' }
            $effect=[pscustomobject]@{ Rule='PE32PlusImportAddressTable'; Source=$route.Import.Source;
                Route=$route; Mapping=$mapping; Offset=$slot; Before=$route.Import.ExpectedInitial;
                DerivedTarget=$route.Target; Bytes=[BitConverter]::GetBytes($route.Target); Applied=$false }
            $mapping.BindingEffects[$mapping.BindingEffectsUsed]=$effect;$mapping.BindingEffectsUsed++
            if ($effect.DerivedTarget -eq 0 -or $effect.DerivedTarget -ne $route.Actual -or
                [BitConverter]::ToUInt64($mapping.Expected,$slot) -ne $effect.Before) {
                throw 'Binding source/effect address is not independently derived from actual export route.'
            }
            Assert-SlcliInitialWindowsBindingSpan -Mapping $mapping -Slot $slot
            # This vector comes from the held import declaration and the
            # held export RVA plus actual observed module base. It is NEVER
            # copied from the observed image or an input expected-value mask.
            [Array]::Copy($effect.Bytes,0,$mapping.Expected,$slot,8)
            $effect.Applied=$true
        }
    }
    Complete-SlcliInitialWindowsTlsIndexOutputs -Owner $Owner -Birth $Birth
    $walk.EffectAfter=Read-SlcliInitialWindowsClock -Owner $Owner
    if ($walk.EffectAfter.Tick -lt $walk.EffectBefore.Tick) { throw 'Actual binding-effect clock regressed.' }
    $Owner.PriorTick=$walk.EffectAfter.Tick
    for ($moduleIndex=0;$moduleIndex -lt $walk.ImagesUsed;$moduleIndex++) {
        $null=Compare-SlcliInitialWindowsRemoteImage -Owner $Owner -Original $walk.Images[$moduleIndex].Mapping
    }
    # Full byte correspondence, including individually derived ordinary IAT
    # effects, is still NOT complete actual loader/source/initial ROOT authority.
    $walk.ImageBytesComplete=$true
    return $walk
}
