# Synthetic source/denial fixtures only. No native creation/case/profile/issuer.
function New-SlcliInitialWindowsTlsCookieFixture {
    $file=New-SlcliInitialWindowsPeFixture
    $base=[UInt64]0x140000000
    foreach ($pair in @(@(272,[UInt32]0x1000),@(276,[UInt32]40),@(280,[UInt32]0x1100),@(284,[UInt32]96),@(768,[UInt32]96))) {
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$file,$pair[0],4)
    }
    foreach ($pair in @(@(512,($base+0x1080)),@(520,($base+0x1088)),@(528,($base+0x10a0)),
        @(536,($base+0x10b0)),@(856,($base+0x11c0)))) {
        [Array]::Copy([BitConverter]::GetBytes([UInt64]$pair[1]),0,$file,$pair[0],8)
    }
    for ($index=0;$index -lt 8;$index++) { $file[640+$index]=[byte]($index+1) }
    return ,$file
}

function New-SlcliInitialWindowsTlsCookieSourceFixture {
    param([Parameter(Mandatory)][byte[]]$Bytes)
    $held=[pscustomobject]@{ Complete=$true; ManagedBody=$Bytes } # parser shape only
    $geometry=Get-SlcliInitialWindowsPeGeometry -Bytes $Bytes -PageBytes 4096 -ImageLimit 8192
    return [pscustomobject]@{ Held=$held; Geometry=$geometry;
        Tls=(Read-SlcliInitialWindowsTlsSource -Held $held -Geometry $geometry -ByteCapacity 64 -CallbackCapacity 4);
        Imports=[pscustomobject]@{ Used=0; Rows=[object[]]::new(1); DescriptorsUsed=0 };
        Exports=[pscustomobject]@{ NameCount=0; Bytes=0 };
        DelayImports=[pscustomobject]@{ DescriptorsUsed=0; Used=0 } }
}

function Test-SlcliInitialWindowsTlsCookieSourceVectors {
    $source=New-SlcliInitialWindowsTlsCookieSourceFixture -Bytes (New-SlcliInitialWindowsTlsCookieFixture)
    $cookie=Read-SlcliInitialWindowsCookieSource -Source $source
    if (-not $source.Tls.Present -or $source.Tls.TemplateBytes -ne 8 -or $source.Tls.TotalBytes -ne 8 -or
        $source.Tls.IndexRva -ne 0x10a0 -or $source.Tls.CallbacksUsed -ne 1 -or
        -not $source.Tls.Callbacks[0].Terminator -or -not $cookie.Present -or $cookie.Rva -ne 0x11c0) {
        throw 'Synthetic source-defined TLS/cookie geometry differs.'
    }
    foreach ($pair in @(@(276,[UInt32]39),@(548,[UInt32]1))) {
        $file=New-SlcliInitialWindowsTlsCookieFixture
        [Array]::Copy([BitConverter]::GetBytes($pair[1]),0,$file,$pair[0],4)
        $denied=$false
        try { $null=New-SlcliInitialWindowsTlsCookieSourceFixture -Bytes $file } catch { $denied=$true }
        if (-not $denied) { throw 'Synthetic truncated/reserved TLS form accepted.' }
    }
    foreach ($case in @('misaligned','outside','TLS-index','TLS-template','directory','import-slot','executable-section','short-load-config')) {
        $source=New-SlcliInitialWindowsTlsCookieSourceFixture -Bytes (New-SlcliInitialWindowsTlsCookieFixture)
        $pointer=[UInt64]0x1400011c0
        switch ($case) {
            'misaligned' { $pointer++ }
            'outside' { $pointer=[UInt64]0x140002000 }
            'TLS-index' { $pointer=[UInt64]0x1400010a0 }
            'TLS-template' { $pointer=[UInt64]0x140001080 }
            'directory' { $pointer=[UInt64]0x140001100 }
            'import-slot' { $source.Imports.Used=1;$source.Imports.Rows[0]=[pscustomobject]@{IatRva=0x11c0;LookupRva=0x1040} }
            'executable-section' { $source.Geometry.Sections[0].Flags=[UInt32]0x20000000 }
            'short-load-config' { [Array]::Copy([BitConverter]::GetBytes([UInt32]95),0,$source.Held.ManagedBody,768,4) }
        }
        [Array]::Copy([BitConverter]::GetBytes($pointer),0,$source.Held.ManagedBody,856,8)
        $denied=$false
        try { $null=Read-SlcliInitialWindowsCookieSource -Source $source } catch { $denied=$true }
        if (-not $denied) { throw 'Overlapping/outside/unselected synthetic cookie slot accepted.' }
    }
    # An incomplete output object must never suppress immutable comparison,
    # even when both raw vectors happen to be identical.
    $mapping=[pscustomobject]@{ Geometry=[pscustomobject]@{ImageBytes=32}; Expected=[byte[]]::new(32);
        Observed=[byte[]]::new(32); Compared=0; TlsIndexOutput=$null; CookieOutput=[pscustomobject]@{Case=$null;Complete=$false} }
    $denied=$false
    try { $null=Find-SlcliInitialWindowsImageDifference -Original $mapping } catch { $denied=$true }
    if (-not $denied) { throw 'Synthetic missing creation provenance suppressed final image comparison.' }
}

function Test-SlcliInitialWindowsCookieCaseDenials {
    # Deliberately synthetic record SHAPES for denial only. No loaded module
    # exists in any fixture, so none can construct a positive creation case.
    foreach ($kind in @('attach','failed-birth','unreturned-birth','wrong-flags','foreign-native-row','foreign-event-row',
        'wrong-pid','wrong-tid','later-load','prior-event','prior-continue','foreign-recipe','foreign-source',
        'wrong-exe-pointer','wrong-environment','wrong-fixed-exe','missing-child','reused-case','missing-slot')) {
        $native=[pscustomobject]@{Name='CreateProcessW';Raw=1;Ordinal=0;Returned=$true;Called=1;NativeReturned=1;
            Arguments=[object[]]@([IntPtr]::new(1),[IntPtr]::new(2),[IntPtr]::Zero,[IntPtr]::Zero,[Int32]1,
                [UInt32]0x08080401,[IntPtr]::new(3),[IntPtr]::new(4),[IntPtr]::new(5),[IntPtr]::new(6))}
        $wait=[pscustomobject]@{Name='WaitForDebugEvent';Raw=1;Ordinal=1;Returned=$true;Called=1;NativeReturned=1}
        $event=[pscustomobject]@{Code=3;Pid=10;Tid=11;Continue=$null;Wait=$wait}
        $birth=[pscustomobject]@{Birth=$native;Pid=10;Tid=11;PendingEvent=$event;Events=[object[]]@($event);
            Child=[pscustomobject]@{Complete=$true;Event=$event};Modules=[pscustomobject]@{Complete=$true;ImagesUsed=0;Images=@()};
            Exe=[pscustomobject]@{Text='C:\fixed\cl.exe';Native=[pscustomobject]@{Pointer=[IntPtr]::new(1)}};
            Command=[pscustomobject]@{Native=[pscustomobject]@{Pointer=[IntPtr]::new(2)}};
            Environment=[pscustomobject]@{Native=[pscustomobject]@{Pointer=[IntPtr]::new(3)}};
            Cwd=[pscustomobject]@{Native=[pscustomobject]@{Pointer=[IntPtr]::new(4)}};
            Startup=[pscustomobject]@{Pointer=[IntPtr]::new(5)};Process=[pscustomobject]@{Pointer=[IntPtr]::new(6)}}
        $plan=[pscustomobject]@{Active=$birth};$sources=[pscustomobject]@{}
        $owner=[pscustomobject]@{Failed=$false;CompilePlan=$plan;ModuleSources=$sources;RowsUsed=2;Rows=[object[]]@($native,$wait);
            Compiler=[pscustomobject]@{Source=[pscustomobject]@{Path='C:\fixed\cl.exe'}}}
        $row=[pscustomobject]@{Case=$null;CaseBacking=[pscustomobject]@{State='Prebirth'};
            Source=[pscustomobject]@{Present=$true;Complete=$true};Module=$null}
        $capture=[pscustomobject]@{Owner=$owner;Recipe=$plan;Sources=$sources;Birth=$birth;Rows=[object[]]@($row)}
        switch ($kind) {
            'attach' { $native.Name='DebugActiveProcess' }
            'failed-birth' { $native.Raw=0 }
            'unreturned-birth' { $native.NativeReturned=0 }
            'wrong-flags' { $native.Arguments[5]=[UInt32]0 }
            'foreign-native-row' { $owner.Rows[0]=[pscustomobject]@{} }
            'foreign-event-row' { $owner.Rows[1]=[pscustomobject]@{} }
            'wrong-pid' { $event.Pid=12 }
            'wrong-tid' { $event.Tid=12 }
            'later-load' { $event.Code=6 }
            'prior-event' { $birth.Events=[object[]]@([pscustomobject]@{Continue=$null;Wait=[pscustomobject]@{Raw=1}},$event) }
            'prior-continue' { $event.Continue=[pscustomobject]@{} }
            'foreign-recipe' { $capture.Recipe=[pscustomobject]@{} }
            'foreign-source' { $capture.Sources=[pscustomobject]@{} }
            'wrong-exe-pointer' { $native.Arguments[0]=[IntPtr]::new(20) }
            'wrong-environment' { $native.Arguments[6]=[IntPtr]::new(20) }
            'wrong-fixed-exe' { $birth.Exe.Text='C:\other\cl.exe' }
            'missing-child' { $birth.Child.Complete=$false }
            'reused-case' { $row.CaseBacking.State='Constructed' }
            'missing-slot' { $row.Source.Present=$false }
        }
        $denied=$false
        try { $null=New-SlcliInitialWindowsCookieCase -Owner $owner -Capture $capture -Row $row } catch { $denied=$true }
        if (-not $denied) { throw "Synthetic unowned/foreign-phase cookie case accepted: $kind" }
    }
}
