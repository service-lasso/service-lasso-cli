# Literal role-derived host issue, using the SAME shared native owner only.
function Invoke-Cli39DerivedHost32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$p=$b.hostOwnerOriginal;$root=$p.inputRead.value
  if(-not [object]::ReferenceEquals($root.derivedRootOriginal,$b) -or -not $b.returned -or
      $null -eq $b.nativeIdentityOriginal -or -not $b.nativeIdentityOriginal.returned -or
      $null -eq $b.immutableCustodyOriginal -or -not $b.immutableCustodyOriginal.sourceWritesClosed -or
      -not [object]::ReferenceEquals($b.immutableCustodyOriginal.subjectOriginal,$b.heldSubjectOriginal) -or
      -not [object]::ReferenceEquals($b.immutableCustodyOriginal.nativeIdentityOriginal,$b.nativeIdentityOriginal) -or
      $null -eq $root.nativeAssociationOriginals -or $null -eq $root.admittedInputOriginal -or
      -not [object]::ReferenceEquals($b.admittedParentOriginal,$root.admittedInputOriginal) -or
      $null -ne $b.childInvocationOriginal -or $null -ne $b.disposalOriginal -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal -or
      $b.heldSubjectOriginal.stream.SafeFileHandle.IsClosed){throw 'Actual once-only held/native derived root and admitted parent required.'}
  $invocation=@{bookOriginal=$b;rootOriginal=$root;hostOwnerOriginal=$p;commandOriginal=$null;
    consumerOriginal=$null;preparationOriginal=$null;argumentsOriginal=$null;issueArgumentsOriginal=$null;
    issueAttempted=$false;issueReturned=$false;resultOriginal=$null;constructorTerminal=$false;exceptionOriginal=$null}
  $b.childInvocationOriginal=$invocation
  try{
    $role=$b.childRoleOriginal;$prefix=if($role -ceq 'retained-caller'){'caller'}else{'host'}
    $controlName=$prefix+'Control';$control=$root.routingOriginal.controlsOriginal[$controlName]
    $environment=$root.routingOriginal.environmentsOriginal[$prefix+'Environment'].valuesOriginal
    $entry=Join-Path $PSScriptRoot 'prerequisite-host32.ps1';$file=$root.fieldMapOriginal['hostPath'].GetString()
    if($entry -cne $root.fieldMapOriginal['entryPath'].GetString() -or -not $control.returned -or
        -not $b.heldSubjectOriginal.eof -or $b.heldSubjectOriginal.used -ne $b.heldSubjectOriginal.bytesExpected){throw 'Exact source-owned child entry/control/held bytes required.'}
    $arguments=[string[]]@('-NoLogo','-NoProfile','-NonInteractive','-File',$entry,
      '-Role',$role,'-Action',$root.actionOriginal,
      '-InputRootPath',$b.pathOriginal,'-InputRootBytes',$b.heldSubjectOriginal.bytesExpected.ToString([Globalization.CultureInfo]::InvariantCulture),
      '-InputRootSha256',$b.heldSubjectOriginal.sha256,'-SharedSourceRoot',$root.fieldMapOriginal['sharedSourceRoot'].GetString(),
      '-PipeName',$control.pipeName,'-InvocationToken',$control.invocationToken,
      '-AbsoluteDeadlineTicks',$p.originalDeadlineTicks.ToString([Globalization.CultureInfo]::InvariantCulture))
    $invocation.argumentsOriginal=$arguments
    $command=@{file=$file;arguments=$arguments;environment=$environment;
      workingDirectory=$root.fieldMapOriginal[$prefix+'WorkingDirectory'].GetString();
      outputPrefix=$root.fieldMapOriginal[$prefix+'OutputPrefix'].GetString();home=$root.fieldMapOriginal[$prefix+'Home'].GetString();
      derivedRootOriginal=$b;nativeIdentityOriginal=$b.nativeIdentityOriginal;
      parentAdmissionOriginal=$root.admittedInputOriginal}
    $invocation.commandOriginal=$command
    $consumer=@{slot='cli39.source-input-host.v1';file=$file;arguments=$arguments;commandOriginal=$command;
      pipeName=$control.pipeName;invocationToken=$control.invocationToken;controlOriginal=$null;
      inputRoot=@{rootPath=$b.pathOriginal;rootBytes=$b.heldSubjectOriginal.bytesExpected;rootHash=$b.heldSubjectOriginal.sha256;
        role=$role;action=$root.actionOriginal;parentInputOriginal=$root.admittedInputOriginal;derivedRootOriginal=$b};
      deadlineResponse=$p.deadlineResponse}
    $invocation.consumerOriginal=$consumer
    $reservation=$root.admittedInputOriginal.parentReservationOriginal
    $preparation=New-OwnedPrerequisitePreparation -Receiver $p.receiver -Reservation $reservation -ConsumerOriginal $consumer -AbsoluteDeadlineTicks $p.originalDeadlineTicks -DeadlineResponse $p.deadlineResponse
    $invocation.preparationOriginal=$preparation
    $issue=@{Abi=$root.abiOriginal;Receiver=$p.receiver;File=$file;Arguments=$arguments;Environment=$environment;
      WorkingDirectory=$command.workingDirectory;OutputPrefix=$command.outputPrefix;AbsoluteDeadlineTicks=$p.originalDeadlineTicks;
      Reservation=$reservation;DeadlineResponse=$p.deadlineResponse;Preparation=$preparation}
    $invocation.issueArgumentsOriginal=$issue
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported -or $null -ne $b.disposalOriginal -or $b.heldSubjectOriginal.stream.SafeFileHandle.IsClosed){throw 'Original deadline/held-root future-start barrier denies child issue.'}
    $invocation.issueAttempted=$true
    $invocation.resultOriginal=Invoke-OwnedNativeProducerTool @issue
    $invocation.issueReturned=$true;return $invocation
  }catch{$invocation.exceptionOriginal=$_;throw}finally{$invocation.constructorTerminal=$true}
}