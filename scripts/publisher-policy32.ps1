# Fixed structural policy original, not a trust issuer or native admission.
function Read-Cli39PublisherFoundationSelector32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($root.actionOriginal -cne 'publisher' -or -not $root.structuralReturned -or
      $root.ContainsKey('publisherFoundationSelectorOriginal') -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$p.receiver)){
    throw 'SAME once-only live publisher root required.'
  }
  $o=@{hostOwnerOriginal=$p;rootCaptureOriginal=$root;mapsOriginal=$null;fieldsOriginal=$null;
    expectedCertificateSourceMemberOrdinalOriginal=0;wintrustLoadedMemberOrdinalOriginal=0;
    expectedCertificateMemberOriginal=$null;wintrustMemberOriginal=$null;
    expectedCertificateNamespaceOriginal=$null;wintrustNamespaceOriginal=$null;
    bindingAttempted=$false;bindingReturned=$false;bindingFailureOriginal=$null;
    returned=$false;constructionTerminal=$false;exceptionOriginal=$null;qualified=$false;
    state='LOCAL_HELD_MEMBER_SELECTION_NOT_OS_STORE_POLICY_AUTHENTICATION'}
  $root.publisherFoundationSelectorOriginal=$o
  try{
    $o.mapsOriginal=[Collections.Generic.List[object]]::new(1)
    $o.fieldsOriginal=Read-Cli39ConsumerMap32 $o $root.fieldMapOriginal['publisherFoundation'] @('kind','expectedCertificateSourceMemberOrdinal','wintrustLoadedMemberOrdinal') 'publisher-foundation'
    if($o.fieldsOriginal['kind'].ValueKind -ne [System.Text.Json.JsonValueKind]::String -or
        $o.fieldsOriginal['kind'].GetString() -cne 'windows-default-authenticode-foundation.v1'){
      throw 'Only fixed local default Windows policy foundation selector exists.'
    }
    foreach($key in @('expectedCertificateSourceMemberOrdinal','wintrustLoadedMemberOrdinal')){
      $element=$o.fieldsOriginal[$key];$ordinal=0L
      $domain=if($key -ceq 'expectedCertificateSourceMemberOrdinal'){'sourceClosure'}else{'loadedClosure'}
      if($element.ValueKind -ne [System.Text.Json.JsonValueKind]::Number -or
          $element.GetRawText() -cnotmatch '^(0|[1-9][0-9]*)$' -or
          -not $element.TryGetInt64([ref]$ordinal) -or $ordinal -ge $root.fieldMapOriginal[$domain].GetArrayLength()){
        throw 'Exact existing held foundation member ordinal required.'
      }
      $o[($key+'Original')]=[int]$ordinal
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.constructionTerminal=$true}
}

function Join-Cli39PublisherFoundationMembers32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$o=$root.publisherFoundationSelectorOriginal
  $members=$root.heldMemberBookOriginal;$namespace=$root.namespaceBookOriginal
  if($null -eq $o -or -not $o.returned -or $o.bindingAttempted -or
      -not [object]::ReferenceEquals($o.hostOwnerOriginal,$p) -or
      -not $members.readReturned -or -not $namespace.observationReturned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $namespace.disposalOriginal){
    throw 'SAME once-only live held foundation member join required.'
  }
  $o.bindingAttempted=$true
  try{
    foreach($role in @('expectedCertificate','wintrust')){
      Assert-Cli39ConsumerReadDeadline32 $o
      $domain=if($role -ceq 'expectedCertificate'){'sourceClosure'}else{'loadedClosure'}
      $ordinal=if($role -ceq 'expectedCertificate'){$o.expectedCertificateSourceMemberOrdinalOriginal}else{$o.wintrustLoadedMemberOrdinalOriginal}
      foreach($member in $members.recordsOriginal){
        if($member.domainOriginal -ceq $domain -and $member.ordinalOriginal -eq $ordinal){
          if($null -ne $o[($role+'MemberOriginal')]){throw 'Duplicate actual foundation ordinal retained.'}
          $o[($role+'MemberOriginal')]=$member
        }
      }
      $member=$o[($role+'MemberOriginal')];$subject=$member.subjectOriginal
      if($null -eq $member -or -not (Test-Cli39HeldMemberSubject32 $p $subject) -or
          -not $subject.eof -or -not $subject.hashReturned -or
          $subject.used -ne $member.bytesExpected -or $subject.digest -cne $member.sha256Expected -or
          -not $member.fileQueryOriginal.rawReturned){throw 'Actual complete held foundation member required.'}
      foreach($owner in $namespace.pathOwnersOriginal){
        if([object]::ReferenceEquals($owner.memberOriginal,$member)){
          if($null -ne $o[($role+'NamespaceOriginal')]){throw 'Duplicate actual foundation namespace owner retained.'}
          $o[($role+'NamespaceOriginal')]=$owner
        }
      }
      $owner=$o[($role+'NamespaceOriginal')]
      if($null -eq $owner -or -not $owner.joinReturned -or $owner.leafDirectoryOriginal -or
          -not [object]::ReferenceEquals($owner.subjectOriginal,$subject) -or
          -not [object]::ReferenceEquals($owner.queryOriginal,$member.fileQueryOriginal)){
        throw 'Actual SAME foundation namespace/file query original required.'
      }
      $leaf=$owner.holdsOriginal[$owner.holdsOriginal.Count-1]
      if(-not $leaf.leafOriginal -or -not $leaf.identityJoined -or $leaf.identityBytesCompared -ne 24){throw 'Full actual foundation volume/FileId join required.'}
      $pinRole=if($role -ceq 'expectedCertificate'){'expected-publisher-cert'}else{'wintrust'}
      $pin=$root.consumerShapeOriginal.prototypeOriginal.subjectPins[$pinRole]
      if($pin.path -cne $subject.path -or $pin.bytes -ne $subject.used -or $pin.sha256.ToLowerInvariant() -cne $subject.digest){
        throw 'Fixed publisher subject pin differs from selected actual foundation member.'
      }
      if($role -ceq 'wintrust' -and [IO.Path]::GetFileName($subject.path) -ine 'wintrust.dll'){throw 'Fixed Wintrust loaded member shape required.'}
    }
    $o.bindingReturned=$true;return $o
  }catch{$o.bindingFailureOriginal=$_;throw}
}

function Join-Cli39PreparedPublisherFoundation32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$root=$c.rootCaptureOriginal;$n=$c.publisherNativeOriginal
  $selector=$root.publisherFoundationSelectorOriginal
  if($null -eq $selector -or -not $selector.bindingReturned -or
      -not [object]::ReferenceEquals($selector.rootCaptureOriginal,$root) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $n.ContainsKey('preparedFoundationOriginal')){throw 'SAME once-only prepared foundation member relation required.'}
  $o=@{preparationOriginal=$p;selectorOriginal=$selector;recordsOriginal=$null;
    returned=$false;constructionTerminal=$false;exceptionOriginal=$null;qualified=$false;
    state='EXACT_HELD_MEMBER_BYTES_NOT_OS_STORE_OR_LOADED_BYTE_AUTHENTICATION'}
  $n.preparedFoundationOriginal=$o
  try{
    $o.recordsOriginal=[Collections.Generic.List[object]]::new(2)
    foreach($role in @('expected-publisher-cert','wintrust')){
      $member=if($role -ceq 'expected-publisher-cert'){$selector.expectedCertificateMemberOriginal}else{$selector.wintrustMemberOriginal}
      $held=$member.subjectOriginal;$prepared=$c.subjectOriginals[$role]
      $r=@{roleOriginal=$role;memberOriginal=$member;heldSubjectOriginal=$held;
        preparedSubjectOriginal=$prepared;comparedBytes=0;equalBytes=$false;returned=$false;exceptionOriginal=$null}
      $o.recordsOriginal.Add($r)
      try{
        if(-not $prepared.eof -or -not $prepared.hashReturned -or -not $held.eof -or
            $prepared.used -ne $held.used -or $prepared.path -cne $held.path -or
            $prepared.digest -cne $held.digest){throw 'Actual prepared/selected foundation extent differs.'}
        for($i=0;$i -lt $held.used;$i++){
          Assert-Cli39PrerequisiteUseful26 $p
          $equal=($prepared.bytes[$i] -eq $held.bytes[$i]);$r.comparedBytes=$i+1
          if(-not $equal){throw 'Actual full prepared foundation bytes differ.'}
        }
        $r.equalBytes=($r.comparedBytes -eq $held.used);$r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.constructionTerminal=$true}
}

function Observe-Cli39PublisherPolicyResults32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$capture=$n.providerCaptureOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      $n.ContainsKey('policyResultOriginal')){throw 'SAME once-only actual publisher policy-result observations required.'}
  $o=@{preparationOriginal=$p;captureOriginal=$capture;verifyWorkerOriginal=$n.verifyWorkerOriginal;
    rawTrustLongOriginal=$null;providerHeaderOriginal=$capture.headerOriginal;
    providerFieldsOriginal=$null;trustStepErrorsOriginal=$null;rawGraphOriginal=$null;
    signerRecordsOriginal=$null;chainRecordsOriginal=$null;providerStateAbsentObserved=$false;
    providerOperationZeroErrorsObserved=$false;zeroRawErrorsObserved=$false;returned=$false;exceptionOriginal=$null;
    state='RAW_POLICY_RESULTS_NOT_AUTHENTIC_POLICY_OR_FRESHNESS_ACCEPTANCE'}
  $n.policyResultOriginal=$o
  try{
    $verify=$o.verifyWorkerOriginal
    if($null -eq $verify -or -not $verify.terminalJoined -or $verify.thread.IsAlive -or
        $verify.results[1] -ne 1 -or $null -ne $verify.exceptionOriginal[0]){throw 'Actual terminal verification LONG required.'}
    $o.rawTrustLongOriginal=$verify.results[0]
    if($null -eq $capture.headerOriginal){
      # The capture driver's explicit state-zero witness remains required;
      # no missing header alone supplies an absence or positive observation.
      $o.providerStateAbsentObserved=($capture.returned -and $capture.sequenceOriginal.noStateObserved)
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended absent-state policy-result publication.'}
      $o.returned=$true;return $o
    }
    $o.rawGraphOriginal=$capture.sequenceOriginal.rawGraphClassificationOriginal
    if($null -eq $o.rawGraphOriginal -or -not $o.rawGraphOriginal.returned -or
        -not $o.rawGraphOriginal.completeObserved){throw 'Complete supported actual raw graph required for policy-result traversal.'}
    $o.providerFieldsOriginal=$capture.headerOriginal.decodedOriginal.fieldsOriginal
    $o.trustStepErrorsOriginal=$capture.providerFieldOriginals['trust-step-errors'].decodedOriginal
    $o.signerRecordsOriginal=$capture.signerOriginals
    $o.chainRecordsOriginal=$n.chainReservationOriginal.recordsOriginal
    $zero=($o.rawTrustLongOriginal -eq 0 -and $o.providerFieldsOriginal['dwError'] -eq 0 -and
      $o.providerFieldsOriginal['dwFinalError'] -eq 0 -and $capture.signerOriginals.Count -gt 0)
    foreach($errorCode in $o.trustStepErrorsOriginal.itemsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended trust-step error observation.'}
      if($errorCode -ne 0){$zero=$false}
    }
    # Preserve native operation/trust-step outcome separately from every
    # captured alternative chain/signature error. Neither is authentication.
    $o.providerOperationZeroErrorsObserved=$zero
    foreach($primary in $o.signerRecordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended primary policy-result observation.'}
      if(-not (Test-Cli39ProviderSignerMember32 $capture $primary) -or
          -not $primary.headerRangeOriginal.decodedOriginal.returned){throw 'Actual supported primary policy fields required.'}
      if($primary.headerRangeOriginal.decodedOriginal.fieldsOriginal['dwError'] -ne 0){$zero=$false}
      foreach($counter in $primary.counterSignerOriginals){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended counter policy-result observation.'}
        if(-not (Test-Cli39ProviderSignerMember32 $capture $counter) -or
            -not $counter.headerRangeOriginal.decodedOriginal.returned -or
            -not [object]::ReferenceEquals($counter.primarySignerOriginal,$primary)){throw 'Actual supported counter policy fields required.'}
        if($counter.headerRangeOriginal.decodedOriginal.fieldsOriginal['dwError'] -ne 0){$zero=$false}
      }
    }
    foreach($range in $o.chainRecordsOriginal){
      if($range.kindOriginal -cnotin @('chain-context','simple-chain','element','revocation')){continue}
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended chain policy-result observation.'}
      if(-not $range.decodedReturned){throw 'Actual complete captured chain policy fields required.'}
      if($range.absenceObserved){continue}
      if($range.kindOriginal -ceq 'revocation'){
        if($range.fieldsOriginal['dwRevocationResult'] -ne 0){$zero=$false}
      }elseif($range.fieldsOriginal['dwErrorStatus'] -ne 0){$zero=$false}
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline ended policy-result publication.'}
    $o.zeroRawErrorsObserved=$zero;$o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function New-Cli39PublisherPolicyAssociation32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$root=$c.rootCaptureOriginal
  $n=$c.publisherNativeOriginal;$policy=$n.policyOriginal
  if($c.slot -cne 'cli39.windows-publisher-trust.v1' -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($root.admittedConsumerOriginal,$c) -or
      -not [object]::ReferenceEquals($policy,$root.publisherPolicyOriginal) -or
      $null -eq $n.providerCaptureOriginal -or -not $n.preparedFoundationOriginal.returned -or
      $policy.ContainsKey('associationConstructionOriginal')){
    throw 'SAME once-only actual publisher policy association construction required.'
  }
  # This constructor accepts no candidate-supplied authority argument. Its
  # fixed sources are actual enrolled originals, never digest-file contents.
  $o=@{preparationOriginal=$p;rootCaptureOriginal=$root;policyOriginal=$policy;
    reservationOriginal=$p.reservation;receiverOriginal=$p.receiver;
    subjectOriginal=$n.subjectOriginal;expectedCertificateOriginal=$null;
    wintrustSubjectOriginal=$null;wintrustModuleOriginal=$n.wintrustModuleOriginal;
    verificationOriginal=$n.verifyWorkerOriginal;captureOriginal=$n.providerCaptureOriginal;
    certificateComparisonOriginal=$n.expectedCertificateComparisonOriginal;
    nativeRootAssociationOriginal=$root.nativeAssociationOriginals;
    foundationSelectorOriginal=$root.publisherFoundationSelectorOriginal;
    preparedFoundationOriginal=$n.preparedFoundationOriginal;providerResultOriginal=$null;
    windowsPolicyFoundationOriginal=$null;foundationTransportOriginal=$null;timeResultOriginal=$null;anchorResultOriginal=$null;evaluationResultOriginal=$null;revocationResultOriginal=$null;timestampResultOriginal=$null;semanticResultOriginal=$null;
    relationOriginals=$null;localCorrespondenceComplete=$false;
    missingOriginals=$null;returned=$false;constructionTerminal=$false;
    exceptionOriginal=$null;qualified=$false;
    state='FIXED_ORIGINAL_RELATIONS_AUTHENTIC_FOUNDATION_AND_POLICY_JOINS_PENDING'}
  $policy.associationConstructionOriginal=$o
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline ended policy association construction.'}
    $o.missingOriginals=[Collections.Generic.List[string]]::new(8)
    $o.expectedCertificateOriginal=$c.subjectOriginals['expected-publisher-cert']
    $o.wintrustSubjectOriginal=$c.subjectOriginals['wintrust']
    $o.foundationTransportOriginal=$root.windowsPolicyFoundationTransportOriginal
    $transport=$o.foundationTransportOriginal
    if($null -eq $transport -or -not $transport.returned -or -not $transport.terminal -or
        $null -ne $transport.exceptionOriginal -or
        -not [object]::ReferenceEquals($transport.rootOriginal,$root) -or
        -not [object]::ReferenceEquals($transport.policyOriginal,$policy) -or
        -not [object]::ReferenceEquals($transport.selectorOriginal,$o.foundationSelectorOriginal) -or
        -not [object]::ReferenceEquals($transport.parentTransportOriginal,$root.childDerivedTransportOriginal) -or
        $transport.subjectRowsOriginal.Count -ne 3){throw 'Actual SAME completed publisher foundation operand transport required.'}
    foreach($row in $transport.subjectRowsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or -not $row.returned){throw 'Original deadline/incomplete publisher transport subject.'}
    }
    $o.providerResultOriginal=Observe-Cli39PublisherPolicyResults32 $p
    $o.timeResultOriginal=Observe-Cli39PublisherTimeResults32 $p
    $o.anchorResultOriginal=Observe-Cli39PublisherAnchorResults32 $p
    $o.evaluationResultOriginal=Observe-Cli39PublisherEvaluationResults32 $p
    $o.revocationResultOriginal=Observe-Cli39PublisherRevocationResults32 $p
    $o.timestampResultOriginal=Observe-Cli39PublisherTimestampResults32 $p
    if(-not [object]::ReferenceEquals($o.subjectOriginal,$c.subjectOriginals['publisher-subject']) -or
        -not [object]::ReferenceEquals($n.policyEvidenceOriginals['expected-publisher-cert'],$o.expectedCertificateOriginal) -or
        ($null -ne $o.captureOriginal.headerOriginal -and
          (-not [object]::ReferenceEquals($o.certificateComparisonOriginal.preparationOriginal,$p) -or
           -not [object]::ReferenceEquals($o.certificateComparisonOriginal.captureOriginal,$o.captureOriginal) -or
           -not [object]::ReferenceEquals($o.certificateComparisonOriginal.expectedSubjectOriginal,$o.expectedCertificateOriginal)))){
      throw 'Actual held subject/certificate/provider comparison originals differ.'
    }
    $o.semanticResultOriginal=Join-Cli39PublisherSemanticResults32 $o
    $execution=$n.executionBindingOriginal
    if($null -eq $execution -or -not $execution.returned -or -not $execution.terminal -or
        $null -ne $execution.exceptionOriginal -or
        -not [object]::ReferenceEquals($execution.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($execution.rootOriginal,$root) -or
        -not [object]::ReferenceEquals($execution.transportOriginal,$transport) -or
        -not [object]::ReferenceEquals($n.namespaceAssociationOriginal,$execution.preparedIdentityOriginal) -or
        -not [object]::ReferenceEquals($n.loadedAssociationOriginal,$execution) -or
        -not [object]::ReferenceEquals($n.completeSourceProfileOriginal,$execution.sourceProfileOriginal) -or
        -not [object]::ReferenceEquals($execution.sourceProfileOriginal.reservationOriginal,$p.reservation) -or
        -not [object]::ReferenceEquals($o.semanticResultOriginal.associationOriginal,$o)){
      throw 'Actual complete SAME publisher execution/semantic source relations required.'}
    # Opaque authentic parent originals stay on the controlling parent. These
    # are completed local relations under that selected genuine input premise,
    # not an authentication flag or cross-process CLR identity transport.
    $o.relationOriginals=@{executionOriginal=$execution;sourceProfileOriginal=$n.completeSourceProfileOriginal;
      namespaceOriginal=$n.namespaceAssociationOriginal;loadedOriginal=$n.loadedAssociationOriginal;
      expectedPublisherOriginal=$o.certificateComparisonOriginal;semanticOriginal=$o.semanticResultOriginal;
      timeOriginal=$o.timeResultOriginal;revocationOriginal=$o.revocationResultOriginal;
      parentTransportOriginal=$transport.parentTransportOriginal;
      qualificationPremise='GENUINE_EXTERNALLY_QUALIFIED_WHOLE_INPUT_OS_STORE_POLICY_SOURCE_CAPACITY_REQUIRED'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies supported publisher relation publication.'}
    $o.localCorrespondenceComplete=$o.semanticResultOriginal.supportedRelationsObserved
    if(-not $o.localCorrespondenceComplete){$o.missingOriginals.Add('supported-expected-publisher-policy-candidate')}
    $o.returned=$true
    return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.constructionTerminal=$true}
}

function Read-Cli39PublisherPolicyOriginal32 {
  param([Parameter(Mandatory)]$RootCaptureOriginal,[Parameter(Mandatory)]$HostOwnerOriginal)
  $root=$RootCaptureOriginal;$hostOriginal=$HostOwnerOriginal
  if(-not [object]::ReferenceEquals($hostOriginal.inputRead.value,$root) -or
      -not $root.structuralReturned -or $root.actionOriginal -cne 'publisher' -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$hostOriginal.receiver) -or
      $root.ContainsKey('publisherPolicyOriginal')){throw 'SAME once-only held publisher root required.'}
  $o=@{rootCaptureOriginal=$root;hostOwnerOriginal=$hostOriginal;
    rootSubjectOriginal=$root.resultOriginal.rootSubjectOriginal;
    rootDocumentOriginal=$root.resultOriginal.rootDocumentOriginal;
    consumerElementOriginal=$root.fieldMapOriginal['consumer'];policyElementOriginal=$null;
    consumerPolicyPropertiesOriginal=$null;
    propertyOriginals=$null;valuesOriginal=$null;constructorReturned=$false;
    constructionTerminal=$false;exceptionOriginal=$null;
    nativeAssociationOriginal=$null;expectedPublisherEvidenceOriginal=$null;
    baselineEvidenceOriginal=$null;revocationEvidenceOriginal=$null;
    validationEvidenceOriginal=$null;sourceEvidenceOriginal=$null;
    state='STRUCTURAL_POLICY_ORIGINAL_GENUINE_EVIDENCE_ASSOCIATIONS_PENDING'}
  $root.publisherPolicyOriginal=$o
  try{
    Observe-OwnedPrerequisiteDeadline $hostOriginal
    if($hostOriginal.deadlineReported){throw 'Original deadline forbids new policy reader work.'}
    $o.consumerPolicyPropertiesOriginal=[Collections.Generic.List[object]]::new(2)
    foreach($member in $o.consumerElementOriginal.EnumerateObject()){
      Observe-OwnedPrerequisiteDeadline $hostOriginal
      if($hostOriginal.deadlineReported){throw 'Original deadline ended policy member discovery.'}
      if($member.Name -ceq 'publisherPolicy'){
        $o.consumerPolicyPropertiesOriginal.Add($member)
        if($o.consumerPolicyPropertiesOriginal.Count -ne 1){throw 'Duplicate decoded publisherPolicy member retained.'}
        $o.policyElementOriginal=$member.Value
      }
    }
    if($o.consumerPolicyPropertiesOriginal.Count -ne 1 -or
        $o.policyElementOriginal.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Actual fixed publisherPolicy object required.'}
    $policy=$o.policyElementOriginal
    $keys=[string[]]@('schema','expectedPublisherCertificateSha256','expectedPublisherIdentity',
      'subjectSha256','policyId','timestampMode')
    $o.propertyOriginals=[Collections.Generic.List[object]]::new(7)
    $o.valuesOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
    foreach($property in $policy.EnumerateObject()){
      Observe-OwnedPrerequisiteDeadline $hostOriginal
      if($hostOriginal.deadlineReported){throw 'Original deadline ended policy reader work.'}
      $o.propertyOriginals.Add($property)
      if($property.Name -cnotin $keys -or $o.valuesOriginal.ContainsKey($property.Name) -or
          $property.Value.ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Unknown/duplicate/nonstring publisher policy field.'}
      $text=$property.Value.GetString()
      if([string]::IsNullOrEmpty($text) -or $text.Contains([char]0)){throw 'Exact nonempty publisher policy string required.'}
      $o.valuesOriginal.Add($property.Name,$text)
    }
    if($o.valuesOriginal.Count -ne $keys.Count -or
        $o.valuesOriginal['schema'] -cne 'service-lasso.cli39-publisher-policy.v2' -or
        $o.valuesOriginal['policyId'] -cne 'default-authenticode-held-file-v1' -or
        $o.valuesOriginal['timestampMode'] -cnotin @('qualified-provider-timestamp','qualified-current-policy')){throw 'Complete fixed publisher policy shape required.'}
    foreach($key in @('expectedPublisherCertificateSha256','subjectSha256')){
      if($o.valuesOriginal[$key] -cnotmatch '^[0-9a-f]{64}$'){throw 'Canonical exact evidence digest required; digest is not authentication.'}
    }
    $o.constructorReturned=$true
    return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.constructionTerminal=$true}
}


function Compare-Cli39PublisherExpectedCertificate32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$policy=$n.policyOriginal
  $expected=$n.policyEvidenceOriginals['expected-publisher-cert']
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($policy,$c.rootCaptureOriginal.publisherPolicyOriginal) -or
      -not [object]::ReferenceEquals($expected,$c.subjectOriginals['expected-publisher-cert']) -or
      -not $expected.eof -or -not $expected.hashReturned -or
      $expected.digest -cne $policy.valuesOriginal['expectedPublisherCertificateSha256'] -or
      $null -eq $capture -or -not $capture.sequenceOriginal.primarySignersReturned -or
      $n.ContainsKey('expectedCertificateComparisonOriginal')){throw 'SAME actual once-only expected certificate comparison required.'}
  $o=@{preparationOriginal=$p;policyOriginal=$policy;captureOriginal=$capture;
    expectedSubjectOriginal=$expected;recordsOriginal=$null;returned=$false;
    constructionTerminal=$false;exceptionOriginal=$null;matchedCount=0;
    state='EXACT_CERTIFICATE_BYTES_ONLY_AUTHENTIC_POLICY_AND_TRUST_PENDING'}
  $n.expectedCertificateComparisonOriginal=$o
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies expected certificate comparison construction.'}
    [uint32]$count=$capture.headerOriginal.decodedOriginal.fieldsOriginal['csSigners']
    if(($count -gt 0 -and $null -eq $capture.signerOriginals) -or
        ($null -ne $capture.signerOriginals -and $capture.signerOriginals.Count -ne $count)){throw 'Complete actual primary signer roster required.'}
    $charge=$n.custodyReservationOriginal
    if($null -eq $charge -or -not [object]::ReferenceEquals($charge.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($charge.reservationOriginal,$p.reservation) -or
        -not [object]::ReferenceEquals($charge.budgetOriginal,$p.reservation.budget) -or
        $charge.comparisonChargeAttempted -or $count -gt [int]::MaxValue){throw 'SAME once-only full comparison charge required.'}
    $charge.comparisonChargeAttempted=$true;$charge.comparisonCountOriginal=$count
    $charge.comparisonChargeBytes=[decimal]$count*$charge.perSignerChargeBytes
    if($charge.usedChargeBytes+$charge.comparisonChargeBytes -gt $charge.limitBytes){throw 'Actual comparison roster exceeds SAME custody subdivision.'}
    $charge.usedChargeBytes+=$charge.comparisonChargeBytes
    $o.chargeOriginal=$charge
    $o.recordsOriginal=[Collections.Generic.List[object]]::new([int]$count)
    foreach($s in $capture.signerOriginals){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies expected certificate comparison record.'}
      $r=@{signerOriginal=$s;certificateOriginal=$null;derOriginal=$null;
        expectedSubjectOriginal=$expected;extentMatches=$false;comparedBytes=0;
        byteMismatchObserved=$false;equalBytes=$false;signerErrorOriginal=$null;
        returned=$false;exceptionOriginal=$null}
      $o.recordsOriginal.Add($r)
      try{
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended expected certificate comparison.'}
        if(-not (Test-Cli39ProviderSignerMember32 $capture $s) -or
            $null -ne $s.primarySignerOriginal -or -not $s.certificateSequenceOriginal.returned -or
            $s.certificateOriginals.Count -le 0){throw 'Actual primary signer leaf certificate required.'}
        $r.signerErrorOriginal=$s.headerRangeOriginal.decodedOriginal.fieldsOriginal['dwError']
        $cert=$s.certificateOriginals[0];$r.certificateOriginal=$cert;$der=$cert.derOriginal;$r.derOriginal=$der
        if(-not (Test-Cli39ProviderCertificateMember32 $capture $cert) -or $cert.indexOriginal -ne 0 -or
            -not $der.rangeReturned -or -not $der.copyReturned -or
            $der.used -ne $der.bytesExpected -or $der.rawBytesOriginal.Length -ne $der.bytesExpected -or
            $null -ne $der.exceptionOriginal){throw 'Complete actual SAME leaf DER copy required.'}
        $r.extentMatches=($der.used -eq $expected.used)
        if($r.extentMatches){
          for($i=0;$i -lt $expected.used;$i++){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline ended certificate byte comparison.'}
            $different=($der.rawBytesOriginal[$i] -ne $expected.bytes[$i])
            $r.comparedBytes=$i+1
            if($different){$r.byteMismatchObserved=$true;break}
          }
          $r.equalBytes=(-not $r.byteMismatchObserved -and $r.comparedBytes -eq $expected.used)
          if($r.equalBytes){$o.matchedCount++}
        }
        $r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies expected certificate comparison publication.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.constructionTerminal=$true}
}

