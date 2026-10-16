# Fixed borrowed resident-kernel32 declarations and binding recipes. Construction
# belongs to genuine containing startup custody, before NPH or parsed budgets.
function Assert-Cli39LoadedStartupDeadline32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if($null -eq $p.loadedRecipesOriginal -or
      -not [object]::ReferenceEquals($p.loadedRecipesOriginal.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)){
    throw 'SAME enrolled loaded startup owner required.'
  }
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original startup deadline forbids subsequent loaded constructor.'}
}

function Initialize-Cli39LoadedRecipes32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if($p.ContainsKey('loadedRecipesOriginal')){throw 'Loaded startup recipes cannot retry.'}
  $b=@{hostOwnerOriginal=$p;typesOriginal=$null;recordsOriginal=$null;
    assemblyNameOriginal=$null;assemblyOriginal=$null;moduleOriginal=$null;
    constructionTerminal=$false;returned=$false;exceptionOriginal=$null;terminalLifetime='host-process'}
  $p.loadedRecipesOriginal=$b
  try{
    Assert-Cli39LoadedStartupDeadline32 $p
    $b.typesOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $b.recordsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    Assert-Cli39LoadedStartupDeadline32 $p
    $b.assemblyNameOriginal=[Reflection.AssemblyName]::new('Cli39.FixedLoadedApi')
    Assert-Cli39LoadedStartupDeadline32 $p
    $b.assemblyOriginal=[Reflection.Emit.AssemblyBuilder]::DefineDynamicAssembly($b.assemblyNameOriginal,[Reflection.Emit.AssemblyBuilderAccess]::Run)
    Assert-Cli39LoadedStartupDeadline32 $p
    $b.moduleOriginal=$b.assemblyOriginal.DefineDynamicModule('FixedLoadedApi')
    foreach($name in @('GetModuleFileNameW','GetSystemInfo','VirtualQuery')){
      Assert-Cli39LoadedStartupDeadline32 $p
      $t=@{nameOriginal=$name;parametersOriginal=$null;returnOriginal=$null;builderOriginal=$null;
        constructorOriginal=$null;invokeOriginal=$null;attributeOriginal=$null;typeOriginal=$null;
        constructionTerminal=$false;returned=$false;exceptionOriginal=$null}
      $b.typesOriginal.Add($name,$t)
      try{
        if($name -ceq 'GetModuleFileNameW'){$t.parametersOriginal=[Type[]]@([IntPtr],[IntPtr],[uint32]);$t.returnOriginal=[uint32]}
        elseif($name -ceq 'GetSystemInfo'){$t.parametersOriginal=[Type[]]@([IntPtr]);$t.returnOriginal=[void]}
        else{$t.parametersOriginal=[Type[]]@([IntPtr],[IntPtr],[UIntPtr]);$t.returnOriginal=[UIntPtr]}
        Assert-Cli39LoadedStartupDeadline32 $p
        $t.builderOriginal=$b.moduleOriginal.DefineType($name,[Reflection.TypeAttributes]'Class,Public,Sealed',[MulticastDelegate])
        Assert-Cli39LoadedStartupDeadline32 $p
        $t.constructorOriginal=$t.builderOriginal.DefineConstructor([Reflection.MethodAttributes]'RTSpecialName,HideBySig,Public',
          [Reflection.CallingConventions]::Standard,[Type[]]@([object],[IntPtr]))
        $t.constructorOriginal.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
        Assert-Cli39LoadedStartupDeadline32 $p
        $t.invokeOriginal=$t.builderOriginal.DefineMethod('Invoke',[Reflection.MethodAttributes]'Public,HideBySig,NewSlot,Virtual',$t.returnOriginal,$t.parametersOriginal)
        $t.invokeOriginal.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
        Assert-Cli39LoadedStartupDeadline32 $p
        $t.attributeOriginal=[Reflection.Emit.CustomAttributeBuilder]::new(
          [Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetConstructor([Type[]]@([Runtime.InteropServices.CallingConvention])),
          [object[]]@([Runtime.InteropServices.CallingConvention]::Winapi),
          [Reflection.FieldInfo[]]@([Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetField('SetLastError')),
          [object[]]@(($name -cne 'GetSystemInfo')))
        $t.builderOriginal.SetCustomAttribute($t.attributeOriginal)
        Assert-Cli39LoadedStartupDeadline32 $p
        $t.typeOriginal=$t.builderOriginal.CreateType();$t.returned=$true
      }catch{$t.exceptionOriginal=$_;throw}finally{$t.constructionTerminal=$true}
      # VirtualQuery export remains exclusively in own13; its type is reused
      # there so fixed region IL can be compiled under actual startup custody.
      if($name -ceq 'VirtualQuery'){continue}
      foreach($phase in @('export','delegate')){
        Assert-Cli39LoadedStartupDeadline32 $p
        $r=@{bookOriginal=$b;typeOriginal=$t;nameOriginal=$name;phaseOriginal=$phase;
          methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;constructionTerminal=$false;returned=$false;exceptionOriginal=$null}
        $b.recordsOriginal.Add($name+':'+$phase,$r)
        try{
          Assert-Cli39LoadedStartupDeadline32 $p
          $r.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39Loaded'+$name+$phase,[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
          Assert-Cli39LoadedStartupDeadline32 $p
          $il=$r.methodOriginal.GetILGenerator();$r.ilOriginal=$il;$failureLocal=$il.DeclareLocal([Exception])
          function Emit-Cli39LoadedBindingContext32([int]$Index,[Type]$Type){
            $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
            $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
          }
          function Emit-Cli39LoadedBindingMark32([int]$Index){
            Emit-Cli39LoadedBindingContext32 3 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
            $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
          }
          $null=$il.BeginExceptionBlock();Emit-Cli39LoadedBindingMark32 0
          if($phase -ceq 'export'){
            Emit-Cli39LoadedBindingContext32 0 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
            Emit-Cli39LoadedBindingContext32 0 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
            $il.Emit([Reflection.Emit.OpCodes]::Ldstr,$name)
            $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.NativeLibrary].GetMethod('GetExport',[Type[]]@([IntPtr],[string])))
            $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
          }else{
            Emit-Cli39LoadedBindingContext32 1 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
            Emit-Cli39LoadedBindingContext32 0 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
            Emit-Cli39LoadedBindingContext32 2 ([Type])
            $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetDelegateForFunctionPointer',[Type[]]@([IntPtr],[Type])))
            $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
          }
          Emit-Cli39LoadedBindingMark32 1
          $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
          Emit-Cli39LoadedBindingContext32 4 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
          $il.BeginFinallyBlock();Emit-Cli39LoadedBindingMark32 2;$il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
          Assert-Cli39LoadedStartupDeadline32 $p
          $r.bodyOriginal=$r.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart]);$r.returned=$true
        }catch{$r.exceptionOriginal=$_;throw}finally{$r.constructionTerminal=$true}
      }
    }
    Initialize-Cli39LoadedCallRecipes32 $p
    $b.returned=$true
  }catch{$b.exceptionOriginal=$_;throw}finally{$b.constructionTerminal=$true}
}

# Context: typed delegate, byte backing, GCHandle[1], IntPtr[1] argument,
# UInt32[1] path capacity, UInt64[2] raw return/error, Int32[8] markers,
# Object[2] independent call/Free exceptions. No supplied worker callback.
function Initialize-Cli39LoadedCallRecipes32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $b=$HostOwnerOriginal.loadedRecipesOriginal
  foreach($phase in @('system','path','region')){
    Assert-Cli39LoadedStartupDeadline32 $HostOwnerOriginal
    $name=switch($phase){'system'{'GetSystemInfo'}'path'{'GetModuleFileNameW'}'region'{'VirtualQuery'}}
    $t=$b.typesOriginal[$name]
    $r=@{bookOriginal=$b;typeOriginal=$t;phaseOriginal=$phase;methodOriginal=$null;ilOriginal=$null;
      bodyOriginal=$null;constructionTerminal=$false;returned=$false;exceptionOriginal=$null}
    $b.recordsOriginal.Add('call:'+$phase,$r)
    try{
      Assert-Cli39LoadedStartupDeadline32 $HostOwnerOriginal
      $r.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39LoadedCall'+$phase,[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
      Assert-Cli39LoadedStartupDeadline32 $HostOwnerOriginal
      $il=$r.methodOriginal.GetILGenerator();$r.ilOriginal=$il
      $failureLocal=$il.DeclareLocal([Exception]);$pointerLocal=$il.DeclareLocal([IntPtr])
      function Emit-Cli39LoadedCallContext32([int]$Index,[Type]$Type){
        $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
      }
      function Emit-Cli39LoadedCallMark32([int]$Index){
        Emit-Cli39LoadedCallContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      }
      function Emit-Cli39LoadedCallException32([int]$Index){
        Emit-Cli39LoadedCallContext32 7 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
        $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
      }
      $null=$il.BeginExceptionBlock();Emit-Cli39LoadedCallMark32 0
      Emit-Cli39LoadedCallContext32 2 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      Emit-Cli39LoadedCallContext32 1 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-Cli39LoadedCallMark32 1
      Emit-Cli39LoadedCallContext32 2 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$pointerLocal)
      Emit-Cli39LoadedCallMark32 2
      if($phase -cne 'system'){
        Emit-Cli39LoadedCallContext32 5 ([uint64[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      }
      Emit-Cli39LoadedCallContext32 0 $t.typeOriginal
      if($phase -cne 'system'){
        Emit-Cli39LoadedCallContext32 3 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
      }
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal)
      if($phase -ceq 'path'){
        Emit-Cli39LoadedCallContext32 4 ([uint32[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_U4)
      }elseif($phase -ceq 'region'){
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,48);$il.Emit([Reflection.Emit.OpCodes]::Conv_U)
      }
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$t.typeOriginal.GetMethod('Invoke'))
      if($phase -cne 'system'){
        $il.Emit([Reflection.Emit.OpCodes]::Conv_U8);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I8)
      }
      Emit-Cli39LoadedCallMark32 3
      if($phase -cne 'system'){
        Emit-Cli39LoadedCallContext32 5 ([uint64[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
        $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
        $il.Emit([Reflection.Emit.OpCodes]::Conv_U4);$il.Emit([Reflection.Emit.OpCodes]::Conv_U8);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I8)
        Emit-Cli39LoadedCallMark32 4
      }
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39LoadedCallException32 0
      $il.BeginFinallyBlock()
      $skipFree=$il.DefineLabel();$canFree=$il.DefineLabel()
      # An entered unknown native call may still borrow backing. A diagnostic
      # failure after actual native return does not prevent independent Free.
      Emit-Cli39LoadedCallContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4)
      $il.Emit([Reflection.Emit.OpCodes]::Brfalse,$canFree)
      Emit-Cli39LoadedCallContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4)
      $il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skipFree);$il.MarkLabel($canFree)
      Emit-Cli39LoadedCallContext32 2 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetProperty('IsAllocated').GetMethod)
      $il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skipFree)
      $null=$il.BeginExceptionBlock();Emit-Cli39LoadedCallMark32 5
      Emit-Cli39LoadedCallContext32 2 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()));Emit-Cli39LoadedCallMark32 6
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39LoadedCallException32 1;$il.EndExceptionBlock()
      $il.MarkLabel($skipFree);Emit-Cli39LoadedCallMark32 7;$il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
      Assert-Cli39LoadedStartupDeadline32 $HostOwnerOriginal
      $r.bodyOriginal=$r.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart]);$r.returned=$true
    }catch{$r.exceptionOriginal=$_;throw}finally{$r.constructionTerminal=$true}
  }
}

function Test-Cli39LoadedRecipesTerminal32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if(-not $p.ContainsKey('loadedRecipesOriginal')){return $true}
  $b=$p.loadedRecipesOriginal
  if(-not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or -not $b.constructionTerminal){return $false}
  if($null -ne $b.typesOriginal){foreach($t in $b.typesOriginal.Values){if(-not $t.constructionTerminal){return $false}}}
  if($null -ne $b.recordsOriginal){foreach($r in $b.recordsOriginal.Values){if(-not $r.constructionTerminal){return $false}}}
  return $true
}

# Pure checked geometry; caller retains its source arguments/result/exception
# before invoking it. No allocation or authority follows merely from this result.
function Get-Cli39LoadedPathGeometry32 {
  param([Parameter(Mandatory)][string]$ExpectedPath)
  if($ExpectedPath.Length -eq 0 -or $ExpectedPath.IndexOf([char]0) -ge 0){throw 'Exact nonempty NUL-free loaded path required.'}
  [decimal]$capacity=[decimal]$ExpectedPath.Length+1
  [decimal]$bytes=2*$capacity
  if($capacity -gt [uint32]::MaxValue -or $bytes -gt [int]::MaxValue){throw 'Loaded path rejection witness is not representable.'}
  return @{expectedPathOriginal=$ExpectedPath;capacityCharacters=[uint32]$capacity;backingBytes=[int]$bytes}
}

function Get-Cli39LoadedRegionGeometry32 {
  param([Parameter(Mandatory)][long]$ModuleBase,[Parameter(Mandatory)][long]$ImageBytes,
    [Parameter(Mandatory)][uint32]$PageBytes)
  if($ModuleBase -le 0 -or $ImageBytes -le 0 -or $PageBytes -eq 0 -or
      ($PageBytes -band ($PageBytes-1)) -ne 0 -or $ModuleBase % [long]$PageBytes -ne 0){
    throw 'Actual positive power-of-two page geometry and aligned module base required.'
  }
  [decimal]$end=[decimal]$ModuleBase+[decimal]$ImageBytes
  [decimal]$rows=[decimal]::Ceiling([decimal]$ImageBytes/[decimal]$PageBytes)
  if($end -gt [long]::MaxValue -or $rows -gt [int]::MaxValue){throw 'Loaded image end/region roster is not representable.'}
  return @{moduleBaseOriginal=$ModuleBase;imageBytesOriginal=$ImageBytes;pageBytesOriginal=$PageBytes;
    imageEnd=[long]$end;maximumRegionRows=[int]$rows;alignmentObserved=$true}
}
