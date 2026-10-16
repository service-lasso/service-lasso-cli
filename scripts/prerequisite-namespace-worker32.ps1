# Fixed CLI native namespace workers, separate from managed read/hash roster.
function Get-Cli39NamespaceHold32 {
  param($HostOwnerOriginal,$HoldOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$h=$HoldOriginal
  if($null -eq $root -or $null -eq $root.namespaceBookOriginal -or $null -eq $h){throw 'Actual namespace hold required.'}
  $b=$root.namespaceBookOriginal
  if(-not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($b.rootOriginal,$root) -or
      -not [object]::ReferenceEquals($h.bookOriginal,$b) -or -not $b.holdsOriginal.Contains($h)){
    throw 'SAME enrolled fixed namespace hold required.'
  }
  return $h
}
function New-Cli39NamespaceWorker32 {
  param($HostOwnerOriginal,$HoldOriginal,[ValidateSet('open','metadata','close')][string]$Phase)
  $p=$HostOwnerOriginal;$h=Get-Cli39NamespaceHold32 $p $HoldOriginal;$b=$h.bookOriginal
  if($null -ne $h.workersOriginal[$Phase] -or $b.rosterReturned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'Once-only prebirth namespace worker required.'}
  $w=@{hostOwnerOriginal=$p;holdOriginal=$h;phaseOriginal=$Phase;recipeOriginal=$null;
    markersOriginal=$null;pinsOriginal=$null;boolOriginal=$null;errorsOriginal=$null;exceptionsOriginal=$null;
    contextOriginal=$null;threadOriginal=$null;constructorTerminal=$false;constructorFailure=$null;
    startAttempted=$false;startReturned=$false;startException=$null;terminalJoined=$false;
    pendingActive=$false;multiplicityUnknown=$false;joinException=$null;rawReturned=$false;
    pendingOriginal=@{attempted=$false;returned=$false;value=$null;exceptionOriginal=$null};
    repeatedFalseOriginal=@{returned=$false;repeats=0L}}
  $h.workersOriginal[$Phase]=$w;$b.workersOriginal.Add($w)
  try{
    $recipeBook=$p.namespaceRecipesOriginal;$r=$recipeBook.recordsOriginal[$Phase]
    $name=switch -CaseSensitive($Phase){'open'{'CreateFileW'}'metadata'{'GetFileInformationByHandleEx'}'close'{'CloseHandle'}}
    $delegate=$b.abiOriginal.delegates[$name]
    if(-not $recipeBook.returned -or -not $recipeBook.constructionTerminal -or
        -not [object]::ReferenceEquals($recipeBook.hostOwnerOriginal,$p) -or
        -not $r.returned -or -not $r.constructionTerminal -or $null -ne $r.exceptionOriginal -or
        -not [object]::ReferenceEquals($delegate.GetType(),$r.typeOriginal.type)){throw 'SAME completed fixed recipe/resident typed delegate required.'}
    $w.recipeOriginal=$r;$w.markersOriginal=[int[]]::new(20)
    $w.pinsOriginal=[Runtime.InteropServices.GCHandle[]]::new(2)
    $w.boolOriginal=[int[]]::new(2);$w.errorsOriginal=[int[]]::new(2)
    $w.exceptionsOriginal=[object[]]::new(3)
    $w.contextOriginal=[object[]]@($h.pathOriginal,$h.buffersOriginal,$delegate,$w.exceptionsOriginal,
      $w.markersOriginal,$h.pointerCellsOriginal,$w.pinsOriginal,$w.boolOriginal,$w.errorsOriginal)
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$r.bodyOriginal)
    $w.threadOriginal.IsBackground=$false;return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}
function Test-Cli39NamespaceWorkerBorrowClosure32 {
  param($WorkerOriginal)
  $w=$WorkerOriginal
  if($null -eq $w -or -not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  $m=$w.markersOriginal
  if($null -eq $w.threadOriginal -or $null -eq $m){return $false}
  if(-not $w.terminalJoined -or $w.threadOriginal.IsAlive -or $w.pendingActive -or
      $w.multiplicityUnknown -or $null -ne $w.joinException -or $m[18] -ne 1 -or $m[19] -ne 1){return $false}
  switch -CaseSensitive($w.phaseOriginal){
    'open'{
      if($m[0] -eq 1 -and $m[1] -ne 1){return $false}
      if($m[1] -eq 1 -and ($m[5] -ne 1 -or $m[6] -ne 1)){return $false}
      if($m[2] -eq 1 -and $m[3] -ne 1){return $false}
    }
    'metadata'{
      for($i=0;$i -lt 2;$i++){
        $o=$i*8
        if($m[$o] -eq 1 -and $m[$o+1] -ne 1){return $false}
        if($m[$o+3] -eq 1 -and $m[$o+4] -ne 1){return $false}
        if($m[$o+4] -eq 1 -and ($m[$o+5] -ne 1 -or $m[$o+6] -ne 1)){return $false}
      }
    }
    'close'{if($m[0] -eq 1 -and $m[1] -ne 1){return $false}}
    default{return $false}
  }
  return $true
}
function Invoke-Cli39NamespaceWorker32 {
  param($WorkerOriginal)
  $w=$WorkerOriginal;$p=$w.hostOwnerOriginal;$h=Get-Cli39NamespaceHold32 $p $w.holdOriginal;$b=$h.bookOriginal
  if(-not [object]::ReferenceEquals($h.workersOriginal[$w.phaseOriginal],$w) -or
      -not $b.rosterReturned -or -not $b.constructorTerminal -or $null -ne $b.constructorFailure -or
      -not $w.constructorTerminal -or
      $null -ne $w.constructorFailure -or $w.startAttempted){throw 'SAME once-only completed namespace worker required.'}
  if($w.phaseOriginal -ceq 'metadata' -and
      (-not $h.workersOriginal['open'].rawReturned -or $h.pointerCellsOriginal[1] -eq [IntPtr]::Zero -or
       $h.pointerCellsOriginal[1] -eq [IntPtr]::new(-1))){throw 'Actual completed SAME open handle required before metadata.'}
  if($w.phaseOriginal -ceq 'close'){
    if($null -eq $b.disposalOriginal -or -not $h.closeEligible){throw 'Actual eligible once-only namespace close required.'}
  }elseif($null -ne $b.disposalOriginal -or $p.consumerOriginal.ContainsKey('disposalOriginal')){
    throw 'Namespace disposal closes all future acquisition/metadata starts.'
  }
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported -and $w.phaseOriginal -cne 'close'){throw 'Original deadline forbids new namespace effect.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Uncertain actual namespace join remains retained.'}
    $w.pendingActive=$true;$o=$w.pendingOriginal
    $o.attempted=$true;$o.returned=$false;$o.value=$null;$o.exceptionOriginal=$null
    try{$o.value=$w.threadOriginal.Join(0);$o.returned=$true}catch{$o.exceptionOriginal=$_;$w.joinException=$_;throw}
    if($o.value){$w.terminalJoined=$true;$w.pendingActive=$false}
    else{
      $w.repeatedFalseOriginal.returned=$true
      if($w.repeatedFalseOriginal.repeats -eq [long]::MaxValue){$w.multiplicityUnknown=$true}
      else{$w.repeatedFalseOriginal.repeats++;$w.pendingActive=$false}
      $null=[Threading.Thread]::Yield()
    }
  }
  # Raw pointer/BOOL and return markers survive diagnostic failure. Physical
  # handle closure is classified independently by the enclosing original book.
  if(-not (Test-Cli39NamespaceWorkerBorrowClosure32 $w)){throw 'Actual namespace worker/pin/path closure remains pending.'}
  foreach($failure in $w.exceptionsOriginal){if($null -ne $failure){throw $failure}}
  if($w.phaseOriginal -ceq 'open'){
    if($w.markersOriginal[3] -ne 1 -or $w.markersOriginal[4] -ne 1){throw 'Complete original open observation required.'}
  }elseif($w.phaseOriginal -ceq 'metadata'){
    foreach($i in @(0,1)){if($w.markersOriginal[$i*8+1] -ne 1 -or
        $w.markersOriginal[$i*8+2] -ne 1){throw 'Complete original metadata observation required.'}}
  }elseif($w.markersOriginal[1] -ne 1 -or $w.markersOriginal[2] -ne 1){throw 'Complete original close observation required.'}
  $w.rawReturned=$true;return $w
}
