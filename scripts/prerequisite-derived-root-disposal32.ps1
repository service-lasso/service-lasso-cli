# Fixed independent disposal worker, preowned before any derived file effect.
function New-Cli39DerivedRootDisposalWorker32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal
  $d=@{bookOriginal=$b;resourcesOriginal=$b.resourcesOriginal;markersOriginal=$null;exceptionsOriginal=$null;
    methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;contextOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinException=$null;joinFalseCount=0L;joinMultiplicityUnknown=$false}
  $b.disposalWorkerOriginal=$d
  try{
    $d.markersOriginal=[int[]]::new(8);$d.exceptionsOriginal=[object[]]::new(4)
    $d.contextOriginal=[object[]]@($d.resourcesOriginal,$d.markersOriginal,$d.exceptionsOriginal)
    $d.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedDerivedRootDispose',[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$d.methodOriginal.GetILGenerator();$d.ilOriginal=$il;$failureLocal=$il.DeclareLocal([Exception])
    function Emit-CliDerivedDisposeContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    # Each actual resource has its own try/catch and once-only attempt marker;
    # one disposal failure cannot skip the other genuinely eligible originals.
    foreach($index in @(3,1,0,2)){
      $skip=$il.DefineLabel();Emit-CliDerivedDisposeContext32 0 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      $null=$il.BeginExceptionBlock()
      Emit-CliDerivedDisposeContext32 1 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,2*$index);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      Emit-CliDerivedDisposeContext32 0 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[IDisposable])
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[IDisposable].GetMethod('Dispose',[Type[]]@()))
      Emit-CliDerivedDisposeContext32 1 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,2*$index+1);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
      Emit-CliDerivedDisposeContext32 2 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$index);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
      $il.EndExceptionBlock();$il.MarkLabel($skip)
    }
    $il.Emit([Reflection.Emit.OpCodes]::Ret);$d.bodyOriginal=$d.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $d.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$d.bodyOriginal,[int]$b.profileOriginal.cliDerivedRootWorkerStackBytes)
    $d.threadOriginal.IsBackground=$false;return $d
  }catch{$d.constructorFailure=$_;throw}finally{$d.constructorTerminal=$true}
}

function Invoke-Cli39DerivedRootDisposal32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$p=$b.hostOwnerOriginal;$w=$b.writeWorkerOriginal;$d=$b.disposalWorkerOriginal
  if(-not [object]::ReferenceEquals($p.inputRead.value.derivedRootOriginal,$b) -or $null -ne $b.disposalOriginal){throw 'SAME once-only derived-root disposal required.'}
  $b.disposalOriginal=@{workerOriginal=$d;pendingBorrow=$false;unknownAcquisition=$false;returned=$false;exceptionOriginal=$null}
  $disposition=$b.disposalOriginal
  if(-not $b.constructorTerminal -or ($null -ne $w -and (-not $w.constructorTerminal -or
      ($w.startAttempted -and (-not $w.terminalJoined -or $w.threadOriginal.IsAlive -or $null -ne $w.joinException -or $w.joinMultiplicityUnknown))))){
    $disposition.pendingBorrow=$true;throw 'Actual serializer/file worker still borrows derived originals.'
  }
  if($null -ne $b.memberOriginal -and -not (Test-Cli39RootMemberFileQueryClosure32 $p $b.memberOriginal)){
    $disposition.pendingBorrow=$true;throw 'Actual derived file query still borrows SAME held stream.'
  }
  if($null -ne $b.childInvocationOriginal){
    $child=$b.childInvocationOriginal;$preparation=$child.preparationOriginal;$native=$preparation.nativeState
    if(-not $child.constructorTerminal){$disposition.pendingBorrow=$true;throw 'Actual child constructor remains active.'}
    if(-not $child.issueAttempted){
      # The fixed preparation constructor enrolls managed receiver metadata only.
      # Retain that original separately; it must not have entered native issue.
      if($null -ne $preparation -and ($preparation.nativeIssued -or $null -ne $native)){
        $disposition.pendingBorrow=$true;throw 'Never-issued child has inconsistent actual native preparation.'
      }
      $disposition.unissuedPreparationOriginal=$preparation
    }elseif($null -eq $native -or -not $native.released -or -not $native.rootObserved -or -not $native.treeObserved -or
        -not $native.streams.stdout.eof -or -not $native.streams.stderr.eof -or
        ($native.retainedHost -and -not $native.callerControl.terminal)){
      $disposition.pendingBorrow=$true;throw 'Actual child/Job/control/EOF/physical owner closure required before held root disposal.'
    }
  }
  # Constructor exceptions before a resource was published are not no-effect
  # evidence for FileStream or cryptographic provider acquisition.
  if($b.markersOriginal[5] -eq 1 -and $b.markersOriginal[6] -ne 1 -or
      $b.markersOriginal[12] -eq 1 -and $b.markersOriginal[13] -ne 1){
    $disposition.unknownAcquisition=$true
  }
  if($null -eq $d -or -not $d.constructorTerminal -or $null -ne $d.constructorFailure -or $d.startAttempted){throw 'Actual preowned once-only disposal worker required.'}
  $d.startAttempted=$true
  try{$d.threadOriginal.Start($d.contextOriginal);$d.startReturned=$true}catch{$d.startException=$_;$disposition.exceptionOriginal=$_;throw}
  while(-not $d.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($null -ne $d.joinException -or $d.joinMultiplicityUnknown){throw 'Actual disposal join remains unknown.'}
    try{$joined=$d.threadOriginal.Join(0)}catch{$d.joinException=$_;$disposition.exceptionOriginal=$_;throw}
    if($joined){$d.terminalJoined=$true}else{
      if($d.joinFalseCount -eq [long]::MaxValue){$d.joinMultiplicityUnknown=$true}else{$d.joinFalseCount++}
      $null=[Threading.Thread]::Yield()
    }
  }
  foreach($index in 0..3){
    if($null -ne $d.resourcesOriginal[$index] -and ($d.markersOriginal[2*$index] -ne 1 -or
        $d.markersOriginal[2*$index+1] -ne 1 -or $null -ne $d.exceptionsOriginal[$index])){throw 'Actual independent derived resource disposal incomplete.'}
  }
  if($disposition.unknownAcquisition){throw 'Known resources independently disposed; unknown original file/hash constructor effect remains retained.'}
  $disposition.returned=$true;return $disposition
}

function Test-Cli39DerivedRootClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $root=$HostOwnerOriginal.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('derivedRootOriginal')){return $true}
  $b=$root.derivedRootOriginal;$w=$b.writeWorkerOriginal
  if(-not [object]::ReferenceEquals($b.hostOwnerOriginal,$HostOwnerOriginal) -or -not $b.constructorTerminal){return $false}
  $d=$b.disposalWorkerOriginal
  if($null -ne $d -and $d.startAttempted -and (-not $d.terminalJoined -or
      $null -eq $d.threadOriginal -or $d.threadOriginal.IsAlive -or
      $null -ne $d.joinException -or $d.joinMultiplicityUnknown)){return $false}
  if($null -eq $w){return $true}
  if(-not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if(-not $w.terminalJoined -or $w.threadOriginal.IsAlive -or $null -ne $w.joinException -or $w.joinMultiplicityUnknown){return $false}
  if($null -eq $b.disposalOriginal -or -not $b.disposalOriginal.returned -or
      $b.disposalOriginal.pendingBorrow -or $b.disposalOriginal.unknownAcquisition){return $false}
  return $true
}
