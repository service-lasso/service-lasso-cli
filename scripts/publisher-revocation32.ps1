# Fixed actual per-element revocation observations, separate from applicability/trust.
function Observe-Cli39PublisherRevocationResults32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$anchors=$n.anchorResultOriginal
  $capture=$n.providerCaptureOriginal;$transport=$p.consumerOriginal.rootCaptureOriginal.windowsPolicyFoundationTransportOriginal
  if($n.ContainsKey('revocationResultOriginal') -or -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $capture.collectionTerminal -or -not $capture.custodyCompletionOriginal.returned -or
      -not $anchors.returned -or -not $anchors.terminal -or $null -ne $anchors.exceptionOriginal -or
      -not [object]::ReferenceEquals($anchors.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($anchors.transportOriginal,$transport)){
    throw 'SAME once-only completed actual anchor/transport required for revocation roster.'}
  $o=@{preparationOriginal=$p;captureOriginal=$capture;anchorOriginal=$anchors;transportOriginal=$transport;
    chargeOriginal=$n.custodyReservationOriginal;recordsOriginal=$null;countOriginal=0D;counted=$false;
    chainBookOriginal=$n.chainReservationOriginal;chainRecordsOriginal=$n.chainReservationOriginal.recordsOriginal;
    anchorRecordsOriginal=$anchors.recordsOriginal;custodyCompletionOriginal=$capture.custodyCompletionOriginal;
    returned=$false;exceptionOriginal=$null;terminal=$false;qualified=$false;
    state='ACTUAL_ELEMENT_REVOCATION_OBSERVATIONS_NOT_APPLICABILITY_OR_FRESHNESS_AUTHENTICATION'}
  $n.revocationResultOriginal=$o
  try{
    # Two fixed passes avoid a temporary uncharged materialized element roster.
    for($pass=0;$pass -lt 2;$pass++){
      foreach($anchor in $anchors.recordsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported -or -not $anchor.returned){throw 'Original deadline/incomplete actual signer anchor.'}
        if($anchor.absentObserved){continue}
        $context=Get-Cli39PublisherChainRange32 $p $anchor.rootOriginal
        $chains=Get-Cli39PublisherPolicyChainChild32 $p $context 'simple-chain-array'
        [uint32]$chainCount=$context.fieldsOriginal['cChain']
        if($chains.itemsOriginal.Count -ne $chainCount -or $chainCount -gt [int]::MaxValue){throw 'Actual represented chain roster differs.'}
        for($chainIndex=0;$chainIndex -lt $chainCount;$chainIndex++){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies actual simple-chain revocation traversal.'}
          $chain=Get-Cli39PublisherPolicyChainChild32 $p $chains 'simple-chain' $chainIndex
          $elements=Get-Cli39PublisherPolicyChainChild32 $p $chain 'element-array'
          [uint32]$elementCount=$chain.fieldsOriginal['cElement']
          if($elementCount -eq 0 -or $elementCount -gt [int]::MaxValue -or $elements.itemsOriginal.Count -ne $elementCount){
            throw 'Actual represented nonempty element roster differs.'}
          if($pass -eq 0){
            $o.countOriginal+=$elementCount
            if($o.countOriginal -gt [int]::MaxValue){throw 'Actual revocation observation roster is unrepresentable.'}
            continue
          }
          for($elementIndex=0;$elementIndex -lt $elementCount;$elementIndex++){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline denies actual element revocation observation.'}
            $element=Get-Cli39PublisherPolicyChainChild32 $p $elements 'element' $elementIndex
            $revocation=Get-Cli39PublisherPolicyChainChild32 $p $element 'revocation'
            $r=@{anchorRecordOriginal=$anchor;contextOriginal=$context;chainOriginal=$chain;chainIndexOriginal=$chainIndex;
              elementOriginal=$element;elementIndexOriginal=$elementIndex;revocationOriginal=$revocation;
              finalAnchorPositionObserved=($chainIndex -eq $chainCount-1 -and $elementIndex -eq $elementCount-1);
              elementErrorStatusOriginal=$element.fieldsOriginal['dwErrorStatus'];
              absenceObserved=$revocation.absenceObserved;resultOriginal=$null;freshnessFlagOriginal=$null;
              freshnessSecondsOriginal=$null;freshnessAvailableObserved=$false;freshnessWithinBoundObserved=$false;
              maximumFreshnessSecondsOriginal=$transport.operandsOriginal.basisOriginals[4].maximumFreshnessSeconds;
              applicabilityOriginal=$null;returned=$false}
            $o.recordsOriginal.Add($r)
            if($r.elementErrorStatusOriginal -isnot [uint32]){throw 'Actual copied element error DWORD required.'}
            if(-not $r.absenceObserved){
              $r.resultOriginal=$revocation.fieldsOriginal['dwRevocationResult']
              $r.freshnessFlagOriginal=$revocation.fieldsOriginal['fHasFreshnessTime']
              $r.freshnessSecondsOriginal=$revocation.fieldsOriginal['dwFreshnessTime']
              foreach($value in @($r.resultOriginal,$r.freshnessFlagOriginal,$r.freshnessSecondsOriginal)){
                Observe-OwnedPrerequisiteDeadline $p
                if($p.deadlineReported -or $value -isnot [uint32]){throw 'Actual copied revocation DWORD required before original D.'}
              }
              $r.freshnessAvailableObserved=($r.freshnessFlagOriginal -ne 0)
              $r.freshnessWithinBoundObserved=($r.freshnessAvailableObserved -and $r.freshnessSecondsOriginal -le $r.maximumFreshnessSecondsOriginal)
            }
            $r.returned=$true
          }
        }
      }
      if($pass -eq 0){
        $o.counted=$true
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies revocation observation precharge.'}
        $charge=$o.chargeOriginal
        if($charge.revocationObservationChargeAttempted -or -not [object]::ReferenceEquals($charge.preparationOriginal,$p)){
          throw 'SAME once-only revocation observation charge required.'}
        $charge.revocationObservationChargeAttempted=$true;$charge.revocationObservationCountOriginal=$o.countOriginal
        $charge.revocationObservationChargeBytes=$o.countOriginal*$charge.perSignerChargeBytes
        if($charge.usedChargeBytes+$charge.revocationObservationChargeBytes -gt $charge.limitBytes){
          throw 'Actual full revocation observation graph exceeds SAME custody subdivision.'}
        $charge.usedChargeBytes+=$charge.revocationObservationChargeBytes
        $o.recordsOriginal=[Collections.Generic.List[object]]::new([int]$o.countOriginal)
      }
    }
    if($o.recordsOriginal.Count -ne $o.countOriginal -or -not $capture.collectionTerminal -or
        -not [object]::ReferenceEquals($n.chainReservationOriginal,$o.chainBookOriginal) -or
        -not [object]::ReferenceEquals($o.chainBookOriginal.recordsOriginal,$o.chainRecordsOriginal) -or
        -not [object]::ReferenceEquals($anchors.recordsOriginal,$o.anchorRecordsOriginal) -or
        -not [object]::ReferenceEquals($capture.custodyCompletionOriginal,$o.custodyCompletionOriginal) -or
        -not $o.custodyCompletionOriginal.returned){throw 'Actual closed captured custody graph changed after precharge.'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies revocation observation publication.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}