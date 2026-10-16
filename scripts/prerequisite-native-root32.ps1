# Fixed native observation prefix for the actual root reader. External startup
# and entire source/input/exact-command admission precede invocation. Decoded
# paths and raw observations do not supply their own authority or reservation.
function Observe-Cli39PrerequisiteNativeRoot32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$capture=$p.inputRead.value
  if($null -eq $capture -or -not $capture.structuralReturned -or
      $capture.ContainsKey('nativeObservationOriginal') -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($capture.resultOriginal.receiverOriginal,$p.receiver)){
    throw 'SAME once-only structural root original required before native observation.'
  }
  $o=@{hostOwnerOriginal=$p;rootCaptureOriginal=$capture;abiAttempted=$false;
    abiOriginal=$null;queryConstructorOriginal=$null;queryBindingOriginal=$null;heldMemberBookOriginal=$null;
    memberQueryCurrentOriginal=$null;memberQueriesCompleted=0;memberQueriesReturned=$false;
    rootFileWorkerOriginal=$null;rootFileResultOriginal=$null;pipePeerWorkerOriginal=$null;pipePeerResultOriginal=$null;processWorkerOriginal=$null;processResultOriginal=$null;imageWorkerOriginal=$null;imageResultOriginal=$null;moduleBookOriginal=$null;namespaceBookOriginal=$null;principalSelectorOriginal=$null;principalModuleOriginal=$null;
    returned=$false;exceptionOriginal=$null;nativeAuthenticated=$false;
    startupAssociationOriginal=$null;sourceAssociationOriginal=$null;
    namespaceAssociationOriginal=$null;loadedAssociationOriginal=$null;
    principalAssociationOriginal=$null;capabilityAssociationOriginal=$null;principalTokenBookOriginal=$null;
    allocationAssociationOriginal=$null;state='NATIVE_OBSERVATIONS_NOT_ADMISSION'}
  $capture.nativeObservationOriginal=$o
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids native root observation.'}
    # SAME shared fixed ABI constructor enrolls its original resident module
    # and each supervised native phase in this receiver before native effects.
    # It keeps the original host deadline response available while pending.
    $o.heldMemberBookOriginal=Read-Cli39HeldMembers32 $p
    $o.abiAttempted=$true
    $a=New-NativeProducerAbi -Kernel32Path $capture.fieldMapOriginal['kernel32Path'].GetString() -Receiver $p.receiver -HostOwnerOriginal $p
    $o.abiOriginal=$a;$capture.abiOriginal=$a
    $w=New-Cli39NativeQueryWorker32 -HostOwnerOriginal $p -AbiOriginal $a
    $o.queryConstructorOriginal=$w
    $o.queryBindingOriginal=Invoke-Cli39NativeQueryBinding32 $w
    if($capture.actionOriginal -ceq 'materialize'){
      $w=New-Cli39NativeOutputQueryWorker32 $p $a
      $null=Invoke-Cli39NativeOutputQueryBinding32 $w
    }
    if($capture.rootRoleOriginal -cne 'qualification-coordinator'){
      $w=New-Cli39RootPipePeerWorker32 $p;$o.pipePeerWorkerOriginal=$w
      $o.pipePeerResultOriginal=Invoke-Cli39RootPipePeer32 $w
    }else{
      $o.pipePeerDispositionOriginal='OUTER_HAS_NO_CLI_CONTROL_INGRESS_NOT_A_PEER_ASSOCIATION'
    }
    $w=New-Cli39RootFileQueryWorker32 $p;$o.rootFileWorkerOriginal=$w
    $o.rootFileResultOriginal=Invoke-Cli39RootFileQuery32 $w
    foreach($member in $o.heldMemberBookOriginal.recordsOriginal){
      $o.memberQueryCurrentOriginal=$member
      $w=New-Cli39RootMemberFileQueryWorker32 $p $member
      $null=Invoke-Cli39RootMemberFileQuery32 $w
      $null=Complete-Cli39RootMemberFileInformation32 $w
      $o.memberQueriesCompleted++
    }
    $o.memberQueriesReturned=$true
    $o.namespaceBookOriginal=Observe-Cli39RootNamespace32 $p
    if($capture.actionOriginal -ceq 'publisher'){$null=Join-Cli39PublisherFoundationMembers32 $p}
    $o.principalSelectorOriginal=Join-Cli39PrincipalHeldMember32 $p
    $o.principalModuleOriginal=New-Cli39PrincipalModule32 $p
    foreach($phase in $o.principalModuleOriginal.workersOriginal){$null=Invoke-Cli39PrincipalModulePhase32 $phase}
    $o.principalTokenBookOriginal=Observe-Cli39PrincipalTokens32 $p
    $w=New-Cli39RootProcessWorker32 $p;$o.processWorkerOriginal=$w
    $o.processResultOriginal=Invoke-Cli39RootProcess32 $w
    $w=New-Cli39RootImageWorker32 $p;$o.imageWorkerOriginal=$w
    $o.imageResultOriginal=Invoke-Cli39RootImage32 $w
    $o.moduleBookOriginal=Observe-Cli39RootModules32 $p
    $o.loadedObservationOriginal=Observe-Cli39Loaded32 $p
    if($capture.rootRoleOriginal -cne 'qualification-coordinator'){
      $o.parentObservationOriginal=Observe-Cli39Parent32 $p
    }
    $o.returned=$true
    # No admitted consumer/reservation or capability is constructed from these
    # observations. Actual whole source/startup/loaded/principal/namespace/
    # capability/profile joins must create their own SAME association originals.
    # Until then the host retains nativeAssociationOriginals=null and refuses
    # all six useful actions, including publisher verification.
    return $o
  }catch{$o.exceptionOriginal=$_;throw}
}
