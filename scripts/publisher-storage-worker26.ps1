# UNRUN fixed publisher storage effects. The SAME resident Core ABI supplies
# typed VirtualAlloc/VirtualFree delegates; no generic native effect callback.
function Test-Cli39PublisherProviderUnentered26 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal; $n=$c.publisherNativeOriginal
  if ($c.slot -cne 'cli39.windows-publisher-trust.v1' -or $null -eq $n -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)) { return $false }
  $v=$n.verifyWorkerOriginal
  # Only this source's fixed worker can enter WinVerifyTrust. Construction and
  # initialization cannot enter it. No Thread.Start attempt means no native
  # entry; after any attempted start, only actual terminal join permits this
  # negative observation. A thrown/unknown Start is never a no-effect proof.
  if ($null -eq $v) { return $null -eq $n.closeWorkerOriginal }
  if (-not [object]::ReferenceEquals($v.preparation,$p) -or
      -not [object]::ReferenceEquals($v.nativeOriginal,$n) -or
      $v.phase -cne 'verify' -or $null -ne $n.closeWorkerOriginal) { return $false }
  if (-not $v.startAttempted) { return $v.results[2] -eq 0 }
  return ($v.terminalJoined -and $null -ne $v.thread -and -not $v.thread.IsAlive -and
    $v.results[2] -eq 0)
}

function Test-Cli39PublisherSubjectBorrowClosed26 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal; $n=$c.publisherNativeOriginal
  if ($c.slot -cne 'cli39.windows-publisher-trust.v1' -or $null -eq $n -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $c.ContainsKey('disposalOriginal') -or $null -eq $c.disposalOriginal) { return $false }
  if(-not (Test-Cli39PublisherPreparedIdentityBorrowClosed32 $p)){return $false}
  # Disposal enrollment prevents every new source-bound acquisition/borrow
  # start. Existing originals still require actual quiescence before handles
  # held for initialization/Load/provider work can be independently disposed.
  foreach ($phase in @('storageAcquireOriginal','initializeWorkerOriginal','verifyWorkerOriginal','closeWorkerOriginal','storageReleaseOriginal')) {
    $worker=$n[$phase]
    if ($null -eq $worker) { continue }
    if (-not [object]::ReferenceEquals($worker.preparation,$p) -or
        -not [object]::ReferenceEquals($worker.nativeOriginal,$n)) { return $false }
    if ($worker.startAttempted -and
        (-not $worker.terminalJoined -or $null -eq $worker.thread -or $worker.thread.IsAlive)) { return $false }
  }
  foreach ($module in $n.moduleOwners) {
    if (-not [object]::ReferenceEquals($module.preparation,$p) -or
        -not $module.constructionTerminal -or
        -not $p.receiver.frames.ContainsKey($module.receiverKey) -or
        -not [object]::ReferenceEquals($p.receiver.frames[$module.receiverKey],$module)) { return $false }
    $worker=$module.workerOriginal
    if ($null -ne $worker -and $worker.startAttempted -and
        (-not $worker.terminalJoined -or $null -eq $worker.thread -or $worker.thread.IsAlive)) { return $false }
  }
  if (-not (Test-Cli39ProviderSignerBorrowClosed32 $p)) { return $false }
  if (-not (Test-Cli39ProviderFieldsBorrowClosed32 $p)) { return $false }
  if(-not (Test-Cli39PublisherChainBorrowClosed32 $p)){return $false}
  if ($null -ne $n.providerCaptureOriginal -and $null -ne $n.providerCaptureOriginal.headerOriginal -and
      -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p)) { return $false }
  if (Test-Cli39PublisherProviderUnentered26 $p) { return $true }
  $close=$n.closeWorkerOriginal; $verify=$n.verifyWorkerOriginal
  return ($null -ne $verify -and $verify.terminalJoined -and
    $null -ne $n.providerCaptureOriginal -and $n.providerCaptureOriginal.returned -and
    $null -ne $close -and $close.terminalJoined -and
    $close.phase -ceq 'close' -and $close.results[1] -eq 1 -and
    $close.results[0] -eq 0 -and $null -eq $close.exceptionOriginal[0])
}

function New-Cli39PublisherStorageWorker26 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AbiOriginal,
    [Parameter(Mandatory)][ValidateSet('storage-acquire','storage-release')][string]$Phase)
  $p=$OriginalPreparation; $c=$p.consumerOriginal; $n=$c.publisherNativeOriginal
  if ($c.slot -cne 'cli39.windows-publisher-trust.v1' -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($n.abiOriginal,$AbiOriginal)) { throw 'Same enrolled publisher preparation and resident ABI required.' }
  $resident=$false
  foreach ($frame in $p.receiver.frames.Values) {
    if ([object]::ReferenceEquals($frame,$AbiOriginal.originalOwner)) { $resident=$true; break }
  }
  if (-not $resident -or
      -not [object]::ReferenceEquals($AbiOriginal.delegates,$AbiOriginal.originalOwner.delegates) -or
      $AbiOriginal.module -ne $AbiOriginal.originalOwner.module) { throw 'Actual resident Core ABI original required.' }
  if ([IntPtr]::Size -ne 8 -or $c.geometry.workerStackBytes -le 0 -or
      $c.geometry.workerStackBytes -gt [int]::MaxValue) { throw 'Complete admitted x64 worker geometry required.' }
  if ($Phase -ceq 'storage-acquire') {
    if ($null -ne $n.storageAcquireOriginal -or $null -ne $n.storageReleaseOriginal -or
        $n.storageBytes -le 0 -or $n.storageBytes -gt [long]::MaxValue) { throw 'Once-only precharged finite publisher storage required.' }
    $call=$AbiOriginal.delegates['VirtualAlloc']
  } else {
    $providerUnentered=Test-Cli39PublisherProviderUnentered26 $p
    $initialize=$n.initializeWorkerOriginal
    $initializationBorrowClosed=($null -eq $initialize)
    if ($null -ne $initialize -and
        [object]::ReferenceEquals($initialize.preparation,$p) -and
        [object]::ReferenceEquals($initialize.nativeOriginal,$n) -and
        $initialize.phase -ceq 'initialize') {
      # An unstarted original has no native destination borrow. After ANY
      # attempted start, only its actual terminal join closes that borrow;
      # constructor failure or a thrown Start never substitutes for a join.
      $initializationBorrowClosed=(-not $initialize.startAttempted -or
        ($initialize.terminalJoined -and $null -ne $initialize.thread -and
         -not $initialize.thread.IsAlive))
    }
    $providerClosed=($null -ne $n.closeWorkerOriginal -and
      [object]::ReferenceEquals($n.closeWorkerOriginal.preparation,$p) -and
      [object]::ReferenceEquals($n.closeWorkerOriginal.nativeOriginal,$n) -and
      $n.closeWorkerOriginal.phase -ceq 'close' -and $n.closeWorkerOriginal.terminalJoined -and
      $null -ne $n.closeWorkerOriginal.thread -and -not $n.closeWorkerOriginal.thread.IsAlive -and
      $n.closeWorkerOriginal.results[1] -eq 1 -and $n.closeWorkerOriginal.results[0] -eq 0 -and
      $null -eq $n.closeWorkerOriginal.exceptionOriginal[0])
    if ($null -ne $n.storageReleaseOriginal -or $null -eq $n.storageAcquireOriginal -or
        -not $n.storageAcquireOriginal.terminalJoined -or
        $n.storageAcquireOriginal.thread.IsAlive -or -not $initializationBorrowClosed -or
        $n.storageAcquireOriginal.results[1] -ne 1 -or
        $n.storageAcquireOriginal.pointerOriginal[0] -eq [IntPtr]::Zero -or
        (-not $providerUnentered -and -not $providerClosed)) { throw 'Release follows actual original acquisition and proven no provider entry or actual terminal provider close, exactly once.' }
    $call=$AbiOriginal.delegates['VirtualFree']
  }
  $w=@{ phase=$Phase; preparation=$p; nativeOriginal=$n; abiOriginal=$AbiOriginal;
    callOriginal=$call; pointerOriginal=[IntPtr[]]::new(1); results=[int[]]::new(5);
    exceptionOriginal=[object[]]::new(1); context=$null; method=$null; il=$null; body=$null;
    thread=$null; startAttempted=$false; startReturned=$false; startException=$null;
    terminalJoined=$false; joinPendingActive=$false; joinMultiplicityUnknown=$false;
    joinPending=@{ attempted=$false; returned=$false; value=$null; exception=$null };
    joinFalse=@{ returned=$false; value=$false; repeats=0L };
    joinTerminal=@{ returned=$false; value=$true };
    joinException=@{ present=$false; original=$null } }
  if ($Phase -ceq 'storage-acquire') { $n.storageAcquireOriginal=$w }
  else { $n.storageReleaseOriginal=$w }
  $address=[IntPtr]::Zero
  if ($Phase -ceq 'storage-release') { $address=$n.storageAcquireOriginal.pointerOriginal[0] }
  $w.context=[object[]]@($call,$address,[UIntPtr]::new([uint64]$n.storageBytes),
    $w.pointerOriginal,$w.results,$w.exceptionOriginal)
  $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedPublisherStorage',[void],
    [Type[]]@([object]),[Threading.Thread].Module,$true)
  $il=$w.method.GetILGenerator(); $w.il=$il
  $pointer=$il.DeclareLocal([IntPtr]); $value=$il.DeclareLocal([int])
  $nativeErrorLocal=$il.DeclareLocal([int]); $exception=$il.DeclareLocal([Exception])
  function Emit-Cli39PublisherContext26([int]$Index,[Type]$Type,[bool]$ValueType=$false) {
    $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
    if ($ValueType) { $il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,$Type) }
    else { $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type) }
  }
  function Emit-Cli39PublisherScalar26([int]$Index,[int]$Value) {
    Emit-Cli39PublisherContext26 4 ([int[]])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Value)
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
  }
  $null=$il.BeginExceptionBlock()
  Emit-Cli39PublisherScalar26 2 1
  Emit-Cli39PublisherContext26 0 ($call.GetType())
  Emit-Cli39PublisherContext26 1 ([IntPtr]) $true
  if ($Phase -ceq 'storage-acquire') {
    Emit-Cli39PublisherContext26 2 ([UIntPtr]) $true
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,0x3000) # MEM_RESERVE|MEM_COMMIT
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4)     # PAGE_READWRITE
  } else {
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0); $il.Emit([Reflection.Emit.OpCodes]::Conv_U)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,0x8000) # MEM_RELEASE, exact original base
  }
  $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$call.GetType().GetMethod('Invoke'))
  if ($Phase -ceq 'storage-acquire') { $il.Emit([Reflection.Emit.OpCodes]::Stloc,$pointer) }
  else { $il.Emit([Reflection.Emit.OpCodes]::Stloc,$value) }
  # Publish raw native result before any fallible diagnostic. Typed local and
  # preowned array stores introduce no intervening SetLastError=true call.
  if ($Phase -ceq 'storage-acquire') {
    Emit-Cli39PublisherContext26 3 ([IntPtr[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$pointer); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I)
  } else {
    Emit-Cli39PublisherContext26 4 ([int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$value); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
  }
  Emit-Cli39PublisherScalar26 1 1
  $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('GetLastWin32Error',[Type[]]@()))
  $il.Emit([Reflection.Emit.OpCodes]::Stloc,$nativeErrorLocal)
  Emit-Cli39PublisherContext26 4 ([int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3)
  $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$nativeErrorLocal); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
  Emit-Cli39PublisherScalar26 4 1
  $il.BeginCatchBlock([Exception]); $il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
  Emit-Cli39PublisherContext26 5 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
  $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$exception); $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
  $il.EndExceptionBlock(); $il.Emit([Reflection.Emit.OpCodes]::Ret)
  $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
  $w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)
  $w.thread.IsBackground=$false
  return $w
}
