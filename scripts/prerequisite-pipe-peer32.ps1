# Fixed actual control-server peer observation; no supplied handle or identity authority.
function New-Cli39RootPipePeerWorker32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$capture=$p.inputRead.value;$m=$capture.nativeQueryOriginal
  $subject=$p.server
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $m.complete -or $capture.ContainsKey('rootPipePeerOriginal') -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -eq $subject -or -not $subject.IsConnected -or $null -eq $subject.SafePipeHandle -or
      $subject.SafePipeHandle.IsClosed -or
      -not [object]::ReferenceEquals($subject,$p.server)){
    throw 'SAME once-only connected control-server handle and fixed query ABI required.'
  }
  $w=@{phase='root-pipe-peer';hostOwnerOriginal=$p;queryOriginal=$m;
    subjectOriginal=$subject;handleOriginal=$subject.SafePipeHandle;
    buffersOriginal=$null;pinsOriginal=$null;classesOriginal=$null;sizesOriginal=$null;
    borrowOriginal=[bool[]]::new(1);handleValueOriginal=[IntPtr[]]::new(1);
    borrowMarkers=[int[]]::new(4);pinMarkers=[int[]]::new(4);
    callMarkers=[int[]]::new(3);resultsOriginal=[int[]]::new(1);clientPidObserved=$null;
    errorsOriginal=[int[]]::new(1);exceptionsOriginal=[object[]]::new(3);
    context=$null;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;
    startReturned=$false;startException=$null;terminalJoined=$false;
    pendingActive=$false;multiplicityUnknown=$false;joinException=$null;
    pending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    repeatedFalse=@{returned=$false;repeats=0L};completionAttempted=$false;
    completionReturned=$false;rawReturned=$false}
  $capture.rootPipePeerOriginal=$w
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids new control-server peer construction.'}
    # One DWORD client PID from the actual connected control-server handle.
    # Raw PID never establishes peer identity, principal or capability.
    $w.classesOriginal=[int[]]@();$w.sizesOriginal=[int[]]@(4)
    $w.buffersOriginal=[object[]]::new(1)
    for($i=0;$i -lt 1;$i++){$w.buffersOriginal[$i]=[byte[]]::new($w.sizesOriginal[$i])}
    $w.pinsOriginal=[Runtime.InteropServices.GCHandle[]]::new(1)
    $w.context=[object[]]@($w.handleOriginal,$m.delegatesOriginal[9],$w.buffersOriginal,
      $w.pinsOriginal,$w.borrowOriginal,$w.handleValueOriginal,$w.borrowMarkers,
      $w.pinMarkers,$w.callMarkers,$w.resultsOriginal,$w.errorsOriginal,$w.exceptionsOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedRootPipePeer',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $failure=$il.DeclareLocal([Exception]);$result=$il.DeclareLocal([int])
    function Emit-Cli39PipePeerContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39PipePeerMark32([int]$ContextIndex,[int]$Index){
      Emit-Cli39PipePeerContext32 $ContextIndex ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39PipePeerError32([int]$Index){
      Emit-Cli39PipePeerContext32 11 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failure);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    Emit-Cli39PipePeerMark32 6 0
    Emit-Cli39PipePeerContext32 0 ([Microsoft.Win32.SafeHandles.SafePipeHandle])
    Emit-Cli39PipePeerContext32 4 ([bool[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[bool])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Runtime.InteropServices.SafeHandle].GetMethod('DangerousAddRef',[Type[]]@([bool].MakeByRefType())))
    Emit-Cli39PipePeerMark32 6 1
    Emit-Cli39PipePeerContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39PipePeerContext32 0 ([Microsoft.Win32.SafeHandles.SafePipeHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Runtime.InteropServices.SafeHandle].GetMethod('DangerousGetHandle',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    for($i=0;$i -lt 1;$i++){
      Emit-Cli39PipePeerMark32 7 ($i*4)
      Emit-Cli39PipePeerContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      Emit-Cli39PipePeerContext32 2 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle])
      Emit-Cli39PipePeerMark32 7 ($i*4+1)
      Emit-Cli39PipePeerMark32 8 ($i*2)
      Emit-Cli39PipePeerContext32 1 $m.delegateTypesOriginal[9]
      Emit-Cli39PipePeerContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
      Emit-Cli39PipePeerContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$m.delegateTypesOriginal[9].GetMethod('Invoke'))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$result)
      Emit-Cli39PipePeerContext32 9 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$result);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      Emit-Cli39PipePeerMark32 8 ($i*2+1)
      Emit-Cli39PipePeerContext32 10 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      Emit-Cli39PipePeerMark32 8 2
    }
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
    Emit-Cli39PipePeerError32 0
    $il.BeginFinallyBlock()
    # Independent cleanup blocks: one throwing Free never skips later pins or
    # the actual SafeHandle borrow release. No uncertain Free is retried.
    for($i=0;$i -lt 1;$i++){
      $skip=$il.DefineLabel();$null=$il.BeginExceptionBlock()
      $callAbsent=$il.DefineLabel()
      Emit-Cli39PipePeerContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($i*2))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callAbsent)
      Emit-Cli39PipePeerContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($i*2+1))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      $il.MarkLabel($callAbsent)
      Emit-Cli39PipePeerContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($i*4+1))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      Emit-Cli39PipePeerMark32 7 ($i*4+2)
      Emit-Cli39PipePeerContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
      Emit-Cli39PipePeerMark32 7 ($i*4+3);$il.MarkLabel($skip)
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
      Emit-Cli39PipePeerError32 ($i+1);$il.EndExceptionBlock()
    }
    $skipRelease=$il.DefineLabel();$null=$il.BeginExceptionBlock()
    for($queryIndex=0;$queryIndex -lt 1;$queryIndex++){
      $callAbsent=$il.DefineLabel()
      Emit-Cli39PipePeerContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($queryIndex*2))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callAbsent)
      Emit-Cli39PipePeerContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($queryIndex*2+1))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skipRelease)
      $il.MarkLabel($callAbsent)
    }
    Emit-Cli39PipePeerContext32 4 ([bool[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_U1);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skipRelease)
    Emit-Cli39PipePeerMark32 6 2
    Emit-Cli39PipePeerContext32 0 ([Microsoft.Win32.SafeHandles.SafePipeHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Runtime.InteropServices.SafeHandle].GetMethod('DangerousRelease',[Type[]]@()))
    Emit-Cli39PipePeerMark32 6 3;$il.MarkLabel($skipRelease)
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
    Emit-Cli39PipePeerError32 2;$il.EndExceptionBlock()
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal)
    $w.threadOriginal.IsBackground=$false
    return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}

function Invoke-Cli39RootPipePeer32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$p=$w.hostOwnerOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($p.inputRead.value.rootPipePeerOriginal,$w) -or
      -not [object]::ReferenceEquals($p.server,$w.subjectOriginal) -or
      -not [object]::ReferenceEquals($w.subjectOriginal.SafePipeHandle,$w.handleOriginal) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      -not $w.constructorTerminal -or $null -ne $w.constructorFailure -or $w.startAttempted){
    throw 'SAME once-only control-server peer query worker required.'
  }
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline forbids control-server peer query.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.context);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Uncertain actual control-server peer query join remains retained.'}
    $w.pendingActive=$true;$o=$w.pending
    $o.attempted=$true;$o.returned=$false;$o.value=$null;$o.exception=$null
    try{$o.value=$w.threadOriginal.Join(0);$o.returned=$true}catch{$o.exception=$_;$w.joinException=$_;throw}
    if($o.value){$w.terminalJoined=$true;$w.pendingActive=$false}
    else{
      $w.repeatedFalse.returned=$true
      if($w.repeatedFalse.repeats -eq [long]::MaxValue){$w.multiplicityUnknown=$true}
      else{$w.repeatedFalse.repeats++;$w.pendingActive=$false}
      $null=[Threading.Thread]::Yield()
    }
  }
  $w.completionAttempted=$true
  if(-not (Test-Cli39RootPipePeerClosure32 $p)){throw 'Actual control-server peer query cleanup is incomplete.'}
  foreach($exception in $w.exceptionsOriginal){if($null -ne $exception){throw $exception}}
  for($i=0;$i -lt 1;$i++){
    if($w.callMarkers[$i*2] -ne 1 -or $w.callMarkers[$i*2+1] -ne 1 -or
        $w.callMarkers[2] -ne 1 -or $w.resultsOriginal[$i] -eq 0){throw 'Actual control-server peer query failed; raw BOOL/error retained.'}
  }
  $w.clientPidObserved=[BitConverter]::ToUInt32($w.buffersOriginal[0],0)
  $w.rawReturned=$true;$w.completionReturned=$true
  # Actual client PID is not an authenticated process/token/capability original.
  return $w
}

function Test-Cli39RootPipePeerClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$capture=$p.inputRead.value
  if($null -eq $capture -or -not $capture.ContainsKey('rootPipePeerOriginal')){return $true}
  $w=$capture.rootPipePeerOriginal
  if($null -eq $w -or -not [object]::ReferenceEquals($w.hostOwnerOriginal,$p) -or
      -not $w.constructorTerminal){return $false}
  # No Start dispatch means this fixed body acquired no pipe borrow or pin.
  # Constructor partials/failure remain charged to actual host-process exit.
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or $w.pendingActive -or
      $w.multiplicityUnknown -or $null -ne $w.joinException){return $false}
  if($w.borrowMarkers[0] -eq 1 -and $w.borrowMarkers[1] -ne 1){return $false}
  if($w.callMarkers[0] -eq 1 -and $w.callMarkers[1] -ne 1){return $false}
  if($w.borrowOriginal[0] -and ($w.borrowMarkers[2] -ne 1 -or $w.borrowMarkers[3] -ne 1)){return $false}
  for($i=0;$i -lt 1;$i++){
    if($w.pinMarkers[$i*4] -eq 1 -and $w.pinMarkers[$i*4+1] -ne 1){return $false}
    if($w.pinMarkers[$i*4+1] -eq 1 -and
        ($w.pinMarkers[$i*4+2] -ne 1 -or $w.pinMarkers[$i*4+3] -ne 1)){return $false}
    if($null -ne $w.exceptionsOriginal[$i+1]){return $false}
  }
  return ($null -eq $w.exceptionsOriginal[2])
}
