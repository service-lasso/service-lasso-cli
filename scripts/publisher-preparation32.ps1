# Fixed publisher preparation under SAME shared owner. This retains complete
# held subject bytes; their namespace/loaded-source authority belongs to the
# actual admitted root collector, not digest equality or this constructor.
function Prepare-Cli39PublisherPrerequisite32 {
  param([Parameter(Mandatory)]$OriginalPreparation,
    [Parameter(Mandatory)]$AbiOriginal)
  $p=$OriginalPreparation;$c=$p.consumerOriginal
  if($null -eq $c -or $c.slot -cne 'cli39.windows-publisher-trust.v1' -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $null -ne $p.preparationOriginal -or $p.nativeIssued -or
      $c.ContainsKey('publisherNativeOriginal')){throw 'SAME once-only publisher enrollment required.'}
  $capture=@{subjects=@{};outputs=[Collections.Generic.List[object]]::new(0);
    abiOriginal=$AbiOriginal;started=$true;returned=$false;exceptionOriginal=$null;
    state='HELD_RAW_PUBLISHER_SUBJECTS_NOT_NATIVE_AUTHORITY';nativeOriginal=$null}
  $p.preparationOriginal=$capture;$c.originalPreparation=$p
  $c.subjectOriginals=$capture.subjects;$c.outputOriginals=$capture.outputs
  # Nine managed phases = three read/hash workers and each actual stream/hash
  # disposal independently. Native/provider capture phases have separate full
  # genuine reservations; this source count never implies a byte/stack fit.
  $n=@{abiOriginal=$AbiOriginal;subjectOriginal=$null;storageBytes=0L;
    storageAcquireOriginal=$null;storageReleaseOriginal=$null;
    moduleOwners=[Collections.Generic.List[object]]::new(1);wintrustModuleOriginal=$null;
    winVerifyTrustOriginal=$null;delegateType=$null;initializeWorkerOriginal=$null;
    actionPointer=[IntPtr]::Zero;wtdPointer=[IntPtr]::Zero;
    verifyWorkerOriginal=$null;providerCaptureOriginal=$null;closeWorkerOriginal=$null;
    completeSourceProfileOriginal=$null;namespaceAssociationOriginal=$null;
    loadedAssociationOriginal=$null;oidReservationOriginal=$null;usefulReturned=$false;
    policyOriginal=$null;policyEvidenceOriginals=$null}
  $c.publisherNativeOriginal=$n;$capture.nativeOriginal=$n
  try{
    $root=$c.rootCaptureOriginal;$policy=$root.publisherPolicyOriginal
    if($null -eq $root.nativeAssociationOriginals -or
        -not [object]::ReferenceEquals($root.admittedConsumerOriginal,$c) -or
        $null -eq $policy -or -not $policy.constructorReturned -or
        -not $policy.constructionTerminal -or $null -ne $policy.exceptionOriginal -or
        -not [object]::ReferenceEquals($policy.rootCaptureOriginal,$root) -or
        -not [object]::ReferenceEquals($policy.rootDocumentOriginal,$root.resultOriginal.rootDocumentOriginal)){
      throw 'SAME actual root policy original required before fixed evidence acquisition.'
    }
    $n.policyOriginal=$policy
    $n.policyEvidenceOriginals=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $profile=$c.rootCaptureOriginal.resultOriginal.consumerProfileOriginal
    $book=@{preparationOriginal=$p;reservationOriginal=$p.reservation;
      profileOriginal=$profile;budgetOriginal=$p.reservation.budget;recordsOriginal=$null;
      usedChargeBytes=[decimal]0;perOidChargeBytes=[decimal]0;maximumOriginals=0;
      constructionTerminal=$false;constructionFailure=$null;terminalLifetime='host-process'}
    $n.oidReservationOriginal=$book
    try{
      if($null -eq $profile -or -not $profile.returned -or $profile.role -cne 'consumerBudget' -or
          $profile.actionOriginal -cne 'publisher' -or
          -not $c.rootCaptureOriginal.profileOriginals.Contains($profile) -or
          -not [object]::ReferenceEquals($profile.valuesOriginal,$p.reservation.budget)){
        throw 'Actual typed publisher consumer profile and SAME reservation budget required.'
      }
      # Fixed journals are charged before any provider capture construction.
      $custody=@{preparationOriginal=$p;reservationOriginal=$p.reservation;profileOriginal=$profile;
        budgetOriginal=$p.reservation.budget;usedChargeBytes=[decimal]$profile.valuesOriginal.publisherCustodyBookChargeBytes;
        limitBytes=[decimal]$profile.valuesOriginal.publisherCustodyReservationBytes;
        perSignerChargeBytes=[decimal]$profile.valuesOriginal.publisherCustodyPerSignerChargeBytes;
        comparisonChargeAttempted=$false;comparisonCountOriginal=$null;comparisonChargeBytes=[decimal]0;
        timeObservationChargeAttempted=$false;timeObservationCountOriginal=$null;timeObservationChargeBytes=[decimal]0;
        anchorObservationChargeAttempted=$false;anchorObservationCountOriginal=$null;anchorObservationChargeBytes=[decimal]0;
        evaluationObservationChargeAttempted=$false;evaluationObservationCountOriginal=$null;evaluationObservationChargeBytes=[decimal]0;
        revocationObservationChargeAttempted=$false;revocationObservationCountOriginal=$null;revocationObservationChargeBytes=[decimal]0;
        timestampObservationChargeAttempted=$false;timestampObservationCountOriginal=$null;timestampObservationChargeBytes=[decimal]0;
        semanticObservationChargeAttempted=$false;semanticObservationCountOriginal=$null;semanticObservationChargeBytes=[decimal]0;
        state='SOURCE_GEOMETRY_ONLY_GENUINE_FULL_ALLOCATOR_PROFILE_REQUIRED'}
      $n.custodyReservationOriginal=$custody
      if($custody.usedChargeBytes -le 0 -or $custody.perSignerChargeBytes -le 0 -or
          $custody.usedChargeBytes+$custody.perSignerChargeBytes -gt $custody.limitBytes){throw 'Complete fixed publisher custody charge required.'}
      $book.perOidChargeBytes=[decimal]$profile.publisherOidChargeBytes
      $limit=[decimal]$book.budgetOriginal.publisherOidCaptureReservationBytes
      $book.usedChargeBytes=[decimal]$book.budgetOriginal.publisherOidCaptureBookChargeBytes
      if($book.perOidChargeBytes -le 0 -or $book.usedChargeBytes -le 0 -or
          $limit -lt ($book.usedChargeBytes+$book.perOidChargeBytes)){throw 'Complete source-selected OID charge geometry required.'}
      $maximum=[decimal]::Floor(($limit-$book.usedChargeBytes)/$book.perOidChargeBytes)
      if($maximum -le 0 -or $maximum -gt [int]::MaxValue){throw 'Supported actual reservation-derived OID roster required.'}
      $book.maximumOriginals=[int]$maximum
      $book.recordsOriginal=[Collections.Generic.List[object]]::new($book.maximumOriginals)
    }catch{$book.constructionFailure=$_;throw}finally{$book.constructionTerminal=$true}
    $null=New-Cli39PublisherChainBook32 $p
    $c.managedPhases=[Collections.Generic.List[object]]::new(9)
    if($c.geometry.managedPhaseCount -ne 9 -or $c.geometry.workerStackBytes -le 0 -or
        $c.geometry.workerStackBytes -gt [int]::MaxValue){throw 'Complete fixed three-subject publisher phase geometry required.'}
    foreach($role in @('publisher-subject','wintrust','expected-publisher-cert')){
      $pin=$c.subjectPins[$role]
      if($null -eq $pin -or -not [IO.Path]::IsPathFullyQualified($pin.path) -or
          $pin.path.Contains([char]0) -or $pin.bytes -le 0 -or
          $pin.bytes -ge [int]::MaxValue -or $pin.sha256 -cnotmatch '^[A-Fa-f0-9]{64}$'){
        throw 'Complete exact publisher subject pin required.'
      }
      if($role -ceq 'wintrust' -and [IO.Path]::GetFileName($pin.path) -ine 'wintrust.dll'){
        throw 'Exact admitted resident Wintrust input path required.'
      }
      $selector=switch -CaseSensitive ($role){
        'publisher-subject' {'subjectSha256'}
        'expected-publisher-cert' {'expectedPublisherCertificateSha256'}
        'wintrust' {$null}
      }
      if($null -ne $selector -and $pin.sha256.ToLowerInvariant() -cne $policy.valuesOriginal[$selector]){
        throw 'Actual fixed evidence pin differs from SAME policy selector.'
      }
      $s=@{role=$role;path=$pin.path;stream=$null;handleOriginal=$null;bytes=$null;
        openAttempted=$false;openReturned=$false;hashAttempted=$false;hashReturned=$false;
        used=0;eof=$false;digest=$null;hashOriginal=$null;exception=$null}
      $capture.subjects.Add($role,$s)
      if($role -ceq 'publisher-subject'){$n.subjectOriginal=$s}
      if($role -cnotin @('publisher-subject','wintrust')){$n.policyEvidenceOriginals.Add($role,$s)}
      $s.bytes=[byte[]]::new([int]($pin.bytes+1))
      $worker=New-Cli39ManagedPhase26 $p 'subject-read-hash' $s
      $null=Start-Cli39ManagedPhase26 $worker;$null=Wait-Cli39ManagedPhase26 $worker
      if(-not $s.eof -or $s.used -ne $pin.bytes){throw 'Complete original publisher subject EOF/extent differs.'}
      $s.digest=[BitConverter]::ToString($s.digestOriginal).Replace('-','').ToLowerInvariant()
      if($s.digest -ine $pin.sha256){throw 'Complete original publisher subject digest differs.'}
    }
    $null=Join-Cli39PreparedPublisherFoundation32 $p
    $null=Join-Cli39PublisherPreparedIdentity32 $p
    # Fixed SDK x64 prefix168 plus entire UTF16 terminated path. The actual
    # strict encoder and native page allocation remain independently retained
    # later phase originals, including every failure and allocator rounding.
    [long]$pathBytes=2L*([long]$n.subjectOriginal.path.Length+1L)
    if($pathBytes -gt ([int]::MaxValue-168L)){throw 'Complete publisher layout exceeds supported retained array extent.'}
    $n.storageBytes=168L+$pathBytes
    Assert-Cli39PrerequisiteUseful26 $p
    $capture.returned=$true;return $capture
  }catch{$capture.exceptionOriginal=$_;throw}
}



