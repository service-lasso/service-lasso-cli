# Fixed derived-root serializer/file worker. Import has no resource effects.
function New-Cli39DerivedRootWorker32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal
  $w=@{bookOriginal=$b;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    contextOriginal=$null;constructorTerminal=$false;constructorFailure=$null;
    startAttempted=$false;startReturned=$false;startException=$null;terminalJoined=$false;
    joinException=$null;joinFalseCount=0L;joinMultiplicityUnknown=$false}
  $b.writeWorkerOriginal=$w
  try{
    $w.contextOriginal=[object[]]@($b.elementsOriginal,$b.namesOriginal,$b.lineageNamesOriginal,$b.lineageValuesOriginal,
      $b.backingOriginal,$b.readbackOriginal,$b.pathOriginal,$b.resourcesOriginal,$b.digestOriginal,
      $b.countsOriginal,$b.markersOriginal,$b.exceptionsOriginal,$b.readLengthsOriginal,$b.readMarkersOriginal,
      $b.projectionOriginal.scalarNamesOriginal,$b.projectionOriginal.scalarValuesOriginal,
      $b.projectionOriginal.memberNamesOriginal,$b.projectionOriginal.membersOriginal,
      $b.projectionOriginal.namespaceNamesOriginal,$b.projectionOriginal.namespaceOriginal,
      $b.projectionOriginal.loaderNamesOriginal,$b.projectionOriginal.loadersOriginal,
      $b.projectionOriginal.nodePolicyNamesOriginal,$b.projectionOriginal.nodePolicyOriginal,
      $b.projectionOriginal.publisherPolicyNamesOriginal,$b.projectionOriginal.publisherPolicyOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedDerivedRootWrite',[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $failureLocal=$il.DeclareLocal([Exception]);$optionsLocal=$il.DeclareLocal([System.Text.Json.JsonWriterOptions])
    $readLocal=$il.DeclareLocal([int]);$usedLocal=$il.DeclareLocal([int]);$readIndexLocal=$il.DeclareLocal([int])
    function Emit-CliDerivedContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-CliDerivedMark32([int]$Index){
      Emit-CliDerivedContext32 10 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-CliDerivedResource32([int]$Index,[Type]$Type){
      Emit-CliDerivedContext32 7 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref);$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-CliDerivedWriter32 {Emit-CliDerivedResource32 1 ([System.Text.Json.Utf8JsonWriter])}
    $null=$il.BeginExceptionBlock();Emit-CliDerivedMark32 0
    Emit-CliDerivedContext32 7 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-CliDerivedContext32 4 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$b.capacityOriginal)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Newobj,[IO.MemoryStream].GetConstructor([Type[]]@([byte[]],[int],[int],[bool],[bool])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref);Emit-CliDerivedMark32 1
    Emit-CliDerivedMark32 2;Emit-CliDerivedContext32 7 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    Emit-CliDerivedResource32 0 ([IO.MemoryStream]);$il.Emit([Reflection.Emit.OpCodes]::Ldloca,$optionsLocal);$il.Emit([Reflection.Emit.OpCodes]::Initobj,[System.Text.Json.JsonWriterOptions])
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$optionsLocal)
    $il.Emit([Reflection.Emit.OpCodes]::Newobj,[System.Text.Json.Utf8JsonWriter].GetConstructor([Type[]]@([IO.Stream],[System.Text.Json.JsonWriterOptions])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref);Emit-CliDerivedMark32 3
    Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteStartObject',[Type[]]@()))
    Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Ldstr,'schema');$il.Emit([Reflection.Emit.OpCodes]::Ldstr,'service-lasso.cli39-prerequisite-input.v3')
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteString',[Type[]]@([string],[string])))
    for($i=0;$i -lt $b.namesOriginal.Length;$i++){
      Emit-CliDerivedWriter32;Emit-CliDerivedContext32 1 ([string[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WritePropertyName',[Type[]]@([string])))
      Emit-CliDerivedContext32 0 ([System.Text.Json.JsonElement[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i);$il.Emit([Reflection.Emit.OpCodes]::Ldelema,[System.Text.Json.JsonElement])
      Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Call,[System.Text.Json.JsonElement].GetMethod('WriteTo',[Type[]]@([System.Text.Json.Utf8JsonWriter])))
    }
    Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Ldstr,'parentLineage');$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteStartObject',[Type[]]@([string])))
    for($i=0;$i -lt $b.lineageNamesOriginal.Length;$i++){
      Emit-CliDerivedWriter32
      foreach($contextIndex in @(2,3)){Emit-CliDerivedContext32 $contextIndex ([string[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)}
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteString',[Type[]]@([string],[string])))
    }
    Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteEndObject',[Type[]]@()))
    Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Ldstr,'foundationProjection')
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteStartObject',[Type[]]@([string])))
    for($i=0;$i -lt $b.projectionOriginal.scalarNamesOriginal.Length;$i++){
      Emit-CliDerivedWriter32
      foreach($contextIndex in @(14,15)){Emit-CliDerivedContext32 $contextIndex ([string[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)}
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteString',[Type[]]@([string],[string])))
    }
    foreach($roster in @(@('members',16,17,$b.projectionOriginal.membersOriginal),@('namespace',18,19,$b.projectionOriginal.namespaceOriginal),
        @('loaders',20,21,$b.projectionOriginal.loadersOriginal),@('nodePolicy',22,23,$b.projectionOriginal.nodePolicyOriginal),
        @('publisherPolicy',24,25,$b.projectionOriginal.publisherPolicyOriginal))){
      Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Ldstr,[string]$roster[0])
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteStartArray',[Type[]]@([string])))
      $rows=$roster[3]
      for($rowIndex=0;$rowIndex -lt $rows.Count;$rowIndex++){
        Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteStartObject',[Type[]]@()))
        for($i=0;$i -lt $rows[$rowIndex].Length;$i++){
          Emit-CliDerivedWriter32
          Emit-CliDerivedContext32 ([int]$roster[1]) ([string[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
          Emit-CliDerivedContext32 ([int]$roster[2]) ([Collections.Generic.List[object]])
          $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$rowIndex)
          $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Collections.Generic.List[object]].GetProperty('Item').GetMethod)
          $il.Emit([Reflection.Emit.OpCodes]::Castclass,[string[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
          $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteString',[Type[]]@([string],[string])))
        }
        Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteEndObject',[Type[]]@()))
      }
      Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteEndArray',[Type[]]@()))
    }
    foreach($ignored in @(0,1)){Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('WriteEndObject',[Type[]]@()))}
    Emit-CliDerivedWriter32;$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[System.Text.Json.Utf8JsonWriter].GetMethod('Flush',[Type[]]@()))
    Emit-CliDerivedContext32 9 ([long[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);Emit-CliDerivedResource32 0 ([IO.MemoryStream])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[IO.MemoryStream].GetProperty('Position').GetMethod);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I8);Emit-CliDerivedMark32 4
    Emit-CliDerivedMark32 5;Emit-CliDerivedContext32 7 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
    Emit-CliDerivedContext32 6 ([string]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][IO.FileMode]::CreateNew)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][IO.FileAccess]::ReadWrite);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][IO.FileShare]::Read)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,4096);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][IO.FileOptions]::WriteThrough)
    $il.Emit([Reflection.Emit.OpCodes]::Newobj,[IO.FileStream].GetConstructor([Type[]]@([string],[IO.FileMode],[IO.FileAccess],[IO.FileShare],[int],[IO.FileOptions])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref);Emit-CliDerivedMark32 6
    Emit-CliDerivedMark32 7;Emit-CliDerivedResource32 2 ([IO.FileStream]);Emit-CliDerivedContext32 4 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-CliDerivedContext32 9 ([long[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I8);$il.Emit([Reflection.Emit.OpCodes]::Conv_I4)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[IO.FileStream].GetMethod('Write',[Type[]]@([byte[]],[int],[int])));Emit-CliDerivedMark32 8
    Emit-CliDerivedMark32 9;Emit-CliDerivedResource32 2 ([IO.FileStream]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[IO.FileStream].GetMethod('Flush',[Type[]]@([bool])));Emit-CliDerivedMark32 10
    Emit-CliDerivedResource32 2 ([IO.FileStream]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I8,0L);$il.Emit([Reflection.Emit.OpCodes]::Callvirt,[IO.FileStream].GetProperty('Position').SetMethod)
    $loop=$il.DefineLabel();$eof=$il.DefineLabel();$il.MarkLabel($loop)
    # At most capacity+1 positive one-byte reads plus the terminal EOF call;
    # every physical read keeps its own preowned length/attempt/return cells.
    Emit-CliDerivedContext32 13 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$readIndexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2);$il.Emit([Reflection.Emit.OpCodes]::Mul)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    Emit-CliDerivedResource32 2 ([IO.FileStream]);Emit-CliDerivedContext32 5 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$usedLocal)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$b.capacityOriginal+1);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$usedLocal);$il.Emit([Reflection.Emit.OpCodes]::Sub)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[IO.FileStream].GetMethod('Read',[Type[]]@([byte[]],[int],[int])));$il.Emit([Reflection.Emit.OpCodes]::Stloc,$readLocal)
    Emit-CliDerivedContext32 12 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$readIndexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$readLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    Emit-CliDerivedContext32 13 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$readIndexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2);$il.Emit([Reflection.Emit.OpCodes]::Mul);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Add)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$readIndexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Add);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$readIndexLocal)
    Emit-CliDerivedContext32 9 ([long[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$readIndexLocal);$il.Emit([Reflection.Emit.OpCodes]::Conv_I8);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I8)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$readLocal);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$eof)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$usedLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$readLocal);$il.Emit([Reflection.Emit.OpCodes]::Add);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$usedLocal)
    Emit-CliDerivedContext32 9 ([long[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$usedLocal);$il.Emit([Reflection.Emit.OpCodes]::Conv_I8);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I8)
    # Full readback capacity is a rejection witness, never a zero-count EOF call.
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$usedLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$b.capacityOriginal+1)
    $supported=$il.DefineLabel();$il.Emit([Reflection.Emit.OpCodes]::Blt,$supported)
    $il.Emit([Reflection.Emit.OpCodes]::Ldstr,'Derived root exceeded preowned extent witness.');$il.Emit([Reflection.Emit.OpCodes]::Newobj,[IO.InvalidDataException].GetConstructor([Type[]]@([string])));$il.Emit([Reflection.Emit.OpCodes]::Throw)
    $il.MarkLabel($supported);$il.Emit([Reflection.Emit.OpCodes]::Br,$loop);$il.MarkLabel($eof)
    Emit-CliDerivedContext32 9 ([long[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$usedLocal);$il.Emit([Reflection.Emit.OpCodes]::Conv_I8);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I8);Emit-CliDerivedMark32 11
    Emit-CliDerivedMark32 12;Emit-CliDerivedContext32 7 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Security.Cryptography.SHA256].GetMethod('Create',[Type[]]@()));$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref);Emit-CliDerivedMark32 13
    Emit-CliDerivedMark32 14;Emit-CliDerivedContext32 8 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-CliDerivedResource32 3 ([Security.Cryptography.HashAlgorithm]);Emit-CliDerivedContext32 5 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$usedLocal)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Security.Cryptography.HashAlgorithm].GetMethod('ComputeHash',[Type[]]@([byte[]],[int],[int])));$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref);Emit-CliDerivedMark32 15
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
    Emit-CliDerivedContext32 11 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.BeginFinallyBlock();Emit-CliDerivedMark32 16;$il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal,[int]$b.profileOriginal.cliDerivedRootWorkerStackBytes)
    $w.threadOriginal.IsBackground=$false;return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}
