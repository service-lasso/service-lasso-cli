# Actual supported API observations; transported operands do not authenticate policy.
function Observe-Cli39PublisherEvaluationResults32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$root=$p.consumerOriginal.rootCaptureOriginal
  $transport=$root.windowsPolicyFoundationTransportOriginal;$anchors=$n.anchorResultOriginal
  if($n.ContainsKey('evaluationResultOriginal') -or -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $capture.returned -or -not $anchors.returned -or -not $anchors.terminal -or
      $null -ne $anchors.exceptionOriginal -or -not [object]::ReferenceEquals($anchors.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($anchors.transportOriginal,$transport)){
    throw 'SAME once-only completed actual publisher anchor/transport required.'}
  $o=@{preparationOriginal=$p;captureOriginal=$capture;transportOriginal=$transport;anchorOriginal=$anchors;
    osOperandsOriginal=$transport.operandsOriginal.basisOriginals[0];
    revocationOperandsOriginal=$transport.operandsOriginal.basisOriginals[4];
    providerFieldsOriginal=$null;providerSettingsEqualObserved=$false;providerStateAbsentObserved=$false;
    providerFlagsOriginal=$null;revocationModeMaskOriginal=[uint32]0x000f0000;
    expectedRevocationModeOriginal=[uint32]0x00040000;wholeChainRevocationModeObserved=$false;
    recordsOriginal=$null;chargeOriginal=$n.custodyReservationOriginal;returned=$false;
    exceptionOriginal=$null;terminal=$false;qualified=$false;
    state='API_SETTINGS_AND_AVAILABLE_FRESHNESS_CORRESPONDENCE_NOT_POLICY_AUTHENTICATION'}
  $n.evaluationResultOriginal=$o
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies evaluation observation charge.'}
    $charge=$o.chargeOriginal;[decimal]$count=$anchors.recordsOriginal.Count
    if($charge.evaluationObservationChargeAttempted -or $count -gt [int]::MaxValue -or
        -not [object]::ReferenceEquals($charge.preparationOriginal,$p)){
      throw 'SAME finite once-only evaluation observation charge required.'}
    $charge.evaluationObservationChargeAttempted=$true;$charge.evaluationObservationCountOriginal=$count
    $charge.evaluationObservationChargeBytes=$count*$charge.perSignerChargeBytes
    if($charge.usedChargeBytes+$charge.evaluationObservationChargeBytes -gt $charge.limitBytes){
      throw 'Actual evaluation observation graph exceeds SAME custody subdivision.'}
    $charge.usedChargeBytes+=$charge.evaluationObservationChargeBytes
    $o.recordsOriginal=[Collections.Generic.List[object]]::new([int]$count)
    if($null -eq $capture.headerOriginal){
      if(-not $capture.sequenceOriginal.noStateObserved -or $count -ne 0){throw 'Actual absent provider state differs.'}
      $o.providerStateAbsentObserved=$true
    }else{
      $o.providerFieldsOriginal=$capture.headerOriginal.decodedOriginal.fieldsOriginal
      $f=$o.providerFieldsOriginal;$v=$o.osOperandsOriginal
      foreach($field in @('dwRegSecuritySettings','dwRegPolicySettings','dwTrustPubSettings','dwProvFlags')){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported -or $f[$field] -isnot [uint32]){throw 'Actual copied DWORD provider setting required before original D.'}
      }
      $o.providerFlagsOriginal=$f['dwProvFlags']
      $o.wholeChainRevocationModeObserved=(($o.providerFlagsOriginal -band $o.revocationModeMaskOriginal) -eq $o.expectedRevocationModeOriginal)
      $o.providerSettingsEqualObserved=($f['dwRegSecuritySettings'] -eq $v.registrySecuritySettings -and
        $f['dwRegPolicySettings'] -eq $v.registryPolicySettings -and $f['dwTrustPubSettings'] -eq $v.trustPublisherSettings)
      foreach($anchor in $anchors.recordsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported -or -not $anchor.returned){throw 'Original deadline/incomplete actual anchor record.'}
        $r=@{anchorRecordOriginal=$anchor;signerOriginal=$anchor.signerOriginal;
          rootOriginal=$anchor.rootOriginal;finalChainOriginal=$anchor.finalChainOriginal;
          absentObserved=$anchor.absentObserved;freshnessFlagOriginal=$null;freshnessSecondsOriginal=$null;
          freshnessAvailableObserved=$false;freshnessWithinBoundObserved=$false;
          rootErrorStatusOriginal=$null;finalChainErrorStatusOriginal=$null;selectedChainZeroErrorsObserved=$false;
          maximumFreshnessSecondsOriginal=$o.revocationOperandsOriginal.maximumFreshnessSeconds;returned=$false}
        $o.recordsOriginal.Add($r)
        if(-not $r.absentObserved){
          $context=Get-Cli39PublisherChainRange32 $p $r.rootOriginal
          $final=Get-Cli39PublisherChainRange32 $p $r.finalChainOriginal
          $r.freshnessFlagOriginal=$context.fieldsOriginal['fHasRevocationFreshnessTime']
          $r.freshnessSecondsOriginal=$context.fieldsOriginal['dwRevocationFreshnessTime']
          $r.rootErrorStatusOriginal=$context.fieldsOriginal['dwErrorStatus']
          $r.finalChainErrorStatusOriginal=$final.fieldsOriginal['dwErrorStatus']
          foreach($value in @($r.freshnessFlagOriginal,$r.freshnessSecondsOriginal,$r.rootErrorStatusOriginal,$r.finalChainErrorStatusOriginal)){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported -or $value -isnot [uint32]){throw 'Actual copied selected chain DWORD required before original D.'}
          }
          # BOOL is nonzero, not necessarily one. No available flag means no age claim.
          $r.freshnessAvailableObserved=($r.freshnessFlagOriginal -ne 0)
          $r.freshnessWithinBoundObserved=($r.freshnessAvailableObserved -and
            $r.freshnessSecondsOriginal -le $r.maximumFreshnessSecondsOriginal)
          $r.selectedChainZeroErrorsObserved=($r.rootErrorStatusOriginal -eq 0 -and $r.finalChainErrorStatusOriginal -eq 0)
        }
        $r.returned=$true
      }
    }
    if($o.recordsOriginal.Count -ne $count){throw 'Actual evaluation roster changed after precharge.'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies evaluation observation publication.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}