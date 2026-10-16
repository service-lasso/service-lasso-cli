# Closed CLI-only shared-owner interface; never an arbitrary effect dispatcher.
function Prepare-Cli39OwnedHostBirth32 {
  param([Parameter(Mandatory)]$PreparationOriginal,[Parameter(Mandatory)]$NativeOwnerOriginal)
  $r=$PreparationOriginal;$n=$NativeOwnerOriginal
  if($r.consumerOriginal.slot -cne 'cli39.source-input-host.v1' -or
      -not [object]::ReferenceEquals($r.nativeState,$n) -or
      -not [object]::ReferenceEquals($r.receiver.frames[$r.key],$r) -or $null -ne $n.cliBirthOriginal){throw 'SAME once-only fixed CLI birth preparation required.'}
  $b=@{preparationOriginal=$r;nativeOwnerOriginal=$n;hostOwnerOriginal=$null;rootOriginal=$null;
    profileOriginal=$null;queryOriginal=$null;moduleOriginal=$null;closeDelegateOriginal=$null;
    workersOriginal=$null;processHandleOriginal=$null;tokenHandleOriginal=$null;
    imageExpectedOriginal=$null;imageExpectedBytesOriginal=$null;imageCapacity=0;imageLengthOffset=0;imageBackingBytes=0;
    backingBytes=0L;chargeBytes=0D;nextPhaseOriginal=0;birthBound=$false;pidOriginal=$null;createOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;failureOriginal=$null;ready=$false;closeRequired=$false;
    imageComparedBytes=0;imageEqual=$false;observationsReturned=$false;
    parentTokenBookOriginal=$null;parentUserOriginal=$null;parentStatisticsOriginal=$null;
    inheritedSidComparedBytes=0;inheritedPrincipalReturned=$false;derivedTransportOriginal=$null}
  $n.cliBirthOriginal=$b
  try{
    $p=$n.abi.originalOwner.hostOwnerOriginal;$root=$p.inputRead.value
    $b.hostOwnerOriginal=$p;$b.rootOriginal=$root;$b.profileOriginal=$r.reservation.budget
    $b.queryOriginal=$root.nativeQueryOriginal;$b.moduleOriginal=$root.principalModuleOriginal
    if(-not [object]::ReferenceEquals($p.receiver,$r.receiver) -or
        -not [object]::ReferenceEquals($root.abiOriginal,$n.abi) -or
        -not $b.queryOriginal.complete -or -not $b.moduleOriginal.returned -or
        $r.originalDeadlineTicks -ne $p.originalDeadlineTicks -or
        $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal){throw 'SAME live root/ABI/query/module/deadline required before birth construction.'}
    $profileMatched=$false
    foreach($profile in $root.profileOriginals){
      if($profile.role -cin @('callerBudget','hostBudget') -and $profile.returned -and
          [object]::ReferenceEquals($profile.valuesOriginal,$b.profileOriginal)){$profileMatched=$true}
    }
    if(-not $profileMatched){throw 'Actual typed caller/host profile original required.'}
    $b.parentTokenBookOriginal=$root.principalModuleOriginal.tokenBookOriginal
    if($null -eq $b.parentTokenBookOriginal -or -not $b.parentTokenBookOriginal.returned -or
        $b.parentTokenBookOriginal.workersOriginal.Count -ne 2){throw 'Actual complete containing principal observations required before birth.'}
    $b.parentUserOriginal=$b.parentTokenBookOriginal.workersOriginal[0]
    $b.parentStatisticsOriginal=$b.parentTokenBookOriginal.workersOriginal[1]
    if(-not $b.parentUserOriginal.rawReturned -or -not $b.parentUserOriginal.decodedOriginal.returned -or
        -not $b.parentStatisticsOriginal.rawReturned -or -not $b.parentStatisticsOriginal.decodedOriginal.returned){
      throw 'Actual retained containing user/statistics captures required.'
    }
    $b.imageExpectedOriginal=$r.consumerOriginal.commandOriginal.file
    if([string]::IsNullOrEmpty($b.imageExpectedOriginal) -or $b.imageExpectedOriginal.Contains([char]0)){throw 'Exact source-bound executable required.'}
    [long]$capacity=[long]$b.imageExpectedOriginal.Length+1
    [long]$offset=(2*$capacity+3) -band (-4L)
    if($offset -gt [int]::MaxValue-4){throw 'Selected image extent cannot be represented.'}
    $b.imageCapacity=[int]$capacity;$b.imageLengthOffset=[int]$offset;$b.imageBackingBytes=[int]($offset+4)
    $b.backingBytes=32L+$b.imageBackingBytes+8L+88L+60L+2L*$b.imageExpectedOriginal.Length
    $v=$b.profileOriginal
    $b.chargeBytes=[decimal]$v.cliBirthBookChargeBytes+6*[decimal]$v.cliBirthPerWorkerChargeBytes+
      [decimal]$b.backingBytes*[decimal]$v.cliBirthPerByteChargeBytes
    if($v.cliBirthWorkerStackBytes -le 0 -or $v.cliBirthWorkerStackBytes -gt [int]::MaxValue -or
        $v.cliBirthPerWorkerChargeBytes -le $v.cliBirthWorkerStackBytes -or
        $v.cliBirthPerByteChargeBytes -le 0 -or $b.chargeBytes -gt $v.cliBirthReservationBytes -or
        $v.cliBirthReservationBytes -ge $v.managedOperationReservationBytes){throw 'Complete fixed birth backing/worker charge cannot fit SAME subdivision.'}
    # No numeric field authenticates this profile. Its genuine admission remains
    # the original reservation's obligation; this constructor retains identity.
    $b.imageExpectedBytesOriginal=[Text.Encoding]::Unicode.GetBytes($b.imageExpectedOriginal)
    $b.processHandleOriginal=[IntPtr[]]::new(1);$b.tokenHandleOriginal=[IntPtr[]]::new(1)
    $b.closeDelegateOriginal=$n.abi.delegates['CloseHandle']
    if($null -eq $b.closeDelegateOriginal){throw 'SAME resident typed CloseHandle required.'}
    $b.workersOriginal=[Collections.Generic.List[object]]::new(6)
    foreach($role in @('times','image','token-open','token-user','token-statistics','token-close')){
      Observe-OwnedPrerequisiteDeadline $r
      if($r.deadlineReported){throw 'Original deadline denies birth worker construction.'}
      $null=New-Cli39OwnedBirthWorker32 $b $role
    }
  }catch{$b.constructorFailure=$_;throw}finally{$b.constructorTerminal=$true}
  # Bare shared callsite: intentionally no success-stream object.
}

function Test-Cli39OwnedBirthWorkerClosure32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal
  if(-not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
      $null -ne $w.joinException -or $w.joinMultiplicityUnknown){return $false}
  $marks=$w.markersOriginal
  if($w.roleOriginal -ceq 'token-close'){return ($marks[3] -ne 1 -or $marks[4] -eq 1)}
  if($marks[1] -eq 1 -and $marks[2] -ne 1){return $false}
  if($marks[3] -eq 1 -and $marks[4] -ne 1){return $false}
  if($w.roleOriginal -ceq 'token-open' -and $marks[4] -eq 1 -and $marks[8] -ne 1){return $false}
  if($marks[2] -eq 1 -and ($marks[6] -ne 1 -or $marks[7] -ne 1 -or
      $w.pinsOriginal[0].IsAllocated -or $null -ne $w.exceptionsOriginal[1])){return $false}
  return $true
}

# Token handle borrowing ends with actual native-return and physical thread join.
# A retained failed GCHandle.Free is a separate resource obligation, not a
# continued token borrow; it still prevents overall closure/readiness.
function Test-Cli39OwnedBirthTokenBorrowClosed32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal
  if(-not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
      $null -ne $w.joinException -or $w.joinMultiplicityUnknown){return $false}
  if($w.markersOriginal[3] -eq 1 -and $w.markersOriginal[4] -ne 1){return $false}
  if($w.roleOriginal -ceq 'token-open' -and $w.markersOriginal[4] -eq 1 -and
      $w.resultsOriginal[0] -ne 0 -and $w.markersOriginal[8] -ne 1){return $false}
  return $true
}
function Advance-Cli39OwnedHostBirth32 {
  param([Parameter(Mandatory)]$PreparationOriginal,[Parameter(Mandatory)]$NativeOwnerOriginal)
  $r=$PreparationOriginal;$n=$NativeOwnerOriginal;$b=$n.cliBirthOriginal
  if($null -eq $b -or -not [object]::ReferenceEquals($b.preparationOriginal,$r) -or
      -not [object]::ReferenceEquals($b.nativeOwnerOriginal,$n) -or
      -not [object]::ReferenceEquals($r.nativeState,$n) -or -not $b.constructorTerminal -or
      $null -ne $b.constructorFailure){throw 'Actual completed fixed birth constructor required.'}
  if($b.ready){
    if($b.hostOwnerOriginal.consumerOriginal.ContainsKey('disposalOriginal') -or
        $null -ne $b.rootOriginal.namespaceBookOriginal.disposalOriginal){throw 'Original root future-start closure denies birth-ready grant.'}
    return $true
  }
  if(-not $b.birthBound){
    if($null -eq $n.processCreateOriginal -or -not $n.processCreateOriginal.returned -or
        -not $n.processCreateOriginal.value -or -not $n.assigned -or -not $n.resumed -or
        -not $n.resumeChronologyValid -or -not $n.cliPeerObserved -or
        $n.cliPeerOriginal.before.value -ne 258 -or $n.cliPeerOriginal.after.value -ne 258 -or
        $n.callerControl.peerPid -ne $n.pid -or $n.pid -eq 0 -or
        $n.handles.process -eq [IntPtr]::Zero -or $n.handles.process -eq [IntPtr]::new(-1)){
      throw 'Actual successful suspended birth/assignment/resume/live native peer required.'
    }
    $b.processHandleOriginal[0]=$n.handles.process;$b.pidOriginal=$n.pid
    $b.createOriginal=$n.processCreateOriginal;$b.birthBound=$true
  }
  if($b.processHandleOriginal[0] -ne $n.handles.process -or $b.pidOriginal -ne $n.pid -or
      -not [object]::ReferenceEquals($b.createOriginal,$n.processCreateOriginal)){throw 'Original native child changed during birth observation.'}
  Observe-OwnedPrerequisiteDeadline $r
  if($r.deadlineReported -and $null -eq $b.failureOriginal){$b.failureOriginal=$r.deadlineOriginal}
  if(($b.hostOwnerOriginal.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -ne $b.rootOriginal.namespaceBookOriginal.disposalOriginal) -and $null -eq $b.failureOriginal){
    $b.failureOriginal=@{phase='original-birth-future-start-closed';bookOriginal=$b}
  }
  # A known failure skips unissued useful queries, but independently closes an
  # actually acquired token after every issued borrower has physically joined.
  if($null -ne $b.failureOriginal -and $b.nextPhaseOriginal -lt 5 -and
      (-not $b.workersOriginal[$b.nextPhaseOriginal].startAttempted -or $b.workersOriginal[$b.nextPhaseOriginal].terminalJoined)){
    foreach($prior in $b.workersOriginal){if($prior.startAttempted -and -not (Test-Cli39OwnedBirthTokenBorrowClosed32 $prior)){return $false}}
    $open=$b.workersOriginal[2]
    if($open.startAttempted -and $open.markersOriginal[4] -eq 1 -and $open.resultsOriginal[0] -ne 0){
      if($open.markersOriginal[8] -ne 1 -or $b.tokenHandleOriginal[0] -eq [IntPtr]::Zero){return $false}
      $b.closeRequired=$true;$b.nextPhaseOriginal=5
    }else{throw $b.failureOriginal}
  }
  if($b.nextPhaseOriginal -ge 6){
    if($null -ne $b.failureOriginal){throw $b.failureOriginal}
    # Distinct child token ID/creation are retained; inheritance is SID/AuthId.
    # One bounded byte per Advance keeps native capture/control serviced.
    $parentBook=$b.rootOriginal.principalModuleOriginal.tokenBookOriginal
    $parentUser=$b.parentUserOriginal;$parentStatistics=$b.parentStatisticsOriginal
    $childUser=$b.workersOriginal[3];$childStatistics=$b.workersOriginal[4]
    if(-not [object]::ReferenceEquals($parentBook,$b.parentTokenBookOriginal) -or
        -not [object]::ReferenceEquals($parentBook.workersOriginal[0],$parentUser) -or
        -not [object]::ReferenceEquals($parentBook.workersOriginal[1],$parentStatistics) -or
        -not $childUser.sidOriginal.returned -or $null -eq $childStatistics.statisticsOriginal -or
        $childUser.sidOriginal.extentOriginal -ne $parentUser.decodedOriginal.sidExtentOriginal -or
        $childStatistics.statisticsOriginal.authenticationId -ne
          $parentStatistics.decodedOriginal.fieldsOriginal['authenticationIdObserved']){
      $b.failureOriginal=@{phase='inherited-principal-correspondence';bookOriginal=$b};throw $b.failureOriginal
    }
    $i=$b.inheritedSidComparedBytes
    if($i -lt $childUser.sidOriginal.extentOriginal){
      if($childUser.bufferOriginal[$childUser.sidOriginal.offsetOriginal+$i] -ne
          $parentUser.backingOriginal[$parentUser.decodedOriginal.sidOffsetOriginal+$i]){
        $b.failureOriginal=@{phase='inherited-sid-correspondence';bookOriginal=$b;byteOriginal=$i};throw $b.failureOriginal
      }
      $b.inheritedSidComparedBytes=$i+1
      if($b.inheritedSidComparedBytes -lt $childUser.sidOriginal.extentOriginal){return $false}
    }
    $null=New-Cli39OwnedDerivedTransport32 $b
    $b.inheritedPrincipalReturned=$true
    $b.observationsReturned=$true;$b.ready=$true;return $true
  }
  $w=$b.workersOriginal[$b.nextPhaseOriginal]
  if(-not $w.startAttempted){
    if($w.roleOriginal -ceq 'token-close'){
      foreach($borrower in $b.workersOriginal){if($borrower.roleOriginal -cne 'token-close' -and
          -not (Test-Cli39OwnedBirthTokenBorrowClosed32 $borrower)){return $false}}
      if(-not $b.closeRequired -or $b.tokenHandleOriginal[0] -eq [IntPtr]::Zero -or
          $b.tokenHandleOriginal[0] -eq [IntPtr]::new(-4)){throw 'Actual queried child token required for once-close.'}
    }elseif($r.deadlineReported){return $false}
    $w.startAttempted=$true
    try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
    return $false
  }
  if(-not $w.terminalJoined){
    if($null -ne $w.joinException -or $w.joinMultiplicityUnknown){return $false}
    $w.joinAttempted=$true;$w.joinReturned=$false
    try{$w.joinValue=$w.threadOriginal.Join(0);$w.joinReturned=$true}catch{$w.joinException=$_;throw}
    if(-not $w.joinValue){
      if($w.joinFalseCount -eq [long]::MaxValue){$w.joinMultiplicityUnknown=$true}else{$w.joinFalseCount++}
      return $false
    };$w.terminalJoined=$true
  }
  if(-not $w.completionAttempted){
    $w.completionAttempted=$true
    if(-not (Test-Cli39OwnedBirthTokenBorrowClosed32 $w)){throw 'Actual birth worker native borrow remains incomplete.'}
    if(-not (Test-Cli39OwnedBirthWorkerClosure32 $w) -and $null -eq $b.failureOriginal){$b.failureOriginal=$w}
    foreach($failure in $w.exceptionsOriginal){if($null -ne $failure -and $null -eq $b.failureOriginal){$b.failureOriginal=$failure}}
    if($w.markersOriginal[4] -ne 1 -or $w.markersOriginal[5] -ne 1 -or $w.resultsOriginal[0] -eq 0){
      if($null -eq $b.failureOriginal){$b.failureOriginal=$w}
    }
    if($w.roleOriginal -ceq 'token-open' -and $w.markersOriginal[4] -eq 1 -and $w.resultsOriginal[0] -ne 0){
      if($w.markersOriginal[8] -ne 1 -or $b.tokenHandleOriginal[0] -eq [IntPtr]::Zero -or
          $b.tokenHandleOriginal[0] -eq [IntPtr]::new(-4)){throw 'Acquired token output remains unsupported/unknown.'}
      $b.closeRequired=$true
    }
    if($w.roleOriginal -ceq 'times' -and $null -eq $b.failureOriginal){
      $w.timesOriginal=@{backingOriginal=$w.bufferOriginal;
        creationTime=[BitConverter]::ToUInt64($w.bufferOriginal,0);exitTime=[BitConverter]::ToUInt64($w.bufferOriginal,8);
        kernelTime=[BitConverter]::ToUInt64($w.bufferOriginal,16);userTime=[BitConverter]::ToUInt64($w.bufferOriginal,24)}
    }
    if($w.roleOriginal -ceq 'image' -and $null -eq $b.failureOriginal){
      $count=[BitConverter]::ToUInt32($w.bufferOriginal,$b.imageLengthOffset)
      if($count -ne $b.imageExpectedOriginal.Length -or $count -ge $b.imageCapacity){$b.failureOriginal=$w}
      else{
        # One source-selected byte per advance; raw capture/control remain
        # serviced between comparisons even for a long exact image path.
        $i=$b.imageComparedBytes
        if($i -lt $b.imageExpectedBytesOriginal.Length){
          if($w.bufferOriginal[$i] -ne $b.imageExpectedBytesOriginal[$i]){$b.failureOriginal=$w}
          $b.imageComparedBytes=$i+1
        }
        if($null -eq $b.failureOriginal -and $b.imageComparedBytes -lt $b.imageExpectedBytesOriginal.Length){
          $w.completionAttempted=$false;return $false
        }
        $b.imageEqual=($null -eq $b.failureOriginal -and $b.imageComparedBytes -eq $b.imageExpectedBytesOriginal.Length)
      }
    }
    if($w.roleOriginal -cin @('token-user','token-statistics') -and $null -eq $b.failureOriginal){
      $expected=if($w.roleOriginal -ceq 'token-user'){84}else{56}
      $actual=[BitConverter]::ToUInt32($w.bufferOriginal,$expected)
      if($actual -eq 0 -or $actual -gt $expected){$b.failureOriginal=$w}
      elseif($w.roleOriginal -ceq 'token-statistics'){
        if($actual -ne 56){$b.failureOriginal=$w}
        else{
          $w.statisticsOriginal=@{backingOriginal=$w.bufferOriginal;tokenId=[BitConverter]::ToInt64($w.bufferOriginal,0);
            authenticationId=[BitConverter]::ToInt64($w.bufferOriginal,8);expirationTime=[BitConverter]::ToInt64($w.bufferOriginal,16);
            tokenType=[BitConverter]::ToUInt32($w.bufferOriginal,24);impersonationLevel=[BitConverter]::ToUInt32($w.bufferOriginal,28);
            dynamicCharged=[BitConverter]::ToUInt32($w.bufferOriginal,32);dynamicAvailable=[BitConverter]::ToUInt32($w.bufferOriginal,36);
            groupCount=[BitConverter]::ToUInt32($w.bufferOriginal,40);privilegeCount=[BitConverter]::ToUInt32($w.bufferOriginal,44);
            modifiedId=[BitConverter]::ToInt64($w.bufferOriginal,48)}
        }
      }else{
        # Numeric alias into retained output only; never dereference freed pin.
        $w.sidOriginal=@{backingOriginal=$w.bufferOriginal;pointerObserved=$null;offsetOriginal=$null;extentOriginal=$null;returned=$false}
        if($actual -lt 16){$b.failureOriginal=$w}
        else{
          $w.sidOriginal.pointerObserved=[BitConverter]::ToInt64($w.bufferOriginal,0)
          [decimal]$offset=[decimal]$w.sidOriginal.pointerObserved-[decimal]$w.pointerOriginal[0].ToInt64()
          if($offset -lt 16 -or $offset -gt ([long]$actual-8)){$b.failureOriginal=$w}
          else{
            $w.sidOriginal.offsetOriginal=[int]$offset
            $revision=$w.bufferOriginal[[int]$offset];$count=$w.bufferOriginal[[int]$offset+1]
            $extent=8+4*[int]$count;$w.sidOriginal.extentOriginal=$extent
            if($revision -ne 1 -or $count -gt 15 -or $extent -gt 68 -or $offset+$extent -gt $actual){$b.failureOriginal=$w}
            else{$w.sidOriginal.returned=$true}
          }
        }
      }
    }
    $w.returned=($null -eq $b.failureOriginal);$b.nextPhaseOriginal++
  }
  return $false
}

function Test-Cli39OwnedHostBirthClosure32 {
  param([Parameter(Mandatory)]$PreparationOriginal,[Parameter(Mandatory)]$NativeOwnerOriginal)
  $r=$PreparationOriginal;$n=$NativeOwnerOriginal;$b=$n.cliBirthOriginal
  if($null -eq $b){return $true}
  if(-not [object]::ReferenceEquals($b.preparationOriginal,$r) -or
      -not [object]::ReferenceEquals($b.nativeOwnerOriginal,$n) -or -not $b.constructorTerminal){return $false}
  foreach($w in $b.workersOriginal){if(-not (Test-Cli39OwnedBirthWorkerClosure32 $w)){return $false}}
  if($null -eq $b.workersOriginal -or $b.workersOriginal.Count -lt 3){return $true}
  $open=$b.workersOriginal[2]
  if(-not $open.startAttempted -or $open.markersOriginal[3] -ne 1){return $true}
  if($open.markersOriginal[4] -ne 1){return $false}
  if($open.resultsOriginal[0] -eq 0){return $true}
  if($open.markersOriginal[8] -ne 1 -or $b.workersOriginal.Count -ne 6){return $false}
  $close=$b.workersOriginal[5]
  return ($close.startAttempted -and $close.terminalJoined -and $close.markersOriginal[4] -eq 1 -and $close.resultsOriginal[0] -ne 0)
}
function Test-Cli39AllOwnedHostBirthClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  foreach($frame in $p.receiver.frames.Values){
    if($null -ne $frame.consumerOriginal -and $frame.consumerOriginal.slot -ceq 'cli39.source-input-host.v1' -and
        $frame.ContainsKey('nativeState') -and $null -ne $frame.nativeState -and
        $null -ne $frame.nativeState.cliBirthOriginal){
      if(-not [object]::ReferenceEquals($frame.nativeState.cliBirthOriginal.hostOwnerOriginal,$p)){continue}
      if(-not (Test-Cli39OwnedHostBirthClosure32 $frame $frame.nativeState)){return $false}
    }
  }
  return $true
}
function New-Cli39OwnedDerivedTransport32 {
  param([Parameter(Mandatory)]$BirthOriginal)
  $birth=$BirthOriginal;$r=$birth.preparationOriginal;$n=$birth.nativeOwnerOriginal;$p=$birth.hostOwnerOriginal
  $derived=$r.consumerOriginal.inputRoot.derivedRootOriginal
  if($null -ne $birth.derivedTransportOriginal -or $null -eq $derived -or $null -ne $derived.parentTransportOriginal -or
      -not $birth.birthBound -or $birth.nextPhaseOriginal -ne 6 -or $null -ne $birth.failureOriginal -or
      -not $birth.imageEqual -or $birth.inheritedSidComparedBytes -ne $birth.workersOriginal[3].sidOriginal.extentOriginal -or
      $birth.workersOriginal[4].statisticsOriginal.authenticationId -ne
        $birth.parentStatisticsOriginal.decodedOriginal.fieldsOriginal['authenticationIdObserved']){
    throw 'SAME once-only completed actual derived birth required for transport.'
  }
  $a=@{birthOriginal=$birth;preparationOriginal=$r;nativeOwnerOriginal=$n;derivedRootOriginal=$derived;
    hostOwnerOriginal=$p;receiverOriginal=$p.receiver;deadlineTicks=$p.originalDeadlineTicks;
    writerOriginal=$derived.writeWorkerOriginal;subjectOriginal=$derived.heldSubjectOriginal;
    identityOriginal=$derived.nativeIdentityOriginal;immutableCustodyOriginal=$derived.immutableCustodyOriginal;
    admittedParentOriginal=$derived.admittedParentOriginal;projectionOriginal=$derived.projectionOriginal;
    nodePolicyProjectionOriginal=$derived.projectionOriginal.nodePolicyConstructionOriginal;
    publisherPolicyProjectionOriginal=$derived.projectionOriginal.publisherPolicyConstructionOriginal;
    commandOriginal=$r.consumerOriginal.commandOriginal;inputOriginal=$r.consumerOriginal.inputRoot;
    parentRootOriginal=$birth.rootOriginal;processCreateOriginal=$n.processCreateOriginal;
    peerOriginal=$n.cliPeerOriginal;childProcessHandleOriginal=$birth.processHandleOriginal;
    childTimesOriginal=$birth.workersOriginal[0].timesOriginal;childUserOriginal=$birth.workersOriginal[3].sidOriginal;
    childStatisticsOriginal=$birth.workersOriginal[4].statisticsOriginal;
    parentUserOriginal=$birth.parentUserOriginal;parentStatisticsOriginal=$birth.parentStatisticsOriginal;
    returned=$false;exceptionOriginal=$null;terminal=$false}
  $birth.derivedTransportOriginal=$a;$derived.parentTransportOriginal=$a
  try{
    if(-not [object]::ReferenceEquals($p.inputRead.value.derivedRootOriginal,$derived) -or
        -not [object]::ReferenceEquals($derived.hostOwnerOriginal,$p) -or
        -not [object]::ReferenceEquals($derived.admittedParentOriginal,$p.inputRead.value.admittedInputOriginal) -or
        -not [object]::ReferenceEquals($a.commandOriginal,$derived.childInvocationOriginal.commandOriginal) -or
        -not [object]::ReferenceEquals($r.consumerOriginal,$derived.childInvocationOriginal.consumerOriginal) -or
        -not [object]::ReferenceEquals($derived.childInvocationOriginal.preparationOriginal,$r) -or
        -not [object]::ReferenceEquals($r.nativeState,$n) -or -not [object]::ReferenceEquals($r.receiver,$p.receiver) -or
        $r.originalDeadlineTicks -ne $p.originalDeadlineTicks -or
        -not $derived.returned -or -not $a.writerOriginal.terminalJoined -or
        -not $a.identityOriginal.returned -or -not $a.immutableCustodyOriginal.sourceWritesClosed -or
        -not $a.subjectOriginal.eof -or $a.subjectOriginal.stream.SafeFileHandle.IsClosed -or
        $a.inputOriginal.rootPath -cne $a.subjectOriginal.path -or $a.inputOriginal.rootBytes -ne $a.subjectOriginal.bytesExpected -or
        $a.inputOriginal.rootHash -cne $a.subjectOriginal.sha256 -or $a.inputOriginal.role -cne $derived.childRoleOriginal -or
        $a.inputOriginal.action -cne $birth.rootOriginal.actionOriginal -or -not $a.projectionOriginal.returned -or
        -not $n.processCreateOriginal.returned -or -not $n.processCreateOriginal.value -or
        -not $n.assigned -or -not $n.resumed -or -not $n.resumeChronologyValid -or -not $n.cliPeerObserved -or
        $n.callerControl.peerPid -ne $birth.pidOriginal -or $n.pid -ne $birth.pidOriginal -or
        $n.handles.process -ne $birth.processHandleOriginal[0] -or
        -not [object]::ReferenceEquals($birth.createOriginal,$n.processCreateOriginal) -or
        -not (Test-Cli39OwnedHostBirthClosure32 $r $n)){
      throw 'Actual derived writer/native identity/command/parent/birth/peer transport originals differ.'
    }
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
