# Fixed book for source-selected supported chain-result ranges. No native effect.
function New-Cli39PublisherChainBook32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $profile=$c.rootCaptureOriginal.resultOriginal.consumerProfileOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $n.ContainsKey('chainReservationOriginal') -or $null -eq $profile -or
      -not $profile.returned -or $profile.role -cne 'consumerBudget' -or
      $profile.actionOriginal -cne 'publisher' -or
      -not $c.rootCaptureOriginal.profileOriginals.Contains($profile) -or
      -not [object]::ReferenceEquals($profile.valuesOriginal,$p.reservation.budget)){
    throw 'SAME once-only typed publisher chain reservation required.'
  }
  $v=$profile.valuesOriginal
  $book=@{preparationOriginal=$p;profileOriginal=$profile;reservationOriginal=$p.reservation;
    budgetOriginal=$p.reservation.budget;recordsOriginal=$null;maximumOriginals=0;
    usedChargeBytes=[decimal]0;fixedChargeBytes=[decimal]$v.publisherChainCaptureFixedChargeBytes;
    perByteChargeBytes=[decimal]$v.publisherChainCapturePerByteChargeBytes;
    constructionTerminal=$false;constructionFailure=$null;terminalLifetime='host-process';
    traversalOriginal=$null;rangeConstructionUnknown=$false;rangeConstructionFailure=$null;pendingParentOriginal=$null;pendingRoleOriginal=$null;pendingIndexOriginal=-1;
    state='SOURCE_BOOK_ONLY_NATIVE_RANGE_COLLECTOR_AND_GENUINE_PROFILE_PENDING'}
  $n.chainReservationOriginal=$book
  try{
    $book.usedChargeBytes=[decimal]$v.publisherChainCaptureBookChargeBytes
    [decimal]$maximum=[decimal]::Floor((([decimal]$v.publisherChainCaptureReservationBytes)-$book.usedChargeBytes)/$book.fixedChargeBytes)
    if($maximum -le 0 -or $maximum -gt [int]::MaxValue){throw 'Supported reservation-derived actual chain record roster required.'}
    $book.maximumOriginals=[int]$maximum
    $book.recordsOriginal=[Collections.Generic.List[object]]::new($book.maximumOriginals)
    return $book
  }catch{$book.constructionFailure=$_;throw}finally{$book.constructionTerminal=$true}
}

function Resolve-Cli39PublisherChainRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ParentOriginal,
    [Parameter(Mandatory)][ValidateSet('chain-context','simple-chain-array','lower-context-array','simple-chain','lower-context','element-array','element','revocation','crl-info','base-crl','delta-crl','crl-der','certificate-context','certificate-der','issuance-usage','application-usage','usage-pointer-array','trust-list','ctl-context','ctl-encoded','ctl-content')][string]$Role,[int]$Index=-1)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$capture=$n.providerCaptureOriginal
  $book=$n.chainReservationOriginal;$parent=$ParentOriginal
  if(-not [object]::ReferenceEquals($book.preparationOriginal,$p) -or
      -not $book.constructionTerminal -or $null -ne $book.constructionFailure){throw 'SAME actual chain book required.'}
  [IntPtr]$pointer=[IntPtr]::Zero;[long]$extent=0;$kind=$Role
  if($Role -ceq 'chain-context'){
    if($Index -ne -1 -or -not (Test-Cli39ProviderSignerMember32 $capture $parent) -or
        -not $parent.headerRangeOriginal.copyReturned -or -not $parent.headerRangeOriginal.decodedOriginal.returned){throw 'Actual copied SAME signer chain parent required.'}
    $pointer=$parent.headerRangeOriginal.decodedOriginal.fieldsOriginal['pChainContext'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{72L}
  }else{
    if(-not $book.recordsOriginal.Contains($parent) -or
        -not [object]::ReferenceEquals($parent.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($parent.bookOriginal,$book) -or
        -not $parent.copyReturned -or -not $parent.decodedReturned){throw 'Actual copied decoded SAME chain parent required.'}
    $f=$parent.fieldsOriginal
    switch -CaseSensitive ($Role){
      'simple-chain-array'{
        if($parent.kindOriginal -cne 'chain-context' -or $Index -ne -1){throw 'Actual chain-context array parent required.'}
        $pointer=$f['rgpChain'];$extent=8L*[long][uint32]$f['cChain']
      }
      'lower-context-array'{
        if($parent.kindOriginal -cne 'chain-context' -or $Index -ne -1){throw 'Actual lower-context array parent required.'}
        $pointer=$f['rgpLowerQualityChainContext'];$extent=8L*[long][uint32]$f['cLowerQualityChainContext']
      }
      'simple-chain'{
        if($parent.kindOriginal -cne 'simple-chain-array' -or $Index -lt 0 -or $Index -ge $parent.itemsOriginal.Count){throw 'Actual simple-chain pointer ordinal required.'}
        $pointer=$parent.itemsOriginal[$Index];$extent=40L
      }
      'lower-context'{
        if($parent.kindOriginal -cne 'lower-context-array' -or $Index -lt 0 -or $Index -ge $parent.itemsOriginal.Count){throw 'Actual lower-context pointer ordinal required.'}
        $pointer=$parent.itemsOriginal[$Index];$extent=72L;$kind='chain-context'
      }
      'element-array'{
        if($parent.kindOriginal -cne 'simple-chain' -or $Index -ne -1){throw 'Actual simple-chain element array parent required.'}
        $pointer=$f['rgpElement'];$extent=8L*[long][uint32]$f['cElement']
      }
      'element'{
        if($parent.kindOriginal -cne 'element-array' -or $Index -lt 0 -or $Index -ge $parent.itemsOriginal.Count){throw 'Actual chain-element ordinal required.'}
        $pointer=$parent.itemsOriginal[$Index];$extent=56L
      }
      'revocation'{
        if($parent.kindOriginal -cne 'element' -or $Index -ne -1){throw 'Actual element revocation parent required.'}
        $pointer=$f['pRevocationInfo'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{40L}
      }
      'crl-info'{
        if($parent.kindOriginal -cne 'revocation' -or $Index -ne -1){throw 'Actual revocation CRL parent required.'}
        $pointer=$f['pCrlInfo'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{40L}
      }
      'base-crl'{
        if($parent.kindOriginal -cne 'crl-info' -or $Index -ne -1){throw 'Actual base CRL parent required.'}
        $pointer=$f['pBaseCrlContext'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{40L};$kind='crl-context'
      }
      'delta-crl'{
        if($parent.kindOriginal -cne 'crl-info' -or $Index -ne -1){throw 'Actual delta CRL parent required.'}
        $pointer=$f['pDeltaCrlContext'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{40L};$kind='crl-context'
      }
      'crl-der'{
        if($parent.kindOriginal -cne 'crl-context' -or $Index -ne -1){throw 'Actual encoded CRL parent required.'}
        $pointer=$f['pbCrlEncoded'];$extent=[long][uint32]$f['cbCrlEncoded']
      }
      'certificate-context'{
        if($parent.kindOriginal -cne 'element' -or $Index -ne -1){throw 'Actual element certificate parent required.'}
        $pointer=$f['pCertContext'];$extent=40L
      }
      {$_ -cin @('issuance-usage','application-usage')}{
        if($parent.kindOriginal -cne 'element' -or $Index -ne -1){throw 'Actual chain element usage parent required.'}
        $key=if($Role -ceq 'issuance-usage'){'pIssuanceUsage'}else{'pApplicationUsage'}
        $pointer=$f[$key];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{16L};$kind='usage'
      }
      'usage-pointer-array'{
        if($parent.kindOriginal -cne 'usage' -or $Index -ne -1){throw 'Actual usage pointer-array parent required.'}
        $pointer=$f['rgpszUsageIdentifier'];$extent=8L*[long][uint32]$f['cUsageIdentifier']
      }
      'trust-list'{
        if($parent.kindOriginal -cne 'simple-chain' -or $Index -ne -1){throw 'Actual simple-chain trust-list parent required.'}
        $pointer=$f['pTrustListInfo'];$extent=if($pointer -eq [IntPtr]::Zero){0L}else{24L}
      }
      'ctl-context'{
        if($parent.kindOriginal -cne 'trust-list' -or $Index -ne -1){throw 'Actual trust-list CTL parent required.'}
        $pointer=$f['pCtlContext'];$extent=64L
      }
      {$_ -cin @('ctl-encoded','ctl-content')}{
        if($parent.kindOriginal -cne 'ctl-context' -or $Index -ne -1){throw 'Actual CTL encoded/content parent required.'}
        $pointerKey=if($Role -ceq 'ctl-encoded'){'pbCtlEncoded'}else{'pbCtlContent'}
        $lengthKey=if($Role -ceq 'ctl-encoded'){'cbCtlEncoded'}else{'cbCtlContent'}
        $pointer=$f[$pointerKey];$extent=[long][uint32]$f[$lengthKey]
      }
      'certificate-der'{
        if($parent.kindOriginal -cne 'certificate-context' -or $Index -ne -1){throw 'Actual encoded certificate parent required.'}
        $pointer=$f['pbCertEncoded'];$extent=[long][uint32]$f['cbCertEncoded']
      }
    }
  }
  if($extent -gt [int]::MaxValue -or ($extent -gt 0 -and ($pointer -eq [IntPtr]::Zero -or
      $pointer.ToInt64() -lt 0 -or $extent -gt ([long]::MaxValue-$pointer.ToInt64())))){
    throw 'Actual complete chain range is unsupported/inconsistent; retain incomplete original.'
  }
  return @{pointerOriginal=$pointer;extentOriginal=$extent;kindOriginal=$kind}
}

function New-Cli39PublisherChainRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ParentOriginal,
    [Parameter(Mandatory)][ValidateSet('chain-context','simple-chain-array','lower-context-array','simple-chain','lower-context','element-array','element','revocation','crl-info','base-crl','delta-crl','crl-der','certificate-context','certificate-der','issuance-usage','application-usage','usage-pointer-array','trust-list','ctl-context','ctl-encoded','ctl-content')][string]$Role,[int]$Index=-1)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$book=$n.chainReservationOriginal
  if(-not [object]::ReferenceEquals($book.preparationOriginal,$p) -or
      -not $book.constructionTerminal -or $null -ne $book.constructionFailure -or
      $book.rangeConstructionUnknown -or $null -ne $book.rangeConstructionFailure -or $book.recordsOriginal.Count -ge $book.maximumOriginals){throw 'SAME available fixed chain book required.'}
  Observe-OwnedPrerequisiteDeadline $p
  foreach($prior in $book.recordsOriginal){
    if([object]::ReferenceEquals($prior.parentOriginal,$ParentOriginal) -and
        $prior.roleOriginal -ceq $Role -and $prior.indexOriginal -eq $Index){throw 'Actual chain parent/role/ordinal cannot retry.'}
  }
  [decimal]$fixedNext=$book.usedChargeBytes+$book.fixedChargeBytes
  if($fixedNext -gt $book.budgetOriginal.publisherChainCaptureReservationBytes){throw 'Actual chain fixed charge exhausted.'}
  $book.usedChargeBytes=$fixedNext;$book.rangeConstructionUnknown=$true
  $book.pendingParentOriginal=$ParentOriginal;$book.pendingRoleOriginal=$Role;$book.pendingIndexOriginal=$Index
  try{
    $r=@{preparationOriginal=$p;bookOriginal=$book;parentOriginal=$ParentOriginal;roleOriginal=$Role;indexOriginal=$Index;
      kindOriginal=$null;pointerOriginal=[IntPtr]::Zero;providerPointerOriginal=[IntPtr]::Zero;bytesExpected=0;used=0;
      captureOriginal=$n.providerCaptureOriginal;stateWorkerOriginal=$n.providerCaptureOriginal.stateWorkerOriginal;
      backingConstructorAttempted=$false;backingConstructorTerminal=$false;backingConstructorFailure=$null;fixedChargeBytes=$book.fixedChargeBytes;
      byteChargeBytes=[decimal]0;charged=$false;rawBytesOriginal=$null;pagesOriginal=$null;
      copyWorkerOriginal=$null;rangeReturned=$false;copyReturned=$false;decodedReturned=$false;
      fieldsOriginal=$null;itemsOriginal=$null;aliasOriginal=$null;aliasReturned=$false;absenceObserved=$false;expansionAttempted=$false;expansionReturned=$false;expansionFailure=$null;
      constructionTerminal=$false;constructionFailure=$null;exceptionOriginal=$null}
    $book.recordsOriginal.Add($r);$book.rangeConstructionUnknown=$false
    try{
      $resolved=Resolve-Cli39PublisherChainRange32 $p $ParentOriginal $Role $Index
      $r.kindOriginal=$resolved.kindOriginal;$r.pointerOriginal=$resolved.pointerOriginal;$r.providerPointerOriginal=$resolved.pointerOriginal;$r.bytesExpected=$resolved.extentOriginal
      Observe-OwnedPrerequisiteDeadline $p
  foreach($prior in $book.recordsOriginal){
        if(-not [object]::ReferenceEquals($prior,$r) -and $r.bytesExpected -gt 0 -and
            $prior.kindOriginal -ceq $r.kindOriginal -and $prior.pointerOriginal -eq $r.pointerOriginal -and
            $prior.bytesExpected -eq $r.bytesExpected){
          if(-not $prior.charged -or -not $prior.constructionTerminal -or $null -ne $prior.constructionFailure){throw 'Actual alias has incomplete canonical construction.'}
          $r.aliasOriginal=$prior;break}
      }
      if($null -eq $r.aliasOriginal){
        $r.byteChargeBytes=[decimal]$r.bytesExpected*$book.perByteChargeBytes
        [decimal]$next=$book.usedChargeBytes+$r.byteChargeBytes
        if($next -gt $book.budgetOriginal.publisherChainCaptureReservationBytes){throw 'Actual chain bytes/workers exceed SAME reservation.'}
        $book.usedChargeBytes=$next
      }
      $r.charged=$true;return $r
    }catch{$r.constructionFailure=$_;throw}finally{$r.constructionTerminal=$true}
  }catch{$book.rangeConstructionFailure=$_;throw}
}



function Get-Cli39PublisherChainRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ChainOriginal)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$book=$n.chainReservationOriginal;$r=$ChainOriginal
  if(-not $book.recordsOriginal.Contains($r) -or
      -not [object]::ReferenceEquals($r.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($r.bookOriginal,$book) -or
      -not [object]::ReferenceEquals($r.captureOriginal,$n.providerCaptureOriginal) -or
      -not $r.charged -or -not $r.constructionTerminal -or $null -ne $r.constructionFailure){throw 'SAME actual charged chain range original required.'}
  $resolved=Resolve-Cli39PublisherChainRange32 $p $r.parentOriginal $r.roleOriginal $r.indexOriginal
  if($r.pointerOriginal -ne $resolved.pointerOriginal -or $r.providerPointerOriginal -ne $resolved.pointerOriginal -or
      $r.bytesExpected -ne $resolved.extentOriginal -or $r.kindOriginal -cne $resolved.kindOriginal){throw 'Actual chain source parent range association changed.'}
  return $r
}
function New-Cli39PublisherChainPageWorker32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ChainOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$h=Get-Cli39PublisherChainRange32 $p $ChainOriginal
  $root=$c.rootCaptureOriginal;$q=$root.nativeQueryOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$p.receiver) -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
      -not $capture.stateReadReturned -or -not $capture.providerCallReturned -or
      $capture.providerDataOriginal -eq [IntPtr]::Zero -or $null -ne $n.closeWorkerOriginal -or
      $null -ne $n.storageReleaseOriginal -or $null -eq $h -or
      -not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($h.stateWorkerOriginal,$capture.stateWorkerOriginal) -or
      $null -ne $h.aliasOriginal -or -not $h.backingConstructorTerminal -or $null -ne $h.backingConstructorFailure -or
      $h.used -lt 0 -or $h.used -ge $h.bytesExpected -or
      $h.pagesOriginal.Count -ge $h.bytesExpected -or -not $q.complete -or
      $q.namesOriginal[12] -cne 'VirtualQuery' -or $null -eq $q.delegatesOriginal[12]){
    throw 'SAME original fixed provider header/page source required.'
  }
  $start=$h.providerPointerOriginal.ToInt64()
  if($start -le 0 -or $start -gt ([long]::MaxValue-$h.bytesExpected)){throw 'Complete provider header pointer extent required.'}
  if($h.pagesOriginal.Count -gt 0 -and -not $h.pagesOriginal[$h.pagesOriginal.Count-1].accepted){
    throw 'Previous actual page must be accepted before another query; no retry.'
  }
  $record=@{offset=$h.used;address=[IntPtr]::new($start+$h.used);bytesOriginal=$null;
    workerOriginal=$null;accepted=$false;baseAddress=$null;regionBytes=$null;
    state=$null;protection=$null;type=$null;coveredBytes=0;exceptionOriginal=$null}
  $h.pagesOriginal.Add($record)
  $w=@{phase='provider-chain-page';preparation=$p;nativeOriginal=$n;captureOriginal=$capture;
    headerOriginal=$h;chainOriginal=$ChainOriginal;pageOriginal=$record;callOriginal=$q.delegatesOriginal[12];
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
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedChainPage',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il;$exception=$il.DeclareLocal([Exception])
    $size=$il.DeclareLocal([UIntPtr])
    function Emit-Cli39ChainPageContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($Type.IsValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}
      else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39ChainPageMark32([int]$Index){
      Emit-Cli39ChainPageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    function Emit-Cli39ChainPageException32([int]$Index){
      Emit-Cli39ChainPageContext32 6 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$exception);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
    $null=$il.BeginExceptionBlock()
    Emit-Cli39ChainPageMark32 0
    Emit-Cli39ChainPageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    Emit-Cli39ChainPageContext32 2 ([byte[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][Runtime.InteropServices.GCHandleType]::Pinned)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Alloc',[Type[]]@([object],[Runtime.InteropServices.GCHandleType])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem,[Runtime.InteropServices.GCHandle])
    Emit-Cli39ChainPageMark32 1
    Emit-Cli39ChainPageMark32 4
    Emit-Cli39ChainPageContext32 0 ($w.callOriginal.GetType());Emit-Cli39ChainPageContext32 1 ([IntPtr])
    Emit-Cli39ChainPageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('AddrOfPinnedObject',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,48);$il.Emit([Reflection.Emit.OpCodes]::Conv_U)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$w.callOriginal.GetType().GetMethod('Invoke'))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$size)
    Emit-Cli39ChainPageContext32 5 ([UIntPtr[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$size);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
    Emit-Cli39ChainPageMark32 5
    Emit-Cli39ChainPageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,6)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4);Emit-Cli39ChainPageMark32 7
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
    Emit-Cli39ChainPageException32 0
    $il.BeginFinallyBlock();$skip=$il.DefineLabel();$null=$il.BeginExceptionBlock()
    Emit-Cli39ChainPageContext32 4 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_I4);$il.Emit([Reflection.Emit.OpCodes]::Brfalse,$skip)
    Emit-Cli39ChainPageMark32 2
    Emit-Cli39ChainPageContext32 3 ([Runtime.InteropServices.GCHandle[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelema,[Runtime.InteropServices.GCHandle])
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.GCHandle].GetMethod('Free',[Type[]]@()))
    Emit-Cli39ChainPageMark32 3;$il.MarkLabel($skip)
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
    Emit-Cli39ChainPageException32 1;$il.EndExceptionBlock();$il.EndExceptionBlock()
    $il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false;return $w
  }catch{$w.constructionFailure=$_;throw}finally{$w.constructionTerminal=$true}
}


function Copy-Cli39PublisherChainRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ChainOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$h=Get-Cli39PublisherChainRange32 $p $ChainOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
      -not [object]::ReferenceEquals($h.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($h.stateWorkerOriginal,$capture.stateWorkerOriginal) -or
      $null -ne $h.aliasOriginal -or -not $h.backingConstructorTerminal -or $null -ne $h.backingConstructorFailure -or
      -not $h.rangeReturned -or $h.used -ne $h.bytesExpected -or $h.rawBytesOriginal.Length -ne $h.bytesExpected -or
      $null -ne $h.copyWorkerOriginal -or $null -ne $n.closeWorkerOriginal -or
      $null -ne $n.storageReleaseOriginal){throw 'SAME complete page-checked provider header required before once-only copy.'}
  foreach($page in $h.pagesOriginal){
    if(-not $page.accepted -or -not $page.workerOriginal.terminalJoined -or
        $page.workerOriginal.thread.IsAlive){throw 'Every actual header page must be terminal and accepted.'}
  }
  $w=@{phase='provider-chain-header';preparation=$p;nativeOriginal=$n;captureOriginal=$capture;
    headerOriginal=$h;chainOriginal=$ChainOriginal;results=[int[]]::new(2);exceptionOriginal=[object[]]::new(1);
    context=$null;method=$null;il=$null;body=$null;thread=$null;
    startAttempted=$false;startReturned=$false;startException=$null;
    terminalJoined=$false;joinPendingActive=$false;joinMultiplicityUnknown=$false;
    joinPending=@{attempted=$false;returned=$false;value=$null;exception=$null};
    joinFalse=@{returned=$false;value=$false;repeats=0L};joinTerminal=@{returned=$false;value=$true};
    joinException=@{present=$false;original=$null};constructionTerminal=$false;constructionFailure=$null}
  $h.copyWorkerOriginal=$w
  try{
    if($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue){throw 'Complete publisher copy worker geometry required.'}
    $w.context=[object[]]@($h.providerPointerOriginal,$h.rawBytesOriginal,$w.results,$w.exceptionOriginal)
    $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedChainHeaderCopy',[void],
      [Type[]]@([object]),[Threading.Thread].Module,$true)
    $il=$w.method.GetILGenerator();$w.il=$il;$failureLocal=$il.DeclareLocal([Exception])
    function Emit-Cli39ChainHeaderContext32([int]$Index,[Type]$Type){
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0);$il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index);$il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      if($Type.IsValueType){$il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type)}
      else{$il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)}
    }
    function Emit-Cli39ChainHeaderMark32([int]$Index){
      Emit-Cli39ChainHeaderContext32 2 ([int[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1);$il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    }
    $null=$il.BeginExceptionBlock();Emit-Cli39ChainHeaderMark32 0
    Emit-Cli39ChainHeaderContext32 0 ([IntPtr]);Emit-Cli39ChainHeaderContext32 1 ([byte[]])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int]$h.bytesExpected)
    $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('Copy',[Type[]]@([IntPtr],[byte[]],[int],[int])))
    Emit-Cli39ChainHeaderMark32 1
    $il.BeginCatchBlock([Exception]);$il.Emit([Reflection.Emit.OpCodes]::Stloc,$failureLocal)
    Emit-Cli39ChainHeaderContext32 3 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failureLocal);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    $il.EndExceptionBlock();$il.Emit([Reflection.Emit.OpCodes]::Ret)
    $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
    $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
    $w.thread.IsBackground=$false;$w.constructionTerminal=$true
    $null=Start-Cli39PublisherWorker26 $w
    while(-not $w.terminalJoined){
      $null=Observe-Cli39PublisherWorker26 $w
      if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
    }
    if($w.thread.IsAlive -or $null -ne $w.exceptionOriginal[0] -or $w.results[1] -ne 1){throw 'Actual provider header copy did not return completely.'}
    $h.copyReturned=$true
    $null=Decode-Cli39PublisherChainRange32 $p $h
    return $h
  }catch{$h.exceptionOriginal=$_;throw}
  finally{if(-not $w.constructionTerminal){$w.constructionFailure=$h.exceptionOriginal;$w.constructionTerminal=$true}}
}

function Decode-Cli39PublisherChainRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ChainOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$r=Get-Cli39PublisherChainRange32 $p $ChainOriginal;$w=$r.copyWorkerOriginal
  if($r.decodedReturned -or $null -ne $r.fieldsOriginal -or $null -ne $r.aliasOriginal -or
      -not $r.copyReturned -or -not $r.rangeReturned -or $r.used -ne $r.bytesExpected -or
      $r.rawBytesOriginal.Length -ne $r.bytesExpected -or
      -not [object]::ReferenceEquals((Get-Cli39PublisherWorkerOriginal26 $w),$w) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $w.results[1] -ne 1 -or
      $null -ne $w.exceptionOriginal[0]){throw 'Actual once-only complete canonical chain copy required.'}
  try{
    $r.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $f=$r.fieldsOriginal;$b=$r.rawBytesOriginal
    if($r.kindOriginal -cin @('chain-context','simple-chain','element','revocation','crl-info','trust-list')){
      $f.Add('cbSize',[BitConverter]::ToUInt32($b,0))
      if($f['cbSize'] -ne $r.bytesExpected){throw 'Actual chain header differs from SDK26100 x64.'}
    }
    $dwords=@();$pointers=@()
    switch -CaseSensitive ($r.kindOriginal){
      'chain-context'{
        $dwords=@(@('dwErrorStatus',4),@('dwInfoStatus',8),@('cChain',12),@('cLowerQualityChainContext',24),@('fHasRevocationFreshnessTime',40),@('dwRevocationFreshnessTime',44),@('dwCreateFlags',48))
        $pointers=@(@('rgpChain',16),@('rgpLowerQualityChainContext',32))
        $guid=[byte[]]::new(16);$f.Add('ChainIdOriginal',$guid);[Array]::Copy($b,52,$guid,0,16)
      }
      'simple-chain'{
        $dwords=@(@('dwErrorStatus',4),@('dwInfoStatus',8),@('cElement',12),@('fHasRevocationFreshnessTime',32),@('dwRevocationFreshnessTime',36))
        $pointers=@(@('rgpElement',16),@('pTrustListInfo',24))
      }
      'element'{
        $dwords=@(@('dwErrorStatus',16),@('dwInfoStatus',20))
        $pointers=@(@('pCertContext',8),@('pRevocationInfo',24),@('pIssuanceUsage',32),@('pApplicationUsage',40),@('pwszExtendedErrorInfo',48))
      }
      'revocation'{
        $dwords=@(@('dwRevocationResult',4),@('fHasFreshnessTime',24),@('dwFreshnessTime',28))
        $pointers=@(@('pszRevocationOid',8),@('pvOidSpecificInfo',16),@('pCrlInfo',32))
      }
      'crl-info'{
        $dwords=@(@('fDeltaCrlEntry',32));$pointers=@(@('pBaseCrlContext',8),@('pDeltaCrlContext',16),@('pCrlEntry',24))
      }
      'crl-context'{
        $dwords=@(@('dwCertEncodingType',0),@('cbCrlEncoded',16));$pointers=@(@('pbCrlEncoded',8),@('pCrlInfo',24),@('hCertStore',32))
      }
      'trust-list'{
        $pointers=@(@('pCtlEntry',8),@('pCtlContext',16))
      }
      'ctl-context'{
        $dwords=@(@('dwMsgAndCertEncodingType',0),@('cbCtlEncoded',16),@('cbCtlContent',56))
        $pointers=@(@('pbCtlEncoded',8),@('pCtlInfo',24),@('hCertStore',32),@('hCryptMsg',40),@('pbCtlContent',48))
      }
      'usage'{
        $dwords=@(@('cUsageIdentifier',0));$pointers=@(@('rgpszUsageIdentifier',8))
      }
      'certificate-context'{
        $dwords=@(@('dwCertEncodingType',0),@('cbCertEncoded',16));$pointers=@(@('pbCertEncoded',8),@('pCertInfo',24),@('hCertStore',32))
      }
      {$_ -cin @('simple-chain-array','lower-context-array','element-array','usage-pointer-array')}{
        if(($r.bytesExpected%8) -ne 0){throw 'Actual complete chain pointer array extent required.'}
        $r.itemsOriginal=[Collections.Generic.List[object]]::new([int]($r.bytesExpected/8))
        for($i=0;$i -lt ($r.bytesExpected/8);$i++){$r.itemsOriginal.Add([IntPtr]::new([BitConverter]::ToInt64($b,8*$i)))}
      }
      {$_ -cin @('crl-der','certificate-der','ctl-encoded','ctl-content')}{<# Entire encoded bytes retained; no ASN/trust inference. #>}
      default{throw 'No generic chain decoder kind exists.'}
    }
    foreach($entry in $dwords){$f.Add($entry[0],[BitConverter]::ToUInt32($b,$entry[1]))}
    foreach($entry in $pointers){$f.Add($entry[0],[IntPtr]::new([BitConverter]::ToInt64($b,$entry[1])))}
    $r.decodedReturned=$true;return $r
  }catch{$r.exceptionOriginal=$_;throw}
}

function Capture-Cli39PublisherChainRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ChainOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$r=Get-Cli39PublisherChainRange32 $p $ChainOriginal
  if($r.backingConstructorAttempted){throw 'Actual chain backing/capture cannot retry.'}
  $r.backingConstructorAttempted=$true
  try{
    if($null -ne $r.aliasOriginal){
      $canonical=Get-Cli39PublisherChainRange32 $p $r.aliasOriginal
      if($null -ne $canonical.aliasOriginal -or -not $canonical.copyReturned -or
          -not $canonical.decodedReturned -or $null -ne $canonical.exceptionOriginal){throw 'Actual completed canonical alias capture required.'}
      $r.rawBytesOriginal=$canonical.rawBytesOriginal;$r.pagesOriginal=$canonical.pagesOriginal
      $r.fieldsOriginal=$canonical.fieldsOriginal;$r.itemsOriginal=$canonical.itemsOriginal
      $r.used=$canonical.used;$r.rangeReturned=$canonical.rangeReturned
      $r.copyReturned=$canonical.copyReturned;$r.decodedReturned=$canonical.decodedReturned
      $r.aliasReturned=$true;return $r
    }
    $r.pagesOriginal=[Collections.Generic.List[object]]::new([int]$r.bytesExpected)
    $r.rawBytesOriginal=[byte[]]::new([int]$r.bytesExpected)
    if($r.bytesExpected -eq 0){
      $r.absenceObserved=$true;$r.rangeReturned=$true;$r.decodedReturned=$true
      $r.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
      return $r
    }
  }catch{$r.backingConstructorFailure=$_;throw}finally{$r.backingConstructorTerminal=$true}
  try{
    while(-not $r.rangeReturned){
      $w=New-Cli39PublisherChainPageWorker32 $p $r
      $null=Start-Cli39PublisherWorker26 $w
      while(-not $w.terminalJoined){$null=Observe-Cli39PublisherWorker26 $w;if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}}
      $null=Complete-Cli39ProviderPage32 $w
    }
    $null=Copy-Cli39PublisherChainRange32 $p $r
    return $r
  }catch{$r.exceptionOriginal=$_;throw}
}
function Capture-Cli39PublisherChains32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal
  $capture=$n.providerCaptureOriginal;$book=$n.chainReservationOriginal
  if(-not [object]::ReferenceEquals($book.preparationOriginal,$p) -or
      -not $book.constructionTerminal -or $null -ne $book.constructionFailure -or
      $null -ne $book.traversalOriginal -or $book.recordsOriginal.Count -ne 0 -or
      -not $capture.sequenceOriginal.primarySignersReturned -or $capture.returned -or
      $null -ne $n.closeWorkerOriginal -or $null -ne $n.storageReleaseOriginal){throw 'SAME once-only open provider chain traversal required.'}
  $book.traversalOriginal=@{bookOriginal=$book;captureOriginal=$capture;
    rootSignerOriginal=$null;rootAttemptedCount=0L;rootReturnedCount=0L;
    rootsReturned=$false;cursor=0;returned=$false;exceptionOriginal=$null}
  $t=$book.traversalOriginal
  try{
    foreach($primary in $capture.signerOriginals){
      Observe-OwnedPrerequisiteDeadline $p
      $t.rootSignerOriginal=$primary;$t.rootAttemptedCount++
      $root=New-Cli39PublisherChainRange32 $p $primary 'chain-context'
      $null=Capture-Cli39PublisherChainRange32 $p $root;$t.rootReturnedCount++
      foreach($counter in $primary.counterSignerOriginals){
        Observe-OwnedPrerequisiteDeadline $p
        $t.rootSignerOriginal=$counter;$t.rootAttemptedCount++
        $root=New-Cli39PublisherChainRange32 $p $counter 'chain-context'
        $null=Capture-Cli39PublisherChainRange32 $p $root;$t.rootReturnedCount++
      }
    }
    $t.rootsReturned=$true
    while($t.cursor -lt $book.recordsOriginal.Count){
      Observe-OwnedPrerequisiteDeadline $p
      $r=Get-Cli39PublisherChainRange32 $p $book.recordsOriginal[$t.cursor]
      if($r.expansionAttempted -or -not $r.decodedReturned -or
          $null -ne $r.exceptionOriginal){throw 'Actual once-only decoded chain expansion required.'}
      $r.expansionAttempted=$true
      try{
        if($null -ne $r.aliasOriginal){
          if(-not $r.aliasReturned -or -not $r.aliasOriginal.decodedReturned){throw 'Actual canonical chain alias completion required.'}
        }elseif(-not $r.absenceObserved){
          $roles=@()
          switch -CaseSensitive ($r.kindOriginal){
            'chain-context'{$roles=@('simple-chain-array','lower-context-array')}
            'simple-chain'{$roles=@('element-array','trust-list')}
            'element'{$null=Capture-Cli39NamedProviderOid32 $p $r 'extended-error';$roles=@('revocation','certificate-context','issuance-usage','application-usage')}
            'usage'{$roles=@('usage-pointer-array')}
            'revocation'{$null=Capture-Cli39NamedProviderOid32 $p $r 'revocation-algorithm';$roles=@('crl-info')}
            'crl-info'{$roles=@('base-crl','delta-crl')}
            'crl-context'{$roles=@('crl-der')}
            'certificate-context'{$roles=@('certificate-der')}
            'trust-list'{$roles=@('ctl-context')}
            'ctl-context'{$roles=@('ctl-encoded','ctl-content')}
            {$_ -cin @('simple-chain-array','lower-context-array','element-array','usage-pointer-array')}{
              if($r.kindOriginal -ceq 'usage-pointer-array'){
                for($ordinal=0;$ordinal -lt $r.itemsOriginal.Count;$ordinal++){Observe-OwnedPrerequisiteDeadline $p;$null=Capture-Cli39NamedProviderOid32 $p $r 'chain-usage' $ordinal}
                break
              }
              $childRole=switch -CaseSensitive ($r.kindOriginal){'simple-chain-array'{'simple-chain'}'lower-context-array'{'lower-context'}'element-array'{'element'}}
              for($ordinal=0;$ordinal -lt $r.itemsOriginal.Count;$ordinal++){
                Observe-OwnedPrerequisiteDeadline $p
                $child=New-Cli39PublisherChainRange32 $p $r $childRole $ordinal
                $null=Capture-Cli39PublisherChainRange32 $p $child
              }
            }
            {$_ -cin @('crl-der','certificate-der','ctl-encoded','ctl-content')}{<# Full encoded bytes already retained. #>}
            default{throw 'No generic chain expansion exists.'}
          }
          foreach($role in $roles){
            Observe-OwnedPrerequisiteDeadline $p
            $child=New-Cli39PublisherChainRange32 $p $r $role
            $null=Capture-Cli39PublisherChainRange32 $p $child
          }
        }
        $r.expansionReturned=$true
      }catch{$r.expansionFailure=$_;throw}
      $t.cursor++
    }
    $t.returned=$true;return $t
  }catch{$t.exceptionOriginal=$_;throw}
}

function Test-Cli39PublisherChainBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$book=$n.chainReservationOriginal
  if($null -eq $book){return $true}
  if(-not [object]::ReferenceEquals($book.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($book.reservationOriginal,$p.reservation) -or
      -not $book.constructionTerminal -or $book.rangeConstructionUnknown){return $false}
  foreach($r in $book.recordsOriginal){
    if(-not [object]::ReferenceEquals($r.bookOriginal,$book) -or
        -not [object]::ReferenceEquals($r.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($r.captureOriginal,$n.providerCaptureOriginal) -or
        -not $r.constructionTerminal -or
        ($r.backingConstructorAttempted -and -not $r.backingConstructorTerminal)){return $false}
    if($null -ne $r.aliasOriginal){
      if(-not $book.recordsOriginal.Contains($r.aliasOriginal) -or
          -not [object]::ReferenceEquals($r.aliasOriginal.bookOriginal,$book) -or
          $null -ne $r.aliasOriginal.aliasOriginal -or $null -ne $r.copyWorkerOriginal){return $false}
      continue
    }
    foreach($page in $r.pagesOriginal){
      $w=$page.workerOriginal
      if($null -eq $w -or -not [object]::ReferenceEquals($w.chainOriginal,$r) -or
          -not [object]::ReferenceEquals($w.pageOriginal,$page) -or
          -not [object]::ReferenceEquals($w.preparation,$p) -or
          -not $w.constructionTerminal -or $w.joinPendingActive -or
          $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
      if($w.startAttempted -and (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
      if($w.startAttempted -and (($w.results[0] -eq 1 -and $w.results[1] -ne 1) -or
          ($w.results[1] -eq 1 -and ($w.results[2] -ne 1 -or $w.results[3] -ne 1)) -or
          $w.pinOriginal[0].IsAllocated -or $null -ne $w.exceptionOriginal[1])){return $false}
    }
    $w=$r.copyWorkerOriginal
    if($null -ne $w){
      if(-not [object]::ReferenceEquals($w.chainOriginal,$r) -or
          -not [object]::ReferenceEquals($w.headerOriginal,$r) -or
          -not [object]::ReferenceEquals($w.preparation,$p) -or
          -not $w.constructionTerminal -or $w.joinPendingActive -or
          $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
      if($w.startAttempted -and (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
    }
  }
  # Actual required data disposition/full capture and future-start closure are
  # independent conditions in the enclosing subject/native closure predicates.
  return $true
}
function Test-Cli39PublisherChainCaptureComplete32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal
  $book=$n.chainReservationOriginal;$capture=$n.providerCaptureOriginal
  if($null -eq $book -or -not [object]::ReferenceEquals($book.preparationOriginal,$p) -or
      -not $book.constructionTerminal -or $null -ne $book.constructionFailure -or
      $book.rangeConstructionUnknown -or $null -ne $book.rangeConstructionFailure -or
      -not (Test-Cli39PublisherChainBorrowClosed32 $p) -or
      -not (Test-Cli39NamedProviderOidsBorrowClosed32 $p)){return $false}
  $t=$book.traversalOriginal
  if($null -eq $t -or -not [object]::ReferenceEquals($t.bookOriginal,$book) -or
      -not [object]::ReferenceEquals($t.captureOriginal,$capture) -or
      -not $t.rootsReturned -or -not $t.returned -or $null -ne $t.exceptionOriginal -or
      $t.rootAttemptedCount -ne $t.rootReturnedCount -or
      $t.cursor -ne $book.recordsOriginal.Count){return $false}
  foreach($r in $book.recordsOriginal){
    $null=Get-Cli39PublisherChainRange32 $p $r
    if(-not $r.charged -or -not $r.constructionTerminal -or $null -ne $r.constructionFailure -or
        -not $r.backingConstructorAttempted -or -not $r.backingConstructorTerminal -or
        $null -ne $r.backingConstructorFailure -or $null -ne $r.exceptionOriginal -or
        -not $r.rangeReturned -or -not $r.decodedReturned -or
        -not $r.expansionAttempted -or -not $r.expansionReturned -or
        $null -ne $r.expansionFailure -or $r.used -ne $r.bytesExpected){return $false}
    if($r.absenceObserved){
      if($r.bytesExpected -ne 0 -or $null -ne $r.aliasOriginal -or
          $null -ne $r.copyWorkerOriginal -or $r.pagesOriginal.Count -ne 0 -or
          $r.rawBytesOriginal.Length -ne 0){return $false}
    }else{
      if(-not $r.copyReturned -or $r.rawBytesOriginal.Length -ne $r.bytesExpected){return $false}
      if($null -ne $r.aliasOriginal){
        $canonical=$r.aliasOriginal
        if(-not $r.aliasReturned -or -not $canonical.copyReturned -or -not $canonical.decodedReturned -or
            -not $canonical.expansionReturned -or $null -ne $canonical.exceptionOriginal -or
            -not [object]::ReferenceEquals($r.rawBytesOriginal,$canonical.rawBytesOriginal) -or
            -not [object]::ReferenceEquals($r.fieldsOriginal,$canonical.fieldsOriginal)){return $false}
      }
    }
  }
  foreach($a in $capture.namedOidParentsOriginal){
    if($a.roleOriginal -cnotin @('revocation-algorithm','chain-usage','extended-error')){continue}
    if(-not (Test-Cli39OidParentMember32 $p $a) -or $null -ne $a.exceptionOriginal){return $false}
    if($a.absenceObserved){if($a.oidPointerOriginal -ne [IntPtr]::Zero -or $null -ne $a.oidOriginal){return $false}}
    else{
      $h=Get-Cli39ProviderOidOriginal32 $p $a
      if(-not $h.constructionTerminal -or $null -ne $h.constructionFailure -or
          $null -ne $h.exceptionOriginal -or -not $h.returned -or -not $h.terminated){return $false}
    }
  }
  return $true
}
function Join-Cli39PublisherCertificateChain32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$CertificateOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$capture=$n.providerCaptureOriginal
  $book=$n.chainReservationOriginal;$cert=$CertificateOriginal
  if(-not (Test-Cli39ProviderCertificateMember32 $capture $cert) -or
      -not (Test-Cli39PublisherChainCaptureComplete32 $p) -or
      $cert.chainAssociationAttempted -or $null -ne $cert.chainAssociationOriginal -or
      -not $cert.providerHeaderOriginal.copyReturned -or -not $cert.providerHeaderOriginal.decodedOriginal.returned -or
      -not $cert.derOriginal.copyReturned -or $null -ne $cert.derOriginal.exceptionOriginal){throw 'Actual once-only copied provider certificate/chain association required.'}
  [decimal]$charge=$book.usedChargeBytes+$book.fixedChargeBytes
  if($charge -gt $book.budgetOriginal.publisherChainCaptureReservationBytes){throw 'Actual association exceeds SAME chain reservation.'}
  $book.usedChargeBytes=$charge;$cert.chainAssociationAttempted=$true
  $cert.chainAssociationOriginal=@{certificateOriginal=$cert;bookOriginal=$book;captureOriginal=$capture;
    chargeBytes=$book.fixedChargeBytes;providerHeaderOriginal=$cert.providerHeaderOriginal;
    signerOriginal=$cert.signerOriginal;chainPointerOriginal=$cert.providerHeaderOriginal.decodedOriginal.fieldsOriginal['pChainElement'];
    elementOriginal=$null;rootOriginal=$null;contextOriginal=$null;derOriginal=$null;
    absentObserved=$false;chainContextMatches=$false;certificateContextMatches=$false;
    extentMatches=$false;comparedBytes=0;byteMismatchObserved=$false;equalBytes=$false;
    returned=$false;exceptionOriginal=$null}
  $o=$cert.chainAssociationOriginal
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies certificate/chain association progression.'}
    if($o.chainPointerOriginal -eq [IntPtr]::Zero){$o.absentObserved=$true;$o.returned=$true;return $o}
    foreach($candidate in $book.recordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies certificate/chain association progression.'}
      if($candidate.kindOriginal -cne 'element' -or $candidate.pointerOriginal -ne $o.chainPointerOriginal){continue}
      $root=$candidate
      while($root.roleOriginal -cne 'chain-context'){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies certificate/chain association progression.'}
        if(-not $book.recordsOriginal.Contains($root.parentOriginal)){throw 'Actual chain ancestry does not reach enrolled signer root.'}
        $root=$root.parentOriginal
      }
      if($root.pointerOriginal -ne $cert.signerOriginal.headerRangeOriginal.decodedOriginal.fieldsOriginal['pChainContext']){continue}
      $o.elementOriginal=$candidate;$o.rootOriginal=$root;$o.chainContextMatches=$true;break
    }
    if($null -eq $o.elementOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies certificate/chain association completion.'}
      $o.returned=$true;return $o
    }
    $element=$o.elementOriginal
    $o.certificateContextMatches=($element.fieldsOriginal['pCertContext'] -eq $cert.contextHeaderOriginal.providerPointerOriginal)
    foreach($candidate in $book.recordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies certificate/chain association progression.'}
      if($candidate.roleOriginal -ceq 'certificate-context' -and
          [object]::ReferenceEquals($candidate.parentOriginal,$element)){$o.contextOriginal=$candidate;break}
    }
    # Alias elements share their canonical child's source graph, while this
    # record keeps the actual original alias/parent association above.
    if($null -eq $o.contextOriginal -and $null -ne $element.aliasOriginal){
      foreach($candidate in $book.recordsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies certificate/chain association progression.'}
        if($candidate.roleOriginal -ceq 'certificate-context' -and
            [object]::ReferenceEquals($candidate.parentOriginal,$element.aliasOriginal)){$o.contextOriginal=$candidate;break}
      }
    }
    if($null -eq $o.contextOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies certificate/chain association completion.'}
      $o.returned=$true;return $o
    }
    foreach($candidate in $book.recordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies certificate/chain association progression.'}
      if($candidate.roleOriginal -ceq 'certificate-der' -and
          ([object]::ReferenceEquals($candidate.parentOriginal,$o.contextOriginal) -or
           ($null -ne $o.contextOriginal.aliasOriginal -and [object]::ReferenceEquals($candidate.parentOriginal,$o.contextOriginal.aliasOriginal)))){$o.derOriginal=$candidate;break}
    }
    if($null -ne $o.derOriginal){
      $der=$o.derOriginal;$actual=$cert.derOriginal
      $o.extentMatches=($der.bytesExpected -eq $actual.bytesExpected)
      if($o.extentMatches){
        for($i=0;$i -lt $actual.bytesExpected;$i++){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies certificate/chain association progression.'}
          $different=($der.rawBytesOriginal[$i] -ne $actual.rawBytesOriginal[$i]);$o.comparedBytes=$i+1
          if($different){$o.byteMismatchObserved=$true;break}
        }
        $o.equalBytes=(-not $o.byteMismatchObserved -and $o.comparedBytes -eq $actual.bytesExpected)
      }
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies certificate/chain association completion.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}