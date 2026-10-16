# Local correspondence under the externally qualified whole-input foundation.
# This function neither authenticates supplied policy nor changes native settings.
function Join-Cli39PublisherSemanticResults32 {
  param([Parameter(Mandatory)]$AssociationOriginal)
  $a=$AssociationOriginal;$p=$a.preparationOriginal;$n=$p.consumerOriginal.publisherNativeOriginal
  $capture=$a.captureOriginal;$transport=$a.foundationTransportOriginal
  if(-not [object]::ReferenceEquals($n.policyOriginal.associationConstructionOriginal,$a) -or
      $n.ContainsKey('semanticResultOriginal') -or -not $capture.collectionTerminal -or
      -not $capture.returned -or -not $capture.custodyCompletionOriginal.returned -or
      -not $transport.returned -or $null -ne $transport.exceptionOriginal){throw 'SAME closed publisher semantic originals required.'}
  $o=@{associationOriginal=$a;preparationOriginal=$p;captureOriginal=$capture;transportOriginal=$transport;
    recordsOriginal=$null;chargeOriginal=$n.custodyReservationOriginal;matchedCount=0;
    returned=$false;terminal=$false;exceptionOriginal=$null;supportedRelationsObserved=$false;
    state='LOCAL_SEMANTIC_CORRESPONDENCE_REQUIRES_EXTERNALLY_QUALIFIED_INPUT'}
  $n.semanticResultOriginal=$o
  try{
    foreach($result in @($a.providerResultOriginal,$a.timeResultOriginal,$a.anchorResultOriginal,
        $a.evaluationResultOriginal,$a.revocationResultOriginal,$a.timestampResultOriginal)){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or $null -eq $result -or -not $result.returned -or
          $null -ne $result.exceptionOriginal -or -not [object]::ReferenceEquals($result.captureOriginal,$capture)){
        throw 'Actual completed SAME publisher observations required before original D.'}
    }
    [decimal]$count=$capture.signerOriginals.Count
    $comparison=$a.certificateComparisonOriginal
    if($count -gt 0 -and ($null -eq $comparison -or -not $comparison.returned -or -not $comparison.constructionTerminal -or
        $null -ne $comparison.exceptionOriginal -or -not [object]::ReferenceEquals($comparison.captureOriginal,$capture))){
      throw 'Actual completed expected publisher leaf comparison required.'}
    $charge=$o.chargeOriginal
    if($charge.semanticObservationChargeAttempted -or $count -gt [int]::MaxValue -or
        -not [object]::ReferenceEquals($charge.preparationOriginal,$p)){throw 'SAME once-only semantic charge required.'}
    $charge.semanticObservationChargeAttempted=$true;$charge.semanticObservationCountOriginal=$count
    $charge.semanticObservationChargeBytes=$count*$charge.perSignerChargeBytes
    if($charge.usedChargeBytes+$charge.semanticObservationChargeBytes -gt $charge.limitBytes){throw 'Full semantic records exceed SAME custody subdivision.'}
    $charge.usedChargeBytes+=$charge.semanticObservationChargeBytes
    $o.recordsOriginal=[Collections.Generic.List[object]]::new([int]$count)
    foreach($primary in $capture.signerOriginals){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or -not (Test-Cli39ProviderSignerMember32 $capture $primary) -or
          $null -ne $primary.primarySignerOriginal){throw 'Actual primary member required before original D.'}
      $r=@{primaryOriginal=$primary;leafOriginal=$null;anchorOriginal=$null;evaluationOriginal=$null;
        timeOriginal=$null;timestampOriginal=$null;expectedLeafObserved=$false;storeAnchorObserved=$false;
        selectedChainObserved=$false;timeObserved=$false;revocationObserved=$false;
        timestampObserved=$false;timestampCounterOriginal=$null;timestampEvaluationOriginal=$null;timestampTimeOriginal=$null;eligibleObserved=$false;returned=$false}
      $o.recordsOriginal.Add($r)
      foreach($pair in @(@('leafOriginal',$comparison.recordsOriginal,'signerOriginal'),
          @('anchorOriginal',$a.anchorResultOriginal.recordsOriginal,'signerOriginal'),
          @('evaluationOriginal',$a.evaluationResultOriginal.recordsOriginal,'signerOriginal'),
          @('timeOriginal',$a.timeResultOriginal.signerRecordsOriginal,'signerOriginal'),
          @('timestampOriginal',$a.timestampResultOriginal.recordsOriginal,'primaryOriginal'))){
        foreach($record in $pair[1]){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies semantic source lookup.'}
          if([object]::ReferenceEquals($record[$pair[2]],$primary)){
            if($null -ne $r[$pair[0]]){throw 'Duplicate actual primary semantic association.'}
            $r[$pair[0]]=$record
          }
        }
        if($null -eq $r[$pair[0]] -or -not $r[$pair[0]].returned){throw 'Actual primary semantic association absent.'}
      }
      $leafChain=$r.leafOriginal.certificateOriginal.chainAssociationOriginal
      $r.expectedLeafObserved=($r.leafOriginal.equalBytes -and $r.leafOriginal.signerErrorOriginal -eq 0 -and
        $null -ne $leafChain -and $leafChain.returned -and $null -eq $leafChain.exceptionOriginal -and
        $leafChain.chainContextMatches -and $leafChain.certificateContextMatches -and $leafChain.equalBytes)
      $r.storeAnchorObserved=(-not $r.anchorOriginal.absentObserved -and
        $r.anchorOriginal.terminalTrustListAbsentObserved -and $r.anchorOriginal.matchedCount -gt 0)
      $r.selectedChainObserved=$r.evaluationOriginal.selectedChainZeroErrorsObserved
      $r.revocationObserved=($a.evaluationResultOriginal.wholeChainRevocationModeObserved -and
        $r.evaluationOriginal.freshnessAvailableObserved -and $r.evaluationOriginal.freshnessWithinBoundObserved)
      # The qualified default-provider policy determines applicability. Missing
      # individual age flags remain unavailable; the checked aggregate is kept.
      foreach($element in $a.revocationResultOriginal.recordsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies selected revocation correspondence.'}
        if(-not [object]::ReferenceEquals($element.anchorRecordOriginal.signerOriginal,$primary)){continue}
        if($element.elementErrorStatusOriginal -ne 0 -or
            (-not $element.absenceObserved -and $element.resultOriginal -ne 0)){$r.revocationObserved=$false}
      }
      $mode=$a.timeResultOriginal.timestampModeOriginal
      $r.timeObserved=$a.timeResultOriginal.providerTimeInIntervalObserved
      if($mode -ceq 'qualified-current-policy'){
        # No primary/system FILETIME equality or ban on ordinary timestamps.
        $r.timestampObserved=$true
      }elseif($mode -ceq 'qualified-provider-timestamp'){
        $r.timeObserved=($r.timeObserved -and $r.timeOriginal.insideIntervalObserved -and $r.timeOriginal.selectedValidationEqualObserved)
        if($r.timestampOriginal.recognizedAttributeCount -gt 0){
          foreach($counter in $r.timestampOriginal.counterRecordsOriginal){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline denies timestamp counter correspondence.'}
            if($counter.returned -and $counter.timestampTypeObserved -and $counter.signerErrorOriginal -eq 0 -and
                $null -ne $counter.timeRecordOriginal -and $counter.timeRecordOriginal.returned -and
                [object]::ReferenceEquals($counter.timeRecordOriginal.signerOriginal,$counter.counterOriginal) -and
                [object]::ReferenceEquals($counter.timeRecordOriginal.primaryOriginal,$primary) -and
                -not $counter.anchorRecordOriginal.absentObserved -and
                $counter.anchorRecordOriginal.terminalTrustListAbsentObserved -and $counter.anchorRecordOriginal.matchedCount -gt 0){
              $counterEvaluation=$null
              foreach($evaluation in $a.evaluationResultOriginal.recordsOriginal){
                Observe-OwnedPrerequisiteDeadline $p
                if($p.deadlineReported){throw 'Original deadline denies timestamp evaluation lookup.'}
                if([object]::ReferenceEquals($evaluation.signerOriginal,$counter.counterOriginal)){
                  if($null -ne $counterEvaluation){throw 'Duplicate actual timestamp evaluation.'}
                  $counterEvaluation=$evaluation
                }
              }
              if($null -ne $counterEvaluation -and $counterEvaluation.returned -and
                  $counterEvaluation.selectedChainZeroErrorsObserved -and $counterEvaluation.freshnessAvailableObserved -and
                  $counterEvaluation.freshnessWithinBoundObserved){
                # SGNR documents each verify time as current OR timestamp; it
                # does not require equality between the primary and counter.
                # Preserve both actual fields under the qualified default policy.
                $r.timestampCounterOriginal=$counter;$r.timestampEvaluationOriginal=$counterEvaluation
                $r.timestampTimeOriginal=$counter.timeRecordOriginal;$r.timestampObserved=$true
              }
            }
          }
        }
      }else{throw 'Closed qualified validation mode required.'}
      $r.eligibleObserved=($a.providerResultOriginal.providerOperationZeroErrorsObserved -and
        $a.evaluationResultOriginal.providerSettingsEqualObserved -and $r.expectedLeafObserved -and
        $r.storeAnchorObserved -and $r.selectedChainObserved -and $r.timeObserved -and
        $r.revocationObserved -and $r.timestampObserved)
      if($r.eligibleObserved){$o.matchedCount++};$r.returned=$true
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported -or $o.recordsOriginal.Count -ne $count){throw 'Original deadline/changed semantic roster denies publication.'}
    $o.supportedRelationsObserved=($o.matchedCount -gt 0);$o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}
