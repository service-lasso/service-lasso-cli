# Two fixed raw process-token observations, never an atomic or startup identity
# assertion. The SDK pseudo-token -4 acquires no token and is never closed.
function New-Cli39PrincipalTokenBook32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$m=$root.principalModuleOriginal
  if($null -eq $m -or -not $m.returned -or $null -ne $m.tokenBookOriginal -or
      -not [object]::ReferenceEquals($m.hostOwnerOriginal,$p) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal){
    throw 'SAME once-only completed fixed principal module required.'
  }
  $book=@{hostOwnerOriginal=$p;rootCaptureOriginal=$root;moduleOriginal=$m;workersOriginal=$null;
    constructionTerminal=$false;constructorFailure=$null;returned=$false;exceptionOriginal=$null;
    state='RAW_PROCESS_TOKEN_OBSERVATIONS_NOT_PRINCIPAL_OR_CAPABILITY_ADMISSION'}
  $m.tokenBookOriginal=$book
  try {
    $book.workersOriginal=[Collections.Generic.List[object]]::new(2)
    foreach($phase in @('token-user','token-statistics')){
      $size=84;if($phase -ceq 'token-statistics'){$size=56}
      $w=@{bookOriginal=$book;phaseOriginal=$phase;informationBytes=$size;recipeOriginal=$m.recipesOriginal.recipesOriginal[$phase];
        backingOriginal=$null;pinsOriginal=$null;pointerOriginal=$null;markersOriginal=$null;resultsOriginal=$null;exceptionsOriginal=$null;
        contextOriginal=$null;threadOriginal=$null;decodedOriginal=$null;returnedBytesObserved=0U;
        constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
        terminalJoined=$false;pendingActive=$false;multiplicityUnknown=$false;joinException=$null;
        pendingOriginal=@{attempted=$false;returned=$false;value=$null;exceptionOriginal=$null};
        repeatedFalseOriginal=@{returned=$false;repeats=0L};rawReturned=$false}
      $book.workersOriginal.Add($w)
      try {
        $w.backingOriginal=[byte[]]::new($size+4);$w.pinsOriginal=[Runtime.InteropServices.GCHandle[]]::new(1)
        $w.pointerOriginal=[IntPtr[]]::new(1);$w.markersOriginal=[int[]]::new(9)
        $w.resultsOriginal=[int[]]::new(2);$w.exceptionsOriginal=[object[]]::new(2)
        $w.contextOriginal=[object[]]@($m.delegatesOriginal[0],$w.backingOriginal,$w.pinsOriginal,
          $w.pointerOriginal,$w.markersOriginal,$w.exceptionsOriginal,$w.resultsOriginal)
        $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.recipeOriginal.bodyOriginal)
        $w.threadOriginal.IsBackground=$false
      }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
    }
    return $book
  }catch{$book.constructorFailure=$_;throw}finally{$book.constructionTerminal=$true}
}

function Test-Cli39PrincipalTokenClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$BookOriginal)
  $p=$HostOwnerOriginal;$book=$BookOriginal;$m=$p.inputRead.value.principalModuleOriginal
  if($null -eq $m -or -not [object]::ReferenceEquals($m.tokenBookOriginal,$book) -or
      -not [object]::ReferenceEquals($book.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($book.moduleOriginal,$m) -or -not $book.constructionTerminal){return $false}
  foreach($w in $book.workersOriginal){
    if(-not [object]::ReferenceEquals($w.bookOriginal,$book) -or -not $w.constructorTerminal){return $false}
    if(-not $w.startAttempted){if($null -ne $w.threadOriginal -and $w.threadOriginal.IsAlive){return $false};continue}
    if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
        $w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException -or $w.markersOriginal[8] -ne 1){return $false}
    if($w.markersOriginal[1] -eq 1 -and $w.markersOriginal[2] -ne 1){return $false}
    if($w.markersOriginal[3] -eq 1 -and $w.markersOriginal[4] -ne 1){return $false}
    if($w.markersOriginal[2] -eq 1 -and ($w.markersOriginal[6] -ne 1 -or $w.markersOriginal[7] -ne 1 -or
        $w.pinsOriginal[0].IsAllocated -or $null -ne $w.exceptionsOriginal[1])){return $false}
  }
  return $true
}

function Invoke-Cli39PrincipalTokenWorker32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$book=$w.bookOriginal;$p=$book.hostOwnerOriginal;$root=$p.inputRead.value;$m=$book.moduleOriginal
  if(-not [object]::ReferenceEquals($root.principalModuleOriginal,$m) -or
      -not [object]::ReferenceEquals($m.tokenBookOriginal,$book) -or -not $book.workersOriginal.Contains($w) -or
      -not $book.constructionTerminal -or $null -ne $book.constructorFailure -or $book.workersOriginal.Count -ne 2 -or
      -not $w.constructorTerminal -or $null -ne $w.constructorFailure -or $w.startAttempted -or
      -not [object]::ReferenceEquals($w.recipeOriginal,$m.recipesOriginal.recipesOriginal[$w.phaseOriginal]) -or
      -not [object]::ReferenceEquals($w.contextOriginal[0],$m.delegatesOriginal[0]) -or
      -not [object]::ReferenceEquals($w.contextOriginal[1],$w.backingOriginal) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal){
    throw 'SAME once-only fixed live token worker required.'
  }
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline denies new token query.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Unknown actual token worker remains retained.'}
    $w.pendingActive=$true;$observation=$w.pendingOriginal;$observation.attempted=$true;$observation.returned=$false;$observation.value=$null
    try{$observation.value=$w.threadOriginal.Join(0);$observation.returned=$true}catch{$observation.exceptionOriginal=$_;$w.joinException=$_;throw}
    if($observation.value){$w.terminalJoined=$true;$w.pendingActive=$false}
    else{$w.repeatedFalseOriginal.returned=$true;if($w.repeatedFalseOriginal.repeats -eq [long]::MaxValue){$w.multiplicityUnknown=$true}else{$w.repeatedFalseOriginal.repeats++;$w.pendingActive=$false};$null=[Threading.Thread]::Yield()}
  }
  $w.returnedBytesObserved=[BitConverter]::ToUInt32($w.backingOriginal,$w.informationBytes)
  if(-not (Test-Cli39PrincipalTokenClosure32 $p $book)){throw 'Actual token query/pin closure remains pending.'}
  foreach($failure in $w.exceptionsOriginal){if($null -ne $failure){throw $failure}}
  foreach($index in @(0,1,2,3,4,5,6,7,8)){if($w.markersOriginal[$index] -ne 1){throw 'Complete actual token query markers required.'}}
  if($w.resultsOriginal[0] -eq 0 -or $w.returnedBytesObserved -gt $w.informationBytes){throw 'Raw token BOOL/extent failure retained.'}
  $decoded=@{workerOriginal=$w;rawBackingOriginal=$w.backingOriginal;fieldsOriginal=$null;
    sidOffsetOriginal=$null;sidExtentOriginal=$null;returned=$false;exceptionOriginal=$null}
  $w.decodedOriginal=$decoded
  try {
    $decoded.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $bytes=$w.backingOriginal
    if($w.phaseOriginal -ceq 'token-user'){
      if($w.returnedBytesObserved -lt 16){throw 'TOKEN_USER header extent incomplete.'}
      $sidPointer=[BitConverter]::ToInt64($bytes,0);$basePointer=$w.pointerOriginal[0].ToInt64()
      [decimal]$offset=[decimal]$sidPointer-[decimal]$basePointer
      $decoded.fieldsOriginal.Add('sidPointerObserved',$sidPointer)
      $decoded.fieldsOriginal.Add('attributesObserved',[BitConverter]::ToUInt32($bytes,8))
      if($offset -lt 16 -or $offset -gt ([long]$w.returnedBytesObserved-8)){throw 'Actual returned SID pointer is outside SAME captured output.'}
      $decoded.sidOffsetOriginal=[int]$offset
      $revision=$bytes[[int]$offset];$count=$bytes[[int]$offset+1];$extent=8+4*[int]$count
      $decoded.fieldsOriginal.Add('sidRevisionObserved',$revision);$decoded.fieldsOriginal.Add('sidSubAuthorityCountObserved',$count)
      if($revision -ne 1 -or $count -gt 15 -or $extent -gt 68 -or $offset+$extent -gt $w.returnedBytesObserved){throw 'Source-bounded actual SID bytes incomplete.'}
      # Alias into retained output, not a stale pointer dereference or string identity.
      $decoded.sidExtentOriginal=$extent
    }elseif($w.phaseOriginal -ceq 'token-statistics'){
      if($w.returnedBytesObserved -ne 56){throw 'Exact TOKEN_STATISTICS extent differs.'}
      foreach($pair in @(@('tokenIdObserved',0),@('authenticationIdObserved',8),@('expirationTimeObserved',16),@('modifiedIdObserved',48))){
        $decoded.fieldsOriginal.Add($pair[0],[BitConverter]::ToInt64($bytes,$pair[1]))
      }
      foreach($pair in @(@('tokenTypeObserved',24),@('impersonationLevelObserved',28),@('dynamicChargedObserved',32),@('dynamicAvailableObserved',36),@('groupCountObserved',40),@('privilegeCountObserved',44))){
        $decoded.fieldsOriginal.Add($pair[0],[BitConverter]::ToUInt32($bytes,$pair[1]))
      }
    }else{throw 'No generic token information decoder exists.'}
    $decoded.returned=$true;$w.rawReturned=$true;return $w
  }catch{$decoded.exceptionOriginal=$_;throw}
}

function Observe-Cli39PrincipalTokens32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $book=New-Cli39PrincipalTokenBook32 $HostOwnerOriginal
  try {
    foreach($worker in $book.workersOriginal){$null=Invoke-Cli39PrincipalTokenWorker32 $worker}
    $book.returned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}
}
