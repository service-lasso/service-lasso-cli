# UNRUN fixed Wintrust module/export/delegate construction. Explicit module
# reference remains resident until externally witnessed real host disposition.
function New-Cli39PublisherModuleWorker26 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal; $n=$c.publisherNativeOriginal
  if ($c.slot -cne 'cli39.windows-publisher-trust.v1' -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $n.moduleOwners.Count -ne 0) { throw 'Same enrolled once-only fixed Wintrust module preparation required.' }
  $subject=$c.subjectOriginals['wintrust']
  if ($null -eq $subject -or $null -eq $subject.stream -or $subject.handleOriginal.IsClosed -or
      -not [IO.Path]::IsPathFullyQualified($subject.path) -or
      [IO.Path]::GetFileName($subject.path) -ine 'wintrust.dll') { throw 'Exact held qualified Wintrust subject required.' }
  if ([IntPtr]::Size -ne 8 -or $c.geometry.workerStackBytes -le 0 -or
      $c.geometry.workerStackBytes -gt [int]::MaxValue) { throw 'Complete finite x64 worker geometry required.' }
  $m=@{ kind='cli39.publisher-resident-module.v1'; preparation=$p;
    terminalLifetime='host-process'; constructionTerminal=$false;
    module=[IntPtr]::Zero; loadAttempted=$false; loadWorkerStartAttempted=$false; constructorFailure=$null;
    subjectOriginal=$subject; path=$subject.path; assembly=$null; dynamicModule=$null;
    assemblyNameOriginal=$null; namesOriginal=$null; signaturesOriginal=$null;
    builderOriginal=$null; buildersOriginal=[object[]]::new(4);
    parameterTypesOriginal=[object[]]::new(4); constructorsOriginal=[object[]]::new(4);
    invokesOriginal=[object[]]::new(4); attributesOriginal=[object[]]::new(4);
    delegateTypeOriginals=[Type[]]::new(4);
    moduleAndExportsOriginal=[IntPtr[]]::new(5); delegatesOriginal=[object[]]::new(4);
    workerOriginal=$null; receiverKey=('cli39-publisher-module-'+[Guid]::NewGuid().ToString('N')) }
  # Same actual original enters BOTH rosters before Load or delegate effects.
  $n.moduleOwners.Add($m); $p.receiver.frames.Add($m.receiverKey,$m)
  try {
    $assemblyName=[Reflection.AssemblyName]::new('Cli39.Publisher.FixedWintrustAbi')
    $m.assemblyNameOriginal=$assemblyName
    $m.assembly=[Reflection.Emit.AssemblyBuilder]::DefineDynamicAssembly($assemblyName,[Reflection.Emit.AssemblyBuilderAccess]::Run)
    $m.dynamicModule=$m.assembly.DefineDynamicModule('FixedWintrust')
    $names=[string[]]@('WinVerifyTrust','WTHelperProvDataFromStateData',
      'WTHelperGetProvSignerFromChain','WTHelperGetProvCertFromChain')
    $m.namesOriginal=$names
    # Return followed by argument types; exact SDK x64 declarations, not a
    # caller-supplied native signature or effect callback.
    $signatures=@(@([int],[IntPtr],[IntPtr],[IntPtr]),@([IntPtr],[IntPtr]),
      @([IntPtr],[IntPtr],[uint32],[int],[uint32]),@([IntPtr],[IntPtr],[uint32]))
    $m.signaturesOriginal=$signatures
    for ($i=0;$i -lt 4;$i++) {
      $signature=$signatures[$i]
      $parameters=[Type[]]::new($signature.Count-1)
      $m.parameterTypesOriginal[$i]=$parameters
      for ($j=1;$j -lt $signature.Count;$j++) { $parameters[$j-1]=$signature[$j] }
      $builder=$m.dynamicModule.DefineType($names[$i],[Reflection.TypeAttributes]'Class,Public,Sealed',[MulticastDelegate])
      $m.builderOriginal=$builder
      $m.buildersOriginal[$i]=$builder
      $constructor=$builder.DefineConstructor([Reflection.MethodAttributes]'RTSpecialName,HideBySig,Public',
        [Reflection.CallingConventions]::Standard,[Type[]]@([object],[IntPtr]))
      $m.constructorsOriginal[$i]=$constructor
      $constructor.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
      $invoke=$builder.DefineMethod('Invoke',[Reflection.MethodAttributes]'Public,HideBySig,NewSlot,Virtual',$signature[0],$parameters)
      $m.invokesOriginal[$i]=$invoke
      $invoke.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
      $attributeConstructor=[Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetConstructor([Type[]]@([Runtime.InteropServices.CallingConvention]))
      $attribute=[Reflection.Emit.CustomAttributeBuilder]::new($attributeConstructor,
        [object[]]@([Runtime.InteropServices.CallingConvention]::Winapi))
      $m.attributesOriginal[$i]=$attribute
      $builder.SetCustomAttribute($attribute)
      $m.delegateTypeOriginals[$i]=$builder.CreateType()
    }
    $w=@{ phase='module-load'; preparation=$p; nativeOriginal=$n; moduleOwnerOriginal=$m;
      results=[int[]]::new(5); exceptionOriginal=[object[]]::new(1);
      context=$null; method=$null; il=$null; body=$null; thread=$null;
      startAttempted=$false; startReturned=$false; startException=$null;
      terminalJoined=$false; joinPendingActive=$false; joinMultiplicityUnknown=$false;
      joinPending=@{ attempted=$false; returned=$false; value=$null; exception=$null };
      joinFalse=@{ returned=$false; value=$false; repeats=0L };
      joinTerminal=@{ returned=$false; value=$true };
      joinException=@{ present=$false; original=$null } }
    $m.workerOriginal=$w
    $w.context=[object[]]@($m.path,$names,$m.delegateTypeOriginals,$m.moduleAndExportsOriginal,
      $m.delegatesOriginal,$w.results,$w.exceptionOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedWintrustModule',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator(); $w.il=$il
    $module=$il.DeclareLocal([IntPtr]); $address=$il.DeclareLocal([IntPtr])
    $delegate=$il.DeclareLocal([Delegate]); $exception=$il.DeclareLocal([Exception])
    function Emit-Cli39ModuleContext26([int]$Index,[Type]$Type) {
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39ModuleScalar26([int]$Index,[int]$Value) {
      Emit-Cli39ModuleContext26 5 ([int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Value); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $null=$il.BeginExceptionBlock()
    Emit-Cli39ModuleScalar26 0 1
    Emit-Cli39ModuleContext26 0 ([string])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.NativeLibrary].GetMethod('Load',[Type[]]@([string])))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$module)
    Emit-Cli39ModuleContext26 3 ([IntPtr[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$module); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    Emit-Cli39ModuleScalar26 1 1
    for ($i=0;$i -lt 4;$i++) {
      Emit-Cli39ModuleScalar26 2 ($i+1)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$module)
      Emit-Cli39ModuleContext26 1 ([string[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.NativeLibrary].GetMethod('GetExport',[Type[]]@([IntPtr],[string])))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$address)
      Emit-Cli39ModuleContext26 3 ([IntPtr[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($i+1))
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$address); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$address)
      Emit-Cli39ModuleContext26 2 ([Type[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetDelegateForFunctionPointer',[Type[]]@([IntPtr],[Type])))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$delegate)
      Emit-Cli39ModuleContext26 4 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$delegate); $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
      Emit-Cli39ModuleScalar26 3 ($i+1)
    }
    Emit-Cli39ModuleScalar26 4 1
    $il.BeginCatchBlock([Exception]); $il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
    Emit-Cli39ModuleContext26 6 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$exception); $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock(); $il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false
    return $w
  } catch { $m.constructorFailure=$_; $m.constructionTerminal=$true; throw }
}

function Complete-Cli39PublisherModule26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal; $m=$w.moduleOwnerOriginal; $p=$w.preparation
  if (-not [object]::ReferenceEquals($m.workerOriginal,$w) -or
      -not [object]::ReferenceEquals($m.preparation,$p) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$m.receiverKey],$m) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $m.constructionTerminal) { throw 'Actual original terminal module construction required.' }
  try {
    $m.module=$m.moduleAndExportsOriginal[0]
    $m.loadAttempted=($w.results[0] -eq 1)
    if ($null -ne $w.exceptionOriginal[0]) { $m.constructorFailure=$w.exceptionOriginal[0]; throw $w.exceptionOriginal[0] }
    if ($w.results[4] -ne 1 -or $m.module -eq [IntPtr]::Zero) { throw 'Complete original fixed module/export/delegate result required.' }
    $n=$w.nativeOriginal
    $n.wintrustModuleOriginal=$m
    $n.winVerifyTrustOriginal=$m.delegatesOriginal[0]; $n.delegateType=$m.delegateTypeOriginals[0]
    return $m
  } finally { $m.constructionTerminal=$true }
}
