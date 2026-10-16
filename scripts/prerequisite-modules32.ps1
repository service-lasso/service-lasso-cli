# Fixed Toolhelp snapshot originals, not loaded-source admission.
function New-Cli39RootModuleBook32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$m=$root.nativeQueryOriginal;$a=$root.abiOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $m.complete -or -not $root.rootProcessOriginal.rawReturned -or
      -not (Test-Cli39RootProcessClosure32 $p) -or $root.ContainsKey('rootModuleBookOriginal') -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      -not [object]::ReferenceEquals($m.moduleOwnerOriginal,$a.originalOwner) -or
      -not [object]::ReferenceEquals($a.delegates,$a.originalOwner.delegates) -or
      $null -eq $a.delegates['CloseHandle']){throw 'SAME once-only snapshot owner and resident typed close required.'}
  $book=@{hostOwnerOriginal=$p;rootCaptureOriginal=$root;queryOriginal=$m;abiOriginal=$a;
    processOriginal=$root.rootProcessOriginal;loadedElementOriginal=$root.fieldMapOriginal['loadedClosure'];
    terminalLifetime='host-process';handleOriginal=$null;acquireOriginal=$null;closeOriginal=$null;
    rowsOriginal=$null;maximumRows=0;expectedRows=0;pendingRowOriginal=$null;rowConstructorUnknown=$false;
    constructorTerminal=$false;constructorFailure=$null;acquisitionUnknown=$false;
    endObserved=$false;failureObserved=$false;enumerationReturned=$false;returned=$false;exceptionOriginal=$null;primaryFailureOriginal=$null;closeFailureOriginal=$null;disposalOriginal=$null;
    state='RAW_SNAPSHOT_SOURCE_ONLY_INSTALLED_LOADED_ADMISSION_PENDING'}
  $root.rootModuleBookOriginal=$book
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids snapshot book construction.'}
    $count=$book.loadedElementOriginal.GetArrayLength()
    if($count -ge [int]::MaxValue){throw 'Actual loaded input roster plus rejection witness unsupported.'}
    $book.expectedRows=$count;$book.maximumRows=$count+1
    $book.handleOriginal=[IntPtr[]]::new(1)
    $book.rowsOriginal=[Collections.Generic.List[object]]::new($book.maximumRows)
    return $book
  }catch{$book.constructorFailure=$_;throw}finally{$book.constructorTerminal=$true}
}

function Get-Cli39RootModuleBook32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$b=$root.rootModuleBookOriginal
  if($null -eq $b -or -not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($b.rootCaptureOriginal,$root) -or
      -not [object]::ReferenceEquals($b.queryOriginal,$root.nativeQueryOriginal) -or
      -not [object]::ReferenceEquals($b.processOriginal,$root.rootProcessOriginal) -or
      -not [object]::ReferenceEquals($b.abiOriginal,$root.abiOriginal) -or
      -not $b.constructorTerminal -or $null -ne $b.constructorFailure){throw 'Actual SAME complete snapshot book constructor required.'}
  return $b
}

function New-Cli39RootModuleRow32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$b=Get-Cli39RootModuleBook32 $p
  if($p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $b.disposalOriginal -or $null -ne $b.closeOriginal -or
      $b.rowConstructorUnknown -or $null -ne $b.exceptionOriginal -or $b.endObserved -or $b.failureObserved -or
      $null -eq $b.acquireOriginal -or -not $b.acquireOriginal.terminalJoined -or
      -not $b.acquireOriginal.rawReturned -or $b.acquisitionUnknown -or
      $b.handleOriginal[0] -eq [IntPtr]::Zero -or $b.handleOriginal[0] -eq [IntPtr]::new(-1) -or
      $b.rowsOriginal.Count -ge $b.maximumRows){throw 'Actual open acquired snapshot and next fixed row required.'}
  if($b.rowsOriginal.Count -gt 0 -and -not $b.rowsOriginal[$b.rowsOriginal.Count-1].completionReturned){throw 'Previous actual complete row required; no next/retry after uncertainty.'}
  $b.rowConstructorUnknown=$true
  try{
    $row=@{bookOriginal=$b;ordinalOriginal=$b.rowsOriginal.Count;bytesOriginal=$null;workerOriginal=$null;
      constructorTerminal=$false;constructorFailure=$null;completionReturned=$false;moduleObserved=$false;
      endObserved=$false;failureObserved=$false;decodedOriginal=$null;exceptionOriginal=$null}
    $b.pendingRowOriginal=$row;$b.rowsOriginal.Add($row);$b.rowConstructorUnknown=$false
    try{
      $row.bytesOriginal=[byte[]]::new(1080)
      [Array]::Copy([BitConverter]::GetBytes([uint32]1080),0,$row.bytesOriginal,0,4)
      return $row
    }catch{$row.constructorFailure=$_;$b.exceptionOriginal=$_;throw}finally{$row.constructorTerminal=$true}
  }catch{$b.exceptionOriginal=$_;throw}
}
function New-Cli39RootModuleWorker32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)][ValidateSet('acquire','first','next','close')][string]$Phase,$RowOriginal=$null)
  $p=$HostOwnerOriginal;$b=Get-Cli39RootModuleBook32 $p;$m=$b.queryOriginal;$row=$RowOriginal
  if($Phase -ceq 'acquire'){
    if($null -ne $b.acquireOriginal -or $null -ne $b.disposalOriginal -or $null -ne $b.closeOriginal -or $b.rowsOriginal.Count -ne 0 -or
        $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'Actual snapshot acquisition cannot retry.'}
  }elseif($Phase -ceq 'close'){
    if($null -ne $row -or $null -ne $b.closeOriginal -or -not (Test-Cli39RootModuleBorrowClosed32 $p) -or
        $b.acquisitionUnknown -or $null -eq $b.acquireOriginal -or -not $b.acquireOriginal.nativeReturned -or
        $b.handleOriginal[0] -eq [IntPtr]::Zero -or $b.handleOriginal[0] -eq [IntPtr]::new(-1)){throw 'Actual known acquired snapshot and closed borrowers required for once-only close.'}
  }else{
    if($null -eq $row -or -not $b.rowsOriginal.Contains($row) -or
        -not [object]::ReferenceEquals($row.bookOriginal,$b) -or $null -ne $row.workerOriginal -or
        -not $row.constructorTerminal -or $null -ne $row.constructorFailure -or
        $null -ne $b.closeOriginal -or $null -ne $b.disposalOriginal -or $b.endObserved -or $b.failureObserved -or
        $p.consumerOriginal.ContainsKey('disposalOriginal') -or
        ($Phase -ceq 'first' -and $row.ordinalOriginal -ne 0) -or
        ($Phase -ceq 'next' -and $row.ordinalOriginal -le 0)){throw 'Actual next enrolled fixed snapshot row required.'}
  }
  $w=@{phaseOriginal=$Phase;hostOwnerOriginal=$p;bookOriginal=$b;rowOriginal=$row;
    delegateOriginal=$null;pinOriginal=$null;resultsOriginal=$null;errorsOriginal=$null;markersOriginal=$null;exceptionsOriginal=$null;
    context=$null;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;pendingActive=$false;multiplicityUnknown=$false;joinException=$null;
    pending=@{attempted=$false;returned=$false;value=$null;exception=$null};repeatedFalse=@{returned=$false;repeats=0L};
    completionAttempted=$false;nativeReturned=$false;rawReturned=$false;bufferOriginal=$null}
  switch -CaseSensitive ($Phase){'acquire'{$b.acquireOriginal=$w}'close'{$b.closeOriginal=$w}default{$row.workerOriginal=$w}}
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported -and $Phase -cne 'close'){throw 'Original deadline forbids new snapshot phase.'}
    $w.delegateOriginal=switch -CaseSensitive ($Phase){
      'acquire'{$m.delegatesOriginal[6]}'first'{$m.delegatesOriginal[7]}
      'next'{$m.delegatesOriginal[8]}'close'{$b.abiOriginal.delegates['CloseHandle']}}
    $w.pinOriginal=[Runtime.InteropServices.GCHandle[]]::new(1);$w.resultsOriginal=[int[]]::new(1)
    $w.errorsOriginal=[int[]]::new(1);$w.markersOriginal=[int[]]::new(7);$w.exceptionsOriginal=[object[]]::new(2)
    if($null -ne $row){$w.bufferOriginal=$row.bytesOriginal}
    $w.context=[object[]]@($w.delegateOriginal,$b.handleOriginal,$w.bufferOriginal,$w.pinOriginal,
      $w.resultsOriginal,$w.errorsOriginal,$w.markersOriginal,$w.exceptionsOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new(('Cli39FixedSnapshot'+$Phase),[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $failureLocal=$il.DeclareLocal([Exception]);$resultLocal=$il.DeclareLocal([int])
    function Emit-Cli39ModuleContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39ModuleMark32([int]$Index){
      Emit-Cli39ModuleContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39ModuleFailure32([int]$Index){
      Emit-Cli39ModuleContext32 7 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    if($Phase -cin @('first','next')){
      Emit-Cli39ModuleMark32 0
      Emit-Cli39ModuleContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      Emit-Cli39ModuleContext32 2 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-Cli39ModuleMark32 1
    }
    Emit-Cli39ModuleMark32 2
    if($Phase -ceq 'acquire'){
      Emit-Cli39ModuleContext32 1 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      Emit-Cli39ModuleContext32 0 ($w.delegateOriginal.GetType())
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_8);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I8,[long]$b.processOriginal.pidOriginal[0]);$il.Emit([Reflection.Emit.OpCodes]::Conv_U4)
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.delegateOriginal.GetType().GetMethod('Invoke'))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    }else{
      Emit-Cli39ModuleContext32 0 ($w.delegateOriginal.GetType())
      Emit-Cli39ModuleContext32 1 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
      if($Phase -cin @('first','next')){
        Emit-Cli39ModuleContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
        $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
        $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
      }
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.delegateOriginal.GetType().GetMethod('Invoke'))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$resultLocal)
      Emit-Cli39ModuleContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$resultLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    Emit-Cli39ModuleMark32 6
    Emit-Cli39ModuleContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ModuleMark32 3
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39ModuleFailure32 0
    $il.BeginFinallyBlock()
    if($Phase -cin @('first','next')){
      $skip=$il.DefineLabel();$null=$il.BeginExceptionBlock()
      Emit-Cli39ModuleContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip);Emit-Cli39ModuleMark32 4
      Emit-Cli39ModuleContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
      Emit-Cli39ModuleMark32 5;$il.MarkLabel($skip)
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39ModuleFailure32 1;$il.EndExceptionBlock()
    }
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal);$w.threadOriginal.IsBackground=$false
    return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}
function Get-Cli39RootModuleWorker32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$p=$w.hostOwnerOriginal;$b=Get-Cli39RootModuleBook32 $p
  if(-not [object]::ReferenceEquals($w.bookOriginal,$b)){throw 'SAME actual snapshot worker required.'}
  switch -CaseSensitive ($w.phaseOriginal){
    'acquire'{if(-not [object]::ReferenceEquals($b.acquireOriginal,$w)){throw 'Actual acquisition original required.'}}
    'close'{if(-not [object]::ReferenceEquals($b.closeOriginal,$w)){throw 'Actual once close original required.'}}
    {$_ -cin @('first','next')}{
      $r=$w.rowOriginal
      if(-not $b.rowsOriginal.Contains($r) -or -not [object]::ReferenceEquals($r.workerOriginal,$w) -or
          -not [object]::ReferenceEquals($r.bookOriginal,$b) -or
          -not [object]::ReferenceEquals($w.bufferOriginal,$r.bytesOriginal) -or
          ($w.phaseOriginal -ceq 'first' -and $r.ordinalOriginal -ne 0) -or
          ($w.phaseOriginal -ceq 'next' -and $r.ordinalOriginal -le 0)){throw 'Actual fixed row worker required.'}
    }
    default{throw 'No generic snapshot worker exists.'}
  }
  return $w
}

function Test-Cli39RootModuleWorkerClosure32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=Get-Cli39RootModuleWorker32 $WorkerOriginal
  if(-not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if(-not $w.terminalJoined -or $null -eq $w.threadOriginal -or $w.threadOriginal.IsAlive -or
      $w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){return $false}
  if($w.phaseOriginal -cin @('first','next')){
    if($w.markersOriginal[0] -eq 1 -and $w.markersOriginal[1] -ne 1){return $false}
    if($w.markersOriginal[1] -eq 1 -and ($w.markersOriginal[4] -ne 1 -or $w.markersOriginal[5] -ne 1 -or
        $w.pinOriginal[0].IsAllocated -or $null -ne $w.exceptionsOriginal[1])){return $false}
  }
  return $true
}
function Test-Cli39RootModuleBorrowClosed32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('rootModuleBookOriginal')){return $true}
  $b=$root.rootModuleBookOriginal
  if(-not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or -not $b.constructorTerminal -or
      $b.rowConstructorUnknown){return $false}
  if($null -ne $b.acquireOriginal -and -not (Test-Cli39RootModuleWorkerClosure32 $b.acquireOriginal)){return $false}
  foreach($r in $b.rowsOriginal){
    if(-not [object]::ReferenceEquals($r.bookOriginal,$b) -or -not $r.constructorTerminal){return $false}
    if($null -ne $r.workerOriginal -and -not (Test-Cli39RootModuleWorkerClosure32 $r.workerOriginal)){return $false}
  }
  return $true
}

function Invoke-Cli39RootModuleWorker32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=Get-Cli39RootModuleWorker32 $WorkerOriginal;$p=$w.hostOwnerOriginal;$b=$w.bookOriginal
  if(-not $w.constructorTerminal -or $null -ne $w.constructorFailure -or $w.startAttempted){throw 'Actual complete once-only snapshot worker required.'}
  if($w.phaseOriginal -cne 'close' -and ($null -ne $b.disposalOriginal -or $null -ne $b.closeOriginal -or $p.consumerOriginal.ContainsKey('disposalOriginal'))){throw 'Snapshot future-start gate closed.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported -and $w.phaseOriginal -cne 'close'){throw 'Original deadline forbids new snapshot acquisition/enumeration.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.context);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Uncertain actual snapshot join retained.'}
    $w.pendingActive=$true;$o=$w.pending;$o.attempted=$true;$o.returned=$false;$o.value=$null;$o.exception=$null
    try{$o.value=$w.threadOriginal.Join(0);$o.returned=$true}catch{$o.exception=$_;$w.joinException=$_;throw}
    if($o.value){$w.terminalJoined=$true;$w.pendingActive=$false}
    else{$w.repeatedFalse.returned=$true;if($w.repeatedFalse.repeats -eq [long]::MaxValue){$w.multiplicityUnknown=$true}else{$w.repeatedFalse.repeats++;$w.pendingActive=$false};$null=[Threading.Thread]::Yield()}
  }
  $w.completionAttempted=$true;$w.nativeReturned=($w.markersOriginal[6] -eq 1)
  if($w.phaseOriginal -ceq 'acquire'){
    $b.acquisitionUnknown=(($w.markersOriginal[2] -eq 1 -and -not $w.nativeReturned) -or
      ($w.nativeReturned -and $b.handleOriginal[0] -eq [IntPtr]::Zero))
  }
  if(-not (Test-Cli39RootModuleWorkerClosure32 $w)){throw 'Actual snapshot worker/pin closure incomplete.'}
  foreach($failureOriginal in $w.exceptionsOriginal){if($null -ne $failureOriginal){throw $failureOriginal}}
  if($w.markersOriginal[2] -ne 1 -or $w.markersOriginal[3] -ne 1 -or -not $w.nativeReturned){throw 'Actual snapshot native call/output/error did not return completely.'}
  $w.rawReturned=$true;return $w
}
function Complete-Cli39RootModuleRow32 {
  param([Parameter(Mandatory)]$RowOriginal)
  $r=$RowOriginal;$b=$r.bookOriginal;$p=$b.hostOwnerOriginal;$w=Get-Cli39RootModuleWorker32 $r.workerOriginal
  if(-not $b.rowsOriginal.Contains($r) -or -not [object]::ReferenceEquals($w.rowOriginal,$r) -or
      -not $w.rawReturned -or -not $w.terminalJoined -or $r.completionReturned){throw 'Actual once-only complete snapshot row required.'}
  try{
    if($w.resultsOriginal[0] -eq 0){
      if($w.errorsOriginal[0] -eq 18){$r.endObserved=$true;$b.endObserved=$true}
      else{$r.failureObserved=$true;$b.failureObserved=$true}
      $r.completionReturned=$true;return $r
    }
    $r.moduleObserved=$true
    $r.decodedOriginal=@{rowOriginal=$r;sizeOriginal=0;moduleIdOriginal=0;processIdOriginal=0;
      globalCountOriginal=0;processCountOriginal=0;baseOriginal=[IntPtr]::Zero;imageBytesOriginal=0;
      moduleHandleOriginal=[IntPtr]::Zero;moduleNameOffset=48;moduleNameBytes=0;moduleNameTerminated=$false;
      pathOffset=560;pathBytes=0;pathTerminated=$false;pathCompletenessQualified=$false;returned=$false;exceptionOriginal=$null}
    $d=$r.decodedOriginal;$bytes=$r.bytesOriginal
    $d.sizeOriginal=[BitConverter]::ToUInt32($bytes,0);$d.moduleIdOriginal=[BitConverter]::ToUInt32($bytes,4)
    $d.processIdOriginal=[BitConverter]::ToUInt32($bytes,8);$d.globalCountOriginal=[BitConverter]::ToUInt32($bytes,12)
    $d.processCountOriginal=[BitConverter]::ToUInt32($bytes,16);$d.baseOriginal=[IntPtr]::new([BitConverter]::ToInt64($bytes,24))
    $d.imageBytesOriginal=[BitConverter]::ToUInt32($bytes,32);$d.moduleHandleOriginal=[IntPtr]::new([BitConverter]::ToInt64($bytes,40))
    foreach($part in @('moduleName','path')){
      $offset=if($part -ceq 'moduleName'){48}else{560};$capacity=if($part -ceq 'moduleName'){256}else{260}
      for($ordinal=0;$ordinal -lt $capacity;$ordinal++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended raw module string inspection.'}
        if($bytes[$offset+2*$ordinal] -eq 0 -and $bytes[$offset+2*$ordinal+1] -eq 0){$d[$part+'Terminated']=$true;break}
        $d[$part+'Bytes']=2*($ordinal+1)
      }
    }
    if($d.sizeOriginal -ne 1080 -or $d.processIdOriginal -ne $b.processOriginal.pidOriginal[0]){throw 'Actual module row layout/PID is inconsistent; raw row retained.'}
    $d.returned=$true;$r.completionReturned=$true;return $r
  }catch{$r.exceptionOriginal=$_;if($null -ne $r.decodedOriginal){$r.decodedOriginal.exceptionOriginal=$_};throw}
}

function Invoke-Cli39RootModuleDisposal32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('rootModuleBookOriginal')){return $null}
  $b=$root.rootModuleBookOriginal
  if($null -ne $b.disposalOriginal){throw 'Snapshot disposal enrollment cannot retry.'}
  $d=@{bookOriginal=$b;attempted=$true;pendingBorrow=$false;unknownAcquisition=$false;
    noAcquisitionObserved=$false;closeWorkerOriginal=$null;returned=$false;exceptionOriginal=$null}
  $b.disposalOriginal=$d
  try{
    if(-not (Test-Cli39RootModuleBorrowClosed32 $p)){$d.pendingBorrow=$true;throw 'Actual snapshot native worker/pin borrow pending.'}
    $a=$b.acquireOriginal
    if($null -eq $a -or -not $a.startAttempted){$d.noAcquisitionObserved=$true;$d.returned=$true;return $d}
    if(-not $a.nativeReturned -or $b.acquisitionUnknown){$d.unknownAcquisition=$true;throw 'Actual snapshot acquisition unknown; retain original.'}
    if($b.handleOriginal[0] -eq [IntPtr]::new(-1)){$d.noAcquisitionObserved=$true;$d.returned=$true;return $d}
    if($b.handleOriginal[0] -eq [IntPtr]::Zero){$d.unknownAcquisition=$true;throw 'Unexpected null snapshot retained unknown.'}
    $w=New-Cli39RootModuleWorker32 $p 'close';$d.closeWorkerOriginal=$w
    $null=Invoke-Cli39RootModuleWorker32 $w
    if(-not $w.nativeReturned -or $w.resultsOriginal[0] -eq 0){throw 'Actual snapshot CloseHandle did not return success; no retry.'}
    $d.returned=$true;return $d
  }catch{$d.exceptionOriginal=$_;$b.closeFailureOriginal=$_;throw}
}
function Test-Cli39RootModuleClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('rootModuleBookOriginal')){return $true}
  $b=$root.rootModuleBookOriginal;$d=$b.disposalOriginal
  if($null -eq $d -or -not [object]::ReferenceEquals($d.bookOriginal,$b) -or
      $d.pendingBorrow -or $d.unknownAcquisition -or -not (Test-Cli39RootModuleBorrowClosed32 $p)){return $false}
  if($d.noAcquisitionObserved){return ($d.returned -and ($null -eq $b.acquireOriginal -or
      -not $b.acquireOriginal.startAttempted -or ($b.acquireOriginal.nativeReturned -and $b.handleOriginal[0] -eq [IntPtr]::new(-1))))}
  $w=$b.closeOriginal
  # Typed successful CloseHandle return proves physical close even if later
  # diagnostic capture threw; that actual failure remains retained separately.
  return ($null -ne $w -and [object]::ReferenceEquals($w,$d.closeWorkerOriginal) -and
    (Test-Cli39RootModuleWorkerClosure32 $w) -and $w.nativeReturned -and $w.resultsOriginal[0] -ne 0)
}
function Observe-Cli39RootModules32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$b=New-Cli39RootModuleBook32 $p
  try{
    $w=New-Cli39RootModuleWorker32 $p 'acquire';$null=Invoke-Cli39RootModuleWorker32 $w
    if($b.acquisitionUnknown -or $b.handleOriginal[0] -eq [IntPtr]::new(-1)){$b.failureObserved=$true;throw 'Actual Toolhelp acquisition failed/unknown; no retry.'}
    while(-not $b.endObserved -and -not $b.failureObserved){
      $r=New-Cli39RootModuleRow32 $p;$phase=if($r.ordinalOriginal -eq 0){'first'}else{'next'}
      $w=New-Cli39RootModuleWorker32 $p $phase $r;$null=Invoke-Cli39RootModuleWorker32 $w
      $null=Complete-Cli39RootModuleRow32 $r
      if($r.failureObserved){throw 'Actual module enumeration failed; raw BOOL/error/row retained.'}
      if($r.moduleObserved -and $r.ordinalOriginal -ge $b.expectedRows){$b.failureObserved=$true;throw 'Actual extra module rejection witness retained; no silent truncation.'}
    }
    $b.enumerationReturned=$b.endObserved
  }catch{$b.primaryFailureOriginal=$_;$b.exceptionOriginal=$_}
  finally{try{$null=Invoke-Cli39RootModuleDisposal32 $p}catch{$b.closeFailureOriginal=$_}}
  if($null -ne $b.primaryFailureOriginal){throw $b.primaryFailureOriginal}
  if($null -ne $b.closeFailureOriginal){throw $b.closeFailureOriginal}
  if(-not $b.enumerationReturned -or -not (Test-Cli39RootModuleClosure32 $p)){throw 'Actual snapshot enumeration/physical closure incomplete.'}
  $b.returned=$true;return $b
}