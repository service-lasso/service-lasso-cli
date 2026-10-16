# Source-only fixed query ABI. This borrows the SAME resident Core module;
# no Load, FreeLibrary, process/Job owner, callback or supplied signature exists.
# Whole external startup/source/input admission remains required before use.
function New-Cli39NativeOutputQueryWorker32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)]$AbiOriginal)
  $p=$HostOwnerOriginal; $a=$AbiOriginal; $capture=$p.inputRead.value
  if (-not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $null -eq $capture -or -not $capture.structuralReturned -or
      $capture.actionOriginal -cne 'materialize' -or
      $capture.ContainsKey('nativeOutputQueryOriginal') -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -eq $a.originalOwner -or -not $a.originalOwner.constructionTerminal -or
      $a.module -ne $a.originalOwner.module -or
      -not [object]::ReferenceEquals($a.delegates,$a.originalOwner.delegates) -or
      $a.originalOwner.module -eq [IntPtr]::Zero -or [IntPtr]::Size -ne 8) {
    throw 'SAME root owner and already constructed resident Core ABI required.'
  }
  $ownerFound=$false
  foreach($frame in $p.receiver.frames.Values){
    if([object]::ReferenceEquals($frame,$a.originalOwner)){$ownerFound=$true}
  }
  if(-not $ownerFound){throw 'Borrowed module must belong to SAME actual receiver.'}
  $m=@{kind='cli39.fixed-native-output-query-abi.v1';hostOwnerOriginal=$p;
    abiOriginal=$a;moduleOwnerOriginal=$a.originalOwner;module=$a.originalOwner.module;
    terminalLifetime='host-process';constructorAttempted=$false;constructorTerminal=$false;
    constructorFailure=$null;assemblyNameOriginal=$null;assemblyOriginal=$null;
    dynamicModuleOriginal=$null;namesOriginal=$null;signaturesOriginal=$null;
    parameterTypesOriginal=$null;buildersOriginal=$null;
    constructorsOriginal=$null;invokesOriginal=$null;
    attributeConstructorsOriginal=$null;attributeFieldsOriginal=$null;
    attributeArgumentsOriginal=$null;attributeFieldValuesOriginal=$null;
    attributesOriginal=$null;delegateTypesOriginal=$null;
    exportsOriginal=$null;delegatesOriginal=$null;
    workerOriginal=$null;complete=$false}
  # Partial managed construction and every export/delegate stay in the root
  # graph. No new receiver frame competes with shared owner classification.
  $capture.nativeOutputQueryOriginal=$m
  $m.constructorAttempted=$true
  try {
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids query construction.'}
    $m.parameterTypesOriginal=[object[]]::new(4)
    $m.buildersOriginal=[object[]]::new(4)
    $m.constructorsOriginal=[object[]]::new(4)
    $m.invokesOriginal=[object[]]::new(4)
    $m.attributeConstructorsOriginal=[object[]]::new(4)
    $m.attributeFieldsOriginal=[object[]]::new(4)
    $m.attributeArgumentsOriginal=[object[]]::new(4)
    $m.attributeFieldValuesOriginal=[object[]]::new(4)
    $m.attributesOriginal=[object[]]::new(4)
    $m.delegateTypesOriginal=[Type[]]::new(4)
    $m.exportsOriginal=[IntPtr[]]::new(4)
    $m.delegatesOriginal=[object[]]::new(4)
    $m.namesOriginal=[string[]]@('FindFirstFileExW','FindNextFileW','FindClose','DeviceIoControl')
    # Literal SDK26100 signatures, not caller-supplied exports or types.
    $m.signaturesOriginal=@(@([IntPtr],[IntPtr],[int],[IntPtr],[int],[IntPtr],[uint32]),
      @([int],[IntPtr],[IntPtr]),@([int],[IntPtr]),
      @([int],[IntPtr],[uint32],[IntPtr],[uint32],[IntPtr],[uint32],[IntPtr],[IntPtr]))
    $m.assemblyNameOriginal=[Reflection.AssemblyName]::new('Cli39.FixedNativeOutputQueryAbi')
    $m.assemblyOriginal=[Reflection.Emit.AssemblyBuilder]::DefineDynamicAssembly(
      $m.assemblyNameOriginal,[Reflection.Emit.AssemblyBuilderAccess]::Run)
    $m.dynamicModuleOriginal=$m.assemblyOriginal.DefineDynamicModule('FixedOutputQueryAbi')
    for($i=0;$i -lt 4;$i++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline forbids another query constructor.'}
      $s=$m.signaturesOriginal[$i]
      $parameters=[Type[]]::new($s.Count-1);$m.parameterTypesOriginal[$i]=$parameters
      for($j=1;$j -lt $s.Count;$j++){$parameters[$j-1]=$s[$j]}
      $builder=$m.dynamicModuleOriginal.DefineType($m.namesOriginal[$i],
        [Reflection.TypeAttributes]'Class,Public,Sealed',[MulticastDelegate])
      $m.buildersOriginal[$i]=$builder
      $ctor=$builder.DefineConstructor([Reflection.MethodAttributes]'RTSpecialName,HideBySig,Public',
        [Reflection.CallingConventions]::Standard,[Type[]]@([object],[IntPtr]))
      $m.constructorsOriginal[$i]=$ctor
      $ctor.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
      $invoke=$builder.DefineMethod('Invoke',[Reflection.MethodAttributes]'Public,HideBySig,NewSlot,Virtual',
        $s[0],$parameters);$m.invokesOriginal[$i]=$invoke
      $invoke.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
      $attributeCtor=[Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetConstructor(
        [Type[]]@([Runtime.InteropServices.CallingConvention]))
      $m.attributeConstructorsOriginal[$i]=$attributeCtor
      $fields=[Reflection.FieldInfo[]]@([Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetField('SetLastError'))
      $m.attributeFieldsOriginal[$i]=$fields
      $arguments=[object[]]@([Runtime.InteropServices.CallingConvention]::Winapi)
      $values=[object[]]@($true)
      $m.attributeArgumentsOriginal[$i]=$arguments;$m.attributeFieldValuesOriginal[$i]=$values
      $attribute=[Reflection.Emit.CustomAttributeBuilder]::new($attributeCtor,$arguments,$fields,$values)
      $m.attributesOriginal[$i]=$attribute;$builder.SetCustomAttribute($attribute)
      $m.delegateTypesOriginal[$i]=$builder.CreateType()
    }
    $w=@{hostOwnerOriginal=$p;queryOriginal=$m;phase='fixed-output-query-bind';
      results=[int[]]::new(4);exceptionOriginal=[object[]]::new(1);context=$null;
      methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
      startAttempted=$false;startReturned=$false;startException=$null;
      pendingActive=$false;multiplicityUnknown=$false;terminalJoined=$false;
      pending=@{attempted=$false;returned=$false;value=$null;exception=$null};
      repeatedFalse=@{returned=$false;repeats=0L};joinException=$null;
      completionAttempted=$false;completionReturned=$false}
    $m.workerOriginal=$w
    $w.context=[object[]]@($m.module,$m.namesOriginal,$m.delegateTypesOriginal,
      $m.exportsOriginal,$m.delegatesOriginal,$w.results,$w.exceptionOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedOutputQueryBind',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $address=$il.DeclareLocal([IntPtr]);$d=$il.DeclareLocal([Delegate]);$failureLocal=$il.DeclareLocal([Exception])
    function Emit-Cli39OutputQueryContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39OutputQueryMark32([int]$Index,[int]$Value){
      Emit-Cli39OutputQueryContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Value);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $null=$il.BeginExceptionBlock()
    for($i=0;$i -lt 4;$i++){
      Emit-Cli39OutputQueryMark32 0 ($i+1)
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,[IntPtr])
      Emit-Cli39OutputQueryContext32 1 ([string[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.NativeLibrary].GetMethod('GetExport',[Type[]]@([IntPtr],[string])))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$address)
      Emit-Cli39OutputQueryContext32 3 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$address);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
      Emit-Cli39OutputQueryMark32 1 ($i+1)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$address)
      Emit-Cli39OutputQueryContext32 2 ([Type[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetDelegateForFunctionPointer',[Type[]]@([IntPtr],[Type])))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$d)
      Emit-Cli39OutputQueryContext32 4 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$d);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
      Emit-Cli39OutputQueryMark32 2 ($i+1)
    }
    Emit-Cli39OutputQueryMark32 3 1
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
    Emit-Cli39OutputQueryContext32 6 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal)
    $w.threadOriginal.IsBackground=$false
    return $w
  }catch{$m.constructorFailure=$_;throw}
  finally{$m.constructorTerminal=$true}
}

function Invoke-Cli39NativeOutputQueryBinding32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$p=$w.hostOwnerOriginal;$m=$w.queryOriginal
  if(-not [object]::ReferenceEquals($p.inputRead.value.nativeOutputQueryOriginal,$m) -or
      -not [object]::ReferenceEquals($m.workerOriginal,$w) -or
      -not [object]::ReferenceEquals($m.moduleOwnerOriginal,$m.abiOriginal.originalOwner) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $m.module -ne $m.moduleOwnerOriginal.module -or -not $m.constructorTerminal -or
      $null -ne $m.constructorFailure -or $w.startAttempted){throw 'SAME once-only fixed query worker required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline forbids query binding.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.context);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Unknown original query join remains retained.'}
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
  if($w.threadOriginal.IsAlive -or $null -ne $w.exceptionOriginal[0] -or
      $w.results[3] -ne 1 -or $w.results[2] -ne 4){throw 'Incomplete original fixed query binding remains retained.'}
  for($i=0;$i -lt 4;$i++){
    if($m.exportsOriginal[$i] -eq [IntPtr]::Zero -or $null -eq $m.delegatesOriginal[$i]){
      throw 'Missing actual fixed export/delegate original.'
    }
  }
  $w.completionReturned=$true;$m.complete=$true
  # This is ABI binding only. It makes no source, namespace, principal,
  # capability, loaded-module or native admission assertion.
  return $m
}

function Test-Cli39NativeOutputQueryBorrowClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$capture=$p.inputRead.value
  foreach($frame in $p.receiver.frames.Values){
    if($null -ne $frame.consumerOriginal -and
        [object]::ReferenceEquals($frame.consumerOriginal.rootCaptureOriginal,$capture) -and
        $frame.consumerOriginal.ContainsKey('pythonTreeOriginal') -and
        -not (Test-Cli39PythonTreeClosure32 $frame)){return $false}
  }
  if($null -eq $capture -or -not $capture.ContainsKey('nativeOutputQueryOriginal')){return $true}
  $m=$capture.nativeOutputQueryOriginal
  if($null -eq $m -or -not [object]::ReferenceEquals($m.hostOwnerOriginal,$p) -or
      -not $m.constructorTerminal -or
      -not [object]::ReferenceEquals($m.moduleOwnerOriginal,$m.abiOriginal.originalOwner) -or
      $m.module -ne $m.moduleOwnerOriginal.module){return $false}
  $w=$m.workerOriginal
  # Every native export/delegate effect is inside the emitted worker. Managed
  # constructor failure before any Start has no root/file/native-buffer borrow;
  # its partial CLR graph and borrowed resident module still live to host exit.
  if($null -eq $w){return $true}
  if(-not [object]::ReferenceEquals($w.queryOriginal,$m)){return $false}
  if(-not $w.startAttempted){
    # No native call is present in construction; the emitted worker contains
    # only binding. A fully returned Thread constructor with no Start is kept
    # resident through host exit, and cannot hold a root stream/native buffer.
    return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)
  }
  # Failed binding need not produce every delegate to stop borrowing the
  # resident module. Actual terminal join, independently of success, is owed.
  return ($w.terminalJoined -and -not $w.threadOriginal.IsAlive -and
    -not $w.pendingActive -and -not $w.multiplicityUnknown -and
    $null -eq $w.joinException)
}
