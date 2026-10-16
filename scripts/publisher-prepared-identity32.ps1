# Three fixed prepared streams retain their own actual four-query observations.
function Join-Cli39PublisherPreparedIdentity32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal;$root=$c.rootCaptureOriginal
  $transport=$root.windowsPolicyFoundationTransportOriginal
  if($c.slot -cne 'cli39.windows-publisher-trust.v1' -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $n.ContainsKey('preparedIdentityOriginal') -or -not $transport.returned -or
      $null -ne $transport.exceptionOriginal -or $transport.subjectRowsOriginal.Count -ne 3 -or
      $c.ContainsKey('disposalOriginal')){throw 'SAME live once-only three publisher prepared identities required.'}
  $b=@{preparationOriginal=$p;rootOriginal=$root;transportOriginal=$transport;
    chargeOriginal=$n.custodyReservationOriginal;recordsOriginal=$null;
    returned=$false;terminal=$false;exceptionOriginal=$null;
    state='SAME_HANDLE_PREPARED_TO_HELD_CORRESPONDENCE_NOT_OS_POLICY_AUTHENTICATION'}
  $n.preparedIdentityOriginal=$b
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported -or $null -eq $b.chargeOriginal -or
        -not [object]::ReferenceEquals($b.chargeOriginal.preparationOriginal,$p)){
      throw 'Original deadline/SAME precharged full fixed query book required.'}
    # Full fixed book + three query graphs/stacks are part of the genuine
    # publisherCustodyBookChargeBytes, reserved before any subject acquisition.
    $b.recordsOriginal=[Collections.Generic.List[object]]::new(3)
    foreach($role in @('publisher-subject','wintrust','expected-publisher-cert')){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies prepared identity enrollment.'}
      $subject=$c.subjectOriginals[$role]
      $r=@{bookOriginal=$b;roleOriginal=$role;subjectOriginal=$subject;
        bytesExpected=$subject.used;fileQueryOriginal=$null;transportRowOriginal=$null;
        heldRowsOriginal=$null;identityComparedBytes=0;aliasesCompared=0;
        returned=$false;exceptionOriginal=$null}
      $b.recordsOriginal.Add($r)
      if($null -eq $subject -or -not $subject.eof -or -not $subject.hashReturned){throw 'Actual completed prepared subject required.'}
      foreach($row in $transport.subjectRowsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies fixed publisher transport lookup.'}
        if($row.operandOriginal.role -ceq $role){
          if($null -ne $r.transportRowOriginal -or -not $row.returned){throw 'Duplicate/incomplete actual publisher subject transport.'}
          $r.transportRowOriginal=$row;$r.heldRowsOriginal=$row.heldRowsOriginal
        }
      }
      if($null -eq $r.transportRowOriginal -or $r.heldRowsOriginal.Count -eq 0){throw 'Actual held publisher aliases absent.'}
    }
    foreach($r in $b.recordsOriginal){
      try{
        $w=New-Cli39RootMemberFileQueryWorker32 $p $r
        $null=Invoke-Cli39RootMemberFileQuery32 $w
        $d=Complete-Cli39RootMemberFileInformation32 $w
        if(-not $d.returned -or -not $d.heldExtentEqual -or $d.directoryRawOriginal -ne 0 -or
            $d.reparseTagOriginal -ne 0 -or ($d.tagAttributesOriginal -band 0x400) -ne 0){throw 'Actual prepared regular nonreparse extent differs.'}
        foreach($held in $r.heldRowsOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies prepared/held identity association.'}
          $member=$held.memberOriginal;$query=$member.fileQueryOriginal
          if(-not $held.returned -or $held.identityComparedBytes -ne 24 -or
              -not $query.rawReturned -or -not $query.decodeOriginal.returned -or
              $member.subjectOriginal.path -cne $r.subjectOriginal.path -or
              $member.subjectOriginal.used -ne $r.bytesExpected -or
              $member.subjectOriginal.digest -cne $r.subjectOriginal.digest){throw 'Actual prepared/held subject correspondence differs.'}
          $r.identityComparedBytes=0
          for($index=0;$index -lt 24;$index++){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline denies full prepared FileId correspondence.'}
            if($w.buffersOriginal[3][$index] -ne $query.buffersOriginal[3][$index]){throw 'Actual prepared/held volume and full FileId differ.'}
            $r.identityComparedBytes=$index+1
          }
          $r.aliasesCompared++
        }
        if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $r)){throw 'Actual prepared query resource closure incomplete.'}
        $r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies prepared identity publication.'}
    $b.returned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}finally{$b.terminal=$true}
}
function Test-Cli39PublisherPreparedIdentityClosure32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal
  if(-not $n.ContainsKey('preparedIdentityOriginal')){return $true}
  $b=$n.preparedIdentityOriginal
  if($null -eq $b -or -not [object]::ReferenceEquals($b.preparationOriginal,$p) -or -not $b.terminal){return $false}
  foreach($r in $b.recordsOriginal){if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $r)){return $false}}
  return $true
}
function Test-Cli39PublisherPreparedIdentityBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal
  if(-not $n.ContainsKey('preparedIdentityOriginal')){return $true}
  $b=$n.preparedIdentityOriginal
  if($null -eq $b -or -not [object]::ReferenceEquals($b.preparationOriginal,$p) -or -not $b.terminal){return $false}
  foreach($r in $b.recordsOriginal){
    $w=$r.fileQueryOriginal;if($null -eq $w){continue}
    if(-not [object]::ReferenceEquals($w.memberOriginal,$r) -or
        -not [object]::ReferenceEquals($w.subjectOriginal,$r.subjectOriginal) -or
        -not $w.constructorTerminal){return $false}
    if(-not $w.startAttempted){if($null -ne $w.threadOriginal -and $w.threadOriginal.IsAlive){return $false};continue}
    if($null -eq $w.threadOriginal -or -not $w.terminalJoined -or $w.threadOriginal.IsAlive -or
        $w.pendingActive -or $w.multiplicityUnknown -or $null -ne $w.joinException){return $false}
    if($w.borrowMarkers[0] -eq 1 -and $w.borrowMarkers[1] -ne 1){return $false}
    if($w.borrowOriginal[0] -and ($w.borrowMarkers[2] -ne 1 -or $w.borrowMarkers[3] -ne 1)){return $false}
    for($index=0;$index -lt 4;$index++){
      if($w.callMarkers[$index*2] -eq 1 -and $w.callMarkers[$index*2+1] -ne 1){return $false}
    }
    # Failed pin disposal remains retained and denies complete resource closure,
    # but after actual native return/join/release it no longer borrows the file.
  }
  return $true
}
function Join-Cli39PublisherExecutionBindings32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$n=$c.publisherNativeOriginal;$root=$c.rootCaptureOriginal
  $m=$n.wintrustModuleOriginal;$prepared=$n.preparedIdentityOriginal
  $transport=$root.windowsPolicyFoundationTransportOriginal;$a=$root.nativeAssociationOriginals
  if($n.ContainsKey('executionBindingOriginal') -or -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $prepared.returned -or -not $prepared.terminal -or $null -ne $prepared.exceptionOriginal -or
      -not [object]::ReferenceEquals($prepared.preparationOriginal,$p) -or
      $null -eq $m -or -not $m.constructionTerminal -or $null -ne $m.constructorFailure -or
      -not [object]::ReferenceEquals($m.preparation,$p) -or
      -not [object]::ReferenceEquals($m.subjectOriginal,$c.subjectOriginals['wintrust']) -or
      -not [object]::ReferenceEquals($a.consumerReservationOriginal,$p.reservation) -or
      -not [object]::ReferenceEquals($root.admittedConsumerOriginal,$c) -or $c.ContainsKey('disposalOriginal')){
    throw 'SAME completed prepared/module/root reservation relations required.'}
  $b=@{preparationOriginal=$p;rootOriginal=$root;admittedInputOriginal=$root.admittedInputOriginal;
    nativeAssociationsOriginal=$a;preparedIdentityOriginal=$prepared;moduleOriginal=$m;
    transportOriginal=$transport;wintrustRowOriginal=$null;loadedModuleOriginal=$null;
    sourceProfileOriginal=$null;returned=$false;terminal=$false;exceptionOriginal=$null;
    state='LOCAL_EXECUTION_CORRESPONDENCE_UNDER_GENUINE_EXTERNAL_WHOLE_INPUT_PRECONDITION'}
  $n.executionBindingOriginal=$b
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported -or -not $transport.returned -or $null -ne $transport.exceptionOriginal -or
        -not [object]::ReferenceEquals($transport.rootOriginal,$root) -or
        -not [object]::ReferenceEquals($transport.parentTransportOriginal,$root.childDerivedTransportOriginal)){
      throw 'Actual SAME controlling publisher transport required before original D.'}
    foreach($row in $transport.subjectRowsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies actual Wintrust row lookup.'}
      if($row.operandOriginal.role -ceq 'wintrust'){
        if($null -ne $b.wintrustRowOriginal -or -not $row.returned){throw 'Duplicate/incomplete actual Wintrust row.'}
        $b.wintrustRowOriginal=$row
      }
    }
    $loaded=$b.wintrustRowOriginal.loadedRowOriginal;$b.loadedModuleOriginal=$loaded.actualModuleOriginal
    if($null -eq $loaded -or -not $loaded.returned -or -not $b.loadedModuleOriginal.returned -or
        -not [object]::ReferenceEquals($loaded.installedMemberOriginal,$root.publisherFoundationSelectorOriginal.wintrustMemberOriginal) -or
        $m.module -eq [IntPtr]::Zero -or $m.module -ne $b.loadedModuleOriginal.decodedOriginal.moduleHandleOriginal -or
        -not [object]::ReferenceEquals($loaded.actualProcessOriginal,$root.rootProcessOriginal)){
      throw 'Actual Load module differs from SAME current child installed/loaded mapping.'}
    $profile=$root.resultOriginal.consumerProfileOriginal
    if(-not $profile.returned -or $profile.role -cne 'consumerBudget' -or $profile.actionOriginal -cne 'publisher' -or
        -not [object]::ReferenceEquals($profile.valuesOriginal,$p.reservation.budget) -or
        -not [object]::ReferenceEquals($n.custodyReservationOriginal.profileOriginal,$profile) -or
        -not $a.allocationAssociationOriginal.returned -or -not $a.loadedAssociationOriginal.returned -or
        $null -eq $a.sourceAssociationOriginal -or $null -eq $a.toolAssociationOriginal -or $null -eq $a.installedAssociationOriginal){
      throw 'Complete SAME actual source/tool/installed/allocation/profile correspondence required.'}
    $b.sourceProfileOriginal=@{rootOriginal=$root;admittedInputOriginal=$b.admittedInputOriginal;
      sourceOriginal=$a.sourceAssociationOriginal;toolOriginal=$a.toolAssociationOriginal;installedOriginal=$a.installedAssociationOriginal;
      allocationOriginal=$a.allocationAssociationOriginal;loadedOriginal=$a.loadedAssociationOriginal;
      profileOriginal=$profile;reservationOriginal=$p.reservation;custodyOriginal=$n.custodyReservationOriginal;
      qualificationPremise='AUTHENTIC_EXTERNAL_WHOLE_INPUT_AND_GENUINE_PHYSICAL_CAPACITY_REQUIRED'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies execution correspondence publication.'}
    $n.namespaceAssociationOriginal=$prepared
    $n.loadedAssociationOriginal=$b
    $n.completeSourceProfileOriginal=$b.sourceProfileOriginal
    $b.returned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}finally{$b.terminal=$true}
}
