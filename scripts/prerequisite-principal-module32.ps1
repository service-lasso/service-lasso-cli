# Fixed Advapi32 module recipes. All partial CLR graphs remain host-owned.
function Initialize-Cli39PrincipalRecipes32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if($p.ContainsKey('principalRecipesOriginal')){throw 'Principal startup recipes cannot retry.'}
  $book=@{hostOwnerOriginal=$p;typeOriginal=$null;openTokenTypeOriginal=$null;recipesOriginal=$null;
    terminalLifetime='host-process';constructionTerminal=$false;returned=$false;exceptionOriginal=$null}
  $p.principalRecipesOriginal=$book
  try {
    $t=@{assemblyName=$null;assembly=$null;module=$null;builder=$null;constructor=$null;
      invoke=$null;attribute=$null;parametersOriginal=$null;type=$null;returned=$false}
    $book.typeOriginal=$t
    $t.parametersOriginal=[Type[]]@([IntPtr],[int],[IntPtr],[uint32],[IntPtr])
    $t.assemblyName=[Reflection.AssemblyName]::new('Cli39.FixedPrincipalApi')
    $t.assembly=[Reflection.Emit.AssemblyBuilder]::DefineDynamicAssembly($t.assemblyName,[Reflection.Emit.AssemblyBuilderAccess]::Run)
    $t.module=$t.assembly.DefineDynamicModule('FixedPrincipal')
    $t.builder=$t.module.DefineType('GetTokenInformation',[Reflection.TypeAttributes]'Class,Public,Sealed',[MulticastDelegate])
    $t.constructor=$t.builder.DefineConstructor([Reflection.MethodAttributes]'RTSpecialName,HideBySig,Public',[Reflection.CallingConventions]::Standard,[Type[]]@([object],[IntPtr]))
    $t.constructor.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
    $t.invoke=$t.builder.DefineMethod('Invoke',[Reflection.MethodAttributes]'Public,HideBySig,NewSlot,Virtual',[int],$t.parametersOriginal)
    $t.invoke.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
    $t.attribute=[Reflection.Emit.CustomAttributeBuilder]::new(
      [Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetConstructor([Type[]]@([Runtime.InteropServices.CallingConvention])),
      [object[]]@([Runtime.InteropServices.CallingConvention]::Winapi),
      [Reflection.FieldInfo[]]@([Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetField('SetLastError')),[object[]]@($true))
    $t.builder.SetCustomAttribute($t.attribute);$t.type=$t.builder.CreateType();$t.returned=$true
    # The second literal signature belongs to this SAME pre-NPH resident graph.
    $ot=@{builder=$null;constructor=$null;invoke=$null;attribute=$null;type=$null;returned=$false}
    $book.openTokenTypeOriginal=$ot
    $ot.builder=$t.module.DefineType('OpenProcessToken',[Reflection.TypeAttributes]'Class,Public,Sealed',[MulticastDelegate])
    $ot.constructor=$ot.builder.DefineConstructor([Reflection.MethodAttributes]'RTSpecialName,HideBySig,Public',[Reflection.CallingConventions]::Standard,[Type[]]@([object],[IntPtr]))
    $ot.constructor.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
    $ot.invoke=$ot.builder.DefineMethod('Invoke',[Reflection.MethodAttributes]'Public,HideBySig,NewSlot,Virtual',[int],[Type[]]@([IntPtr],[uint32],[IntPtr]))
    $ot.invoke.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
    $ot.attribute=$t.attribute;$ot.builder.SetCustomAttribute($ot.attribute)
    $ot.type=$ot.builder.CreateType();$ot.returned=$true
    $book.recipesOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach($phase in @('load','export','delegate','open-export','open-delegate','token-user','token-statistics','child-token-user','child-token-statistics')){
      $r=@{phaseOriginal=$phase;bookOriginal=$book;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;
        constructorTerminal=$false;returned=$false;exceptionOriginal=$null}
      $book.recipesOriginal.Add($phase,$r)
      try {
        $r.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedPrincipal'+$phase,[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
        $il=$r.methodOriginal.GetILGenerator();$r.ilOriginal=$il;$failureLocal=$il.DeclareLocal([Exception])
        function Emit-Cli39PrincipalModuleContext32([int]$Index,[Type]$Type){
          $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
        }
        function Emit-Cli39PrincipalModuleMark32([int]$Index){
          Emit-Cli39PrincipalModuleContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
        }
        $null=$il.BeginExceptionBlock();Emit-Cli39PrincipalModuleMark32 0
        if($phase -ceq 'load'){
          Emit-Cli39PrincipalModuleContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          Emit-Cli39PrincipalModuleContext32 0 ([string])
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.NativeLibrary].GetMethod('Load',[Type[]]@([string])))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
        }elseif($phase -cin @('export','open-export')){
          Emit-Cli39PrincipalModuleContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$(if($phase -cin @('open-export','open-delegate')){2}else{1}))
          Emit-Cli39PrincipalModuleContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
          if($phase -ceq 'open-export'){$il.Emit([Reflection.Emit.OpCodes]::Ldstr,'OpenProcessToken')}else{$il.Emit([Reflection.Emit.OpCodes]::Ldstr,'GetTokenInformation')}
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.NativeLibrary].GetMethod('GetExport',[Type[]]@([IntPtr],[string])))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
        }elseif($phase -cin @('delegate','open-delegate')){
          Emit-Cli39PrincipalModuleContext32 3 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$(if($phase -ceq 'open-delegate'){1}else{0}))
          Emit-Cli39PrincipalModuleContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$(if($phase -cin @('open-export','open-delegate')){2}else{1}));$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
          Emit-Cli39PrincipalModuleContext32 1 ([Type])
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetDelegateForFunctionPointer',[Type[]]@([IntPtr],[Type])))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
        }else{
          $informationClass=1;$informationBytes=84
          if($phase -cin @('token-statistics','child-token-statistics')){$informationClass=10;$informationBytes=56}
          Emit-Cli39PrincipalModuleMark32 1
          Emit-Cli39PrincipalModuleContext32 2 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          Emit-Cli39PrincipalModuleContext32 1 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-Cli39PrincipalModuleMark32 2
          Emit-Cli39PrincipalModuleContext32 3 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          Emit-Cli39PrincipalModuleContext32 2 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I);Emit-Cli39PrincipalModuleMark32 3
          Emit-Cli39PrincipalModuleContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          Emit-Cli39PrincipalModuleContext32 0 $t.type
          if($phase -cin @('child-token-user','child-token-statistics')){
            Emit-Cli39PrincipalModuleContext32 7 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
          }else{
            $il.Emit([Reflection.Emit.OpCodes]::Ldc_I8,[long]-4);$il.Emit([Reflection.Emit.OpCodes]::Newobj,[IntPtr].GetConstructor([Type[]]@([long])))
          }
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$informationClass)
          Emit-Cli39PrincipalModuleContext32 3 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$informationBytes)
          Emit-Cli39PrincipalModuleContext32 3 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$informationBytes);$il.Emit([Reflection.Emit.OpCodes]::Call,[IntPtr].GetMethod('Add',[Type[]]@([IntPtr],[int])))
          $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$t.type.GetMethod('Invoke'))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39PrincipalModuleMark32 4
          Emit-Cli39PrincipalModuleContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39PrincipalModuleMark32 5
        }
        if($phase -cin @('load','export','delegate','open-export','open-delegate')){Emit-Cli39PrincipalModuleMark32 1}
        $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
        Emit-Cli39PrincipalModuleContext32 5 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
        $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
        $il.BeginFinallyBlock()
        if($phase -cin @('token-user','token-statistics','child-token-user','child-token-statistics')){
          $skipFree=$il.DefineLabel();$callAbsent=$il.DefineLabel();$null=$il.BeginExceptionBlock()
          Emit-Cli39PrincipalModuleContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3)
          $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callAbsent)
          Emit-Cli39PrincipalModuleContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4)
          $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skipFree);$il.MarkLabel($callAbsent)
          Emit-Cli39PrincipalModuleContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
          $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skipFree)
          Emit-Cli39PrincipalModuleMark32 6
          Emit-Cli39PrincipalModuleContext32 2 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
          Emit-Cli39PrincipalModuleMark32 7;$il.MarkLabel($skipFree)
          $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
          Emit-Cli39PrincipalModuleContext32 5 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
          $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
          $il.EndExceptionBlock();Emit-Cli39PrincipalModuleMark32 8
        }else{Emit-Cli39PrincipalModuleMark32 2}
        $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
        $r.bodyOriginal=$r.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart]);$r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}finally{$r.constructorTerminal=$true}
    }
    $book.returned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}finally{$book.constructionTerminal=$true}
}

function New-Cli39PrincipalModule32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$recipes=$p.principalRecipesOriginal
  $selector=$root.principalSelectorOriginal
  if($root.ContainsKey('principalModuleOriginal') -or -not $selector.bindingReturned -or
      -not $recipes.returned -or -not $recipes.constructionTerminal -or
      -not [object]::ReferenceEquals($recipes.hostOwnerOriginal,$p) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal){
    throw 'SAME once-only held principal API module and startup recipes required.'
  }
  $m=@{kind='cli39.principal-resident-module.v1';hostOwnerOriginal=$p;rootCaptureOriginal=$root;
    selectorOriginal=$selector;subjectOriginal=$selector.subjectOriginal;recipesOriginal=$recipes;
    absoluteDeadlineTicks=$p.originalDeadlineTicks;terminalLifetime='host-process';
    moduleAndExportsOriginal=$null;delegatesOriginal=$null;workersOriginal=$null;nextPhaseOriginal=0;tokenBookOriginal=$null;
    constructionTerminal=$false;constructorFailure=$null;returned=$false;exceptionOriginal=$null;
    receiverKey=('cli39-principal-module-'+[Guid]::NewGuid().ToString('N'))}
  $root.principalModuleOriginal=$m;$p.receiver.frames.Add($m.receiverKey,$m)
  try {
    $m.moduleAndExportsOriginal=[IntPtr[]]::new(3);$m.delegatesOriginal=[object[]]::new(2)
    $m.workersOriginal=[Collections.Generic.List[object]]::new(5)
    foreach($phase in @('load','export','delegate','open-export','open-delegate')){
      $w=@{moduleOriginal=$m;phaseOriginal=$phase;recipeOriginal=$recipes.recipesOriginal[$phase];
        markersOriginal=$null;exceptionsOriginal=$null;contextOriginal=$null;threadOriginal=$null;
        constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
        terminalJoined=$false;pendingActive=$false;multiplicityUnknown=$false;joinException=$null;
        pendingOriginal=@{attempted=$false;returned=$false;value=$null;exceptionOriginal=$null};
        repeatedFalseOriginal=@{returned=$false;repeats=0L};returned=$false}
      $m.workersOriginal.Add($w)
      try {
        $w.markersOriginal=[int[]]::new(3);$w.exceptionsOriginal=[object[]]::new(1)
        $w.contextOriginal=[object[]]@($m.subjectOriginal.path,$(if($phase -ceq 'open-delegate'){$recipes.openTokenTypeOriginal.type}else{$recipes.typeOriginal.type}),
          $m.moduleAndExportsOriginal,$m.delegatesOriginal,$w.markersOriginal,$w.exceptionsOriginal)
        $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.recipeOriginal.bodyOriginal)
        $w.threadOriginal.IsBackground=$false
      }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
    }
    return $m
  }catch{$m.constructorFailure=$_;throw}finally{$m.constructionTerminal=$true}
}

function Test-Cli39PrincipalRecipesTerminal32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if(-not $p.ContainsKey('principalRecipesOriginal')){return $true}
  $book=$p.principalRecipesOriginal
  if(-not [object]::ReferenceEquals($book.hostOwnerOriginal,$p) -or
      -not $book.constructionTerminal -or $book.terminalLifetime -cne 'host-process'){return $false}
  foreach($recipe in $book.recipesOriginal.Values){
    if(-not [object]::ReferenceEquals($recipe.bookOriginal,$book) -or -not $recipe.constructorTerminal){return $false}
  }
  return $true
}

function Invoke-Cli39PrincipalModulePhase32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$m=$w.moduleOriginal;$p=$m.hostOwnerOriginal;$root=$p.inputRead.value
  if(-not [object]::ReferenceEquals($root.principalModuleOriginal,$m) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$m.receiverKey],$m) -or
      -not $m.constructionTerminal -or $null -ne $m.constructorFailure -or
      $m.workersOriginal.Count -ne 5 -or $m.nextPhaseOriginal -ge 5 -or
      -not [object]::ReferenceEquals($m.workersOriginal[$m.nextPhaseOriginal],$w) -or
      -not [object]::ReferenceEquals($w.recipeOriginal,$m.recipesOriginal.recipesOriginal[$w.phaseOriginal]) -or
      $w.contextOriginal[0] -cne $m.subjectOriginal.path -or
      $w.contextOriginal[1] -ne $(if($w.phaseOriginal -ceq 'open-delegate'){$m.recipesOriginal.openTokenTypeOriginal.type}else{$m.recipesOriginal.typeOriginal.type}) -or
      -not [object]::ReferenceEquals($w.contextOriginal[2],$m.moduleAndExportsOriginal) -or
      -not [object]::ReferenceEquals($w.contextOriginal[3],$m.delegatesOriginal) -or
      -not $w.constructorTerminal -or $null -ne $w.constructorFailure -or $w.startAttempted -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal){throw 'SAME once-only ordered principal module phase required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline denies new principal module phase.'}
  if($m.nextPhaseOriginal -gt 0 -and -not $m.workersOriginal[$m.nextPhaseOriginal-1].returned){throw 'Actual preceding phase required.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Unknown principal module worker remains retained.'}
    $w.pendingActive=$true;$observation=$w.pendingOriginal;$observation.attempted=$true;$observation.returned=$false;$observation.value=$null
    try{$observation.value=$w.threadOriginal.Join(0);$observation.returned=$true}catch{$observation.exceptionOriginal=$_;$w.joinException=$_;throw}
    if($observation.value){$w.terminalJoined=$true;$w.pendingActive=$false}
    else{$w.repeatedFalseOriginal.returned=$true;if($w.repeatedFalseOriginal.repeats -eq [long]::MaxValue){$w.multiplicityUnknown=$true}else{$w.repeatedFalseOriginal.repeats++;$w.pendingActive=$false};$null=[Threading.Thread]::Yield()}
  }
  foreach($failure in $w.exceptionsOriginal){if($null -ne $failure){$m.exceptionOriginal=$failure;throw $failure}}
  if($w.markersOriginal[0] -ne 1 -or $w.markersOriginal[1] -ne 1 -or $w.markersOriginal[2] -ne 1){throw 'Complete actual principal module phase markers required.'}
  if($w.phaseOriginal -ceq 'load' -and $m.moduleAndExportsOriginal[0] -eq [IntPtr]::Zero){throw 'Actual module reference is zero.'}
  if($w.phaseOriginal -ceq 'export' -and $m.moduleAndExportsOriginal[1] -eq [IntPtr]::Zero){throw 'Actual fixed export is zero.'}
  if($w.phaseOriginal -ceq 'delegate' -and ($null -eq $m.delegatesOriginal[0] -or
      $m.delegatesOriginal[0].GetType() -ne $m.recipesOriginal.typeOriginal.type)){throw 'Actual fixed principal delegate type differs.'}
  if($w.phaseOriginal -ceq 'open-export' -and $m.moduleAndExportsOriginal[2] -eq [IntPtr]::Zero){throw 'Actual fixed OpenProcessToken export is zero.'}
  if($w.phaseOriginal -ceq 'open-delegate' -and ($null -eq $m.delegatesOriginal[1] -or
      $m.delegatesOriginal[1].GetType() -ne $m.recipesOriginal.openTokenTypeOriginal.type)){throw 'Actual fixed OpenProcessToken delegate type differs.'}
  $w.returned=$true;$m.nextPhaseOriginal++
  if($m.nextPhaseOriginal -eq 5){$m.returned=$true}
  return $w
}

function Test-Cli39PrincipalModuleClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$ModuleOriginal)
  $p=$HostOwnerOriginal;$m=$ModuleOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not [object]::ReferenceEquals($root.principalModuleOriginal,$m) -or
      -not [object]::ReferenceEquals($m.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($m.rootCaptureOriginal,$root) -or
      -not [object]::ReferenceEquals($m.selectorOriginal,$root.principalSelectorOriginal) -or
      -not [object]::ReferenceEquals($m.subjectOriginal,$m.selectorOriginal.subjectOriginal) -or
      -not [object]::ReferenceEquals($m.recipesOriginal,$p.principalRecipesOriginal) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$m.receiverKey],$m) -or
      $m.kind -cne 'cli39.principal-resident-module.v1' -or $m.terminalLifetime -cne 'host-process' -or
      $m.absoluteDeadlineTicks -ne $p.originalDeadlineTicks -or -not $m.constructionTerminal){return $false}
  foreach($w in $m.workersOriginal){
    if(-not [object]::ReferenceEquals($w.moduleOriginal,$m) -or -not $w.constructorTerminal){return $false}
    if(-not $w.startAttempted){if($null -ne $w.threadOriginal -and $w.threadOriginal.IsAlive){return $false};continue}
    if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
        $w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException -or
        $w.markersOriginal[2] -ne 1){return $false}
  }
  if($null -ne $m.tokenBookOriginal -and -not (Test-Cli39PrincipalTokenClosure32 $p $m.tokenBookOriginal)){return $false}
  # This is phase quiescence only. No NativeLibrary.Free: raw references,
  # failed-load unknowns and partial delegates remain resident/charged until
  # the controlling outer owner witnesses actual host exit.
  return $true
}

function Test-Cli39RootPrincipalModuleClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.ContainsKey('principalModuleOriginal')){return $true}
  return Test-Cli39PrincipalModuleClosure32 $p $root.principalModuleOriginal
}
