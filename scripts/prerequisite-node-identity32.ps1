# Fixed four prepared Node verification streams, never a generic query callback.
function Join-Cli39NodePreparedIdentity32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$root=$c.rootCaptureOriginal;$v=$p.reservation.budget
  if($c.slot -cnotin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1') -or
      $c.ContainsKey('nodePreparedIdentityOriginal') -or $c.ContainsKey('disposalOriginal') -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $root.heldMemberBookOriginal.readReturned){throw 'SAME once-only fixed Node prepared identities required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported -or $v.nodePreparedIdentityBookChargeBytes -le 0 -or
      $v.nodePreparedIdentityBookChargeBytes -ge $v.nodePreparedIdentityReservationBytes -or
      4*[decimal]$c.geometry.workerStackBytes -ge $v.nodePreparedIdentityBookChargeBytes){
    throw 'Original D and full disjoint fixed four-worker identity charge required.'}
  # Genuine book charge includes all four actual workers/stacks/arrays/pins,
  # complete source-derived held-alias references, partial/error/result overlap.
  $b=@{preparationOriginal=$p;rootOriginal=$root;reservationOriginal=$p.reservation;
    bookChargeBytes=$v.nodePreparedIdentityBookChargeBytes;recordsOriginal=$null;
    returned=$false;terminal=$false;exceptionOriginal=$null}
  $c.nodePreparedIdentityOriginal=$b
  try{
    $b.recordsOriginal=[Collections.Generic.List[object]]::new(4)
    foreach($role in @('executable','keyring','signature','listing')){
      Assert-Cli39PrerequisiteUseful26 $p
      $subject=$c.subjectOriginals[$role];$pin=$c.subjectPins[$role]
      $r=@{bookOriginal=$b;roleOriginal=$role;subjectOriginal=$subject;bytesExpected=$subject.used;
        fileQueryOriginal=$null;heldRowsOriginal=$null;identityComparedBytes=0;returned=$false;exceptionOriginal=$null}
      $b.recordsOriginal.Add($r)
      if($null -eq $subject -or -not $subject.eof -or -not $subject.hashReturned -or
          $subject.path -cne $pin.path -or $subject.used -ne $pin.bytes -or $subject.digest -ine $pin.sha256){
        throw 'Actual complete prepared Node subject differs from selected original.'}
      $r.heldRowsOriginal=[Collections.Generic.List[object]]::new()
      foreach($held in $root.heldMemberBookOriginal.recordsOriginal){
        Assert-Cli39PrerequisiteUseful26 $p
        if($held.subjectOriginal.path -ceq $subject.path){
          if(-not $held.returned -or -not $held.subjectOriginal.eof -or -not $held.subjectOriginal.hashReturned -or
              $held.subjectOriginal.used -ne $subject.used -or $held.subjectOriginal.digest -cne $subject.digest -or
              -not $held.fileQueryOriginal.rawReturned -or -not $held.fileQueryOriginal.decodeOriginal.returned){
            throw 'Actual complete held Node alias differs from prepared stream.'}
          $r.heldRowsOriginal.Add($held)
        }
      }
      if($r.heldRowsOriginal.Count -eq 0){throw 'Prepared Node subject lacks actual held native member.'}
    }
    foreach($r in $b.recordsOriginal){
      try{
        $w=New-Cli39RootMemberFileQueryWorker32 $p $r
        $null=Invoke-Cli39RootMemberFileQuery32 $w
        $d=Complete-Cli39RootMemberFileInformation32 $w
        if(-not $d.returned -or -not $d.heldExtentEqual -or $d.directoryRawOriginal -ne 0 -or
            $d.reparseTagOriginal -ne 0 -or ($d.tagAttributesOriginal -band 0x400) -ne 0){
          throw 'Prepared Node subject native regular extent differs.'}
        foreach($held in $r.heldRowsOriginal){
          $r.identityComparedBytes=0
          for($index=0;$index -lt 24;$index++){
            Assert-Cli39PrerequisiteUseful26 $p
            if($w.buffersOriginal[3][$index] -ne $held.fileQueryOriginal.buffersOriginal[3][$index]){
              throw 'Prepared Node volume/full FileId differs from actual held alias.'}
            $r.identityComparedBytes=$index+1
          }
        }
        if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $r)){throw 'Prepared Node query resource closure incomplete.'}
        $r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}
    }
    Assert-Cli39PrerequisiteUseful26 $p
    $b.returned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}finally{$b.terminal=$true}
}
function Test-Cli39NodePreparedIdentityClosure32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$b=$p.consumerOriginal.nodePreparedIdentityOriginal
  if($null -eq $b){return $true}
  if(-not [object]::ReferenceEquals($b.preparationOriginal,$p) -or -not $b.terminal){return $false}
  foreach($r in $b.recordsOriginal){if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $r)){return $false}}
  return $true
}
function Test-Cli39NodePreparedIdentityBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation,$SubjectOriginal=$null)
  $p=$OriginalPreparation;$b=$p.consumerOriginal.nodePreparedIdentityOriginal
  if($null -eq $b){return $true}
  if(-not [object]::ReferenceEquals($b.preparationOriginal,$p) -or -not $b.terminal){return $false}
  foreach($r in $b.recordsOriginal){
    if($null -ne $SubjectOriginal -and -not [object]::ReferenceEquals($r.subjectOriginal,$SubjectOriginal)){continue}
    if(-not (Test-Cli39FixedFileQueryMember32 $p $r)){return $false}
    $w=$r.fileQueryOriginal
    if($null -eq $w){continue}
    if(-not $w.constructorTerminal -or -not [object]::ReferenceEquals($w.memberOriginal,$r) -or
        -not [object]::ReferenceEquals($w.hostOwnerOriginal,$p) -or
        -not [object]::ReferenceEquals($w.subjectOriginal,$r.subjectOriginal)){return $false}
    if(-not $w.startAttempted){if($null -ne $w.threadOriginal -and $w.threadOriginal.IsAlive){return $false};continue}
    if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
        $w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){return $false}
    for($index=0;$index -lt 4;$index++){
      if($w.callMarkers[$index*2] -eq 1 -and $w.callMarkers[$index*2+1] -ne 1){return $false}
    }
    if($w.borrowMarkers[0] -eq 1 -and $w.borrowMarkers[1] -ne 1){return $false}
    if($w.borrowOriginal[0] -and ($w.borrowMarkers[2] -ne 1 -or $w.borrowMarkers[3] -ne 1)){return $false}
  }
  # Failed pin disposal is retained separately by full Closure; joined known
  # native return and actual SafeHandle release cease borrowing the file.
  return $true
}
function Test-Cli39NodeResourceDisposalEligible32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$ResourceOriginal)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$r=$ResourceOriginal
  if($c.slot -cnotin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1')){throw 'Fixed Node resource route required.'}
  # Native file queries borrow streams/SafeHandles only; hash objects and raw
  # output streams are independent of these four subject query workers.
  if($null -eq $r.handleOriginal){return $true}
  $subjectMatched=$false
  foreach($subject in $c.subjectOriginals.Values){
    if([object]::ReferenceEquals($subject,$r.sourceOriginal)){
      if(-not [object]::ReferenceEquals($subject.stream,$r.objectOriginal) -or
          -not [object]::ReferenceEquals($subject.handleOriginal,$r.handleOriginal)){return $false}
      $subjectMatched=$true
    }
  }
  if(-not $subjectMatched){return $true}
  return (Test-Cli39NodePreparedIdentityBorrowClosed32 $p $r.sourceOriginal)
}