# Fixed loaded observations. Decoded reservations are source geometry; genuine
# complete containing startup/operation qualification remains a precondition.
function New-Cli39LoadedObservationBook32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$recipes=$p.loadedRecipesOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $root.ContainsKey('loadedObservationOriginal') -or -not $recipes.returned -or
      -not [object]::ReferenceEquals($recipes.hostOwnerOriginal,$p) -or
      -not $root.rootModuleBookOriginal.returned -or -not $root.nativeQueryOriginal.complete -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME live once-only loaded observation originals required.'}
  $b=@{hostOwnerOriginal=$p;rootOriginal=$root;recipesOriginal=$recipes;queryOriginal=$root.nativeQueryOriginal;
    abiOriginal=$root.abiOriginal;snapshotOriginal=$root.rootModuleBookOriginal;profileOriginal=$null;
    workersOriginal=$null;modulesOriginal=$null;chargeBytes=0D;workerCount=0;backingBytes=0D;
    constructorTerminal=$false;constructorFailure=$null;returned=$false;exceptionOriginal=$null;
    disposalOriginal=$null;systemOriginal=$null;pageBytesOriginal=0;pendingWorkerOriginal=$null;
    workerConstructionUnknown=$false;state='RAW_LOADED_OBSERVATIONS_NOT_ADMISSION'}
  $root.loadedObservationOriginal=$b
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids loaded book construction.'}
    $role=if($root.rootRoleOriginal -ceq 'operation-host'){'hostBudget'}else{'callerBudget'}
    foreach($profile in $root.profileOriginals){if($profile.role -ceq $role -and $profile.returned){
      if($null -ne $b.profileOriginal){throw 'Duplicate loaded profile original.'};$b.profileOriginal=$profile
    }}
    if($null -eq $b.profileOriginal){throw 'Actual typed role profile required.'}
    $b.chargeBytes=[decimal]$b.profileOriginal.valuesOriginal.cliLoadedBookChargeBytes
    if($b.chargeBytes -ge $b.profileOriginal.valuesOriginal.cliLoadedReservationBytes){throw 'Loaded book leaves no worker capacity.'}
    $b.workersOriginal=[Collections.Generic.List[object]]::new()
    $b.modulesOriginal=[Collections.Generic.List[object]]::new()
    return $b
  }catch{$b.constructorFailure=$_;throw}finally{$b.constructorTerminal=$true}
}

function New-Cli39LoadedWorker32 {
  param([Parameter(Mandatory)]$BookOriginal,
    [Parameter(Mandatory)][ValidateSet('GetModuleFileNameW:export','GetModuleFileNameW:delegate',
      'GetSystemInfo:export','GetSystemInfo:delegate','system','path','region')][string]$Phase,
    [Parameter(Mandatory)][int]$BackingBytes,[IntPtr]$Argument=[IntPtr]::Zero,
    [uint32]$CharacterCapacity=0,$BindingOriginal=$null,$ModuleOriginal=$null)
  $b=$BookOriginal;$p=$b.hostOwnerOriginal;$root=$p.inputRead.value;$v=$b.profileOriginal.valuesOriginal
  if(-not [object]::ReferenceEquals($root.loadedObservationOriginal,$b) -or
      -not $b.constructorTerminal -or $null -ne $b.constructorFailure -or $null -ne $b.disposalOriginal -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $BackingBytes -lt 0){throw 'SAME live loaded worker enrollment required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline forbids loaded worker construction.'}
  [decimal]$charge=[decimal]$v.cliLoadedPerWorkerChargeBytes+[decimal]$BackingBytes*[decimal]$v.cliLoadedPerByteChargeBytes
  [decimal]$next=$b.chargeBytes+$charge
  if($null -ne $ModuleOriginal){
    if(-not $b.modulesOriginal.Contains($ModuleOriginal) -or
        -not [object]::ReferenceEquals($ModuleOriginal.bookOriginal,$b) -or
        $Phase -cnotin @('path','region') -or $ModuleOriginal.remainingChargeBytes -lt $charge -or
        ($Phase -ceq 'path' -and $ModuleOriginal.pathEnrollmentAttempted) -or
        ($Phase -ceq 'region' -and $ModuleOriginal.regionWorkersEnrolled -ge $ModuleOriginal.geometryOriginal.maximumRegionRows)){
      throw 'SAME fixed module precharged path/region roster required.'
    }
    $ModuleOriginal.remainingChargeBytes-=$charge
    if($Phase -ceq 'path'){$ModuleOriginal.pathEnrollmentAttempted=$true}else{$ModuleOriginal.regionWorkersEnrolled++}
  }else{
    if($next -ge $v.cliLoadedReservationBytes){throw 'Actual loaded worker/backing exceeds SAME decoded subdivision.'}
    $b.chargeBytes=$next
  }
  # Precharge before record/list growth as well as capacity-dependent buffers.
  $b.workerCount++;$b.backingBytes+=$BackingBytes;$b.workerConstructionUnknown=$true
  $w=@{bookOriginal=$b;phaseOriginal=$Phase;recipeOriginal=$null;bindingOriginal=$BindingOriginal;
    backingOriginal=$null;pinOriginal=$null;argumentOriginal=$null;capacityOriginal=$null;
    pointersOriginal=$null;delegatesOriginal=$null;resultsOriginal=$null;markersOriginal=$null;
    exceptionsOriginal=$null;contextOriginal=$null;threadOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinException=$null;rawReturned=$false;deadlineAtCompletion=$false;chargeBytes=$charge}
  $b.pendingWorkerOriginal=$w
  $b.workersOriginal.Add($w)
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids loaded worker construction.'}
    $w.exceptionsOriginal=[object[]]::new(2)
    if($Phase -clike '*:*'){
      if($BackingBytes -ne 0 -or $Argument -ne [IntPtr]::Zero -or $CharacterCapacity -ne 0){throw 'Binding has no supplied query arguments.'}
      $w.recipeOriginal=$b.recipesOriginal.recordsOriginal[$Phase]
      $w.pointersOriginal=[IntPtr[]]::new(2);$w.delegatesOriginal=[object[]]::new(1);$w.markersOriginal=[int[]]::new(3)
      $w.pointersOriginal[0]=$b.abiOriginal.originalOwner.module
      if($Phase -clike '*:delegate'){
        if($null -eq $BindingOriginal -or -not $BindingOriginal.rawReturned -or
            -not [object]::ReferenceEquals($BindingOriginal.bookOriginal,$b) -or
            $BindingOriginal.phaseOriginal -cne $Phase.Replace(':delegate',':export')){throw 'SAME actual preceding export required.'}
        $w.pointersOriginal[1]=$BindingOriginal.pointersOriginal[1]
      }elseif($null -ne $BindingOriginal){throw 'Export cannot borrow unrelated binding.'}
      $w.contextOriginal=[object[]]@($w.pointersOriginal,$w.delegatesOriginal,
        $w.recipeOriginal.typeOriginal.typeOriginal,$w.markersOriginal,$w.exceptionsOriginal)
    }else{
      $w.recipeOriginal=$b.recipesOriginal.recordsOriginal['call:'+$Phase]
      if(($Phase -ceq 'system' -and $BackingBytes -ne 48) -or
          ($Phase -ceq 'region' -and $BackingBytes -ne 48) -or
          ($Phase -ceq 'path' -and (2D*$CharacterCapacity -ne $BackingBytes -or $CharacterCapacity -eq 0))){throw 'Exact fixed call backing geometry required.'}
      $delegate=$null
      if($Phase -ceq 'region'){
        if($null -ne $BindingOriginal -or
            -not [object]::ReferenceEquals($b.queryOriginal.delegateTypesOriginal[12],$w.recipeOriginal.typeOriginal.typeOriginal)){
          throw 'SAME preowned own13 VirtualQuery type required.'}
        $delegate=$b.queryOriginal.delegatesOriginal[12]
      }else{
        $expected=if($Phase -ceq 'system'){'GetSystemInfo:delegate'}else{'GetModuleFileNameW:delegate'}
        if($null -eq $BindingOriginal -or -not $BindingOriginal.rawReturned -or
            -not [object]::ReferenceEquals($BindingOriginal.bookOriginal,$b) -or $BindingOriginal.phaseOriginal -cne $expected){throw 'SAME completed fixed delegate binding required.'}
        $delegate=$BindingOriginal.delegatesOriginal[0]
      }
      if($null -eq $delegate -or $delegate.GetType() -ne $w.recipeOriginal.typeOriginal.typeOriginal){throw 'Actual fixed typed delegate required.'}
      $w.backingOriginal=[byte[]]::new($BackingBytes);$w.pinOriginal=[Runtime.InteropServices.GCHandle[]]::new(1)
      $w.argumentOriginal=[IntPtr[]]@($Argument);$w.capacityOriginal=[uint32[]]@($CharacterCapacity)
      $w.resultsOriginal=[uint64[]]::new(2);$w.markersOriginal=[int[]]::new(8)
      $w.contextOriginal=[object[]]@($delegate,$w.backingOriginal,$w.pinOriginal,$w.argumentOriginal,
        $w.capacityOriginal,$w.resultsOriginal,$w.markersOriginal,$w.exceptionsOriginal)
    }
    if(-not $w.recipeOriginal.returned -or -not $w.recipeOriginal.constructionTerminal){throw 'Actual completed startup call recipe required.'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids loaded Thread constructor.'}
    $w.threadOriginal=[Threading.Thread]::new($w.recipeOriginal.bodyOriginal,[int]$v.cliLoadedWorkerStackBytes)
    return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true;$b.workerConstructionUnknown=$false}
}

function Test-Cli39LoadedWorkerClosure32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal
  if(-not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $null -ne $w.joinException -or $w.threadOriginal.IsAlive){return $false}
  if($w.phaseOriginal -clike '*:*'){return ($w.markersOriginal[2] -eq 1)}
  if($w.markersOriginal[7] -ne 1 -or ($w.markersOriginal[2] -eq 1 -and $w.markersOriginal[3] -ne 1) -or
      ($w.markersOriginal[0] -eq 1 -and $w.markersOriginal[1] -ne 1) -or
      $w.pinOriginal[0].IsAllocated -or ($w.markersOriginal[5] -eq 1 -and $w.markersOriginal[6] -ne 1)){return $false}
  return $true
}

function Invoke-Cli39LoadedWorker32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$b=$w.bookOriginal;$p=$b.hostOwnerOriginal
  if(-not $b.workersOriginal.Contains($w) -or -not $w.constructorTerminal -or
      $null -ne $w.constructorFailure -or $w.startAttempted -or $null -ne $b.disposalOriginal -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME complete once-only loaded worker required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline forbids loaded worker Start.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    try{$w.terminalJoined=$w.threadOriginal.Join(0)}catch{$w.joinException=$_;throw}
    if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
  }
  if(-not (Test-Cli39LoadedWorkerClosure32 $w)){throw 'Actual loaded worker physical closure incomplete.'}
  foreach($failure in $w.exceptionsOriginal){if($null -ne $failure){throw $failure}}
  if($w.phaseOriginal -clike '*:*'){
    if($w.markersOriginal[1] -ne 1){throw 'Actual loaded binding did not return.'}
  }elseif($w.markersOriginal[3] -ne 1 -or ($w.phaseOriginal -cne 'system' -and $w.markersOriginal[4] -ne 1)){
    throw 'Actual loaded native call/diagnostic incomplete.'
  }
  $w.rawReturned=$true
  Observe-OwnedPrerequisiteDeadline $p
  $w.deadlineAtCompletion=$p.deadlineReported
  if($w.deadlineAtCompletion){throw 'Actual loaded worker returned after original deadline; useful progression denied.'}
  return $w
}

function Test-Cli39LoadedObservationClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('loadedObservationOriginal')){return $true}
  $b=$root.loadedObservationOriginal
  if(-not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or -not $b.constructorTerminal -or
      $b.workerConstructionUnknown -or $b.moduleConstructionUnknown){return $false}
  if($null -ne $b.modulesOriginal){foreach($module in $b.modulesOriginal){if(-not $module.constructorTerminal){return $false}}}
  if($null -ne $b.workersOriginal){foreach($w in $b.workersOriginal){if(-not (Test-Cli39LoadedWorkerClosure32 $w)){return $false}}}
  return $true
}
