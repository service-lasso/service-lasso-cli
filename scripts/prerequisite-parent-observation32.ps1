# Fixed actual pipe-peer parent query book. Equality observations do not admit
# source, capability, namespace, controlling lifetime or authentic lineage.
function New-Cli39ParentObservation32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$peer=$root.rootPipePeerOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $root.rootRoleOriginal -cnotin @('retained-caller','operation-host') -or
      $root.ContainsKey('parentObservationOriginal') -or $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -eq $peer -or -not $peer.rawReturned -or -not (Test-Cli39RootPipePeerClosure32 $p) -or
      -not $root.parentLineageOriginal.returned -or
      $peer.clientPidObserved -ne $root.parentLineageOriginal.parentProcessIdObserved){throw 'Actual completed fixed peer/lineage locator required.'}
  $b=@{kind='cli39.fixed-parent-observation.v1';hostOwnerOriginal=$p;rootOriginal=$root;peerOriginal=$peer;
    lineageOriginal=$root.parentLineageOriginal;recipesOriginal=$p.parentRecipesOriginal;
    profileOriginal=$null;queryOriginal=$root.nativeQueryOriginal;moduleOriginal=$root.principalModuleOriginal;
    closeDelegateOriginal=$root.abiOriginal.delegates['CloseHandle'];moduleOwnerOriginal=$root.abiOriginal.originalOwner;
    workersOriginal=$null;recipeWorkersOriginal=$null;moduleCellsOriginal=$null;delegatesOriginal=$null;
    processHandleOriginal=$null;tokenHandleOriginal=$null;pidOriginal=$null;
    imageExpectedOriginal=$null;imageExpectedBytesOriginal=$null;imageCapacity=0;imageLengthOffset=0;imageBackingBytes=0;
    backingBytes=0L;chargeBytes=0D;constructorTerminal=$false;constructorFailure=$null;
    returned=$false;failureOriginal=$null;disposalOriginal=$null;disposalExceptionOriginal=$null;nativeParentAssociationOriginal=$null;authenticated=$false}
  $root.parentObservationOriginal=$b
  try{
    $role=if($root.rootRoleOriginal -ceq 'retained-caller'){'callerBudget'}else{'hostBudget'}
    foreach($profile in $root.profileOriginals){if($profile.role -ceq $role -and $profile.returned){$b.profileOriginal=$profile.valuesOriginal}}
    if($null -eq $b.profileOriginal -or -not $b.recipesOriginal.returned -or
        -not [object]::ReferenceEquals($b.recipesOriginal.hostOwnerOriginal,$p) -or
        -not $b.queryOriginal.complete -or -not $b.moduleOriginal.returned -or
        -not $b.moduleOwnerOriginal.constructionTerminal -or $b.moduleOwnerOriginal.module -eq [IntPtr]::Zero -or
        -not [object]::ReferenceEquals($b.closeDelegateOriginal.GetType(),$b.recipesOriginal.closeTypeOriginal.type)){
      throw 'SAME completed fixed recipe/resident modules/typed profile required.'
    }
    $b.imageExpectedOriginal=$root.fieldMapOriginal['hostPath'].GetString()
    [long]$capacity=[long]$b.imageExpectedOriginal.Length+1
    [long]$offset=(2*$capacity+3) -band (-4L)
    if($offset -gt [int]::MaxValue-4){throw 'Exact parent image rejection-witness extent cannot be represented.'}
    $b.imageCapacity=[int]$capacity;$b.imageLengthOffset=[int]$offset;$b.imageBackingBytes=[int]($offset+4)
    $b.backingBytes=32L+$b.imageBackingBytes+8L+88L+60L+2L*$b.imageExpectedOriginal.Length+44L
    $v=$b.profileOriginal
    $b.chargeBytes=[decimal]$v.cliParentBookChargeBytes+10*[decimal]$v.cliParentPerWorkerChargeBytes+
      [decimal]$b.backingBytes*[decimal]$v.cliParentPerByteChargeBytes
    if($b.chargeBytes -gt $v.cliParentReservationBytes -or $v.cliParentPerWorkerChargeBytes -le $v.cliParentWorkerStackBytes -or
        $v.cliParentWorkerStackBytes -le 0 -or $v.cliParentWorkerStackBytes -gt [int]::MaxValue -or
        $v.cliParentPerByteChargeBytes -le 0){throw 'Complete fixed parent backing/ten-worker charge cannot fit SAME subdivision.'}
    $b.imageExpectedBytesOriginal=[Text.Encoding]::Unicode.GetBytes($b.imageExpectedOriginal)
    $b.processHandleOriginal=[IntPtr[]]::new(1);$b.tokenHandleOriginal=[IntPtr[]]::new(1)
    $b.pidOriginal=[uint32[]]@($peer.clientPidObserved)
    $b.moduleCellsOriginal=[IntPtr[]]::new(2);$b.moduleCellsOriginal[0]=$b.moduleOwnerOriginal.module
    $b.delegatesOriginal=[object[]]::new(1)
    $b.workersOriginal=[Collections.Generic.List[object]]::new(6)
    $b.recipeWorkersOriginal=[Collections.Generic.List[object]]::new(4)
    foreach($phase in @('export','delegate','acquire','process-close')){$null=New-Cli39ParentRecipeWorker32 $b $phase}
    foreach($phase in @('times','image','token-open','token-user','token-statistics','token-close')){
      $null=New-Cli39OwnedBirthWorker32 $b $phase
    }
    return $b
  }catch{$b.constructorFailure=$_;throw}finally{$b.constructorTerminal=$true}
}

function New-Cli39ParentRecipeWorker32 {
  param([Parameter(Mandatory)]$BookOriginal,[Parameter(Mandatory)][ValidateSet('export','delegate','acquire','process-close')][string]$Phase)
  $b=$BookOriginal;$p=$b.hostOwnerOriginal
  if(-not [object]::ReferenceEquals($p.inputRead.value.parentObservationOriginal,$b) -or $b.constructorTerminal){throw 'SAME constructing fixed parent book required.'}
  foreach($prior in $b.recipeWorkersOriginal){if($prior.phaseOriginal -ceq $Phase){throw 'Parent recipe worker cannot repeat.'}}
  $w=@{bookOriginal=$b;phaseOriginal=$Phase;recipeOriginal=$null;contextOriginal=$null;threadOriginal=$null;
    markersOriginal=$null;resultsOriginal=$null;exceptionsOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinException=$null;joinFalseCount=0L;joinMultiplicityUnknown=$false;returned=$false}
  $b.recipeWorkersOriginal.Add($w)
  try{
    $r=$b.recipesOriginal.recordsOriginal[$Phase];$w.recipeOriginal=$r
    if(-not $r.returned -or -not $r.constructionTerminal -or $null -ne $r.exceptionOriginal){throw 'Actual fixed completed parent recipe required.'}
    $w.markersOriginal=[int[]]::new(6);$w.resultsOriginal=[int[]]::new(2);$w.exceptionsOriginal=[object[]]::new(1)
    $delegate=$null
    if($Phase -ceq 'process-close'){$delegate=$b.closeDelegateOriginal}
    $w.contextOriginal=[object[]]@($delegate,$b.recipesOriginal.typeOriginal.typeOriginal,$b.moduleCellsOriginal,
      $b.delegatesOriginal,$w.markersOriginal,$b.processHandleOriginal,$b.pidOriginal,$w.resultsOriginal,$w.exceptionsOriginal)
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$r.bodyOriginal,[int]$b.profileOriginal.cliParentWorkerStackBytes)
    $w.threadOriginal.IsBackground=$false;return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}

function Test-Cli39ParentRecipeBorrowClosed32 {
  param($WorkerOriginal)
  $w=$WorkerOriginal
  if($null -eq $w -or -not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
      $null -ne $w.joinException -or $w.joinMultiplicityUnknown -or $w.markersOriginal[5] -ne 1){return $false}
  if($w.phaseOriginal -cin @('acquire','process-close')){return ($w.markersOriginal[2] -ne 1 -or $w.markersOriginal[3] -eq 1)}
  return $true
}

function Test-Cli39ParentObservationClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('parentObservationOriginal')){return $true}
  $b=$root.parentObservationOriginal
  if(-not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or -not $b.constructorTerminal){return $false}
  $acquire=$null;$close=$null
  foreach($w in $b.recipeWorkersOriginal){
    if(-not (Test-Cli39ParentRecipeBorrowClosed32 $w)){return $false}
    if($w.phaseOriginal -ceq 'acquire'){$acquire=$w}
    if($w.phaseOriginal -ceq 'process-close'){$close=$w}
  }
  foreach($w in $b.workersOriginal){if(-not (Test-Cli39OwnedBirthWorkerClosure32 $w)){return $false}}
  if($null -ne $acquire -and $acquire.startAttempted){
    if($acquire.markersOriginal[3] -ne 1){return $false}
    if($b.processHandleOriginal[0] -ne [IntPtr]::Zero -and
        ($null -eq $close -or -not $close.startAttempted -or $close.markersOriginal[3] -ne 1 -or $close.resultsOriginal[0] -eq 0)){return $false}
  }
  $tokenOpen=$null;$tokenClose=$null
  foreach($w in $b.workersOriginal){if($w.roleOriginal -ceq 'token-open'){$tokenOpen=$w};if($w.roleOriginal -ceq 'token-close'){$tokenClose=$w}}
  if($null -ne $tokenOpen -and $tokenOpen.startAttempted){
    if($tokenOpen.markersOriginal[4] -ne 1){return $false}
    if($tokenOpen.resultsOriginal[0] -ne 0 -and ($tokenOpen.markersOriginal[8] -ne 1 -or
        $b.tokenHandleOriginal[0] -eq [IntPtr]::Zero -or $null -eq $tokenClose -or
        -not $tokenClose.startAttempted -or $tokenClose.markersOriginal[4] -ne 1 -or $tokenClose.resultsOriginal[0] -eq 0)){return $false}
  }
  return $true
}

function Invoke-Cli39ParentWorker32 {
  param([Parameter(Mandatory)]$BookOriginal,[Parameter(Mandatory)]$WorkerOriginal)
  $b=$BookOriginal;$w=$WorkerOriginal;$p=$b.hostOwnerOriginal
  $recipeWorker=$b.recipeWorkersOriginal.Contains($w);$queryWorker=$b.workersOriginal.Contains($w)
  if(-not [object]::ReferenceEquals($p.inputRead.value.parentObservationOriginal,$b) -or
      (-not $recipeWorker -and -not $queryWorker) -or -not $b.constructorTerminal -or
      $null -ne $b.constructorFailure -or -not $w.constructorTerminal -or $null -ne $w.constructorFailure -or
      $w.startAttempted){throw 'SAME complete once-only parent worker required.'}
  $closing=($recipeWorker -and $w.phaseOriginal -ceq 'process-close') -or
    ($queryWorker -and $w.roleOriginal -ceq 'token-close')
  if($closing){
    if($recipeWorker){
      $acquire=$null
      foreach($issued in $b.recipeWorkersOriginal){if($issued.phaseOriginal -ceq 'acquire'){$acquire=$issued}}
      if($null -eq $acquire -or -not $acquire.startAttempted -or
          -not (Test-Cli39ParentRecipeBorrowClosed32 $acquire) -or $acquire.markersOriginal[3] -ne 1 -or
          $b.processHandleOriginal[0] -eq [IntPtr]::Zero){throw 'Actual returned parent process acquisition required before once-close.'}
      foreach($borrower in $b.workersOriginal){if($borrower.roleOriginal -cin @('times','image','token-open') -and
          -not (Test-Cli39OwnedBirthTokenBorrowClosed32 $borrower)){throw 'Actual parent process borrower remains pending.'}}
    }else{
      $open=$null
      foreach($issued in $b.workersOriginal){if($issued.roleOriginal -ceq 'token-open'){$open=$issued}}
      if($null -eq $open -or -not $open.startAttempted -or $open.markersOriginal[4] -ne 1 -or
          $open.resultsOriginal[0] -eq 0 -or $open.markersOriginal[8] -ne 1 -or
          $b.tokenHandleOriginal[0] -eq [IntPtr]::Zero -or $b.tokenHandleOriginal[0] -eq [IntPtr]::new(-4)){
        throw 'Actual returned parent token acquisition required before once-close.'
      }
      foreach($borrower in $b.workersOriginal){if($borrower.roleOriginal -cne 'token-close' -and
          -not (Test-Cli39OwnedBirthTokenBorrowClosed32 $borrower)){throw 'Actual parent token borrower remains pending.'}}
    }
  }
  Observe-OwnedPrerequisiteDeadline $p
  if(-not $closing -and ($p.deadlineReported -or $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -ne $b.disposalOriginal)){throw 'Original parent observation future-start barrier closed.'}
  if($recipeWorker -and $w.phaseOriginal -ceq 'acquire'){
    $w.contextOriginal[0]=$b.delegatesOriginal[0]
    if($null -eq $w.contextOriginal[0] -or -not [object]::ReferenceEquals($w.contextOriginal[0].GetType(),$b.recipesOriginal.typeOriginal.typeOriginal)){
      throw 'Actual SAME fixed OpenProcess delegate required.'
    }
  }
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($null -ne $w.joinException -or $w.joinMultiplicityUnknown){throw 'Actual parent worker join unknown.'}
    try{$joined=$w.threadOriginal.Join(0)}catch{$w.joinException=$_;throw}
    if($joined){$w.terminalJoined=$true}else{
      if($w.joinFalseCount -eq [long]::MaxValue){$w.joinMultiplicityUnknown=$true;throw 'Actual parent join multiplicity unknown.'}
      $w.joinFalseCount++;$null=[Threading.Thread]::Yield()
    }
  }
  if($recipeWorker){
    if(-not (Test-Cli39ParentRecipeBorrowClosed32 $w)){throw 'Actual parent recipe borrower remains pending.'}
    foreach($failure in $w.exceptionsOriginal){if($null -ne $failure){throw $failure}}
    if($w.phaseOriginal -cin @('export','delegate')){
      if($w.markersOriginal[1] -ne 1){throw 'Fixed parent export/delegate did not complete.'}
    }elseif($w.markersOriginal[3] -ne 1 -or $w.markersOriginal[4] -ne 1 -or
        ($w.phaseOriginal -ceq 'acquire' -and $b.processHandleOriginal[0] -eq [IntPtr]::Zero) -or
        ($w.phaseOriginal -ceq 'process-close' -and $w.resultsOriginal[0] -eq 0)){
      throw 'Actual parent acquisition/close failed; raw result retained.'
    }
  }else{
    if(-not (Test-Cli39OwnedBirthWorkerClosure32 $w)){throw 'Actual parent query/pin resource closure incomplete.'}
    foreach($failure in $w.exceptionsOriginal){if($null -ne $failure){throw $failure}}
    if($w.markersOriginal[4] -ne 1 -or $w.markersOriginal[5] -ne 1 -or $w.resultsOriginal[0] -eq 0){throw 'Actual parent query/close failed; raw result retained.'}
  }
  $w.returned=$true
}

function Invoke-Cli39ParentDisposal32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$p=$b.hostOwnerOriginal
  if(-not [object]::ReferenceEquals($p.inputRead.value.parentObservationOriginal,$b) -or $null -ne $b.disposalOriginal){throw 'SAME once-only parent disposition required.'}
  $d=@{tokenAttempted=$false;tokenExceptionOriginal=$null;processAttempted=$false;processExceptionOriginal=$null;
    pendingTokenBorrow=$false;pendingProcessBorrow=$false;terminal=$false}
  $b.disposalOriginal=$d
  try{
    $open=$null;$tokenClose=$null;$acquire=$null;$processClose=$null
    foreach($w in $b.workersOriginal){if($w.roleOriginal -ceq 'token-open'){$open=$w};if($w.roleOriginal -ceq 'token-close'){$tokenClose=$w}}
    foreach($w in $b.recipeWorkersOriginal){if($w.phaseOriginal -ceq 'acquire'){$acquire=$w};if($w.phaseOriginal -ceq 'process-close'){$processClose=$w}}
    $tokenBorrowClosed=$true;$processBorrowClosed=$true
    foreach($w in $b.workersOriginal){
      if($w.roleOriginal -cne 'token-close' -and -not (Test-Cli39OwnedBirthTokenBorrowClosed32 $w)){$tokenBorrowClosed=$false}
      if($w.roleOriginal -cin @('times','image','token-open') -and -not (Test-Cli39OwnedBirthTokenBorrowClosed32 $w)){$processBorrowClosed=$false}
    }
    if($null -ne $acquire -and -not (Test-Cli39ParentRecipeBorrowClosed32 $acquire)){$processBorrowClosed=$false}
    if($null -ne $open -and $open.startAttempted -and $open.markersOriginal[4] -eq 1 -and
        $open.resultsOriginal[0] -ne 0 -and $open.markersOriginal[8] -eq 1 -and
        $b.tokenHandleOriginal[0] -ne [IntPtr]::Zero -and $b.tokenHandleOriginal[0] -ne [IntPtr]::new(-4)){
      if($tokenBorrowClosed -and $null -ne $tokenClose -and -not $tokenClose.startAttempted){
        $d.tokenAttempted=$true;try{$null=Invoke-Cli39ParentWorker32 $b $tokenClose}catch{$d.tokenExceptionOriginal=$_}
      }else{$d.pendingTokenBorrow=$true}
    }
    if($null -ne $acquire -and $acquire.startAttempted -and $acquire.markersOriginal[3] -eq 1 -and
        $b.processHandleOriginal[0] -ne [IntPtr]::Zero){
      if($processBorrowClosed -and $null -ne $processClose -and -not $processClose.startAttempted){
        $d.processAttempted=$true;try{$null=Invoke-Cli39ParentWorker32 $b $processClose}catch{$d.processExceptionOriginal=$_}
      }else{$d.pendingProcessBorrow=$true}
    }
  }finally{$d.terminal=$true}
}

function Observe-Cli39Parent32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$b=$null
  try{
    $b=New-Cli39ParentObservation32 $p
    foreach($w in $b.recipeWorkersOriginal){if($w.phaseOriginal -cne 'process-close'){$null=Invoke-Cli39ParentWorker32 $b $w}}
    foreach($w in $b.workersOriginal){
      if($w.roleOriginal -ceq 'token-close'){continue}
      $null=Invoke-Cli39ParentWorker32 $b $w
      switch -CaseSensitive($w.roleOriginal){
        'times'{
          $w.creationTimeObserved=[BitConverter]::ToUInt64($w.bufferOriginal,0)
          $w.exitTimeValidity='RAW_ONLY_EXIT_NOT_ESTABLISHED'
          if($w.creationTimeObserved -ne $b.lineageOriginal.parentCreationTimeObserved){throw 'Actual peer process birth differs from immutable parent lineage; PID reuse cannot pass.'}
        }
        'image'{
          $count=[BitConverter]::ToUInt32($w.bufferOriginal,$b.imageLengthOffset)
          if($count -ne $b.imageExpectedOriginal.Length -or $count -ge $b.imageCapacity){throw 'Actual parent image length differs or rejection witness exhausted.'}
          for($i=0;$i -lt $b.imageExpectedBytesOriginal.Length;$i++){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported -or $w.bufferOriginal[$i] -ne $b.imageExpectedBytesOriginal[$i]){throw 'Actual parent image correspondence incomplete/different.'}
          }
          $w.imageBytesEqual=$true;$w.heldImageAssociationOriginal=$null
        }
        'token-open'{
          if($w.markersOriginal[8] -ne 1 -or $b.tokenHandleOriginal[0] -eq [IntPtr]::Zero -or
              $b.tokenHandleOriginal[0] -eq [IntPtr]::new(-4)){throw 'Actual acquired parent query token output unknown.'}
        }
        'token-user'{
          $actual=[BitConverter]::ToUInt32($w.bufferOriginal,84)
          $w.sidOriginal=@{backingOriginal=$w.bufferOriginal;pointerObserved=[BitConverter]::ToInt64($w.bufferOriginal,0);
            offsetOriginal=$null;extentOriginal=$null;bytesEqual=$false}
          [decimal]$offset=[decimal]$w.sidOriginal.pointerObserved-[decimal]$w.pointerOriginal[0].ToInt64()
          if($actual -lt 16 -or $actual -gt 84 -or $offset -lt 16 -or $offset -gt ([long]$actual-8)){throw 'Actual parent SID alias outside retained output.'}
          $w.sidOriginal.offsetOriginal=[int]$offset
          $revision=$w.bufferOriginal[[int]$offset];$count=$w.bufferOriginal[[int]$offset+1]
          $extent=8+4*[int]$count;$w.sidOriginal.extentOriginal=$extent
          $hex=$b.lineageOriginal.valuesOriginal.parentSidHex
          if($revision -ne 1 -or $count -gt 15 -or $extent -gt 68 -or $offset+$extent -gt $actual -or $hex.Length -ne 2*$extent){throw 'Actual parent SID extent differs from immutable lineage.'}
          $digits='0123456789abcdef'
          for($i=0;$i -lt $extent;$i++){
            Observe-OwnedPrerequisiteDeadline $p
            $value=$w.bufferOriginal[[int]$offset+$i]
            if($p.deadlineReported -or $hex[2*$i] -cne $digits[$value -shr 4] -or $hex[2*$i+1] -cne $digits[$value -band 15]){throw 'Actual parent SID bytes differ/incomplete.'}
          }
          $w.sidOriginal.bytesEqual=$true
        }
        'token-statistics'{
          $actual=[BitConverter]::ToUInt32($w.bufferOriginal,56)
          $w.tokenIdObserved=[BitConverter]::ToInt64($w.bufferOriginal,0)
          $w.authenticationIdObserved=[BitConverter]::ToInt64($w.bufferOriginal,8)
          if($actual -ne 56 -or $w.tokenIdObserved -ne $b.lineageOriginal.parentTokenIdObserved -or
              $w.authenticationIdObserved -ne $b.lineageOriginal.parentAuthenticationIdObserved){throw 'Actual separately sampled parent token identifiers differ from immutable lineage.'}
          $w.atomicPrincipalAssociationOriginal=$null
        }
      }
    }
    $b.correspondenceReturned=$true
  }catch{
    if($null -eq $b){$b=$p.inputRead.value.parentObservationOriginal}
    if($null -ne $b){$b.failureOriginal=$_}
    throw
  }finally{
    if($null -ne $b -and $null -eq $b.disposalOriginal){
      try{$null=Invoke-Cli39ParentDisposal32 $b}catch{$b.disposalExceptionOriginal=$_}
    }
  }
  $disposition=$b.disposalOriginal
  if($null -ne $b.disposalExceptionOriginal -or $null -eq $disposition -or -not $disposition.terminal -or
      $null -ne $disposition.tokenExceptionOriginal -or $null -ne $disposition.processExceptionOriginal -or
      $disposition.pendingTokenBorrow -or $disposition.pendingProcessBorrow -or
      -not (Test-Cli39ParentObservationClosure32 $p)){throw 'Actual parent query disposition failed/incomplete; physical closure is separate from observation success.'}
  $b.returned=$true;return $b
}
