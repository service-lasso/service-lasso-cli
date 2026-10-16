# Fixed policy operand projection. Parent opaque provenance/basis originals stay
# in parent custody; decoded operands are never their replacement.
function Measure-Cli39NodePolicyProjection32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$root=$b.rootOriginal;$p=$b.hostOwnerOriginal
  $m=@{bookOriginal=$b;policyOriginal=$null;inheritedOriginal=$null;characters=0D;rows=0L;returned=$false}
  if($root.actionOriginal -cnotin @('key-policy','checksum-signature')){$m.returned=$true;return $m}
  if($root.rootRoleOriginal -cne 'qualification-coordinator'){
    $prior=$root.staticFoundationOriginal
    if(-not $prior.returned -or $null -eq $prior.nodePolicyOriginal){throw 'Validated inherited Node operands required.'}
    $m.inheritedOriginal=$prior
    foreach($row in $prior.nodePolicyOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies inherited policy sizing.'}
      if($row.Length -ne 4){throw 'Fixed Node policy row width differs.'}
      foreach($value in $row){$m.characters+=$value.Length}
      $m.characters+=('categoryordinalfieldvalue').Length;$m.rows++
    }
  }else{
    $a=$root.nodePolicyOriginal;$m.policyOriginal=$a
    if($null -eq $a -or -not $a.returned -or -not $a.terminal -or $null -ne $a.exceptionOriginal -or
        -not [object]::ReferenceEquals($a.rootOriginal,$root)){throw 'Actual complete selected outer policy required.'}
    if($a.subjectRowsOriginal.Count -ne 4 -or $a.basisRowsOriginal.Count -ne 3){throw 'Complete fixed source-owned policy snapshots required.'}
    foreach($subjectSnapshot in $a.subjectRowsOriginal){if(-not $subjectSnapshot.returned){throw 'Incomplete source-owned policy subject snapshot.'}}
    $f=$a
    # Fixed scalar numeric widths are signed Int64 decimal widths, not guessed
    # policy maxima. Roster/subject lengths derive from actual supplied originals.
    $m.rows=6;$m.characters=('scalar').Length*6+6+('schemaactionprimaryFingerprintnotBeforenotAftervalidationTime').Length+
      'service-lasso.cli39-node-policy-operands.v2'.Length+$f.actionOriginal.Length+$f.primaryFingerprintOriginal.Length+3*20
    foreach($field in @('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms')){
      foreach($value in $f.$field){$m.rows++;$m.characters+='roster'.Length+10+$field.Length+$value.Length}
    }
    foreach($pin in $f.subjectRowsOriginal){
      $m.rows+=4;$m.characters+=4*('subject'.Length+1)+('rolepathbytessha256').Length+
        $pin.role.Length+$pin.path.Length+20+$pin.sha256.Length
    }
    foreach($basis in $f.basisRowsOriginal){
      $m.rows+=4
      $m.characters+=4*('basis'.Length+1)+('kindprimaryFingerprintkeyringSha256validationTime').Length+
        $basis.kind.Length+$basis.primaryFingerprint.Length+$basis.keyringSha256.Length+20
    }
    $loaders=$f.toolLoaderCorrespondenceOriginal
    if($null -eq $loaders -or -not $loaders.returned -or -not $loaders.terminal -or
        -not [object]::ReferenceEquals($loaders.policyOriginal,$f)){throw 'Complete outer tool loader snapshots required before sizing.'}
    foreach($loader in $loaders.rowsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or -not $loader.returned){throw 'Original deadline/incomplete loader denies sizing.'}
      $m.rows+=8
      $m.characters+=8*('loader'.Length+1)+
        ('roleslotworkingDirectoryhomeexecutableDomainexecutableOrdinaldependencyCountenvironmentCount').Length+
        $loader.roleOriginal.Length+$loader.slotOriginal.Length+$loader.workingDirectoryOriginal.Length+
        $loader.homeOriginal.Length+$loader.executableDomainOriginal.Length+3*20
      foreach($member in $loader.membersOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported -or -not $member.returned){throw 'Original deadline/incomplete member denies loader sizing.'}
        $category='loader-'+$loader.roleOriginal+'-member'
        $m.rows+=6;$m.characters+=6*($category.Length+10)+('domainordinalpathbytessha256fileIdentity').Length+
          $member.domainOriginal.Length+$member.pathOriginal.Length+$member.sha256Original.Length+2*20+48
      }
      foreach($entry in $loader.environmentOriginal.GetEnumerator()){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies loader environment sizing.'}
        $category='loader-'+$loader.roleOriginal+'-environment'
        $m.rows+=2;$m.characters+=2*($category.Length+10)+('namevalue').Length+$entry.Key.Length+$entry.Value.Length
      }
    }
    $m.characters+=$m.rows*('categoryordinalfieldvalue').Length
  }
  if($m.rows -gt [int]::MaxValue){throw 'Node policy row roster cannot be represented.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline denies policy size completion.'}
  $m.returned=$true;return $m
}

function New-Cli39NodePolicyProjection32 {
  param([Parameter(Mandatory)]$BookOriginal,[Parameter(Mandatory)]$MeasurementOriginal)
  $b=$BookOriginal;$m=$MeasurementOriginal;$p=$b.hostOwnerOriginal
  if(-not $m.returned -or -not [object]::ReferenceEquals($m.bookOriginal,$b) -or
      $null -eq $b.projectionOriginal -or $b.projectionPreflightCharge -ge $b.profileOriginal.cliDerivedRootReservationBytes){
    throw 'SAME precharged Node projection required.'
  }
  $a=@{measurementOriginal=$m;rowsOriginal=$null;policyOriginal=$m.policyOriginal;
    inheritedOriginal=$m.inheritedOriginal;returned=$false;exceptionOriginal=$null;terminal=$false}
  $b.projectionOriginal.nodePolicyConstructionOriginal=$a
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies Node projection allocation.'}
    $a.rowsOriginal=[Collections.Generic.List[object]]::new([int]$m.rows)
    if($null -ne $m.inheritedOriginal){
      foreach($row in $m.inheritedOriginal.nodePolicyOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies inherited policy retention.'}
        $a.rowsOriginal.Add($row)
      }
    }elseif($null -ne $m.policyOriginal){
      $f=$m.policyOriginal;$culture=[Globalization.CultureInfo]::InvariantCulture
      foreach($field in @('schema','action','primaryFingerprint','notBefore','notAfter','validationTime')){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies policy scalar construction.'}
        $value=if($field -ceq 'schema'){'service-lasso.cli39-node-policy-operands.v2'}elseif($field -cin @('notBefore','notAfter','validationTime')){
          $f.timePolicy.$field.ToString($culture)
        }elseif($field -ceq 'action'){$f.actionOriginal}else{$f.primaryFingerprintOriginal}
        $a.rowsOriginal.Add([string[]]@('scalar','0',$field,$value))
      }
      foreach($field in @('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms')){
        $ordinal=0
        foreach($value in $f.$field){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies policy roster construction.'}
          $a.rowsOriginal.Add([string[]]@('roster',$ordinal.ToString($culture),$field,$value));$ordinal++
        }
      }
      for($ordinal=0;$ordinal -lt 4;$ordinal++){
        $pin=$f.subjectRowsOriginal[$ordinal]
        foreach($field in @('role','path','bytes','sha256')){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies policy subject projection.'}
          $value=if($field -ceq 'bytes'){$pin.bytes.ToString($culture)}else{$pin.$field}
          $a.rowsOriginal.Add([string[]]@('subject',$ordinal.ToString($culture),$field,$value))
        }
      }
      $ordinal=0
      foreach($basis in $f.basisRowsOriginal){
        foreach($field in @('kind','primaryFingerprint','keyringSha256','validationTime')){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies policy basis projection.'}
          $value=if($field -ceq 'validationTime'){$basis.validationTime.ToString($culture)}else{$basis.$field}
          $a.rowsOriginal.Add([string[]]@('basis',$ordinal.ToString($culture),$field,$value))
        };$ordinal++
      }
      $ordinal=0
      foreach($loader in $f.toolLoaderCorrespondenceOriginal.rowsOriginal){
        foreach($field in @('role','slot','workingDirectory','home','executableDomain','executableOrdinal','dependencyCount','environmentCount')){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies loader operand construction.'}
          $value=switch -CaseSensitive ($field){
            'dependencyCount'{$loader.membersOriginal.Count.ToString($culture)}
            'environmentCount'{$loader.environmentOriginal.Count.ToString($culture)}
            'executableOrdinal'{$loader.executableOrdinalOriginal.ToString($culture)}
            default{$loader[($field+'Original')]}
          }
          $a.rowsOriginal.Add([string[]]@('loader',$ordinal.ToString($culture),$field,$value))
        }
        $memberOrdinal=0
        foreach($member in $loader.membersOriginal){
          foreach($field in @('domain','ordinal','path','bytes','sha256','fileIdentity')){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline denies loader member projection.'}
            $value=if($field -ceq 'fileIdentity'){
              [BitConverter]::ToString($member.identityOriginal).Replace('-','').ToLowerInvariant()
            }elseif($field -cin @('ordinal','bytes')){$member[($field+'Original')].ToString($culture)}else{$member[($field+'Original')]}
            $a.rowsOriginal.Add([string[]]@(('loader-'+$loader.roleOriginal+'-member'),$memberOrdinal.ToString($culture),$field,$value))
          };$memberOrdinal++
        }
        $environmentOrdinal=0
        foreach($entry in $loader.environmentOriginal.GetEnumerator()){
          foreach($field in @('name','value')){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline denies loader environment projection.'}
            $value=if($field -ceq 'name'){$entry.Key}else{$entry.Value}
            $a.rowsOriginal.Add([string[]]@(('loader-'+$loader.roleOriginal+'-environment'),$environmentOrdinal.ToString($culture),$field,$value))
          };$environmentOrdinal++
        }
        $ordinal++
      }
    }
    if($a.rowsOriginal.Count -ne $m.rows){throw 'Actual policy projection row count changed after precharge.'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies policy projection completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}

function Read-Cli39NodePolicyOperands32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$ProjectionOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$projection=$ProjectionOriginal
  $a=@{projectionOriginal=$projection;rootOriginal=$root;mapOriginal=$null;rostersOriginal=$null;
    timesOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false}
  $projection.nodePolicyValidationOriginal=$a
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies Node operand allocation.'}
    if($root.actionOriginal -cnotin @('key-policy','checksum-signature')){
      if($projection.nodePolicyOriginal.Count -ne 0){throw 'Non-Node action carries unexpected policy operands.'}
      $a.returned=$true;return $a
    }
    $a.mapOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
    $a.rostersOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach($field in @('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms')){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies Node roster allocation.'}
      $a.rostersOriginal.Add($field,[Collections.Generic.List[string]]::new())
    }
    foreach($row in $projection.nodePolicyOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies policy operand parse.'}
      $category=$row[0];$ordinal=[int]::Parse($row[1],[Globalization.CultureInfo]::InvariantCulture);$field=$row[2];$value=$row[3]
      $allowed=switch -CaseSensitive ($category){
        'scalar'{[string[]]@('schema','action','primaryFingerprint','notBefore','notAfter','validationTime')}
        'roster'{[string[]]@('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms')}
        'subject'{[string[]]@('role','path','bytes','sha256')}
        'basis'{[string[]]@('kind','primaryFingerprint','keyringSha256','validationTime')}
        'loader'{[string[]]@('role','slot','workingDirectory','home','executableDomain','executableOrdinal','dependencyCount','environmentCount')}
        'loader-gpg-member'{[string[]]@('domain','ordinal','path','bytes','sha256','fileIdentity')}
        'loader-gpgv-member'{[string[]]@('domain','ordinal','path','bytes','sha256','fileIdentity')}
        'loader-gpg-environment'{[string[]]@('name','value')}
        'loader-gpgv-environment'{[string[]]@('name','value')}
        default{throw 'No generic Node policy category exists.'}
      }
      if($field -cnotin $allowed -or ($category -ceq 'scalar' -and $ordinal -ne 0) -or
          ($category -ceq 'subject' -and $ordinal -ge 4) -or ($category -ceq 'basis' -and $ordinal -ge 3)){
        throw 'Unknown Node policy field or fixed ordinal.'
      }
      $key=$category+':'+$row[1]+':'+$field
      if($a.mapOriginal.ContainsKey($key)){throw 'Duplicate actual Node operand row.'}
      $a.mapOriginal.Add($key,$value)
      if($category -ceq 'roster'){
        $roster=$a.rostersOriginal[$field]
        if($ordinal -ne $roster.Count -or $roster.Contains($value) -or
            ($field -cin @('expectedKeyFingerprints','expectedSigningFingerprints') -and $value -cnotmatch '^[A-F0-9]{40}$') -or
            ($field -cin @('allowedPublicKeyAlgorithms','allowedHashAlgorithms') -and $value -cnotmatch '^[1-9][0-9]*$')){
          throw 'Noncanonical, duplicate or noncontiguous Node policy roster.'
        }
        $roster.Add($value)
      }
    }
    $rosterRows=0
    foreach($roster in $a.rostersOriginal.Values){if($roster.Count -eq 0){throw 'Complete nonempty Node policy rosters required.'};$rosterRows+=$roster.Count}
    $loaderRows=0;$loaderRoles=if($root.actionOriginal -ceq 'checksum-signature'){@('gpg','gpgv')}else{@('gpg')}
    for($loaderOrdinal=0;$loaderOrdinal -lt $loaderRoles.Count;$loaderOrdinal++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies loader operand geometry.'}
      $prefix='loader:'+$loaderOrdinal.ToString([Globalization.CultureInfo]::InvariantCulture)+':'
      $dependencyCount=0;$environmentCount=0;$executableOrdinal=0
      foreach($field in @('dependencyCount','environmentCount','executableOrdinal')){
        $value=$a.mapOriginal[$prefix+$field];$number=0
        if($value -cnotmatch '^(0|[1-9][0-9]*)$' -or -not [int]::TryParse($value,[ref]$number)){
          throw 'Canonical representable loader geometry required.'
        }
        switch -CaseSensitive ($field){
          'dependencyCount'{$dependencyCount=$number}
          'environmentCount'{$environmentCount=$number}
          'executableOrdinal'{$executableOrdinal=$number}
        }
      }
      if($a.mapOriginal[$prefix+'role'] -cne $loaderRoles[$loaderOrdinal] -or
          $dependencyCount -le 0 -or $environmentCount -le 0 -or
          $a.mapOriginal[$prefix+'executableDomain'] -cnotin @('startupClosure','loadedClosure')){
        throw 'Exact fixed loader role/dependency/environment geometry required.'
      }
      $expectedSlot=if($loaderRoles[$loaderOrdinal] -ceq 'gpg'){'cli39.node-key-policy.v1'}else{'cli39.node-checksum-signature.v1'}
      if($a.mapOriginal[$prefix+'slot'] -cne $expectedSlot){throw 'Fixed loader slot differs.'}
      foreach($field in @('workingDirectory','home')){
        $value=$a.mapOriginal[$prefix+$field]
        if([string]::IsNullOrEmpty($value) -or $value.Contains([char]0) -or -not [IO.Path]::IsPathFullyQualified($value)){
          throw 'Exact absolute loader path operand required.'
        }
      }
      $loaderRows+=8+6L*$dependencyCount+2L*$environmentCount
      for($memberOrdinal=0;$memberOrdinal -lt $dependencyCount;$memberOrdinal++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies loader member decoding.'}
        $memberPrefix='loader-'+$loaderRoles[$loaderOrdinal]+'-member:'+$memberOrdinal.ToString([Globalization.CultureInfo]::InvariantCulture)+':'
        $memberNumber=0;$extent=0L;$value=$a.mapOriginal[$memberPrefix+'ordinal']
        if($a.mapOriginal[$memberPrefix+'domain'] -cnotin @('startupClosure','loadedClosure') -or
            $value -cnotmatch '^(0|[1-9][0-9]*)$' -or -not [int]::TryParse($value,[ref]$memberNumber) -or
            $a.mapOriginal[$memberPrefix+'bytes'] -cnotmatch '^(0|[1-9][0-9]*)$' -or
            -not [long]::TryParse($a.mapOriginal[$memberPrefix+'bytes'],[ref]$extent) -or
            $a.mapOriginal[$memberPrefix+'sha256'] -cnotmatch '^[A-Fa-f0-9]{64}$' -or
            $a.mapOriginal[$memberPrefix+'fileIdentity'] -cnotmatch '^[a-f0-9]{48}$'){
          throw 'Complete canonical tool member operands required.'
        }
        $value=$a.mapOriginal[$memberPrefix+'path']
        if([string]::IsNullOrEmpty($value) -or $value.Contains([char]0) -or -not [IO.Path]::IsPathFullyQualified($value)){
          throw 'Exact tool member path required.'
        }
      }
      for($environmentOrdinal=0;$environmentOrdinal -lt $environmentCount;$environmentOrdinal++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies loader environment decoding.'}
        $environmentPrefix='loader-'+$loaderRoles[$loaderOrdinal]+'-environment:'+$environmentOrdinal.ToString([Globalization.CultureInfo]::InvariantCulture)+':'
        $name=$a.mapOriginal[$environmentPrefix+'name'];$value=$a.mapOriginal[$environmentPrefix+'value']
        if([string]::IsNullOrEmpty($name) -or $name.IndexOfAny([char[]]@('=',[char]0)) -ge 0 -or
            $null -eq $value -or $value.Contains([char]0)){throw 'Complete literal tool environment operands required.'}
      }
    }
    if($a.mapOriginal.Count -ne 34+$rosterRows+$loaderRows -or
        $a.mapOriginal['scalar:0:schema'] -cne 'service-lasso.cli39-node-policy-operands.v2' -or
        $a.mapOriginal['scalar:0:action'] -cne $root.actionOriginal -or
        $a.mapOriginal['scalar:0:primaryFingerprint'] -cne 'CC68F5A3106FF448322E48ED27F5E38D5B0A215F'){
      throw 'Complete fixed Node scalar/subject/basis graph required.'
    }
    $a.timesOriginal=[Collections.Generic.Dictionary[string,long]]::new([StringComparer]::Ordinal)
    foreach($field in @('notBefore','notAfter','validationTime')){
      $value=$a.mapOriginal['scalar:0:'+$field];$number=0L
      if($value -cnotmatch '^(0|[1-9][0-9]*)$' -or -not [long]::TryParse($value,[ref]$number)){throw 'Canonical nonnegative Node time required.'}
      $a.timesOriginal.Add($field,$number)
    }
    if($a.timesOriginal['notBefore'] -gt $a.timesOriginal['validationTime'] -or
        $a.timesOriginal['validationTime'] -gt $a.timesOriginal['notAfter'] -or
        -not $a.rostersOriginal['expectedKeyFingerprints'].Contains($a.mapOriginal['scalar:0:primaryFingerprint'])){
      throw 'Selected Node time/key bounds differ.'
    }
    foreach($fingerprint in $a.rostersOriginal['expectedSigningFingerprints']){
      if(-not $a.rostersOriginal['expectedKeyFingerprints'].Contains($fingerprint)){throw 'Signing key outside selected key roster.'}
    }
    $roles=[string[]]@('executable','keyring','signature','listing')
    for($ordinal=0;$ordinal -lt 4;$ordinal++){
      $prefix='subject:'+$ordinal.ToString([Globalization.CultureInfo]::InvariantCulture)+':'
      $extent=0L;$path=$a.mapOriginal[$prefix+'path'];$bytes=$a.mapOriginal[$prefix+'bytes']
      if($a.mapOriginal[$prefix+'role'] -cne $roles[$ordinal] -or -not [IO.Path]::IsPathFullyQualified($path) -or
          $bytes -cnotmatch '^[1-9][0-9]*$' -or -not [long]::TryParse($bytes,[ref]$extent) -or
          $a.mapOriginal[$prefix+'sha256'] -cnotmatch '^[A-Fa-f0-9]{64}$'){throw 'Exact nonempty fixed Node subject operands required.'}
    }
    $kinds=[string[]]@('cli39.qualified-node-validation.v1','cli39.qualified-node-baseline.v1','cli39.qualified-node-current-revocation.v1')
    for($ordinal=0;$ordinal -lt 3;$ordinal++){
      $prefix='basis:'+$ordinal.ToString([Globalization.CultureInfo]::InvariantCulture)+':'
      if($a.mapOriginal[$prefix+'kind'] -cne $kinds[$ordinal] -or
          $a.mapOriginal[$prefix+'primaryFingerprint'] -cne $a.mapOriginal['scalar:0:primaryFingerprint'] -or
          $a.mapOriginal[$prefix+'keyringSha256'] -cne $a.mapOriginal['subject:1:sha256'] -or
          $a.mapOriginal[$prefix+'validationTime'] -cne $a.mapOriginal['scalar:0:validationTime']){
        throw 'Node validation/baseline/revocation operands differ from selected key/time.'
      }
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies Node operand completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}

function Join-Cli39ChildNodePolicy32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($root.actionOriginal -cnotin @('key-policy','checksum-signature')){return}
  $s=$root.childStaticCorrespondenceOriginal;$t=$root.childDerivedTransportOriginal
  $projection=$root.staticFoundationOriginal;$operands=$projection.nodePolicyValidationOriginal
  if($root.ContainsKey('nodePolicyOriginal') -or -not $s.bindingsOriginal.returned -or
      -not $t.observationsReturned -or -not $operands.returned -or
      -not [object]::ReferenceEquals($operands.rootOriginal,$root) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only completed child policy transport required.'}
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;consumerOriginal=$root.admittedConsumerOriginal;
    projectionOriginal=$projection;operandsOriginal=$operands;parentTransportOriginal=$t;
    qualificationChainOriginal=$s.qualificationRootChainOriginal;subjectRowsOriginal=$null;
    expectedKeyFingerprints=$null;expectedSigningFingerprints=$null;allowedPublicKeyAlgorithms=$null;
    allowedHashAlgorithms=$null;timePolicy=$null;basisRowsOriginal=$null;
    returned=$false;exceptionOriginal=$null;terminal=$false;
    qualificationPremise='GENUINE_PARENT_POLICY_BASIS_LIFETIME_AND_IMMUTABLE_DERIVED_TRANSPORT_QUALIFIED_EXTERNALLY'}
  $root.nodePolicyOriginal=$a
  try{
    foreach($field in @('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms')){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies child Node set allocation.'}
      $a[$field]=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
      foreach($value in $operands.rostersOriginal[$field]){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies child Node roster construction.'}
        if(-not $a[$field].Add($value)){throw 'Duplicate selected child Node operand.'}
      }
    }
    $a.subjectRowsOriginal=[Collections.Generic.List[object]]::new(4)
    $roles=[string[]]@('executable','keyring','signature','listing')
    for($ordinal=0;$ordinal -lt 4;$ordinal++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies child Node subject join.'}
      $role=$roles[$ordinal];$pin=$a.consumerOriginal.subjectPins[$role]
      $prefix='subject:'+$ordinal.ToString([Globalization.CultureInfo]::InvariantCulture)+':'
      $row=@{roleOriginal=$role;consumerPinOriginal=$pin;operandPrefixOriginal=$prefix;
        heldRowsOriginal=[Collections.Generic.List[object]]::new();returned=$false}
      $a.subjectRowsOriginal.Add($row)
      if($pin.path -cne $operands.mapOriginal[$prefix+'path'] -or
          $pin.bytes.ToString([Globalization.CultureInfo]::InvariantCulture) -cne $operands.mapOriginal[$prefix+'bytes'] -or
          $pin.sha256 -cne $operands.mapOriginal[$prefix+'sha256']){throw 'Child policy operand differs from actual fixed consumer subject.'}
      foreach($held in $s.memberRowsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies held Node subject traversal.'}
        $member=$held.memberOriginal
        if($member.subjectOriginal.path -ceq $pin.path){
          if(-not $held.returned -or $member.bytesExpected -ne $pin.bytes -or
              $member.sha256Expected -ine $pin.sha256 -or -not $member.subjectOriginal.eof -or
              -not $member.subjectOriginal.hashReturned -or $member.subjectOriginal.used -ne $pin.bytes){
            throw 'Actual held child policy subject correspondence differs.'
          }
          $row.heldRowsOriginal.Add($held)
        }
      }
      if($row.heldRowsOriginal.Count -eq 0){throw 'Actual held policy subject absent from full child closure.'}
      $row.returned=$true
    }
    $a.basisRowsOriginal=[Collections.Generic.List[object]]::new(3)
    foreach($kind in @('validationOriginal','baselineOriginal','currentRevocationOriginal')){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies child Node basis retention.'}
      $row=@{roleOriginal=$kind;ordinalOriginal=$a.basisRowsOriginal.Count;operandsOriginal=$operands;
        parentTransportOriginal=$t;qualificationChainOriginal=$a.qualificationChainOriginal;
        keyringSubjectOriginal=$a.subjectRowsOriginal[1];returned=$true}
      $a.basisRowsOriginal.Add($row)
    }
    $a.timePolicy=@{notBefore=$operands.timesOriginal['notBefore'];notAfter=$operands.timesOriginal['notAfter'];
      validationTime=$operands.timesOriginal['validationTime'];validationOriginal=$a.basisRowsOriginal[0];
      baselineOriginal=$a.basisRowsOriginal[1];currentRevocationOriginal=$a.basisRowsOriginal[2]}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child Node policy publication.'}
    foreach($field in @('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms','timePolicy')){
      $a.consumerOriginal[$field]=$a[$field]
    }
    $a.consumerOriginal.nodePolicyOriginal=$a;$a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
