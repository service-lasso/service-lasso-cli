# Numeric correspondence from actual copied API fields only. In particular,
# sftVerifyAsOf and cache-only success are not timestamp/revocation freshness.
function Observe-Cli39PublisherTimeResults32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$capture=$n.providerCaptureOriginal
  $root=$p.consumerOriginal.rootCaptureOriginal;$transport=$root.windowsPolicyFoundationTransportOriginal
  if($n.ContainsKey('timeResultOriginal') -or -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $capture.returned -or -not $capture.custodyCompletionOriginal.returned -or
      -not $transport.returned -or -not [object]::ReferenceEquals($transport.rootOriginal,$root)){
    throw 'SAME once-only completed publisher custody/operand transport required for time observations.'}
  $o=@{preparationOriginal=$p;captureOriginal=$capture;transportOriginal=$transport;
    chargeOriginal=$n.custodyReservationOriginal;providerHeaderOriginal=$capture.headerOriginal;
    providerLowOriginal=$null;providerHighOriginal=$null;providerTimeOriginal=$null;
    numericOperandsOriginal=$transport.operandsOriginal.numericOriginal;
    timestampModeOriginal=$transport.operandsOriginal.mapOriginal['timestampMode'];
    signerRecordsOriginal=$null;providerTimeInIntervalObserved=$false;returned=$false;
    exceptionOriginal=$null;terminal=$false;qualified=$false;
    state='RAW_TIME_CORRESPONDENCE_NOT_AUTHENTIC_TIMESTAMP_OR_FRESHNESS'}
  $n.timeResultOriginal=$o
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies time observation construction.'}
    [decimal]$count=0
    foreach($primary in $capture.signerOriginals){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies time observation roster count.'}
      if(-not (Test-Cli39ProviderSignerMember32 $capture $primary) -or
          $null -ne $primary.primarySignerOriginal -or
          -not (Test-Cli39PublisherSignerRawGraph32 $p $primary)){
        throw 'Actual completed primary/counter custody roster required for time precharge.'}
      $count++;$count+=$primary.counterSignerOriginals.Count
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies time observation charge after roster count.'}
    $charge=$o.chargeOriginal
    if($null -eq $charge -or -not [object]::ReferenceEquals($charge.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($charge.reservationOriginal,$p.reservation) -or
        $charge.timeObservationChargeAttempted -or $count -gt [int]::MaxValue){throw 'SAME finite once-only time observation charge required.'}
    $charge.timeObservationChargeAttempted=$true;$charge.timeObservationCountOriginal=$count
    $charge.timeObservationChargeBytes=$count*$charge.perSignerChargeBytes
    if($charge.usedChargeBytes+$charge.timeObservationChargeBytes -gt $charge.limitBytes){throw 'Full time observation records exceed SAME custody subdivision.'}
    $charge.usedChargeBytes+=$charge.timeObservationChargeBytes
    $o.signerRecordsOriginal=[Collections.Generic.List[object]]::new([int]$count)
    if($null -eq $capture.headerOriginal){
      if(-not $capture.sequenceOriginal.noStateObserved -or $count -ne 0){throw 'Actual absent-state time observation differs.'}
    }else{
      $f=$capture.headerOriginal.decodedOriginal.fieldsOriginal
      $o.providerLowOriginal=$f['sftSystemTimeLow'];$o.providerHighOriginal=$f['sftSystemTimeHigh']
      if($o.providerLowOriginal -isnot [uint32] -or $o.providerHighOriginal -isnot [uint32]){throw 'Actual copied provider FILETIME DWORDs required.'}
      $o.providerTimeOriginal=[uint64]([decimal]$o.providerLowOriginal+4294967296D*[decimal]$o.providerHighOriginal)
      $o.providerTimeInIntervalObserved=([decimal]$o.providerTimeOriginal -ge $o.numericOperandsOriginal.notBeforeFileTime -and
        [decimal]$o.providerTimeOriginal -le $o.numericOperandsOriginal.notAfterFileTime)
      foreach($primary in $capture.signerOriginals){
        # Traverse without a concatenated roster. Counter signers retain their
        # exact source-selected primary relationship and original indices.
        for($index=-1;$index -lt $primary.counterSignerOriginals.Count;$index++){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies signer time traversal.'}
          $signer=if($index -eq -1){$primary}else{$primary.counterSignerOriginals[$index]}
          $r=@{signerOriginal=$signer;primaryOriginal=$primary;counterIndexOriginal=$index;
            headerOriginal=$signer.headerRangeOriginal;lowOriginal=$null;highOriginal=$null;
            timeOriginal=$null;selectedValidationEqualObserved=$false;insideIntervalObserved=$false;returned=$false}
          $o.signerRecordsOriginal.Add($r)
          if(-not (Test-Cli39ProviderSignerMember32 $capture $signer) -or
              -not $r.headerOriginal.decodedOriginal.returned -or
              ($index -ge 0 -and -not [object]::ReferenceEquals($signer.primarySignerOriginal,$primary))){throw 'Actual copied SAME signer time fields required.'}
          $f=$r.headerOriginal.decodedOriginal.fieldsOriginal
          $r.lowOriginal=$f['sftVerifyAsOfLow'];$r.highOriginal=$f['sftVerifyAsOfHigh']
          if($r.lowOriginal -isnot [uint32] -or $r.highOriginal -isnot [uint32]){throw 'Actual copied signer FILETIME DWORDs required.'}
          $r.timeOriginal=[uint64]([decimal]$r.lowOriginal+4294967296D*[decimal]$r.highOriginal)
          $r.selectedValidationEqualObserved=([decimal]$r.timeOriginal -eq $o.numericOperandsOriginal.validationFileTime)
          $r.insideIntervalObserved=([decimal]$r.timeOriginal -ge $o.numericOperandsOriginal.notBeforeFileTime -and
            [decimal]$r.timeOriginal -le $o.numericOperandsOriginal.notAfterFileTime)
          $r.returned=$true
        }
      }
    }
    if($o.signerRecordsOriginal.Count -ne $count){throw 'Actual time observation roster changed after precharge.'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies time observation publication.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}
