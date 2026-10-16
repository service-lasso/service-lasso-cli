# Three fixed CLI namespace recipes. No top-level effects or Core input grammar.
function Initialize-Cli39NamespaceRecipes32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $p.ContainsKey('namespaceRecipesOriginal') -or $p.inputRead.attempted -or $p.serverAttempted){throw 'SAME once-only minimal startup owner required.'}
  $book=@{hostOwnerOriginal=$p;recordsOriginal=$null;returned=$false;
    constructionTerminal=$false;exceptionOriginal=$null;terminalLifetime='host-process'}
  $p.namespaceRecipesOriginal=$book
  try{
    $book.recordsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach($phase in @('open','metadata','close')){$null=New-Cli39NamespaceRecipe32 $p $phase}
    $book.returned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}finally{$book.constructionTerminal=$true}
}
function New-Cli39NamespaceRecipe32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)][ValidateSet('open','metadata','close')][string]$Phase)
  $p=$HostOwnerOriginal;$book=$p.namespaceRecipesOriginal
  if(-not [object]::ReferenceEquals($book.hostOwnerOriginal,$p) -or $book.returned -or
      $book.recordsOriginal.ContainsKey($Phase)){throw 'SAME pre-NPH fixed recipe enrollment required.'}
  $r=@{phaseOriginal=$Phase;bookOriginal=$book;typeOriginal=$null;methodOriginal=$null;
    ilOriginal=$null;bodyOriginal=$null;returned=$false;constructionTerminal=$false;exceptionOriginal=$null}
  $book.recordsOriginal.Add($Phase,$r)
  try{
    $name=switch -CaseSensitive($Phase){'open'{'CreateFileW'}'metadata'{'GetFileInformationByHandleEx'}'close'{'CloseHandle'}}
    $r.typeOriginal=$p.abiLiteralTypes[$name]
    if($null -eq $r.typeOriginal -or -not $r.typeOriginal.returned -or
        -not $r.typeOriginal.constructionTerminal -or $null -eq $r.typeOriginal.type){throw 'SAME completed shared literal startup type required.'}
    $r.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedNamespacePhase',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$r.methodOriginal.GetILGenerator();$r.ilOriginal=$il
    $failureLocal=$il.DeclareLocal([Exception]);$boolLocal=$il.DeclareLocal([int])
    function Emit-Cli39NamespaceContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39NamespaceMark32([int]$Index){
      Emit-Cli39NamespaceContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39NamespaceFailure32([int]$Index){
      Emit-Cli39NamespaceContext32 3 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    function Emit-Cli39NamespacePointer32([int]$Index){
      Emit-Cli39NamespaceContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
    }
    function Emit-Cli39NamespaceDiagnostic32([int]$Index,[int]$Marker){
      Emit-Cli39NamespaceContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39NamespaceMark32 $Marker
    }
    $null=$il.BeginExceptionBlock();Emit-Cli39NamespaceMark32 18
    if($Phase -ceq 'open'){
      Emit-Cli39NamespaceMark32 0
      Emit-Cli39NamespaceContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      Emit-Cli39NamespaceContext32 0 ([string])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('StringToHGlobalUni',[Type[]]@([string])))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_I);Emit-Cli39NamespaceMark32 1
      Emit-Cli39NamespaceMark32 2
      Emit-Cli39NamespaceContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
      Emit-Cli39NamespaceContext32 2 $r.typeOriginal.type
      Emit-Cli39NamespacePointer32 0
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,0x80)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
      $il.Emit([Reflection.Emit.OpCodes]::Ldsfld,[IntPtr].GetField('Zero'))
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,0x02200000)
      $il.Emit([Reflection.Emit.OpCodes]::Ldsfld,[IntPtr].GetField('Zero'))
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$r.typeOriginal.type.GetMethod('Invoke'))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_I);Emit-Cli39NamespaceMark32 3
      Emit-Cli39NamespaceDiagnostic32 0 4
    }elseif($Phase -ceq 'close'){
      Emit-Cli39NamespaceMark32 0
      Emit-Cli39NamespaceContext32 2 $r.typeOriginal.type;Emit-Cli39NamespacePointer32 1
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$r.typeOriginal.type.GetMethod('Invoke'))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$boolLocal)
      Emit-Cli39NamespaceContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$boolLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      Emit-Cli39NamespaceMark32 1;Emit-Cli39NamespaceDiagnostic32 0 2
    }else{
      for($i=0;$i -lt 2;$i++){
        $offset=$i*8
        Emit-Cli39NamespaceMark32 ($offset+3)
        Emit-Cli39NamespaceContext32 6 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
        Emit-Cli39NamespaceContext32 1 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
        $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
        $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
        $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-Cli39NamespaceMark32 ($offset+4)
        Emit-Cli39NamespaceMark32 $offset
        Emit-Cli39NamespaceContext32 2 $r.typeOriginal.type;Emit-Cli39NamespacePointer32 1
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,(@(9,18)[$i]))
        Emit-Cli39NamespaceContext32 6 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
        $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
        $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
        $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,(@(8,24)[$i]))
        $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$r.typeOriginal.type.GetMethod('Invoke'))
        $il.Emit([Reflection.Emit.OpCodes]::Stloc,$boolLocal)
        Emit-Cli39NamespaceContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
        $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$boolLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
        Emit-Cli39NamespaceMark32 ($offset+1)
        Emit-Cli39NamespaceDiagnostic32 $i ($offset+2)
      }
    }
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
    Emit-Cli39NamespaceFailure32 0
    $il.BeginFinallyBlock()
    if($Phase -ceq 'open'){
      $skip=$il.DefineLabel();$callAbsent=$il.DefineLabel();$null=$il.BeginExceptionBlock()
      Emit-Cli39NamespaceContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      Emit-Cli39NamespaceContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callAbsent)
      Emit-Cli39NamespaceContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      $il.MarkLabel($callAbsent);Emit-Cli39NamespaceMark32 5;Emit-Cli39NamespacePointer32 0
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('FreeHGlobal',[Type[]]@([IntPtr])))
      Emit-Cli39NamespaceMark32 6;$il.MarkLabel($skip)
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
      Emit-Cli39NamespaceFailure32 1;$il.EndExceptionBlock()
    }elseif($Phase -ceq 'metadata'){
      for($i=0;$i -lt 2;$i++){
        $offset=$i*8;$skip=$il.DefineLabel();$callAbsent=$il.DefineLabel();$null=$il.BeginExceptionBlock()
        Emit-Cli39NamespaceContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($offset+4))
        $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
        Emit-Cli39NamespaceContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$offset)
        $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callAbsent)
        Emit-Cli39NamespaceContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($offset+1))
        $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
        $il.MarkLabel($callAbsent);Emit-Cli39NamespaceMark32 ($offset+5)
        Emit-Cli39NamespaceContext32 6 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
        $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
        $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
        Emit-Cli39NamespaceMark32 ($offset+6);$il.MarkLabel($skip)
        $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
        Emit-Cli39NamespaceFailure32 ($i+1);$il.EndExceptionBlock()
      }
    }
    Emit-Cli39NamespaceMark32 19;$il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $r.bodyOriginal=$r.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $r.returned=$true;return $r
  }catch{$r.exceptionOriginal=$_;throw}finally{$r.constructionTerminal=$true}
}
function Test-Cli39NamespaceRecipesTerminal32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal
  if(-not $p.ContainsKey('namespaceRecipesOriginal')){return $true}
  $b=$p.namespaceRecipesOriginal
  if($null -eq $b -or -not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or
      -not $b.constructionTerminal -or $b.terminalLifetime -cne 'host-process'){return $false}
  foreach($r in $b.recordsOriginal.Values){
    if(-not [object]::ReferenceEquals($r.bookOriginal,$b) -or -not $r.constructionTerminal){return $false}
  }
  return $true
}