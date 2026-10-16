# Fixed local correspondence under genuine whole-input qualification of the
# actual controlling parent's foundation/reservation/command/root transport.
# These records create neither capacity nor authority from decoded JSON.
function Join-Cli39ChildCapability32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$s=$root.childStaticCorrespondenceOriginal;$t=$root.childDerivedTransportOriginal
  if($null -eq $s -or -not $s.returned -or $null -ne $s.capabilityAssociationOriginal -or
      -not $t.observationsReturned -or -not [object]::ReferenceEquals($t.staticOriginal,$s) -or
      -not $root.consumerShapeOriginal.returned -or -not $root.routingOriginal.returned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME live once-only child capability correspondence required.'}
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;parentTransportOriginal=$t;
    qualificationChainOriginal=$s.qualificationRootChainOriginal;requestOriginal=$t.requestOriginal;
    routingOriginal=$root.routingOriginal;consumerShapeOriginal=$root.consumerShapeOriginal;
    consumerElementOriginal=$null;roleOriginal=$root.rootRoleOriginal;actionOriginal=$root.actionOriginal;
    returned=$false;exceptionOriginal=$null;terminal=$false}
  $s.capabilityAssociationOriginal=$a
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child capability join.'}
    $expected=switch -CaseSensitive ($root.actionOriginal){
      'materialize'{'cli39.full-source-materialization.v2'}'compare'{'cli39.full-source-git-comparison.v2'}
      'keyring'{'cli39.node-keyring-preparation.v1'}'key-policy'{'cli39.node-key-policy.v1'}
      'checksum-signature'{'cli39.node-checksum-signature.v1'}'publisher'{'cli39.windows-publisher-trust.v1'}
      default{throw 'No generic child capability exists.'}
    }
    $prototype=$root.consumerShapeOriginal.prototypeOriginal;$request=$a.requestOriginal
    if($root.rootRoleOriginal -cnotin @('retained-caller','operation-host') -or
        $prototype.slot -cne $expected -or -not [object]::ReferenceEquals($prototype.rootCaptureOriginal,$root) -or
        $request.roleOriginal -cne $root.rootRoleOriginal -or $request.actionOriginal -cne $root.actionOriginal -or
        $request.inputRootPathOriginal -cne $root.path -or $request.inputRootBytesOriginal -ne $root.bytesExpected -or
        $request.inputRootSha256Original -cne $root.sha256Expected -or $request.deadlineTicks -ne $p.originalDeadlineTicks -or
        $a.qualificationChainOriginal.qualificationRootSha256 -cne $s.projectionOriginal.scalarValuesOriginal[1]){
      throw 'Actual fixed child command/consumer/qualification lineage differs.'
    }
    $a.consumerElementOriginal=$root.fieldMapOriginal['consumer']
    if($root.actionOriginal -ceq 'checksum-signature'){
      if(-not $root.keyPolicyConsumerShapeOriginal.returned -or
          $root.keyPolicyConsumerShapeOriginal.prototypeOriginal.slot -cne 'cli39.node-key-policy.v1'){
        throw 'Actual fixed auxiliary key-policy capability shape required.'
      }
      $a.keyPolicyConsumerElementOriginal=$root.fieldMapOriginal['keyPolicyConsumer']
      $a.keyPolicyConsumerShapeOriginal=$root.keyPolicyConsumerShapeOriginal
    }
    # Parent-owned derived writer/command and pre-NPH birth book retain the
    # actual qualified ingress/source disposition. This local counterpart has
    # only exact child observations; it never claims cross-process CLR identity.
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child capability completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}

function Join-Cli39ChildAllocation32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$s=$root.childStaticCorrespondenceOriginal;$t=$root.childDerivedTransportOriginal
  if(-not $s.capabilityAssociationOriginal.returned -or $null -ne $s.allocationAssociationOriginal -or
      -not $t.observationsReturned -or -not $s.loadedAssociationOriginal.returned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME completed child capability/loaded and once-only allocation correspondence required.'}
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;parentTransportOriginal=$t;
    qualificationChainOriginal=$s.qualificationRootChainOriginal;capabilityOriginal=$s.capabilityAssociationOriginal;
    loadedOriginal=$root.loadedObservationOriginal;rowsOriginal=$null;reservationsOriginal=$null;
    sourceDispositionOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false;
    physicalCapacityPremise='GENUINE_PARENT_RETAINED_RESERVATIONS_MEASUREMENTS_AND_DERIVED_ROOT_PROFILE_COPY_QUALIFIED_EXTERNALLY'}
  $s.allocationAssociationOriginal=$a
  try{
    $roles=if($root.actionOriginal -ceq 'checksum-signature'){@('callerBudget','hostBudget','consumerBudget','keyPolicyBudget')}else{@('callerBudget','hostBudget','consumerBudget')}
    if($root.profileOriginals.Count -ne $roles.Count -or -not $a.loadedOriginal.returned -or
        -not (Test-Cli39LoadedObservationClosure32 $p)){throw 'Actual complete fixed-role profiles/loaded worker closure required.'}
    $a.rowsOriginal=[Collections.Generic.List[object]]::new(3)
    $a.reservationsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $a.sourceDispositionOriginal=@{rootOriginal=$root;staticOriginal=$s;parentTransportOriginal=$t;
      capabilityOriginal=$a.capabilityOriginal;sourceRowsOriginal=$s.memberRowsOriginal;
      qualificationChainOriginal=$a.qualificationChainOriginal}
    foreach($profile in $root.profileOriginals){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended child allocation correspondence.'}
      if(-not $profile.returned -or $profile.role -cnotin $roles -or
          $a.reservationsOriginal.ContainsKey($profile.role)){throw 'Actual unique fixed role profile required.'}
      $row=@{profileOriginal=$profile;parentTransportOriginal=$t;
        qualificationRootSha256=$a.qualificationChainOriginal.qualificationRootSha256;
        currentRootSha256=$root.sha256Expected;reservationBindingOriginal=$null;returned=$false}
      $a.rowsOriginal.Add($row)
      $binding=@{inputRoot=$root.sha256Expected;sourceDisposition=$a.sourceDispositionOriginal;
        receiverCharge=$profile.valuesOriginal.receiverChargeBytes;attributeListBytes=$profile.valuesOriginal.attributeListBytes;
        outputLimitBytes=$profile.outputLimitBytes;budget=$profile.valuesOriginal;profileOriginal=$profile;
        externalFoundationTransportOriginal=$t;qualificationChainOriginal=$a.qualificationChainOriginal;
        correspondenceOriginal=$row}
      $row.reservationBindingOriginal=$binding;$a.reservationsOriginal.Add($profile.role,$binding);$row.returned=$true
    }
    $role=if($root.rootRoleOriginal -ceq 'operation-host'){'hostBudget'}else{'callerBudget'}
    $current=$a.reservationsOriginal[$role]
    if(-not [object]::ReferenceEquals($a.loadedOriginal.profileOriginal,$current.profileOriginal) -or
        $a.loadedOriginal.chargeBytes -ge $current.budget.cliLoadedReservationBytes){throw 'Actual local loaded precharge does not bind SAME current role subdivision.'}
    # The parent retains actual allocator/stack/overlap measurements and native
    # reservation originals through ACK. These local bindings consume that
    # source-selected qualified transport relation; arithmetic creates no quota.
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child allocation completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}

function Complete-Cli39ChildFoundationBindings32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$s=$root.childStaticCorrespondenceOriginal;$t=$root.childDerivedTransportOriginal
  if($null -eq $s -or $s.ContainsKey('bindingsOriginal') -or $null -ne $root.nativeAssociationOriginals -or
      $null -ne $root.admittedInputOriginal -or -not $s.returned -or -not $t.observationsReturned -or
      -not $s.heldReturned -or -not $s.namespaceReturned -or -not $s.principalReturned -or
      -not $s.loadedAssociationOriginal.returned -or -not $s.capabilityAssociationOriginal.returned -or
      -not $s.allocationAssociationOriginal.returned -or $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -ne $root.namespaceBookOriginal.disposalOriginal){throw 'SAME complete once-only seven child correspondence obligations required.'}
  $b=@{hostOwnerOriginal=$p;rootOriginal=$root;staticOriginal=$s;transportOriginal=$t;
    domainsOriginal=$null;associationsOriginal=$null;inputOriginal=$null;
    returned=$false;exceptionOriginal=$null;terminal=$false;
    qualificationPremise='GENUINE_EXTERNAL_WHOLE_INPUT_CONTROLLING_PARENT_FOUNDATION_AND_PHYSICAL_RESERVATIONS_REQUIRED'}
  $s.bindingsOriginal=$b
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child foundation binding.'}
    $b.domainsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach($domain in @('sourceClosure','startupClosure','loadedClosure')){
      $d=@{domainOriginal=$domain;staticOriginal=$s;transportOriginal=$t;rowsOriginal=[Collections.Generic.List[object]]::new();returned=$false}
      $b.domainsOriginal.Add($domain,$d)
      foreach($row in $s.memberRowsOriginal){
        if($row.memberOriginal.domainOriginal -ceq $domain){
          if(-not $row.returned){throw 'Actual full held/static member correspondence incomplete.'};$d.rowsOriginal.Add($row)
        }
      }
      if($d.rowsOriginal.Count -ne $root.fieldMapOriginal[$domain].GetArrayLength()){throw 'Complete child source/tool/installed domain differs.'}
      $d.returned=$true
    }
    $reservations=$s.allocationAssociationOriginal.reservationsOriginal
    $associations=@{hostOwnerOriginal=$p;rootOriginal=$root;correspondenceOriginal=$s;
      startupAssociationOriginal=$t;sourceAssociationOriginal=$b.domainsOriginal['sourceClosure'];
      toolAssociationOriginal=$b.domainsOriginal['startupClosure'];installedAssociationOriginal=$b.domainsOriginal['loadedClosure'];
      namespaceAssociationOriginal=$s.namespaceRowsOriginal;loadedAssociationOriginal=$s.loadedAssociationOriginal;
      principalAssociationOriginal=$s;capabilityAssociationOriginal=$s.capabilityAssociationOriginal;
      allocationAssociationOriginal=$s.allocationAssociationOriginal;consumerReservationOriginal=$reservations['consumerBudget']}
    $b.associationsOriginal=$associations
    if($root.actionOriginal -ceq 'checksum-signature'){$associations.keyPolicyReservationOriginal=$reservations['keyPolicyBudget']}
    $parentRole=if($root.rootRoleOriginal -ceq 'retained-caller'){'callerBudget'}else{'hostBudget'}
    $input=@{rootCaptureOriginal=$root;correspondenceOriginal=$s;receiverOriginal=$p.receiver;
      role=$root.rootRoleOriginal;action=$root.actionOriginal;rootHash=$root.sha256Expected;
      principalApiModuleOriginal=$root.principalSelectorOriginal;
      parentReservationOriginal=$reservations[$parentRole];callerReservationOriginal=$reservations['callerBudget'];
      roleReservationsOriginal=$reservations;startupTransportOriginal=$t;
      qualificationChainOriginal=$s.qualificationRootChainOriginal}
    $b.inputOriginal=$input
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies complete child association publication.'}
    # Publication represents completed local correspondence under the documented
    # genuine external input precondition. It neither allocates physical quota
    # nor qualifies supplied semantics. Per-action policy/useful joins are still
    # required independently before any useful consumer preparation/effects.
    $root.consumerReservationOriginal=$associations.consumerReservationOriginal
    $root.callerReservationOriginal=$reservations['callerBudget']
    $root.admittedConsumerOriginal=$root.consumerShapeOriginal.prototypeOriginal
    $root.nativeAssociationOriginals=$associations;$root.admittedInputOriginal=$input
    $b.returned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}finally{$b.terminal=$true}
}
