# Fixed pure lookup in the actual captured graph, including canonical aliases.
function Get-Cli39PublisherPolicyChainChild32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ParentOriginal,
    [Parameter(Mandatory)][ValidateSet('simple-chain-array','simple-chain','element-array','element','certificate-context','certificate-der','revocation')][string]$Role,
    [int]$Index=-1)
  $p=$OriginalPreparation;$book=$p.consumerOriginal.publisherNativeOriginal.chainReservationOriginal
  $parent=Get-Cli39PublisherChainRange32 $p $ParentOriginal
  if($null -ne $parent.aliasOriginal){$parent=Get-Cli39PublisherChainRange32 $p $parent.aliasOriginal}
  $found=$null
  foreach($candidate in $book.recordsOriginal){
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies fixed captured chain-child lookup.'}
    if([object]::ReferenceEquals($candidate.parentOriginal,$parent) -and $candidate.roleOriginal -ceq $Role -and
        $candidate.indexOriginal -eq $Index){
      if($null -ne $found){throw 'Duplicate actual fixed chain child.'}
      $found=Get-Cli39PublisherChainRange32 $p $candidate
    }
  }
  if($null -eq $found -or -not $found.decodedReturned -or -not $found.expansionReturned -or
      $null -ne $found.exceptionOriginal){throw 'Complete actual fixed captured chain child absent.'}
  return $found
}

function Observe-Cli39PublisherAnchorResults32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$capture=$n.providerCaptureOriginal
  $root=$p.consumerOriginal.rootCaptureOriginal;$transport=$root.windowsPolicyFoundationTransportOriginal
  $time=$n.timeResultOriginal;$operands=$transport.operandsOriginal
  if($n.ContainsKey('anchorResultOriginal') -or -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $capture.returned -or -not $capture.custodyCompletionOriginal.returned -or
      -not $time.returned -or -not $time.terminal -or $null -ne $time.exceptionOriginal -or
      -not [object]::ReferenceEquals($time.captureOriginal,$capture) -or -not $transport.returned){
    throw 'SAME once-only actual completed capture/time/anchor operands required.'}
  $o=@{preparationOriginal=$p;captureOriginal=$capture;transportOriginal=$transport;timeOriginal=$time;
    chargeOriginal=$n.custodyReservationOriginal;storeOperandsOriginal=$operands.basisOriginals[1];
    anchorsOriginal=$operands.anchorsOriginal;recordsOriginal=$null;
    returned=$false;exceptionOriginal=$null;terminal=$false;qualified=$false;
    state='ACTUAL_DYNAMIC_CHAIN_ANCHOR_CORRESPONDENCE_NOT_STORE_AUTHENTICATION'}
  $n.anchorResultOriginal=$o
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies anchor observation charge.'}
    $charge=$o.chargeOriginal
    [decimal]$count=$time.signerRecordsOriginal.Count
    [decimal]$recordCount=$count*(1D+$o.anchorsOriginal.Count)
    if($charge.anchorObservationChargeAttempted -or -not [object]::ReferenceEquals($charge.preparationOriginal,$p) -or
        $recordCount -gt [int]::MaxValue){throw 'SAME finite once-only anchor record charge required.'}
    $charge.anchorObservationChargeAttempted=$true;$charge.anchorObservationCountOriginal=$recordCount
    $charge.anchorObservationChargeBytes=$recordCount*$charge.perSignerChargeBytes
    if($charge.usedChargeBytes+$charge.anchorObservationChargeBytes -gt $charge.limitBytes){throw 'Actual complete anchor comparisons exceed SAME custody subdivision.'}
    $charge.usedChargeBytes+=$charge.anchorObservationChargeBytes
    $o.recordsOriginal=[Collections.Generic.List[object]]::new([int]$count)
    foreach($timeRecord in $time.signerRecordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or -not $timeRecord.returned){throw 'Original deadline/incomplete actual signer time record.'}
      $signer=$timeRecord.signerOriginal
      $r=@{timeRecordOriginal=$timeRecord;signerOriginal=$signer;rootOriginal=$null;chainArrayOriginal=$null;
        finalChainOriginal=$null;elementArrayOriginal=$null;anchorElementOriginal=$null;
        contextOriginal=$null;derOriginal=$null;comparisonsOriginal=$null;matchedCount=0;
        absentObserved=$false;terminalTrustListAbsentObserved=$false;returned=$false}
      $o.recordsOriginal.Add($r)
      $r.comparisonsOriginal=[Collections.Generic.List[object]]::new($o.anchorsOriginal.Count)
      foreach($candidate in $n.chainReservationOriginal.recordsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies actual signer root lookup.'}
        if($candidate.roleOriginal -ceq 'chain-context' -and [object]::ReferenceEquals($candidate.parentOriginal,$signer)){
          if($null -ne $r.rootOriginal){throw 'Duplicate actual signer root context.'}
          $r.rootOriginal=Get-Cli39PublisherChainRange32 $p $candidate
        }
      }
      if($null -eq $r.rootOriginal){throw 'Actual signer chain root absent.'}
      if($r.rootOriginal.absenceObserved){$r.absentObserved=$true;$r.returned=$true;continue}
      $chainCount=$r.rootOriginal.fieldsOriginal['cChain']
      if($chainCount -isnot [uint32] -or $chainCount -eq 0 -or $chainCount -gt [int]::MaxValue){throw 'Actual represented nonempty chain array required.'}
      $r.chainArrayOriginal=Get-Cli39PublisherPolicyChainChild32 $p $r.rootOriginal 'simple-chain-array'
      if($r.chainArrayOriginal.itemsOriginal.Count -ne $chainCount){throw 'Actual complete chain array count differs.'}
      $r.finalChainOriginal=Get-Cli39PublisherPolicyChainChild32 $p $r.chainArrayOriginal 'simple-chain' ([int]$chainCount-1)
      $elementCount=$r.finalChainOriginal.fieldsOriginal['cElement']
      if($elementCount -isnot [uint32] -or $elementCount -eq 0 -or $elementCount -gt [int]::MaxValue){throw 'Actual represented nonempty final-chain element array required.'}
      $r.terminalTrustListAbsentObserved=($r.finalChainOriginal.fieldsOriginal['pTrustListInfo'] -eq [IntPtr]::Zero)
      $r.elementArrayOriginal=Get-Cli39PublisherPolicyChainChild32 $p $r.finalChainOriginal 'element-array'
      if($r.elementArrayOriginal.itemsOriginal.Count -ne $elementCount){throw 'Actual complete final-chain element count differs.'}
      $r.anchorElementOriginal=Get-Cli39PublisherPolicyChainChild32 $p $r.elementArrayOriginal 'element' ([int]$elementCount-1)
      $r.contextOriginal=Get-Cli39PublisherPolicyChainChild32 $p $r.anchorElementOriginal 'certificate-context'
      $r.derOriginal=Get-Cli39PublisherPolicyChainChild32 $p $r.contextOriginal 'certificate-der'
      $der=$r.derOriginal
      if($der.absenceObserved -or -not $der.copyReturned -or $der.used -ne $der.bytesExpected -or
          $der.rawBytesOriginal.Length -ne $der.bytesExpected){throw 'Actual complete final trust-anchor DER required.'}
      for($ordinal=0;$ordinal -lt $o.anchorsOriginal.Count;$ordinal++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies accepted-anchor comparison record.'}
        $hex=$o.anchorsOriginal[$ordinal]
        $comparison=@{ordinalOriginal=$ordinal;expectedHexOriginal=$hex;derOriginal=$der;
          extentMatches=([decimal]$hex.Length -eq 2D*$der.used);comparedBytes=0;
          byteMismatchObserved=$false;equalBytes=$false;returned=$false}
        $r.comparisonsOriginal.Add($comparison)
        if($comparison.extentMatches){
          for($index=0;$index -lt $der.used;$index++){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline denies actual accepted-anchor byte comparison.'}
            $high='0123456789abcdef'.IndexOf($hex[2*$index]);$low='0123456789abcdef'.IndexOf($hex[2*$index+1])
            $comparison.comparedBytes=$index+1
            if($high -lt 0 -or $low -lt 0 -or $der.rawBytesOriginal[$index] -ne (16*$high+$low)){$comparison.byteMismatchObserved=$true;break}
          }
          $comparison.equalBytes=(-not $comparison.byteMismatchObserved -and $comparison.comparedBytes -eq $der.used)
          if($comparison.equalBytes){$r.matchedCount++}
        }
        $comparison.returned=$true
      }
      $r.returned=$true
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies actual anchor observation publication.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}
