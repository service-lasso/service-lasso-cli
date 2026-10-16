# Synthetic TLS algorithms/denials only: no actual thread or native index issuer.
function Test-SlcliInitialWindowsTlsTemplateVectors {
    foreach ($kind in @('first-template-byte','last-template-byte','first-zero-fill','last-zero-fill','short-cursor','short-body')) {
        $expected=[byte[]]@(90,1,2,3,4,91)
        $row=[pscustomobject]@{ Source=[pscustomobject]@{Tls=[pscustomobject]@{TotalBytes=6;TemplateBytes=4;TemplateRva=1}};
            Module=[pscustomobject]@{Mapping=[pscustomobject]@{Expected=$expected}};
            Managed=[byte[]]@(1,2,3,4,0,0);Cursor=6 }
        switch ($kind) {
            'first-template-byte' { $row.Managed[0]=9 }
            'last-template-byte' { $row.Managed[3]=9 }
            'first-zero-fill' { $row.Managed[4]=9 }
            'last-zero-fill' { $row.Managed[5]=9 }
            'short-cursor' { $row.Cursor=5 }
            'short-body' { $row.Managed=[byte[]]@(1,2,3,4,0) }
        }
        $denied=$false
        try { Assert-SlcliInitialWindowsTlsTemplate -Row $row } catch { $denied=$true }
        if (-not $denied) { throw 'Incomplete/changed synthetic TLS template/zero-fill accepted.' }
    }
    foreach ($target in @([UInt64]0x140001000,[UInt64]0x140001080,[UInt64]0x1400010b0,[UInt64]0x140002000,[UInt64]0x1400010a1)) {
        $file=New-SlcliInitialWindowsTlsCookieFixture
        [Array]::Copy([BitConverter]::GetBytes($target),0,$file,528,8)
        $denied=$false
        try { $null=New-SlcliInitialWindowsTlsCookieSourceFixture -Bytes $file } catch { $denied=$true }
        if (-not $denied) { throw 'Synthetic TLS index/header/template/callback/outside/alignment overlap accepted.' }
    }
    $mapping=[pscustomobject]@{Geometry=[pscustomobject]@{ImageBytes=32};Expected=[byte[]]::new(32);
        Observed=[byte[]]::new(32);Compared=0;CookieOutput=$null;TlsIndexOutput=[pscustomobject]@{Complete=$false}}
    $denied=$false
    try { $null=Find-SlcliInitialWindowsImageDifference -Original $mapping } catch { $denied=$true }
    if (-not $denied) { throw 'Missing synthetic native TLS provenance suppressed immutable byte comparison.' }
}
