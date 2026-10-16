# Complete successful custody of one source-enumerated issued collector. This
# is stronger than physical borrow quiescence and grants no policy acceptance.
function Test-Cli39PublisherCollectorTerminal32 {
  param($OriginalPreparation,$WorkerOriginal)
  $p=$OriginalPreparation;$w=$WorkerOriginal
  if($null -eq $w -or -not [object]::ReferenceEquals($w.preparation,$p) -or
      -not [object]::ReferenceEquals($w.nativeOriginal,$p.consumerOriginal.publisherNativeOriginal) -or
      -not $w.constructionTerminal -or $null -ne $w.constructionFailure -or
      -not $w.startAttempted -or -not $w.startReturned -or $null -ne $w.startException -or
      -not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive -or
      $w.joinPendingActive -or $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
  try{if(-not [object]::ReferenceEquals((Get-Cli39PublisherWorkerOriginal26 $w),$w)){return $false}}catch{return $false}
  foreach($failure in $w.exceptionOriginal){if($null -ne $failure){return $false}}
  if($w.ContainsKey('pinOriginal')){foreach($pin in $w.pinOriginal){if($pin.IsAllocated){return $false}}}
  return $true
}
# Pure classification of the fixed supported observation graph. No trust or
# close/free authority follows from this result.
function Test-Cli39PublisherRawRange32 {
  param($CaptureOriginal,$RangeOriginal)
  $c=$CaptureOriginal;$h=$RangeOriginal
  if($null -eq $h -or -not [object]::ReferenceEquals($h.captureOriginal,$c) -or
      -not $h.rangeReturned -or $null -ne $h.exceptionOriginal -or
      $null -eq $h.rawBytesOriginal -or $h.rawBytesOriginal.Length -ne $h.bytesExpected){return $false}
  if($h.bytesExpected -eq 0){return ($h.absenceObserved -and $h.used -eq 0)}
  foreach($page in $h.pagesOriginal){
    if(-not $page.accepted -or $null -ne $page.exceptionOriginal -or
        -not (Test-Cli39PublisherCollectorTerminal32 $c.preparationOriginal $page.workerOriginal)) {return $false}
    $pw=$page.workerOriginal
    if($pw.results[0] -ne 1 -or $pw.results[1] -ne 1 -or
        $pw.results[2] -ne 1 -or $pw.results[3] -ne 1 -or
        $pw.results[5] -ne 1 -or $pw.results[7] -ne 1){return $false}
  }
  $w=$h.copyWorkerOriginal
  return ($h.copyReturned -and $h.used -eq $h.bytesExpected -and $null -ne $w -and
    [object]::ReferenceEquals($w.headerOriginal,$h) -and (Test-Cli39PublisherCollectorTerminal32 $c.preparationOriginal $w) -and $w.terminalJoined -and
    $null -ne $w.thread -and -not $w.thread.IsAlive -and $w.results[1] -eq 1 -and
    $null -eq $w.exceptionOriginal[0])
}

function Test-Cli39PublisherRawOid32 {
  param($OriginalPreparation,$ParentOriginal)
  $p=$OriginalPreparation;$a=$ParentOriginal
  if(-not (Test-Cli39OidParentMember32 $p $a) -or $null -ne $a.exceptionOriginal){return $false}
  if($a.oidPointerOriginal -eq [IntPtr]::Zero){
    # Named optional pointers explicitly record absence. Attribute OIDs use
    # their actual attempted reader and cannot invent this named disposition.
    return ($a.absenceObserved -and -not $a.oidConstructorAttempted -and $null -eq $a.oidOriginal)
  }
  $h=$a.oidOriginal
  foreach($page in $h.pagesOriginal){
    if(-not $page.accepted -or -not $page.readReturned -or $null -ne $page.exceptionOriginal -or
        -not (Test-Cli39PublisherCollectorTerminal32 $p $page.workerOriginal) -or
        -not (Test-Cli39PublisherCollectorTerminal32 $p $page.readerOriginal)){return $false}
  }
  return ($null -ne $h -and [object]::ReferenceEquals($h.attributeOriginal,$a) -and
    $h.constructionTerminal -and $null -eq $h.constructionFailure -and
    $null -eq $h.exceptionOriginal -and $h.returned -and $h.terminated -and
    $h.used -gt 0 -and $h.used -le $h.rawBytesOriginal.Length -and
    (Test-Cli39ProviderAttributeOidBorrowClosed32 $p $a))
}

function Test-Cli39PublisherNamedRawOid32 {
  param($OriginalPreparation,$SourceOriginal,[string]$Role,[int]$Index=-1)
  $p=$OriginalPreparation;$c=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal
  $matchingParents=0
  foreach($a in $c.namedOidParentsOriginal){
    if([object]::ReferenceEquals($a.sourceOriginal,$SourceOriginal) -and
        $a.roleOriginal -ceq $Role -and $a.indexOriginal -eq $Index){
      $matchingParents++
      if(-not (Test-Cli39PublisherRawOid32 $p $a)){return $false}
    }
  }
  return ($matchingParents -eq 1)
}

function Test-Cli39PublisherSignerRawGraph32 {
  param($OriginalPreparation,$SignerOriginal)
  $p=$OriginalPreparation;$c=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$s=$SignerOriginal
  if(-not (Test-Cli39ProviderSignerMember32 $c $s) -or -not $s.pointerReturned -or
      $null -ne $s.exceptionOriginal -or -not (Test-Cli39PublisherRawRange32 $c $s.headerRangeOriginal) -or
      -not $s.headerRangeOriginal.decodedOriginal.returned -or
      -not (Test-Cli39PublisherRawRange32 $c $s.signerInfoOriginal) -or
      -not $s.signerInfoOriginal.decodedOriginal.returned){return $false}
  if(-not (Test-Cli39PublisherCollectorTerminal32 $p $s.workerOriginal)){return $false}
  # The fixed SDK-helper model collects primary plus one counter layer only.
  # A counter's own nonzero nested count is retained raw but unsupported by
  # this collector; it must not be reported as a complete supported graph.
  if($null -ne $s.primarySignerOriginal -and
      $s.headerRangeOriginal.decodedOriginal.fieldsOriginal['csCounterSigners'] -ne 0){return $false}
  foreach($role in @('hash-algorithm','encryption-algorithm')){
    if(-not (Test-Cli39PublisherNamedRawOid32 $p $s $role)){return $false}
  }
  foreach($part in @('issuer','serial','hash-parameters','encryption-parameters','encrypted-hash','auth-attributes','unauth-attributes')){
    $h=$s.signerFieldOriginals[$part]
    if(-not (Test-Cli39PublisherRawRange32 $c $h)){return $false}
    if($part -cin @('auth-attributes','unauth-attributes')){
      if(-not $h.attributesConstructorOriginal.returned -or
          -not [object]::ReferenceEquals($h.attributesConstructorOriginal.arrayOriginal,$h) -or
          -not [object]::ReferenceEquals($h.attributesConstructorOriginal.recordsOriginal,$h.attributesOriginal) -or
          $h.attributesOriginal.Count -ne $h.countOriginal){return $false}
      foreach($a in $h.attributesOriginal){
        if(-not (Test-Cli39ProviderAttributeMember32 $c $a) -or -not $a.decodedReturned -or
            $null -ne $a.exceptionOriginal -or -not (Test-Cli39PublisherRawOid32 $p $a) -or
            -not (Test-Cli39PublisherRawRange32 $c $a.valuesRangeOriginal) -or
            -not $a.valuesConstructorOriginal.returned -or
            -not [object]::ReferenceEquals($a.valuesConstructorOriginal.attributeOriginal,$a) -or
            -not [object]::ReferenceEquals($a.valuesConstructorOriginal.arrayOriginal,$a.valuesRangeOriginal) -or
            -not [object]::ReferenceEquals($a.valuesConstructorOriginal.recordsOriginal,$a.valuesOriginal) -or
            $a.valuesOriginal.Count -ne $a.countOriginal){return $false}
        foreach($v in $a.valuesOriginal){
          if(-not (Test-Cli39ProviderAttributeValueMember32 $c $a $v) -or -not $v.decodedReturned -or
              $null -ne $v.exceptionOriginal -or -not (Test-Cli39PublisherRawRange32 $c $v.rangeOriginal)){return $false}
        }
      }
    }
  }
  $sequence=$s.certificateSequenceOriginal
  $count=$s.headerRangeOriginal.decodedOriginal.fieldsOriginal['csCertChain']
  if($null -eq $sequence -or -not $sequence.returned -or $null -ne $sequence.exceptionOriginal -or
      -not [object]::ReferenceEquals($sequence.signerOriginal,$s) -or $sequence.countOriginal -ne $count){return $false}
  if($count -eq 0){return ($null -eq $s.certificateOriginals -and $sequence.zeroCountObserved)}
  $roster=$s.certificateRosterOriginal
  if($null -eq $s.certificateOriginals -or $s.certificateOriginals.Count -ne $count -or
      $null -eq $roster -or -not $roster.constructorReturned -or $roster.countOriginal -ne $count -or
      -not [object]::ReferenceEquals($roster.signerOriginal,$s) -or
      -not [object]::ReferenceEquals($roster.headerOriginal,$s.headerRangeOriginal) -or
      -not [object]::ReferenceEquals($roster.recordsOriginal,$s.certificateOriginals)){return $false}
  foreach($cert in $s.certificateOriginals){
    if(-not (Test-Cli39PublisherCollectorTerminal32 $p $cert.workerOriginal)){return $false}
    if(-not (Test-Cli39ProviderCertificateMember32 $c $cert) -or -not $cert.pointerReturned -or
        $null -ne $cert.exceptionOriginal -or
        -not (Test-Cli39PublisherRawRange32 $c $cert.providerHeaderOriginal) -or
        -not (Test-Cli39PublisherRawRange32 $c $cert.contextHeaderOriginal) -or
        -not (Test-Cli39PublisherRawRange32 $c $cert.derOriginal) -or
        $null -eq $cert.chainAssociationOriginal -or -not $cert.chainAssociationOriginal.returned -or
        -not [object]::ReferenceEquals($cert.chainAssociationOriginal.certificateOriginal,$cert) -or
        -not [object]::ReferenceEquals($cert.chainAssociationOriginal.captureOriginal,$c) -or
        $null -ne $cert.chainAssociationOriginal.exceptionOriginal){return $false}
  }
  return $true
}

function Test-Cli39PublisherSupportedRawGraph32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$c=$n.providerCaptureOriginal
  if($null -eq $c -or -not [object]::ReferenceEquals($c.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($c.nativeOriginal,$n) -or
      -not $c.sequenceOriginal.primarySignersReturned -or -not $c.sequenceOriginal.certificateChainAssociationsReturned -or
      -not (Test-Cli39PublisherRawRange32 $c $c.headerOriginal) -or
      -not (Test-Cli39PublisherChainCaptureComplete32 $p)){return $false}
  foreach($part in @('trust-step-errors','stores','private-headers','sig-state','sig-settings','secondary-signatures','sealing-signature','strong-policy','sealing-parameters','sealing-digest','provider-functions','ui-functions','ui-data','request-usage','usage-oid-pointers')){
    $h=$c.providerFieldOriginals[$part]
    if(-not (Test-Cli39PublisherRawRange32 $c $h) -or -not $h.decodedOriginal.returned){return $false}
  }
  foreach($role in @('provider-usage','ctl-usage')){if(-not (Test-Cli39PublisherNamedRawOid32 $p $c.headerOriginal $role)){return $false}}
  if(-not (Test-Cli39PublisherNamedRawOid32 $p $c.providerFieldOriginals['sealing-signature'] 'sealing-algorithm')){return $false}
  $usage=$c.providerFieldOriginals['usage-oid-pointers']
  if(-not $usage.absenceObserved){for($i=0;$i -lt $usage.decodedOriginal.itemsOriginal.Count;$i++){
    if(-not (Test-Cli39PublisherNamedRawOid32 $p $usage 'requested-usage' $i)){return $false}
  }}
  $count=$c.headerOriginal.decodedOriginal.fieldsOriginal['csSigners']
  if(-not (Test-Cli39ProviderSignerBorrowClosed32 $p) -or
      -not (Test-Cli39ProviderFieldsBorrowClosed32 $p) -or
      -not (Test-Cli39NamedProviderOidsBorrowClosed32 $p)){return $false}
  if($count -eq 0){return ($null -eq $c.signerOriginals -and $c.sequenceOriginal.zeroPrimarySignersObserved)}
  $roster=$c.signerRosterOriginal
  if($null -eq $c.signerOriginals -or $c.signerOriginals.Count -ne $count -or
      $null -eq $roster -or -not $roster.constructorReturned -or $roster.countOriginal -ne $count -or
      -not [object]::ReferenceEquals($roster.headerOriginal,$c.headerOriginal) -or
      -not [object]::ReferenceEquals($roster.decodedOriginal,$c.headerOriginal.decodedOriginal) -or
      -not [object]::ReferenceEquals($roster.recordsOriginal,$c.signerOriginals)){return $false}
  foreach($s in $c.signerOriginals){
    if(-not (Test-Cli39PublisherSignerRawGraph32 $p $s)){return $false}
    $counterCount=$s.headerRangeOriginal.decodedOriginal.fieldsOriginal['csCounterSigners']
    if($counterCount -eq 0){if($null -ne $s.counterSignerOriginals){return $false}}
    else{
      $counterRoster=$s.counterRosterOriginal
      if($null -eq $s.counterSignerOriginals -or $s.counterSignerOriginals.Count -ne $counterCount -or
          $null -eq $counterRoster -or -not $counterRoster.constructorReturned -or
          $counterRoster.countOriginal -ne $counterCount -or
          -not [object]::ReferenceEquals($counterRoster.headerOriginal,$s.headerRangeOriginal) -or
          -not [object]::ReferenceEquals($counterRoster.decodedOriginal,$s.headerRangeOriginal.decodedOriginal) -or
          -not [object]::ReferenceEquals($counterRoster.recordsOriginal,$s.counterSignerOriginals)){return $false}
      foreach($counter in $s.counterSignerOriginals){if(-not (Test-Cli39PublisherSignerRawGraph32 $p $counter)){return $false}}
    }
  }
  return $true
}

function Complete-Cli39PublisherSupportedRawGraph32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal
  $sequence=$c.sequenceOriginal
  if($null -eq $sequence -or -not [object]::ReferenceEquals($sequence.captureOriginal,$c) -or
      -not [object]::ReferenceEquals($sequence.preparationOriginal,$p) -or
      $sequence.ContainsKey('rawGraphClassificationOriginal')){throw 'SAME once-only raw graph classification required.'}
  $o=@{sequenceOriginal=$sequence;captureOriginal=$c;preparationOriginal=$p;
    attempted=$true;returned=$false;completeObserved=$false;exceptionOriginal=$null;
    state='SUPPORTED_RAW_GRAPH_ONLY_NOT_POLICY_TRUST_OR_DESTRUCTION_AUTHORITY'}
  $sequence.rawGraphClassificationOriginal=$o
  try{$o.completeObserved=Test-Cli39PublisherSupportedRawGraph32 $p;$o.returned=$true;return $o}
  catch{$o.exceptionOriginal=$_;throw}
}

# Once-only selected API-visible custody transition. It is deliberately not
# Wintrust policy acceptance, authentic input admission or useful completion.
function Complete-Cli39PublisherCaptureCustody32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$c=$n.providerCaptureOriginal
  if($null -eq $c -or -not [object]::ReferenceEquals($c.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($c.nativeOriginal,$n) -or
      $c.collectionTerminal -or $null -eq $c.custodyCompletionOriginal -or $c.custodyCompletionOriginal.attempted){throw 'SAME once-only capture custody completion required.'}
  # Close future starts BEFORE validating the preowned completion record.
  # A validation failure cannot reopen collection.
  $c.collectionTerminal=$true
  $o=$c.custodyCompletionOriginal
  $o.attempted=$true;$o.sequenceOriginal=$c.sequenceOriginal
  try{
    $sequence=$o.sequenceOriginal;$verify=$n.verifyWorkerOriginal;$state=$c.stateWorkerOriginal
    if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
        $null -eq $sequence -or -not [object]::ReferenceEquals($sequence.captureOriginal,$c) -or
        -not [object]::ReferenceEquals($sequence.preparationOriginal,$p) -or
        -not $sequence.stateReturned -or $null -ne $sequence.exceptionOriginal -or
        $null -ne $c.exceptionOriginal -or -not $c.stateReadReturned -or
        -not (Test-Cli39PublisherCollectorTerminal32 $p $state) -or
        -not (Test-Cli39PublisherCollectorTerminal32 $p $verify) -or
        $verify.results[1] -ne 1 -or $verify.results[2] -ne 1 -or
        -not $c.namedOidParentsConstructorOriginal.terminal -or
        -not $c.namedOidParentsConstructorOriginal.returned -or
        $null -ne $c.namedOidParentsConstructorOriginal.exceptionOriginal){throw 'Actual complete verification/state/collector construction required.'}
    if($c.noStateObserved){
      if(-not $sequence.noStateObserved -or $c.providerCallReturned -or
          $c.stateOriginal -ne [IntPtr]::Zero -or $c.providerDataOriginal -ne [IntPtr]::Zero -or
          $state.results[0] -ne 1 -or $state.results[1] -ne 1 -or
          $state.results[2] -ne 0 -or $state.results[3] -ne 0 -or $state.results[4] -ne 1 -or
          $null -ne $c.headerOriginal -or $null -ne $c.signerRosterOriginal -or
          $null -ne $c.signerOriginals -or $c.namedOidParentsOriginal.Count -ne 0){throw 'Exact actual absent-state collector roster required.'}
      foreach($field in $c.providerFieldOriginals.Values){if($null -ne $field){throw 'No-state result has contradictory provider body.'}}
      if($n.chainReservationOriginal.recordsOriginal.Count -ne 0){throw 'No-state result has contradictory chain collection.'}
    }else{
      $o.classificationOriginal=$sequence.rawGraphClassificationOriginal
      $o.expectedComparisonOriginal=$n.expectedCertificateComparisonOriginal
      $classification=$o.classificationOriginal;$comparison=$o.expectedComparisonOriginal
      if(-not $c.providerCallReturned -or $c.stateOriginal -eq [IntPtr]::Zero -or
          $c.providerDataOriginal -eq [IntPtr]::Zero -or $null -eq $classification -or
          -not [object]::ReferenceEquals($classification.sequenceOriginal,$sequence) -or
          -not [object]::ReferenceEquals($classification.captureOriginal,$c) -or
          -not $classification.returned -or -not $classification.completeObserved -or
          $null -ne $classification.exceptionOriginal -or
          -not (Test-Cli39PublisherSupportedRawGraph32 $p) -or
          -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p) -or
          -not (Test-Cli39ProviderSignerBorrowClosed32 $p) -or
          -not (Test-Cli39ProviderFieldsBorrowClosed32 $p) -or
          -not (Test-Cli39PublisherChainCaptureComplete32 $p)){throw 'Complete supported actual body custody/borrow roster required.'}
      foreach($parent in $c.namedOidParentsOriginal){
        if(-not (Test-Cli39PublisherRawOid32 $p $parent)){throw 'Every actual named OID reader/pin original must be complete.'}
      }
      foreach($range in $n.chainReservationOriginal.recordsOriginal){
        if(-not $range.absenceObserved -and $null -eq $range.aliasOriginal -and
            -not (Test-Cli39PublisherRawRange32 $c $range)){throw 'Every actual canonical chain page/copy worker must be complete.'}
      }
      if($null -eq $comparison -or -not [object]::ReferenceEquals($comparison.preparationOriginal,$p) -or
          -not [object]::ReferenceEquals($comparison.captureOriginal,$c) -or
          -not [object]::ReferenceEquals($comparison.policyOriginal,$n.policyOriginal) -or
          -not [object]::ReferenceEquals($comparison.expectedSubjectOriginal,$p.consumerOriginal.subjectOriginals['expected-publisher-cert']) -or
          -not [object]::ReferenceEquals($comparison.chargeOriginal,$n.custodyReservationOriginal) -or
          -not $comparison.chargeOriginal.comparisonChargeAttempted -or
          $comparison.chargeOriginal.comparisonCountOriginal -ne $sequence.primarySignerCountOriginal -or
          -not $comparison.constructionTerminal -or -not $comparison.returned -or
          $null -ne $comparison.exceptionOriginal -or
          $comparison.recordsOriginal.Count -ne $sequence.primarySignerCountOriginal){throw 'Actual complete expected-certificate comparison roster required.'}
      foreach($record in $comparison.recordsOriginal){
        if(-not $record.returned -or $null -ne $record.exceptionOriginal -or
            -not [object]::ReferenceEquals($record.expectedSubjectOriginal,$comparison.expectedSubjectOriginal) -or
            -not (Test-Cli39ProviderSignerMember32 $c $record.signerOriginal)){throw 'Every actual certificate comparison disposition must be retained.'}
      }
      # A mismatch/Wintrust error is raw rejection data, not incomplete custody.
    }
    $sequence.returned=$true;$sequence.state='COMPLETE_SUPPORTED_API_VISIBLE_CUSTODY'
    $o.returned=$true;$c.returned=$true
    return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.constructionTerminal=$true}
}