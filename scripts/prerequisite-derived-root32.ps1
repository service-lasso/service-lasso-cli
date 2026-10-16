# Fixed role-specific derived root; no hash or copied parent field grants trust.
function New-Cli39DerivedRoot32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)][ValidateSet('retained-caller','operation-host')][string]$ChildRole)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($root.ContainsKey('derivedRootOriginal') -or -not $root.structuralReturned -or
      -not $root.nativeObservationOriginal.returned -or $null -eq $root.admittedInputOriginal -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal -or
      ($ChildRole -ceq 'retained-caller' -and $root.rootRoleOriginal -cne 'qualification-coordinator') -or
      ($ChildRole -ceq 'operation-host' -and $root.rootRoleOriginal -cne 'retained-caller')){
    throw 'SAME once-only admitted parent and fixed child role required.'
  }
  $b=@{hostOwnerOriginal=$p;rootOriginal=$root;admittedParentOriginal=$root.admittedInputOriginal;
    childRoleOriginal=$ChildRole;profileOriginal=$null;pathOriginal=$null;capacityOriginal=0;backingChargeBytes=0L;chargeBytes=0D;
    namesOriginal=$null;elementsOriginal=$null;lineageNamesOriginal=$null;lineageValuesOriginal=$null;
    projectionOriginal=$null;projectionPreflightBytes=0D;projectionPreflightCharge=0D;parentTransportOriginal=$null;
    backingOriginal=$null;readbackOriginal=$null;resourcesOriginal=$null;digestOriginal=$null;countsOriginal=$null;
    markersOriginal=$null;exceptionsOriginal=$null;readLengthsOriginal=$null;readMarkersOriginal=$null;
    writeWorkerOriginal=$null;disposalWorkerOriginal=$null;constructorTerminal=$false;constructorFailure=$null;
    completionAttempted=$false;returned=$false;exceptionOriginal=$null;heldSubjectOriginal=$null;memberOriginal=$null;
    childInvocationOriginal=$null;disposalOriginal=$null;nativeIdentityOriginal=$null;immutableCustodyOriginal=$null}
  $root.derivedRootOriginal=$b
  try{
    $reservation=$b.admittedParentOriginal.parentReservationOriginal;$b.profileOriginal=$reservation.budget
    $profileMatched=$false
    foreach($profile in $root.profileOriginals){if($profile.role -cin @('callerBudget','hostBudget') -and $profile.returned -and
        [object]::ReferenceEquals($profile.valuesOriginal,$b.profileOriginal)){$profileMatched=$true}}
    if(-not $profileMatched -or $reservation.inputRoot -cne $root.sha256Expected){throw 'Actual parent typed reservation/root original required.'}
    $v=$b.profileOriginal
    if($v.cliDerivedRootBookChargeBytes -le 0 -or $v.cliDerivedRootPerWorkerChargeBytes -le 0 -or
        [decimal]$v.cliDerivedRootBookChargeBytes+3*[decimal]$v.cliDerivedRootPerWorkerChargeBytes -ge $v.cliDerivedRootReservationBytes){
      throw 'Preexisting derived book/complete small lineage and worker reservation required before encoding originals.'
    }
    $process=$root.rootProcessOriginal;$tokens=$root.principalModuleOriginal.tokenBookOriginal
    if(-not $process.rawReturned -or -not $tokens.returned -or -not $tokens.workersOriginal[0].rawReturned -or
        -not $tokens.workersOriginal[1].rawReturned){throw 'Actual parent process and two captured token observations required.'}
    $user=$tokens.workersOriginal[0];$statistics=$tokens.workersOriginal[1].decodedOriginal.fieldsOriginal
    $sid=$user.decodedOriginal
    if(-not $sid.returned -or $null -eq $sid.sidOffsetOriginal -or $null -eq $sid.sidExtentOriginal){throw 'Actual captured parent SID alias required.'}
    $b.lineageNamesOriginal=[string[]]@('schema','role','action','absoluteDeadlineTicks','parentInputSha256',
      'parentProcessId','parentCreationTime','parentSidHex','parentTokenId','parentAuthenticationId')
    $culture=[Globalization.CultureInfo]::InvariantCulture
    $b.lineageValuesOriginal=[string[]]@('service-lasso.cli39-parent-lineage.v1',$ChildRole,$root.actionOriginal,
      $p.originalDeadlineTicks.ToString($culture),$root.sha256Expected,
      $process.pidOriginal[0].ToString($culture),[BitConverter]::ToUInt64($process.timesOriginal,0).ToString($culture),
      [BitConverter]::ToString($user.backingOriginal,$sid.sidOffsetOriginal,$sid.sidExtentOriginal).Replace('-','').ToLowerInvariant(),
      $statistics['tokenIdObserved'].ToString($culture),$statistics['authenticationIdObserved'].ToString($culture))
    $projection=New-Cli39StaticFoundationProjection32 $b
    # Six bytes per original input byte conservatively covers JSON string
    # escaping; fixed added string/key punctuation is derived independently.
    [decimal]$capacity=6*[decimal]$root.bytesExpected+6*[decimal]('service-lasso.cli39-prerequisite-input.v3'.Length)+2
    $capacity+=6*[decimal]('parentLineage'.Length)+6
    $capacity+=$projection.escapedCapacity
    [decimal]$lineageBacking=$projection.utf16Bytes
    for($i=0;$i -lt $b.lineageNamesOriginal.Length;$i++){
      $capacity+=6*([decimal]$b.lineageNamesOriginal[$i].Length+[decimal]$b.lineageValuesOriginal[$i].Length)+6
      $lineageBacking+=2*([decimal]$b.lineageNamesOriginal[$i].Length+[decimal]$b.lineageValuesOriginal[$i].Length)
    }
    if($capacity -le 0 -or $capacity -gt ([int]::MaxValue-4)/2){throw 'Derived serialized/read roster extent cannot be represented.'}
    $b.capacityOriginal=[int]$capacity
    # Two byte buffers, one Int32 length and two Int32 markers per maximum
    # physical read, plus actual retained lineage UTF16 backing. Object/array
    # allocation and writer/FileStream4096 buffers belong to genuine book/
    # per-byte charges rather than an extra domain allowance.
    $b.backingChargeBytes=[long](2*$capacity+1+12*($capacity+2)+$lineageBacking+96)
    $v=$b.profileOriginal
    $b.chargeBytes=[decimal]$v.cliDerivedRootBookChargeBytes+3*[decimal]$v.cliDerivedRootPerWorkerChargeBytes+
      [decimal]$b.backingChargeBytes*[decimal]$v.cliDerivedRootPerByteChargeBytes
    if($v.cliDerivedRootWorkerStackBytes -le 0 -or $v.cliDerivedRootWorkerStackBytes -gt [int]::MaxValue -or
        $v.cliDerivedRootPerWorkerChargeBytes -le $v.cliDerivedRootWorkerStackBytes -or
        $v.cliDerivedRootPerByteChargeBytes -le 0 -or $b.chargeBytes -gt $v.cliDerivedRootReservationBytes){
      throw 'Actual derived-root full source backing/worker geometry cannot fit SAME subdivision.'
    }
    $prefixField=if($ChildRole -ceq 'retained-caller'){'callerOutputPrefix'}else{'hostOutputPrefix'}
    $b.pathOriginal=$root.fieldMapOriginal[$prefixField].GetString()+'.'+$ChildRole+'.input.json'
    $count=$root.fieldMapOriginal.Count-1
    if($root.fieldMapOriginal.ContainsKey('parentLineage')){$count--}
    if($root.fieldMapOriginal.ContainsKey('foundationProjection')){$count--}
    $b.namesOriginal=[string[]]::new($count);$b.elementsOriginal=[System.Text.Json.JsonElement[]]::new($count)
    $index=0
    foreach($property in $root.fieldOriginals){
      if($property.Name -cin @('schema','parentLineage','foundationProjection')){continue}
      $b.namesOriginal[$index]=$property.Name;$b.elementsOriginal[$index]=$property.Value;$index++
    }
    if($index -ne $count){throw 'Exact original static root property roster changed.'}
    $b.backingOriginal=[byte[]]::new($b.capacityOriginal);$b.readbackOriginal=[byte[]]::new($b.capacityOriginal+1)
    $b.resourcesOriginal=[object[]]::new(4);$b.digestOriginal=[object[]]::new(1);$b.countsOriginal=[long[]]::new(3)
    $b.markersOriginal=[int[]]::new(17);$b.exceptionsOriginal=[object[]]::new(1)
    $b.readLengthsOriginal=[int[]]::new($b.capacityOriginal+2);$b.readMarkersOriginal=[int[]]::new(2*($b.capacityOriginal+2))
    $null=New-Cli39DerivedRootWorker32 $b
    $null=New-Cli39DerivedRootDisposalWorker32 $b
    return $b
  }catch{$b.constructorFailure=$_;throw}finally{$b.constructorTerminal=$true}
}

function Invoke-Cli39DerivedRootWrite32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$p=$b.hostOwnerOriginal;$w=$b.writeWorkerOriginal
  if(-not [object]::ReferenceEquals($p.inputRead.value.derivedRootOriginal,$b) -or -not $b.constructorTerminal -or
      $null -ne $b.constructorFailure -or $w.startAttempted -or $null -ne $b.disposalOriginal -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $b.rootOriginal.namespaceBookOriginal.disposalOriginal){throw 'SAME once-only derived-root worker required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline denies derived-root creation.'}
  $w.startAttempted=$true
  try{$w.threadOriginal.Start($w.contextOriginal);$w.startReturned=$true}catch{$w.startException=$_;throw}
  while(-not $w.terminalJoined){
    Observe-OwnedPrerequisiteDeadline $p
    if($null -ne $w.joinException -or $w.joinMultiplicityUnknown){throw 'Unknown actual derived-root worker join retained.'}
    try{$joined=$w.threadOriginal.Join(0)}catch{$w.joinException=$_;throw}
    if($joined){$w.terminalJoined=$true}else{
      if($w.joinFalseCount -eq [long]::MaxValue){$w.joinMultiplicityUnknown=$true}else{$w.joinFalseCount++}
      $null=[Threading.Thread]::Yield()
    }
  }
  $b.completionAttempted=$true
  foreach($failure in $b.exceptionsOriginal){if($null -ne $failure){$b.exceptionOriginal=$failure;throw $failure}}
  foreach($marker in 0..16){if($b.markersOriginal[$marker] -ne 1){throw 'Actual complete serializer/file/hash markers required.'}}
  if($b.countsOriginal[0] -le 0 -or $b.countsOriginal[0] -ge $b.capacityOriginal -or
      $b.countsOriginal[1] -ne $b.countsOriginal[0] -or $b.countsOriginal[2] -le 0 -or
      $b.countsOriginal[2] -gt $b.capacityOriginal+2 -or $b.digestOriginal[0].Length -ne 32){throw 'Actual derived-root held extent/EOF/digest incomplete.'}
  for($i=0;$i -lt $b.countsOriginal[2];$i++){
    if($b.readMarkersOriginal[2*$i] -ne 1 -or $b.readMarkersOriginal[2*$i+1] -ne 1){throw 'Actual physical read roster incomplete.'}
  }
  if($b.readLengthsOriginal[$b.countsOriginal[2]-1] -ne 0){throw 'Actual terminal EOF witness required.'}
  for($i=0;$i -lt $b.countsOriginal[0];$i++){
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline ended derived-root readback comparison.'}
    if($b.backingOriginal[$i] -ne $b.readbackOriginal[$i]){throw 'SAME held derived-root bytes differ from serialized source originals.'}
  }
  $b.heldSubjectOriginal=@{path=$b.pathOriginal;stream=$b.resourcesOriginal[2];bytesExpected=$b.countsOriginal[0];
    sha256=[BitConverter]::ToString($b.digestOriginal[0]).Replace('-','').ToLowerInvariant();
    handleOriginal=$b.resourcesOriginal[2].SafeFileHandle;bytesOriginal=$b.readbackOriginal;used=$b.countsOriginal[1];eof=$true;writerOriginal=$w}
  $b.memberOriginal=@{derivedRootOriginal=$b;subjectOriginal=$b.heldSubjectOriginal;
    bytesExpected=$b.countsOriginal[0];fileQueryOriginal=$null}
  $query=New-Cli39RootMemberFileQueryWorker32 $p $b.memberOriginal
  $null=Invoke-Cli39RootMemberFileQuery32 $query
  $null=Complete-Cli39RootMemberFileInformation32 $query
  if(-not $query.decodeOriginal.heldExtentEqual -or $query.decodeOriginal.directoryRawOriginal -ne 0 -or
      ($query.decodeOriginal.tagAttributesOriginal -band 0x400) -ne 0 -or $query.decodeOriginal.reparseTagOriginal -ne 0){
    throw 'Actual created held derived root is not same-extent ordinary nonreparse file.'
  }
  $b.nativeIdentityOriginal=$query.decodeOriginal
  $b.immutableCustodyOriginal=@{bookOriginal=$b;writeWorkerOriginal=$b.writeWorkerOriginal;
    subjectOriginal=$b.heldSubjectOriginal;nativeIdentityOriginal=$query.decodeOriginal;
    constructorShareOriginal=[IO.FileShare]::Read;constructorModeOriginal=[IO.FileMode]::CreateNew;
    parentNamespaceBookOriginal=$b.rootOriginal.namespaceBookOriginal;sourceWritesClosed=$true;
    nativeAuthorityOriginal=$null;state='ACTUAL_FRESH_HELD_SOURCE_WRITE_CLOSED_CUSTODY_NOT_NAMESPACE_AUTHORITY'}
  $b.returned=$true;return $b
}