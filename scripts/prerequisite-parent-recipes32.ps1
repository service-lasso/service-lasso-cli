# Literal child-parent query recipes. Pure definitions; startup construction
# requires the SAME genuine external owner/profile before the first statement.
function Initialize-Cli39ParentRecipes32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if($p.ContainsKey('parentRecipesOriginal')){throw 'Fixed parent recipes cannot retry.'}
  $book=@{hostOwnerOriginal=$p;typeOriginal=$null;closeTypeOriginal=$null;recordsOriginal=$null;
    constructionTerminal=$false;returned=$false;exceptionOriginal=$null;terminalLifetime='host-process'}
  $p.parentRecipesOriginal=$book
  try{
    $t=@{nameOriginal=$null;assemblyOriginal=$null;moduleOriginal=$null;builderOriginal=$null;
      constructorOriginal=$null;invokeOriginal=$null;attributeOriginal=$null;typeOriginal=$null;returned=$false}
    $book.typeOriginal=$t
    $t.nameOriginal=[Reflection.AssemblyName]::new('Cli39.FixedParentQuery')
    $t.assemblyOriginal=[Reflection.Emit.AssemblyBuilder]::DefineDynamicAssembly($t.nameOriginal,[Reflection.Emit.AssemblyBuilderAccess]::Run)
    $t.moduleOriginal=$t.assemblyOriginal.DefineDynamicModule('FixedParentQuery')
    $t.builderOriginal=$t.moduleOriginal.DefineType('OpenProcess',[Reflection.TypeAttributes]'Class,Public,Sealed',[MulticastDelegate])
    $t.constructorOriginal=$t.builderOriginal.DefineConstructor([Reflection.MethodAttributes]'RTSpecialName,HideBySig,Public',
      [Reflection.CallingConventions]::Standard,[Type[]]@([object],[IntPtr]))
    $t.constructorOriginal.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
    $t.invokeOriginal=$t.builderOriginal.DefineMethod('Invoke',[Reflection.MethodAttributes]'Public,HideBySig,NewSlot,Virtual',
      [IntPtr],[Type[]]@([uint32],[int],[uint32]))
    $t.invokeOriginal.SetImplementationFlags([Reflection.MethodImplAttributes]'Runtime,Managed')
    $t.attributeOriginal=[Reflection.Emit.CustomAttributeBuilder]::new(
      [Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetConstructor([Type[]]@([Runtime.InteropServices.CallingConvention])),
      [object[]]@([Runtime.InteropServices.CallingConvention]::Winapi),
      [Reflection.FieldInfo[]]@([Runtime.InteropServices.UnmanagedFunctionPointerAttribute].GetField('SetLastError')),[object[]]@($true))
    $t.builderOriginal.SetCustomAttribute($t.attributeOriginal)
    $t.typeOriginal=$t.builderOriginal.CreateType();$t.returned=$true
    $book.recordsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $book.closeTypeOriginal=$p.abiLiteralTypes['CloseHandle']
    if(-not $book.closeTypeOriginal.returned -or -not $book.closeTypeOriginal.constructionTerminal){throw 'SAME pre-NPH shared CloseHandle literal type required.'}
    foreach($phase in @('export','delegate','acquire','process-close')){
      $r=@{bookOriginal=$book;phaseOriginal=$phase;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;
        constructionTerminal=$false;returned=$false;exceptionOriginal=$null}
      $book.recordsOriginal.Add($phase,$r)
      try{
        $r.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39Parent'+$phase,[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
        $il=$r.methodOriginal.GetILGenerator();$r.ilOriginal=$il
        $failureLocal=$il.DeclareLocal([Exception])
        function Emit-Cli39ParentContext32([int]$Index,[Type]$Type){
          $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
          $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
        }
        function Emit-Cli39ParentMark32([int]$Index){
          Emit-Cli39ParentContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
        }
        $null=$il.BeginExceptionBlock();Emit-Cli39ParentMark32 0
        if($phase -ceq 'export'){
          Emit-Cli39ParentContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
          Emit-Cli39ParentContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
          $il.Emit([Reflection.Emit.OpCodes]::Ldstr,'OpenProcess')
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.NativeLibrary].GetMethod('GetExport',[Type[]]@([IntPtr],[string])))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I);Emit-Cli39ParentMark32 1
        }elseif($phase -ceq 'delegate'){
          Emit-Cli39ParentContext32 3 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          Emit-Cli39ParentContext32 2 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
          Emit-Cli39ParentContext32 1 ([Type])
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetDelegateForFunctionPointer',[Type[]]@([IntPtr],[Type])))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref);Emit-Cli39ParentMark32 1
        }elseif($phase -ceq 'process-close'){
          Emit-Cli39ParentMark32 2
          Emit-Cli39ParentContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          Emit-Cli39ParentContext32 0 $book.closeTypeOriginal.type
          Emit-Cli39ParentContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
          $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$book.closeTypeOriginal.type.GetMethod('Invoke'))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ParentMark32 3
          Emit-Cli39ParentContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ParentMark32 4
        }else{
          # Context5 holds the acquired HANDLE before diagnostic capture.
          Emit-Cli39ParentMark32 2
          Emit-Cli39ParentContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          Emit-Cli39ParentContext32 0 $t.typeOriginal
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,0x1000);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          Emit-Cli39ParentContext32 6 ([uint32[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_U4)
          $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$t.typeOriginal.GetMethod('Invoke'))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I);Emit-Cli39ParentMark32 3
          Emit-Cli39ParentContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
          $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
          $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ParentMark32 4
        }
        $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
        Emit-Cli39ParentContext32 8 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
        $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
        $il.BeginFinallyBlock();Emit-Cli39ParentMark32 5
        $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
        $r.bodyOriginal=$r.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart]);$r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}finally{$r.constructionTerminal=$true}
    }
    $book.returned=$true
  }catch{$book.exceptionOriginal=$_;throw}finally{$book.constructionTerminal=$true}
}

function Test-Cli39ParentRecipesTerminal32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if(-not $p.ContainsKey('parentRecipesOriginal')){return $true}
  $b=$p.parentRecipesOriginal
  if(-not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or -not $b.constructionTerminal){return $false}
  if($null -ne $b.recordsOriginal){foreach($r in $b.recordsOriginal.Values){if(-not $r.constructionTerminal){return $false}}}
  # Failed managed recipe graphs remain charged until actual host exit.
  return $true
}
