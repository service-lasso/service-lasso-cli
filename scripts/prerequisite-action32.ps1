# Fixed source action dispatcher. This consumes actual reader-created originals,
# never decoded candidate JSON as an executable/native effect description.
function Invoke-Cli39FixedPrerequisiteAction32 {
  param([Parameter(Mandatory)]$RootCaptureOriginal,
    [Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)][scriptblock]$DeadlineResponse)
  $root=$RootCaptureOriginal;$hostOriginal=$HostOwnerOriginal
  if(-not [object]::ReferenceEquals($hostOriginal.inputRead.value,$root) -or
      $null -eq $root.nativeAssociationOriginals -or
      -not [object]::ReferenceEquals($hostOriginal.deadlineResponse,$DeadlineResponse) -or
      $hostOriginal.consumerOriginal.ContainsKey('disposalOriginal') -or
      ($null -ne $root.namespaceBookOriginal -and $null -ne $root.namespaceBookOriginal.disposalOriginal) -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$hostOriginal.receiver)){
    throw 'Actual SAME complete native root association required before action enrollment.'
  }
  $slot=switch -CaseSensitive ($root.actionOriginal){
    'materialize' {'cli39.full-source-materialization.v2'}
    'compare' {'cli39.full-source-git-comparison.v2'}
    'keyring' {'cli39.node-keyring-preparation.v1'}
    'key-policy' {'cli39.node-key-policy.v1'}
    'checksum-signature' {'cli39.node-checksum-signature.v1'}
    'publisher' {'cli39.windows-publisher-trust.v1'}
    default {throw 'No generic CLI action exists.'}
  }
  $consumer=$root.admittedConsumerOriginal;$reservation=$root.consumerReservationOriginal
  if($root.actionOriginal -cin @('key-policy','checksum-signature') -and
      ($null -eq $root.nodePolicyOriginal -or -not $root.nodePolicyOriginal.returned -or
       -not [object]::ReferenceEquals($consumer.nodePolicyOriginal,$root.nodePolicyOriginal))){
    throw 'Actual complete inherited Node policy correspondence required before useful preparation.'
  }
  if($root.actionOriginal -ceq 'publisher' -and
      ($null -eq $root.windowsPolicyFoundationTransportOriginal -or
       -not $root.windowsPolicyFoundationTransportOriginal.returned -or
       -not $root.windowsPolicyFoundationTransportOriginal.terminal -or
       $null -ne $root.windowsPolicyFoundationTransportOriginal.exceptionOriginal -or
       -not [object]::ReferenceEquals($root.windowsPolicyFoundationTransportOriginal.rootOriginal,$root) -or
       -not [object]::ReferenceEquals($root.windowsPolicyFoundationTransportOriginal.policyOriginal,$root.publisherPolicyOriginal))){
    throw 'Actual complete SAME Windows publisher operand correspondence required before preparation.'
  }
  if($null -eq $consumer -or $consumer.slot -cne $slot -or
      -not [object]::ReferenceEquals($consumer.rootCaptureOriginal,$root) -or
      -not [object]::ReferenceEquals($reservation,$root.nativeAssociationOriginals.consumerReservationOriginal)){
    throw 'Source reader must supply SAME original fixed consumer/reservation, not a reconstructed receipt.'
  }
  if($root.ContainsKey('actionPreparationOriginal')){throw 'Fixed action enrollment is once-only, including failure.'}
  $enrollment=@{slot=$slot;consumerOriginal=$consumer;reservationOriginal=$reservation;
    preparationOriginal=$null;preparationResultOriginal=$null;
    commandOriginal=$null;issueArgumentsOriginal=$null;resultOriginal=$null;
    preparationAttempted=$false;issueAttempted=$false;returned=$false;exceptionOriginal=$null}
  $root.actionPreparationOriginal=$enrollment
  try{
    $enrollment.preparationAttempted=$true
    if($root.actionOriginal -ceq 'checksum-signature'){
      $enrollment.resultOriginal=Invoke-Cli39NodeChecksumSequence32 $hostOriginal $enrollment
      $enrollment.returned=$true;return $enrollment.resultOriginal
    }
    $preparation=New-OwnedPrerequisitePreparation -Receiver $hostOriginal.receiver -Reservation $reservation -ConsumerOriginal $consumer -AbsoluteDeadlineTicks $root.resultOriginal.absoluteDeadlineTicks -DeadlineResponse $DeadlineResponse
    $enrollment.preparationOriginal=$preparation
    if($slot -ceq 'cli39.windows-publisher-trust.v1'){
      # Dedicated fixed in-host sequence, never issued as a child. Complete
      # raw/physical closure remains distinct from useful trust acceptance.
      $enrollment.resultOriginal=Invoke-Cli39PublisherPrerequisite32 -OriginalPreparation $preparation -AbiOriginal $root.abiOriginal
      if(-not $enrollment.resultOriginal.usefulReturned){throw 'Actual full publisher trust/source acceptance remains pending; retained result is not useful completion.'}
    }else{
      $enrollment.preparationResultOriginal=Invoke-CliPrerequisitePreparation -Slot $slot -OriginalPreparation $preparation
      $command=$enrollment.preparationResultOriginal.commandOriginal
      $enrollment.commandOriginal=$command
      $issue=@{Abi=$root.abiOriginal;Receiver=$hostOriginal.receiver;
        File=$command.file;Arguments=$command.arguments;Environment=$command.environment;
        WorkingDirectory=$command.workingDirectory;OutputPrefix=$command.outputPrefix;
        AbsoluteDeadlineTicks=$root.resultOriginal.absoluteDeadlineTicks;Reservation=$reservation;
        DeadlineResponse=$DeadlineResponse;Preparation=$preparation}
      $enrollment.issueArgumentsOriginal=$issue;$enrollment.issueAttempted=$true
      # Shared owner independently invokes fixed completion/disposal/closure.
      $enrollment.resultOriginal=Invoke-OwnedNativeProducerTool @issue
      if($slot -ceq 'cli39.node-key-policy.v1'){
        $enrollment.nodeBirthOriginal=Join-Cli39NodeProducerBirth32 $preparation $enrollment.resultOriginal
        $enrollment.nodeUsefulOriginal=Complete-Cli39NodeUsefulResult32 $preparation $enrollment.resultOriginal
        if(-not $enrollment.nodeUsefulOriginal.usefulReturned){throw 'Full actual key-policy useful result required.'}
      }
    }
    $enrollment.returned=$true;return $enrollment.resultOriginal
  }catch{$enrollment.exceptionOriginal=$_;throw}
}
