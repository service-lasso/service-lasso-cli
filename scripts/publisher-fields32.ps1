# Fixed provider metadata. Private pvProvData is never dereferenced postreturn.
function Capture-Cli39ProviderFields32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal
  $parent=$capture.headerOriginal
  if(-not $parent.copyReturned -or -not $parent.decodedOriginal.returned){throw 'Actual SAME DATA240 before provider fields required.'}
  $f=$parent.decodedOriginal.fieldsOriginal
  foreach($part in @('trust-step-errors','stores','private-headers','sig-state','sig-settings','secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')){
    $parent=$capture.headerOriginal;$f=$parent.decodedOriginal.fieldsOriginal
    if($part -cin @('secondary-signatures','sealing-signature')){
      $parent=$capture.providerFieldOriginals['sig-state']
    }elseif($part -ceq 'strong-policy'){$parent=$capture.providerFieldOriginals['sig-settings']}
    elseif($part -cin @('sealing-parameters','sealing-digest')){$parent=$capture.providerFieldOriginals['sealing-signature']}
    elseif($part -ceq 'ui-functions'){$parent=$capture.providerFieldOriginals['provider-functions']}
    elseif($part -ceq 'ui-data'){$parent=$capture.providerFieldOriginals['ui-functions']}
    elseif($part -ceq 'usage-oid-pointers'){$parent=$capture.providerFieldOriginals['request-usage']}
    if($part -cin @('secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')){
      if(-not $parent.decodedOriginal.returned){throw 'Actual decoded signature parent required.'}
      $f=$parent.decodedOriginal.fieldsOriginal
    }
    switch -CaseSensitive ($part){
      'trust-step-errors'{$pointer=$f['padwTrustStepErrors'];[long]$extent=4L*[long][uint32]$f['cdwTrustStepErrors']}
      'stores'{$pointer=$f['pahStores'];[long]$extent=8L*[long][uint32]$f['chStores']}
      'private-headers'{$pointer=$f['pasProvPrivData'];[long]$extent=32L*[long][uint32]$f['csProvPrivData']}
      'sig-state'{$pointer=$f['pSigState'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{64L}}
      'sig-settings'{$pointer=$f['pSigSettings'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{32L}}
      'secondary-signatures'{$pointer=if($parent.absenceObserved){[IntPtr]::Zero}else{$f['rhSecondarySigs']};$extent=if($parent.absenceObserved){0L}else{8L*[long][uint32]$f['cSecondarySigs']}}
      'sealing-signature'{$pointer=if($parent.absenceObserved){[IntPtr]::Zero}else{$f['pSealingSignature']};$extent=if($pointer -eq [IntPtr]::Zero){0L}else{48L}}
      'strong-policy'{$pointer=if($parent.absenceObserved){[IntPtr]::Zero}else{$f['pCryptoPolicy']};$extent=if($pointer -eq [IntPtr]::Zero){0L}else{16L}}
      'sealing-parameters'{$pointer=if($parent.absenceObserved){[IntPtr]::Zero}else{$f['signatureAlgorithm.Parameters.pbData']};$extent=if($parent.absenceObserved){0L}else{[long][uint32]$f['signatureAlgorithm.Parameters.cbData']}}
      'sealing-digest'{$pointer=if($parent.absenceObserved){[IntPtr]::Zero}else{$f['encryptedDigest.pbData']};$extent=if($parent.absenceObserved){0L}else{[long][uint32]$f['encryptedDigest.cbData']}}
      'provider-functions'{$pointer=$f['psPfns'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{128L}}
      'ui-functions'{$pointer=if($parent.absenceObserved){[IntPtr]::Zero}else{$f['psUIpfns']};$extent=if($pointer -eq [IntPtr]::Zero){0L}else{48L}}
      'ui-data'{$pointer=if($parent.absenceObserved){[IntPtr]::Zero}else{$f['psUIData']};$extent=if($pointer -eq [IntPtr]::Zero){0L}else{64L}}
      'request-usage'{$pointer=$f['pRequestUsage'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{24L}}
      'usage-oid-pointers'{$pointer=if($parent.absenceObserved){[IntPtr]::Zero}else{$f['Usage.rgpszUsageIdentifier']};$extent=if($parent.absenceObserved){0L}else{8L*[long][uint32]$f['Usage.cUsageIdentifier']}}
    }
    if($null -ne $capture.providerFieldOriginals[$part]){throw 'Actual provider field cannot reset/retry.'}
    $h=@{captureOriginal=$capture;parentHeaderOriginal=$parent;providerPart=$part;
      providerPointerOriginal=$pointer;stateWorkerOriginal=$capture.stateWorkerOriginal;
      bytesExpected=$extent;used=0;pagesOriginal=$null;rangeReturned=$false;rawBytesOriginal=$null;
      copyWorkerOriginal=$null;copyReturned=$false;absenceObserved=$false;decodedOriginal=$null;exceptionOriginal=$null}
    $capture.providerFieldOriginals[$part]=$h
    try{
      $null=Get-Cli39ProviderHeaderRange32 $p $null $null '' '' $null $null $part
      $h.pagesOriginal=[Collections.Generic.List[object]]::new([int]$extent)
      $h.rawBytesOriginal=[byte[]]::new([int]$extent)
      if($extent -eq 0){
        $h.absenceObserved=$true;$h.rangeReturned=$true
        $null=Decode-Cli39ProviderField32 $p $part
        continue
      }
      while(-not $h.rangeReturned){
        $w=New-Cli39ProviderPageWorker32 $p $null $null '' '' $null $null $part
        $null=Start-Cli39PublisherWorker26 $w
        while(-not $w.terminalJoined){
          $null=Observe-Cli39PublisherWorker26 $w
          if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
        }
        $null=Complete-Cli39ProviderPage32 $w
      }
      $null=Copy-Cli39ProviderHeader32 $p $null $null '' '' $null $null $part
    }catch{$h.exceptionOriginal=$_;throw}
  }
  $null=Capture-Cli39NamedProviderOid32 $p $capture.headerOriginal 'provider-usage'
  $null=Capture-Cli39NamedProviderOid32 $p $capture.headerOriginal 'ctl-usage'
  $null=Capture-Cli39NamedProviderOid32 $p $capture.providerFieldOriginals['sealing-signature'] 'sealing-algorithm'
  $usage=$capture.providerFieldOriginals['usage-oid-pointers']
  if(-not $usage.absenceObserved){
    for($index=0;$index -lt $usage.decodedOriginal.itemsOriginal.Count;$index++){
      $null=Capture-Cli39NamedProviderOid32 $p $usage 'requested-usage' $index
    }
  }
  return $capture
}

function Decode-Cli39ProviderField32 {
  param([Parameter(Mandatory)]$OriginalPreparation,
    [Parameter(Mandatory)][ValidateSet('trust-step-errors','stores','private-headers','sig-state','sig-settings','secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')][string]$Part)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$h=Get-Cli39ProviderHeaderRange32 $p $null $null '' '' $null $null $Part
  if((-not $h.copyReturned -and -not $h.absenceObserved) -or
      $h.rawBytesOriginal.Length -ne $h.bytesExpected -or $null -ne $h.decodedOriginal){throw 'Actual SAME once-only complete provider field required.'}
  if(-not $h.absenceObserved){
    $w=$h.copyWorkerOriginal
    if(-not $w.terminalJoined -or $w.thread.IsAlive -or $w.results[1] -ne 1 -or
        $null -ne $w.exceptionOriginal[0] -or -not [object]::ReferenceEquals($w.headerOriginal,$h)){throw 'Actual terminal provider field copy required.'}
  }
  $d=@{rangeOriginal=$h;rawBytesOriginal=$h.rawBytesOriginal;wordsOriginal=$null;
    itemsOriginal=$null;fieldsOriginal=$null;returned=$false;exceptionOriginal=$null;
    absenceObserved=$h.absenceObserved;state='RAW_PROVIDER_METADATA_NOT_COMPLETE_NATIVE_CUSTODY'}
  $h.decodedOriginal=$d
  try{
    $b=$d.rawBytesOriginal;$wordCount=[int]($h.bytesExpected/4)
    $d.wordsOriginal=[uint32[]]::new($wordCount)
    $d.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    for($i=0;$i -lt $wordCount;$i++){$d.wordsOriginal[$i]=[BitConverter]::ToUInt32($b,4*$i)}
    $f=$d.fieldsOriginal
    if($h.absenceObserved){$d.returned=$true;return $d}
    switch -CaseSensitive ($Part){
      'trust-step-errors'{
        $d.itemsOriginal=[Collections.Generic.List[object]]::new($wordCount)
        for($i=0;$i -lt $wordCount;$i++){$d.itemsOriginal.Add($d.wordsOriginal[$i])}
      }
      'stores'{
        $count=[int]($h.bytesExpected/8);$d.itemsOriginal=[Collections.Generic.List[object]]::new($count)
        for($i=0;$i -lt $count;$i++){$d.itemsOriginal.Add([IntPtr]::new([BitConverter]::ToInt64($b,8*$i)))}
      }
      'secondary-signatures'{
        $count=[int]($h.bytesExpected/8);$d.itemsOriginal=[Collections.Generic.List[object]]::new($count)
        for($i=0;$i -lt $count;$i++){$d.itemsOriginal.Add([IntPtr]::new([BitConverter]::ToInt64($b,8*$i)))}
        # Provider-owned HCRYPTMSG aliases; no individual close authority.
      }
      'sealing-signature'{
        foreach($entry in @(@('version',0),@('signerIndex',4),@('signatureAlgorithm.Parameters.cbData',16),@('encryptedDigest.cbData',32))){$f.Add($entry[0],[BitConverter]::ToUInt32($b,$entry[1]))}
        foreach($entry in @(@('signatureAlgorithm.pszObjId',8),@('signatureAlgorithm.Parameters.pbData',24),@('encryptedDigest.pbData',40))){$f.Add($entry[0],[IntPtr]::new([BitConverter]::ToInt64($b,$entry[1])))}
      }
      'strong-policy'{
        $f.Add('cbSize',[BitConverter]::ToUInt32($b,0));$f.Add('dwInfoChoice',[BitConverter]::ToUInt32($b,4))
        $f.Add('pvInfo',[IntPtr]::new([BitConverter]::ToInt64($b,8)))
      }
      'provider-functions'{
        $f.Add('cbStruct',[BitConverter]::ToUInt32($b,0))
        $names=[string[]]@('pfnAlloc','pfnFree','pfnAddStore2Chain','pfnAddSgnr2Chain',
          'pfnAddCert2Chain','pfnAddPrivData2Chain','pfnInitialize','pfnObjectTrust',
          'pfnSignatureTrust','pfnCertificateTrust','pfnFinalPolicy','pfnCertCheckPolicy',
          'pfnTestFinalPolicy','psUIpfns','pfnCleanupPolicy')
        for($i=0;$i -lt $names.Length;$i++){$f.Add($names[$i],[IntPtr]::new([BitConverter]::ToInt64($b,8+8*$i)))}
        # Captured callback aliases are never called, replaced or converted
        # into generic delegates. They grant no instrumentation authority.
      }
      'ui-functions'{
        $f.Add('cbStruct',[BitConverter]::ToUInt32($b,0))
        $names=[string[]]@('psUIData','pfnOnMoreInfoClick','pfnOnMoreInfoClickDefault',
          'pfnOnAdvancedClick','pfnOnAdvancedClickDefault')
        for($i=0;$i -lt $names.Length;$i++){$f.Add($names[$i],[IntPtr]::new([BitConverter]::ToInt64($b,8+8*$i)))}
      }
      'ui-data'{
        $f.Add('cbStruct',[BitConverter]::ToUInt32($b,0));$f.Add('dwFinalError',[BitConverter]::ToUInt32($b,4))
        $names=[string[]]@('pYesButtonText','pNoButtonText','pMoreInfoButtonText','pAdvancedLinkText',
          'pCopyActionText','pCopyActionTextNoTS','pCopyActionTextNotSigned')
        for($i=0;$i -lt $names.Length;$i++){$f.Add($names[$i],[IntPtr]::new([BitConverter]::ToInt64($b,8+8*$i)))}
      }
      'request-usage'{
        $f.Add('dwType',[BitConverter]::ToUInt32($b,0));$f.Add('Usage.cUsageIdentifier',[BitConverter]::ToUInt32($b,8))
        $f.Add('Usage.rgpszUsageIdentifier',[IntPtr]::new([BitConverter]::ToInt64($b,16)))
      }
      'usage-oid-pointers'{
        $count=[int]($h.bytesExpected/8);$d.itemsOriginal=[Collections.Generic.List[object]]::new($count)
        for($i=0;$i -lt $count;$i++){$d.itemsOriginal.Add([IntPtr]::new([BitConverter]::ToInt64($b,8*$i)))}
      }
      'private-headers'{
        $count=[int]($h.bytesExpected/32);$d.itemsOriginal=[Collections.Generic.List[object]]::new($count)
        for($i=0;$i -lt $count;$i++){
          $item=@{rangeOriginal=$h;indexOriginal=$i;wordsOriginal=$null;providerIdOriginal=$null;
            cbStructOriginal=$null;bytesOriginal=$null;pointerOriginal=[IntPtr]::Zero;
            disposition='API_VISIBLE_PRIVATE_HEADER_ALIAS_ONLY_PROVIDER_INTERNAL_LIFETIME';bodyReadAuthority=$false;
            bodyCaptureRequired=$false;internalFreeObserved=$false;
            lifetimeRuleOriginal='SDK_PROVIDER_PRIVATE_DATA_FREED_BEFORE_FINAL_POLICY_RETURN_NOT_ACTUAL_FREE_EVIDENCE'}
          $d.itemsOriginal.Add($item);$offset=32*$i
          $item.wordsOriginal=[uint32[]]::new(8);$item.providerIdOriginal=[byte[]]::new(16)
          for($word=0;$word -lt 8;$word++){$item.wordsOriginal[$word]=[BitConverter]::ToUInt32($b,$offset+4*$word)}
          [Array]::Copy($b,$offset+4,$item.providerIdOriginal,0,16)
          $item.cbStructOriginal=[BitConverter]::ToUInt32($b,$offset)
          $item.bytesOriginal=[BitConverter]::ToUInt32($b,$offset+20)
          $item.pointerOriginal=[IntPtr]::new([BitConverter]::ToInt64($b,$offset+24))
          if($item.cbStructOriginal -ne 32){throw 'Actual private provider header differs from SDK26100 x64.'}
          # SDK requires provider-private member data freed before Final Policy
          # returns. A retained pointer alias cannot authorize postreturn read.
        }
      }
      'sig-state'{
        foreach($entry in @(@('cbStruct',0),@('fFirstAttemptMade',24),@('fNoMoreSigs',28),
          @('cSecondarySigs',32),@('dwCurrentIndex',36),@('fSupportMultiSig',40),
          @('dwCryptoPolicySupport',44),@('iAttemptCount',48),@('fCheckedSealing',52))){$f.Add($entry[0],[BitConverter]::ToUInt32($b,$entry[1]))}
        foreach($entry in @(@('rhSecondarySigs',8),@('hPrimarySig',16),@('pSealingSignature',56))){
          $f.Add($entry[0],[IntPtr]::new([BitConverter]::ToInt64($b,$entry[1])))
        }
      }
      'sig-settings'{
        foreach($entry in @(@('cbStruct',0),@('dwIndex',4),@('dwFlags',8),@('cSecondarySigs',12),@('dwVerifiedSigIndex',16))){
          $f.Add($entry[0],[BitConverter]::ToUInt32($b,$entry[1]))
        }
        $f.Add('pCryptoPolicy',[IntPtr]::new([BitConverter]::ToInt64($b,24)))
      }
    }
    $d.returned=$true;return $d
  }catch{$d.exceptionOriginal=$_;throw}
}

function Test-Cli39ProviderFieldsBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal
  if($null -eq $capture){return $true}
  foreach($part in @('trust-step-errors','stores','private-headers','sig-state','sig-settings','secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')){
    if($null -ne $capture.providerFieldOriginals[$part] -and
        -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p $null $null '' '' $null $null $part)){return $false}
  }
  if(-not (Test-Cli39NamedProviderOidsBorrowClosed32 $p)){return $false}
  return $true
}

