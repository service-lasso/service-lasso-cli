# Pure source-bound gate; absent capture is valid only for preparatory books or
# the first state constructor, whose own once-only guards remain mandatory.
function Assert-Cli39PublisherCollectionOpen32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal
  if($null -eq $n -or -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)){
    throw 'SAME enrolled publisher preparation required for collection.'
  }
  $capture=$n.providerCaptureOriginal
  if($null -ne $n.closeWorkerOriginal -or $null -ne $n.storageReleaseOriginal -or
      ($null -ne $capture -and ($capture.collectionTerminal -or $capture.returned))){
    throw 'Irrevocable publisher collection terminal/close/release closes every new collector effect.'
  }
}
# UNRUN fixed WinVerifyTrust worker phase for the SAME Core preparation owner.
# No PowerShell runspace/scriptblock executes on the worker. The emitted body
# makes exactly one typed native WinVerifyTrust call and stores its raw LONG
# into preallocated int[] BEFORE any boxing, reporting or post-call diagnostic.
# Actual ABI/module/storage construction and full provider capture remain
# source-bound prerequisites; this worker does not grant their authority.
. (Join-Path $PSScriptRoot 'publisher-storage-worker26.ps1')
. (Join-Path $PSScriptRoot 'publisher-module-worker26.ps1')
. (Join-Path $PSScriptRoot 'publisher-initialize-worker26.ps1')
. (Join-Path $PSScriptRoot 'publisher-capture-worker32.ps1')
. (Join-Path $PSScriptRoot 'publisher-prepared-identity32.ps1')
. (Join-Path $PSScriptRoot 'publisher-preparation32.ps1')
. (Join-Path $PSScriptRoot 'publisher-policy32.ps1')
. (Join-Path $PSScriptRoot 'publisher-time32.ps1')
. (Join-Path $PSScriptRoot 'publisher-anchor32.ps1')
. (Join-Path $PSScriptRoot 'publisher-evaluation32.ps1')
. (Join-Path $PSScriptRoot 'publisher-revocation32.ps1')
. (Join-Path $PSScriptRoot 'publisher-timestamp32.ps1')
. (Join-Path $PSScriptRoot 'publisher-semantic32.ps1')
. (Join-Path $PSScriptRoot 'publisher-chain32.ps1')
. (Join-Path $PSScriptRoot 'publisher-closure32.ps1')
. (Join-Path $PSScriptRoot 'publisher-page-worker32.ps1')
. (Join-Path $PSScriptRoot 'publisher-header32.ps1')
. (Join-Path $PSScriptRoot 'publisher-signer-worker32.ps1')
. (Join-Path $PSScriptRoot 'publisher-certificate-worker32.ps1')
. (Join-Path $PSScriptRoot 'publisher-attributes32.ps1')
. (Join-Path $PSScriptRoot 'publisher-fields32.ps1')
. (Join-Path $PSScriptRoot 'publisher-oid32.ps1')
. (Join-Path $PSScriptRoot 'publisher-raw-graph32.ps1')
. (Join-Path $PSScriptRoot 'publisher-invoke32.ps1')

function Get-Cli39PublisherWorkerOriginal26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal; $n=$w.nativeOriginal
  switch -CaseSensitive ($w.phase) {
    'module-load' {
      $m=$w.moduleOwnerOriginal; $p=$w.preparation
      if (-not [object]::ReferenceEquals($m.preparation,$p) -or
          $m.kind -cne 'cli39.publisher-resident-module.v1' -or
          $m.terminalLifetime -cne 'host-process' -or
          -not $p.receiver.frames.ContainsKey($m.receiverKey) -or
          -not [object]::ReferenceEquals($p.receiver.frames[$m.receiverKey],$m)) { throw 'Same enrolled resident module original required.' }
      $found=$false
      foreach ($original in $n.moduleOwners) {
        if ([object]::ReferenceEquals($original,$m)) { $found=$true; break }
      }
      if (-not $found) { throw 'Module worker must belong to the same preparation module roster.' }
      return $m.workerOriginal
    }
    'verify' { return $n.verifyWorkerOriginal }
    'close' { return $n.closeWorkerOriginal }
    'storage-acquire' { return $n.storageAcquireOriginal }
    'storage-release' { return $n.storageReleaseOriginal }
    'initialize' { return $n.initializeWorkerOriginal }
    'provider-state' { return $n.providerCaptureOriginal.stateWorkerOriginal }
    'provider-oid-page' {
      $h=Get-Cli39ProviderOidOriginal32 $w.preparation $w.attributeOriginal;$r=$w.pageOriginal
      if(-not [object]::ReferenceEquals($w.captureOriginal,$n.providerCaptureOriginal) -or
          -not [object]::ReferenceEquals($w.oidOriginal,$h) -or
          -not $h.pagesOriginal.Contains($r) -or -not [object]::ReferenceEquals($r.workerOriginal,$w)){
        throw 'SAME enrolled fixed OID query worker required.'
      }
      return $r.workerOriginal
    }
    'provider-oid-read' {
      $h=Get-Cli39ProviderOidOriginal32 $w.preparation $w.attributeOriginal;$r=$w.pageOriginal
      if(-not [object]::ReferenceEquals($w.captureOriginal,$n.providerCaptureOriginal) -or
          -not [object]::ReferenceEquals($w.oidOriginal,$h) -or
          -not $h.pagesOriginal.Contains($r) -or -not [object]::ReferenceEquals($r.readerOriginal,$w)){
        throw 'SAME enrolled fixed OID reader worker required.'
      }
      return $r.readerOriginal
    }
    'provider-chain-page' {
      $h=Get-Cli39PublisherChainRange32 $w.preparation $w.chainOriginal;$r=$w.pageOriginal
      if(-not [object]::ReferenceEquals($w.captureOriginal,$n.providerCaptureOriginal) -or
          -not [object]::ReferenceEquals($w.headerOriginal,$h) -or
          -not $h.pagesOriginal.Contains($r) -or -not [object]::ReferenceEquals($r.workerOriginal,$w)){
        throw 'SAME enrolled fixed chain query worker required.'
      }
      return $r.workerOriginal
    }
    'provider-chain-header' {
      $h=Get-Cli39PublisherChainRange32 $w.preparation $w.chainOriginal
      if(-not [object]::ReferenceEquals($w.captureOriginal,$n.providerCaptureOriginal) -or
          -not [object]::ReferenceEquals($w.headerOriginal,$h)){
        throw 'SAME enrolled fixed chain copy worker required.'
      }
      return $h.copyWorkerOriginal
    }
    'provider-certificate' {
      $capture=$n.providerCaptureOriginal;$cert=$w.certificateOriginal
      if(-not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
          -not [object]::ReferenceEquals($capture.preparationOriginal,$w.preparation) -or
          -not (Test-Cli39ProviderCertificateMember32 $capture $cert) -or
          -not [object]::ReferenceEquals($cert.workerOriginal,$w)){
        throw 'SAME enrolled fixed certificate helper required.'
      }
      return $cert.workerOriginal
    }
    'provider-signer' {
      $capture=$n.providerCaptureOriginal;$signer=$w.signerOriginal
      if(-not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
          -not [object]::ReferenceEquals($capture.preparationOriginal,$w.preparation) -or
          -not [object]::ReferenceEquals($signer.captureOriginal,$capture) -or
          -not (Test-Cli39ProviderSignerMember32 $capture $signer) -or
          -not [object]::ReferenceEquals($signer.workerOriginal,$w)){
        throw 'SAME enrolled fixed signer helper worker required.'
      }
      return $signer.workerOriginal
    }
    'provider-page' {
      $capture=$n.providerCaptureOriginal;$header=Get-Cli39ProviderHeaderRange32 $w.preparation $w.signerOriginal $w.certificateOriginal $w.certificatePart $w.signerPart $w.attributeOriginal $w.attributeValueOriginal $w.providerPart;$page=$w.pageOriginal
      if(-not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
          -not [object]::ReferenceEquals($capture.preparationOriginal,$w.preparation) -or
          -not [object]::ReferenceEquals($capture.nativeOriginal,$n) -or
          -not [object]::ReferenceEquals($w.headerOriginal,$header) -or
          -not $header.pagesOriginal.Contains($page) -or
          -not [object]::ReferenceEquals($page.workerOriginal,$w)){
        throw 'SAME enrolled fixed provider page worker required.'
      }
      return $page.workerOriginal
    }
    'provider-header' {
      $capture=$n.providerCaptureOriginal;$header=Get-Cli39ProviderHeaderRange32 $w.preparation $w.signerOriginal $w.certificateOriginal $w.certificatePart $w.signerPart $w.attributeOriginal $w.attributeValueOriginal $w.providerPart
      if(-not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
          -not [object]::ReferenceEquals($capture.preparationOriginal,$w.preparation) -or
          -not [object]::ReferenceEquals($w.headerOriginal,$header)){
        throw 'SAME enrolled fixed provider header copy required.'
      }
      return $header.copyWorkerOriginal
    }
    default { throw 'No generic publisher worker phase exists.' }
  }
}

function New-Cli39PublisherWorker26 {
  param([Parameter(Mandatory)]$OriginalPreparation,
    [Parameter(Mandatory)][ValidateSet('verify','close')][string]$Phase)
  $p=$OriginalPreparation; $c=$p.consumerOriginal
  if ($null -eq $c -or $c.slot -cne 'cli39.windows-publisher-trust.v1' -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)) { throw 'Original enrolled publisher preparation required.' }
  $native=$c.publisherNativeOriginal
  if ($null -eq $native -or $native.wtdPointer -eq [IntPtr]::Zero -or
      $native.actionPointer -eq [IntPtr]::Zero -or $native.winVerifyTrustOriginal -eq $null -or
      $native.delegateType -eq $null) { throw 'Actual retained source-bound ABI and native storage required.' }
  if ([IntPtr]::Size -ne 8 -or $c.geometry.workerStackBytes -le 0 -or
      $c.geometry.workerStackBytes -gt [int]::MaxValue) { throw 'Qualified x64 worker geometry required.' }
  if ($null -eq $native.storageAcquireOriginal -or -not $native.storageAcquireOriginal.terminalJoined -or
      $native.storageAcquireOriginal.results[1] -ne 1 -or
      $native.storageAcquireOriginal.pointerOriginal[0] -eq [IntPtr]::Zero -or
      $null -ne $native.storageReleaseOriginal) { throw 'Actual original live publisher storage required.' }
  $base=$native.storageAcquireOriginal.pointerOriginal[0].ToInt64()
  if ($base -le 0 -or $base -gt ([long]::MaxValue-136) -or $native.storageBytes -lt 136 -or
      $native.actionPointer.ToInt64() -ne $base -or $native.wtdPointer.ToInt64() -ne ($base+48)) { throw 'Native WTD and action must belong to the SAME acquired storage layout.' }
  if ($Phase -ceq 'verify') {
    if ($null -ne $native.verifyWorkerOriginal -or $null -ne $native.closeWorkerOriginal) { throw 'Original verification worker is once-only.' }
    $initialized=$native.initializeWorkerOriginal
    if ($null -eq $initialized -or -not $initialized.terminalJoined -or $initialized.thread.IsAlive -or
        $initialized.results[1] -ne 1 -or $null -ne $initialized.exceptionOriginal[0] -or
        -not [object]::ReferenceEquals($initialized.nativeOriginal,$native) -or
        -not [object]::ReferenceEquals($initialized.preparation,$p)) { throw 'SAME original native initialization must actually finish before verification.' }
  } else {
    if ($null -ne $native.closeWorkerOriginal -or $null -eq $native.verifyWorkerOriginal -or
        -not $native.verifyWorkerOriginal.terminalJoined -or
        -not $native.providerCaptureOriginal.returned) { throw 'Close follows actual original call terminal and complete retained provider capture, exactly once.' }
  }
  $worker=@{ phase=$Phase; preparation=$p; nativeOriginal=$native; results=[int[]]::new(5);
    exceptionOriginal=[object[]]::new(1); context=$null; method=$null; il=$null; body=$null;
    thread=$null; startAttempted=$false; startReturned=$false; startException=$null;
    terminalJoined=$false; joinPendingActive=$false; joinMultiplicityUnknown=$false;
    joinPending=@{ attempted=$false; returned=$false; value=$null; exception=$null };
    joinFalse=@{ returned=$false; value=$false; repeats=0L };
    joinTerminal=@{ returned=$false; value=$true };
    joinException=@{ present=$false; original=$null }; constructionTerminal=$false; constructionFailure=$null }
  if ($Phase -ceq 'verify') { $native.verifyWorkerOriginal=$worker }
  else { $native.closeWorkerOriginal=$worker }
  try{
  # The exact SDK26100 x64 WINTRUST_DATA.dwStateAction is at offset48.
  # The entire WTD allocation must be created by the retained native origin,
  # not reconstructed from a foreign pointer or serialized receipt.
  $worker.context=[object[]]@($native.winVerifyTrustOriginal,[IntPtr]::new(-1),
    $native.actionPointer,$native.wtdPointer,$worker.results,$worker.exceptionOriginal)
  $worker.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedWinVerifyTrustOnce',[void],
    [Type[]]@([object]),[Threading.Thread].Module,$true)
  $il=$worker.method.GetILGenerator(); $worker.il=$il
  $result=$il.DeclareLocal([int]); $failure=$il.DeclareLocal([Exception])
  $null=$il.BeginExceptionBlock()
  # Native storage writes are inside the same supervised worker too. Separate
  # original scalars distinguish write entry/return from WinVerifyTrust entry.
  foreach ($index in @(3,4)) {
    $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
    $il.Emit([Reflection.Emit.OpCodes]::Castclass,[int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$index)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    if ($index -eq 3) {
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,[IntPtr]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,48)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$(if ($Phase -ceq 'verify') {1} else {2}))
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Runtime.InteropServices.Marshal].GetMethod('WriteInt32',[Type[]]@([IntPtr],[int],[int])))
    }
  }
  # Publish native-call entry into preowned scalar storage before the effect.
  $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
  $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
  $il.Emit([Reflection.Emit.OpCodes]::Castclass,[int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
  $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
  $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
  $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
  $il.Emit([Reflection.Emit.OpCodes]::Castclass,$native.delegateType)
  foreach ($index in @(1,2,3)) {
    $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int]$index); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
    $il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,[IntPtr])
  }
  $il.Emit([Reflection.Emit.OpCodes]::Callvirt,$native.delegateType.GetMethod('Invoke'))
  $il.Emit([Reflection.Emit.OpCodes]::Stloc,$result)
  $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
  $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
  $il.Emit([Reflection.Emit.OpCodes]::Castclass,[int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
  $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$result); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
  $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
  $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_4); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
  $il.Emit([Reflection.Emit.OpCodes]::Castclass,[int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
  $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
  $il.BeginCatchBlock([Exception]); $il.Emit([Reflection.Emit.OpCodes]::Stloc,$failure)
  $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
  $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_5); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
  $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
  $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$failure); $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
  $il.EndExceptionBlock(); $il.Emit([Reflection.Emit.OpCodes]::Ret)
  $worker.body=$worker.method.CreateDelegate([Threading.ParameterizedThreadStart])
  $worker.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$worker.body,[int]$c.geometry.workerStackBytes)
  $worker.thread.IsBackground=$false
  return $worker
  }catch{$worker.constructionFailure=$_;throw}finally{$worker.constructionTerminal=$true}
}

function Start-Cli39PublisherWorker26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal; $n=$w.nativeOriginal
  $expected=Get-Cli39PublisherWorkerOriginal26 $w
  if (-not [object]::ReferenceEquals($expected,$w) -or $w.startAttempted) { throw 'Only the original once-only publisher worker may start.' }
  if ($null -ne $n.storageReleaseOriginal -and $w.phase -cne 'storage-release') {
    throw 'Original release enrollment closes all earlier publisher start gates, including constructed-but-unstarted workers.'
  }
  if(($null -ne $n.closeWorkerOriginal -or
      ($null -ne $n.providerCaptureOriginal -and $n.providerCaptureOriginal.returned)) -and
      $w.phase -cnotin @('close','storage-release')){
    throw 'Actual capture completion/close enrollment closes every earlier publisher start gate.'
  }
  if($w.phase -cin @('provider-state','provider-page','provider-header','provider-oid-page','provider-oid-read','provider-chain-page','provider-chain-header','provider-signer','provider-certificate')){
    Assert-Cli39PublisherCollectionOpen32 $w.preparation
  }
  $consumer=$w.preparation.consumerOriginal
  if ($consumer.ContainsKey('disposalOriginal') -and $null -ne $consumer.disposalOriginal -and
      $w.phase -cnotin @('provider-state','provider-page','provider-header','provider-oid-page','provider-oid-read','provider-chain-page','provider-chain-header','provider-signer','provider-certificate','close','storage-release')) {
    throw 'Consumer disposal enrollment closes every new publisher acquisition/borrow/verification start gate.'
  }
  Observe-OwnedPrerequisiteDeadline $w.preparation
  if ($w.preparation.deadlineReported -and $w.phase -cnotin @('provider-state','provider-page','provider-header','provider-oid-page','provider-oid-read','provider-chain-page','provider-chain-header','provider-signer','provider-certificate','close','storage-release')) { throw 'Original deadline forbids new publisher acquisition/verification; pending originals remain held.' }
  $w.startAttempted=$true
  # This is an attempted worker start, not proof that NativeLibrary.Load was
  # entered. The worker's original scalar independently retains actual entry.
  if ($w.phase -ceq 'module-load') { $w.moduleOwnerOriginal.loadWorkerStartAttempted=$true }
  try { $w.thread.Start($w.context); $w.startReturned=$true }
  catch { $w.startException=$_; $_.Exception.Data['originalPreparation']=$w.preparation; throw }
  return $w
}

function Observe-Cli39PublisherWorker26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal; $o=$w.joinPending
  $expected=Get-Cli39PublisherWorkerOriginal26 $w
  if (-not [object]::ReferenceEquals($expected,$w) -or
      -not $w.preparation.receiver.frames.ContainsKey($w.preparation.key) -or
      -not [object]::ReferenceEquals($w.preparation.receiver.frames[$w.preparation.key],$w.preparation)) { throw 'Actual original worker/receiver relation required for observation.' }
  # Exactly four source-derived representations: pending call, identical false
  # scalar repetitions, terminal true and first original managed exception.
  # Polls never consume terminal/exception retention. Unknown multiplicity or
  # an exception stops normal polling before another effect; the original
  # control owner still owes the separate terminal-await/disposition phase.
  if ($w.joinPendingActive -or $w.joinException.present -or $w.joinMultiplicityUnknown -or
      -not $w.startAttempted -or $w.terminalJoined) { throw 'No fresh normal join attempt is available; original terminal custody remains owed.' }
  Observe-OwnedPrerequisiteDeadline $w.preparation
  $w.joinPendingActive=$true
  $o.attempted=$true; $o.returned=$false; $o.value=$null; $o.exception=$null
  try { $o.value=$w.thread.Join(0); $o.returned=$true }
  catch {
    $o.exception=$_; $w.joinException.present=$true; $w.joinException.original=$_
    $_.Exception.Data['originalPreparation']=$w.preparation; throw
  }
  if ($o.value) {
    $w.joinTerminal.returned=$true; $w.terminalJoined=$true; $w.joinPendingActive=$false
  } else {
    $w.joinFalse.returned=$true
    if ($w.joinFalse.repeats -eq [long]::MaxValue) {
      # Preserve the actual pending false outcome and explicitly unknown exact
      # multiplicity. Never wrap, pretend an exact count or reuse the pending
      # slot; terminal and exception originals remain independently reserved.
      $w.joinMultiplicityUnknown=$true
    } else { $w.joinFalse.repeats++; $w.joinPendingActive=$false }
  }
  # False is still live/unknown, never permission to read/finalize/free WTD or
  # close provider state. Original deadline notification does not abort a call.
  return $o
}



