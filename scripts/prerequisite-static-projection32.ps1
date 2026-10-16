# Fixed static projection only. No CLR authority, future process identity or
# module addresses cross the role-derived immutable input boundary.
function New-Cli39StaticFoundationProjection32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$p=$b.hostOwnerOriginal;$root=$b.rootOriginal
  if($null -ne $b.projectionOriginal -or $null -ne $b.disposalOriginal -or
      -not [object]::ReferenceEquals($root.derivedRootOriginal,$b)){throw 'SAME once-only live derived projection required.'}
  # Precharge capacity-dependent rows before their arrays/strings. Existing
  # derived book charge includes this fixed constructor/partial scalar graph.
  # Numeric maxima are source type decimal widths (Int32:10, Int64:19,
  # UInt32:10; signed AuthId:20); FileId24 has exactly48 lowercase hexadecimal characters.
  [decimal]$preflightChars=('schemaqualificationRootSha256inheritedSidHexinheritedAuthenticationId'+
    'domainordinalpathbytessha256fileIdentitypathdirectoryfileIdentityoperationRoledesiredAccessshareModecreationDispositionflags'+
    'installedDomaininstalledOrdinalschemapolicyosMemberDomainosMemberOrdinalosMemberSha256osMemberFileIdentityloaderMemberDomainloaderMemberOrdinalloaderMemberSha256loaderMemberFileIdentityprovenanceBindingSha256').Length+
    'service-lasso.cli39-static-foundation.v1'.Length+64+$b.lineageValuesOriginal[7].Length+20
  if($root.rootRoleOriginal -ceq 'qualification-coordinator'){
    $preflight=$root.foundationCorrespondenceOriginal
    if($null -eq $preflight -or -not $preflight.returned -or -not $preflight.bindingsOriginal.returned){throw 'Complete outer correspondence required before projection precharge.'}
    foreach($domain in $preflight.heldDomainsOriginal){foreach($row in $domain.rowsOriginal){
      $preflightChars+=$row.pinOriginal.domain.Length+$row.pinOriginal.path.Length+10+19+64+48
    }}
    foreach($row in $preflight.namespaceOriginals.rowsOriginal){
      $preflightChars+=$row.pinOriginal.path.Length+$row.pinOriginal.rightsOriginal.operationRole.Length+1+48+4*10
    }
    foreach($row in $preflight.loadedOriginals.rowsOriginal){
      $declaration=$row.loaderDeclarationCorrespondenceOriginal
      if($null -eq $declaration -or -not $declaration.returned){throw 'Complete loader declaration correspondence required before projection.'}
      $preflightChars+=('installedDomaininstalledOrdinalschemapolicyosMemberDomainosMemberOrdinalosMemberSha256osMemberFileIdentityloaderMemberDomainloaderMemberOrdinalloaderMemberSha256loaderMemberFileIdentityprovenanceBindingSha256').Length+
        $row.heldOriginal.pinOriginal.domain.Length+10
      foreach($value in $declaration.valuesOriginal.Values){$preflightChars+=$value.Length}
    }
  }else{
    $prior=$root.staticFoundationOriginal
    if($null -eq $prior -or -not $prior.returned){throw 'Validated inherited projection required before precharge.'}
    foreach($roster in @($prior.membersOriginal,$prior.namespaceOriginal,$prior.loadersOriginal)){foreach($row in $roster){
      foreach($value in $row){$preflightChars+=$value.Length}
    }}
  }
  $nodeMeasurement=Measure-Cli39NodePolicyProjection32 $b
  $preflightChars+=$nodeMeasurement.characters+('nodePolicycategoryordinalfieldvalue').Length
  $publisherMeasurement=Measure-Cli39PublisherPolicyProjection32 $b
  $preflightChars+=$publisherMeasurement.characters+('publisherPolicycategoryordinalfieldvalue').Length
  [decimal]$preflightBytes=2*$preflightChars
  $v=$b.profileOriginal
  [decimal]$preflightCharge=[decimal]$v.cliDerivedRootBookChargeBytes+3*[decimal]$v.cliDerivedRootPerWorkerChargeBytes+
    $preflightBytes*[decimal]$v.cliDerivedRootPerByteChargeBytes
  if($v.cliDerivedRootPerByteChargeBytes -le 0 -or $preflightCharge -ge $v.cliDerivedRootReservationBytes){
    throw 'Complete static projection row/backing precharge cannot fit SAME derived subdivision.'
  }
  $b.projectionPreflightBytes=$preflightBytes;$b.projectionPreflightCharge=$preflightCharge
  $a=@{bookOriginal=$b;correspondenceOriginal=$null;inheritedProjectionOriginal=$null;
    scalarNamesOriginal=[string[]]@('schema','qualificationRootSha256','inheritedSidHex','inheritedAuthenticationId');
    scalarValuesOriginal=$null;memberNamesOriginal=[string[]]@('domain','ordinal','path','bytes','sha256','fileIdentity');
    namespaceNamesOriginal=[string[]]@('path','directory','fileIdentity','operationRole','desiredAccess','shareMode','creationDisposition','flags');
    loaderNamesOriginal=[string[]]@('installedDomain','installedOrdinal','schema','policy','osMemberDomain','osMemberOrdinal','osMemberSha256','osMemberFileIdentity',
      'loaderMemberDomain','loaderMemberOrdinal','loaderMemberSha256','loaderMemberFileIdentity','provenanceBindingSha256');
    nodePolicyNamesOriginal=[string[]]@('category','ordinal','field','value');nodePolicyOriginal=$null;nodePolicyConstructionOriginal=$null;
    publisherPolicyNamesOriginal=[string[]]@('category','ordinal','field','value');publisherPolicyOriginal=$null;publisherPolicyConstructionOriginal=$null;
    membersOriginal=$null;namespaceOriginal=$null;loadersOriginal=$null;utf16Bytes=0L;escapedCapacity=0D;
    returned=$false;exceptionOriginal=$null;terminal=$false}
  $b.projectionOriginal=$a
  try{
    $nodeProjection=New-Cli39NodePolicyProjection32 $b $nodeMeasurement
    $a.nodePolicyOriginal=$nodeProjection.rowsOriginal
    $publisherProjection=New-Cli39PublisherPolicyProjection32 $b $publisherMeasurement
    $a.publisherPolicyOriginal=$publisherProjection.rowsOriginal
    $culture=[Globalization.CultureInfo]::InvariantCulture
    if($root.rootRoleOriginal -ceq 'qualification-coordinator'){
      $j=$root.foundationCorrespondenceOriginal;$a.correspondenceOriginal=$j
      if($null -eq $j -or -not $j.returned -or -not $j.constructionTerminal -or
          $null -ne $j.exceptionOriginal -or -not $j.bindingsOriginal.returned){throw 'Actual completed containing correspondence required for projection.'}
      $a.scalarValuesOriginal=[string[]]@('service-lasso.cli39-static-foundation.v1',$root.sha256Expected,
        $b.lineageValuesOriginal[7],$b.lineageValuesOriginal[9])
      $a.membersOriginal=[Collections.Generic.List[object]]::new($root.heldMemberBookOriginal.recordsOriginal.Count)
      foreach($domain in $j.heldDomainsOriginal){
        foreach($row in $domain.rowsOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported -or -not $row.returned){throw 'Original deadline/incomplete member denies static projection.'}
          $pin=$row.pinOriginal
          $values=[string[]]@($pin.domain,$pin.ordinal.ToString($culture),$pin.path,$pin.bytes.ToString($culture),
            $pin.sha256,[BitConverter]::ToString($pin.fileIdentityOriginal).Replace('-','').ToLowerInvariant())
          $a.membersOriginal.Add($values)
        }
      }
      $a.namespaceOriginal=[Collections.Generic.List[object]]::new($j.namespaceOriginals.rowsOriginal.Count)
      foreach($row in $j.namespaceOriginals.rowsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported -or -not $row.returned){throw 'Original deadline/incomplete namespace denies static projection.'}
        $pin=$row.pinOriginal;$rights=$pin.rightsOriginal
        $directory=if($pin.directory){'1'}else{'0'}
        $values=[string[]]@($pin.path,$directory,[BitConverter]::ToString($pin.fileIdentityOriginal).Replace('-','').ToLowerInvariant(),
          $rights.operationRole,$rights.desiredAccess.ToString($culture),$rights.shareMode.ToString($culture),
          $rights.creationDisposition.ToString($culture),$rights.flags.ToString($culture))
        $a.namespaceOriginal.Add($values)
      }
      $a.loadersOriginal=[Collections.Generic.List[object]]::new($j.loadedOriginals.rowsOriginal.Count)
      foreach($row in $j.loadedOriginals.rowsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        $d=$row.loaderDeclarationCorrespondenceOriginal
        if($p.deadlineReported -or -not $row.returned -or -not $d.returned){throw 'Incomplete loader declaration cannot project.'}
        $values=[string[]]::new(13)
        $values[0]=$row.heldOriginal.pinOriginal.domain;$values[1]=$row.heldOriginal.pinOriginal.ordinal.ToString($culture)
        for($i=2;$i -lt 13;$i++){$values[$i]=$d.valuesOriginal[$a.loaderNamesOriginal[$i]]}
        $a.loadersOriginal.Add($values)
      }
    }else{
      # A later role forwards already validated immutable static facts; actual
      # child-local process/loaded/principal observations remain separate.
      $prior=$root.staticFoundationOriginal;$a.inheritedProjectionOriginal=$prior
      if($null -eq $prior -or -not $prior.returned){throw 'Actual validated inherited static projection required.'}
      $a.scalarValuesOriginal=$prior.scalarValuesOriginal
      $a.membersOriginal=$prior.membersOriginal;$a.namespaceOriginal=$prior.namespaceOriginal;$a.loadersOriginal=$prior.loadersOriginal
    }
    [decimal]$capacity=2
    foreach($names in @($a.scalarNamesOriginal,$a.memberNamesOriginal,$a.namespaceNamesOriginal,$a.loaderNamesOriginal,$a.nodePolicyNamesOriginal,$a.publisherPolicyNamesOriginal)){
      foreach($name in $names){$a.utf16Bytes+=2L*$name.Length;$capacity+=6*[decimal]$name.Length+6}
    }
    foreach($value in $a.scalarValuesOriginal){$a.utf16Bytes+=2L*$value.Length;$capacity+=6*[decimal]$value.Length+6}
    foreach($roster in @($a.membersOriginal,$a.namespaceOriginal,$a.loadersOriginal,$a.nodePolicyOriginal,$a.publisherPolicyOriginal)){
      $capacity+=4
      foreach($row in $roster){
        $capacity+=2
        for($i=0;$i -lt $row.Length;$i++){
          $value=$row[$i];$names=if($row.Length -eq 4){$a.nodePolicyNamesOriginal}elseif($row.Length -eq 6){$a.memberNamesOriginal}elseif($row.Length -eq 8){$a.namespaceNamesOriginal}else{$a.loaderNamesOriginal}
          $a.utf16Bytes+=2L*$value.Length
          $capacity+=6*([decimal]$value.Length+[decimal]$names[$i].Length)+6
        }
      }
    }
    $capacity+=6*[decimal]('members'.Length+'namespace'.Length+'loaders'.Length+'nodePolicy'.Length+'publisherPolicy'.Length+'foundationProjection'.Length)+40
    if($a.utf16Bytes -gt $preflightBytes){throw 'Projection exceeds its preowned row/backing extent.'}
    $a.escapedCapacity=$capacity;$a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
function Read-Cli39StaticFoundationProjection32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($root.rootRoleOriginal -ceq 'qualification-coordinator' -or $root.ContainsKey('staticFoundationOriginal')){
    throw 'SAME once-only child static projection required.'
  }
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;elementOriginal=$root.fieldMapOriginal['foundationProjection'];
    propertiesOriginal=$null;mapOriginal=$null;scalarValuesOriginal=$null;membersOriginal=$null;namespaceOriginal=$null;loadersOriginal=$null;nodePolicyOriginal=$null;publisherPolicyOriginal=$null;
    rowsOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false}
  $root.staticFoundationOriginal=$a
  try{
    $a.propertiesOriginal=[Collections.Generic.List[object]]::new(10)
    $a.mapOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $names=[string[]]@('schema','qualificationRootSha256','inheritedSidHex','inheritedAuthenticationId','members','namespace','loaders','nodePolicy','publisherPolicy')
    if($a.elementOriginal.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Fixed static projection object required.'}
    foreach($property in $a.elementOriginal.EnumerateObject()){
      $a.propertiesOriginal.Add($property)
      if($property.Name -cnotin $names -or $a.mapOriginal.ContainsKey($property.Name)){throw 'Unknown/duplicate static projection property.'}
      $a.mapOriginal.Add($property.Name,$property.Value)
    }
    if($a.mapOriginal.Count -ne 9){throw 'Complete static projection field set required.'}
    $a.scalarValuesOriginal=[string[]]::new(4)
    for($i=0;$i -lt 4;$i++){
      $element=$a.mapOriginal[$names[$i]]
      if($element.ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Exact static projection string required.'}
      $a.scalarValuesOriginal[$i]=$element.GetString()
    }
    if($a.scalarValuesOriginal[0] -cne 'service-lasso.cli39-static-foundation.v1' -or
        $a.scalarValuesOriginal[1] -cnotmatch '^[0-9a-f]{64}$' -or
        $a.scalarValuesOriginal[2] -cnotmatch '^(?:[0-9a-f]{2}){8,68}$' -or
        $a.scalarValuesOriginal[3] -cnotmatch '^(?:0|-[1-9][0-9]*|[1-9][0-9]*)$'){
      throw 'Static projection scalar grammar differs.'
    }
    $authentication=0L
    if(-not [long]::TryParse($a.scalarValuesOriginal[3],[Globalization.NumberStyles]::AllowLeadingSign,
        [Globalization.CultureInfo]::InvariantCulture,[ref]$authentication)){throw 'Static authentication ID cannot be represented.'}
    $a.rowsOriginal=[Collections.Generic.List[object]]::new()
    foreach($selection in @(@('members',[string[]]@('domain','ordinal','path','bytes','sha256','fileIdentity')),
        @('namespace',[string[]]@('path','directory','fileIdentity','operationRole','desiredAccess','shareMode','creationDisposition','flags')),
        @('loaders',[string[]]@('installedDomain','installedOrdinal','schema','policy','osMemberDomain','osMemberOrdinal','osMemberSha256','osMemberFileIdentity',
          'loaderMemberDomain','loaderMemberOrdinal','loaderMemberSha256','loaderMemberFileIdentity','provenanceBindingSha256')),
        @('nodePolicy',[string[]]@('category','ordinal','field','value')),
        @('publisherPolicy',[string[]]@('category','ordinal','field','value')))){
      $key=$selection[0];$fields=$selection[1];$element=$a.mapOriginal[$key]
      if($element.ValueKind -ne [System.Text.Json.JsonValueKind]::Array){throw 'Exact complete static row array required.'}
      $roster=[Collections.Generic.List[object]]::new($element.GetArrayLength());$a[$key+'Original']=$roster
      foreach($item in $element.EnumerateArray()){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies static projection parse.'}
        $row=@{elementOriginal=$item;propertiesOriginal=[Collections.Generic.List[object]]::new($fields.Length+1);
          mapOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal);valuesOriginal=$null}
        $a.rowsOriginal.Add($row)
        if($item.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Static row object required.'}
        foreach($property in $item.EnumerateObject()){
          $row.propertiesOriginal.Add($property)
          if($property.Name -cnotin $fields -or $row.mapOriginal.ContainsKey($property.Name) -or
              $property.Value.ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Unknown/duplicate/nonstring static row field.'}
          $row.mapOriginal.Add($property.Name,$property.Value)
        }
        if($row.mapOriginal.Count -ne $fields.Length){throw 'Incomplete static row.'}
        $row.valuesOriginal=[string[]]::new($fields.Length)
        for($i=0;$i -lt $fields.Length;$i++){$row.valuesOriginal[$i]=$row.mapOriginal[$fields[$i]].GetString()}
        $v=$row.valuesOriginal
        if($key -ceq 'members'){
          $ordinal=0;$extent=0L
          if($v[0] -cnotin @('sourceClosure','startupClosure','loadedClosure') -or $v[1] -cnotmatch '^(0|[1-9][0-9]*)$' -or
              -not [int]::TryParse($v[1],[ref]$ordinal) -or -not [IO.Path]::IsPathFullyQualified($v[2]) -or
              $v[2].Contains([char]0) -or $v[3] -cnotmatch '^(0|[1-9][0-9]*)$' -or
              -not [long]::TryParse($v[3],[ref]$extent) -or $v[4] -cnotmatch '^[0-9a-f]{64}$' -or
              $v[5] -cnotmatch '^[0-9a-f]{48}$'){throw 'Static member pin grammar differs.'}
        }elseif($key -ceq 'namespace'){
          if(-not [IO.Path]::IsPathFullyQualified($v[0]) -or $v[0].Contains([char]0) -or
              $v[1] -cnotin @('0','1') -or $v[2] -cnotmatch '^[0-9a-f]{48}$' -or
              [string]::IsNullOrEmpty($v[3]) -or $v[3].Contains([char]0) -or
              $v[4] -cne '128' -or $v[5] -cne '1' -or $v[6] -cne '3' -or $v[7] -cne '35651584'){
            throw 'Static fixed namespace acquisition pin grammar differs.'
          }
        }elseif($key -cin @('nodePolicy','publisherPolicy')){
          $ordinal=0
          foreach($value in $v){if([string]::IsNullOrEmpty($value) -or $value.IndexOf([char]0) -ge 0){throw 'Nonempty NUL-free policy operands required.'}}
          $categories=if($key -ceq 'publisherPolicy'){@('scalar','subject','basis','anchor')}else{@('scalar','roster','subject','basis')}
          if($v[0] -cnotin $categories -or $v[1] -cnotmatch '^(0|[1-9][0-9]*)$' -or
              -not [int]::TryParse($v[1],[ref]$ordinal)){throw 'Closed policy category/canonical ordinal required.'}
          if($key -ceq 'publisherPolicy' -and $v[0] -cnotin @('scalar','subject','basis','anchor')){throw 'Closed publisher category required.'}
        }else{
          foreach($value in $v){if([string]::IsNullOrEmpty($value) -or $value.IndexOf([char]0) -ge 0){throw 'Nonempty NUL-free loader pins required.'}}
          if($v[0] -cne 'loadedClosure' -or $v[2] -cne 'service-lasso.cli39-loader-semantics.v1' -or
              $v[3] -cne 'file-backed-image-under-exclusive-held-namespace' -or $v[12] -cnotmatch '^[0-9a-f]{64}$'){
            throw 'Fixed inherited loader declaration differs.'}
          foreach($index in @(1,5,9)){$ordinal=0;if($v[$index] -cnotmatch '^(0|[1-9][0-9]*)$' -or
              -not [int]::TryParse($v[$index],[ref]$ordinal)){throw 'Canonical loader member ordinal required.'}}
          foreach($index in @(4,8)){if($v[$index] -cnotin @('startupClosure','loadedClosure')){throw 'Closed OS/loader domain required.'}}
          foreach($index in @(6,10)){if($v[$index] -cnotmatch '^[0-9a-f]{64}$'){throw 'Exact loader digest pin required.'}}
          foreach($index in @(7,11)){if($v[$index] -cnotmatch '^[0-9a-f]{48}$'){throw 'Full24 loader identity pin required.'}}
        }
        $roster.Add($row.valuesOriginal)
      }
    }
    $null=Read-Cli39NodePolicyOperands32 $p $a
    $null=Read-Cli39PublisherPolicyOperands32 $p $a
    # This retained closed data is not an authority grant. Actual local held,
    # namespace, parent, principal, loaded and reservation joins remain required.
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
function Join-Cli39ChildStaticObservations32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$projection=$root.staticFoundationOriginal
  if($root.rootRoleOriginal -ceq 'qualification-coordinator' -or $root.ContainsKey('childStaticCorrespondenceOriginal') -or
      $null -eq $projection -or -not $projection.returned -or -not $root.nativeObservationOriginal.returned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal){
    throw 'SAME once-only live child static/native observations required.'
  }
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;projectionOriginal=$projection;memberRowsOriginal=$null;
    namespaceRowsOriginal=$null;qualificationRootPinOriginal=$null;qualificationRootChainOriginal=$null;localRootSubjectOriginal=$root.rootSubjectOriginal;
    localRootQueryOriginal=$root.rootFileQueryOriginal;lineageOriginal=$root.parentLineageOriginal;
    parentObservationOriginal=$root.nativeObservationOriginal.parentObservationOriginal;
    principalBookOriginal=$root.principalModuleOriginal.tokenBookOriginal;principalComparedBytes=0;
    heldReturned=$false;namespaceReturned=$false;principalReturned=$false;
    loadedAssociationOriginal=$null;capabilityAssociationOriginal=$null;allocationAssociationOriginal=$null;
    returned=$false;exceptionOriginal=$null;terminal=$false}
  $root.childStaticCorrespondenceOriginal=$a
  try{
    $members=$root.heldMemberBookOriginal.recordsOriginal
    if($projection.membersOriginal.Count -ne $members.Count){throw 'Child static/full held roster differs.'}
    $a.memberRowsOriginal=[Collections.Generic.List[object]]::new($members.Count)
    for($ordinal=0;$ordinal -lt $members.Count;$ordinal++){
      $member=$members[$ordinal];$pin=$projection.membersOriginal[$ordinal];$query=$member.fileQueryOriginal
      $row=@{pinOriginal=$pin;memberOriginal=$member;queryOriginal=$query;identityComparedBytes=0;returned=$false}
      $a.memberRowsOriginal.Add($row)
      if($pin[0] -cne $member.domainOriginal -or $pin[1] -cne $member.ordinalOriginal.ToString([Globalization.CultureInfo]::InvariantCulture) -or
          $pin[2] -cne $member.subjectOriginal.path -or $pin[3] -cne $member.bytesExpected.ToString([Globalization.CultureInfo]::InvariantCulture) -or
          $pin[4] -cne $member.sha256Expected -or -not $member.subjectOriginal.eof -or -not $member.subjectOriginal.hashReturned -or
          $member.subjectOriginal.used -ne $member.bytesExpected -or -not $query.rawReturned -or
          -not $query.decodeOriginal.returned -or -not $query.decodeOriginal.heldExtentEqual){throw 'Child actual held member differs from static source pin.'}
      for($i=0;$i -lt 24;$i++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended child static identity join.'}
        if($query.buffersOriginal[3][$i] -ne [Convert]::ToByte($pin[5].Substring(2*$i,2),16)){throw 'Child actual held native identity differs from static pin.'}
        $row.identityComparedBytes=$i+1
      }
      $row.returned=$true
    }
    $a.heldReturned=$true
    $namespace=$root.namespaceBookOriginal
    if(-not $namespace.observationReturned -or $projection.namespaceOriginal.Count -ne $namespace.pathOwnersOriginal.Count -or
        $projection.namespaceOriginal.Count -eq 0){throw 'Child static namespace full roster differs.'}
    # Row0 is the original qualification-root provenance. Each child has a
    # different freshly created input root, joined to its parent separately.
    $a.qualificationRootPinOriginal=$projection.namespaceOriginal[0]
    if($a.qualificationRootPinOriginal[1] -cne '0' -or $a.qualificationRootPinOriginal[3] -cne 'held-file' -or
        -not $a.localRootSubjectOriginal.eof -or -not $a.localRootSubjectOriginal.hashReturned -or
        -not $a.localRootQueryOriginal.rawReturned -or -not $a.lineageOriginal.returned -or
        -not $a.parentObservationOriginal.returned){throw 'Actual child root/parent observation and original qualification provenance required.'}
    $a.qualificationRootChainOriginal=@{projectionOriginal=$projection;lineageOriginal=$a.lineageOriginal;
      qualificationRootSha256=$projection.scalarValuesOriginal[1];qualificationRootPinOriginal=$a.qualificationRootPinOriginal;
      parentInputSha256=$a.lineageOriginal.valuesOriginal.parentInputSha256;
      roleOriginal=$root.rootRoleOriginal;directParentHashEqual=$false;parentProjectionTransportOriginal=$null;returned=$false}
    if($root.rootRoleOriginal -ceq 'retained-caller'){
      if($projection.scalarValuesOriginal[1] -cne $a.lineageOriginal.valuesOriginal.parentInputSha256){
        throw 'Direct qualification-parent root digest differs from inherited projection root.'
      }
      $a.qualificationRootChainOriginal.directParentHashEqual=$true
      # Hash correspondence is retained separately from genuine controlling
      # native parent/command/projection transport admission.
    }
    $a.namespaceRowsOriginal=[Collections.Generic.List[object]]::new($namespace.pathOwnersOriginal.Count-1)
    for($ordinal=1;$ordinal -lt $namespace.pathOwnersOriginal.Count;$ordinal++){
      $owner=$namespace.pathOwnersOriginal[$ordinal];$pin=$projection.namespaceOriginal[$ordinal]
      $row=@{ownerOriginal=$owner;pinOriginal=$pin;leafOriginal=$null;identityComparedBytes=0;returned=$false}
      $a.namespaceRowsOriginal.Add($row)
      $directory=if($owner.leafDirectoryOriginal){'1'}else{'0'}
      $role=if($owner.leafDirectoryOriginal){$owner.directoryRoleOriginal.roleOriginal}else{'held-file'}
      if($pin[0] -cne $owner.pathOriginal -or $pin[1] -cne $directory -or $pin[3] -cne $role -or
          -not $owner.observationReturned){throw 'Child static namespace kind/path/role differs.'}
      foreach($hold in $owner.holdsOriginal){
        if(-not $hold.metadataReturned -or -not $hold.kindObserved -or $hold.closePhysicalReturned){throw 'Child static ancestry no longer observed/held.'}
        if($hold.leafOriginal){if($null -ne $row.leafOriginal){throw 'Duplicate child namespace leaf.'};$row.leafOriginal=$hold}
      }
      if($null -eq $row.leafOriginal){throw 'Child static namespace leaf absent.'}
      for($i=0;$i -lt 24;$i++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended child static namespace join.'}
        if($row.leafOriginal.buffersOriginal[1][$i] -ne [Convert]::ToByte($pin[2].Substring(2*$i,2),16)){
          throw 'Child static namespace full native identity differs.'
        }
        $row.identityComparedBytes=$i+1
      }
      $row.returned=$true
    }
    $a.namespaceReturned=$true
    $tokens=$a.principalBookOriginal;$user=$tokens.workersOriginal[0];$statistics=$tokens.workersOriginal[1]
    if(-not $tokens.returned -or -not $user.rawReturned -or -not $user.decodedOriginal.returned -or
        -not $statistics.rawReturned -or -not $statistics.decodedOriginal.returned -or
        $projection.scalarValuesOriginal[2].Length -ne 2*$user.decodedOriginal.sidExtentOriginal -or
        $projection.scalarValuesOriginal[3] -cne $statistics.decodedOriginal.fieldsOriginal['authenticationIdObserved'].ToString([Globalization.CultureInfo]::InvariantCulture)){
      throw 'Actual child inherited principal differs from static relation.'
    }
    for($i=0;$i -lt $user.decodedOriginal.sidExtentOriginal;$i++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended child inherited SID join.'}
      if($user.backingOriginal[$user.decodedOriginal.sidOffsetOriginal+$i] -ne
          [Convert]::ToByte($projection.scalarValuesOriginal[2].Substring(2*$i,2),16)){throw 'Actual child SID differs from inherited static relation.'}
      $a.principalComparedBytes=$i+1
    }
    $a.principalReturned=$true;$a.returned=$true;return $a
    # Completed local observations are only part of seven-obligation admission.
    # No nativeAssociationOriginals/admittedInputOriginal is published here.
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
function Join-Cli39ChildDerivedTransport32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$static=$root.childStaticCorrespondenceOriginal
  if($root.ContainsKey('childDerivedTransportOriginal') -or $null -eq $static -or -not $static.returned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal){
    throw 'SAME once-only live completed child counterparts required.'
  }
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;receiverOriginal=$p.receiver;deadlineTicks=$p.originalDeadlineTicks;
    requestOriginal=$p.requestValidationOriginal;peerOriginal=$root.rootPipePeerOriginal;
    parentOriginal=$root.parentObservationOriginal;lineageOriginal=$root.parentLineageOriginal;
    staticOriginal=$static;projectionOriginal=$root.staticFoundationOriginal;
    rootSubjectOriginal=$root.rootSubjectOriginal;rootQueryOriginal=$root.rootFileQueryOriginal;
    routingOriginal=$root.routingOriginal;sourceContract='CLI39-DERIVED-TRANSPORT-JOIN32';
    externalPremise='GENUINE_WHOLE_INPUT_OFFICIAL_HOST_PARENT_COMMAND_ROOT_NATIVE_RELATION';
    requestBytesObserved=0;observationsReturned=$false;exceptionOriginal=$null;terminal=$false}
  $root.childDerivedTransportOriginal=$a
  try{
    $request=$a.requestOriginal;$peer=$a.peerOriginal;$lineage=$a.lineageOriginal
    if($null -eq $request -or -not $request.returned -or -not [object]::ReferenceEquals($request.frameOriginal,$p.requestFrame) -or
        -not [object]::ReferenceEquals($request.serverOriginal,$p.server) -or
        -not [object]::ReferenceEquals($request.readsOriginal,$p.requestReads) -or $request.readCountOriginal -ne $p.requestReadCount -or
        $request.roleOriginal -cne $root.rootRoleOriginal -or $request.actionOriginal -cne $root.actionOriginal -or
        $request.inputRootPathOriginal -cne $root.path -or $request.inputRootBytesOriginal -ne $root.bytesExpected -or
        $request.inputRootSha256Original -cne $root.sha256Expected -or $request.deadlineTicks -ne $p.originalDeadlineTicks -or
        $request.sharedSourceRootOriginal -cne $root.fieldMapOriginal['sharedSourceRoot'].GetString() -or
        -not $peer.rawReturned -or -not (Test-Cli39RootPipePeerClosure32 $p) -or
        -not $a.parentOriginal.returned -or -not [object]::ReferenceEquals($a.parentOriginal.peerOriginal,$peer) -or
        -not [object]::ReferenceEquals($a.parentOriginal.lineageOriginal,$lineage) -or
        $peer.clientPidObserved -ne $lineage.parentProcessIdObserved -or -not $lineage.returned -or
        -not $a.rootSubjectOriginal.eof -or -not $a.rootSubjectOriginal.hashReturned -or
        $a.rootSubjectOriginal.used -ne $root.bytesExpected -or -not $a.rootQueryOriginal.rawReturned){
      throw 'Actual NPH/command/peer/parent/root immutable counterparts differ.'
    }
    for($i=0;$i -lt $request.readCountOriginal;$i++){
      $read=$request.readsOriginal[$i]
      if(-not $read.attempted -or -not $read.returned -or $null -ne $read.original -or $read.value -le 0 -or
          $null -eq $read.task -or -not $read.task.IsCompleted){throw 'Actual NPH byte read remains incomplete.'}
      $a.requestBytesObserved+=$read.value
    }
    if($a.requestBytesObserved -ne 40 -or $request.frameOriginal.Length -ne 40 -or
        [Text.Encoding]::ASCII.GetString($request.frameOriginal,0,4) -cne 'NPH1' -or
        [BitConverter]::ToUInt32($request.frameOriginal,36) -ne 1){throw 'Exact complete NPH source-owned frame required.'}
    $chain=$static.qualificationRootChainOriginal
    if($null -eq $chain -or -not [object]::ReferenceEquals($chain.projectionOriginal,$a.projectionOriginal) -or
        -not [object]::ReferenceEquals($chain.lineageOriginal,$lineage) -or
        ($root.rootRoleOriginal -ceq 'retained-caller' -and -not $chain.directParentHashEqual)){
      throw 'Original qualification-root chain counterpart differs.'
    }
    $chain.parentProjectionTransportOriginal=$a
    # This is the child counterpart of the source-selected parent-owned book,
    # not a cross-process ReferenceEquals claim or authentication from NPH/PID.
    # Entire genuine input qualification supplies the native command/root
    # provenance relation. Seven-obligation admission stays separate.
    $a.observationsReturned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
