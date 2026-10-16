# Fixed SDK26100 x64 CRYPT_PROVIDER_DATA observation. This performs no native
# access. Pointer/handle values and aliases remain observations, never authority.
function Decode-Cli39ProviderHeader32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$h=$capture.headerOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
      -not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not $h.copyReturned -or -not $h.rangeReturned -or $h.used -ne 240 -or
      $h.rawBytesOriginal.Length -ne 240 -or $h.cbStructOriginal -ne 240 -or
      $null -ne $h.decodedOriginal -or $null -ne $n.closeWorkerOriginal -or
      $null -ne $n.storageReleaseOriginal){throw 'SAME once-only complete provider header copy required.'}
  $w=$h.copyWorkerOriginal
  if($null -eq $w -or -not [object]::ReferenceEquals($w.headerOriginal,$h) -or
      -not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $w.results[1] -ne 1 -or
      $null -ne $w.exceptionOriginal[0]){throw 'Actual terminal SAME header copy required.'}
  $d=@{headerOriginal=$h;rawBytesOriginal=$h.rawBytesOriginal;wordsOriginal=$null;
    fieldsOriginal=$null;returned=$false;exceptionOriginal=$null;
    state='RAW_PROVIDER_HEADER_OBSERVATIONS_NOT_COMPLETE_CAPTURE'}
  $h.decodedOriginal=$d
  try{
    # Fixed sixty words preserve every byte, including reserved alignment,
    # without count-driven allocations or omission of errors/partial aliases.
    $d.wordsOriginal=[uint32[]]::new(60)
    $d.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $b=$d.rawBytesOriginal
    for($i=0;$i -lt 60;$i++){$d.wordsOriginal[$i]=[BitConverter]::ToUInt32($b,4*$i)}
    $f=$d.fieldsOriginal
    foreach($entry in @(
      @('cbStruct',0),@('fOpenedFile',16),@('dwError',48),
      @('dwRegSecuritySettings',52),@('dwRegPolicySettings',56),
      @('cdwTrustStepErrors',72),@('chStores',88),@('dwEncoding',104),
      @('csSigners',120),@('csProvPrivData',136),@('dwSubjectChoice',152),
      @('fRecallWithState',176),@('sftSystemTimeLow',180),@('sftSystemTimeHigh',184),
      @('dwProvFlags',200),@('dwFinalError',204),@('dwTrustPubSettings',216),
      @('dwUIStateFlags',220))){$f.Add($entry[0],[BitConverter]::ToUInt32($b,$entry[1]))}
    foreach($entry in @(
      @('pWintrustData',8),@('hWndParent',24),@('pgActionID',32),@('hProv',40),
      @('psPfns',64),@('padwTrustStepErrors',80),@('pahStores',96),@('hMsg',112),
      @('pasSigners',128),@('pasProvPrivData',144),@('pPDSip',160),
      @('pszUsageOID',168),@('pszCTLSignerUsageOID',192),@('pRequestUsage',208),
      @('pSigState',224),@('pSigSettings',232))){$f.Add($entry[0],[IntPtr]::new([BitConverter]::ToInt64($b,$entry[1])))}
    # These equalities are retained facts only. Provider pWintrustData is
    # explicitly NOT verified by the SDK; equality does not authenticate it.
    $f.Add('sameWtdPointerObserved',($f['pWintrustData'] -eq $n.wtdPointer))
    $f.Add('sameActionPointerObserved',($f['pgActionID'] -eq $n.actionPointer))
    $d.returned=$true
    return $d
  }catch{$d.exceptionOriginal=$_;throw}
}

function Decode-Cli39PrimarySignerHeader32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$s=$SignerOriginal;$h=Get-Cli39ProviderHeaderRange32 $p $s
  $w=$h.copyWorkerOriginal
  if(-not $h.copyReturned -or -not $h.rangeReturned -or $h.used -ne 64 -or
      $h.rawBytesOriginal.Length -ne 64 -or $h.cbStructOriginal -ne 64 -or
      $null -ne $h.decodedOriginal -or $null -eq $w -or
      -not [object]::ReferenceEquals($w.headerOriginal,$h) -or
      -not [object]::ReferenceEquals($w.signerOriginal,$s) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $w.results[1] -ne 1 -or
      $null -ne $w.exceptionOriginal[0]){throw 'SAME actual once-only primary signer64 copy required.'}
  $d=@{headerOriginal=$h;signerOriginal=$s;rawBytesOriginal=$h.rawBytesOriginal;
    wordsOriginal=$null;fieldsOriginal=$null;returned=$false;exceptionOriginal=$null;
    state='RAW_PRIMARY_SIGNER_OBSERVATIONS_NOT_COMPLETE_CAPTURE'}
  $h.decodedOriginal=$d
  try{
    $d.wordsOriginal=[uint32[]]::new(16)
    $d.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $b=$d.rawBytesOriginal;$f=$d.fieldsOriginal
    for($i=0;$i -lt 16;$i++){$d.wordsOriginal[$i]=[BitConverter]::ToUInt32($b,4*$i)}
    foreach($entry in @(@('cbStruct',0),@('sftVerifyAsOfLow',4),@('sftVerifyAsOfHigh',8),
      @('csCertChain',12),@('dwSignerType',24),@('dwError',40),@('csCounterSigners',44))){
      $f.Add($entry[0],[BitConverter]::ToUInt32($b,$entry[1]))
    }
    foreach($entry in @(@('pasCertChain',16),@('psSigner',32),
      @('pasCounterSigners',48),@('pChainContext',56))){
      $f.Add($entry[0],[IntPtr]::new([BitConverter]::ToInt64($b,$entry[1])))
    }
    $d.returned=$true;return $d
  }catch{$d.exceptionOriginal=$_;throw}
}

function Decode-Cli39CertificateHeader32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$CertificateOriginal,
    [Parameter(Mandatory)][ValidateSet('provider-cert','cert-context')][string]$CertificatePart)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$cert=$CertificateOriginal;$h=Get-Cli39ProviderHeaderRange32 $p $null $cert $CertificatePart
  $w=$h.copyWorkerOriginal
  if(-not $h.copyReturned -or -not $h.rangeReturned -or $h.used -ne $h.bytesExpected -or
      $h.rawBytesOriginal.Length -ne $h.bytesExpected -or $null -ne $h.decodedOriginal -or
      -not [object]::ReferenceEquals($w.headerOriginal,$h) -or
      -not [object]::ReferenceEquals($w.certificateOriginal,$cert) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $w.results[1] -ne 1 -or
      $null -ne $w.exceptionOriginal[0]){throw 'Actual SAME once-only complete certificate header required.'}
  $d=@{headerOriginal=$h;certificateOriginal=$cert;partOriginal=$CertificatePart;
    rawBytesOriginal=$h.rawBytesOriginal;wordsOriginal=$null;fieldsOriginal=$null;
    returned=$false;exceptionOriginal=$null;state='RAW_CERTIFICATE_OBSERVATIONS_NOT_TRUST_AUTHORITY'}
  $h.decodedOriginal=$d
  try{
    $wordCount=if($CertificatePart -ceq 'provider-cert'){22}else{10}
    $d.wordsOriginal=[uint32[]]::new($wordCount)
    $d.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $b=$d.rawBytesOriginal;$f=$d.fieldsOriginal
    for($i=0;$i -lt $wordCount;$i++){$d.wordsOriginal[$i]=[BitConverter]::ToUInt32($b,4*$i)}
    if($CertificatePart -ceq 'provider-cert'){
      foreach($entry in @(@('cbStruct',0),@('fCommercial',16),@('fTrustedRoot',20),@('fSelfSigned',24),
        @('fTestCert',28),@('dwRevokedReason',32),@('dwConfidence',36),@('dwError',40),
        @('fTrustListSignerCert',56),@('dwCtlError',72),@('fIsCyclic',76))){$f.Add($entry[0],[BitConverter]::ToUInt32($b,$entry[1]))}
      foreach($entry in @(@('pCert',8),@('pTrustListContext',48),@('pCtlContext',64),@('pChainElement',80))){
        $f.Add($entry[0],[IntPtr]::new([BitConverter]::ToInt64($b,$entry[1])))
      }
    }else{
      $f.Add('dwCertEncodingType',[BitConverter]::ToUInt32($b,0));$f.Add('cbCertEncoded',[BitConverter]::ToUInt32($b,16))
      foreach($entry in @(@('pbCertEncoded',8),@('pCertInfo',24),@('hCertStore',32))){
        $f.Add($entry[0],[IntPtr]::new([BitConverter]::ToInt64($b,$entry[1])))
      }
    }
    $d.returned=$true;return $d
  }catch{$d.exceptionOriginal=$_;throw}
}

function Decode-Cli39SignerInfo32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$s=$SignerOriginal;$h=Get-Cli39ProviderHeaderRange32 $p $s $null '' 'signer-info';$w=$h.copyWorkerOriginal
  if(-not $h.copyReturned -or -not $h.rangeReturned -or $h.used -ne 136 -or
      $h.rawBytesOriginal.Length -ne 136 -or $null -ne $h.decodedOriginal -or
      -not [object]::ReferenceEquals($w.headerOriginal,$h) -or
      -not [object]::ReferenceEquals($w.signerOriginal,$s) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $w.results[1] -ne 1 -or
      $null -ne $w.exceptionOriginal[0]){throw 'Actual SAME once-only CMSG signer-info136 required.'}
  $d=@{headerOriginal=$h;signerOriginal=$s;rawBytesOriginal=$h.rawBytesOriginal;
    wordsOriginal=$null;fieldsOriginal=$null;returned=$false;exceptionOriginal=$null;
    state='RAW_CMSG_SIGNER_INFO_NOT_COMPLETE_ATTRIBUTE_CAPTURE'}
  $h.decodedOriginal=$d
  try{
    $d.wordsOriginal=[uint32[]]::new(34)
    $d.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $b=$d.rawBytesOriginal;$f=$d.fieldsOriginal
    for($i=0;$i -lt 34;$i++){$d.wordsOriginal[$i]=[BitConverter]::ToUInt32($b,4*$i)}
    foreach($entry in @(@('dwVersion',0),@('Issuer.cbData',8),@('SerialNumber.cbData',24),
      @('HashAlgorithm.Parameters.cbData',48),@('HashEncryptionAlgorithm.Parameters.cbData',72),
      @('EncryptedHash.cbData',88),@('AuthAttrs.cAttr',104),@('UnauthAttrs.cAttr',120))){
      $f.Add($entry[0],[BitConverter]::ToUInt32($b,$entry[1]))
    }
    foreach($entry in @(@('Issuer.pbData',16),@('SerialNumber.pbData',32),@('HashAlgorithm.pszObjId',40),
      @('HashAlgorithm.Parameters.pbData',56),@('HashEncryptionAlgorithm.pszObjId',64),
      @('HashEncryptionAlgorithm.Parameters.pbData',80),@('EncryptedHash.pbData',96),
      @('AuthAttrs.rgAttr',112),@('UnauthAttrs.rgAttr',128))){
      $f.Add($entry[0],[IntPtr]::new([BitConverter]::ToInt64($b,$entry[1])))
    }
    $d.returned=$true;return $d
  }catch{$d.exceptionOriginal=$_;throw}
}
