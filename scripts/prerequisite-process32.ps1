# Fixed raw current-process observations. Not principal/capability admission.
function New-Cli39RootProcessWorker32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$m=$root.nativeQueryOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $m.complete -or $root.ContainsKey('rootProcessOriginal') -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only root process observation required.'}
  $w=@{hostOwnerOriginal=$p;rootCaptureOriginal=$root;queryOriginal=$m;
    timesOriginal=$null;pinOriginal=$null;pseudoHandleOriginal=$null;pidOriginal=$null;
    resultsOriginal=$null;errorsOriginal=$null;markersOriginal=$null;exceptionsOriginal=$null;
    context=$null;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;pendingActive=$false;multiplicityUnknown=$false;joinException=$null;
    pending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    repeatedFalse=@{returned=$false;repeats=0L};completionAttempted=$false;rawReturned=$false}
  $root.rootProcessOriginal=$w
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids process worker construction.'}
    $w.timesOriginal=[byte[]]::new(32);$w.pinOriginal=[Runtime.InteropServices.GCHandle[]]::new(1)
    $w.pseudoHandleOriginal=[IntPtr[]]::new(1);$w.pidOriginal=[uint32[]]::new(1)
    $w.resultsOriginal=[int[]]::new(1);$w.errorsOriginal=[int[]]::new(1)
    $w.markersOriginal=[int[]]::new(11);$w.exceptionsOriginal=[object[]]::new(2)
    $w.context=[object[]]@($m.delegatesOriginal[11],$m.delegatesOriginal[10],$m.delegatesOriginal[4],
      $w.timesOriginal,$w.pinOriginal,$w.pseudoHandleOriginal,$w.pidOriginal,
      $w.resultsOriginal,$w.errorsOriginal,$w.markersOriginal,$w.exceptionsOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedCurrentProcess',[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $failureLocal=$il.DeclareLocal([Exception]);$pointerLocal=$il.DeclareLocal([IntPtr]);$resultLocal=$il.DeclareLocal([int])
    function Emit-Cli39ProcessContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39ProcessMark32([int]$Index){
      Emit-Cli39ProcessContext32 9 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39ProcessFailure32([int]$Index){
      Emit-Cli39ProcessContext32 10 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    Emit-Cli39ProcessMark32 0
    Emit-Cli39ProcessContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39ProcessContext32 0 $m.delegateTypesOriginal[11]
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$m.delegateTypesOriginal[11].GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I);Emit-Cli39ProcessMark32 1
    Emit-Cli39ProcessMark32 2
    Emit-Cli39ProcessContext32 6 ([uint32[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39ProcessContext32 1 $m.delegateTypesOriginal[10]
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$m.delegateTypesOriginal[10].GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ProcessMark32 3
    Emit-Cli39ProcessMark32 4
    Emit-Cli39ProcessContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39ProcessContext32 3 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-Cli39ProcessMark32 5
    Emit-Cli39ProcessContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$pointerLocal)
    Emit-Cli39ProcessMark32 6
    Emit-Cli39ProcessContext32 2 $m.delegateTypesOriginal[4]
    Emit-Cli39ProcessContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
    foreach($offset in @(0,8,16,24)){
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$offset)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[IntPtr].GetMethod('Add',[Type[]]@([IntPtr],[int])))
    }
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$m.delegateTypesOriginal[4].GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$resultLocal)
    Emit-Cli39ProcessContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$resultLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    Emit-Cli39ProcessMark32 10
    Emit-Cli39ProcessContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ProcessMark32 7
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39ProcessFailure32 0
    $il.BeginFinallyBlock();$skip=$il.DefineLabel();$null=$il.BeginExceptionBlock()
    $callKnown=$il.DefineLabel()
    Emit-Cli39ProcessContext32 9 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_6)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callKnown)
    Emit-Cli39ProcessContext32 9 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,10)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
    $il.MarkLabel($callKnown)
    Emit-Cli39ProcessContext32 9 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_5)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
    Emit-Cli39ProcessMark32 8
    Emit-Cli39ProcessContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
    Emit-Cli39ProcessMark32 9;$il.MarkLabel($skip)
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39ProcessFailure32 1
    $il.EndExceptionBlock();$il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal)
    $w.threadOriginal.IsBackground=$false;return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}

function Test-Cli39RootProcessClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('rootProcessOriginal')){return $true}
  $w=$root.rootProcessOriginal
  if(-not [object]::ReferenceEquals($w.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($w.rootCaptureOriginal,$root) -or -not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if(-not $w.terminalJoined -or $null -eq $w.threadOriginal -or $w.threadOriginal.IsAlive -or
      $w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){return $false}
  if($w.markersOriginal[4] -eq 1 -and $w.markersOriginal[5] -ne 1){return $false}
  if($w.markersOriginal[6] -eq 1 -and $w.markersOriginal[10] -ne 1){return $false}
  if($w.markersOriginal[5] -eq 1 -and ($w.markersOriginal[8] -ne 1 -or $w.markersOriginal[9] -ne 1 -or
      $w.pinOriginal[0].IsAllocated -or $null -ne $w.exceptionsOriginal[1])){return $false}
  return $true
}

function Invoke-Cli39RootProcess32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$p=$w.hostOwnerOriginal
  if(-not [object]::ReferenceEquals($p.inputRead.value.rootProcessOriginal,$w) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or -not $w.constructorTerminal -or
      $null -ne $w.constructorFailure -or $w.startAttempted){throw 'Actual once-only fixed process worker required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline forbids process query.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.context);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Uncertain actual process join retained.'}
    $w.pendingActive=$true;$o=$w.pending;$o.attempted=$true;$o.returned=$false;$o.value=$null;$o.exception=$null
    try{$o.value=$w.threadOriginal.Join(0);$o.returned=$true}catch{$o.exception=$_;$w.joinException=$_;throw}
    if($o.value){$w.terminalJoined=$true;$w.pendingActive=$false}
    else{$w.repeatedFalse.returned=$true;if($w.repeatedFalse.repeats -eq [long]::MaxValue){$w.multiplicityUnknown=$true}else{$w.repeatedFalse.repeats++;$w.pendingActive=$false};$null=[Threading.Thread]::Yield()}
  }
  $w.completionAttempted=$true
  if(-not (Test-Cli39RootProcessClosure32 $p)){throw 'Actual process pin/worker closure incomplete.'}
  foreach($failureOriginal in $w.exceptionsOriginal){if($null -ne $failureOriginal){throw $failureOriginal}}
  foreach($marker in @(0,1,2,3,4,5,6,7,8,9)){if($w.markersOriginal[$marker] -ne 1){throw 'Actual complete process observations required.'}}
  if($w.pidOriginal[0] -eq 0 -or $w.resultsOriginal[0] -eq 0 -or $w.pseudoHandleOriginal[0] -ne [IntPtr]::new(-1)){throw 'Actual native process query failed/inconsistent; raw outputs retained.'}
  $w.rawReturned=$true;return $w
}
