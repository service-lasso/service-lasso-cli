# Fixed copied publisher operands. Actual OS/store/provider originals remain
# retained by the controlling parent; this data cannot authenticate them.
function Measure-Cli39PublisherPolicyProjection32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$root=$b.rootOriginal;$p=$b.hostOwnerOriginal
  $m=@{bookOriginal=$b;foundationOriginal=$null;inheritedOriginal=$null;characters=0D;rows=0L;returned=$false}
  if($root.actionOriginal -cne 'publisher'){$m.returned=$true;return $m}
  if($root.rootRoleOriginal -cne 'qualification-coordinator'){
    $prior=$root.staticFoundationOriginal
    if(-not $prior.returned -or -not $prior.publisherPolicyValidationOriginal.returned){throw 'Validated inherited publisher operands required.'}
    $m.inheritedOriginal=$prior
    foreach($row in $prior.publisherPolicyOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or $row.Length -ne 4){throw 'Original deadline/fixed publisher row width differs.'}
      foreach($value in $row){$m.characters+=$value.Length}
      $m.characters+=('categoryordinalfieldvalue').Length;$m.rows++
    }
  }else{
    $o=$root.publisherWindowsFoundationOriginal;$m.foundationOriginal=$o
    if($null -eq $o -or -not $o.returned -or -not $o.constructionTerminal -or $null -ne $o.exceptionOriginal -or
        -not [object]::ReferenceEquals($o.rootOriginal,$root) -or $o.subjectRowsOriginal.Count -ne 3){throw 'Complete SAME outer publisher snapshots required.'}
    if(-not $o.basisConstructionOriginal.returned -or -not $o.basisConstructionOriginal.terminal -or
        $null -ne $o.basisConstructionOriginal.exceptionOriginal){throw 'Complete actual source-owned publisher basis snapshots required.'}
    $m.rows=24
    $m.characters=24*('categoryordinalfieldvalue').Length+12*('scalar'.Length+1)+
      ('schema').Length+'service-lasso.cli39-publisher-policy-operands.v1'.Length
    foreach($field in $o.declarationOriginal.Keys){
      $value=$o.declarationOriginal[$field]
      $m.characters+=$field.Length
      if($value -is [string]){$m.characters+=$value.Length}elseif($value -is [uint32]){$m.characters+=10}else{$m.characters+=20}
    }
    foreach($r in $o.subjectRowsOriginal){
      if(-not $r.returned){throw 'Incomplete publisher subject snapshot.'}
      $m.characters+=4*('subject'.Length+1)+('rolepathbytessha256').Length+
        $r.roleOriginal.Length+$r.pathOriginal.Length+20+$r.sha256Original.Length
    }
    foreach($basis in $o.basisConstructionOriginal.recordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or -not $basis.returned){throw 'Original deadline/incomplete publisher basis sizing.'}
      foreach($field in $basis.valuesOriginal.Keys){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies publisher basis field sizing.'}
        $value=$basis.valuesOriginal[$field];$m.rows++
        $m.characters+=('categoryordinalfieldvaluebasis').Length+1+$field.Length
        if($value -is [string]){$m.characters+=$value.Length}elseif($value -is [uint32]){$m.characters+=10}else{$m.characters+=20}
      }
    }
    foreach($anchor in $o.basisConstructionOriginal.anchorRecordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or -not $anchor.returned -or $anchor.copiedBytes -ne $anchor.snapshotOriginal.Length){throw 'Actual complete anchor snapshot sizing required.'}
      $m.rows++;$m.characters+=('categoryordinalfieldvalueanchorcertificateDerHex').Length+10+2D*$anchor.snapshotOriginal.Length
    }
  }
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported -or $m.rows -lt 65 -or $m.rows -gt [int]::MaxValue){throw 'Original deadline/complete publisher projection geometry differs.'}
  $m.returned=$true;return $m
}

function New-Cli39PublisherPolicyProjection32 {
  param([Parameter(Mandatory)]$BookOriginal,[Parameter(Mandatory)]$MeasurementOriginal)
  $b=$BookOriginal;$m=$MeasurementOriginal;$p=$b.hostOwnerOriginal
  if(-not $m.returned -or -not [object]::ReferenceEquals($m.bookOriginal,$b) -or
      $null -eq $b.projectionOriginal -or $b.projectionPreflightCharge -ge $b.profileOriginal.cliDerivedRootReservationBytes){throw 'SAME precharged publisher projection required.'}
  $a=@{measurementOriginal=$m;foundationOriginal=$m.foundationOriginal;inheritedOriginal=$m.inheritedOriginal;
    rowsOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false}
  $b.projectionOriginal.publisherPolicyConstructionOriginal=$a
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies publisher projection allocation.'}
    $a.rowsOriginal=[Collections.Generic.List[object]]::new([int]$m.rows)
    if($null -ne $m.inheritedOriginal){
      foreach($row in $m.inheritedOriginal.publisherPolicyOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies inherited publisher row retention.'}
        $a.rowsOriginal.Add($row)
      }
    }elseif($null -ne $m.foundationOriginal){
      $o=$m.foundationOriginal;$culture=[Globalization.CultureInfo]::InvariantCulture
      foreach($field in @('schema','policyId','timestampMode','expectedPublisherIdentity','actionGuid','providerFlags',
          'revocationChecks','uiChoice','unionChoice','notBeforeFileTime','notAfterFileTime','validationFileTime')){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies publisher scalar projection.'}
        $value=if($field -ceq 'schema'){'service-lasso.cli39-publisher-policy-operands.v1'}else{
          $v=$o.declarationOriginal[$field];if($v -is [string]){$v}else{$v.ToString($culture)}
        }
        $a.rowsOriginal.Add([string[]]@('scalar','0',$field,$value))
      }
      for($ordinal=0;$ordinal -lt 3;$ordinal++){
        $r=$o.subjectRowsOriginal[$ordinal]
        foreach($field in @('role','path','bytes','sha256')){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies publisher subject projection.'}
          $value=if($field -ceq 'bytes'){$r.bytesOriginal.ToString($culture)}else{$r[$field+'Original']}
          $a.rowsOriginal.Add([string[]]@('subject',$ordinal.ToString($culture),$field,$value))
        }
      }
      $ordinal=0
      foreach($basis in $o.basisConstructionOriginal.recordsOriginal){
        foreach($field in $basis.valuesOriginal.Keys){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies publisher basis projection.'}
          $v=$basis.valuesOriginal[$field];$value=if($v -is [string]){$v}else{$v.ToString($culture)}
          $a.rowsOriginal.Add([string[]]@('basis',$ordinal.ToString($culture),$field,$value))
        };$ordinal++
      }
      $ordinal=0
      foreach($anchor in $o.basisConstructionOriginal.anchorRecordsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies publisher anchor encoding.'}
        $value=[BitConverter]::ToString($anchor.snapshotOriginal).Replace('-','').ToLowerInvariant()
        $a.rowsOriginal.Add([string[]]@('anchor',$ordinal.ToString($culture),'certificateDerHex',$value));$ordinal++
      }
    }
    if($a.rowsOriginal.Count -ne $m.rows){throw 'Publisher projection count changed after precharge.'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies publisher projection completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}

function Read-Cli39PublisherPolicyOperands32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$ProjectionOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$projection=$ProjectionOriginal
  $a=@{projectionOriginal=$projection;rootOriginal=$root;mapOriginal=$null;subjectsOriginal=$null;
    numericOriginal=$null;basisOriginals=$null;anchorsOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false}
  $projection.publisherPolicyValidationOriginal=$a
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies publisher operand construction.'}
    if($root.actionOriginal -cne 'publisher'){
      if($projection.publisherPolicyOriginal.Count -ne 0){throw 'Nonpublisher action carries publisher operands.'}
      $a.returned=$true;return $a
    }
    if($projection.publisherPolicyOriginal.Count -lt 65){throw 'Complete publisher declaration/basis/anchor operand roster required.'}
    $a.mapOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
    $a.subjectsOriginal=[object[]]@(@{},@{},@{});$a.numericOriginal=@{}
    $a.basisOriginals=[object[]]@(@{},@{},@{},@{},@{})
    $a.anchorsOriginal=[Collections.Generic.Dictionary[int,string]]::new()
    $scalars=[string[]]@('schema','policyId','timestampMode','expectedPublisherIdentity','actionGuid','providerFlags',
      'revocationChecks','uiChoice','unionChoice','notBeforeFileTime','notAfterFileTime','validationFileTime')
    foreach($row in $projection.publisherPolicyOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies publisher operand parse.'}
      if($row[0] -ceq 'scalar'){
        if($row[1] -cne '0' -or $row[2] -cnotin $scalars -or $a.mapOriginal.ContainsKey($row[2])){throw 'Unknown/duplicate publisher scalar.'}
        $a.mapOriginal.Add($row[2],$row[3])
      }elseif($row[0] -ceq 'subject'){
        if($row[1] -cnotin @('0','1','2') -or $row[2] -cnotin @('role','path','bytes','sha256')){throw 'Unknown publisher subject row.'}
        $s=$a.subjectsOriginal[[int]$row[1]]
        if($s.ContainsKey($row[2])){throw 'Duplicate publisher subject operand.'}
        $s[$row[2]]=$row[3]
      }elseif($row[0] -ceq 'basis'){
        if($row[1] -cnotin @('0','1','2','3','4')){throw 'Unknown publisher basis ordinal.'}
        $basis=$a.basisOriginals[[int]$row[1]]
        if($basis.ContainsKey($row[2])){throw 'Duplicate publisher basis operand.'}
        $basis[$row[2]]=$row[3]
      }elseif($row[0] -ceq 'anchor'){
        $ordinal=0
        if($row[1] -cnotmatch '^(0|[1-9][0-9]*)$' -or -not [int]::TryParse($row[1],[ref]$ordinal) -or
            $row[2] -cne 'certificateDerHex' -or $row[3] -cnotmatch '^(?:[0-9a-f]{2})+$' -or
            $a.anchorsOriginal.ContainsKey($ordinal)){throw 'Canonical unique accepted-anchor operand required.'}
        $a.anchorsOriginal.Add($ordinal,$row[3])
      }else{throw 'Unknown publisher operand category.'}
    }
    $d=$a.mapOriginal
    if($d.Count -ne 12 -or $d['schema'] -cne 'service-lasso.cli39-publisher-policy-operands.v1' -or
        $d['policyId'] -cne 'default-authenticode-held-file-v1' -or
        $d['timestampMode'] -cnotin @('qualified-provider-timestamp','qualified-current-policy') -or
        $d['actionGuid'] -cne '00aac56b-cd44-11d0-8cc2-00c04fc295ee'){throw 'Fixed publisher scalar policy differs.'}
    foreach($field in @('providerFlags','revocationChecks','uiChoice','unionChoice')){
      $n=[uint32]0
      if($d[$field] -cnotmatch '^(0|[1-9][0-9]*)$' -or -not [uint32]::TryParse($d[$field],[ref]$n)){throw 'Canonical DWORD publisher operand required.'}
      $a.numericOriginal[$field]=$n
    }
    if($a.numericOriginal.providerFlags -ne 0x3040 -or $a.numericOriginal.revocationChecks -ne 1 -or
        $a.numericOriginal.uiChoice -ne 2 -or $a.numericOriginal.unionChoice -ne 1){throw 'Existing default publisher flags differ.'}
    foreach($field in @('notBeforeFileTime','notAfterFileTime','validationFileTime')){
      $n=0L
      if($d[$field] -cnotmatch '^(0|[1-9][0-9]*)$' -or -not [long]::TryParse($d[$field],[ref]$n)){throw 'Canonical FILETIME publisher operand required.'}
      $a.numericOriginal[$field]=$n
    }
    if($a.numericOriginal.notBeforeFileTime -gt $a.numericOriginal.validationFileTime -or
        $a.numericOriginal.validationFileTime -gt $a.numericOriginal.notAfterFileTime){throw 'Publisher validation time outside supplied interval.'}
    $roles=[string[]]@('publisher-subject','wintrust','expected-publisher-cert')
    for($i=0;$i -lt 3;$i++){
      $s=$a.subjectsOriginal[$i];$extent=0L
      if($s.Count -ne 4 -or $s.role -cne $roles[$i] -or -not [IO.Path]::IsPathFullyQualified($s.path) -or
          $s.bytes -cnotmatch '^[1-9][0-9]*$' -or -not [long]::TryParse($s.bytes,[ref]$extent) -or
          $s.sha256 -cnotmatch '^[0-9a-fA-F]{64}$'){throw 'Fixed publisher subject operand differs.'}
    }
    $null=Read-Cli39PublisherBasisOperands32 $p $a
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies publisher operand completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}

function Read-Cli39PublisherBasisOperands32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$OperandsOriginal)
  $p=$HostOwnerOriginal;$a=$OperandsOriginal;$d=$a.mapOriginal
  $common=[string[]]@('kind','policyId','subjectSha256','expectedCertificateSha256','wintrustSha256')
  $kinds=[string[]]@('cli39.qualified-windows-os-policy.v1','cli39.qualified-windows-store-baseline.v1',
    'cli39.qualified-windows-expected-publisher.v1','cli39.qualified-windows-validation.v1','cli39.qualified-windows-revocation.v1')
  for($ordinal=0;$ordinal -lt 5;$ordinal++){
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child publisher basis validation.'}
    $v=$a.basisOriginals[$ordinal]
    $extra=switch($ordinal){
      0{[string[]]@('registrySecuritySettings','registryPolicySettings','trustPublisherSettings')}
      1{[string[]]@('storeIdentity')}
      2{[string[]]@('expectedPublisherIdentity')}
      3{[string[]]@('timestampMode','providerEvaluationPolicy','notBeforeFileTime','notAfterFileTime','validationFileTime')}
      4{[string[]]@('disposition','notBeforeFileTime','notAfterFileTime','validationFileTime','maximumFreshnessSeconds')}
    }
    $fields=$common+$extra
    if($v.Count -ne $fields.Length){throw 'Complete closed publisher basis fields required.'}
    foreach($field in $v.Keys){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or $field -cnotin $fields){throw 'Original deadline/unknown publisher basis field.'}
    }
    if($v.kind -cne $kinds[$ordinal] -or $v.policyId -cne $d['policyId'] -or
        $v.subjectSha256 -ine $a.subjectsOriginal[0].sha256 -or
        $v.wintrustSha256 -ine $a.subjectsOriginal[1].sha256 -or
        $v.expectedCertificateSha256 -ine $a.subjectsOriginal[2].sha256){throw 'Publisher basis subject/policy binding differs.'}
    $uintFields=if($ordinal -eq 0){@('registrySecuritySettings','registryPolicySettings','trustPublisherSettings')}elseif($ordinal -eq 4){@('maximumFreshnessSeconds')}else{@()}
    foreach($field in $uintFields){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies publisher basis DWORD parsing.'}
      $n=[uint32]0
      if($v[$field] -cnotmatch '^(0|[1-9][0-9]*)$' -or -not [uint32]::TryParse($v[$field],[ref]$n)){throw 'Canonical basis DWORD required.'}
      $v[$field]=$n
    }
    if($ordinal -eq 2 -and $v.expectedPublisherIdentity -cne $d['expectedPublisherIdentity']){throw 'Independent publisher identity operand differs.'}
    if($ordinal -eq 3 -and ($v.timestampMode -cne $d['timestampMode'] -or
        $v.providerEvaluationPolicy -cne 'default-provider-authenticode-evaluation.v1')){throw 'Qualified default-provider semantic operand differs.'}
    if($ordinal -eq 4 -and $v.disposition -cne 'qualified-applicable-cached-evidence.v1'){throw 'Qualified applicable cached revocation operand differs.'}
    if($ordinal -ge 3){
      foreach($field in @('notBeforeFileTime','notAfterFileTime','validationFileTime')){
        if($v[$field] -cne $d[$field]){throw 'Publisher basis FILETIME operand differs from declaration.'}
        $v[$field]=$a.numericOriginal[$field]
      }
    }
  }
  if($a.anchorsOriginal.Count -le 0 -or
      $a.projectionOriginal.publisherPolicyOriginal.Count -ne 64L+$a.anchorsOriginal.Count){throw 'Complete fixed basis plus accepted-anchor roster required.'}
  for($ordinal=0;$ordinal -lt $a.anchorsOriginal.Count;$ordinal++){
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported -or -not $a.anchorsOriginal.ContainsKey($ordinal)){throw 'Original deadline/noncontiguous accepted-anchor roster.'}
  }
  return $a
}

function Join-Cli39ChildPublisherPolicy32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($root.actionOriginal -cne 'publisher'){return}
  $s=$root.childStaticCorrespondenceOriginal;$t=$root.childDerivedTransportOriginal
  $projection=$root.staticFoundationOriginal;$operands=$projection.publisherPolicyValidationOriginal
  $selector=$root.publisherFoundationSelectorOriginal;$policy=$root.publisherPolicyOriginal
  if($root.ContainsKey('windowsPolicyFoundationTransportOriginal') -or -not $s.bindingsOriginal.returned -or
      -not $t.observationsReturned -or -not $operands.returned -or -not $selector.bindingReturned -or
      -not $s.loadedAssociationOriginal.returned -or -not $policy.constructorReturned -or
      -not $policy.constructionTerminal -or $null -ne $policy.exceptionOriginal -or
      -not [object]::ReferenceEquals($operands.rootOriginal,$root) -or
      -not [object]::ReferenceEquals($t.staticOriginal,$s) -or
      -not [object]::ReferenceEquals($t.projectionOriginal,$projection) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only live child publisher transport required.'}
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;projectionOriginal=$projection;operandsOriginal=$operands;
    parentTransportOriginal=$t;qualificationChainOriginal=$s.qualificationRootChainOriginal;
    policyOriginal=$policy;selectorOriginal=$selector;subjectRowsOriginal=$null;
    loadedOriginal=$s.loadedAssociationOriginal;namespaceOriginal=$s.namespaceRowsOriginal;
    semanticCoverageOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false;
    qualificationPremise='GENUINE_SUPPLIED_PARENT_WINDOWS_FOUNDATION_AND_TRANSPORT_QUALIFIED_EXTERNALLY';
    state='LOCAL_OPERAND_CORRESPONDENCE_NOT_POLICY_FRESHNESS_OR_USEFUL_ACCEPTANCE'}
  $root.windowsPolicyFoundationTransportOriginal=$a
  try{
    if($a.qualificationChainOriginal.qualificationRootSha256 -cne $projection.scalarValuesOriginal[1] -or
        -not [object]::ReferenceEquals($a.qualificationChainOriginal.parentProjectionTransportOriginal,$t)){
      throw 'Original qualification-root chain differs from publisher operand transport.'}
    foreach($field in @('policyId','timestampMode','expectedPublisherIdentity')){
      if($operands.mapOriginal[$field] -cne $policy.valuesOriginal[$field]){throw 'Actual child policy differs from transported publisher declaration.'}
    }
    $a.subjectRowsOriginal=[Collections.Generic.List[object]]::new(3)
    for($ordinal=0;$ordinal -lt 3;$ordinal++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies child publisher subject join.'}
      $pin=$operands.subjectsOriginal[$ordinal];$actual=$root.admittedConsumerOriginal.subjectPins[$pin.role]
      $r=@{operandOriginal=$pin;consumerPinOriginal=$actual;heldRowsOriginal=$null;
        namespaceRowsOriginal=$null;loadedRowOriginal=$null;returned=$false}
      $a.subjectRowsOriginal.Add($r)
      $r.heldRowsOriginal=[Collections.Generic.List[object]]::new()
      $r.namespaceRowsOriginal=[Collections.Generic.List[object]]::new()
      if($actual.path -cne $pin.path -or $actual.bytes.ToString([Globalization.CultureInfo]::InvariantCulture) -cne $pin.bytes -or
          $actual.sha256 -ine $pin.sha256){throw 'Child publisher consumer pin differs from transported snapshot.'}
      foreach($held in $s.memberRowsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies publisher held alias traversal.'}
        $member=$held.memberOriginal
        if($member.subjectOriginal.path -cne $pin.path){continue}
        if(-not $held.returned -or $held.identityComparedBytes -ne 24 -or
            $member.bytesExpected -ne $actual.bytes -or $member.sha256Expected -ine $pin.sha256 -or
            -not $member.subjectOriginal.hashReturned -or -not $member.subjectOriginal.eof -or
            $member.subjectOriginal.used -ne $actual.bytes){throw 'Actual child publisher held/native alias differs.'}
        $r.heldRowsOriginal.Add($held)
      }
      foreach($ns in $s.namespaceRowsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies publisher namespace traversal.'}
        if($ns.pinOriginal[0] -ceq $pin.path){
          if(-not $ns.returned){throw 'Actual publisher namespace correspondence incomplete.'}
          $r.namespaceRowsOriginal.Add($ns)
        }
      }
      if($r.heldRowsOriginal.Count -eq 0 -or $r.namespaceRowsOriginal.Count -eq 0){throw 'Actual child publisher held/namespace body absent.'}
      if($pin.role -ceq 'wintrust'){
        foreach($loadedRow in $s.loadedAssociationOriginal.rowsOriginal){
          if([object]::ReferenceEquals($loadedRow.installedMemberOriginal,$selector.wintrustMemberOriginal)){
            if($null -ne $r.loadedRowOriginal -or -not $loadedRow.returned){throw 'Duplicate/incomplete actual Wintrust loaded correspondence.'}
            $r.loadedRowOriginal=$loadedRow
          }
        }
        if($null -eq $r.loadedRowOriginal){throw 'Actual child Wintrust installed/loaded association absent.'}
      }
      $r.returned=$true
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child publisher transport completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
