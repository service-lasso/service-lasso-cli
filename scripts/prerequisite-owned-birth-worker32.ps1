# Fixed CLI birth workers. No native or worker effect occurs at import time.
function New-Cli39OwnedBirthWorker32 {
  param([Parameter(Mandatory)]$BookOriginal,[Parameter(Mandatory)][ValidateSet('times','image','token-open','token-user','token-statistics','token-close')][string]$Role)
  $b=$BookOriginal;$p=$b.hostOwnerOriginal;$q=$b.queryOriginal;$m=$b.moduleOriginal
  $w=@{bookOriginal=$b;roleOriginal=$Role;constructorTerminal=$false;constructorFailure=$null;
    contextOriginal=$null;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    bufferOriginal=$null;pinsOriginal=$null;markersOriginal=$null;exceptionsOriginal=$null;resultsOriginal=$null;
    pointerOriginal=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinAttempted=$false;joinReturned=$false;joinValue=$false;joinException=$null;joinFalseCount=0L;joinMultiplicityUnknown=$false;
    completionAttempted=$false;returned=$false}
  $b.workersOriginal.Add($w)
  try{
    [int]$bytes=switch -CaseSensitive ($Role){'times'{32};'image'{$b.imageBackingBytes};'token-open'{8};'token-user'{88};'token-statistics'{60};'token-close'{0}}
    $w.bufferOriginal=[byte[]]::new($bytes);$w.pinsOriginal=[Runtime.InteropServices.GCHandle[]]::new(1)
    $w.markersOriginal=[int[]]::new(9);$w.exceptionsOriginal=[object[]]::new(2);$w.resultsOriginal=[int[]]::new(2);$w.pointerOriginal=[IntPtr[]]::new(1)
    if($Role -ceq 'image'){[Buffer]::BlockCopy([BitConverter]::GetBytes([uint32]$b.imageCapacity),0,$w.bufferOriginal,$b.imageLengthOffset,4)}
    if($Role -cin @('token-user','token-statistics')){
      $w.bodyOriginal=$m.recipesOriginal.recipesOriginal['child-'+$Role].bodyOriginal
      $w.contextOriginal=[object[]]@($m.delegatesOriginal[0],$w.bufferOriginal,$w.pinsOriginal,$w.pointerOriginal,
        $w.markersOriginal,$w.exceptionsOriginal,$w.resultsOriginal,$b.tokenHandleOriginal)
    }else{
      $delegate=$null;$type=$null;$handles=$b.processHandleOriginal
      switch -CaseSensitive ($Role){
        'times'{$delegate=$q.delegatesOriginal[4];$type=$q.delegateTypesOriginal[4]}
        'image'{$delegate=$q.delegatesOriginal[5];$type=$q.delegateTypesOriginal[5]}
        'token-open'{$delegate=$m.delegatesOriginal[1];$type=$m.recipesOriginal.openTokenTypeOriginal.type}
        'token-close'{$delegate=$b.closeDelegateOriginal;$type=$delegate.GetType();$handles=$b.tokenHandleOriginal}
      }
      $w.contextOriginal=[object[]]@($delegate,$handles,$w.bufferOriginal,$w.pinsOriginal,
        $w.markersOriginal,$w.exceptionsOriginal,$w.resultsOriginal,$b.tokenHandleOriginal,$w.pointerOriginal)
      $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39OwnedBirth'+$Role,[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
      $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
      $failureLocal=$il.DeclareLocal([Exception]);$pointerLocal=$il.DeclareLocal([IntPtr])
      function Emit-CliBirthContext32([int]$Index,[Type]$Type){
        $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
      }
      function Emit-CliBirthMark32([int]$Index){
        Emit-CliBirthContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      }
      function Emit-CliBirthFailure32([int]$Index){
        Emit-CliBirthContext32 5 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
        $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
      }
      $null=$il.BeginExceptionBlock();Emit-CliBirthMark32 0
      if($Role -cne 'token-close'){
        Emit-CliBirthMark32 1
        Emit-CliBirthContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
        Emit-CliBirthContext32 2 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
        $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
        $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-CliBirthMark32 2
        Emit-CliBirthContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
        $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle]);$il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
        $il.Emit([Reflection.Emit.OpCodes]::Stloc,$pointerLocal)
        Emit-CliBirthContext32 8 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
      }
      Emit-CliBirthMark32 3
      Emit-CliBirthContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      Emit-CliBirthContext32 0 $type
      Emit-CliBirthContext32 1 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
      switch -CaseSensitive ($Role){
        'times'{foreach($offset in @(0,8,16,24)){$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$offset);$il.Emit([Reflection.Emit.OpCodes]::Call,[IntPtr].GetMethod('Add',[Type[]]@([IntPtr],[int])))}}
        'image'{
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal)
          $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$b.imageLengthOffset);$il.Emit([Reflection.Emit.OpCodes]::Call,[IntPtr].GetMethod('Add',[Type[]]@([IntPtr],[int])))
        }
        'token-open'{$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_8);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal)}
      }
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$type.GetMethod('Invoke'));$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-CliBirthMark32 4
      if($Role -ceq 'token-open'){
        # Publish the actual output cell before diagnostic capture. If this copy
        # itself fails, retain the pinned output and unknown acquisition.
        Emit-CliBirthContext32 7 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
        $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointerLocal);$il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('ReadIntPtr',[Type[]]@([IntPtr])))
        $il.Emit([Reflection.Emit.OpCodes]::Stelem_I);Emit-CliBirthMark32 8
      }
      Emit-CliBirthContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-CliBirthMark32 5
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-CliBirthFailure32 0
      $il.BeginFinallyBlock()
      if($Role -cne 'token-close'){
        $skip=$il.DefineLabel();$absent=$il.DefineLabel();$null=$il.BeginExceptionBlock()
        Emit-CliBirthContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$absent)
        Emit-CliBirthContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
        if($Role -ceq 'token-open'){
          Emit-CliBirthContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_8);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
        }
        $il.MarkLabel($absent)
        Emit-CliBirthContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
        Emit-CliBirthMark32 6;Emit-CliBirthContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
        $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle]);$il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()));Emit-CliBirthMark32 7
        $il.MarkLabel($skip);$il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-CliBirthFailure32 1;$il.EndExceptionBlock()
      }
      $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
      $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    }
    $stackBytes=$b.profileOriginal.cliBirthWorkerStackBytes
    if($b.kind -ceq 'cli39.fixed-parent-observation.v1'){
      if(-not [object]::ReferenceEquals($p.inputRead.value.parentObservationOriginal,$b)){throw 'SAME fixed parent query membership required.'}
      $stackBytes=$b.profileOriginal.cliParentWorkerStackBytes
    }
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal,[int]$stackBytes)
    $w.threadOriginal.IsBackground=$false
    return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}
