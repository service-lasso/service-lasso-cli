# Literal checksum prerequisite sequence only; no generic action runner.
function Invoke-Cli39NodeChecksumSequence32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$EnrollmentOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$e=$EnrollmentOriginal
  $signature=$root.admittedConsumerOriginal;$key=$root.keyPolicyConsumerShapeOriginal.prototypeOriginal
  $reservations=$root.admittedInputOriginal.roleReservationsOriginal
  if($root.actionOriginal -cne 'checksum-signature' -or $root.ContainsKey('nodeSequenceOriginal') -or
      -not $root.keyPolicyConsumerShapeOriginal.returned -or -not $root.nodePolicyOriginal.returned -or
      $signature.slot -cne 'cli39.node-checksum-signature.v1' -or $key.slot -cne 'cli39.node-key-policy.v1' -or
      -not [object]::ReferenceEquals($key.rootCaptureOriginal,$root) -or
      [object]::ReferenceEquals($reservations['keyPolicyBudget'],$reservations['consumerBudget']) -or
      -not [object]::ReferenceEquals($reservations['keyPolicyBudget'],$root.nativeAssociationOriginals.keyPolicyReservationOriginal)){
    throw 'Actual fixed separately reserved checksum/key-policy originals required.'
  }
  $b=@{hostOwnerOriginal=$p;rootOriginal=$root;enrollmentOriginal=$e;policyOriginal=$root.nodePolicyOriginal;
    keyConsumerOriginal=$key;signatureConsumerOriginal=$signature;keyReservationOriginal=$reservations['keyPolicyBudget'];
    signatureReservationOriginal=$reservations['consumerBudget'];keyPreparationOriginal=$null;signaturePreparationOriginal=$null;
    keyNativeOwnerOriginal=$null;signatureNativeOwnerOriginal=$null;keyResultOriginal=$null;
    gpgHeldRowsOriginal=$null;gpgvHeldRowsOriginal=$null;keyringComparedBytes=0;firstFailureOriginal=$null;
    keyDisposalExceptionOriginal=$null;signatureDisposalExceptionOriginal=$null;returned=$false;terminal=$false}
  $root.nodeSequenceOriginal=$b
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies checksum sequence.'}
    $b.outputPathsOriginal=[Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
    $b.outputPathRosterOriginal=[string[]]::new(5)
    $b.outputPathRosterOriginal[0]=$key.outputPrefix+'.stdout.raw';$b.outputPathRosterOriginal[1]=$key.outputPrefix+'.stderr.raw'
    $b.outputPathRosterOriginal[2]=$signature.outputPrefix+'.stdout.raw';$b.outputPathRosterOriginal[3]=$signature.outputPrefix+'.stderr.raw'
    $b.outputPathRosterOriginal[4]=$signature.paths.payload
    foreach($path in $b.outputPathRosterOriginal){
      if(-not $b.outputPathsOriginal.Add($path)){throw 'Distinct actual key-policy/signature output originals required.'}
      foreach($consumer in @($key,$signature)){foreach($pin in $consumer.subjectPins.Values){
        if([StringComparer]::OrdinalIgnoreCase.Equals($path,$pin.path)){throw 'Output aliases an actual held verification input.'}
      }}
    }
    foreach($role in @('keyring','signature','listing')){
      $left=$key.subjectPins[$role];$right=$signature.subjectPins[$role]
      if($left.path -cne $right.path -or $left.bytes -ne $right.bytes -or $left.sha256 -ine $right.sha256 -or
          $key.paths[$role] -cne $signature.paths[$role]){throw 'Exact SAME selected keyring/signature/listing pins required.'}
    }
    $b.gpgHeldRowsOriginal=[Collections.Generic.List[object]]::new()
    $b.gpgvHeldRowsOriginal=[Collections.Generic.List[object]]::new()
    $gpg=$key.subjectPins['executable']
    $gpgv=$signature.subjectPins['executable'];$gpgStartup=$false;$gpgvStartup=$false
    foreach($member in $root.heldMemberBookOriginal.recordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended gpg held-member join.'}
      if($member.subjectOriginal.path -ceq $gpg.path){
        if(-not $member.returned -or $member.bytesExpected -ne $gpg.bytes -or $member.sha256Expected -ine $gpg.sha256 -or
            -not $member.subjectOriginal.eof -or -not $member.subjectOriginal.hashReturned -or
            -not $member.fileQueryOriginal.decodeOriginal.returned){throw 'Actual independently held gpg tool differs.'}
        $b.gpgHeldRowsOriginal.Add($member)
        if($member.domainOriginal -ceq 'startupClosure'){$gpgStartup=$true}
      }
      if($member.subjectOriginal.path -ceq $gpgv.path){
        if(-not $member.returned -or $member.bytesExpected -ne $gpgv.bytes -or $member.sha256Expected -ine $gpgv.sha256 -or
            -not $member.subjectOriginal.eof -or -not $member.subjectOriginal.hashReturned -or
            -not $member.fileQueryOriginal.decodeOriginal.returned){throw 'Actual independently held gpgv tool differs.'}
        $b.gpgvHeldRowsOriginal.Add($member)
        if($member.domainOriginal -ceq 'startupClosure'){$gpgvStartup=$true}
      }
    }
    if(-not $gpgStartup -or -not $gpgvStartup -or $gpg.path -ceq $gpgv.path){
      throw 'Separate actual held startup gpg/gpgv tools required; a source-only alias cannot supply tool closure.'
    }
    foreach($field in @('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms','timePolicy')){
      $key[$field]=$signature[$field]
    }
    $key.nodePolicyOriginal=$b.policyOriginal
    # Both original preparations/reservations exist before either tool starts.
    # Their full retained overlap is an externally qualified allocation input.
    $b.keyPreparationOriginal=New-OwnedPrerequisitePreparation $p.receiver $b.keyReservationOriginal $key $p.originalDeadlineTicks $p.deadlineResponse
    $b.signaturePreparationOriginal=New-OwnedPrerequisitePreparation $p.receiver $b.signatureReservationOriginal $signature $p.originalDeadlineTicks $p.deadlineResponse
    $e.preparationOriginal=$b.signaturePreparationOriginal
    $prepared=Invoke-CliPrerequisitePreparation $key.slot $b.keyPreparationOriginal
    $command=$prepared.commandOriginal;$b.keyCommandOriginal=$command;$b.keyIssueAttempted=$true
    $b.keyNativeOwnerOriginal=Invoke-OwnedNativeProducerTool -Abi $root.abiOriginal -Receiver $p.receiver -File $command.file -Arguments $command.arguments -Environment $command.environment -WorkingDirectory $command.workingDirectory -OutputPrefix $command.outputPrefix -AbsoluteDeadlineTicks $p.originalDeadlineTicks -Reservation $b.keyReservationOriginal -DeadlineResponse $p.deadlineResponse -Preparation $b.keyPreparationOriginal
    $result=$b.keyNativeOwnerOriginal.consumerCompletionResult
    if(-not [object]::ReferenceEquals($result,$key.keyParseOriginal) -or -not $result.availableKeyChecks -or
        $null -ne $result.exception -or -not $b.keyNativeOwnerOriginal.released -or
        -not [object]::ReferenceEquals($b.keyNativeOwnerOriginal.originalPreparation,$b.keyPreparationOriginal) -or
        -not [object]::ReferenceEquals($b.keyNativeOwnerOriginal.consumerOriginal,$key)){
      throw 'Actual complete key-policy owner/result correspondence required.'
    }
    $b.keyBirthOriginal=Join-Cli39NodeProducerBirth32 $b.keyPreparationOriginal $b.keyNativeOwnerOriginal
    $b.keyUsefulOriginal=Complete-Cli39NodeUsefulResult32 $b.keyPreparationOriginal $b.keyNativeOwnerOriginal
    if(-not $b.keyUsefulOriginal.usefulReturned){throw 'Full actual key-policy useful result required before signature issue.'}
    $b.keyResultOriginal=@{sequenceOriginal=$b;nativeOwnerOriginal=$b.keyNativeOwnerOriginal;
      producerBirthOriginal=$b.keyBirthOriginal;
      usefulOriginal=$b.keyUsefulOriginal;
      preparationOriginal=$b.keyPreparationOriginal;consumerOriginal=$key;parseOriginal=$result;
      rootOriginal=$root;policyOriginal=$b.policyOriginal;keyringSubjectOriginal=$key.subjectOriginals['keyring']}
    $signature.keyPolicyResultOriginal=$b.keyResultOriginal
    $prepared=Invoke-CliPrerequisitePreparation $signature.slot $b.signaturePreparationOriginal
    $e.preparationResultOriginal=$prepared;$left=$key.subjectOriginals['keyring'];$right=$signature.subjectOriginals['keyring']
    if(-not $left.eof -or -not $right.eof -or $left.used -ne $right.used -or $left.path -cne $right.path -or
        $left.digest -cne $right.digest){throw 'Actual retained gpg/gpgv keyring bytes differ.'}
    for($index=0;$index -lt $left.used;$index++){
      Assert-Cli39PrerequisiteUseful26 $b.signaturePreparationOriginal
      if($left.bytes[$index] -ne $right.bytes[$index]){throw 'Actual retained keyring byte differs.'}
      $b.keyringComparedBytes=$index+1
    }
    Assert-Cli39PrerequisiteUseful26 $b.signaturePreparationOriginal
    $command=$prepared.commandOriginal;$e.commandOriginal=$command;$e.issueAttempted=$true
    $b.signatureNativeOwnerOriginal=Invoke-OwnedNativeProducerTool -Abi $root.abiOriginal -Receiver $p.receiver -File $command.file -Arguments $command.arguments -Environment $command.environment -WorkingDirectory $command.workingDirectory -OutputPrefix $command.outputPrefix -AbsoluteDeadlineTicks $p.originalDeadlineTicks -Reservation $b.signatureReservationOriginal -DeadlineResponse $p.deadlineResponse -Preparation $b.signaturePreparationOriginal
    $b.signatureBirthOriginal=Join-Cli39NodeProducerBirth32 $b.signaturePreparationOriginal $b.signatureNativeOwnerOriginal
    $b.signatureUsefulOriginal=Complete-Cli39NodeUsefulResult32 $b.signaturePreparationOriginal $b.signatureNativeOwnerOriginal
    if(-not $b.signatureUsefulOriginal.usefulReturned){throw 'Full actual signature useful result required.'}
    $b.returned=$true;return $b.signatureNativeOwnerOriginal
  }catch{
    $b.firstFailureOriginal=$_
    if($null -eq $b.keyNativeOwnerOriginal -and $null -ne $b.keyPreparationOriginal){$b.keyNativeOwnerOriginal=$b.keyPreparationOriginal.nativeState}
    if($null -eq $b.signatureNativeOwnerOriginal -and $null -ne $b.signaturePreparationOriginal){$b.signatureNativeOwnerOriginal=$b.signaturePreparationOriginal.nativeState}
    throw
  }finally{
    # Shared producer already visits disposal after native issue. Only visit a
    # never-issued preparation whose fixed disposal has not started; no retry.
    foreach($role in @('key','signature')){
      $preparation=if($role -ceq 'key'){$b.keyPreparationOriginal}else{$b.signaturePreparationOriginal}
      if($null -ne $preparation -and -not $preparation.nativeIssued -and
          $preparation.consumerOriginal.ContainsKey('originalPreparation') -and
          -not $preparation.consumerOriginal.ContainsKey('disposalOriginal')){
        try{$null=Invoke-CliPrerequisiteDisposal26 $preparation.consumerOriginal.slot $preparation}
        catch{$b[($role+'DisposalExceptionOriginal')]=$_}
      }
    }
    $b.terminal=$true
  }
}

function Join-Cli39SignatureCheckedKey32 {
  param([Parameter(Mandatory)]$OwnerOriginal,[Parameter(Mandatory)]$SignatureParseOriginal)
  $owner=$OwnerOriginal;$c=$owner.consumerOriginal;$relation=$c.keyPolicyResultOriginal;$parse=$SignatureParseOriginal
  if($null -eq $relation -or -not [object]::ReferenceEquals($relation.rootOriginal,$c.rootCaptureOriginal) -or
      -not [object]::ReferenceEquals($relation.policyOriginal,$c.nodePolicyOriginal) -or
      -not [object]::ReferenceEquals($relation.parseOriginal,$relation.consumerOriginal.keyParseOriginal) -or
      -not [object]::ReferenceEquals($relation.nativeOwnerOriginal.consumerCompletionResult,$relation.parseOriginal) -or
      $relation.preparationOriginal.originalDeadlineTicks -ne $c.originalPreparation.originalDeadlineTicks -or
      -not [object]::ReferenceEquals($relation.preparationOriginal.receiver,$c.originalPreparation.receiver) -or
      -not $relation.parseOriginal.availableKeyChecks -or $null -ne $relation.parseOriginal.exception -or
      -not $relation.usefulOriginal.usefulReturned -or
      -not [object]::ReferenceEquals($relation.usefulOriginal,$relation.consumerOriginal.nodeUsefulResultOriginal) -or
      $relation.sequenceOriginal.keyringComparedBytes -ne $c.subjectOriginals['keyring'].used){
    throw 'Actual SAME checked key-policy result/keyring/policy/deadline relation required.'
  }
  $join=@{relationOriginal=$relation;signatureParseOriginal=$parse;keyOriginal=$null;returned=$false}
  $parse.checkedKeyAssociationOriginal=$join
  foreach($key in $relation.parseOriginal.keys){
    Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
    if($key.fingerprint -ceq $parse.valid[1]){
      if($null -ne $join.keyOriginal -or $key.fields[3] -cne $parse.valid[7] -or -not $key.fields[11].Contains('s')){
        throw 'Actual signature signer differs from unique checked local signing key/algorithm.'
      }
      $join.keyOriginal=$key
    }
  }
  if($null -eq $join.keyOriginal){throw 'Actual signature signer absent from complete checked key graph.'}
  Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
  $join.returned=$true;return $join
}
