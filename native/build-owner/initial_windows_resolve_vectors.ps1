# Synthetic layout/selection regressions only. No actual child, held original,
# native family, initial admission or successful publication is constructed.
function Test-SlcliInitialWindowsImportSelectionVectors {
    $schema=Read-SlcliInitialWindowsApiSetSchema -Bytes (New-SlcliInitialWindowsApiSetFixture) -EntryCapacity 2 -ValueCapacity 2
    $alias=Select-SlcliInitialWindowsApiSetHost -Schema $schema -Contract 'API-MS-TEST.DLL' -Importer 'CLIENT.EXE'
    $fallback=Select-SlcliInitialWindowsApiSetHost -Schema $schema -Contract 'api-ms-test' -Importer 'other.exe'
    if (-not [Object]::ReferenceEquals($alias,$schema.Values[1]) -or
        -not [Object]::ReferenceEquals($fallback,$schema.Values[0]) -or
        $null -ne (Select-SlcliInitialWindowsApiSetHost -Schema $schema -Contract 'kernel32.dll' -Importer 'client.exe')) {
        throw 'Synthetic actual namespace alias/default/ordinary selection differs.'
    }
    foreach ($name in @('../kernel32','C:\kernel32.dll','kernel32/dll','kernel32..dll','.kernel32','kernel32.','kernel32 dll')) {
        $denied=$false
        try { $null=Get-SlcliInitialWindowsModuleKey -Name $name } catch { $denied=$true }
        if (-not $denied) { throw 'Path/ambiguous synthetic module name accepted.' }
    }
    foreach ($case in @('absent-contract','wrong-contract-extension','empty-alias-host','absent-default','duplicate-normalized-alias')) {
        $schema=Read-SlcliInitialWindowsApiSetSchema -Bytes (New-SlcliInitialWindowsApiSetFixture) -EntryCapacity 2 -ValueCapacity 2
        $contract='api-ms-test';$importer='client.exe'
        switch ($case) {
            'absent-contract' { $contract='api-ms-absent' }
            'wrong-contract-extension' { $contract='api-ms-test.exe' }
            'empty-alias-host' { $schema.Values[1].Host='' }
            'absent-default' { $schema.Values[0].Alias='different.exe';$importer='other.exe' }
            'duplicate-normalized-alias' { $schema.Values[0].Alias='client';$schema.Values[1].Alias='client.dll';$importer='client.dll' }
        }
        $denied=$false
        try { $null=Select-SlcliInitialWindowsApiSetHost -Schema $schema -Contract $contract -Importer $importer } catch { $denied=$true }
        if (-not $denied) { throw 'Missing/empty/ambiguous synthetic API-set host accepted.' }
    }
}

function Test-SlcliInitialWindowsBindingSpanVectors {
    # Only synthetic overlap backing. No route, target, native mapping or issuer.
    foreach ($case in @('duplicate','partial-overlap','relocation-first-byte','relocation-last-byte','outside','short-backing')) {
        $mapping=[pscustomobject]@{ Expected=[byte[]]::new(32); BindingSeen=[byte[]]::new(4); RelocationSeen=[byte[]]::new(4) }
        $slot=[Int64]8
        switch ($case) {
            'duplicate' { Assert-SlcliInitialWindowsBindingSpan -Mapping $mapping -Slot 8 }
            'partial-overlap' { Assert-SlcliInitialWindowsBindingSpan -Mapping $mapping -Slot 15 }
            'relocation-first-byte' { $mapping.RelocationSeen[1]=1 }
            'relocation-last-byte' { $mapping.RelocationSeen[1]=128 }
            'outside' { $slot=25 }
            'short-backing' { $mapping.BindingSeen=[byte[]]::new(3) }
        }
        $denied=$false
        try { Assert-SlcliInitialWindowsBindingSpan -Mapping $mapping -Slot $slot } catch { $denied=$true }
        if (-not $denied) { throw 'Ambiguous/outside synthetic binding effect accepted.' }
    }
}
