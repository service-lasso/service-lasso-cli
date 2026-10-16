# Fixed in-host sixth consumer. No child process or second native owner.
function Invoke-Cli39PublisherPrerequisite32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AbiOriginal)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$root=$c.rootCaptureOriginal
  if($c.slot -cne 'cli39.windows-publisher-trust.v1' -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($root.admittedConsumerOriginal,$c) -or
      -not [object]::ReferenceEquals($root.consumerReservationOriginal,$p.reservation) -or
      -not [object]::ReferenceEquals($root.abiOriginal,$AbiOriginal) -or
      $null -eq $root.nativeAssociationOriginals -or $c.ContainsKey('publisherInvocationOriginal')){
    throw 'SAME admitted once-only fixed inhost publisher required.'
  }
  $o=@{preparationOriginal=$p;abiOriginal=$AbiOriginal;stagesOriginal=@{};
    firstFailureOriginal=$null;providerCaptureOriginal=$null;policyAssociationOriginal=$null;closureReturned=$false;
    usefulCompletionOriginal=$null;usefulReturned=$false;returned=$false;state='INCOMPLETE_PROVIDER_TRUST_AND_SOURCE_ASSOCIATION'}
  $c.publisherInvocationOriginal=$o
  foreach($name in @('prepare','storage','module','initialize','verify','capture','close','dispose','release','closure')){
    $o.stagesOriginal.Add($name,@{nameOriginal=$name;attempted=$false;returned=$false;
      original=$null;exceptionOriginal=$null})
  }
  $stage=$o.stagesOriginal['prepare']
  try{
    $stage.attempted=$true;$stage.original=Prepare-Cli39PublisherPrerequisite32 $p $AbiOriginal;$stage.returned=$true
    $n=$c.publisherNativeOriginal
    $stage=$o.stagesOriginal['storage'];$stage.attempted=$true
    $w=New-Cli39PublisherStorageWorker26 $p $AbiOriginal 'storage-acquire';$stage.original=$w
    $null=Start-Cli39PublisherWorker26 $w
    while(-not $w.terminalJoined){$null=Observe-Cli39PublisherWorker26 $w;if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}}
    if($w.results[1] -ne 1 -or $w.pointerOriginal[0] -eq [IntPtr]::Zero -or
        $null -ne $w.exceptionOriginal[0]){throw 'Actual complete positive publisher allocation required.'}
    $stage.returned=$true
    $stage=$o.stagesOriginal['module'];$stage.attempted=$true
    $w=New-Cli39PublisherModuleWorker26 $p;$stage.original=$w
    $null=Start-Cli39PublisherWorker26 $w
    while(-not $w.terminalJoined){$null=Observe-Cli39PublisherWorker26 $w;if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}}
    $null=Complete-Cli39PublisherModule26 $w
    $null=Join-Cli39PublisherExecutionBindings32 $p
    $stage.returned=$true
    $stage=$o.stagesOriginal['initialize'];$stage.attempted=$true
    $w=New-Cli39PublisherInitializationWorker26 $p;$stage.original=$w
    $null=Start-Cli39PublisherWorker26 $w
    while(-not $w.terminalJoined){$null=Observe-Cli39PublisherWorker26 $w;if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}}
    if($w.results[1] -ne 1 -or $null -ne $w.exceptionOriginal[0]){throw 'Actual same publisher initialization required.'}
    $stage.returned=$true
    $stage=$o.stagesOriginal['verify'];$stage.attempted=$true
    $w=New-Cli39PublisherWorker26 $p 'verify';$stage.original=$w
    $null=Start-Cli39PublisherWorker26 $w
    while(-not $w.terminalJoined){$null=Observe-Cli39PublisherWorker26 $w;if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}}
    if($w.results[1] -ne 1 -or $null -ne $w.exceptionOriginal[0]){throw 'Actual raw publisher verification return required.'}
    # A nonzero raw trust result still requires complete provider custody.
    $stage.returned=$true
    $stage=$o.stagesOriginal['capture'];$stage.attempted=$true
    $provider=Capture-Cli39PublisherProvider32 $p;$stage.original=$provider;$o.providerCaptureOriginal=$provider
    $o.policyAssociationOriginal=New-Cli39PublisherPolicyAssociation32 $p
    if(-not $provider.returned){throw 'Complete selected provider data custody/disposition remains pending.'}
    $stage.returned=$true
    $stage=$o.stagesOriginal['close'];$stage.attempted=$true
    $w=New-Cli39PublisherWorker26 $p 'close';$stage.original=$w
    $null=Start-Cli39PublisherWorker26 $w
    while(-not $w.terminalJoined){$null=Observe-Cli39PublisherWorker26 $w;if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}}
    if($w.results[1] -ne 1 -or $w.results[0] -ne 0 -or $null -ne $w.exceptionOriginal[0]){throw 'Actual successful once-only provider close required.'}
    $stage.returned=$true
  }catch{
    $stage.exceptionOriginal=$_;$o.firstFailureOriginal=$_
    # Constructors enroll partial actual originals before effects. Preserve
    # their association even when the call never assigned its return value.
    $n=if($c.ContainsKey('publisherNativeOriginal')){$c.publisherNativeOriginal}else{$null}
    if($null -eq $stage.original){
      switch -CaseSensitive ($stage.nameOriginal){
        'prepare'{$stage.original=$p.preparationOriginal}
        'storage'{if($null -ne $n){$stage.original=$n.storageAcquireOriginal}}
        'module'{if($null -ne $n -and $n.moduleOwners.Count -gt 0){$stage.original=$n.moduleOwners[0]}}
        'initialize'{if($null -ne $n){$stage.original=$n.initializeWorkerOriginal}}
        'verify'{if($null -ne $n){$stage.original=$n.verifyWorkerOriginal}}
        'capture'{if($null -ne $n){$stage.original=$n.providerCaptureOriginal;$o.providerCaptureOriginal=$n.providerCaptureOriginal}}
        'close'{if($null -ne $n){$stage.original=$n.closeWorkerOriginal}}
      }
    }
  }finally{
    # Policy construction can fail after complete capture. Visit independently
    # eligible provider close before subject disposal, retaining its own error.
    $n=if($c.ContainsKey('publisherNativeOriginal')){$c.publisherNativeOriginal}else{$null}
    if($null -ne $n -and $null -ne $n.providerCaptureOriginal -and
        $n.providerCaptureOriginal.collectionTerminal -and $n.providerCaptureOriginal.returned -and
        $n.providerCaptureOriginal.custodyCompletionOriginal.returned -and
        -not $o.stagesOriginal['close'].attempted -and
        $null -eq $n.closeWorkerOriginal){
      $stage=$o.stagesOriginal['close'];$stage.attempted=$true
      try{
        $w=New-Cli39PublisherWorker26 $p 'close';$stage.original=$w
        $null=Start-Cli39PublisherWorker26 $w
        while(-not $w.terminalJoined){$null=Observe-Cli39PublisherWorker26 $w;if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}}
        if($w.results[1] -ne 1 -or $w.results[0] -ne 0 -or $null -ne $w.exceptionOriginal[0]){throw 'Actual independent once-only provider close remains incomplete.'}
        $stage.returned=$true
      }catch{
        $stage.exceptionOriginal=$_;$stage.original=$n.closeWorkerOriginal
        if($null -eq $o.firstFailureOriginal){$o.firstFailureOriginal=$_}
      }
    }
    # Independent disposal is once-only. Its existing predicate refuses
    # destruction when actual provider data/borrowers remain required.
    $stage=$o.stagesOriginal['dispose'];$stage.attempted=$true
    try{$stage.original=Invoke-CliPrerequisiteDisposal26 $c.slot $p;$stage.returned=$true}
    catch{
      $stage.exceptionOriginal=$_
      if($c.ContainsKey('disposalOriginal')){$stage.original=$c.disposalOriginal}
      if($null -eq $o.firstFailureOriginal){$o.firstFailureOriginal=$_}
    }
    $n=if($c.ContainsKey('publisherNativeOriginal')){$c.publisherNativeOriginal}else{$null}
    $a=if($null -ne $n){$n.storageAcquireOriginal}else{$null}
    if($null -ne $a -and $a.terminalJoined -and $a.results[1] -eq 1 -and
        $a.pointerOriginal[0] -ne [IntPtr]::Zero -and $null -eq $n.storageReleaseOriginal){
      $stage=$o.stagesOriginal['release'];$stage.attempted=$true
      try{
        # Constructor itself enforces actual unentered/closed provider and
        # initialization borrow eligibility; no error-handler grant exists.
        $w=New-Cli39PublisherStorageWorker26 $p $AbiOriginal 'storage-release';$stage.original=$w
        $null=Start-Cli39PublisherWorker26 $w
        while(-not $w.terminalJoined){$null=Observe-Cli39PublisherWorker26 $w;if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}}
        if($w.results[1] -ne 1 -or $w.results[0] -eq 0 -or $null -ne $w.exceptionOriginal[0]){throw 'Actual once-only native backing free remains incomplete.'}
        $stage.returned=$true
      }catch{
        $stage.exceptionOriginal=$_;$stage.original=$n.storageReleaseOriginal
        if($null -eq $o.firstFailureOriginal){$o.firstFailureOriginal=$_}
      }
    }
    $stage=$o.stagesOriginal['closure'];$stage.attempted=$true
    try{$stage.original=Test-Cli39PrerequisiteClosure26 $c.slot $p;$stage.returned=$true;$o.closureReturned=($stage.original -eq $true)}
    catch{$stage.exceptionOriginal=$_;if($null -eq $o.firstFailureOriginal){$o.firstFailureOriginal=$_}}
    $o.returned=$true
  }
  $null=Complete-Cli39PublisherInvocationResult32 $o
  return $o
}

# Conditional local result after all independently completed source relations.
# Genuine input authenticity/physical qualification precedes command admission;
# this result does not manufacture those external facts.
function Complete-Cli39PublisherInvocationResult32 {
  param([Parameter(Mandatory)]$InvocationOriginal)
  $o=$InvocationOriginal;$p=$o.preparationOriginal;$c=$p.consumerOriginal
  if(-not [object]::ReferenceEquals($c.publisherInvocationOriginal,$o) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $null -ne $o.usefulCompletionOriginal -or -not $o.returned){throw 'SAME once-only completed publisher invocation required.'}
  $r=@{invocationOriginal=$o;policyAssociationOriginal=$o.policyAssociationOriginal;
    eligibleObserved=$false;returned=$false;exceptionOriginal=$null;terminal=$false;
    qualificationPremise='GENUINE_EXTERNAL_WHOLE_INPUT_AND_NATIVE_PHYSICAL_PROFILE_ADMISSION_REQUIRED'}
  $o.usefulCompletionOriginal=$r
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies publisher result publication.'}
    $a=$r.policyAssociationOriginal
    $eligible=($null -eq $o.firstFailureOriginal -and $o.closureReturned -and
      $null -ne $a -and $a.returned -and $a.constructionTerminal -and
      $null -eq $a.exceptionOriginal -and $a.localCorrespondenceComplete -and
      $null -ne $a.relationOriginals -and
      [object]::ReferenceEquals($a.preparationOriginal,$p))
    foreach($name in @('prepare','storage','module','initialize','verify','capture','close','dispose','release','closure')){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies publisher stage result join.'}
      $stage=$o.stagesOriginal[$name]
      if($null -eq $stage -or -not $stage.attempted -or -not $stage.returned -or
          $null -ne $stage.exceptionOriginal){$eligible=$false}
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies useful publisher completion.'}
    $r.eligibleObserved=$eligible
    if($eligible){
      $o.usefulReturned=$true;$c.publisherNativeOriginal.usefulReturned=$true
      $o.state='SUPPORTED_LOCAL_PUBLISHER_RESULT_UNDER_EXTERNALLY_QUALIFIED_INPUT'
    }else{$o.state='RETAINED_NONUSEFUL_PUBLISHER_RESULT'}
    $r.returned=$true;return $r
  }catch{
    $r.exceptionOriginal=$_
    if($null -eq $o.firstFailureOriginal){$o.firstFailureOriginal=$_}
    return $r
  }finally{$r.terminal=$true}
}
