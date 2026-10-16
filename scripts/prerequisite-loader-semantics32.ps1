# Declaration correspondence under the explicit genuine supplied foundation
# precondition. This reader neither issues nor authenticates OS-loader facts.
function Join-Cli39LoaderDeclaration32 {
  param([Parameter(Mandatory)]$CollectorOriginal,[Parameter(Mandatory)]$LoadedRowOriginal)
  $j=$CollectorOriginal;$p=$j.hostOwnerOriginal;$row=$LoadedRowOriginal
  $image=$row.mappedCoverageOriginal.fileBackedImageOriginal;$declaration=$image.loaderDeclarationOriginal
  if($row.ContainsKey('loaderDeclarationCorrespondenceOriginal')){throw 'Loader declaration cannot reenroll.'}
  $a=@{collectorOriginal=$j;loadedRowOriginal=$row;imageOriginal=$image;declarationOriginal=$declaration;
    osLoaderOriginal=$image.osLoaderOriginal;sectionOriginal=$image.sectionOriginal;
    provenanceOriginal=$image.provenanceOriginal;lifetimeOriginal=$image.lifetimeOriginal;
    valuesOriginal=$null;osMemberOriginal=$null;loaderMemberOriginal=$null;
    returned=$false;exceptionOriginal=$null;terminal=$false;
    qualificationPremise='EXTERNAL_WHOLE_INPUT_BINDS_DECLARATION_TO_ACTUAL_RETAINED_OS_LOADER_SOURCE_NAMESPACE_ORIGINALS'}
  $row.loaderDeclarationCorrespondenceOriginal=$a
  try{
    $names=[string[]]@('schema','policy','osMemberDomain','osMemberOrdinal','osMemberSha256','osMemberFileIdentity',
      'loaderMemberDomain','loaderMemberOrdinal','loaderMemberSha256','loaderMemberFileIdentity','provenanceBindingSha256')
    Assert-Cli39FoundationShape32 $declaration $names
    $a.valuesOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
    foreach($name in $names){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended loader declaration correspondence.'}
      $value=$declaration.$name
      if($value -isnot [string] -or $value.Length -eq 0 -or $value.IndexOf([char]0) -ge 0){throw 'Exact nonempty loader declaration strings required.'}
      $a.valuesOriginal.Add($name,$value)
    }
    $v=$a.valuesOriginal
    if($v['schema'] -cne 'service-lasso.cli39-loader-semantics.v1' -or
        $v['policy'] -cne 'file-backed-image-under-exclusive-held-namespace' -or
        $v['provenanceBindingSha256'] -cnotmatch '^[0-9a-f]{64}$'){
      throw 'Fixed loader declaration schema/policy/provenance custody digest required.'
    }
    foreach($prefix in @('os','loader')){
      $domain=$v[$prefix+'MemberDomain'];$ordinalText=$v[$prefix+'MemberOrdinal'];$ordinal=0
      if($domain -cnotin @('startupClosure','loadedClosure') -or $ordinalText -cnotmatch '^(0|[1-9][0-9]*)$' -or
          -not [int]::TryParse($ordinalText,[ref]$ordinal) -or
          $v[$prefix+'MemberSha256'] -cnotmatch '^[0-9a-f]{64}$' -or
          $v[$prefix+'MemberFileIdentity'] -cnotmatch '^[0-9a-f]{48}$'){throw 'Closed held OS/loader selector geometry required.'}
      $selected=$null
      foreach($heldDomain in $j.heldDomainsOriginal){foreach($held in $heldDomain.rowsOriginal){
        if($held.pinOriginal.domain -ceq $domain -and $held.pinOriginal.ordinal -eq $ordinal){
          if($null -ne $selected){throw 'Duplicate selected OS/loader held member.'};$selected=$held
        }
      }}
      if($null -eq $selected -or -not $selected.returned -or
          $selected.pinOriginal.sha256 -cne $v[$prefix+'MemberSha256']){throw 'Actual complete held OS/loader digest correspondence absent.'}
      $raw=$selected.nativeQueryOriginal.buffersOriginal[3]
      if($null -eq $raw -or $raw.Length -ne 24){throw 'Actual full24 OS/loader identity backing required.'}
      for($i=0;$i -lt 24;$i++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended OS/loader identity comparison.'}
        $hex='0123456789abcdef';$text=$v[$prefix+'MemberFileIdentity']
        if($text[2*$i] -cne $hex[$raw[$i] -shr 4] -or $text[2*$i+1] -cne $hex[$raw[$i] -band 15]){
          throw 'Actual held OS/loader full identity differs from declaration.'
        }
      }
      $a[$prefix+'MemberOriginal']=$selected
    }
    if($null -eq $a.osLoaderOriginal -or $null -eq $a.sectionOriginal -or $null -eq $a.provenanceOriginal -or
        -not [object]::ReferenceEquals($a.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal)){
      throw 'Actual supplied parent OS-loader/section/provenance/lifetime originals absent.'
    }
    # Exact declaration/held correspondence is not authentic qualification of
    # the opaque originals or this policy. That is the selected external whole
    # input precondition and must bind the declaration custody digest as well.
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}

function Join-Cli39ChildLoaderSemantics32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$static=$root.childStaticCorrespondenceOriginal
  $projection=$root.staticFoundationOriginal;$transport=$root.childDerivedTransportOriginal;$loaded=$root.loadedObservationOriginal
  if($null -eq $static -or -not $static.returned -or $null -ne $static.loadedAssociationOriginal -or
      -not $projection.returned -or -not $transport.observationsReturned -or -not $loaded.returned -or
      -not [object]::ReferenceEquals($transport.staticOriginal,$static) -or
      -not [object]::ReferenceEquals($transport.projectionOriginal,$projection) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only actual child loader correspondence required.'}
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;staticOriginal=$static;projectionOriginal=$projection;
    parentTransportOriginal=$transport;qualificationChainOriginal=$static.qualificationRootChainOriginal;
    actualLoadedOriginal=$loaded;processOriginal=$root.rootProcessOriginal;rowsOriginal=$null;
    returned=$false;exceptionOriginal=$null;terminal=$false;
    qualificationPremise='EXTERNAL_WHOLE_INPUT_AUTHENTICATES_INHERITED_DECLARATION_AND_CONTROLLING_TRANSPORT_NOT_STATIC_PINS'}
  $static.loadedAssociationOriginal=$a
  try{
    if($projection.loadersOriginal.Count -ne $loaded.modulesOriginal.Count -or
        $a.qualificationChainOriginal.qualificationRootSha256 -cne $projection.scalarValuesOriginal[1] -or
        -not [object]::ReferenceEquals($a.qualificationChainOriginal.parentProjectionTransportOriginal,$transport)){
      throw 'Original qualification chain/complete loader declaration roster differs.'}
    $a.rowsOriginal=[Collections.Generic.List[object]]::new($projection.loadersOriginal.Count)
    foreach($pin in $projection.loadersOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended inherited child loader join.'}
      $row=@{pinOriginal=$pin;actualModuleOriginal=$null;installedMemberOriginal=$null;
        osMemberOriginal=$null;loaderMemberOriginal=$null;parentDeclarationTransportOriginal=$transport;
        actualProcessOriginal=$a.processOriginal;actualCreationOriginal=$a.processOriginal.timesOriginal;
        identityComparedBytes=0;returned=$false}
      $a.rowsOriginal.Add($row)
      foreach($module in $loaded.modulesOriginal){
        if($module.memberOriginal.domainOriginal -ceq $pin[0] -and
            $module.memberOriginal.ordinalOriginal.ToString([Globalization.CultureInfo]::InvariantCulture) -ceq $pin[1]){
          if($null -ne $row.actualModuleOriginal){throw 'Duplicate child installed loader member.'}
          $row.actualModuleOriginal=$module;$row.installedMemberOriginal=$module.memberOriginal
        }
      }
      if($null -eq $row.actualModuleOriginal -or -not $row.actualModuleOriginal.returned -or
          -not [object]::ReferenceEquals($row.actualModuleOriginal.processOriginal,$a.processOriginal)){
        throw 'Actual child-specific process/module observation absent.'}
      foreach($prior in $a.rowsOriginal){if(-not [object]::ReferenceEquals($prior,$row) -and
          [object]::ReferenceEquals($prior.actualModuleOriginal,$row.actualModuleOriginal)){throw 'Duplicate inherited installed declaration.'}}
      foreach($selection in @(@('os',4,5,6,7),@('loader',8,9,10,11))){
        $selected=$null
        foreach($member in $root.heldMemberBookOriginal.recordsOriginal){
          if($member.domainOriginal -ceq $pin[$selection[1]] -and
              $member.ordinalOriginal.ToString([Globalization.CultureInfo]::InvariantCulture) -ceq $pin[$selection[2]]){
            if($null -ne $selected){throw 'Duplicate actual child OS/loader member.'};$selected=$member
          }
        }
        if($null -eq $selected -or $selected.sha256Expected -cne $pin[$selection[3]] -or
            -not $selected.subjectOriginal.hashReturned -or -not $selected.subjectOriginal.eof -or
            -not $selected.fileQueryOriginal.decodeOriginal.returned){throw 'Actual child held OS/loader correspondence incomplete.'}
        $raw=$selected.fileQueryOriginal.buffersOriginal[3];$hex='0123456789abcdef';$identity=$pin[$selection[4]]
        for($i=0;$i -lt 24;$i++){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline ended child loader identity comparison.'}
          if($identity[2*$i] -cne $hex[$raw[$i] -shr 4] -or $identity[2*$i+1] -cne $hex[$raw[$i] -band 15]){
            throw 'Actual child held OS/loader identity differs from inherited declaration.'}
          $row.identityComparedBytes++
        }
        $row[$selection[0]+'MemberOriginal']=$selected
      }
      # Parent OS/section originals stay on the parent. This row retains only
      # inherited semantic pins plus separately acquired child observations.
      # Their authentic provenance/semantics is an external whole-input fact.
      $row.returned=$true
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child loader correspondence completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
