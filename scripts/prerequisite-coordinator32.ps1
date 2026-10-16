# Fixed outer qualification entry. Invoke only under the separately admitted
# official host/exact source+input+command+whole startup profile. Import is pure.
function New-Cli39QualificationCoordinator32 {
  param([Parameter(Mandatory)]$StartupOriginal,
    [Parameter(Mandatory)][string]$InputRootPath,
    [Parameter(Mandatory)][long]$InputRootBytes,[Parameter(Mandatory)][string]$InputRootSha256,
    [Parameter(Mandatory)][string]$SharedSourceRoot,[Parameter(Mandatory)][long]$AbsoluteDeadlineTicks,
    [Parameter(Mandatory)][ValidateSet('materialize','compare','keyring','key-policy','checksum-signature','publisher')][string]$Action)
  if($StartupOriginal.GetType() -ne [Cli39QualificationStartup32]){throw 'Actual fixed same-process startup carrier required.'}
  $s=$StartupOriginal;$p=$s.HostOwnerOriginal;$receiver=$p.receiver
  Assert-Cli39QualificationStartupCustody32 -StartupOriginal $s
  if($null -ne $script:Cli39QualificationCoordinatorOriginal -or $null -ne $p.coordinatorOriginal -or
      -not $s.ConstructorTerminal -or $null -ne $s.ConstructorFailure -or
      $null -eq $s.CallerOriginal -or -not $s.CallerOriginal.attempted -or $s.CallerOriginal.terminal -or
      -not [object]::ReferenceEquals($s.CallerOriginal.startupOriginal,$s) -or
      -not [object]::ReferenceEquals($s.CallerOriginal.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($s.CallerOriginal.reservationOriginal,$s.StartupReservationOriginal) -or
      -not [object]::ReferenceEquals($p.qualificationStartupOriginal,$s) -or
      -not [object]::ReferenceEquals($receiver,$s.ReceiverOriginal) -or $p.key -cne $s.HostKeyOriginal -or
      -not [object]::ReferenceEquals($receiver.frames[$p.key],$p) -or
      $p.originalDeadlineTicks -ne $AbsoluteDeadlineTicks -or $AbsoluteDeadlineTicks -ne $s.DeadlineTicksOriginal -or
      -not [object]::ReferenceEquals($p.deadlineResponse,$s.DeadlineResponseOriginal) -or
      $InputRootPath -cne $s.InputRootPathOriginal -or $InputRootBytes -ne $s.InputRootBytesOriginal -or
      $InputRootSha256 -cne $s.InputRootSha256Original -or $SharedSourceRoot -cne $s.SharedSourceRootOriginal -or
      $Action -cne $s.ActionOriginal -or $null -ne $p.consumerOriginal -or $null -ne $p.inputRead.value){
    throw 'SAME dedicated containing host/startup/invocation originals required; existing roots cannot be replaced.'
  }
  $script:Cli39QualificationCoordinatorOriginal=$p
  $coordinator=@{hostOwnerOriginal=$p;actionOriginal=$Action;sourceRootOriginal=$SharedSourceRoot;
    startupOriginal=$s;startupReservationOriginal=$s.StartupReservationOriginal;
    sourceLoadsOriginal=$null;
    constructorTerminal=$false;constructorFailure=$null;prefixReturned=$false;invocationOriginal=$null;dispositionOriginal=$null;
    externalStartupPremise='SAME_OFFICIAL_HOST_EXACT_COMMAND_ADMISSION_PRECEDES_FIRST_STATEMENT_NOT_ROOT_AUTHORITY'}
  $p.coordinatorOriginal=$coordinator
  try{
    # Every import is a source-fixed selected member, never an input action or
    # callback. All failed/partial loads remain on the initial outer owner.
    $names=[string[]]@('prerequisite-managed-worker26.ps1','prerequisite-members32.ps1',
      'prerequisite-native-query32.ps1','prerequisite-output-query32.ps1','prerequisite-file-query32.ps1',
      'prerequisite-member-file-query32.ps1','prerequisite-namespace-recipe32.ps1','prerequisite-namespace-worker32.ps1',
      'prerequisite-namespace32.ps1','prerequisite-process32.ps1','prerequisite-image32.ps1','prerequisite-modules32.ps1','prerequisite-loaded-recipes32.ps1','prerequisite-loaded-worker32.ps1','prerequisite-loaded32.ps1',
      'prerequisite-profile32.ps1','prerequisite-loader-semantics32.ps1','prerequisite-child-bindings32.ps1','prerequisite-foundation32.ps1','prerequisite-node-policy32.ps1','prerequisite-native-root32.ps1','prerequisite-input32.ps1','publisher-policy32.ps1','publisher-basis32.ps1','publisher-foundation32.ps1',
      'node-release-verification-consumer26.ps1','prerequisite-consumer-structure32.ps1','prerequisite-namespace-roles32.ps1',
      'prerequisite-pipe-peer32.ps1','prerequisite-routing32.ps1','prerequisite-lineage32.ps1',
      'prerequisite-principal-selector32.ps1','prerequisite-principal-module32.ps1','prerequisite-principal-token32.ps1','prerequisite-parent-recipes32.ps1','prerequisite-parent-observation32.ps1',
      'prerequisite-owned-birth-worker32.ps1','prerequisite-owned-birth32.ps1','prerequisite-derived-root-worker32.ps1',
      'prerequisite-derived-root-disposal32.ps1','prerequisite-node-projection32.ps1','publisher-projection32.ps1','prerequisite-node-sequence32.ps1','prerequisite-static-projection32.ps1','prerequisite-derived-root32.ps1','prerequisite-derived-host32.ps1','prerequisite-caller32.ps1','prerequisite-action32.ps1')
    $coordinator.sourceLoadsOriginal=[Collections.Generic.List[object]]::new($names.Length+1)
    foreach($name in $names){$coordinator.sourceLoadsOriginal.Add(@{nameOriginal=$name;pathOriginal=(Join-Path $PSScriptRoot $name);attempted=$false;returned=$false;exceptionOriginal=$null})}
    $coordinator.sourceLoadsOriginal.Add(@{nameOriginal='windows-native-producer-owner.ps1';pathOriginal=(Join-Path $SharedSourceRoot 'windows-native-producer-owner.ps1');attempted=$false;returned=$false;exceptionOriginal=$null})
    foreach($load in $coordinator.sourceLoadsOriginal){
      $load.attempted=$true
      try{. $load.pathOriginal;$load.returned=$true}catch{$load.exceptionOriginal=$_;throw}
    }
    # These are the containing host's actual preowned startup recipes. Do not
    # reconstruct them or replace its deadline response in the coordinator.
    if(-not $p.startupRecipes.returned -or -not $p.startupRecipes.terminal -or
        $null -eq $p.abiRecipes -or $null -eq $p.sinkRecipe -or
        -not $p.namespaceRecipesOriginal.returned -or -not $p.namespaceRecipesOriginal.constructionTerminal -or
        -not $p.principalRecipesOriginal.returned -or -not $p.principalRecipesOriginal.constructionTerminal -or
        -not $p.parentRecipesOriginal.returned -or -not $p.parentRecipesOriginal.constructionTerminal -or
        -not $p.loadedRecipesOriginal.returned -or -not $p.loadedRecipesOriginal.constructionTerminal){
      throw 'Actual containing host startup recipes remain absent/incomplete.'
    }
    foreach($recipe in $p.abiRecipes.Values){if(-not [object]::ReferenceEquals($recipe.hostOwnerOriginal,$p) -or
        -not $recipe.constructionTerminal -or -not $recipe.returned){throw 'SAME completed containing ABI recipe originals required.'}}
    foreach($recipe in @($p.sinkRecipe,$p.namespaceRecipesOriginal,$p.principalRecipesOriginal,$p.parentRecipesOriginal,$p.loadedRecipesOriginal)){
      if(-not [object]::ReferenceEquals($recipe.hostOwnerOriginal,$p)){throw 'Containing recipe host original differs.'}
    }
    $p.inputRead.attempted=$true
    try{
      $root=Read-Cli39PrerequisiteRootStructure32 $p $InputRootPath $InputRootBytes $InputRootSha256 $Action 'qualification-coordinator'
      $p.inputRead.returned=$true
    }catch{$p.inputRead.original=$_;throw}
    $null=Read-Cli39ConsumerStructure32 $root $p
    if($Action -ceq 'checksum-signature'){$null=Read-Cli39ConsumerStructure32 $root $p -KeyPolicyPrerequisite}
    $null=Read-Cli39PrincipalSelector32 $p
    $null=Read-Cli39RootRouting32 $p 'qualification-coordinator' '' '' $SharedSourceRoot
    $null=Read-Cli39NamespaceRoles32 $p
    if($Action -ceq 'publisher'){
      $null=Read-Cli39PublisherPolicyOriginal32 $root $p
      $null=Read-Cli39PublisherFoundationSelector32 $p
    }
    $null=Observe-Cli39PrerequisiteNativeRoot32 $p
    $null=New-Cli39FoundationCorrespondence32 $p
    if($Action -cin @('key-policy','checksum-signature')){
      $null=Join-Cli39NodeVerificationPolicy32 $p $s.PolicyFoundationOriginal $s.StartupLifetimeOriginal
      $null=Join-Cli39OuterNodeLoader32 $p $root.nodePolicyOriginal $root.nodePolicyOriginal.toolLoaderFoundationsOriginal
    }
    if($Action -ceq 'publisher'){$null=Join-Cli39PublisherWindowsFoundation32 $p}
    $null=Complete-Cli39OuterFoundationBindings32 $p
    $coordinator.prefixReturned=$true
    $coordinator.constructorTerminal=$true
    $null=Invoke-Cli39QualificationCoordinator32 $p
    return $p
  }catch{
    $coordinator.constructorFailure=$_
    $null=Invoke-Cli39QualificationDisposition32 $p
    throw
  }finally{$coordinator.constructorTerminal=$true}
}

function Invoke-Cli39QualificationDisposition32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$coordinator=$p.coordinatorOriginal
  if($null -ne $coordinator.dispositionOriginal){return}
  $disposition=@{derivedAttempted=$false;derivedExceptionOriginal=$null;
    rootAttempted=$false;rootExceptionOriginal=$null;importsIncomplete=$false;exceptionOriginal=$null;terminal=$false}
  $coordinator.dispositionOriginal=$disposition
  try{
    # Root work starts only after the complete fixed import roster returned.
    # An unavailable cleanup body is retained, never replaced by guessed code.
    if($null -eq $coordinator.sourceLoadsOriginal){$disposition.importsIncomplete=$true;return}
    foreach($load in $coordinator.sourceLoadsOriginal){
      if(-not $load.returned){$disposition.importsIncomplete=$true;return}
    }
    $root=$p.inputRead.value
    if($null -ne $root -and $null -ne $root.derivedRootOriginal -and
        $null -eq $root.derivedRootOriginal.disposalOriginal){
      $disposition.derivedAttempted=$true
      try{$null=Invoke-Cli39DerivedRootDisposal32 $root.derivedRootOriginal}
      catch{$disposition.derivedExceptionOriginal=$_}
    }
    if($null -ne $p.consumerOriginal -and -not $p.consumerOriginal.ContainsKey('disposalOriginal')){
      $disposition.rootAttempted=$true
      try{$p.rootDisposalOriginal=Invoke-Cli39RootDisposal32 $p}
      catch{$disposition.rootExceptionOriginal=$_}
    }
  }catch{$disposition.exceptionOriginal=$_}finally{$disposition.terminal=$true}
}

function Invoke-Cli39QualificationCoordinator32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$coordinator=$p.coordinatorOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not [object]::ReferenceEquals($coordinator.hostOwnerOriginal,$p) -or
      -not $coordinator.constructorTerminal -or $null -ne $coordinator.constructorFailure -or
      -not $coordinator.prefixReturned -or $null -ne $coordinator.invocationOriginal -or
      $root.rootRoleOriginal -cne 'qualification-coordinator' -or $null -eq $root.nativeAssociationOriginals -or
      $null -eq $root.admittedInputOriginal){throw 'Actual whole outer qualification association constructor remains required before caller birth.'}
  $invocation=@{hostOwnerOriginal=$p;derivedRootOriginal=$null;childOriginal=$null;
    returned=$false;exceptionOriginal=$null;constructorTerminal=$false}
  $coordinator.invocationOriginal=$invocation
  try{
    $invocation.derivedRootOriginal=New-Cli39DerivedRoot32 $p 'retained-caller'
    $null=Invoke-Cli39DerivedRootWrite32 $invocation.derivedRootOriginal
    $invocation.childOriginal=Invoke-Cli39DerivedHost32 $invocation.derivedRootOriginal
    $null=Invoke-Cli39QualificationDisposition32 $p
    if(-not (Test-Cli39RootClosure32 $p) -or -not (Test-NativeProducerReceiverPhysicalClosure -Receiver $p.receiver -HostOwner $p)){throw 'Actual outer root/receiver closure incomplete.'}
    $invocation.returned=$true;return $invocation
  }catch{$invocation.exceptionOriginal=$_;throw}finally{
    $null=Invoke-Cli39QualificationDisposition32 $p
    $invocation.constructorTerminal=$true
  }
}
