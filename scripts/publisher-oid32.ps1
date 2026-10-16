# Fixed actual attribute OID source. Other named OID parents remain required.
function Get-Cli39ProviderOidOriginal32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal)
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$a=$AttributeOriginal
  $h=$a.oidOriginal
  if(-not (Test-Cli39OidParentMember32 $p $a) -or -not $a.decodedReturned -or
      $null -eq $h -or -not [object]::ReferenceEquals($h.attributeOriginal,$a) -or
      -not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($h.stateWorkerOriginal,$capture.stateWorkerOriginal) -or
      -not [object]::ReferenceEquals($h.reservationOriginal,$p.reservation) -or
      -not [object]::ReferenceEquals($h.budgetOriginal,$p.reservation.budget) -or
      -not [object]::ReferenceEquals($h.chargeOriginal.oidOriginal,$h) -or
      -not $h.chargeOriginal.charged -or
      -not $capture.nativeOriginal.oidReservationOriginal.recordsOriginal.Contains($h.chargeOriginal) -or
      $h.bytesExpected -ne $p.reservation.budget.publisherOidCaptureCapacityBytes -or
      $h.providerPointerOriginal -ne $a.oidPointerOriginal -or $h.providerPointerOriginal -eq [IntPtr]::Zero -or
      $h.bytesExpected -le 0 -or $h.bytesExpected -gt [int]::MaxValue){throw 'SAME actual attribute OID and complete reservation-derived capacity required.'}
  if($h.codeUnitBytes -ne $(if($a.roleOriginal -ceq 'extended-error'){2}else{1})){throw 'Actual fixed string width changed.'}
  return $h
}
function New-Cli39ProviderOidPageWorker32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$h=Get-Cli39ProviderOidOriginal32 $p $AttributeOriginal
  $root=$c.rootCaptureOriginal;$q=$root.nativeQueryOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$p.receiver) -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
      -not $capture.stateReadReturned -or -not $capture.providerCallReturned -or
      $capture.providerDataOriginal -eq [IntPtr]::Zero -or $null -ne $n.closeWorkerOriginal -or
      $null -ne $n.storageReleaseOriginal -or $null -eq $h -or
      -not $h.constructionTerminal -or $null -ne $h.constructionFailure -or
      -not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($h.stateWorkerOriginal,$capture.stateWorkerOriginal) -or
      $h.used -lt 0 -or $h.used -ge $h.bytesExpected -or
      $h.pagesOriginal.Count -ge $h.bytesExpected -or -not $q.complete -or
      $q.namesOriginal[12] -cne 'VirtualQuery' -or $null -eq $q.delegatesOriginal[12]){
    throw 'SAME original fixed provider header/page source required.'
  }
  $start=$h.providerPointerOriginal.ToInt64()
  if($start -le 0 -or $start -gt ([long]::MaxValue-$h.bytesExpected)){throw 'Complete provider header pointer extent required.'}
  if($h.terminated){throw 'Actual NUL closes all future OID reads/queries.'}
  if($h.pagesOriginal.Count -gt 0){
    $previous=$h.pagesOriginal[$h.pagesOriginal.Count-1]
    if(-not $previous.accepted -or -not $previous.readReturned -or
        $previous.offset+$previous.coveredBytes -ne $h.used){throw 'Previous actual full nonterminated OID segment must finish before next query; no retry.'}
  }
  $record=@{offset=$h.used;address=[IntPtr]::new($start+$h.used);bytesOriginal=$null;
    workerOriginal=$null;accepted=$false;baseAddress=$null;regionBytes=$null;
    state=$null;protection=$null;type=$null;coveredBytes=0;exceptionOriginal=$null;
    readerOriginal=$null;readReturned=$false;rawReadCount=0;rawStoredCount=0;terminated=$false}
  $h.pagesOriginal.Add($record)
  $w=@{phase='provider-oid-page';preparation=$p;nativeOriginal=$n;captureOriginal=$capture;
    headerOriginal=$h;oidOriginal=$h;signerOriginal=$null;certificateOriginal=$null;
    attributeOriginal=$AttributeOriginal;attributeValueOriginal=$null;
    providerPart='';
    certificatePart='';signerPart='';pageOriginal=$record;callOriginal=$q.delegatesOriginal[12];
    pinOriginal=[Runtime.InteropServices.GCHandle[]]::new(1);
    results=[int[]]::new(8);sizeOriginal=[UIntPtr[]]::new(1);exceptionOriginal=[object[]]::new(2);
    context=$null;method=$null;il=$null;body=$null;thread=$null;
    startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinPendingActive=$false;joinMultiplicityUnknown=$false;
    joinPending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    joinFalse=@{returned=$false;value=$false;repeats=0L};
    joinTerminal=@{returned=$false;value=$true};joinException=@{present=$false;original=$null};
    constructionTerminal=$false;constructionFailure=$null;completionAttempted=$false}
  $record.workerOriginal=$w
  try{
    $record.bytesOriginal=[byte[]]::new(48)
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){
      throw 'Complete genuine publisher worker geometry required.'
    }
    $w.context=[object[]]@($w.callOriginal,$record.address,$record.bytesOriginal,$w.pinOriginal,
      $w.results,$w.sizeOriginal,$w.exceptionOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedProviderOidPage',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il;$exception=$il.DeclareLocal([Exception])
    $size=$il.DeclareLocal([UIntPtr])
    function Emit-Cli39OidPageContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($Type.IsValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}
      else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39OidPageMark32([int]$Index){
      Emit-Cli39OidPageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39OidPageException32([int]$Index){
      Emit-Cli39OidPageContext32 6 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$exception);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    Emit-Cli39OidPageMark32 0
    Emit-Cli39OidPageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39OidPageContext32 2 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle])
    Emit-Cli39OidPageMark32 1
    Emit-Cli39OidPageMark32 4
    Emit-Cli39OidPageContext32 0 ($w.callOriginal.GetType());Emit-Cli39OidPageContext32 1 ([IntPtr])
    Emit-Cli39OidPageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,48);$il.Emit([Reflection.Emit.OpCodes]::Conv_U)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.callOriginal.GetType().GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$size)
    Emit-Cli39OidPageContext32 5 ([UIntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$size);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    Emit-Cli39OidPageMark32 5
    Emit-Cli39OidPageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,6)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39OidPageMark32 7
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
    Emit-Cli39OidPageException32 0
    $il.BeginFinallyBlock();$skip=$il.DefineLabel();$null=$il.BeginExceptionBlock()
    Emit-Cli39OidPageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
    Emit-Cli39OidPageMark32 2
    Emit-Cli39OidPageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
    Emit-Cli39OidPageMark32 3;$il.MarkLabel($skip)
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
    Emit-Cli39OidPageException32 1;$il.EndExceptionBlock();$il.EndExceptionBlock()
    $il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false;return $w
  }catch{$w.constructionFailure=$_;throw}finally{$w.constructionTerminal=$true}
}

function New-Cli39ProviderAttributeOid32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$a=$AttributeOriginal;$c=$p.consumerOriginal;$capture=$c.publisherNativeOriginal.providerCaptureOriginal
  if(-not (Test-Cli39OidParentMember32 $p $a) -or -not $a.decodedReturned -or
      $null -ne $a.oidOriginal -or $a.oidConstructorAttempted){throw 'Actual once-only attribute OID parent required.'}
  $a.oidConstructorAttempted=$true
  $h=@{attributeOriginal=$a;captureOriginal=$capture;stateWorkerOriginal=$capture.stateWorkerOriginal;
    reservationOriginal=$p.reservation;budgetOriginal=$p.reservation.budget;
    providerPointerOriginal=$a.oidPointerOriginal;bytesExpected=0;used=0;pagesOriginal=$null;
    codeUnitBytes=1;rawBytesOriginal=$null;chargeOriginal=$null;terminated=$false;returned=$false;constructionTerminal=$false;
    constructionFailure=$null;exceptionOriginal=$null}
  if($a.roleOriginal -ceq 'extended-error'){$h.codeUnitBytes=2}
  $a.oidOriginal=$h
  try{
    $book=$capture.nativeOriginal.oidReservationOriginal
    if($null -eq $book -or -not $book.constructionTerminal -or $null -ne $book.constructionFailure -or
        -not [object]::ReferenceEquals($book.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($book.reservationOriginal,$p.reservation) -or
        -not [object]::ReferenceEquals($book.budgetOriginal,$p.reservation.budget) -or
        -not [object]::ReferenceEquals($book.profileOriginal,$c.rootCaptureOriginal.resultOriginal.consumerProfileOriginal) -or
        $book.recordsOriginal.Count -ge $book.maximumOriginals){throw 'SAME retained disjoint OID reservation roster required before allocation.'}
    $charge=@{oidOriginal=$h;bookOriginal=$book;ordinalOriginal=$book.recordsOriginal.Count;
      chargeBytes=$book.perOidChargeBytes;charged=$false;terminalLifetime='host-process'}
    $h.chargeOriginal=$charge
    $book.recordsOriginal.Add($charge)
    [decimal]$nextCharge=$book.usedChargeBytes+$charge.chargeBytes
    if($nextCharge -gt $book.budgetOriginal.publisherOidCaptureReservationBytes){throw 'Actual retained OID aggregate exceeds SAME managed subreservation.'}
    $book.usedChargeBytes=$nextCharge;$charge.charged=$true
    $capacity=$h.budgetOriginal.publisherOidCaptureCapacityBytes
    if(($capacity -isnot [int] -and $capacity -isnot [long]) -or $capacity -le 0 -or
        $capacity -gt [int]::MaxValue){throw 'SAME complete consumer reservation OID capacity required; numeric profile remains unqualified.'}
    if($capacity -lt $h.codeUnitBytes){throw 'Actual capacity cannot contain one fixed string code unit.'}
    $h.bytesExpected=[int]$capacity
    $null=Get-Cli39ProviderOidOriginal32 $p $a
    $h.pagesOriginal=[Collections.Generic.List[object]]::new([int]$capacity)
    $h.rawBytesOriginal=[byte[]]::new([int]$capacity)
    return $h
  }catch{$h.constructionFailure=$_;throw}finally{$h.constructionTerminal=$true}
}

function Complete-Cli39ProviderOidPage32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $WorkerOriginal.preparation
  $w=$WorkerOriginal;$h=$w.oidOriginal;$r=$w.pageOriginal
  if(-not [object]::ReferenceEquals((Get-Cli39PublisherWorkerOriginal26 $w),$w) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $w.completionAttempted){throw 'Actual terminal once-only OID query required.'}
  $w.completionAttempted=$true
  try{
    foreach($failure in $w.exceptionOriginal){if($null -ne $failure){throw $failure}}
    if($w.results[0] -ne 1 -or $w.results[1] -ne 1 -or $w.results[2] -ne 1 -or
        $w.results[3] -ne 1 -or $w.results[5] -ne 1 -or $w.results[7] -ne 1 -or
        $w.sizeOriginal[0].ToUInt64() -ne 48){throw 'Incomplete actual OID query/pin disposition.'}
    $b=$r.bytesOriginal;$base=[BitConverter]::ToInt64($b,0);$region=[BitConverter]::ToUInt64($b,24)
    $state=[BitConverter]::ToUInt32($b,32);$protection=[BitConverter]::ToUInt32($b,36)
    $r.baseAddress=$base;$r.regionBytes=$region;$r.state=$state;$r.protection=$protection
    $r.type=[BitConverter]::ToUInt32($b,40);$cursor=$r.address.ToInt64()
    if($base -lt 0 -or $base -gt $cursor -or $region -eq 0 -or
        $region -gt ([long]::MaxValue-$base) -or ($base+[long]$region) -le $cursor -or
        $state -ne 0x1000 -or ($protection -band 0x100) -ne 0 -or
        ($protection -band 0xff) -notin @(2,4,8,0x20,0x40,0x80) -or $r.offset -ne $h.used){throw 'Actual current readable OID region required.'}
    $r.coveredBytes=[int][Math]::Min(($base+[long]$region-$cursor),($h.bytesExpected-$h.used))
    if($r.coveredBytes -le 0){throw 'Actual OID region must contain unread capacity.'}
    $r.accepted=$true;return $r
  }catch{$r.exceptionOriginal=$_;throw}
}

function New-Cli39ProviderOidReadWorker32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $h=Get-Cli39ProviderOidOriginal32 $p $AttributeOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($h.captureOriginal.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($h.captureOriginal.nativeOriginal,$n) -or
      -not $h.constructionTerminal -or $null -ne $h.constructionFailure -or
      $h.terminated -or $null -ne $n.closeWorkerOriginal -or $null -ne $n.storageReleaseOriginal -or
      $null -eq $h.pagesOriginal -or $h.pagesOriginal.Count -eq 0){throw 'Actual current OID page before fixed read required.'}
  $r=$h.pagesOriginal[$h.pagesOriginal.Count-1]
  if(-not $r.accepted -or $r.offset -ne $h.used -or $null -ne $r.readerOriginal -or
      $r.coveredBytes -le 0 -or $r.coveredBytes -gt ($h.bytesExpected-$h.used)){throw 'Actual once-only current OID segment required.'}
  $w=@{phase='provider-oid-read';preparation=$p;nativeOriginal=$n;captureOriginal=$n.providerCaptureOriginal;
    oidOriginal=$h;attributeOriginal=$AttributeOriginal;pageOriginal=$r;
    results=[int[]]::new(6);lastByteOriginal=[byte[]]::new(1);exceptionOriginal=[object[]]::new(1);
    context=$null;method=$null;il=$null;body=$null;thread=$null;
    startAttempted=$false;startReturned=$false;startException=$null;terminalJoined=$false;
    joinPendingActive=$false;joinMultiplicityUnknown=$false;
    joinPending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    joinFalse=@{returned=$false;value=$false;repeats=0L};joinTerminal=@{returned=$false;value=$true};
    joinException=@{present=$false;original=$null};constructionTerminal=$false;
    constructionFailure=$null;completionAttempted=$false}
  $r.readerOriginal=$w
  try{
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){throw 'Complete original OID worker stack geometry required.'}
    $w.context=[object[]]@($r.address,$h.rawBytesOriginal,$w.results,$w.exceptionOriginal,$w.lastByteOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedOidSegment',[void],[Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il
    $indexLocal=$il.DeclareLocal([int]);$byteLocal=$il.DeclareLocal([byte]);$failureLocal=$il.DeclareLocal([Exception])
    function Emit-Cli39OidReadContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($Type.IsValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39OidReadMark32([int]$Index){
      Emit-Cli39OidReadContext32 2 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $loop=$il.DefineLabel();$done=$il.DefineLabel();$next=$il.DefineLabel()
    $null=$il.BeginExceptionBlock();Emit-Cli39OidReadMark32 0
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$indexLocal)
    $il.MarkLabel($loop)
    Emit-Cli39OidReadContext32 2 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Add);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    Emit-Cli39OidReadContext32 0 ([IntPtr]);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('ReadByte',[Type[]]@([IntPtr],[int])))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$byteLocal)
    Emit-Cli39OidReadContext32 4 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$byteLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I1)
    Emit-Cli39OidReadContext32 2 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Add);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    Emit-Cli39OidReadContext32 1 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int]$r.offset)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal);$il.Emit([Reflection.Emit.OpCodes]::Add)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$byteLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I1)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Add);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$indexLocal)
    Emit-Cli39OidReadContext32 2 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$byteLocal);$il.Emit([Reflection.Emit.OpCodes]::Brtrue,$next)
    if($h.codeUnitBytes -eq 2){
      # Count includes this actual stored byte. Only an aligned full UTF16 pair
      # may terminate; previous byte may belong to the preceding queried page.
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int]$r.offset)
      $il.Emit([Reflection.Emit.OpCodes]::Add);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
      $il.Emit([Reflection.Emit.OpCodes]::And);$il.Emit([Reflection.Emit.OpCodes]::Brtrue,$next)
      Emit-Cli39OidReadContext32 1 ([byte[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int]$r.offset)
      $il.Emit([Reflection.Emit.OpCodes]::Add);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
      $il.Emit([Reflection.Emit.OpCodes]::Sub);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_U1)
      $il.Emit([Reflection.Emit.OpCodes]::Brtrue,$next)
    }
    Emit-Cli39OidReadMark32 5;$il.Emit([Reflection.Emit.OpCodes]::Br,$done)
    $il.MarkLabel($next);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$indexLocal);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int]$r.coveredBytes)
    $il.Emit([Reflection.Emit.OpCodes]::Blt,$loop)
    $il.MarkLabel($done);Emit-Cli39OidReadMark32 1
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
    Emit-Cli39OidReadContext32 3 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false;return $w
  }catch{$w.constructionFailure=$_;throw}finally{$w.constructionTerminal=$true}
}

function Complete-Cli39ProviderOidRead32 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $WorkerOriginal.preparation
  $w=$WorkerOriginal;$p=$w.preparation;$h=$w.oidOriginal;$r=$w.pageOriginal
  if(-not [object]::ReferenceEquals((Get-Cli39PublisherWorkerOriginal26 $w),$w) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $w.completionAttempted){throw 'Actual terminal once-only OID read required.'}
  $w.completionAttempted=$true
  $r.rawReadCount=$w.results[3];$r.rawStoredCount=$w.results[4];$r.terminated=($w.results[5] -eq 1)
  try{
    if($null -ne $w.exceptionOriginal[0]){throw $w.exceptionOriginal[0]}
    $count=$r.rawStoredCount
    if($w.results[0] -ne 1 -or $w.results[1] -ne 1 -or $count -le 0 -or
        $count -gt $r.coveredBytes -or $w.results[2] -ne $count -or $r.rawReadCount -ne $count -or
        $r.offset -ne $h.used -or $h.terminated -or
        ($r.terminated -and $h.rawBytesOriginal[$r.offset+$count-1] -ne 0) -or
        (-not $r.terminated -and $count -ne $r.coveredBytes)){throw 'Incomplete actual OID segment/raw progress.'}
    if($r.terminated -and $h.codeUnitBytes -eq 2 -and
        ((($r.offset+$count)%2) -ne 0 -or ($r.offset+$count) -lt 2 -or
         $h.rawBytesOriginal[$r.offset+$count-2] -ne 0)){throw 'Incomplete actual UTF16 terminal code unit.'}
    $h.used+=$count;$r.readReturned=$true
    if($r.terminated){$h.terminated=$true;$h.returned=$true}
    return $r
  }catch{$r.exceptionOriginal=$_;throw}
}

function Capture-Cli39ProviderAttributeOid32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$a=$AttributeOriginal;$h=New-Cli39ProviderAttributeOid32 $p $a
  try{
    while(-not $h.terminated){
      if($h.used -ge $h.bytesExpected){throw 'Actual OID capacity exhausted without NUL; retained incomplete.'}
      $query=New-Cli39ProviderOidPageWorker32 $p $a
      $null=Start-Cli39PublisherWorker26 $query
      while(-not $query.terminalJoined){$null=Observe-Cli39PublisherWorker26 $query;if(-not $query.terminalJoined){$null=[Threading.Thread]::Yield()}}
      $null=Complete-Cli39ProviderOidPage32 $query
      $reader=New-Cli39ProviderOidReadWorker32 $p $a
      $null=Start-Cli39PublisherWorker26 $reader
      while(-not $reader.terminalJoined){$null=Observe-Cli39PublisherWorker26 $reader;if(-not $reader.terminalJoined){$null=[Threading.Thread]::Yield()}}
      $null=Complete-Cli39ProviderOidRead32 $reader
    }
    return $h
  }catch{$h.exceptionOriginal=$_;throw}
}

function Test-Cli39ProviderAttributeOidBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal)
  $p=$OriginalPreparation;$a=$AttributeOriginal;$h=$a.oidOriginal
  if($null -eq $h){return $true}
  if(-not (Test-Cli39OidParentMember32 $p $a)){return $false}
  if(-not [object]::ReferenceEquals($h.attributeOriginal,$a) -or -not $h.constructionTerminal){return $false}
  foreach($r in $h.pagesOriginal){
    foreach($w in @($r.workerOriginal,$r.readerOriginal)){
      if($null -eq $w){continue}
      if(-not [object]::ReferenceEquals($w.oidOriginal,$h) -or
          -not [object]::ReferenceEquals($w.preparation,$p) -or -not $w.constructionTerminal -or
          $w.joinPendingActive -or $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
      if($w.startAttempted -and (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
      if($w.phase -ceq 'provider-oid-page' -and $w.startAttempted -and
          (($w.results[0] -eq 1 -and $w.results[1] -ne 1) -or
           ($w.results[1] -eq 1 -and $w.results[3] -ne 1) -or $w.pinOriginal[0].IsAllocated -or
           $null -ne $w.exceptionOriginal[1])){return $false}
    }
  }
  # Quiescent workers alone do not satisfy useful capture or required data
  # disposition. Whole provider capture remains separately false/pending.
  return $true
}

function Resolve-Cli39NamedOidPointer32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SourceOriginal,
    [Parameter(Mandatory)][ValidateSet('hash-algorithm','encryption-algorithm','sealing-algorithm','provider-usage','ctl-usage','requested-usage','revocation-algorithm','chain-usage','extended-error')][string]$Role,
    [int]$Index=-1)
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$source=$SourceOriginal
  if($Role -ceq 'extended-error'){
    $r=Get-Cli39PublisherChainRange32 $p $source
    if($Index -ne -1 -or $r.kindOriginal -cne 'element' -or $r.absenceObserved -or
        -not $r.copyReturned -or -not $r.decodedReturned -or $null -ne $r.exceptionOriginal){throw 'Actual SAME copied chain extended-error parent required.'}
    return $r.fieldsOriginal['pwszExtendedErrorInfo']
  }
  if($Role -ceq 'chain-usage'){
    $r=Get-Cli39PublisherChainRange32 $p $source
    if($r.kindOriginal -cne 'usage-pointer-array' -or $r.absenceObserved -or
        -not $r.copyReturned -or -not $r.decodedReturned -or $null -ne $r.exceptionOriginal -or
        $Index -lt 0 -or $Index -ge $r.itemsOriginal.Count){throw 'Actual SAME copied usage OID ordinal required.'}
    return $r.itemsOriginal[$Index]
  }
  if($Role -ceq 'revocation-algorithm'){
    $r=Get-Cli39PublisherChainRange32 $p $source
    if($Index -ne -1 -or $r.kindOriginal -cne 'revocation' -or
        $r.absenceObserved -or -not $r.copyReturned -or -not $r.decodedReturned -or
        $null -ne $r.exceptionOriginal){throw 'Actual SAME copied revocation OID parent required.'}
    return $r.fieldsOriginal['pszRevocationOid']
  }
  if($Role -cin @('hash-algorithm','encryption-algorithm')){
    if($Index -ne -1 -or -not (Test-Cli39ProviderSignerMember32 $capture $source) -or
        -not $source.signerInfoOriginal.copyReturned -or -not $source.signerInfoOriginal.decodedOriginal.returned){throw 'Actual SAME CMSG algorithm OID parent required.'}
    $key=if($Role -ceq 'hash-algorithm'){'HashAlgorithm.pszObjId'}else{'HashEncryptionAlgorithm.pszObjId'}
    return $source.signerInfoOriginal.decodedOriginal.fieldsOriginal[$key]
  }
  if($Role -ceq 'sealing-algorithm'){
    if($Index -ne -1 -or -not [object]::ReferenceEquals($source,$capture.providerFieldOriginals['sealing-signature']) -or
        -not $source.decodedOriginal.returned){throw 'Actual SAME sealing OID parent required.'}
    if($source.absenceObserved){return [IntPtr]::Zero}
    return $source.decodedOriginal.fieldsOriginal['signatureAlgorithm.pszObjId']
  }
  if($Role -ceq 'requested-usage'){
    if(-not [object]::ReferenceEquals($source,$capture.providerFieldOriginals['usage-oid-pointers']) -or
        -not $source.decodedOriginal.returned -or $source.absenceObserved -or $Index -lt 0 -or
        $Index -ge $source.decodedOriginal.itemsOriginal.Count){throw 'Actual SAME usage pointer-array ordinal required.'}
    return $source.decodedOriginal.itemsOriginal[$Index]
  }
  if($Index -ne -1 -or -not [object]::ReferenceEquals($source,$capture.headerOriginal) -or
      -not $source.copyReturned -or -not $source.decodedOriginal.returned){throw 'Actual SAME DATA usage OID parent required.'}
  $key=if($Role -ceq 'provider-usage'){'pszUsageOID'}else{'pszCTLSignerUsageOID'}
  return $source.decodedOriginal.fieldsOriginal[$key]
}

function Test-Cli39OidParentMember32 {
  param($OriginalPreparation,$ParentOriginal)
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$a=$ParentOriginal
  if(Test-Cli39ProviderAttributeMember32 $capture $a){return $a.decodedReturned}
  if($null -eq $a -or $null -eq $capture.namedOidParentsOriginal -or
      -not $capture.namedOidParentsOriginal.Contains($a) -or
      -not [object]::ReferenceEquals($a.captureOriginal,$capture) -or -not $a.decodedReturned){return $false}
  $pointer=Resolve-Cli39NamedOidPointer32 $p $a.sourceOriginal $a.roleOriginal $a.indexOriginal
  return ($a.oidPointerOriginal -eq $pointer)
}

function Capture-Cli39NamedProviderOid32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SourceOriginal,
    [Parameter(Mandatory)][ValidateSet('hash-algorithm','encryption-algorithm','sealing-algorithm','provider-usage','ctl-usage','requested-usage','revocation-algorithm','chain-usage','extended-error')][string]$Role,
    [int]$Index=-1)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal
  if($null -eq $capture.namedOidParentsOriginal -or
      $capture.namedOidParentsOriginal.Count -ge $capture.nativeOriginal.oidReservationOriginal.maximumOriginals){throw 'Actual retained reservation-derived named OID parent roster exhausted.'}
  foreach($prior in $capture.namedOidParentsOriginal){
    if($prior.roleOriginal -ceq $Role -and $prior.indexOriginal -eq $Index -and
        [object]::ReferenceEquals($prior.sourceOriginal,$SourceOriginal)){throw 'Named actual OID parent cannot retry/reset.'}
  }
  $a=@{captureOriginal=$capture;sourceOriginal=$SourceOriginal;roleOriginal=$Role;indexOriginal=$Index;
    oidPointerOriginal=[IntPtr]::Zero;decodedReturned=$false;oidOriginal=$null;
    oidConstructorAttempted=$false;absenceObserved=$false;exceptionOriginal=$null}
  $capture.namedOidParentsOriginal.Add($a)
  try{
    $a.oidPointerOriginal=Resolve-Cli39NamedOidPointer32 $p $SourceOriginal $Role $Index
    $a.decodedReturned=$true
    if($a.oidPointerOriginal -eq [IntPtr]::Zero){$a.absenceObserved=$true;return $a}
    $null=Capture-Cli39ProviderAttributeOid32 $p $a
    return $a
  }catch{$a.exceptionOriginal=$_;throw}
}

function Test-Cli39NamedProviderOidsBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal
  if($null -eq $capture){return $true}
  foreach($a in $capture.namedOidParentsOriginal){
    if(-not (Test-Cli39ProviderAttributeOidBorrowClosed32 $p $a)){return $false}
  }
  return $true
}
