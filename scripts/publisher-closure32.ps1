# Actual native failure/success closure, independently of trust acceptance.
# Resident module/builders/delegates remain charged until external host exit.
function Test-Cli39PublisherNativeClosure32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal
  if($c.slot -cne 'cli39.windows-publisher-trust.v1' -or $null -eq $n -or
      -not (Test-Cli39PublisherSubjectBorrowClosed26 $p)){return $false}
  if(-not (Test-Cli39PublisherPreparedIdentityClosure32 $p)){return $false}
  foreach($field in @('storageAcquireOriginal','initializeWorkerOriginal',
      'verifyWorkerOriginal','closeWorkerOriginal','storageReleaseOriginal')){
    $w=$n[$field];if($null -eq $w){continue}
    if(-not [object]::ReferenceEquals($w.preparation,$p) -or
        -not [object]::ReferenceEquals($w.nativeOriginal,$n) -or
        $w.joinPendingActive -or $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
    if($w.startAttempted -and
        (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
  }
  $capture=$n.providerCaptureOriginal
  if(-not (Test-Cli39ProviderSignerBorrowClosed32 $p)){return $false}
  if(-not (Test-Cli39ProviderFieldsBorrowClosed32 $p)){return $false}
  if(-not (Test-Cli39PublisherChainBorrowClosed32 $p)){return $false}
  if($null -ne $capture){
    if(-not [object]::ReferenceEquals($capture.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($capture.nativeOriginal,$n)){return $false}
    $w=$capture.stateWorkerOriginal
    if($null -ne $w){
      if(-not [object]::ReferenceEquals($w.preparation,$p) -or
          -not [object]::ReferenceEquals($w.nativeOriginal,$n) -or
          -not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
          $w.joinPendingActive -or $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
      if($w.startAttempted -and
          (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
    }
    # The complete signer/counterchain/DER/timestamp collector must extend this
    # exact worker roster with all its original borrow joins before use.
    if($null -ne $capture.headerOriginal){
      $header=$capture.headerOriginal
      if(-not [object]::ReferenceEquals($header.captureOriginal,$capture)){return $false}
      foreach($page in $header.pagesOriginal){
        $w=$page.workerOriginal
        if($null -eq $w -or -not [object]::ReferenceEquals($w.pageOriginal,$page) -or
            -not [object]::ReferenceEquals($w.headerOriginal,$header) -or
            -not $w.constructionTerminal -or $null -ne $w.constructionFailure -or
            $w.joinPendingActive -or $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
        if($w.startAttempted -and
            (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
        if($w.results[0] -eq 1 -and $w.results[1] -ne 1){return $false}
        if($w.results[1] -eq 1 -and
            ($w.results[2] -ne 1 -or $w.results[3] -ne 1 -or $null -ne $w.exceptionOriginal[1])){return $false}
      }
      $w=$header.copyWorkerOriginal
      if($null -ne $w){
        if(-not [object]::ReferenceEquals($w.headerOriginal,$header) -or
            -not [object]::ReferenceEquals($w.captureOriginal,$capture) -or
            -not $w.constructionTerminal -or $null -ne $w.constructionFailure -or
            $w.joinPendingActive -or $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
        if($w.startAttempted -and
            (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
      }
    }
    if(-not $capture.returned){return $false}
  }
  $unentered=Test-Cli39PublisherProviderUnentered26 $p
  if(-not $unentered){
    $v=$n.verifyWorkerOriginal;$close=$n.closeWorkerOriginal
    if($null -eq $v -or -not $v.terminalJoined -or $v.results[1] -ne 1 -or
        $null -eq $capture -or -not $capture.returned -or
        $null -eq $close -or -not $close.terminalJoined -or
        $close.phase -cne 'close' -or $close.results[1] -ne 1 -or
        $close.results[0] -ne 0 -or $null -ne $close.exceptionOriginal[0]){return $false}
  }
  $a=$n.storageAcquireOriginal;$release=$n.storageReleaseOriginal
  if($null -eq $a){
    if($null -ne $release -or $null -ne $n.initializeWorkerOriginal -or -not $unentered){return $false}
  }elseif(-not $a.startAttempted){
    # Exact source stores the allocation worker before constructing its body.
    # No Start means no VirtualAlloc entry. Partial CLR constructor objects
    # remain host-lifetime originals; no native backing pointer is invented.
    if($a.results[2] -ne 0 -or $a.pointerOriginal[0] -ne [IntPtr]::Zero -or
        $null -ne $release -or -not $unentered){return $false}
  }else{
    if(-not $a.terminalJoined -or $a.results[1] -ne 1){return $false}
    if($a.pointerOriginal[0] -eq [IntPtr]::Zero){
      # Actual returned null allocation is different from unknown acquisition.
      if($null -ne $release -or $null -ne $n.initializeWorkerOriginal -or -not $unentered){return $false}
    }else{
      if($null -eq $release -or -not $release.startAttempted -or
          -not $release.terminalJoined -or $release.phase -cne 'storage-release' -or
          $release.results[1] -ne 1 -or $release.results[0] -eq 0 -or
          $null -ne $release.exceptionOriginal[0]){return $false}
    }
  }
  if($null -eq $n.moduleOwners -or $n.moduleOwners.Count -gt 1){return $false}
  foreach($m in $n.moduleOwners){
    if($m.kind -cne 'cli39.publisher-resident-module.v1' -or
        $m.terminalLifetime -cne 'host-process' -or -not $m.constructionTerminal -or
        -not [object]::ReferenceEquals($m.preparation,$p) -or
        -not $p.receiver.frames.ContainsKey($m.receiverKey) -or
        -not [object]::ReferenceEquals($p.receiver.frames[$m.receiverKey],$m)){return $false}
    $w=$m.workerOriginal
    if($null -eq $w){
      if($m.loadAttempted -or $m.loadWorkerStartAttempted -or $m.module -ne [IntPtr]::Zero){return $false}
      continue
    }
    if(-not [object]::ReferenceEquals($w.preparation,$p) -or
        -not [object]::ReferenceEquals($w.nativeOriginal,$n) -or
        -not [object]::ReferenceEquals($w.moduleOwnerOriginal,$m) -or
        $w.joinPendingActive -or $w.joinMultiplicityUnknown -or $w.joinException.present){return $false}
    if($w.startAttempted -and
        (-not $w.terminalJoined -or $null -eq $w.thread -or $w.thread.IsAlive)){return $false}
    # All returned/partial Load/export/delegate state is held in this original
    # graph. No FreeLibrary or successful construction is claimed at ACK.
  }
  return $true
}
