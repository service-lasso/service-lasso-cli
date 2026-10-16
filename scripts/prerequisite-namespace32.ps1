# Actual root/member namespace observations. No authority from path/hash equality.
function Observe-Cli39RootNamespace32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$members=$root.heldMemberBookOriginal;$directories=$root.namespaceRolesOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$p.receiver) -or
      $root.ContainsKey('namespaceBookOriginal') -or -not $members.readReturned -or
      $null -eq $directories -or -not $directories.returned -or -not $directories.constructionTerminal -or
      -not [object]::ReferenceEquals($directories.rootCaptureOriginal,$root) -or
      -not $root.rootFileQueryOriginal.rawReturned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only complete held originals required.'}
  $b=@{hostOwnerOriginal=$p;rootOriginal=$root;abiOriginal=$root.abiOriginal;
    pathOwnersOriginal=$null;holdsOriginal=$null;workersOriginal=$null;componentCount=0;
    rosterReturned=$false;constructorTerminal=$false;constructorFailure=$null;
    observationReturned=$false;exceptionOriginal=$null;disposalOriginal=$null;nativeAuthority='UNKNOWN'}
  $root.namespaceBookOriginal=$b
  try{
    [long]$ownerCount=[long]$members.memberCount+1+$directories.rowsOriginal.Count
    if($ownerCount -ge [int]::MaxValue){throw 'Complete file/directory owner roster exceeds source representation.'}
    $b.pathOwnersOriginal=[Collections.Generic.List[object]]::new([int]$ownerCount)
    # Root, every held file, then each fixed directory role. Aliases remain distinct.
    for($sourceIndex=0;$sourceIndex -lt $ownerCount;$sourceIndex++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended namespace roster construction.'}
      $directory=$null;$subject=$null;$query=$null;$member=$null
      if($sourceIndex -eq 0){$subject=$root.rootSubjectOriginal;$query=$root.rootFileQueryOriginal}
      elseif($sourceIndex -le $members.memberCount){$member=$members.recordsOriginal[$sourceIndex-1];$subject=$member.subjectOriginal;$query=$member.fileQueryOriginal}
      else{$directory=$directories.rowsOriginal[$sourceIndex-$members.memberCount-1]}
      if($null -eq $directory -and (-not $query.rawReturned -or -not [object]::ReferenceEquals($query.subjectOriginal,$subject))){
        throw 'Actual SAME complete held subject query required before named leaf join.'
      }
      $path=$null;if($null -ne $directory){$path=$directory.pathOriginal}else{$path=$subject.path}
      $o=@{bookOriginal=$b;subjectOriginal=$subject;memberOriginal=$member;queryOriginal=$query;
        directoryRoleOriginal=$directory;leafDirectoryOriginal=($null -ne $directory);
        pathOriginal=$path;rootPathOriginal=$null;componentsOriginal=$null;holdsOriginal=$null;
        rosterReturned=$false;joinReturned=$false;directoryObserved=$false;observationReturned=$false;exceptionOriginal=$null}
      $b.pathOwnersOriginal.Add($o)
      $o.componentsOriginal=[Collections.Generic.List[string]]::new()
      $o.rootPathOriginal=[IO.Path]::GetPathRoot($o.pathOriginal)
      if([string]::IsNullOrEmpty($o.rootPathOriginal)){throw 'Actual absolute path root required.'}
      $trim=[char[]]@(92,47);$rootText=$o.rootPathOriginal.TrimEnd($trim);$current=$o.pathOriginal
      while(-not [string]::IsNullOrEmpty($current)){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended actual path component construction.'}
        if($o.componentsOriginal.Count -gt $o.pathOriginal.Length){throw 'Original path component geometry escaped string extent.'}
        $o.componentsOriginal.Add($current)
        if($current.TrimEnd($trim) -ceq $rootText){break}
        $parent=[IO.Path]::GetDirectoryName($current)
        if([string]::IsNullOrEmpty($parent) -or $parent -ceq $current){throw 'Actual component chain did not reach original volume/share root.'}
        $current=$parent
      }
      $o.componentsOriginal.Reverse()
      [long]$total=[long]$b.componentCount+$o.componentsOriginal.Count
      if($total -gt ([int]::MaxValue/3)){throw 'Complete three-worker component geometry unsupported.'}
      $b.componentCount=[int]$total
      $o.holdsOriginal=[Collections.Generic.List[object]]::new($o.componentsOriginal.Count)
    }
    $b.holdsOriginal=[Collections.Generic.List[object]]::new($b.componentCount)
    $b.workersOriginal=[Collections.Generic.List[object]]::new(3*$b.componentCount)
    foreach($o in $b.pathOwnersOriginal){
      for($index=0;$index -lt $o.componentsOriginal.Count;$index++){
        $h=@{bookOriginal=$b;pathOwnerOriginal=$o;ordinalOriginal=$index;
          pathOriginal=$o.componentsOriginal[$index];leafOriginal=($index -eq $o.componentsOriginal.Count-1);
          pointerCellsOriginal=$null;buffersOriginal=$null;workersOriginal=@{open=$null;metadata=$null;close=$null};
          attributesOriginal=0U;reparseTagOriginal=0U;identityBytesCompared=0;metadataReturned=$false;identityJoined=$false;kindObserved=$false;
          closeEligible=$false;closePhysicalReturned=$false;noHandleObserved=$false;exceptionOriginal=$null;closeExceptionOriginal=$null}
        $b.holdsOriginal.Add($h);$o.holdsOriginal.Add($h)
        $h.pointerCellsOriginal=[IntPtr[]]::new(2);$h.buffersOriginal=[object[]]::new(2)
        $h.buffersOriginal[0]=[byte[]]::new(8);$h.buffersOriginal[1]=[byte[]]::new(24)
        foreach($phase in @('open','metadata','close')){$null=New-Cli39NamespaceWorker32 $p $h $phase}
      }
      $o.rosterReturned=$true
    }
    if($b.holdsOriginal.Count -ne $b.componentCount -or $b.workersOriginal.Count -ne 3*$b.componentCount){
      throw 'Complete original namespace worker roster differs.'
    }
    $b.rosterReturned=$true
  }catch{$b.constructorFailure=$_;throw}finally{$b.constructorTerminal=$true}
  try{
    foreach($o in $b.pathOwnersOriginal){
      foreach($h in $o.holdsOriginal){
        $null=Invoke-Cli39NamespaceWorker32 $h.workersOriginal['open']
        if($h.pointerCellsOriginal[1] -eq [IntPtr]::Zero -or $h.pointerCellsOriginal[1] -eq [IntPtr]::new(-1)){
          throw 'Actual namespace acquisition supplied no usable handle; raw output/error retained.'
        }
        $q=Invoke-Cli39NamespaceWorker32 $h.workersOriginal['metadata']
        if($q.boolOriginal[0] -eq 0 -or $q.boolOriginal[1] -eq 0){throw 'Actual namespace metadata BOOL failure retained.'}
        $h.attributesOriginal=[BitConverter]::ToUInt32($h.buffersOriginal[0],0)
        $h.reparseTagOriginal=[BitConverter]::ToUInt32($h.buffersOriginal[0],4);$h.metadataReturned=$true
        $expectedDirectory=(-not $h.leafOriginal) -or $o.leafDirectoryOriginal
        if(($h.attributesOriginal -band 0x400) -ne 0 -or
            ((($h.attributesOriginal -band 0x10) -ne 0) -ne $expectedDirectory)){
          throw 'Actual namespace component is redirected or has a different file/directory kind.'
        }
        $h.kindObserved=$true
        if($h.leafOriginal -and -not $o.leafDirectoryOriginal){
          for($byte=0;$byte -lt 24;$byte++){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline ended SAME named-to-held identity comparison.'}
            if($h.buffersOriginal[1][$byte] -ne $o.queryOriginal.buffersOriginal[3][$byte]){
              throw 'Actual named leaf volume/FileId differs from SAME held source.'
            }
            $h.identityBytesCompared=$byte+1
          }
          $h.identityJoined=$true
        }
      }
      if($o.leafDirectoryOriginal){$o.directoryObserved=$true}else{$o.joinReturned=$true}
      $o.observationReturned=$true
    }
    $b.observationReturned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}
}
function Invoke-Cli39RootNamespaceDisposal32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$b=$root.namespaceBookOriginal
  if($null -eq $b){return}
  if($null -ne $b.disposalOriginal){throw 'Namespace disposal cannot retry/reset an original.'}
  $d=@{bookOriginal=$b;returned=$false;pendingOriginal=$false;failuresOriginal=$null;
    receiverClosureOriginal=@{attempted=$false;returned=$false;value=$false;exceptionOriginal=$null}}
  $b.disposalOriginal=$d
  $d.failuresOriginal=[Collections.Generic.List[object]]::new($b.componentCount)
  # Future acquisition/metadata is now closed. The root disposal caller has
  # joined actual managed readers first. The SAME shared receiver below joins
  # every source-enrolled child/publisher borrower before these holds close.
  if(-not (Test-Cli39RootFileQueryClosure32 $p) -or
      -not (Test-Cli39AllRootMemberFileQueriesClosure32 $p)){
    $d.pendingOriginal=$true;return $d
  }
  $receiverObservation=$d.receiverClosureOriginal;$receiverObservation.attempted=$true
  try{
    $receiverObservation.value=Test-NativeProducerReceiverPhysicalClosure -Receiver $p.receiver -HostOwner $p
    $receiverObservation.returned=$true
  }catch{$receiverObservation.exceptionOriginal=$_;$d.pendingOriginal=$true;return $d}
  if(-not $receiverObservation.value){$d.pendingOriginal=$true;return $d}
  foreach($h in $b.holdsOriginal){
    try{
      $open=$h.workersOriginal['open'];$metadata=$h.workersOriginal['metadata']
      if($null -eq $open -or -not (Test-Cli39NamespaceWorkerBorrowClosure32 $open) -or
          $null -eq $metadata -or -not (Test-Cli39NamespaceWorkerBorrowClosure32 $metadata)){
        $d.pendingOriginal=$true;continue
      }
      if(-not $open.startAttempted){$h.noHandleObserved=$true;continue}
      if($open.markersOriginal[3] -ne 1){$d.pendingOriginal=$true;continue}
      if($h.pointerCellsOriginal[1] -eq [IntPtr]::new(-1)){$h.noHandleObserved=$true;continue}
      if($h.pointerCellsOriginal[1] -eq [IntPtr]::Zero){$d.pendingOriginal=$true;continue}
      $h.closeEligible=$true
      try{$null=Invoke-Cli39NamespaceWorker32 $h.workersOriginal['close']}
      catch{$h.closeExceptionOriginal=$_;$d.failuresOriginal.Add($_)}
      $close=$h.workersOriginal['close']
      if((Test-Cli39NamespaceWorkerBorrowClosure32 $close) -and
          $close.markersOriginal[1] -eq 1 -and $close.boolOriginal[0] -ne 0){$h.closePhysicalReturned=$true}
      else{$d.pendingOriginal=$true}
    }catch{$h.exceptionOriginal=$_;$d.failuresOriginal.Add($_);$d.pendingOriginal=$true}
  }
  $d.returned=$true;return $d
}
function Test-Cli39RootNamespaceClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or $null -eq $root.namespaceBookOriginal){return $true}
  $b=$root.namespaceBookOriginal;$d=$b.disposalOriginal
  if(-not $b.constructorTerminal -or $null -ne $b.constructorFailure -or
      $null -eq $d -or -not $d.returned -or $d.pendingOriginal -or
      -not $d.receiverClosureOriginal.attempted -or -not $d.receiverClosureOriginal.returned -or
      -not $d.receiverClosureOriginal.value -or $null -ne $d.receiverClosureOriginal.exceptionOriginal){return $false}
  foreach($h in $b.holdsOriginal){
    if(-not $h.noHandleObserved -and -not $h.closePhysicalReturned){return $false}
    foreach($phase in @('open','metadata','close')){
      if(-not (Test-Cli39NamespaceWorkerBorrowClosure32 $h.workersOriginal[$phase])){return $false}
    }
  }
  return $true
}
