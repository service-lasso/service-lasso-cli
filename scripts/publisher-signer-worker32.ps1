# Fixed primary/counter signer helper; no pointer/signature/callback input.
function Test-Cli39ProviderSignerMember32 {
  param($CaptureOriginal,$SignerOriginal)
  $capture=$CaptureOriginal;$s=$SignerOriginal
  if($null -eq $s -or $null -eq $capture.signerOriginals -or
      -not [object]::ReferenceEquals($s.captureOriginal,$capture)){return $false}
  if($capture.signerOriginals.Contains($s)){return $null -eq $s.primarySignerOriginal}
  $primary=$s.primarySignerOriginal
  return ($null -ne $primary -and $capture.signerOriginals.Contains($primary) -and
    $null -eq $primary.primarySignerOriginal -and $null -ne $primary.counterSignerOriginals -and
    $primary.counterSignerOriginals.Contains($s))
}
function New-Cli39ProviderSignerWorker32 {
  param([Parameter(Mandatory)]$OriginalPreparation,$PrimarySignerOriginal=$null)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$h=$capture.headerOriginal;$d=$h.decodedOriginal
  $m=$n.wintrustModuleOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
      -not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($d.headerOriginal,$h) -or -not $d.returned -or
      -not $h.copyReturned -or $capture.returned -or
      -not $capture.stateWorkerOriginal.terminalJoined -or $capture.stateWorkerOriginal.thread.IsAlive -or
      $capture.providerDataOriginal -ne $capture.stateWorkerOriginal.pointerOriginals[1] -or
      -not [object]::ReferenceEquals($capture.stateWorkerOriginal.moduleOwnerOriginal,$m) -or
      $null -ne $n.closeWorkerOriginal -or $null -ne $n.storageReleaseOriginal -or
      $null -eq $m -or -not $n.moduleOwners.Contains($m) -or
      -not [object]::ReferenceEquals($m.preparation,$p) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$m.receiverKey],$m) -or
      -not $m.constructionTerminal -or $null -ne $m.constructorFailure -or
      -not $m.workerOriginal.terminalJoined -or $m.workerOriginal.thread.IsAlive -or
      $m.workerOriginal.results[4] -ne 1 -or $m.module -eq [IntPtr]::Zero -or
      $m.namesOriginal[2] -cne 'WTHelperGetProvSignerFromChain' -or
      $null -eq $m.delegatesOriginal[2]){throw 'SAME copied provider header and resident fixed signer helper required.'}
  $owner=$capture;$rosterKey='signerRosterOriginal';$recordsKey='signerOriginals'
  if($null -ne $PrimarySignerOriginal){
    $primary=$PrimarySignerOriginal
    if(-not $capture.signerOriginals.Contains($primary) -or
        $null -ne $primary.primarySignerOriginal -or
        -not [object]::ReferenceEquals($primary.captureOriginal,$capture) -or
        $null -eq $primary.headerRangeOriginal -or -not $primary.headerRangeOriginal.copyReturned -or
        -not $primary.headerRangeOriginal.decodedOriginal.returned){throw 'SAME actual copied primary signer required for counter helper.'}
    $owner=$primary;$h=$primary.headerRangeOriginal;$d=$h.decodedOriginal
    $rosterKey='counterRosterOriginal';$recordsKey='counterSignerOriginals'
    [uint32]$count=$d.fieldsOriginal['csCounterSigners']
  }else{[uint32]$count=$d.fieldsOriginal['csSigners']}
  # Count is the actual provider observation, never a guessed policy cap.
  # Complete measured allocation/stack/retained overlaps remain mandatory.
  if($count -eq 0 -or $count -gt [int]::MaxValue){throw 'Actual provider signer count cannot supply a next supported roster member.'}
  if($null -eq $owner[$recordsKey]){
    if($null -ne $owner[$rosterKey]){throw 'Unknown/failed actual signer roster constructor is retained; no retry.'}
    $owner[$rosterKey]=@{headerOriginal=$h;decodedOriginal=$d;countOriginal=$count;
      recordsOriginal=$null;constructorAttempted=$true;constructorReturned=$false;exceptionOriginal=$null}
    try{
      $owner[$recordsKey]=[Collections.Generic.List[object]]::new([int]$count)
      $owner[$rosterKey].recordsOriginal=$owner[$recordsKey]
      $owner[$rosterKey].constructorReturned=$true
    }catch{$owner[$rosterKey].exceptionOriginal=$_;throw}
  }
  $roster=$owner[$rosterKey];$records=$owner[$recordsKey]
  if(-not $roster.constructorReturned -or
      -not [object]::ReferenceEquals($roster.headerOriginal,$h) -or
      -not [object]::ReferenceEquals($roster.decodedOriginal,$d) -or
      -not [object]::ReferenceEquals($roster.recordsOriginal,$records) -or
      $roster.countOriginal -ne $count -or $records.Count -ge $count){throw 'SAME actual remaining signer roster required.'}
  if($records.Count -gt 0 -and
      -not $records[$records.Count-1].pointerReturned){throw 'Previous signer result must actually complete; no retry.'}
  $s=@{captureOriginal=$capture;headerOriginal=$h;rosterOriginal=$roster;
    indexOriginal=[uint32]$records.Count;primarySignerOriginal=$PrimarySignerOriginal;
    counterSignerOriginals=$null;counterRosterOriginal=$null;
    certificateOriginals=$null;certificateRosterOriginal=$null;certificateSequenceOriginal=$null;workerOriginal=$null;
    pointerOriginal=[IntPtr]::Zero;pointerReturned=$false;completionAttempted=$false;
    headerRangeOriginal=$null;signerInfoOriginal=$null;
    signerFieldOriginals=@{issuer=$null;serial=$null;'hash-parameters'=$null;'encryption-parameters'=$null;
      'encrypted-hash'=$null;'auth-attributes'=$null;'unauth-attributes'=$null};exceptionOriginal=$null}
  $records.Add($s)
  $w=@{phase='provider-signer';preparation=$p;nativeOriginal=$n;captureOriginal=$capture;
    signerOriginal=$s;moduleOwnerOriginal=$m;callOriginal=$m.delegatesOriginal[2];
    pointerOriginals=[IntPtr[]]::new(1);results=[int[]]::new(2);exceptionOriginal=[object[]]::new(1);
    context=$null;method=$null;il=$null;body=$null;thread=$null;
    startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinPendingActive=$false;joinMultiplicityUnknown=$false;
    joinPending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    joinFalse=@{returned=$false;value=$false;repeats=0L};joinTerminal=@{returned=$false;value=$true};
    joinException=@{present=$false;original=$null};constructionTerminal=$false;constructionFailure=$null}
  $s.workerOriginal=$w
  try{
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){throw 'Genuine complete publisher worker geometry required.'}
    [uint32]$signerIndex=$s.indexOriginal;[int]$counter=0;[uint32]$counterIndex=0
    if($null -ne $PrimarySignerOriginal){$signerIndex=$PrimarySignerOriginal.indexOriginal;$counter=1;$counterIndex=$s.indexOriginal}
    $w.context=[object[]]@($w.callOriginal,$capture.providerDataOriginal,$signerIndex,
      $w.pointerOriginals,$w.results,$w.exceptionOriginal,$counter,$counterIndex)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedProviderSigner',[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il;$failureLocal=$il.DeclareLocal([Exception]);$pointer=$il.DeclareLocal([IntPtr])
    function Emit-Cli39SignerContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($Type.IsValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39SignerMark32([int]$Index){
      Emit-Cli39SignerContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $null=$il.BeginExceptionBlock();Emit-Cli39SignerMark32 0
    Emit-Cli39SignerContext32 0 ($w.callOriginal.GetType())
    Emit-Cli39SignerContext32 1 ([IntPtr]);Emit-Cli39SignerContext32 2 ([uint32])
    Emit-Cli39SignerContext32 6 ([int]);Emit-Cli39SignerContext32 7 ([uint32])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.callOriginal.GetType().GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$pointer)
    Emit-Cli39SignerContext32 3 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointer);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    Emit-Cli39SignerMark32 1
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
    Emit-Cli39SignerContext32 5 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false
    return $w
  }catch{$w.constructionFailure=$_;$s.exceptionOriginal=$_;throw}
  finally{$w.constructionTerminal=$true}
}

function Complete-Cli39ProviderSigner32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $WorkerOriginal.preparation
  $w=$WorkerOriginal;$s=$w.signerOriginal;$capture=$w.captureOriginal
  if(-not [object]::ReferenceEquals($s.workerOriginal,$w) -or
      -not [object]::ReferenceEquals($s.captureOriginal,$capture) -or
      -not (Test-Cli39ProviderSignerMember32 $capture $s) -or $s.completionAttempted -or
      -not $w.terminalJoined -or $w.thread.IsAlive){throw 'Actual SAME once-only terminal signer helper required.'}
  $s.completionAttempted=$true
  $s.pointerOriginal=$w.pointerOriginals[0];$s.exceptionOriginal=$w.exceptionOriginal[0]
  if($null -ne $s.exceptionOriginal){throw $s.exceptionOriginal}
  if($w.results[1] -ne 1){throw 'No actual returned signer helper result.'}
  $s.pointerReturned=$true
  # Null is an actual result retained before rejection; never blindly retry.
  if($s.pointerOriginal -eq [IntPtr]::Zero){throw 'Actual provider signer helper returned null.'}
  return $s
}

function Capture-Cli39PrimarySignerHeader32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$s=$SignerOriginal
  if(-not (Test-Cli39ProviderSignerMember32 $capture $s) -or
      -not [object]::ReferenceEquals($s.captureOriginal,$capture) -or
      -not $s.pointerReturned -or $null -ne $s.headerRangeOriginal){throw 'SAME once-only actual primary signer required.'}
  $h=@{captureOriginal=$capture;providerPointerOriginal=$s.pointerOriginal;
    stateWorkerOriginal=$capture.stateWorkerOriginal;signerOriginal=$s;bytesExpected=64;used=0;
    pagesOriginal=$null;rangeReturned=$false;rawBytesOriginal=$null;copyWorkerOriginal=$null;
    copyReturned=$false;decodedOriginal=$null;exceptionOriginal=$null}
  $s.headerRangeOriginal=$h
  try{
    $null=Get-Cli39ProviderHeaderRange32 $p $s
    $h.pagesOriginal=[Collections.Generic.List[object]]::new(64)
    $h.rawBytesOriginal=[byte[]]::new(64)
    while(-not $h.rangeReturned){
      $w=New-Cli39ProviderPageWorker32 $p $s
      $null=Start-Cli39PublisherWorker26 $w
      while(-not $w.terminalJoined){
        $null=Observe-Cli39PublisherWorker26 $w
        if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
      }
      $null=Complete-Cli39ProviderPage32 $w
    }
    $null=Copy-Cli39ProviderHeader32 $p $s
    return $h
  }catch{$h.exceptionOriginal=$_;throw}
}

function Capture-Cli39SignerInfo32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$s=$SignerOriginal
  if(-not (Test-Cli39ProviderSignerMember32 $capture $s) -or
      -not $s.headerRangeOriginal.copyReturned -or -not $s.headerRangeOriginal.decodedOriginal.returned -or
      $null -ne $s.signerInfoOriginal){throw 'Actual SAME copied signer64 before once-only signer-info required.'}
  $h=@{captureOriginal=$capture;providerPointerOriginal=$s.headerRangeOriginal.decodedOriginal.fieldsOriginal['psSigner'];
    stateWorkerOriginal=$capture.stateWorkerOriginal;signerOriginal=$s;signerPart='signer-info';
    bytesExpected=136;used=0;pagesOriginal=$null;rangeReturned=$false;rawBytesOriginal=$null;
    copyWorkerOriginal=$null;copyReturned=$false;decodedOriginal=$null;versionOriginal=$null;exceptionOriginal=$null}
  $s.signerInfoOriginal=$h
  try{
    $null=Get-Cli39ProviderHeaderRange32 $p $s $null '' 'signer-info'
    $h.pagesOriginal=[Collections.Generic.List[object]]::new(136)
    $h.rawBytesOriginal=[byte[]]::new(136)
    while(-not $h.rangeReturned){
      $w=New-Cli39ProviderPageWorker32 $p $s $null '' 'signer-info'
      $null=Start-Cli39PublisherWorker26 $w
      while(-not $w.terminalJoined){
        $null=Observe-Cli39PublisherWorker26 $w
        if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
      }
      $null=Complete-Cli39ProviderPage32 $w
    }
    $null=Copy-Cli39ProviderHeader32 $p $s $null '' 'signer-info'
    $null=Capture-Cli39NamedProviderOid32 $p $s 'hash-algorithm'
    $null=Capture-Cli39NamedProviderOid32 $p $s 'encryption-algorithm'
    return $h
  }catch{$h.exceptionOriginal=$_;throw}
}

function Capture-Cli39SignerFields32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$s=$SignerOriginal
  if(-not (Test-Cli39ProviderSignerMember32 $capture $s) -or
      -not $s.signerInfoOriginal.copyReturned -or -not $s.signerInfoOriginal.decodedOriginal.returned){throw 'Actual SAME CMSG136 parent before fixed field capture required.'}
  $info=$s.signerInfoOriginal;$fields=$info.decodedOriginal.fieldsOriginal
  foreach($part in @('issuer','serial','hash-parameters','encryption-parameters','encrypted-hash','auth-attributes','unauth-attributes')){
    switch -CaseSensitive ($part){
      'issuer'{$countField='Issuer.cbData';$pointerField='Issuer.pbData';$unit=1L}
      'serial'{$countField='SerialNumber.cbData';$pointerField='SerialNumber.pbData';$unit=1L}
      'hash-parameters'{$countField='HashAlgorithm.Parameters.cbData';$pointerField='HashAlgorithm.Parameters.pbData';$unit=1L}
      'encryption-parameters'{$countField='HashEncryptionAlgorithm.Parameters.cbData';$pointerField='HashEncryptionAlgorithm.Parameters.pbData';$unit=1L}
      'encrypted-hash'{$countField='EncryptedHash.cbData';$pointerField='EncryptedHash.pbData';$unit=1L}
      'auth-attributes'{$countField='AuthAttrs.cAttr';$pointerField='AuthAttrs.rgAttr';$unit=24L}
      'unauth-attributes'{$countField='UnauthAttrs.cAttr';$pointerField='UnauthAttrs.rgAttr';$unit=24L}
    }
    if($null -ne $s.signerFieldOriginals[$part]){throw 'Actual fixed signer field cannot be reset/retried.'}
    [uint32]$count=$fields[$countField];[long]$extent=$unit*[long]$count
    $h=@{captureOriginal=$capture;providerPointerOriginal=$fields[$pointerField];
      stateWorkerOriginal=$capture.stateWorkerOriginal;signerOriginal=$s;signerPart=$part;
      parentInfoOriginal=$info;countOriginal=$count;unitBytesOriginal=$unit;bytesExpected=$extent;used=0;
      pagesOriginal=$null;rangeReturned=$false;rawBytesOriginal=$null;copyWorkerOriginal=$null;
      copyReturned=$false;absenceObserved=$false;attributesOriginal=$null;
      attributesConstructorOriginal=$null;exceptionOriginal=$null}
    $s.signerFieldOriginals[$part]=$h
    try{
      $null=Get-Cli39ProviderHeaderRange32 $p $s $null '' $part
      $h.pagesOriginal=[Collections.Generic.List[object]]::new([int]$extent)
      $h.rawBytesOriginal=[byte[]]::new([int]$extent)
      if($extent -eq 0){
        # The actual parent count/length establishes an absent body. Preserve
        # its pointer alias; no native copy/read or invented returned worker.
        $h.absenceObserved=$true;$h.rangeReturned=$true
        continue
      }
      while(-not $h.rangeReturned){
        $w=New-Cli39ProviderPageWorker32 $p $s $null '' $part
        $null=Start-Cli39PublisherWorker26 $w
        while(-not $w.terminalJoined){
          $null=Observe-Cli39PublisherWorker26 $w
          if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
        }
        $null=Complete-Cli39ProviderPage32 $w
      }
      $null=Copy-Cli39ProviderHeader32 $p $s $null '' $part
    }catch{$h.exceptionOriginal=$_;throw}
  }
  return $s
}

# Quiescence is independent of successful helper results or complete capture.
function Test-Cli39ProviderSignerBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation,$PrimarySignerOriginal=$null)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$capture=$n.providerCaptureOriginal
  if($null -eq $capture){return $true}
  if(-not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n)){return $false}
  $owner=$capture;$header=$capture.headerOriginal;$rosterKey='signerRosterOriginal';$recordsKey='signerOriginals'
  if($null -ne $PrimarySignerOriginal){
    if(-not $capture.signerOriginals.Contains($PrimarySignerOriginal) -or
        $null -ne $PrimarySignerOriginal.primarySignerOriginal){return $false}
    $owner=$PrimarySignerOriginal;$header=$owner.headerRangeOriginal
    $rosterKey='counterRosterOriginal';$recordsKey='counterSignerOriginals'
  }
  $roster=$owner[$rosterKey];$records=$owner[$recordsKey]
  if($null -eq $roster){return $null -eq $records}
  if(-not [object]::ReferenceEquals($roster.headerOriginal,$header) -or
      -not [object]::ReferenceEquals($roster.decodedOriginal,$header.decodedOriginal)){return $false}
  if($null -eq $records){
    # The failed List constructor precedes all signer worker/native entry.
    # Its CLR partial original is retained at host lifetime, not reset/retried.
    return (-not $roster.constructorReturned -and $null -ne $roster.exceptionOriginal)
  }
  if(-not $roster.constructorReturned -or
      -not [object]::ReferenceEquals($roster.recordsOriginal,$records) -or
      $records.Count -gt $roster.countOriginal){return $false}
  foreach($s in $records){
    if(-not [object]::ReferenceEquals($s.captureOriginal,$capture) -or
        -not [object]::ReferenceEquals($s.rosterOriginal,$roster)){return $false}
    $w=$s.workerOriginal
    # No native helper can start before its original is published in $s.
    if($null -eq $w){continue}
    if(-not [object]::ReferenceEquals($w.signerOriginal,$s) -or
        -not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
        -not [object]::ReferenceEquals($w.preparation,$p) -or
        -not [object]::ReferenceEquals($w.nativeOriginal,$n) -or
        -not $w.constructionTerminal -or $w.joinPendingActive -or
        $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
    if($w.startAttempted){
      if(-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive){return $false}
    }elseif($w.results[0] -ne 0 -or $w.pointerOriginals[0] -ne [IntPtr]::Zero){return $false}
    if($null -ne $s.headerRangeOriginal -and
        -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p $s)){return $false}
    if($null -ne $s.signerInfoOriginal -and
        -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p $s $null '' 'signer-info')){return $false}
    foreach($part in @('issuer','serial','hash-parameters','encryption-parameters','encrypted-hash','auth-attributes','unauth-attributes')){
      if($null -ne $s.signerFieldOriginals[$part] -and
          -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p $s $null '' $part)){return $false}
    }
    if(-not (Test-Cli39ProviderCertificateBorrowClosed32 $p $s)){return $false}
    if(-not (Test-Cli39ProviderAttributesBorrowClosed32 $p $s)){return $false}
    # Exactly two source-selected levels: primary -> SDK helper counter.
    # Further nested counter fields remain recorded, unfinished obligations.
    if($null -eq $PrimarySignerOriginal -and
        -not (Test-Cli39ProviderSignerBorrowClosed32 $p $s)){return $false}
  }
  return $true
}
