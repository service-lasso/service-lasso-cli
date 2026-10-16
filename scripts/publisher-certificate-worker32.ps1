# Fixed SDK certificate helper; input is a SAME actual signer ORIGINAL only.
function Test-Cli39ProviderCertificateMember32 {
  param($CaptureOriginal,$CertificateOriginal)
  $capture=$CaptureOriginal;$cert=$CertificateOriginal
  if($null -eq $cert -or -not [object]::ReferenceEquals($cert.captureOriginal,$capture)){return $false}
  $s=$cert.signerOriginal
  return ((Test-Cli39ProviderSignerMember32 $capture $s) -and
    $null -ne $s.certificateOriginals -and $s.certificateOriginals.Contains($cert))
}

function New-Cli39ProviderCertificateWorker32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$s=$SignerOriginal;$h=$s.headerRangeOriginal;$m=$n.wintrustModuleOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not (Test-Cli39ProviderSignerMember32 $capture $s) -or
      -not $s.pointerReturned -or -not $h.copyReturned -or -not $h.decodedOriginal.returned -or
      -not [object]::ReferenceEquals($h.signerOriginal,$s) -or
      $s.pointerOriginal -ne $s.workerOriginal.pointerOriginals[0] -or
      $h.providerPointerOriginal -ne $s.workerOriginal.pointerOriginals[0] -or
      -not $s.workerOriginal.terminalJoined -or $s.workerOriginal.thread.IsAlive -or
      $capture.returned -or $null -ne $n.closeWorkerOriginal -or $null -ne $n.storageReleaseOriginal -or
      -not $m.constructionTerminal -or $null -ne $m.constructorFailure -or
      -not [object]::ReferenceEquals($s.workerOriginal.moduleOwnerOriginal,$m) -or
      $m.namesOriginal[3] -cne 'WTHelperGetProvCertFromChain' -or
      $null -eq $m.delegatesOriginal[3]){throw 'SAME copied signer and fixed resident certificate helper required.'}
  [uint32]$count=$h.decodedOriginal.fieldsOriginal['csCertChain']
  if($count -eq 0 -or $count -gt [int]::MaxValue){throw 'Actual certificate count cannot supply a next supported roster member.'}
  if($null -eq $s.certificateOriginals){
    if($null -ne $s.certificateRosterOriginal){throw 'Actual partial certificate roster retained; no constructor retry.'}
    $s.certificateRosterOriginal=@{signerOriginal=$s;headerOriginal=$h;countOriginal=$count;
      recordsOriginal=$null;constructorAttempted=$true;constructorReturned=$false;exceptionOriginal=$null}
    try{
      $s.certificateOriginals=[Collections.Generic.List[object]]::new([int]$count)
      $s.certificateRosterOriginal.recordsOriginal=$s.certificateOriginals
      $s.certificateRosterOriginal.constructorReturned=$true
    }catch{$s.certificateRosterOriginal.exceptionOriginal=$_;throw}
  }
  $roster=$s.certificateRosterOriginal;$records=$s.certificateOriginals
  if(-not $roster.constructorReturned -or
      -not [object]::ReferenceEquals($roster.signerOriginal,$s) -or
      -not [object]::ReferenceEquals($roster.headerOriginal,$h) -or
      -not [object]::ReferenceEquals($roster.recordsOriginal,$records) -or
      $roster.countOriginal -ne $count -or $records.Count -ge $count){throw 'SAME remaining actual certificate roster required.'}
  if($records.Count -gt 0 -and -not $records[$records.Count-1].pointerReturned){throw 'Previous actual certificate helper must return; no retry.'}
  $cert=@{captureOriginal=$capture;signerOriginal=$s;rosterOriginal=$roster;
    indexOriginal=[uint32]$records.Count;workerOriginal=$null;pointerOriginal=[IntPtr]::Zero;
    pointerReturned=$false;completionAttempted=$false;exceptionOriginal=$null;
    chainAssociationOriginal=$null;chainAssociationAttempted=$false;providerHeaderOriginal=$null;contextHeaderOriginal=$null;derOriginal=$null}
  $records.Add($cert)
  $w=@{phase='provider-certificate';preparation=$p;nativeOriginal=$n;captureOriginal=$capture;
    certificateOriginal=$cert;moduleOwnerOriginal=$m;callOriginal=$m.delegatesOriginal[3];
    pointersOriginal=[IntPtr[]]::new(1);results=[int[]]::new(2);exceptionOriginal=[object[]]::new(1);
    context=$null;method=$null;il=$null;body=$null;thread=$null;
    startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinPendingActive=$false;joinMultiplicityUnknown=$false;
    joinPending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    joinFalse=@{returned=$false;value=$false;repeats=0L};joinTerminal=@{returned=$false;value=$true};
    joinException=@{present=$false;original=$null};constructionTerminal=$false;constructionFailure=$null}
  $cert.workerOriginal=$w
  try{
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){throw 'Genuine complete certificate worker geometry required.'}
    $w.context=[object[]]@($w.callOriginal,$s.pointerOriginal,$cert.indexOriginal,$w.pointersOriginal,$w.results,$w.exceptionOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedProviderCertificate',[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il;$failureLocal=$il.DeclareLocal([Exception]);$pointer=$il.DeclareLocal([IntPtr])
    function Emit-Cli39CertificateContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($Type.IsValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39CertificateMark32([int]$Index){
      Emit-Cli39CertificateContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $null=$il.BeginExceptionBlock();Emit-Cli39CertificateMark32 0
    Emit-Cli39CertificateContext32 0 ($w.callOriginal.GetType())
    Emit-Cli39CertificateContext32 1 ([IntPtr]);Emit-Cli39CertificateContext32 2 ([uint32])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.callOriginal.GetType().GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$pointer)
    Emit-Cli39CertificateContext32 3 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointer);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    Emit-Cli39CertificateMark32 1
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
    Emit-Cli39CertificateContext32 5 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false;return $w
  }catch{$w.constructionFailure=$_;$cert.exceptionOriginal=$_;throw}
  finally{$w.constructionTerminal=$true}
}

function Complete-Cli39ProviderCertificate32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $WorkerOriginal.preparation
  $w=$WorkerOriginal;$cert=$w.certificateOriginal
  if(-not (Test-Cli39ProviderCertificateMember32 $w.captureOriginal $cert) -or
      -not [object]::ReferenceEquals($cert.workerOriginal,$w) -or $cert.completionAttempted -or
      -not $w.terminalJoined -or $w.thread.IsAlive){throw 'Actual SAME once-only terminal certificate helper required.'}
  $cert.completionAttempted=$true;$cert.pointerOriginal=$w.pointersOriginal[0];$cert.exceptionOriginal=$w.exceptionOriginal[0]
  if($null -ne $cert.exceptionOriginal){throw $cert.exceptionOriginal}
  if($w.results[1] -ne 1){throw 'No actual returned certificate helper result.'}
  $cert.pointerReturned=$true
  if($cert.pointerOriginal -eq [IntPtr]::Zero){throw 'Actual certificate helper returned null; no retry.'}
  return $cert
}

function Capture-Cli39ProviderCertificate32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$CertificateOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$cert=$CertificateOriginal
  if(-not (Test-Cli39ProviderCertificateMember32 $capture $cert) -or -not $cert.pointerReturned){throw 'Actual SAME returned certificate helper required.'}
  foreach($part in @('provider-cert','cert-context','cert-der')){
    switch -CaseSensitive ($part){
      'provider-cert'{$key='providerHeaderOriginal';$pointer=$cert.pointerOriginal;$extent=88}
      'cert-context'{
        $key='contextHeaderOriginal';$parent=$cert.providerHeaderOriginal
        if(-not $parent.copyReturned -or -not $parent.decodedOriginal.returned){throw 'Actual provider certificate parent required.'}
        $pointer=$parent.decodedOriginal.fieldsOriginal['pCert'];$extent=40
      }
      'cert-der'{
        $key='derOriginal';$parent=$cert.contextHeaderOriginal
        if(-not $parent.copyReturned -or -not $parent.decodedOriginal.returned){throw 'Actual certificate context parent required.'}
        $pointer=$parent.decodedOriginal.fieldsOriginal['pbCertEncoded'];[uint32]$extent=$parent.decodedOriginal.fieldsOriginal['cbCertEncoded']
      }
    }
    if($null -ne $cert[$key]){throw 'Actual certificate range cannot be reset or copied again.'}
    $h=@{captureOriginal=$capture;providerPointerOriginal=$pointer;stateWorkerOriginal=$capture.stateWorkerOriginal;
      signerOriginal=$null;certificateOriginal=$cert;certificatePart=$part;bytesExpected=$extent;used=0;
      pagesOriginal=$null;rangeReturned=$false;rawBytesOriginal=$null;copyWorkerOriginal=$null;
      copyReturned=$false;decodedOriginal=$null;encodingOriginal=$null;exceptionOriginal=$null}
    $cert[$key]=$h
    try{
      $null=Get-Cli39ProviderHeaderRange32 $p $null $cert $part
      $h.pagesOriginal=[Collections.Generic.List[object]]::new([int]$extent)
      $h.rawBytesOriginal=[byte[]]::new([int]$extent)
      while(-not $h.rangeReturned){
        $w=New-Cli39ProviderPageWorker32 $p $null $cert $part
        $null=Start-Cli39PublisherWorker26 $w
        while(-not $w.terminalJoined){
          $null=Observe-Cli39PublisherWorker26 $w
          if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
        }
        $null=Complete-Cli39ProviderPage32 $w
      }
      $null=Copy-Cli39ProviderHeader32 $p $null $cert $part
    }catch{$h.exceptionOriginal=$_;$cert.exceptionOriginal=$_;throw}
  }
  return $cert
}

function Capture-Cli39SignerCertificates32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$s=$SignerOriginal
  if(-not (Test-Cli39ProviderSignerMember32 $capture $s) -or
      -not $s.headerRangeOriginal.copyReturned -or -not $s.headerRangeOriginal.decodedOriginal.returned -or
      $null -ne $s.certificateSequenceOriginal){throw 'Actual SAME once-only copied signer certificate sequence required.'}
  [uint32]$count=$s.headerRangeOriginal.decodedOriginal.fieldsOriginal['csCertChain']
  $sequence=@{signerOriginal=$s;headerOriginal=$s.headerRangeOriginal;countOriginal=$count;
    zeroCountObserved=($count -eq 0);returned=$false;exceptionOriginal=$null}
  $s.certificateSequenceOriginal=$sequence
  try{
    for([long]$index=0;$index -lt $count;$index++){
      $w=New-Cli39ProviderCertificateWorker32 $p $s
      if($w.certificateOriginal.indexOriginal -ne $index){throw 'Actual certificate roster order differs.'}
      $null=Start-Cli39PublisherWorker26 $w
      while(-not $w.terminalJoined){
        $null=Observe-Cli39PublisherWorker26 $w
        if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
      }
      $cert=Complete-Cli39ProviderCertificate32 $w
      $null=Capture-Cli39ProviderCertificate32 $p $cert
    }
    $sequence.returned=$true;return $sequence
  }catch{$sequence.exceptionOriginal=$_;throw}
}

function Test-Cli39ProviderCertificateBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$s=$SignerOriginal
  if(-not (Test-Cli39ProviderSignerMember32 $capture $s)){return $false}
  $roster=$s.certificateRosterOriginal;$records=$s.certificateOriginals
  if($null -eq $roster){return $null -eq $records}
  if(-not [object]::ReferenceEquals($roster.signerOriginal,$s) -or
      -not [object]::ReferenceEquals($roster.headerOriginal,$s.headerRangeOriginal)){return $false}
  if($null -eq $records){return (-not $roster.constructorReturned -and $null -ne $roster.exceptionOriginal)}
  if(-not $roster.constructorReturned -or -not [object]::ReferenceEquals($roster.recordsOriginal,$records) -or
      $records.Count -gt $roster.countOriginal){return $false}
  foreach($cert in $records){
    if(-not (Test-Cli39ProviderCertificateMember32 $capture $cert) -or
        -not [object]::ReferenceEquals($cert.rosterOriginal,$roster)){return $false}
    $w=$cert.workerOriginal
    # Publication precedes every worker constructor/Start/native entry.
    if($null -eq $w){continue}
    if(-not [object]::ReferenceEquals($w.certificateOriginal,$cert) -or
        -not [object]::ReferenceEquals($w.preparation,$p) -or
        -not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
        -not $w.constructionTerminal -or $w.joinPendingActive -or
        $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
    if($w.startAttempted){
      if(-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive){return $false}
    }elseif($w.results[0] -ne 0 -or $w.pointersOriginal[0] -ne [IntPtr]::Zero){return $false}
    foreach($part in @('provider-cert','cert-context','cert-der')){
      $key=switch -CaseSensitive ($part){
        'provider-cert'{'providerHeaderOriginal'}
        'cert-context'{'contextHeaderOriginal'}
        'cert-der'{'derOriginal'}
      }
      if($null -ne $cert[$key] -and
          -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p $null $cert $part)){return $false}
    }
  }
  return $true
}
