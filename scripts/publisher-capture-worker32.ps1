# UNRUN first fixed provider-capture phase. This only captures original state
# and provider-data pointers; it does NOT mark complete provider capture returned.
function New-Cli39PublisherStateCaptureWorker32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation; $c=$p.consumerOriginal; $n=$c.publisherNativeOriginal
  if ($c.slot -cne 'cli39.windows-publisher-trust.v1' -or $null -eq $n -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $null -ne $n.closeWorkerOriginal -or $null -ne $n.storageReleaseOriginal -or
      $null -ne $n.providerCaptureOriginal) { throw 'SAME original once-only pre-close provider capture required.' }
  $verify=$n.verifyWorkerOriginal; $module=$n.wintrustModuleOriginal
  if ($null -eq $verify -or -not $verify.terminalJoined -or $verify.thread.IsAlive -or
      -not [object]::ReferenceEquals($verify.preparation,$p) -or
      -not [object]::ReferenceEquals($verify.nativeOriginal,$n) -or
      $verify.results[2] -ne 1 -or $verify.results[1] -ne 1 -or
      $null -ne $verify.exceptionOriginal[0] -or
      $null -eq $module -or -not $module.constructionTerminal -or
      -not [object]::ReferenceEquals($module.preparation,$p) -or
      -not [object]::ReferenceEquals($n.winVerifyTrustOriginal,$module.delegatesOriginal[0]) -or
      $null -eq $module.delegatesOriginal[1]) { throw 'Actual terminal native verification and SAME resident helper delegate required.' }
  $charge=$n.custodyReservationOriginal
  if($null -eq $charge -or -not [object]::ReferenceEquals($charge.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($charge.reservationOriginal,$p.reservation) -or
      -not [object]::ReferenceEquals($charge.budgetOriginal,$p.reservation.budget) -or
      $charge.usedChargeBytes -le 0 -or $charge.usedChargeBytes -gt $charge.limitBytes){throw 'SAME precharged full capture/classifier/custody/failure journal graph required.'}
  $capture=@{preparationOriginal=$p;nativeOriginal=$n;stateWorkerOriginal=$null;
    stateOriginal=[IntPtr]::Zero;providerDataOriginal=[IntPtr]::Zero;
    stateReadReturned=$false;providerCallReturned=$false;noStateObserved=$false;
    returned=$false;collectionTerminal=$false;custodyCompletionOriginal=@{preparationOriginal=$p;nativeOriginal=$n;captureOriginal=$null;
      sequenceOriginal=$null;classificationOriginal=$null;expectedComparisonOriginal=$null;attempted=$false;returned=$false;
      exceptionOriginal=$null;constructionTerminal=$false;state='API_VISIBLE_CUSTODY_ONLY_NOT_POLICY_OR_AUTHENTICATION'};exceptionOriginal=$null;signerOriginals=$null;
    counterSignerOriginals=$null;certificateOriginals=$null;timestampOriginals=$null;
    headerOriginal=$null;signerRosterOriginal=$null;sequenceOriginal=$null;
    namedOidParentsOriginal=$null;namedOidParentsConstructorOriginal=$null;
    providerFieldOriginals=@{'trust-step-errors'=$null;stores=$null;'private-headers'=$null;'sig-state'=$null;'sig-settings'=$null;'secondary-signatures'=$null;'sealing-signature'=$null;'strong-policy'=$null;'sealing-parameters'=$null;'sealing-digest'=$null;'provider-functions'=$null;'ui-functions'=$null;'ui-data'=$null;'request-usage'=$null;'usage-oid-pointers'=$null}}
  $n.providerCaptureOriginal=$capture
  $capture.custodyCompletionOriginal.captureOriginal=$capture
  $namedCtor=@{attempted=$true;returned=$false;exceptionOriginal=$null;terminal=$false}
  $capture.namedOidParentsConstructorOriginal=$namedCtor
  try{
    $book=$n.oidReservationOriginal
    if(-not $book.constructionTerminal -or $null -ne $book.constructionFailure -or
        -not [object]::ReferenceEquals($book.preparationOriginal,$p) -or $book.maximumOriginals -le 0){throw 'SAME precharged named OID roster backing required.'}
    $capture.namedOidParentsOriginal=[Collections.Generic.List[object]]::new($book.maximumOriginals)
    $namedCtor.returned=$true
  }catch{$namedCtor.exceptionOriginal=$_;throw}finally{$namedCtor.terminal=$true}
  $w=@{phase='provider-state';preparation=$p;nativeOriginal=$n;
    captureOriginal=$capture;moduleOwnerOriginal=$module;
    pointerOriginals=[IntPtr[]]::new(2);results=[int[]]::new(5);
    exceptionOriginal=[object[]]::new(1);context=$null;method=$null;il=$null;body=$null;
    thread=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinPendingActive=$false;joinMultiplicityUnknown=$false;
    joinPending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    joinFalse=@{returned=$false;value=$false;repeats=0L};
    joinTerminal=@{returned=$false;value=$true};
    joinException=@{present=$false;original=$null};constructionFailure=$null;constructionTerminal=$false}
  $capture.stateWorkerOriginal=$w
  try {
    if ([IntPtr]::Size -ne 8 -or $c.geometry.workerStackBytes -le 0 -or
        $c.geometry.workerStackBytes -gt [int]::MaxValue) { throw 'Genuine admitted x64 provider-capture stack geometry required.' }
    $call=$module.delegatesOriginal[1]
    $w.context=[object[]]@($n.wtdPointer,$call,$w.pointerOriginals,$w.results,$w.exceptionOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedProviderStateCapture',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il
    $state=$il.DeclareLocal([IntPtr]);$data=$il.DeclareLocal([IntPtr]);$failure=$il.DeclareLocal([Exception])
    function Emit-Cli39CaptureContext32([int]$Index,[Type]$Type,[bool]$ValueType=$false) {
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($ValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}
      else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39CaptureScalar32([int]$Index) {
      Emit-Cli39CaptureContext32 3 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $null=$il.BeginExceptionBlock()
    Emit-Cli39CaptureScalar32 0
    Emit-Cli39CaptureContext32 0 ([IntPtr]) $true
    # SDK26100 x64 WINTRUST_DATA.hWVTStateData offset56, within SAME WTD.
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,56)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('ReadIntPtr',[Type[]]@([IntPtr],[int])))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$state)
    Emit-Cli39CaptureContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$state);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    Emit-Cli39CaptureScalar32 1
    $noState=$il.DefineLabel();$done=$il.DefineLabel()
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$state)
    $il.Emit([Reflection.Emit.OpCodes]::Ldsfld,[IntPtr].GetField('Zero'))
    $il.Emit([Reflection.Emit.OpCodes]::Call,[IntPtr].GetMethod('op_Equality',[Type[]]@([IntPtr],[IntPtr])))
    $il.Emit([Reflection.Emit.OpCodes]::Brtrue,$noState)
    Emit-Cli39CaptureScalar32 2
    Emit-Cli39CaptureContext32 1 ($call.GetType())
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$state)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$call.GetType().GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$data)
    Emit-Cli39CaptureContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$data);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    Emit-Cli39CaptureScalar32 3
    $il.Emit([Reflection.Emit.OpCodes]::Br,$done)
    $il.MarkLabel($noState);Emit-Cli39CaptureScalar32 4
    $il.MarkLabel($done)
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
    Emit-Cli39CaptureContext32 4 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failure);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false
    return $w
  } catch {$w.constructionFailure=$_;throw} finally {$w.constructionTerminal=$true}
}

function Complete-Cli39PublisherStateCapture32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $WorkerOriginal.preparation
  $w=$WorkerOriginal;$p=$w.preparation;$capture=$w.captureOriginal
  if (-not [object]::ReferenceEquals($p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal,$capture) -or
      -not [object]::ReferenceEquals($capture.stateWorkerOriginal,$w) -or
      -not $w.terminalJoined -or $w.thread.IsAlive) {throw 'Actual SAME terminal provider-state capture required.'}
  $capture.stateOriginal=$w.pointerOriginals[0];$capture.providerDataOriginal=$w.pointerOriginals[1]
  $capture.stateReadReturned=($w.results[1] -eq 1)
  $capture.providerCallReturned=($w.results[3] -eq 1)
  $capture.noStateObserved=($w.results[4] -eq 1)
  $capture.exceptionOriginal=$w.exceptionOriginal[0]
  if($null -ne $capture.exceptionOriginal){throw $capture.exceptionOriginal}
  if(-not $capture.stateReadReturned -or
      (-not $capture.providerCallReturned -and -not $capture.noStateObserved)){throw 'Provider-state capture has no complete original result.'}
  # Full provider/signer/counterchain/DER/timestamp capture remains required.
  # In particular this result must not unlock the existing close worker.
  return $capture
}

# Source-fixed sequence; this frontier deliberately does not publish complete
# provider capture until the remaining named graph has actually been collected.
function Capture-Cli39PublisherProvider32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal
  if($null -ne $n.providerCaptureOriginal){throw 'Provider capture sequence cannot reset/retry an original.'}
  $w=New-Cli39PublisherStateCaptureWorker32 $p
  $capture=$n.providerCaptureOriginal
  $sequence=@{captureOriginal=$capture;preparationOriginal=$p;stateWorkerOriginal=$w;
    stateReturned=$false;headerReturned=$false;primarySignersReturned=$false;certificateChainAssociationsReturned=$false;
    noStateObserved=$false;zeroPrimarySignersObserved=$false;primarySignerCountOriginal=$null;
    returned=$false;exceptionOriginal=$null;state='INCOMPLETE_NAMED_PROVIDER_CAPTURE'}
  $capture.sequenceOriginal=$sequence
  try{
    $null=Start-Cli39PublisherWorker26 $w
    while(-not $w.terminalJoined){
      $null=Observe-Cli39PublisherWorker26 $w
      if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
    }
    $null=Complete-Cli39PublisherStateCapture32 $w
    $sequence.stateReturned=$true;$sequence.noStateObserved=$capture.noStateObserved
    if($capture.noStateObserved){
      # Complete only the actual negative API-visible result. This cannot
      # establish a signer, trust, current revocation or useful acceptance.
      if(-not $w.constructionTerminal -or $null -ne $w.constructionFailure -or
          -not $w.startReturned -or -not $w.terminalJoined -or $w.thread.IsAlive -or
          $w.results[0] -ne 1 -or $w.results[1] -ne 1 -or $w.results[2] -ne 0 -or
          $w.results[3] -ne 0 -or $w.results[4] -ne 1 -or
          $capture.stateOriginal -ne [IntPtr]::Zero -or
          $capture.providerDataOriginal -ne [IntPtr]::Zero -or
          $null -ne $capture.headerOriginal -or $null -ne $capture.signerRosterOriginal -or
          $capture.namedOidParentsOriginal.Count -ne 0){throw 'Actual complete no-state original required.'}
      foreach($field in $capture.providerFieldOriginals.Values){if($null -ne $field){throw 'No-state capture has a contradictory issued field.'}}
      $sequence.returned=$true;$sequence.state='COMPLETE_API_VISIBLE_NO_STATE_NEGATIVE_RESULT'
      $null=Complete-Cli39PublisherCaptureCustody32 $p
      return $capture
    }
    $null=Capture-Cli39ProviderHeaderRange32 $p
    $null=Copy-Cli39ProviderHeader32 $p
    $sequence.headerReturned=$true
    $null=Capture-Cli39ProviderFields32 $p
    [uint32]$count=$capture.headerOriginal.decodedOriginal.fieldsOriginal['csSigners']
    $sequence.primarySignerCountOriginal=$count
    if($count -eq 0){
      $sequence.zeroPrimarySignersObserved=$true
      $sequence.primarySignersReturned=$true
      $null=Capture-Cli39PublisherChains32 $p
      if(-not (Test-Cli39PublisherChainCaptureComplete32 $p)){throw 'Actual selected supported chain range collection remains incomplete.'}
      $sequence.certificateChainAssociationsReturned=$true
      $null=Compare-Cli39PublisherExpectedCertificate32 $p
      $null=Complete-Cli39PublisherSupportedRawGraph32 $p
      $null=Complete-Cli39PublisherCaptureCustody32 $p
      return $capture
    }
    for([long]$index=0;$index -lt $count;$index++){
      $signerWorker=New-Cli39ProviderSignerWorker32 $p
      if($signerWorker.signerOriginal.indexOriginal -ne $index){throw 'Actual primary signer roster order differs.'}
      $null=Start-Cli39PublisherWorker26 $signerWorker
      while(-not $signerWorker.terminalJoined){
        $null=Observe-Cli39PublisherWorker26 $signerWorker
        if(-not $signerWorker.terminalJoined){$null=[Threading.Thread]::Yield()}
      }
      $signer=Complete-Cli39ProviderSigner32 $signerWorker
      $null=Capture-Cli39PrimarySignerHeader32 $p $signer
      $null=Capture-Cli39SignerCertificates32 $p $signer
      $null=Capture-Cli39SignerInfo32 $p $signer
      $null=Capture-Cli39SignerFields32 $p $signer
      $null=Capture-Cli39ProviderAttributes32 $p $signer
      [uint32]$counterCount=$signer.headerRangeOriginal.decodedOriginal.fieldsOriginal['csCounterSigners']
      for([long]$counterIndex=0;$counterIndex -lt $counterCount;$counterIndex++){
        $counterWorker=New-Cli39ProviderSignerWorker32 $p $signer
        if($counterWorker.signerOriginal.indexOriginal -ne $counterIndex){throw 'Actual counter signer roster order differs.'}
        $null=Start-Cli39PublisherWorker26 $counterWorker
        while(-not $counterWorker.terminalJoined){
          $null=Observe-Cli39PublisherWorker26 $counterWorker
          if(-not $counterWorker.terminalJoined){$null=[Threading.Thread]::Yield()}
        }
        $counterSigner=Complete-Cli39ProviderSigner32 $counterWorker
        $null=Capture-Cli39PrimarySignerHeader32 $p $counterSigner
        $null=Capture-Cli39SignerCertificates32 $p $counterSigner
        $null=Capture-Cli39SignerInfo32 $p $counterSigner
        $null=Capture-Cli39SignerFields32 $p $counterSigner
        $null=Capture-Cli39ProviderAttributes32 $p $counterSigner
      }
    }
    $sequence.primarySignersReturned=$true
    $null=Capture-Cli39PublisherChains32 $p
    if(-not (Test-Cli39PublisherChainCaptureComplete32 $p)){throw 'Actual selected supported chain range collection remains incomplete.'}
    foreach($primary in $capture.signerOriginals){
      foreach($cert in $primary.certificateOriginals){$null=Join-Cli39PublisherCertificateChain32 $p $cert}
      foreach($counter in $primary.counterSignerOriginals){
        foreach($cert in $counter.certificateOriginals){$null=Join-Cli39PublisherCertificateChain32 $p $cert}
      }
    }
    $sequence.certificateChainAssociationsReturned=$true
    $null=Compare-Cli39PublisherExpectedCertificate32 $p
    $null=Complete-Cli39PublisherSupportedRawGraph32 $p
    $null=Complete-Cli39PublisherCaptureCustody32 $p
    # Supported API-visible custody now permits only independently eligible cleanup.
    # Complete counterchain/cert DER/CMSG attributes/timestamps/SIGSTATE and
    # source/namespace/profile joins remain required before useful policy acceptance.
    return $capture
  }catch{$sequence.exceptionOriginal=$_;$capture.exceptionOriginal=$_;throw}
}




