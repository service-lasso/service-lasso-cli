# UNRUN fixed output directory workers. No callback, supplied signature, loader,
# recursive traversal or independent native owner exists here.
function New-Cli39PythonDirectory32 {
  param([Parameter(Mandatory)]$DescriptorOriginal)
  $s=$DescriptorOriginal;$b=$s.bookOriginal;$p=$b.preparationOriginal;$c=$p.consumerOriginal
  if(-not [object]::ReferenceEquals($c.pythonTreeOriginal,$b) -or -not $b.returned -or
      $null -eq $b.descriptorsOriginal -or -not $b.descriptorsOriginal.Contains($s) -or
      -not $s.returned -or $s.ContainsKey('directoryOriginal') -or
      $c.ContainsKey('disposalOriginal') -or $null -ne $b.disposalOriginal -or
      $b.directoriesOriginal.Count -ge $b.maximumDirectories){
    throw 'Actual fixed report-derived once-only directory descriptor required.'
  }
  $d=@{bookOriginal=$b;descriptorOriginal=$s;pathOriginal=$s.pathOriginal;
    expectedNamesOriginal=$s.expectedNamesOriginal;maximumCalls=0;chargeBytes=[decimal]0;
    searchOriginal=$null;searchBytesOriginal=$null;handleOriginal=$null;workersOriginal=$null;
    closeOriginal=$null;disposalOriginal=$null;closeEligible=$false;
    acquiredObserved=$false;acquisitionReturned=$false;acquisitionUnknown=$false;
    physicalCloseObserved=$false;endObserved=$false;failureObserved=$false;
    rosterReturned=$false;constructorTerminal=$false;constructorFailure=$null}
  $s.directoryOriginal=$d;$b.directoriesOriginal.Add($d)
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    $v=$b.budgetOriginal
    [decimal]$calls=[decimal]$s.expectedNamesOriginal.Count+3
    [decimal]$pathBacking=2*([decimal]$s.pathOriginal.Length+3)
    if($null -eq $b.descriptorConstructionOriginal -or -not $b.descriptorConstructionOriginal.returned -or
        -not $b.descriptorConstructionOriginal.fixedMetadataPrecharged){throw 'Actual factory metadata precharge required.'}
    $d.chargeBytes=$calls*[decimal]$v.pythonTreeCapturePerEntryChargeBytes+
      $pathBacking*[decimal]$v.pythonTreeCapturePerByteChargeBytes
    if($calls -gt [int]::MaxValue -or $b.usedChargeBytes+$d.chargeBytes -gt $b.plannedChargeBytes -or
        $b.usedChargeBytes+$d.chargeBytes -gt $v.pythonTreeCaptureReservationBytes){
      throw 'Actual complete directory overlap exceeds SAME precharged tree subdivision.'
    }
    $b.usedChargeBytes+=$d.chargeBytes;$d.maximumCalls=[int]$calls
    $d.searchOriginal=$s.pathOriginal+'\*'
    $d.searchBytesOriginal=[Text.UnicodeEncoding]::new($false,$false,$true).GetBytes($d.searchOriginal+[char]0)
    $d.handleOriginal=[IntPtr[]]::new(1);$d.handleOriginal[0]=[IntPtr]::new(-1)
    $d.workersOriginal=[Collections.Generic.List[object]]::new($d.maximumCalls)
    for($i=0;$i -lt $d.maximumCalls;$i++){
      $phase='next';if($i -eq 0){$phase='first'}
      $null=New-Cli39PythonDirectoryWorker32 $d $phase $i
    }
    $null=New-Cli39PythonDirectoryWorker32 $d 'close' 0
    $d.rosterReturned=$true;return $d
  }catch{$d.constructorFailure=$_;throw}finally{$d.constructorTerminal=$true}
}

function New-Cli39PythonDirectoryWorker32 {
  param([Parameter(Mandatory)]$DirectoryOriginal,
    [Parameter(Mandatory)][ValidateSet('first','next','close')][string]$Phase,
    [Parameter(Mandatory)][int]$Ordinal)
  $d=$DirectoryOriginal;$b=$d.bookOriginal;$p=$b.preparationOriginal;$c=$p.consumerOriginal
  if(-not [object]::ReferenceEquals($c.pythonTreeOriginal,$b) -or
      -not $b.directoriesOriginal.Contains($d) -or $d.rosterReturned -or
      $Ordinal -lt 0 -or $Ordinal -ge $d.maximumCalls -or
      ($Phase -ceq 'first' -and $Ordinal -ne 0) -or
      ($Phase -ceq 'next' -and $Ordinal -eq 0) -or
      ($Phase -ceq 'close' -and $Ordinal -ne 0)){
    throw 'Actual preowned fixed directory roster required before search.'
  }
  $w=@{directoryOriginal=$d;phaseOriginal=$Phase;ordinalOriginal=$Ordinal;
    delegateOriginal=$null;bytesOriginal=$null;pinsOriginal=$null;markersOriginal=$null;
    resultsOriginal=$null;errorsOriginal=$null;exceptionsOriginal=$null;context=$null;
    methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;
    startException=$null;terminalJoined=$false;joinException=$null;nativeReturned=$false;
    joinCount=0L;joinCountUnknown=$false;
    pendingOriginal=@{attempted=$false;returned=$false;value=$false;exceptionOriginal=$null};
    completionReturned=$false;exceptionOriginal=$null}
  if($Phase -ceq 'close'){
    if($null -ne $d.closeOriginal){throw 'Directory close constructor cannot retry.'}
    $d.closeOriginal=$w
  }else{
    if($d.workersOriginal.Count -ne $Ordinal){throw 'Fixed directory worker ordinal must be contiguous.'}
    $d.workersOriginal.Add($w)
  }
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){
      throw 'Actual fixed admitted worker stack geometry required.'
    }
    $q=$b.queryOriginal
    $index=switch -CaseSensitive ($Phase){'first'{0}'next'{1}'close'{2}}
    $w.delegateOriginal=$q.delegatesOriginal[$index]
    if($null -eq $w.delegateOriginal -or
        $w.delegateOriginal.GetType() -ne $q.delegateTypesOriginal[$index]){
      throw 'SAME completed fixed directory delegate required.'
    }
    if($Phase -cne 'close'){$w.bytesOriginal=[byte[]]::new(592)}
    $w.pinsOriginal=[Runtime.InteropServices.GCHandle[]]::new(2)
    $w.markersOriginal=[int[]]::new(13);$w.resultsOriginal=[int[]]::new(1)
    $w.errorsOriginal=[int[]]::new(1);$w.exceptionsOriginal=[object[]]::new(3)
    $w.context=[object[]]@($w.delegateOriginal,$d.handleOriginal,$d.searchBytesOriginal,
      $w.bytesOriginal,$w.pinsOriginal,$w.markersOriginal,$w.resultsOriginal,
      $w.errorsOriginal,$w.exceptionsOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new(('Cli39FixedDirectory'+$Phase),
      [void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $failureLocal=$il.DeclareLocal([Exception]);$resultLocal=$il.DeclareLocal([int])
    function Emit-Cli39DirectoryContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39DirectoryMark32([int]$Index){
      Emit-Cli39DirectoryContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39DirectoryPin32([int]$Pin,[int]$Buffer,[int]$Marker){
      Emit-Cli39DirectoryMark32 $Marker
      Emit-Cli39DirectoryContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Pin)
      Emit-Cli39DirectoryContext32 $Buffer ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-Cli39DirectoryMark32 ($Marker+1)
    }
    function Emit-Cli39DirectoryAddress32([int]$Pin){
      Emit-Cli39DirectoryContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Pin)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
    }
    function Emit-Cli39DirectoryFailure32([int]$Index){
      Emit-Cli39DirectoryContext32 8 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    if($Phase -ceq 'first'){Emit-Cli39DirectoryPin32 0 2 0}
    if($Phase -cne 'close'){Emit-Cli39DirectoryPin32 1 3 2}
    Emit-Cli39DirectoryMark32 4
    if($Phase -ceq 'first'){
      Emit-Cli39DirectoryContext32 1 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      Emit-Cli39DirectoryContext32 0 ($w.delegateOriginal.GetType())
      Emit-Cli39DirectoryAddress32 0;$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
      Emit-Cli39DirectoryAddress32 1;$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Conv_I)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.delegateOriginal.GetType().GetMethod('Invoke'))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    }else{
      Emit-Cli39DirectoryContext32 0 ($w.delegateOriginal.GetType())
      Emit-Cli39DirectoryContext32 1 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
      if($Phase -ceq 'next'){Emit-Cli39DirectoryAddress32 1}
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.delegateOriginal.GetType().GetMethod('Invoke'))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$resultLocal)
      Emit-Cli39DirectoryContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$resultLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    Emit-Cli39DirectoryMark32 5
    Emit-Cli39DirectoryContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39DirectoryMark32 6
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39DirectoryFailure32 0
    $il.BeginFinallyBlock()
    foreach($pin in @(0,1)){
      $allocated=1+2*$pin;$freeAttempt=7+2*$pin;$freeReturned=$freeAttempt+1
      $skip=$il.DefineLabel();$safe=$il.DefineLabel();$null=$il.BeginExceptionBlock()
      Emit-Cli39DirectoryContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$allocated)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      Emit-Cli39DirectoryContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$safe)
      Emit-Cli39DirectoryContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_5)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      $il.MarkLabel($safe);Emit-Cli39DirectoryMark32 $freeAttempt
      Emit-Cli39DirectoryContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$pin)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
      Emit-Cli39DirectoryMark32 $freeReturned;$il.MarkLabel($skip)
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
      Emit-Cli39DirectoryFailure32 ($pin+1);$il.EndExceptionBlock()
    }
    Emit-Cli39DirectoryMark32 11;$il.EndExceptionBlock();Emit-Cli39DirectoryMark32 12
    $il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal,
      [int]$c.geometry.workerStackBytes)
    $w.threadOriginal.IsBackground=$false
    return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}

function Test-Cli39PythonDirectoryWorkerClosure32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$d=$w.directoryOriginal;$b=$d.bookOriginal
  if(-not [object]::ReferenceEquals($b.preparationOriginal.consumerOriginal.pythonTreeOriginal,$b) -or
      -not $b.directoriesOriginal.Contains($d) -or -not $w.constructorTerminal){return $false}
  if($w.phaseOriginal -ceq 'close'){
    if(-not [object]::ReferenceEquals($d.closeOriginal,$w)){return $false}
  }elseif(-not $d.workersOriginal.Contains($w)){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
      $null -ne $w.joinException -or $w.joinCountUnknown -or -not $w.pendingOriginal.returned -or
      -not $w.pendingOriginal.value -or $w.markersOriginal[11] -ne 1 -or $w.markersOriginal[12] -ne 1){return $false}
  $m=$w.markersOriginal
  if($m[4] -eq 1 -and $m[5] -ne 1){return $false}
  foreach($pin in @(0,1)){
    $attempt=2*$pin;$returned=$attempt+1;$freeAttempt=7+2*$pin;$freeReturned=$freeAttempt+1
    if(($m[$attempt] -eq 1 -and $m[$returned] -ne 1) -or
        ($m[$returned] -eq 1 -and ($m[$freeAttempt] -ne 1 -or $m[$freeReturned] -ne 1 -or
          $w.pinsOriginal[$pin].IsAllocated))){return $false}
  }
  return $true
}

function Invoke-Cli39PythonDirectoryWorker32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$d=$w.directoryOriginal;$b=$d.bookOriginal;$p=$b.preparationOriginal
  $c=$p.consumerOriginal
  if(-not [object]::ReferenceEquals($c.pythonTreeOriginal,$b) -or
      -not $b.directoriesOriginal.Contains($d) -or -not $d.rosterReturned -or
      -not $w.constructorTerminal -or $null -ne $w.constructorFailure -or
      $null -eq $w.threadOriginal -or $w.startAttempted){throw 'Actual completed fixed once-start worker roster required.'}
  if($w.phaseOriginal -ceq 'close'){
    if(-not [object]::ReferenceEquals($d.closeOriginal,$w) -or $null -eq $d.disposalOriginal -or
        -not $d.closeEligible -or -not $d.acquiredObserved -or $d.acquisitionUnknown -or
        $d.handleOriginal[0] -eq [IntPtr]::new(-1)){
      throw 'Known acquired find handle and actual independently eligible disposal required.'
    }
    foreach($borrower in $d.workersOriginal){
      if(-not (Test-Cli39PythonDirectoryWorkerClosure32 $borrower)){throw 'Actual issued directory borrower remains unresolved.'}
    }
  }else{
    Assert-Cli39PrerequisiteUseful26 $p
    if($null -eq $d.holdOriginal -or -not $d.holdOriginal.metadataReturned -or
        -not [object]::ReferenceEquals($d.holdOriginal.directoryOriginal,$d)){
      throw 'Actual SAME held nonreparse directory required before enumeration.'
    }
    if($c.ContainsKey('disposalOriginal') -or $null -ne $b.disposalOriginal -or
        $null -ne $d.disposalOriginal -or $d.endObserved -or $d.failureObserved -or
        -not [object]::ReferenceEquals($d.workersOriginal[$w.ordinalOriginal],$w)){
      throw 'Directory future-start barrier closed or row mismatch.'
    }
    if($w.ordinalOriginal -gt 0){
      $prior=$d.workersOriginal[$w.ordinalOriginal-1]
      if(-not $prior.completionReturned -or -not $prior.rowObserved -or
          -not (Test-Cli39PythonDirectoryWorkerClosure32 $prior) -or
          $d.handleOriginal[0] -eq [IntPtr]::new(-1)){
        throw 'Previous actual successful directory row required; uncertain calls cannot retry.'
      }
    }
  }
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.context);$w.startReturned=$true}
  catch{$w.startException=$_;$w.exceptionOriginal=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.joinCount -eq [long]::MaxValue){$w.joinCountUnknown=$true;throw 'Original directory join transcript exhausted.'}
    $w.joinCount++
    $w.pendingOriginal.attempted=$true;$w.pendingOriginal.returned=$false;$w.pendingOriginal.value=$false
    try{
      $w.pendingOriginal.value=$w.threadOriginal.Join(0);$w.pendingOriginal.returned=$true
      $w.terminalJoined=$w.pendingOriginal.value
    }catch{$w.pendingOriginal.exceptionOriginal=$_;$w.joinException=$_;$w.exceptionOriginal=$_;throw}
    if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
  }
  $w.nativeReturned=($w.markersOriginal[5] -eq 1)
  if($w.phaseOriginal -ceq 'first'){
    $d.acquisitionUnknown=($w.markersOriginal[4] -eq 1 -and -not $w.nativeReturned)
    $d.acquisitionReturned=$w.nativeReturned
    # A diagnostic/pin/row failure cannot erase the raw returned HANDLE.
    # INVALID_HANDLE_VALUE alone denotes failure in this API contract.
    $d.acquiredObserved=($w.nativeReturned -and $d.handleOriginal[0] -ne [IntPtr]::new(-1))
  }
  if($w.phaseOriginal -ceq 'close' -and $w.nativeReturned -and $w.resultsOriginal[0] -ne 0){
    # Physical return is retained before diagnostic/worker acceptance checks.
    $d.physicalCloseObserved=$true
  }
  if(-not (Test-Cli39PythonDirectoryWorkerClosure32 $w) -or
      $w.markersOriginal[6] -ne 1 -or -not $w.nativeReturned){
    throw 'Actual terminal native/diagnostic/pin closure is incomplete.'
  }
  foreach($failureOriginal in $w.exceptionsOriginal){
    if($null -ne $failureOriginal){$w.exceptionOriginal=$failureOriginal;throw 'Original directory worker failure retained.'}
  }
  $w.rowObserved=$false;$w.endObserved=$false
  if($w.phaseOriginal -ceq 'first'){
    if($d.handleOriginal[0] -eq [IntPtr]::new(-1)){
      $d.failureObserved=$true;throw 'First search failed; indeterminate buffer is not a directory row.'
    }
    $w.rowObserved=$true
  }elseif($w.phaseOriginal -ceq 'next'){
    if($w.resultsOriginal[0] -ne 0){$w.rowObserved=$true}
    elseif($w.errorsOriginal[0] -eq 18){$w.endObserved=$true;$d.endObserved=$true}
    else{$d.failureObserved=$true;throw 'Next search failed without actual no-more-files observation.'}
  }elseif(-not $d.physicalCloseObserved){throw 'FindClose did not report physical success.'}
  $w.completionReturned=$true
  return $w
}

function Read-Cli39PythonDirectoryRow32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$d=$w.directoryOriginal;$p=$d.bookOriginal.preparationOriginal
  if($w.phaseOriginal -cnotin @('first','next') -or -not $w.completionReturned -or
      -not $w.rowObserved -or -not (Test-Cli39PythonDirectoryWorkerClosure32 $w) -or
      $null -eq $w.bytesOriginal -or $w.bytesOriginal.Length -ne 592 -or
      $w.ContainsKey('decodedOriginal')){throw 'Actual successful once-decoded fixed directory row required.'}
  $o=@{workerOriginal=$w;bytesOriginal=$w.bytesOriginal;nameOriginal=$null;
    alternateNameOriginal=$null;nameCodeUnitsOriginal=$null;alternateCodeUnitsOriginal=$null;
    attributesOriginal=0U;reparseTagOriginal=0U;sizeOriginal=[decimal]0;
    creationLowOriginal=0U;creationHighOriginal=0U;accessLowOriginal=0U;accessHighOriginal=0U;
    writeLowOriginal=0U;writeHighOriginal=0U;returned=$false;exceptionOriginal=$null;
    state='ENUMERATED_ROW_NOT_CURRENT_HELD_FILE_AUTHENTICATION'}
  $w.decodedOriginal=$o
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    $bytes=$o.bytesOriginal
    $o.attributesOriginal=[BitConverter]::ToUInt32($bytes,0)
    $o.creationLowOriginal=[BitConverter]::ToUInt32($bytes,4);$o.creationHighOriginal=[BitConverter]::ToUInt32($bytes,8)
    $o.accessLowOriginal=[BitConverter]::ToUInt32($bytes,12);$o.accessHighOriginal=[BitConverter]::ToUInt32($bytes,16)
    $o.writeLowOriginal=[BitConverter]::ToUInt32($bytes,20);$o.writeHighOriginal=[BitConverter]::ToUInt32($bytes,24)
    $o.sizeOriginal=[decimal][BitConverter]::ToUInt32($bytes,28)*4294967296+
      [decimal][BitConverter]::ToUInt32($bytes,32)
    $o.reparseTagOriginal=[BitConverter]::ToUInt32($bytes,36)
    foreach($part in @(@('name',44,260),@('alternate',564,14))){
      $offset=[int]$part[1];$capacity=[int]$part[2];$count=0;$terminated=$false
      while($count -lt $capacity){
        Assert-Cli39PrerequisiteUseful26 $p
        if([BitConverter]::ToUInt16($bytes,$offset+2*$count) -eq 0){$terminated=$true;break}
        $count++
      }
      if(-not $terminated){throw 'Fixed directory UTF16 name lacks in-buffer terminator.'}
      $units=[char[]]::new($count)
      if($part[0] -ceq 'name'){$o.nameCodeUnitsOriginal=$units}else{$o.alternateCodeUnitsOriginal=$units}
      for($i=0;$i -lt $count;$i++){
        Assert-Cli39PrerequisiteUseful26 $p
        $units[$i]=[char][BitConverter]::ToUInt16($bytes,$offset+2*$i)
      }
      $text=[string]::new($units)
      if($part[0] -ceq 'name'){$o.nameOriginal=$text}else{$o.alternateNameOriginal=$text}
    }
    if([string]::IsNullOrEmpty($o.nameOriginal) -or
        $o.nameOriginal.IndexOfAny([char[]]@('/','\',':',[char]0)) -ge 0){
      throw 'Actual enumerated component is empty or contains a path delimiter.'
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Dispose-Cli39PythonDirectory32 {
  param([Parameter(Mandatory)]$DirectoryOriginal)
  $d=$DirectoryOriginal;$b=$d.bookOriginal;$p=$b.preparationOriginal
  if(-not [object]::ReferenceEquals($p.consumerOriginal.pythonTreeOriginal,$b) -or
      -not $b.directoriesOriginal.Contains($d) -or $null -ne $d.disposalOriginal){
    throw 'Actual directory disposal original is once-only.'
  }
  $o=@{directoryOriginal=$d;borrowersClosed=$false;noAcquisitionObserved=$false;
    closeAttempted=$false;closeOriginal=$null;returned=$false;exceptionOriginal=$null}
  $d.disposalOriginal=$o
  try{
    if(-not $d.constructorTerminal){throw 'Directory constructor remains unknown.'}
    foreach($worker in $d.workersOriginal){
      if(-not (Test-Cli39PythonDirectoryWorkerClosure32 $worker)){
        throw 'Actual directory borrower/pin closure is incomplete.'
      }
    }
    $o.borrowersClosed=$true
    $first=$null
    if($null -ne $d.workersOriginal -and $d.workersOriginal.Count -gt 0){$first=$d.workersOriginal[0]}
    if($null -eq $first -or -not $first.startAttempted){
      $o.noAcquisitionObserved=$true;$o.returned=$true;return $o
    }
    if($first.markersOriginal[4] -eq 1 -and $first.markersOriginal[5] -ne 1){
      $d.acquisitionUnknown=$true;throw 'Entered first search has no actual return.'
    }
    if($first.markersOriginal[4] -ne 1){
      $o.noAcquisitionObserved=$true;$o.returned=$true;return $o
    }
    # Derive again from SAME raw original even if Invoke's diagnostic acceptance
    # threw; neither a successful row nor successful last-error capture is owed
    # for known HANDLE acquisition to remain visible.
    $d.acquisitionReturned=($first.markersOriginal[5] -eq 1)
    $d.acquisitionUnknown=-not $d.acquisitionReturned
    $d.acquiredObserved=($d.acquisitionReturned -and $d.handleOriginal[0] -ne [IntPtr]::new(-1))
    if(-not $d.acquiredObserved){
      if($d.acquisitionUnknown){throw 'First search acquisition remains unknown.'}
      $o.noAcquisitionObserved=$true;$o.returned=$true;return $o
    }
    $close=$d.closeOriginal
    if($null -eq $close -or -not $close.constructorTerminal -or $null -ne $close.constructorFailure){
      throw 'Preowned close constructor is incomplete; acquired find handle retained.'
    }
    $d.closeEligible=$true;$o.closeAttempted=$true;$o.closeOriginal=$close
    $null=Invoke-Cli39PythonDirectoryWorker32 $close
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Test-Cli39PythonDirectoryClosure32 {
  param([Parameter(Mandatory)]$DirectoryOriginal)
  $d=$DirectoryOriginal;$b=$d.bookOriginal
  if(-not [object]::ReferenceEquals($b.preparationOriginal.consumerOriginal.pythonTreeOriginal,$b) -or
      -not $b.directoriesOriginal.Contains($d) -or -not $d.constructorTerminal -or
      $null -eq $d.disposalOriginal -or -not $d.disposalOriginal.borrowersClosed){return $false}
  foreach($worker in $d.workersOriginal){if(-not (Test-Cli39PythonDirectoryWorkerClosure32 $worker)){return $false}}
  if($d.acquisitionUnknown){return $false}
  if($d.disposalOriginal.noAcquisitionObserved){return $true}
  # Physical close remains true independently of later diagnostic failure.
  return ($d.acquiredObserved -and $d.physicalCloseObserved -and
    $null -ne $d.closeOriginal -and (Test-Cli39PythonDirectoryWorkerClosure32 $d.closeOriginal))
}
