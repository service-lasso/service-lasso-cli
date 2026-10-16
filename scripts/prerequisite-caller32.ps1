# Source-only fixed caller. External authentic startup/root/native admission is
# required before invocation; this module creates no process at import time.
# The actual retained caller entry owns this receiver through host disposition.
function Invoke-Cli39OwnedPrerequisiteHost32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)]$AbiOriginal,
    [Parameter(Mandatory)]$Receiver,
    [Parameter(Mandatory)]$AdmittedInputOriginal,
    [Parameter(Mandatory)]$ReservationOriginal,
    [Parameter(Mandatory)][scriptblock]$DeadlineResponse)
  $admitted=$AdmittedInputOriginal;$hostOriginal=$HostOwnerOriginal;$root=$hostOriginal.inputRead.value
  if (-not [object]::ReferenceEquals($hostOriginal.receiver,$Receiver) -or
      -not [object]::ReferenceEquals($Receiver.frames[$hostOriginal.key],$hostOriginal) -or
      -not [object]::ReferenceEquals($root.admittedInputOriginal,$admitted) -or
      -not [object]::ReferenceEquals($admitted.principalApiModuleOriginal,$root.principalSelectorOriginal) -or
      -not $root.principalSelectorOriginal.bindingReturned -or
      $null -eq $root.nativeAssociationOriginals -or
      -not [object]::ReferenceEquals($root.abiOriginal,$AbiOriginal) -or
      -not [object]::ReferenceEquals($hostOriginal.deadlineResponse,$DeadlineResponse) -or
      $hostOriginal.consumerOriginal.ContainsKey('disposalOriginal') -or
      ($null -ne $root.namespaceBookOriginal -and $null -ne $root.namespaceBookOriginal.disposalOriginal) -or
      $admitted.action -cnotin @('materialize','compare','keyring','key-policy',
      'checksum-signature','publisher') -or
      $admitted.role -cne 'retained-caller' -or
      -not [object]::ReferenceEquals($admitted.receiverOriginal,$Receiver) -or
      -not [object]::ReferenceEquals($admitted.callerReservationOriginal,$ReservationOriginal) -or
      $ReservationOriginal.inputRoot -cne $admitted.rootHash) {
    throw 'SAME admitted fixed CLI caller/root/reservation required.'
  }
  if($admitted.ContainsKey('callerInvocationOriginal') -or
      -not [object]::ReferenceEquals($admitted.parentReservationOriginal,$ReservationOriginal)){
    throw 'SAME fixed parent reservation and once-only caller invocation required.'
  }
  $invocation=@{hostOwnerOriginal=$hostOriginal;inputOriginal=$admitted;reservationOriginal=$ReservationOriginal;
    derivedRootOriginal=$null;childInvocationOriginal=$null;constructorTerminal=$false;exceptionOriginal=$null}
  $admitted.callerInvocationOriginal=$invocation
  try{
    $invocation.derivedRootOriginal=New-Cli39DerivedRoot32 $hostOriginal 'operation-host'
    $null=Invoke-Cli39DerivedRootWrite32 $invocation.derivedRootOriginal
    $invocation.childInvocationOriginal=Invoke-Cli39DerivedHost32 $invocation.derivedRootOriginal
    return $invocation.childInvocationOriginal.resultOriginal
  }catch{$invocation.exceptionOriginal=$_;throw}finally{$invocation.constructorTerminal=$true}
}