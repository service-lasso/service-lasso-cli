# Exact source-selected expected image extent, never a guessed path maximum.
function New-Cli39RootImageWorker32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$m=$root.nativeQueryOriginal;$process=$root.rootProcessOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $m.complete -or -not $process.rawReturned -or
      -not (Test-Cli39RootProcessClosure32 $p) -or $root.ContainsKey('rootImageOriginal') -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only observed process/image required.'}
  $w=@{hostOwnerOriginal=$p;rootCaptureOriginal=$root;queryOriginal=$m;processOriginal=$process;
    expectedPathOriginal=$null;expectedCharactersOriginal=$null;expectedBytesOriginal=$null;characterCapacity=0;lengthOffset=0;backingOriginal=$null;
    pinOriginal=$null;resultsOriginal=$null;errorsOriginal=$null;markersOriginal=$null;exceptionsOriginal=$null;
    context=$null;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;pendingActive=$false;multiplicityUnknown=$false;joinException=$null;
    pending=@{attempted=$false;returned=$false;value=$null;exception=$null};repeatedFalse=@{returned=$false;repeats=0L};
    completionAttempted=$false;rawReturned=$false;returnedCharacters=0;comparedBytes=0;pathBytesEqual=$false;byteMismatchObserved=$false}
  $root.rootImageOriginal=$w
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids image worker construction.'}
    $w.expectedPathOriginal=$root.fieldMapOriginal['hostPath'].GetString()
    if([string]::IsNullOrEmpty($w.expectedPathOriginal) -or $w.expectedPathOriginal.Contains([char]0)){throw 'Actual selected image path required.'}
    [long]$capacity=[long]$w.expectedPathOriginal.Length+1
    [long]$offset=([long](2*$capacity)+3) -band (-4L)
    if($offset -gt ([int]::MaxValue-4)){throw 'Actual selected image extent unsupported.'}
    $w.characterCapacity=[int]$capacity;$w.lengthOffset=[int]$offset
    $w.expectedCharactersOriginal=$w.expectedPathOriginal.ToCharArray()
    $w.expectedBytesOriginal=[byte[]]::new(2*$w.expectedCharactersOriginal.Length)
    [Buffer]::BlockCopy($w.expectedCharactersOriginal,0,$w.expectedBytesOriginal,0,$w.expectedBytesOriginal.Length)
    $w.backingOriginal=[byte[]]::new([int]($offset+4))
    [Array]::Copy([BitConverter]::GetBytes([uint32]$capacity),0,$w.backingOriginal,[int]$offset,4)
    $w.pinOriginal=[Runtime.InteropServices.GCHandle[]]::new(1);$w.resultsOriginal=[int[]]::new(1)
    $w.errorsOriginal=[int[]]::new(1);$w.markersOriginal=[int[]]::new(7);$w.exceptionsOriginal=[object[]]::new(2)
    $w.context=[object[]]@($m.delegatesOriginal[5],$process.pseudoHandleOriginal,$w.backingOriginal,
      $w.pinOriginal,$w.resultsOriginal,$w.errorsOriginal,$w.markersOriginal,$w.exceptionsOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedCurrentImage',[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $failureLocal=$il.DeclareLocal([Exception]);$pointerLocal=$il.DeclareLocal([IntPtr]);$resultLocal=$il.DeclareLocal([int])
    function Emit-Cli39ImageContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39ImageMark32([int]$Index){
      Emit-Cli39ImageContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39ImageFailure32([int]$Index){
      Emit-Cli39ImageContext32 7 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock();Emit-Cli39ImageMark32 0
    Emit-Cli39ImageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39ImageContext32 2 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-Cli39ImageMark32 1
    Emit-Cli39ImageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$pointerLocal);Emit-Cli39ImageMark32 2
    Emit-Cli39ImageContext32 0 $m.delegateTypesOriginal[5]
    Emit-Cli39ImageContext32 1 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$w.lengthOffset)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[IntPtr].GetMethod('Add',[Type[]]@([IntPtr],[int])))
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$m.delegateTypesOriginal[5].GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$resultLocal)
    Emit-Cli39ImageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$resultLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    Emit-Cli39ImageMark32 6
    Emit-Cli39ImageContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ImageMark32 3
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39ImageFailure32 0
    $il.BeginFinallyBlock();$skip=$il.DefineLabel();$null=$il.BeginExceptionBlock()
    $callKnown=$il.DefineLabel()
    Emit-Cli39ImageContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callKnown)
    Emit-Cli39ImageContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_6)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
    $il.MarkLabel($callKnown)
    Emit-Cli39ImageContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip);Emit-Cli39ImageMark32 4
    Emit-Cli39ImageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
    Emit-Cli39ImageMark32 5;$il.MarkLabel($skip)
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39ImageFailure32 1
    $il.EndExceptionBlock();$il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal);$w.threadOriginal.IsBackground=$false
    return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}
function Test-Cli39RootImageClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('rootImageOriginal')){return $true}
  $w=$root.rootImageOriginal
  if(-not [object]::ReferenceEquals($w.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($w.rootCaptureOriginal,$root) -or -not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if(-not $w.terminalJoined -or $null -eq $w.threadOriginal -or $w.threadOriginal.IsAlive -or
      $w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){return $false}
  if($w.markersOriginal[0] -eq 1 -and $w.markersOriginal[1] -ne 1){return $false}
  if($w.markersOriginal[2] -eq 1 -and $w.markersOriginal[6] -ne 1){return $false}
  if($w.markersOriginal[1] -eq 1 -and ($w.markersOriginal[4] -ne 1 -or $w.markersOriginal[5] -ne 1 -or
      $w.pinOriginal[0].IsAllocated -or $null -ne $w.exceptionsOriginal[1])){return $false}
  return $true
}
function Invoke-Cli39RootImage32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$p=$w.hostOwnerOriginal
  if(-not [object]::ReferenceEquals($p.inputRead.value.rootImageOriginal,$w) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or -not $w.constructorTerminal -or
      $null -ne $w.constructorFailure -or $w.startAttempted){throw 'Actual once-only image worker required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline forbids image query.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.context);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Uncertain actual image join retained.'}
    $w.pendingActive=$true;$o=$w.pending;$o.attempted=$true;$o.returned=$false;$o.value=$null;$o.exception=$null
    try{$o.value=$w.threadOriginal.Join(0);$o.returned=$true}catch{$o.exception=$_;$w.joinException=$_;throw}
    if($o.value){$w.terminalJoined=$true;$w.pendingActive=$false}
    else{$w.repeatedFalse.returned=$true;if($w.repeatedFalse.repeats -eq [long]::MaxValue){$w.multiplicityUnknown=$true}else{$w.repeatedFalse.repeats++;$w.pendingActive=$false};$null=[Threading.Thread]::Yield()}
  }
  $w.completionAttempted=$true
  $w.returnedCharacters=[BitConverter]::ToUInt32($w.backingOriginal,$w.lengthOffset)
  if(-not (Test-Cli39RootImageClosure32 $p)){throw 'Actual image pin/worker closure incomplete.'}
  foreach($failureOriginal in $w.exceptionsOriginal){if($null -ne $failureOriginal){throw $failureOriginal}}
  foreach($marker in @(0,1,2,3,4,5)){if($w.markersOriginal[$marker] -ne 1){throw 'Actual complete image observations required.'}}
  if($w.resultsOriginal[0] -eq 0 -or $w.returnedCharacters -eq 0 -or
      $w.returnedCharacters -ge $w.characterCapacity){throw 'Actual image query failed/unsupported; raw result/count retained, no retry.'}
  $w.rawReturned=$true
  if(([long]$w.returnedCharacters*2) -eq $w.expectedBytesOriginal.Length){
    for($i=0;$i -lt $w.expectedBytesOriginal.Length;$i++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended image comparison.'}
      $different=($w.backingOriginal[$i] -ne $w.expectedBytesOriginal[$i]);$w.comparedBytes=$i+1
      if($different){$w.byteMismatchObserved=$true;break}
    }
    $w.pathBytesEqual=(-not $w.byteMismatchObserved -and $w.comparedBytes -eq $w.expectedBytesOriginal.Length)
  }
  return $w
}
