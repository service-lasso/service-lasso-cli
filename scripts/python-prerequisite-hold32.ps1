# Fixed output holds borrow existing source-selected pre-NPH CLI recipes.
# There is no supplied path/signature/callback or competing native owner.
function New-Cli39PythonOutputHold32 {
  param([Parameter(Mandatory)]$BookOriginal,$DirectoryOriginal=$null,$MemberOriginal=$null,$FixedOutputOriginal=$null)
  $b=$BookOriginal;$p=$b.preparationOriginal;$c=$p.consumerOriginal;$d=$DirectoryOriginal;$row=$MemberOriginal;$fixed=$FixedOutputOriginal
  $sourcesPresent=0;foreach($candidate in @($d,$row,$fixed)){if($null -ne $candidate){$sourcesPresent++}}
  if(-not [object]::ReferenceEquals($c.pythonTreeOriginal,$b) -or
      $null -eq $b.descriptorConstructionOriginal -or -not $b.descriptorConstructionOriginal.returned -or
      $c.ContainsKey('disposalOriginal') -or $null -ne $b.disposalOriginal -or
      $sourcesPresent -ne 1 -or $null -eq $b.holdsOriginal -or
      $b.holdsOriginal.Count -ge $b.maximumHolds){throw 'Actual fixed output hold source roster required.'}
  if($null -ne $d){
    if(-not $b.directoriesOriginal.Contains($d) -or -not [object]::ReferenceEquals($d.bookOriginal,$b) -or
        $d.ContainsKey('holdOriginal')){throw 'Actual once-held output directory required.'}
    $source=$d;$kind='35';$pathUnits=$d.pathOriginal.Length
  }elseif($null -ne $row){
    if(-not $b.descriptorConstructionOriginal.membersOriginal.Contains($row)){
      throw 'Actual retained member row required.'
    }
    $source=$row;$kind=$row.propertiesOriginal['kind'].childOriginal.textOriginal
    if($kind -ceq '35'){throw 'Archive directories use their actual directory hold original.'}
    $pathUnits=$c.paths.outputRoot.Length+6+$row.propertiesOriginal['path'].childOriginal.textOriginal.Length
  }else{
    if($fixed.kind -cne 'fixed-output' -or $fixed.nameOriginal -cnotin
        @('plan-original.json','decompression.json','members.json','complete.json','original.tar') -or
        -not [object]::ReferenceEquals($b.descriptorsOriginal[0].expectedNamesOriginal[$fixed.nameOriginal],$fixed)){
      throw 'Actual source-selected fixed output file witness required.'
    }
    $source=$fixed;$kind='30';$pathUnits=$c.paths.outputRoot.Length+1+$fixed.nameOriginal.Length
  }
  foreach($prior in $b.holdsOriginal){if([object]::ReferenceEquals($prior.sourceOriginal,$source)){throw 'Output hold cannot retry.'}}
  $h=@{bookOriginal=$b;sourceOriginal=$source;directoryOriginal=$d;memberOriginal=$row;fixedOutputOriginal=$fixed;kindOriginal=$kind;
    pathOriginal=$null;pointerCellsOriginal=$null;buffersOriginal=$null;workersOriginal=$null;
    chargeBytes=[decimal]0;constructorTerminal=$false;constructorFailure=$null;rosterReturned=$false;
    disposalOriginal=$null;closeEligible=$false;physicalCloseObserved=$false;noHandleObserved=$false;
    acquisitionUnknown=$false;metadataReturned=$false;attributesOriginal=0U;reparseTagOriginal=0U}
  $b.holdsOriginal.Add($h);if($null -ne $d){$d.holdOriginal=$h}
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    $v=$b.budgetOriginal
    $h.chargeBytes=[decimal]$v.pythonTreeCapturePerMemberChargeBytes+
      2*([decimal]$pathUnits+1)*[decimal]$v.pythonTreeCapturePerByteChargeBytes
    if($b.usedChargeBytes+$h.chargeBytes -gt $b.plannedChargeBytes){throw 'Output hold overlap exceeds SAME planned reservation.'}
    $b.usedChargeBytes+=$h.chargeBytes
    if($null -ne $d){$h.pathOriginal=$d.pathOriginal}
    elseif($null -ne $row){$h.pathOriginal=$c.paths.outputRoot+'\tree\'+$row.propertiesOriginal['path'].childOriginal.textOriginal.Replace('/','\')}
    else{$h.pathOriginal=$c.paths.outputRoot+'\'+$fixed.nameOriginal}
    $h.pointerCellsOriginal=[IntPtr[]]::new(2)
    $h.pointerCellsOriginal[1]=[IntPtr]::new(-1)
    $h.buffersOriginal=[object[]]@([byte[]]::new(8),[byte[]]::new(24))
    $h.workersOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach($phase in @('open','metadata','close')){$null=New-Cli39PythonOutputHoldWorker32 $h $phase}
    if($kind -ceq '32'){$null=New-Cli39PythonReparseWorker32 $h}
    $h.rosterReturned=$true;return $h
  }catch{$h.constructorFailure=$_;throw}finally{$h.constructorTerminal=$true}
}

function New-Cli39PythonOutputHoldWorker32 {
  param([Parameter(Mandatory)]$HoldOriginal,
    [Parameter(Mandatory)][ValidateSet('open','metadata','close')][string]$Phase)
  $h=$HoldOriginal;$b=$h.bookOriginal;$p=$b.preparationOriginal
  if(-not [object]::ReferenceEquals($p.consumerOriginal.pythonTreeOriginal,$b) -or
      -not $b.holdsOriginal.Contains($h) -or $h.rosterReturned -or $h.workersOriginal.ContainsKey($Phase)){
    throw 'Actual preowned output hold phase required.'
  }
  $w=@{holdOriginal=$h;phaseOriginal=$Phase;recipeOriginal=$null;delegateOriginal=$null;
    markersOriginal=$null;pinsOriginal=$null;boolOriginal=$null;errorsOriginal=$null;exceptionsOriginal=$null;
    contextOriginal=$null;threadOriginal=$null;constructorTerminal=$false;constructorFailure=$null;
    startAttempted=$false;startReturned=$false;startException=$null;terminalJoined=$false;joinException=$null;
    pendingOriginal=@{attempted=$false;returned=$false;value=$false;exceptionOriginal=$null};
    joinCount=0L;joinCountUnknown=$false;rawReturned=$false}
  $h.workersOriginal.Add($Phase,$w)
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    $hostOwner=$b.queryOriginal.hostOwnerOriginal;$abi=$b.queryOriginal.abiOriginal
    $recipe=$hostOwner.namespaceRecipesOriginal.recordsOriginal[$Phase];$w.recipeOriginal=$recipe
    $name=switch -CaseSensitive($Phase){'open'{'CreateFileW'}'metadata'{'GetFileInformationByHandleEx'}'close'{'CloseHandle'}}
    $w.delegateOriginal=$abi.delegates[$name]
    if(-not $recipe.returned -or -not $recipe.constructionTerminal -or
        -not [object]::ReferenceEquals($recipe.bookOriginal.hostOwnerOriginal,$hostOwner) -or
        $null -eq $w.delegateOriginal -or $w.delegateOriginal.GetType() -ne $recipe.typeOriginal.type -or
        $p.consumerOriginal.geometry.workerStackBytes -le 0 -or $p.consumerOriginal.geometry.workerStackBytes -gt [int]::MaxValue){
      throw 'SAME completed literal pre-NPH recipe/delegate and worker geometry required.'
    }
    $w.markersOriginal=[int[]]::new(20);$w.pinsOriginal=[Runtime.InteropServices.GCHandle[]]::new(2)
    $w.boolOriginal=[int[]]::new(2);$w.errorsOriginal=[int[]]::new(2);$w.exceptionsOriginal=[object[]]::new(3)
    $w.contextOriginal=[object[]]@($h.pathOriginal,$h.buffersOriginal,$w.delegateOriginal,$w.exceptionsOriginal,
      $w.markersOriginal,$h.pointerCellsOriginal,$w.pinsOriginal,$w.boolOriginal,$w.errorsOriginal)
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$recipe.bodyOriginal,
      [int]$p.consumerOriginal.geometry.workerStackBytes)
    $w.threadOriginal.IsBackground=$false;return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}

function Test-Cli39PythonOutputHoldWorkerClosure32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$h=$w.holdOriginal;$b=$h.bookOriginal
  if(-not [object]::ReferenceEquals($b.preparationOriginal.consumerOriginal.pythonTreeOriginal,$b) -or
      -not $b.holdsOriginal.Contains($h) -or
      -not [object]::ReferenceEquals($h.workersOriginal[$w.phaseOriginal],$w) -or -not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
      $null -ne $w.joinException -or $w.joinCountUnknown -or -not $w.pendingOriginal.returned -or
      -not $w.pendingOriginal.value -or $w.markersOriginal[18] -ne 1 -or $w.markersOriginal[19] -ne 1){return $false}
  $m=$w.markersOriginal
  switch -CaseSensitive($w.phaseOriginal){
    'open'{
      if(($m[0] -eq 1 -and $m[1] -ne 1) -or ($m[2] -eq 1 -and $m[3] -ne 1) -or
          ($m[1] -eq 1 -and ($m[5] -ne 1 -or $m[6] -ne 1))){return $false}
    }
    'metadata'{
      foreach($i in @(0,1)){
        $o=8*$i
        if(($m[$o] -eq 1 -and $m[$o+1] -ne 1) -or ($m[$o+3] -eq 1 -and $m[$o+4] -ne 1) -or
            ($m[$o+4] -eq 1 -and ($m[$o+5] -ne 1 -or $m[$o+6] -ne 1 -or $w.pinsOriginal[$i].IsAllocated))){return $false}
      }
    }
    'close'{if($m[0] -eq 1 -and $m[1] -ne 1){return $false}}
    default{return $false}
  }
  return $true
}

function Invoke-Cli39PythonOutputHoldWorker32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$h=$w.holdOriginal;$b=$h.bookOriginal;$p=$b.preparationOriginal;$c=$p.consumerOriginal
  if(-not [object]::ReferenceEquals($c.pythonTreeOriginal,$b) -or -not $b.holdsOriginal.Contains($h) -or
      -not [object]::ReferenceEquals($h.workersOriginal[$w.phaseOriginal],$w) -or
      -not $h.rosterReturned -or -not $h.constructorTerminal -or $null -ne $h.constructorFailure -or
      -not $w.constructorTerminal -or $null -ne $w.constructorFailure -or $w.startAttempted){
    throw 'Actual completed once-only output hold phase required.'
  }
  if($w.phaseOriginal -ceq 'close'){
    if($null -eq $h.disposalOriginal -or -not $h.closeEligible){throw 'Actual independently eligible hold close required.'}
  }else{
    Assert-Cli39PrerequisiteUseful26 $p
    if($c.ContainsKey('disposalOriginal') -or $null -ne $b.disposalOriginal -or $null -ne $h.disposalOriginal){
      throw 'Output hold future-start barrier closed.'
    }
    if($w.phaseOriginal -ceq 'metadata' -and
        (-not $h.workersOriginal['open'].rawReturned -or $h.pointerCellsOriginal[1] -eq [IntPtr]::new(-1))){
      throw 'Actual complete successful output open required before metadata.'
    }
  }
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.joinCount -eq [long]::MaxValue){$w.joinCountUnknown=$true;throw 'Output hold join transcript exhausted.'}
    $w.joinCount++;$w.pendingOriginal.attempted=$true;$w.pendingOriginal.returned=$false;$w.pendingOriginal.value=$false
    try{$w.pendingOriginal.value=$w.threadOriginal.Join(0);$w.pendingOriginal.returned=$true;$w.terminalJoined=$w.pendingOriginal.value}
    catch{$w.pendingOriginal.exceptionOriginal=$_;$w.joinException=$_;throw}
    if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
  }
  if($w.phaseOriginal -ceq 'open'){
    $h.acquisitionUnknown=($w.markersOriginal[2] -eq 1 -and $w.markersOriginal[3] -ne 1)
  }elseif($w.phaseOriginal -ceq 'close' -and $w.markersOriginal[1] -eq 1 -and $w.boolOriginal[0] -ne 0){
    $h.physicalCloseObserved=$true
  }
  if(-not (Test-Cli39PythonOutputHoldWorkerClosure32 $w)){throw 'Output hold worker borrow closure incomplete.'}
  foreach($failureOriginal in $w.exceptionsOriginal){if($null -ne $failureOriginal){throw $failureOriginal}}
  switch -CaseSensitive($w.phaseOriginal){
    'open'{if($w.markersOriginal[3] -ne 1 -or $w.markersOriginal[4] -ne 1 -or
        $h.pointerCellsOriginal[1] -eq [IntPtr]::new(-1)){throw 'Actual successful open observation required.'}}
    'metadata'{foreach($i in @(0,1)){if($w.markersOriginal[8*$i+1] -ne 1 -or
        $w.markersOriginal[8*$i+2] -ne 1 -or $w.boolOriginal[$i] -eq 0){throw 'Actual complete metadata observations required.'}}}
    'close'{if(-not $h.physicalCloseObserved -or $w.markersOriginal[2] -ne 1){throw 'Actual close success/diagnostic observation required.'}}
  }
  $w.rawReturned=$true;return $w
}

function Observe-Cli39PythonOutputHolds32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$p=$b.preparationOriginal
  if($b.ContainsKey('holdObservationOriginal')){throw 'Output hold observation is once-only.'}
  $o=@{bookOriginal=$b;returned=$false;exceptionOriginal=$null};$b.holdObservationOriginal=$o
  try{
    foreach($h in $b.holdsOriginal){
      $null=Invoke-Cli39PythonOutputHoldWorker32 $h.workersOriginal['open']
      $null=Invoke-Cli39PythonOutputHoldWorker32 $h.workersOriginal['metadata']
      $h.attributesOriginal=[BitConverter]::ToUInt32($h.buffersOriginal[0],0)
      $h.reparseTagOriginal=[BitConverter]::ToUInt32($h.buffersOriginal[0],4)
      $directory=($h.kindOriginal -ceq '35');$symlink=($h.kindOriginal -ceq '32')
      if($symlink){
        if(($h.attributesOriginal -band 0x400) -eq 0 -or $h.reparseTagOriginal -ne [uint32]2684354572){
          throw 'Actual symbolic-link reparse tag required.'
        }
      }elseif(($h.attributesOriginal -band 0x400) -ne 0 -or
          ((($h.attributesOriginal -band 0x10) -ne 0) -ne $directory)){
        throw 'Actual nonreparse output file/directory kind differs.'
      }
      $h.metadataReturned=$true
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Test-Cli39PythonOutputManagedClosure32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$d=$c.disposalOriginal
  if($null -eq $d -or $d.unknownAcquisition){return $false}
  if(-not (Test-Cli39PythonTreeFilesClosure32 $p)){return $false}
  foreach($worker in $c.managedPhases){
    if($worker.deadlineRefused -and -not $worker.startAttempted -and $worker.results[0] -eq 0 -and
        $null -ne $worker.thread -and -not $worker.thread.IsAlive){continue}
    if(-not $worker.startReturned -or -not $worker.terminalJoined -or $worker.thread.IsAlive -or
        -not $worker.completionAttempted -or $worker.pendingActive -or $worker.joinException.present -or
        $worker.multiplicityUnknown){return $false}
  }
  foreach($r in $d.resources){
    if(-not $r.disposalAttempted -or -not $r.disposalReturned -or $null -ne $r.disposalOriginal){return $false}
    if($r.objectOriginal -is [IO.FileStream] -and
        ($null -eq $r.handleOriginal -or -not $r.handleOriginal.IsClosed -or $r.objectOriginal.CanRead)){return $false}
  }
  return $true
}

function Dispose-Cli39PythonOutputHold32 {
  param([Parameter(Mandatory)]$HoldOriginal)
  $h=$HoldOriginal;$b=$h.bookOriginal;$p=$b.preparationOriginal
  if(-not [object]::ReferenceEquals($p.consumerOriginal.pythonTreeOriginal,$b) -or
      -not $b.holdsOriginal.Contains($h) -or $null -ne $h.disposalOriginal){throw 'Actual output hold close is once-only.'}
  $o=@{holdOriginal=$h;returned=$false;exceptionOriginal=$null;borrowersClosed=$false}
  $h.disposalOriginal=$o
  try{
    if(-not $h.constructorTerminal -or -not (Test-Cli39PythonOutputManagedClosure32 $p)){
      throw 'Actual output hold constructor/managed borrower closure incomplete.'
    }
    foreach($d in $b.directoriesOriginal){if(-not (Test-Cli39PythonDirectoryClosure32 $d)){throw 'Directory search still borrows output namespace.'}}
    if($null -eq $h.workersOriginal){
      # Only managed record/array construction preceded the missing worker map;
      # no body or Start could have acquired an external handle.
      $o.borrowersClosed=$true;$h.noHandleObserved=$true;$o.returned=$true;return $o
    }
    foreach($phase in @('open','metadata')){
      $worker=$h.workersOriginal[$phase]
      if($null -ne $worker -and -not (Test-Cli39PythonOutputHoldWorkerClosure32 $worker)){
        throw 'Actual output native hold borrower remains pending.'
      }
    }
    if(-not (Test-Cli39PythonReparseClosure32 $h)){throw 'Actual reparse borrower remains pending.'}
    $o.borrowersClosed=$true;$open=$h.workersOriginal['open']
    if($null -eq $open -or -not $open.startAttempted -or $open.markersOriginal[2] -ne 1){
      $h.noHandleObserved=$true;$o.returned=$true;return $o
    }
    if($open.markersOriginal[3] -ne 1){$h.acquisitionUnknown=$true;throw 'Actual output acquisition return unknown.'}
    $h.acquisitionUnknown=$false
    if($h.pointerCellsOriginal[1] -eq [IntPtr]::new(-1)){$h.noHandleObserved=$true;$o.returned=$true;return $o}
    $close=$h.workersOriginal['close']
    if($null -eq $close -or -not $close.constructorTerminal -or $null -ne $close.constructorFailure){
      throw 'Preowned actual output close constructor incomplete; handle retained.'
    }
    $h.closeEligible=$true;$null=Invoke-Cli39PythonOutputHoldWorker32 $close
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Test-Cli39PythonOutputHoldClosure32 {
  param([Parameter(Mandatory)]$HoldOriginal)
  $h=$HoldOriginal;$b=$h.bookOriginal
  if(-not [object]::ReferenceEquals($b.preparationOriginal.consumerOriginal.pythonTreeOriginal,$b) -or
      -not $b.holdsOriginal.Contains($h) -or -not $h.constructorTerminal -or
      $null -eq $h.disposalOriginal -or -not $h.disposalOriginal.borrowersClosed -or $h.acquisitionUnknown){return $false}
  if($null -eq $h.workersOriginal){return $h.noHandleObserved}
  if(-not (Test-Cli39PythonReparseClosure32 $h)){return $false}
  foreach($phase in @('open','metadata')){
    $worker=$h.workersOriginal[$phase]
    if($null -ne $worker -and -not (Test-Cli39PythonOutputHoldWorkerClosure32 $worker)){return $false}
  }
  if($h.noHandleObserved){return $true}
  return ($h.physicalCloseObserved -and $null -ne $h.workersOriginal['close'] -and
    (Test-Cli39PythonOutputHoldWorkerClosure32 $h.workersOriginal['close']))
}
