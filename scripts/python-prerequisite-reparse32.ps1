# Fixed held symbolic-link observation. No reparse target is opened or followed.
function New-Cli39PythonReparseWorker32 {
  param([Parameter(Mandatory)]$HoldOriginal)
  $h=$HoldOriginal;$b=$h.bookOriginal;$p=$b.preparationOriginal;$c=$p.consumerOriginal
  if(-not [object]::ReferenceEquals($c.pythonTreeOriginal,$b) -or -not $b.holdsOriginal.Contains($h) -or
      $h.kindOriginal -cne '32' -or $h.ContainsKey('reparseOriginal') -or
      $c.ContainsKey('disposalOriginal') -or $null -ne $b.disposalOriginal -or $null -ne $h.disposalOriginal){
    throw 'Actual once-only SAME held symbolic link required.'
  }
  $w=@{holdOriginal=$h;delegateOriginal=$null;bytesOriginal=$null;extentOriginal=$null;
    pinsOriginal=$null;markersOriginal=$null;boolOriginal=$null;errorsOriginal=$null;exceptionsOriginal=$null;
    contextOriginal=$null;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinException=$null;joinCount=0L;joinCountUnknown=$false;
    pendingOriginal=@{attempted=$false;returned=$false;value=$false;exceptionOriginal=$null};
    returned=$false;exceptionOriginal=$null;decodedOriginal=$null;chargeBytes=[decimal]0}
  $h.reparseOriginal=$w
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){throw 'Actual reparse worker stack geometry required.'}
    $w.chargeBytes=16384*[decimal]$b.budgetOriginal.pythonTreeCapturePerByteChargeBytes
    if($b.usedChargeBytes+$w.chargeBytes -gt $b.plannedChargeBytes){throw 'Reparse backing exceeds SAME planned charge.'}
    $b.usedChargeBytes+=$w.chargeBytes
    $q=$b.queryOriginal;$w.delegateOriginal=$q.delegatesOriginal[3]
    if($null -eq $w.delegateOriginal -or $w.delegateOriginal.GetType() -ne $q.delegateTypesOriginal[3]){
      throw 'SAME completed fixed DeviceIoControl delegate required.'
    }
    $w.bytesOriginal=[byte[]]::new(16384);$w.extentOriginal=[byte[]]::new(4)
    $w.pinsOriginal=[Runtime.InteropServices.GCHandle[]]::new(2);$w.markersOriginal=[int[]]::new(13)
    $w.boolOriginal=[int[]]::new(1);$w.errorsOriginal=[int[]]::new(1);$w.exceptionsOriginal=[object[]]::new(3)
    $w.contextOriginal=[object[]]@($w.delegateOriginal,$h.pointerCellsOriginal,$w.bytesOriginal,$w.extentOriginal,
      $w.pinsOriginal,$w.markersOriginal,$w.boolOriginal,$w.errorsOriginal,$w.exceptionsOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedOutputReparse',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $failureLocal=$il.DeclareLocal([Exception]);$boolLocal=$il.DeclareLocal([int])
    function Emit-Cli39ReparseContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39ReparseMark32([int]$Index){
      Emit-Cli39ReparseContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39ReparseAddress32([int]$Index){
      Emit-Cli39ReparseContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
    }
    function Emit-Cli39ReparseFailure32([int]$Index){
      Emit-Cli39ReparseContext32 8 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    foreach($i in @(0,1)){
      Emit-Cli39ReparseMark32 (2*$i)
      Emit-Cli39ReparseContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      Emit-Cli39ReparseContext32 ($i+2) ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle]);Emit-Cli39ReparseMark32 (2*$i+1)
    }
    Emit-Cli39ReparseMark32 4
    Emit-Cli39ReparseContext32 0 ($w.delegateOriginal.GetType())
    Emit-Cli39ReparseContext32 1 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,589992)
    $il.Emit([Reflection.Emit.OpCodes]::Ldsfld,[IntPtr].GetField('Zero'));$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39ReparseAddress32 0;$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,16384)
    Emit-Cli39ReparseAddress32 1;$il.Emit([Reflection.Emit.OpCodes]::Ldsfld,[IntPtr].GetField('Zero'))
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.delegateOriginal.GetType().GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$boolLocal)
    Emit-Cli39ReparseContext32 6 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$boolLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ReparseMark32 5
    Emit-Cli39ReparseContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ReparseMark32 6
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39ReparseFailure32 0
    $il.BeginFinallyBlock()
    foreach($i in @(0,1)){
      $skip=$il.DefineLabel();$safe=$il.DefineLabel();$null=$il.BeginExceptionBlock()
      Emit-Cli39ReparseContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,(2*$i+1))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      Emit-Cli39ReparseContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$safe)
      Emit-Cli39ReparseContext32 5 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_5)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      $il.MarkLabel($safe);Emit-Cli39ReparseMark32 (7+2*$i)
      Emit-Cli39ReparseContext32 4 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
      Emit-Cli39ReparseMark32 (8+2*$i);$il.MarkLabel($skip)
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal);Emit-Cli39ReparseFailure32 ($i+1)
      $il.EndExceptionBlock()
    }
    Emit-Cli39ReparseMark32 11;$il.EndExceptionBlock();Emit-Cli39ReparseMark32 12;$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal,[int]$c.geometry.workerStackBytes)
    $w.threadOriginal.IsBackground=$false;return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}

function Test-Cli39PythonReparseClosure32 {
  param([Parameter(Mandatory)]$HoldOriginal)
  $h=$HoldOriginal;$b=$h.bookOriginal
  if(-not $h.ContainsKey('reparseOriginal')){return $true}
  $w=$h.reparseOriginal
  if(-not [object]::ReferenceEquals($b.preparationOriginal.consumerOriginal.pythonTreeOriginal,$b) -or
      -not $b.holdsOriginal.Contains($h) -or -not [object]::ReferenceEquals($w.holdOriginal,$h) -or
      -not $w.constructorTerminal){return $false}
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
      $null -ne $w.joinException -or $w.joinCountUnknown -or -not $w.pendingOriginal.returned -or
      -not $w.pendingOriginal.value -or $w.markersOriginal[11] -ne 1 -or $w.markersOriginal[12] -ne 1){return $false}
  $m=$w.markersOriginal
  if($m[4] -eq 1 -and $m[5] -ne 1){return $false}
  foreach($i in @(0,1)){
    if(($m[2*$i] -eq 1 -and $m[2*$i+1] -ne 1) -or
        ($m[2*$i+1] -eq 1 -and ($m[7+2*$i] -ne 1 -or $m[8+2*$i] -ne 1 -or $w.pinsOriginal[$i].IsAllocated))){return $false}
  }
  return $true
}

function Invoke-Cli39PythonReparse32 {
  param([Parameter(Mandatory)]$HoldOriginal)
  $h=$HoldOriginal;$b=$h.bookOriginal;$p=$b.preparationOriginal;$w=$h.reparseOriginal
  if($null -eq $w -or -not [object]::ReferenceEquals($w.holdOriginal,$h) -or -not $h.metadataReturned -or
      $h.kindOriginal -cne '32' -or -not $w.constructorTerminal -or $null -ne $w.constructorFailure -or
      $w.startAttempted -or $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -ne $b.disposalOriginal -or $null -ne $h.disposalOriginal){throw 'Actual once-only same held symbolic-link reparse phase required.'}
  Assert-Cli39PrerequisiteUseful26 $p
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.joinCount -eq [long]::MaxValue){$w.joinCountUnknown=$true;throw 'Reparse join transcript exhausted.'}
    $w.joinCount++;$w.pendingOriginal.attempted=$true;$w.pendingOriginal.returned=$false;$w.pendingOriginal.value=$false
    try{$w.pendingOriginal.value=$w.threadOriginal.Join(0);$w.pendingOriginal.returned=$true;$w.terminalJoined=$w.pendingOriginal.value}
    catch{$w.pendingOriginal.exceptionOriginal=$_;$w.joinException=$_;throw}
    if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
  }
  if(-not (Test-Cli39PythonReparseClosure32 $h) -or $w.markersOriginal[5] -ne 1 -or
      $w.markersOriginal[6] -ne 1){throw 'Actual reparse native/diagnostic/pin closure incomplete.'}
  foreach($failureOriginal in $w.exceptionsOriginal){if($null -ne $failureOriginal){throw $failureOriginal}}
  if($w.boolOriginal[0] -eq 0){throw 'Reparse request failed; output is not decoded.'}
  $w.returned=$true
  return Read-Cli39PythonReparse32 $h
}

function Read-Cli39PythonReparse32 {
  param([Parameter(Mandatory)]$HoldOriginal)
  $h=$HoldOriginal;$b=$h.bookOriginal;$p=$b.preparationOriginal;$w=$h.reparseOriginal
  if($null -eq $w -or -not $w.returned -or -not (Test-Cli39PythonReparseClosure32 $h) -or
      $null -ne $w.decodedOriginal){throw 'Actual successful once-decoded held reparse result required.'}
  $o=@{workerOriginal=$w;bytesOriginal=$w.bytesOriginal;extentBytesOriginal=$w.extentOriginal;
    returnedBytesOriginal=0U;tagOriginal=0U;payloadBytesOriginal=0;reservedOriginal=0;flagsOriginal=0U;
    substituteOffsetOriginal=0;substituteBytesOriginal=0;printOffsetOriginal=0;printBytesOriginal=0;
    substituteOriginal=$null;printOriginal=$null;returned=$false;exceptionOriginal=$null;
    state='RAW_REPARSE_BODY_NOT_TAR_TARGET_OR_SOURCE_AUTHENTICATION'}
  $w.decodedOriginal=$o
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    $bytes=$o.bytesOriginal;$o.returnedBytesOriginal=[BitConverter]::ToUInt32($o.extentBytesOriginal,0)
    if($o.returnedBytesOriginal -lt 20 -or $o.returnedBytesOriginal -gt $bytes.Length){throw 'Actual reparse extent cannot contain fixed symlink prefix.'}
    $o.tagOriginal=[BitConverter]::ToUInt32($bytes,0);$o.payloadBytesOriginal=[BitConverter]::ToUInt16($bytes,4)
    $o.reservedOriginal=[BitConverter]::ToUInt16($bytes,6)
    if($o.tagOriginal -ne [uint32]2684354572 -or $o.payloadBytesOriginal -lt 12 -or
        8+$o.payloadBytesOriginal -gt $o.returnedBytesOriginal){throw 'Actual supported symlink payload extent required.'}
    $o.substituteOffsetOriginal=[BitConverter]::ToUInt16($bytes,8);$o.substituteBytesOriginal=[BitConverter]::ToUInt16($bytes,10)
    $o.printOffsetOriginal=[BitConverter]::ToUInt16($bytes,12);$o.printBytesOriginal=[BitConverter]::ToUInt16($bytes,14)
    $o.flagsOriginal=[BitConverter]::ToUInt32($bytes,16)
    if($o.flagsOriginal -notin @([uint32]0,[uint32]1)){throw 'Unsupported actual symbolic-link flags retained.'}
    foreach($part in @(@('substitute',$o.substituteOffsetOriginal,$o.substituteBytesOriginal),
        @('print',$o.printOffsetOriginal,$o.printBytesOriginal))){
      Assert-Cli39PrerequisiteUseful26 $p
      $offset=[int]$part[1];$length=[int]$part[2]
      if($offset%2 -ne 0 -or $length%2 -ne 0 -or $offset+$length -gt $o.payloadBytesOriginal-12){
        throw 'Actual UTF16 reparse byte range lies outside returned PathBuffer.'
      }
      # Optional terminator is not part of length; never read beyond it.
      $text=[Text.UnicodeEncoding]::new($false,$false,$true).GetString($bytes,20+$offset,$length)
      if($part[0] -ceq 'substitute'){$o.substituteOriginal=$text}else{$o.printOriginal=$text}
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}
