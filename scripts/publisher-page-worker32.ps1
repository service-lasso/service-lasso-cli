# Fixed provider-data240/primary-signer64 header queries only. VirtualQuery
# comes from SAME root query ABI; no arbitrary native query/pointer callback.
function Get-Cli39ProviderHeaderRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation,$SignerOriginal=$null,
    $CertificateOriginal=$null,[ValidateSet('','provider-cert','cert-context','cert-der')][string]$CertificatePart='',
    [ValidateSet('','signer-info','issuer','serial','hash-parameters','encryption-parameters','encrypted-hash','auth-attributes','unauth-attributes')][string]$SignerPart='',
    $AttributeOriginal=$null,$AttributeValueOriginal=$null,
    [ValidateSet('','trust-step-errors','stores','private-headers','sig-state','sig-settings','secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')][string]$ProviderPart='')
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal
  if($ProviderPart -cne ''){
    if($null -ne $SignerOriginal -or $null -ne $CertificateOriginal -or $null -ne $AttributeOriginal -or
        $null -ne $AttributeValueOriginal -or $CertificatePart -cne '' -or $SignerPart -cne ''){throw 'Provider role requires only its SAME actual DATA parent.'}
    $parent=$capture.headerOriginal
    if($ProviderPart -cin @('secondary-signatures','sealing-signature')){$parent=$capture.providerFieldOriginals['sig-state']}
    elseif($ProviderPart -ceq 'strong-policy'){$parent=$capture.providerFieldOriginals['sig-settings']}
    elseif($ProviderPart -cin @('sealing-parameters','sealing-digest')){$parent=$capture.providerFieldOriginals['sealing-signature']}
    elseif($ProviderPart -ceq 'ui-functions'){$parent=$capture.providerFieldOriginals['provider-functions']}
    elseif($ProviderPart -ceq 'ui-data'){$parent=$capture.providerFieldOriginals['ui-functions']}
    elseif($ProviderPart -ceq 'usage-oid-pointers'){$parent=$capture.providerFieldOriginals['request-usage']}
    if((-not $parent.copyReturned -and -not $parent.absenceObserved) -or -not $parent.decodedOriginal.returned){throw 'Actual copied or observed-absent fixed provider parent required.'}
    $f=$parent.decodedOriginal.fieldsOriginal
    switch -CaseSensitive ($ProviderPart){
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
    $h=$capture.providerFieldOriginals[$ProviderPart]
    if($extent -gt [int]::MaxValue -or ($extent -gt 0 -and $pointer -eq [IntPtr]::Zero) -or
        -not [object]::ReferenceEquals($h.parentHeaderOriginal,$parent) -or
        $h.providerPart -cne $ProviderPart -or $h.bytesExpected -ne $extent -or
        $h.providerPointerOriginal -ne $pointer){throw 'SAME supported complete provider field required.'}
  }elseif($null -ne $AttributeOriginal){
    $a=$AttributeOriginal;$v=$AttributeValueOriginal
    if($null -ne $CertificateOriginal -or $null -ne $SignerOriginal -or
        $CertificatePart -cne '' -or $SignerPart -cne '' -or
        -not (Test-Cli39ProviderAttributeMember32 $capture $a) -or -not $a.decodedReturned){throw 'Only SAME actual attribute may select this fixed range.'}
    if($null -eq $v){
      $h=$a.valuesRangeOriginal;$pointer=$a.valuesPointerOriginal;[long]$extent=16L*[long]$a.countOriginal
    }else{
      if(-not (Test-Cli39ProviderAttributeValueMember32 $capture $a $v) -or -not $v.decodedReturned -or
          -not [object]::ReferenceEquals($v.arrayOriginal,$a.valuesRangeOriginal)){throw 'SAME actual decoded attribute value required.'}
      $h=$v.rangeOriginal;$pointer=$v.pointerOriginal;[long]$extent=$v.countOriginal
    }
    if($extent -gt [int]::MaxValue -or ($extent -gt 0 -and $pointer -eq [IntPtr]::Zero) -or
        -not [object]::ReferenceEquals($h.attributeOriginal,$a) -or
        -not [object]::ReferenceEquals($h.attributeValueOriginal,$v) -or
        $h.bytesExpected -ne $extent -or $h.providerPointerOriginal -ne $pointer){throw 'SAME supported complete attribute range required.'}
  }elseif($null -ne $AttributeValueOriginal){throw 'Attribute value requires its SAME actual parent attribute.'}
  elseif($null -ne $CertificateOriginal){
    $cert=$CertificateOriginal;$w=$cert.workerOriginal
    if($null -ne $SignerOriginal -or $SignerPart -cne '' -or -not (Test-Cli39ProviderCertificateMember32 $capture $cert) -or
        -not $cert.pointerReturned -or -not $w.terminalJoined -or $w.thread.IsAlive -or
        $cert.pointerOriginal -eq [IntPtr]::Zero -or $cert.pointerOriginal -ne $w.pointersOriginal[0]){throw 'SAME actual certificate ORIGINAL required.'}
    switch -CaseSensitive ($CertificatePart){
      'provider-cert'{$h=$cert.providerHeaderOriginal;$pointer=$cert.pointerOriginal;$extent=88}
      'cert-context'{
        $parent=$cert.providerHeaderOriginal
        if(-not $parent.copyReturned -or -not $parent.decodedOriginal.returned){throw 'Actual provider certificate88 copy required.'}
        $h=$cert.contextHeaderOriginal;$pointer=$parent.decodedOriginal.fieldsOriginal['pCert'];$extent=40
      }
      'cert-der'{
        $parent=$cert.contextHeaderOriginal
        if(-not $parent.copyReturned -or -not $parent.decodedOriginal.returned){throw 'Actual certificate context40 copy required.'}
        $h=$cert.derOriginal;$pointer=$parent.decodedOriginal.fieldsOriginal['pbCertEncoded']
        [uint32]$extent=$parent.decodedOriginal.fieldsOriginal['cbCertEncoded']
        if($extent -eq 0 -or $extent -gt [int]::MaxValue){throw 'Actual supported complete encoded certificate extent required.'}
      }
      default{throw 'No arbitrary certificate range role exists.'}
    }
    if($pointer -eq [IntPtr]::Zero -or -not [object]::ReferenceEquals($h.certificateOriginal,$cert) -or
        $h.certificatePart -cne $CertificatePart -or $h.bytesExpected -ne $extent -or
        $h.providerPointerOriginal -ne $pointer){throw 'SAME source-derived certificate range required.'}
  }elseif($CertificatePart -cne ''){throw 'Certificate range role requires its actual certificate ORIGINAL.'}
  elseif($null -eq $SignerOriginal){
    if($SignerPart -cne ''){throw 'Signer-info role requires its actual signer ORIGINAL.'}
    $h=$capture.headerOriginal
    if($h.bytesExpected -ne 240 -or $null -ne $h.signerOriginal -or
        $h.providerPointerOriginal -ne $capture.providerDataOriginal -or
        $h.providerPointerOriginal -ne $capture.stateWorkerOriginal.pointerOriginals[1]){throw 'SAME actual provider-data240 header required.'}
  }else{
    $s=$SignerOriginal;$h=$s.headerRangeOriginal;$w=$s.workerOriginal;$extent=64;$pointer=$s.pointerOriginal
    if($SignerPart -ceq 'signer-info'){
      if(-not $h.copyReturned -or -not $h.decodedOriginal.returned){throw 'Actual signer64 parent required before signer-info.'}
      $pointer=$h.decodedOriginal.fieldsOriginal['psSigner'];$h=$s.signerInfoOriginal;$extent=136
      if($h.signerPart -cne 'signer-info'){throw 'SAME fixed signer-info role required.'}
    }elseif($SignerPart -cne ''){
      $info=$s.signerInfoOriginal
      if(-not $info.copyReturned -or -not $info.decodedOriginal.returned){throw 'Actual CMSG136 parent required before fixed signer field.'}
      $fields=$info.decodedOriginal.fieldsOriginal
      switch -CaseSensitive ($SignerPart){
        'issuer'{$countField='Issuer.cbData';$pointerField='Issuer.pbData';$unit=1L}
        'serial'{$countField='SerialNumber.cbData';$pointerField='SerialNumber.pbData';$unit=1L}
        'hash-parameters'{$countField='HashAlgorithm.Parameters.cbData';$pointerField='HashAlgorithm.Parameters.pbData';$unit=1L}
        'encryption-parameters'{$countField='HashEncryptionAlgorithm.Parameters.cbData';$pointerField='HashEncryptionAlgorithm.Parameters.pbData';$unit=1L}
        'encrypted-hash'{$countField='EncryptedHash.cbData';$pointerField='EncryptedHash.pbData';$unit=1L}
        'auth-attributes'{$countField='AuthAttrs.cAttr';$pointerField='AuthAttrs.rgAttr';$unit=24L}
        'unauth-attributes'{$countField='UnauthAttrs.cAttr';$pointerField='UnauthAttrs.rgAttr';$unit=24L}
        default{throw 'No generic signer field role exists.'}
      }
      [long]$extent=$unit*[long][uint32]$fields[$countField]
      $pointer=$fields[$pointerField];$h=$s.signerFieldOriginals[$SignerPart]
      if($extent -gt [int]::MaxValue -or $h.signerPart -cne $SignerPart -or
          -not [object]::ReferenceEquals($h.parentInfoOriginal,$info)){throw 'SAME complete supported signer field extent required.'}
    }
    if(-not (Test-Cli39ProviderSignerMember32 $capture $s) -or
        -not [object]::ReferenceEquals($s.captureOriginal,$capture) -or
        -not [object]::ReferenceEquals($h.signerOriginal,$s) -or
        -not [object]::ReferenceEquals($w.signerOriginal,$s) -or
        -not $s.pointerReturned -or -not $w.terminalJoined -or $w.thread.IsAlive -or
        $s.pointerOriginal -eq [IntPtr]::Zero -or $s.pointerOriginal -ne $w.pointerOriginals[0] -or
        ($extent -gt 0 -and $pointer -eq [IntPtr]::Zero) -or $h.bytesExpected -ne $extent -or
        $h.providerPointerOriginal -ne $pointer){throw 'SAME actual source-derived signer header required.'}
  }
  if(-not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($h.stateWorkerOriginal,$capture.stateWorkerOriginal)){throw 'SAME actual provider header range parent required.'}
  return $h
}

function Test-Cli39ProviderHeaderRangeBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation,$SignerOriginal=$null,
    $CertificateOriginal=$null,[ValidateSet('','provider-cert','cert-context','cert-der')][string]$CertificatePart='',
    [ValidateSet('','signer-info','issuer','serial','hash-parameters','encryption-parameters','encrypted-hash','auth-attributes','unauth-attributes')][string]$SignerPart='',
    $AttributeOriginal=$null,$AttributeValueOriginal=$null,
    [ValidateSet('','trust-step-errors','stores','private-headers','sig-state','sig-settings','secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')][string]$ProviderPart='')
  try{$h=Get-Cli39ProviderHeaderRange32 $OriginalPreparation $SignerOriginal $CertificateOriginal $CertificatePart $SignerPart $AttributeOriginal $AttributeValueOriginal $ProviderPart}catch{return $false}
  if($h.ContainsKey('absenceObserved') -and $h.absenceObserved){
    if($h.bytesExpected -ne 0 -or $h.used -ne 0 -or $h.copyReturned -or
        $null -ne $h.copyWorkerOriginal -or
        ($null -ne $h.pagesOriginal -and $h.pagesOriginal.Count -ne 0)){return $false}
  }
  foreach($page in $h.pagesOriginal){
    $w=$page.workerOriginal
    if($null -eq $w -or -not [object]::ReferenceEquals($w.pageOriginal,$page) -or
        -not [object]::ReferenceEquals($w.headerOriginal,$h) -or
        -not $w.constructionTerminal -or $w.joinPendingActive -or
        $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
    if($w.startAttempted -and
        (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
    if($w.results[0] -eq 1 -and $w.results[1] -ne 1){return $false}
    if($w.results[1] -eq 1 -and
        ($w.results[2] -ne 1 -or $w.results[3] -ne 1 -or $null -ne $w.exceptionOriginal[1])){return $false}
  }
  $w=$h.copyWorkerOriginal
  if($null -ne $w){
    if(-not [object]::ReferenceEquals($w.headerOriginal,$h) -or
        -not $w.constructionTerminal -or $w.joinPendingActive -or
        $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
    if($w.startAttempted -and
        (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
  }
  return $true
}
function Capture-Cli39ProviderHeaderRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$capture=$n.providerCaptureOriginal
  if($null -eq $capture -or $null -ne $capture.headerOriginal -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
      $null -eq $capture.stateWorkerOriginal -or -not $capture.stateWorkerOriginal.terminalJoined -or
      $capture.stateWorkerOriginal.thread.IsAlive -or -not $capture.stateReadReturned -or
      -not $capture.providerCallReturned -or $capture.providerDataOriginal -eq [IntPtr]::Zero){
    throw 'SAME actual returned provider data pointer required before page capture.'
  }
  $h=@{captureOriginal=$capture;providerPointerOriginal=$capture.providerDataOriginal;
    stateWorkerOriginal=$capture.stateWorkerOriginal;bytesExpected=240;used=0;pagesOriginal=$null;
    rangeReturned=$false;rawBytesOriginal=$null;copyWorkerOriginal=$null;
    copyReturned=$false;decodedOriginal=$null;signerOriginal=$null;exceptionOriginal=$null}
  $capture.headerOriginal=$h
  try{
    # Every accepted query covers at least one of the fixed240 header bytes.
    # Hence240 original records suffice without guessing OS page size or adding
    # a file/count admission gate. Their complete allocator charge is required.
    $h.pagesOriginal=[Collections.Generic.List[object]]::new(240)
    $h.rawBytesOriginal=[byte[]]::new(240)
    while(-not $h.rangeReturned){
      $w=New-Cli39ProviderPageWorker32 $p
      $null=Start-Cli39PublisherWorker26 $w
      while(-not $w.terminalJoined){
        $null=Observe-Cli39PublisherWorker26 $w
        if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
      }
      $null=Complete-Cli39ProviderPage32 $w
    }
    # This range observation does not copy the header or complete provider
    # capture. SAME-state supervised copy and all signers/chains/DER/timestamps
    # remain required before close/acceptance.
    return $h
  }catch{$h.exceptionOriginal=$_;throw}
}

function New-Cli39ProviderPageWorker32 {
  param([Parameter(Mandatory)]$OriginalPreparation,$SignerOriginal=$null,
    $CertificateOriginal=$null,[ValidateSet('','provider-cert','cert-context','cert-der')][string]$CertificatePart='',
    [ValidateSet('','signer-info','issuer','serial','hash-parameters','encryption-parameters','encrypted-hash','auth-attributes','unauth-attributes')][string]$SignerPart='',
    $AttributeOriginal=$null,$AttributeValueOriginal=$null,
    [ValidateSet('','trust-step-errors','stores','private-headers','sig-state','sig-settings','secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')][string]$ProviderPart='')
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$h=Get-Cli39ProviderHeaderRange32 $p $SignerOriginal $CertificateOriginal $CertificatePart $SignerPart $AttributeOriginal $AttributeValueOriginal $ProviderPart
  $root=$c.rootCaptureOriginal;$q=$root.nativeQueryOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$p.receiver) -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
      -not $capture.stateReadReturned -or -not $capture.providerCallReturned -or
      $capture.providerDataOriginal -eq [IntPtr]::Zero -or $null -ne $n.closeWorkerOriginal -or
      $null -ne $n.storageReleaseOriginal -or $null -eq $h -or
      -not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($h.stateWorkerOriginal,$capture.stateWorkerOriginal) -or
      $h.used -lt 0 -or $h.used -ge $h.bytesExpected -or
      $h.pagesOriginal.Count -ge $h.bytesExpected -or -not $q.complete -or
      $q.namesOriginal[12] -cne 'VirtualQuery' -or $null -eq $q.delegatesOriginal[12]){
    throw 'SAME original fixed provider header/page source required.'
  }
  $start=$h.providerPointerOriginal.ToInt64()
  if($start -le 0 -or $start -gt ([long]::MaxValue-$h.bytesExpected)){throw 'Complete provider header pointer extent required.'}
  if($h.pagesOriginal.Count -gt 0 -and -not $h.pagesOriginal[$h.pagesOriginal.Count-1].accepted){
    throw 'Previous actual page must be accepted before another query; no retry.'
  }
  $record=@{offset=$h.used;address=[IntPtr]::new($start+$h.used);bytesOriginal=$null;
    workerOriginal=$null;accepted=$false;baseAddress=$null;regionBytes=$null;
    state=$null;protection=$null;type=$null;coveredBytes=0;exceptionOriginal=$null}
  $h.pagesOriginal.Add($record)
  $w=@{phase='provider-page';preparation=$p;nativeOriginal=$n;captureOriginal=$capture;
    headerOriginal=$h;signerOriginal=$SignerOriginal;certificateOriginal=$CertificateOriginal;
    attributeOriginal=$AttributeOriginal;attributeValueOriginal=$AttributeValueOriginal;
    providerPart=$ProviderPart;
    certificatePart=$CertificatePart;signerPart=$SignerPart;pageOriginal=$record;callOriginal=$q.delegatesOriginal[12];
    pinOriginal=[Runtime.InteropServices.GCHandle[]]::new(1);
    results=[int[]]::new(8);sizeOriginal=[UIntPtr[]]::new(1);exceptionOriginal=[object[]]::new(2);
    context=$null;method=$null;il=$null;body=$null;thread=$null;
    startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinPendingActive=$false;joinMultiplicityUnknown=$false;
    joinPending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    joinFalse=@{returned=$false;value=$false;repeats=0L};
    joinTerminal=@{returned=$false;value=$true};joinException=@{present=$false;original=$null};
    constructionTerminal=$false;constructionFailure=$null;completionAttempted=$false}
  $record.workerOriginal=$w
  try{
    $record.bytesOriginal=[byte[]]::new(48)
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){
      throw 'Complete genuine publisher worker geometry required.'
    }
    $w.context=[object[]]@($w.callOriginal,$record.address,$record.bytesOriginal,$w.pinOriginal,
      $w.results,$w.sizeOriginal,$w.exceptionOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedProviderPage',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il;$exception=$il.DeclareLocal([Exception])
    $size=$il.DeclareLocal([UIntPtr])
    function Emit-Cli39PageContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($Type.IsValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}
      else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39PageMark32([int]$Index){
      Emit-Cli39PageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39PageException32([int]$Index){
      Emit-Cli39PageContext32 6 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$exception);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    Emit-Cli39PageMark32 0
    Emit-Cli39PageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39PageContext32 2 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle])
    Emit-Cli39PageMark32 1
    Emit-Cli39PageMark32 4
    Emit-Cli39PageContext32 0 ($w.callOriginal.GetType());Emit-Cli39PageContext32 1 ([IntPtr])
    Emit-Cli39PageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,48);$il.Emit([Reflection.Emit.OpCodes]::Conv_U)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.callOriginal.GetType().GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$size)
    Emit-Cli39PageContext32 5 ([UIntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$size);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    Emit-Cli39PageMark32 5
    Emit-Cli39PageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,6)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39PageMark32 7
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
    Emit-Cli39PageException32 0
    $il.BeginFinallyBlock();$skip=$il.DefineLabel();$null=$il.BeginExceptionBlock()
    Emit-Cli39PageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
    Emit-Cli39PageMark32 2
    Emit-Cli39PageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
    Emit-Cli39PageMark32 3;$il.MarkLabel($skip)
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
    Emit-Cli39PageException32 1;$il.EndExceptionBlock();$il.EndExceptionBlock()
    $il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false;return $w
  }catch{$w.constructionFailure=$_;throw}finally{$w.constructionTerminal=$true}
}

function Complete-Cli39ProviderPage32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $WorkerOriginal.preparation
  $w=$WorkerOriginal;$p=$w.preparation;$h=$w.headerOriginal;$r=$w.pageOriginal
  if(-not [object]::ReferenceEquals((Get-Cli39PublisherWorkerOriginal26 $w),$w) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $r.accepted -or
      $w.completionAttempted){throw 'SAME actual terminal once-only provider page result required.'}
  $w.completionAttempted=$true
  try{
    foreach($exception in $w.exceptionOriginal){if($null -ne $exception){throw $exception}}
    if($w.results[0] -ne 1 -or $w.results[1] -ne 1 -or $w.results[2] -ne 1 -or
        $w.results[3] -ne 1 -or $w.results[5] -ne 1 -or $w.results[7] -ne 1 -or
        $w.sizeOriginal[0].ToUInt64() -ne 48){throw 'Incomplete actual provider VirtualQuery/pin cleanup result.'}
    $b=$r.bytesOriginal
    $base=[BitConverter]::ToInt64($b,0);$region=[BitConverter]::ToUInt64($b,24)
    $state=[BitConverter]::ToUInt32($b,32);$protection=[BitConverter]::ToUInt32($b,36)
    $type=[BitConverter]::ToUInt32($b,40);$cursor=$r.address.ToInt64()
    $r.baseAddress=$base;$r.regionBytes=$region;$r.state=$state;$r.protection=$protection;$r.type=$type
    if($base -lt 0 -or $base -gt $cursor -or $region -eq 0 -or
        $region -gt ([long]::MaxValue-$base) -or ($base+[long]$region) -le $cursor -or
        $state -ne 0x1000 -or ($protection -band 0x100) -ne 0 -or
        ($protection -band 0xff) -notin @(2,4,8,0x20,0x40,0x80)){
      throw 'Complete committed readable native provider region required.'
    }
    $end=$h.providerPointerOriginal.ToInt64()+$h.bytesExpected
    $covered=[Math]::Min(($base+[long]$region),$end)-$cursor
    if($covered -le 0 -or $covered -gt ($h.bytesExpected-$h.used) -or $r.offset -ne $h.used){throw 'Actual page range must advance the SAME remaining provider header.'}
    $r.coveredBytes=[int]$covered;$r.accepted=$true;$h.used+=[int]$covered
    if($h.used -eq $h.bytesExpected){$h.rangeReturned=$true}
    return $r
  }catch{$r.exceptionOriginal=$_;throw}
}

function Copy-Cli39ProviderHeader32 {
  param([Parameter(Mandatory)]$OriginalPreparation,$SignerOriginal=$null,
    $CertificateOriginal=$null,[ValidateSet('','provider-cert','cert-context','cert-der')][string]$CertificatePart='',
    [ValidateSet('','signer-info','issuer','serial','hash-parameters','encryption-parameters','encrypted-hash','auth-attributes','unauth-attributes')][string]$SignerPart='',
    $AttributeOriginal=$null,$AttributeValueOriginal=$null,
    [ValidateSet('','trust-step-errors','stores','private-headers','sig-state','sig-settings','secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')][string]$ProviderPart='')
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$h=Get-Cli39ProviderHeaderRange32 $p $SignerOriginal $CertificateOriginal $CertificatePart $SignerPart $AttributeOriginal $AttributeValueOriginal $ProviderPart
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
      -not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($h.stateWorkerOriginal,$capture.stateWorkerOriginal) -or
      -not $h.rangeReturned -or $h.used -ne $h.bytesExpected -or $h.rawBytesOriginal.Length -ne $h.bytesExpected -or
      $null -ne $h.copyWorkerOriginal -or $null -ne $n.closeWorkerOriginal -or
      $null -ne $n.storageReleaseOriginal){throw 'SAME complete page-checked provider header required before once-only copy.'}
  foreach($page in $h.pagesOriginal){
    if(-not $page.accepted -or -not $page.workerOriginal.terminalJoined -or
        $page.workerOriginal.thread.IsAlive){throw 'Every actual header page must be terminal and accepted.'}
  }
  $w=@{phase='provider-header';preparation=$p;nativeOriginal=$n;captureOriginal=$capture;
    headerOriginal=$h;signerOriginal=$SignerOriginal;certificateOriginal=$CertificateOriginal;
    attributeOriginal=$AttributeOriginal;attributeValueOriginal=$AttributeValueOriginal;
    providerPart=$ProviderPart;
    certificatePart=$CertificatePart;signerPart=$SignerPart;results=[int[]]::new(2);exceptionOriginal=[object[]]::new(1);
    context=$null;method=$null;il=$null;body=$null;thread=$null;
    startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinPendingActive=$false;joinMultiplicityUnknown=$false;
    joinPending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    joinFalse=@{returned=$false;value=$false;repeats=0L};joinTerminal=@{returned=$false;value=$true};
    joinException=@{present=$false;original=$null};constructionTerminal=$false;constructionFailure=$null}
  $h.copyWorkerOriginal=$w
  try{
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){throw 'Complete publisher copy worker geometry required.'}
    $w.context=[object[]]@($h.providerPointerOriginal,$h.rawBytesOriginal,$w.results,$w.exceptionOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedProviderHeaderCopy',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il;$failureLocal=$il.DeclareLocal([Exception])
    function Emit-Cli39HeaderContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($Type.IsValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}
      else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39HeaderMark32([int]$Index){
      Emit-Cli39HeaderContext32 2 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $null=$il.BeginExceptionBlock();Emit-Cli39HeaderMark32 0
    Emit-Cli39HeaderContext32 0 ([IntPtr]);Emit-Cli39HeaderContext32 1 ([byte[]])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int]$h.bytesExpected)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('Copy',[Type[]]@([IntPtr],[byte[]],[int],[int])))
    Emit-Cli39HeaderMark32 1
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
    Emit-Cli39HeaderContext32 3 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false;$w.constructionTerminal=$true
    $null=Start-Cli39PublisherWorker26 $w
    while(-not $w.terminalJoined){
      $null=Observe-Cli39PublisherWorker26 $w
      if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
    }
    if($w.thread.IsAlive -or $null -ne $w.exceptionOriginal[0] -or $w.results[1] -ne 1){throw 'Actual provider header copy did not return completely.'}
    if($ProviderPart -cin @('sig-state','sig-settings','strong-policy','provider-functions','ui-functions','ui-data')){
      $h.cbStructOriginal=[BitConverter]::ToUInt32($h.rawBytesOriginal,0)
      if($h.cbStructOriginal -ne $h.bytesExpected){throw 'Actual SDK26100 signature metadata layout differs.'}
    }elseif($ProviderPart -cne ''){<# Fixed raw arrays have no leading cbStruct. #>}
    elseif($SignerPart -ceq 'signer-info'){$h.versionOriginal=[BitConverter]::ToUInt32($h.rawBytesOriginal,0)}
    elseif($SignerPart -ceq '' -and $CertificatePart -cne 'cert-der' -and $null -eq $AttributeOriginal){
      # CERT_CONTEXT has encoding at0, not cbStruct. Its layout is fixed40.
      if($CertificatePart -ceq 'cert-context'){$h.encodingOriginal=[BitConverter]::ToUInt32($h.rawBytesOriginal,0)}
      else{
        $h.cbStructOriginal=[BitConverter]::ToUInt32($h.rawBytesOriginal,0)
        if($h.cbStructOriginal -ne $h.bytesExpected){throw 'Actual provider header differs from admitted fixed SDK26100 x64 layout.'}
      }
    }
    $h.copyReturned=$true
    if($ProviderPart -cne ''){$null=Decode-Cli39ProviderField32 $p $ProviderPart}
    elseif($SignerPart -ceq 'signer-info'){$null=Decode-Cli39SignerInfo32 $p $SignerOriginal}
    elseif($null -ne $CertificateOriginal){
      if($CertificatePart -cne 'cert-der'){$null=Decode-Cli39CertificateHeader32 $p $CertificateOriginal $CertificatePart}
    }elseif($SignerPart -ceq '' -and $null -eq $AttributeOriginal){
      if($null -eq $SignerOriginal){$null=Decode-Cli39ProviderHeader32 $p}
      else{$null=Decode-Cli39PrimarySignerHeader32 $p $SignerOriginal}
    }
    # Full raw header is now retained. No signer/counterchain/DER/timestamp
    # capture or trust/native input authority follows from this copy alone.
    return $h
  }catch{$h.exceptionOriginal=$_;throw}
  finally{if(-not $w.constructionTerminal){$w.constructionFailure=$h.exceptionOriginal;$w.constructionTerminal=$true}}
}
