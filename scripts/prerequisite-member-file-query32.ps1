# Fixed root-handle query. Raw SDK26100 x64 records are retained, not treated
# as namespace/exclusion or source admission. No path reopen or supplied query.
function Get-Cli39FixedFileQueryRoot32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $c=$OriginalPreparation.consumerOriginal
  if($c.slot -ceq 'cli39.source-input-host.v1'){return $OriginalPreparation.inputRead.value}
  if($c.slot -cin @('cli39.full-source-materialization.v2','cli39.windows-publisher-trust.v1','cli39.node-key-policy.v1','cli39.node-checksum-signature.v1')){return $c.rootCaptureOriginal}
  throw 'Only source-selected root/materialize/publisher/Node held-stream query routes exist.'
}

function Test-Cli39FixedFileQueryMember32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$MemberOriginal)
  $p=$OriginalPreparation;$c=$p.consumerOriginal
  if($c.slot -ceq 'cli39.source-input-host.v1'){
    $root=$p.inputRead.value
    if($null -ne $root -and $root.ContainsKey('derivedRootOriginal')){
      $derived=$root.derivedRootOriginal
      if($null -ne $derived.memberOriginal -and [object]::ReferenceEquals($derived.memberOriginal,$MemberOriginal)){
        return ([object]::ReferenceEquals($derived.hostOwnerOriginal,$p) -and
          [object]::ReferenceEquals($MemberOriginal.derivedRootOriginal,$derived) -and
          [object]::ReferenceEquals($MemberOriginal.subjectOriginal,$derived.heldSubjectOriginal) -and
          $derived.completionAttempted -and
          $derived.writeWorkerOriginal.terminalJoined -and $derived.markersOriginal[11] -eq 1 -and
          $derived.markersOriginal[15] -eq 1 -and $derived.heldSubjectOriginal.used -eq $MemberOriginal.bytesExpected)
      }
    }
    return ($null -ne $root -and $null -ne $root.heldMemberBookOriginal -and
      $root.heldMemberBookOriginal.recordsOriginal.Contains($MemberOriginal) -and
      (Test-Cli39HeldMemberSubject32 $p $MemberOriginal.subjectOriginal))
  }
  if($c.slot -ceq 'cli39.windows-publisher-trust.v1'){
    $b=$c.publisherNativeOriginal.preparedIdentityOriginal
    return ($null -ne $b -and [object]::ReferenceEquals($b.preparationOriginal,$p) -and
      $null -ne $b.recordsOriginal -and $b.recordsOriginal.Contains($MemberOriginal) -and
      [object]::ReferenceEquals($MemberOriginal.bookOriginal,$b) -and
      $MemberOriginal.roleOriginal -cin @('publisher-subject','wintrust','expected-publisher-cert') -and
      [object]::ReferenceEquals($MemberOriginal.subjectOriginal,$c.subjectOriginals[$MemberOriginal.roleOriginal]))
  }
  if($c.slot -cin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1')){
    $b=$c.nodePreparedIdentityOriginal
    return ($null -ne $b -and [object]::ReferenceEquals($b.preparationOriginal,$p) -and
      $null -ne $b.recordsOriginal -and $b.recordsOriginal.Contains($MemberOriginal) -and
      [object]::ReferenceEquals($MemberOriginal.bookOriginal,$b) -and
      $MemberOriginal.roleOriginal -cin @('executable','keyring','signature','listing') -and
      [object]::ReferenceEquals($MemberOriginal.subjectOriginal,$c.subjectOriginals[$MemberOriginal.roleOriginal]))
  }
  if($c.slot -ceq 'cli39.full-source-materialization.v2' -and $c.ContainsKey('pythonTreeOriginal')){
    $b=$c.pythonTreeOriginal
    return ($null -ne $b -and [object]::ReferenceEquals($b.preparationOriginal,$p) -and
      $null -ne $b.readsOriginal -and $b.readsOriginal.Contains($MemberOriginal) -and
      [object]::ReferenceEquals($MemberOriginal.bookOriginal,$b) -and
      $null -ne $b.holdsOriginal -and $b.holdsOriginal.Contains($MemberOriginal.holdOriginal) -and
      $null -ne $MemberOriginal.subjectOriginal)
  }
  return $false
}

function New-Cli39RootMemberFileQueryWorker32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$MemberOriginal)
  $p=$HostOwnerOriginal;$capture=Get-Cli39FixedFileQueryRoot32 $p;$m=$capture.nativeQueryOriginal
  $member=$MemberOriginal;$book=$capture.heldMemberBookOriginal;$subject=$member.subjectOriginal
  if($p.consumerOriginal.slot -ceq 'cli39.source-input-host.v1' -and
      ($null -eq $book -or -not $book.readReturned)){throw 'Actual completed held root member roster required.'}
  if($p.consumerOriginal.slot -ceq 'cli39.full-source-materialization.v2' -and
      (-not $subject.eof -or $subject.used -ne $member.bytesExpected)){
    throw 'Actual complete fixed output read required before its native query.'
  }
  if($null -ne $member.derivedRootOriginal -and $null -ne $member.derivedRootOriginal.disposalOriginal){throw 'Original derived-root future-start barrier denies query construction.'}
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $m.complete -or $null -ne $member.fileQueryOriginal -or
      -not (Test-Cli39FixedFileQueryMember32 $p $member) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -eq $subject.stream -or $null -eq $subject.handleOriginal -or
      $subject.handleOriginal.IsClosed -or
      -not [object]::ReferenceEquals($subject.stream.SafeFileHandle,$subject.handleOriginal)){
    throw 'SAME once-only held root handle and fixed query ABI required.'
  }
  $w=@{phase='root-member-file-information';hostOwnerOriginal=$p;queryOriginal=$m;memberOriginal=$member;
    subjectOriginal=$subject;handleOriginal=$subject.handleOriginal;
    buffersOriginal=$null;pinsOriginal=$null;classesOriginal=$null;sizesOriginal=$null;
    borrowOriginal=[bool[]]::new(1);handleValueOriginal=[IntPtr[]]::new(1);
    borrowMarkers=[int[]]::new(4);pinMarkers=[int[]]::new(16);
    callMarkers=[int[]]::new(8);resultsOriginal=[int[]]::new(4);
    errorsOriginal=[int[]]::new(4);exceptionsOriginal=[object[]]::new(6);
    context=$null;methodOriginal=$null;ilOriginal=$null;bodyOriginal=$null;threadOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;startAttempted=$false;
    startReturned=$false;startException=$null;terminalJoined=$false;
    pendingActive=$false;multiplicityUnknown=$false;joinException=$null;
    pending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    repeatedFalse=@{returned=$false;repeats=0L};completionAttempted=$false;
    completionReturned=$false;rawReturned=$false;decodeOriginal=$null}
  $member.fileQueryOriginal=$w
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids new root query construction.'}
    # FILE_BASIC_INFO40 / STANDARD_INFO24 / ATTRIBUTE_TAG_INFO8 / ID_INFO24.
    # Fixed class ordinals0,1,9,18; FILE_ID_INFO retains all volume8 + ID12816.
    $w.classesOriginal=[int[]]@(0,1,9,18);$w.sizesOriginal=[int[]]@(40,24,8,24)
    $w.buffersOriginal=[object[]]::new(4)
    for($i=0;$i -lt 4;$i++){$w.buffersOriginal[$i]=[byte[]]::new($w.sizesOriginal[$i])}
    $w.pinsOriginal=[Runtime.InteropServices.GCHandle[]]::new(4)
    $w.context=[object[]]@($w.handleOriginal,$m.delegatesOriginal[0],$w.buffersOriginal,
      $w.pinsOriginal,$w.borrowOriginal,$w.handleValueOriginal,$w.borrowMarkers,
      $w.pinMarkers,$w.callMarkers,$w.resultsOriginal,$w.errorsOriginal,$w.exceptionsOriginal)
    $w.methodOriginal=[Reflection.Emit.DynamicMethod]::new('Cli39FixedRootMemberFileQuery',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.methodOriginal.GetILGenerator();$w.ilOriginal=$il
    $failure=$il.DeclareLocal([Exception]);$result=$il.DeclareLocal([int])
    function Emit-Cli39MemberFileQueryContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
    }
    function Emit-Cli39MemberFileQueryMark32([int]$ContextIndex,[int]$Index){
      Emit-Cli39MemberFileQueryContext32 $ContextIndex ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39MemberFileQueryError32([int]$Index){
      Emit-Cli39MemberFileQueryContext32 11 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failure);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    Emit-Cli39MemberFileQueryMark32 6 0
    Emit-Cli39MemberFileQueryContext32 0 ([Microsoft.Win32.SafeHandles.SafeFileHandle])
    Emit-Cli39MemberFileQueryContext32 4 ([bool[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[bool])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Runtime.InteropServices.SafeHandle].GetMethod('DangerousAddRef',[Type[]]@([bool].MakeByRefType())))
    Emit-Cli39MemberFileQueryMark32 6 1
    Emit-Cli39MemberFileQueryContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39MemberFileQueryContext32 0 ([Microsoft.Win32.SafeHandles.SafeFileHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Runtime.InteropServices.SafeHandle].GetMethod('DangerousGetHandle',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    for($i=0;$i -lt 4;$i++){
      Emit-Cli39MemberFileQueryMark32 7 ($i*4)
      Emit-Cli39MemberFileQueryContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      Emit-Cli39MemberFileQueryContext32 2 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle])
      Emit-Cli39MemberFileQueryMark32 7 ($i*4+1)
      Emit-Cli39MemberFileQueryMark32 8 ($i*2)
      Emit-Cli39MemberFileQueryContext32 1 $m.delegateTypesOriginal[0]
      Emit-Cli39MemberFileQueryContext32 5 ([IntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$w.classesOriginal[$i])
      Emit-Cli39MemberFileQueryContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$w.sizesOriginal[$i])
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$m.delegateTypesOriginal[0].GetMethod('Invoke'))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$result)
      Emit-Cli39MemberFileQueryContext32 9 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$result);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
      Emit-Cli39MemberFileQueryMark32 8 ($i*2+1)
      Emit-Cli39MemberFileQueryContext32 10 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
    Emit-Cli39MemberFileQueryError32 0
    $il.BeginFinallyBlock()
    # Independent cleanup blocks: one throwing Free never skips later pins or
    # the actual SafeHandle borrow release. No uncertain Free is retried.
    for($i=0;$i -lt 4;$i++){
      $skip=$il.DefineLabel();$null=$il.BeginExceptionBlock()
      $callAbsent=$il.DefineLabel()
      Emit-Cli39MemberFileQueryContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($i*2))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callAbsent)
      Emit-Cli39MemberFileQueryContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($i*2+1))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      $il.MarkLabel($callAbsent)
      Emit-Cli39MemberFileQueryContext32 7 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($i*4+1))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
      Emit-Cli39MemberFileQueryMark32 7 ($i*4+2)
      Emit-Cli39MemberFileQueryContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$i)
      $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
      Emit-Cli39MemberFileQueryMark32 7 ($i*4+3);$il.MarkLabel($skip)
      $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
      Emit-Cli39MemberFileQueryError32 ($i+1);$il.EndExceptionBlock()
    }
    $skipRelease=$il.DefineLabel();$null=$il.BeginExceptionBlock()
    for($queryIndex=0;$queryIndex -lt 4;$queryIndex++){
      $callAbsent=$il.DefineLabel()
      Emit-Cli39MemberFileQueryContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($queryIndex*2))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$callAbsent)
      Emit-Cli39MemberFileQueryContext32 8 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,($queryIndex*2+1))
      $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skipRelease)
      $il.MarkLabel($callAbsent)
    }
    Emit-Cli39MemberFileQueryContext32 4 ([bool[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_U1);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skipRelease)
    Emit-Cli39MemberFileQueryMark32 6 2
    Emit-Cli39MemberFileQueryContext32 0 ([Microsoft.Win32.SafeHandles.SafeFileHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Runtime.InteropServices.SafeHandle].GetMethod('DangerousRelease',[Type[]]@()))
    Emit-Cli39MemberFileQueryMark32 6 3;$il.MarkLabel($skipRelease)
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
    Emit-Cli39MemberFileQueryError32 5;$il.EndExceptionBlock()
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.bodyOriginal=$w.methodOriginal.CreateDelegate([Threading.ParameterizedThreadStart])
    if($p.consumerOriginal.slot -ceq 'cli39.full-source-materialization.v2'){
      $stack=$p.consumerOriginal.geometry.workerStackBytes
      if($stack -le 0 -or $stack -gt [int]::MaxValue){throw 'SAME admitted materialize worker stack required.'}
      $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal,[int]$stack)
    }elseif($p.consumerOriginal.slot -cin @('cli39.windows-publisher-trust.v1','cli39.node-key-policy.v1','cli39.node-checksum-signature.v1')){
      $stack=$p.consumerOriginal.geometry.workerStackBytes
      if($stack -le 0 -or $stack -gt [int]::MaxValue){throw 'SAME admitted fixed publisher/Node query worker stack required.'}
      $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal,[int]$stack)
    }elseif($capture.ContainsKey('derivedRootOriginal') -and
        [object]::ReferenceEquals($capture.derivedRootOriginal.memberOriginal,$member)){
      $stack=$capture.derivedRootOriginal.profileOriginal.cliDerivedRootWorkerStackBytes
      if($stack -le 0 -or $stack -gt [int]::MaxValue){throw 'SAME selected derived query worker stack required.'}
      $w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal,[int]$stack)
    }else{$w.threadOriginal=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.bodyOriginal)}
    $w.threadOriginal.IsBackground=$false
    return $w
  }catch{$w.constructorFailure=$_;throw}finally{$w.constructorTerminal=$true}
}

function Invoke-Cli39RootMemberFileQuery32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$p=$w.hostOwnerOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not (Test-Cli39FixedFileQueryMember32 $p $w.memberOriginal) -or
      -not [object]::ReferenceEquals($w.memberOriginal.fileQueryOriginal,$w) -or
      -not [object]::ReferenceEquals($w.memberOriginal.subjectOriginal,$w.subjectOriginal) -or
      -not [object]::ReferenceEquals($w.subjectOriginal.handleOriginal,$w.handleOriginal) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      -not $w.constructorTerminal -or $null -ne $w.constructorFailure -or $w.startAttempted){
    throw 'SAME once-only root file query worker required.'
  }
  if($null -ne $w.memberOriginal.derivedRootOriginal -and $null -ne $w.memberOriginal.derivedRootOriginal.disposalOriginal){throw 'Original derived-root future-start barrier denies query start.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline forbids root file query.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.context);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){throw 'Uncertain actual root file query join remains retained.'}
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
  if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $w.memberOriginal)){throw 'Actual root file query cleanup is incomplete.'}
  foreach($exception in $w.exceptionsOriginal){if($null -ne $exception){throw $exception}}
  for($i=0;$i -lt 4;$i++){
    if($w.callMarkers[$i*2] -ne 1 -or $w.callMarkers[$i*2+1] -ne 1 -or
        $w.resultsOriginal[$i] -eq 0){throw 'Actual native root information query failed; raw BOOL/error retained.'}
  }
  $w.rawReturned=$true;$w.completionReturned=$true
  # Full FILE_ID_128 and other raw records are only actual observations of the
  # held root. Principal/rights/reparse ancestors/source/loaded/tree joins and
  # genuine whole startup allocation remain owed before input admission.
  return $w
}

function Test-Cli39RootMemberFileQueryClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$MemberOriginal)
  $p=$HostOwnerOriginal
  if(-not (Test-Cli39FixedFileQueryMember32 $p $MemberOriginal)){return $false}
  $w=$MemberOriginal.fileQueryOriginal
  if($null -eq $w){return $true}
  if(-not [object]::ReferenceEquals($w.memberOriginal,$MemberOriginal) -or
      -not [object]::ReferenceEquals($w.subjectOriginal,$MemberOriginal.subjectOriginal)){return $false}
  if($null -eq $w -or -not [object]::ReferenceEquals($w.hostOwnerOriginal,$p) -or
      -not $w.constructorTerminal){return $false}
  # Constructor errors retain their graph, but no worker can acquire a pin or
  # SafeHandle borrow before Start. An attempted Start stays on the joined path.
  if(-not $w.startAttempted){return ($null -eq $w.threadOriginal -or -not $w.threadOriginal.IsAlive)}
  if($null -eq $w.threadOriginal){return $false}
  if(-not $w.terminalJoined -or $w.threadOriginal.IsAlive -or $w.pendingActive -or
      $w.multiplicityUnknown -or $null -ne $w.joinException){return $false}
  if($w.borrowMarkers[0] -eq 1 -and $w.borrowMarkers[1] -ne 1){return $false}
  if($w.borrowOriginal[0] -and ($w.borrowMarkers[2] -ne 1 -or $w.borrowMarkers[3] -ne 1)){return $false}
  for($i=0;$i -lt 4;$i++){
    if($w.pinMarkers[$i*4] -eq 1 -and $w.pinMarkers[$i*4+1] -ne 1){return $false}
    if($w.pinMarkers[$i*4+1] -eq 1 -and
        ($w.pinMarkers[$i*4+2] -ne 1 -or $w.pinMarkers[$i*4+3] -ne 1)){return $false}
    if($null -ne $w.exceptionsOriginal[$i+1]){return $false}
  }
  return ($null -eq $w.exceptionsOriginal[5])
}

function Test-Cli39AllRootMemberFileQueriesClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or $null -eq $root.heldMemberBookOriginal){return $true}
  foreach($member in $root.heldMemberBookOriginal.recordsOriginal){
    if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $member)){return $false}
  }
  if($root.ContainsKey('derivedRootOriginal') -and $null -ne $root.derivedRootOriginal.memberOriginal -and
      -not (Test-Cli39RootMemberFileQueryClosure32 $p $root.derivedRootOriginal.memberOriginal)){return $false}
  return $true
}

function Complete-Cli39RootMemberFileInformation32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$p=$w.hostOwnerOriginal;$member=$w.memberOriginal
  if(-not (Test-Cli39FixedFileQueryMember32 $p $member) -or
      -not [object]::ReferenceEquals($member.fileQueryOriginal,$w) -or
      -not $w.rawReturned -or -not $w.completionReturned -or $null -ne $w.decodeOriginal -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only complete member information required.'}
  $d=@{workerOriginal=$w;memberOriginal=$member;buffersOriginal=$w.buffersOriginal;
    creationTimeOriginal=0L;lastAccessTimeOriginal=0L;lastWriteTimeOriginal=0L;changeTimeOriginal=0L;
    basicAttributesOriginal=0U;allocationSizeOriginal=0L;endOfFileOriginal=0L;numberOfLinksOriginal=0U;
    deletePendingRawOriginal=0;directoryRawOriginal=0;tagAttributesOriginal=0U;reparseTagOriginal=0U;
    volumeSerialOriginal=0UL;fileIdBufferOriginal=$null;fileIdOffsetOriginal=8;fileIdBytesOriginal=16;
    heldExtentEqual=$false;returned=$false;exceptionOriginal=$null}
  $w.decodeOriginal=$d
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline ended member information decode.'}
    $b=$w.buffersOriginal
    if($b.Count -ne 4 -or $b[0].Length -ne 40 -or $b[1].Length -ne 24 -or
        $b[2].Length -ne 8 -or $b[3].Length -ne 24){throw 'Actual fixed member information backing differs.'}
    $d.creationTimeOriginal=[BitConverter]::ToInt64($b[0],0)
    $d.lastAccessTimeOriginal=[BitConverter]::ToInt64($b[0],8)
    $d.lastWriteTimeOriginal=[BitConverter]::ToInt64($b[0],16)
    $d.changeTimeOriginal=[BitConverter]::ToInt64($b[0],24)
    $d.basicAttributesOriginal=[BitConverter]::ToUInt32($b[0],32)
    $d.allocationSizeOriginal=[BitConverter]::ToInt64($b[1],0)
    $d.endOfFileOriginal=[BitConverter]::ToInt64($b[1],8)
    $d.numberOfLinksOriginal=[BitConverter]::ToUInt32($b[1],16)
    $d.deletePendingRawOriginal=$b[1][20];$d.directoryRawOriginal=$b[1][21]
    $d.tagAttributesOriginal=[BitConverter]::ToUInt32($b[2],0)
    $d.reparseTagOriginal=[BitConverter]::ToUInt32($b[2],4)
    $d.volumeSerialOriginal=[BitConverter]::ToUInt64($b[3],0)
    $d.fileIdBufferOriginal=$b[3]
    $d.heldExtentEqual=($d.endOfFileOriginal -eq $member.bytesExpected)
    $d.returned=$true
    # Raw full ID remains the same 24-byte backing at offset8/extent16.
    # EOF equality alone supplies no exclusion, mapping or provenance authority.
    return $d
  }catch{$d.exceptionOriginal=$_;throw}
}
