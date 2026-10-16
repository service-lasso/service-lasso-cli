# Fixed Node verification policy correspondence. Supplied policy facts must be
# genuine qualified whole-input originals; this source creates no trust issuer.
function Join-Cli39NodeVerificationPolicy32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$PolicyFoundationOriginal,
    [Parameter(Mandatory)]$LifetimeOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$f=$PolicyFoundationOriginal
  if($root.actionOriginal -cnotin @('key-policy','checksum-signature') -or
      $root.ContainsKey('nodePolicyOriginal') -or -not $root.consumerShapeOriginal.returned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only fixed Node verification policy required.'}
  $a=@{hostOwnerOriginal=$p;rootOriginal=$root;foundationOriginal=$f;lifetimeOriginal=$LifetimeOriginal;
    consumerOriginal=$root.consumerShapeOriginal.prototypeOriginal;subjectRowsOriginal=$null;
    expectedKeyFingerprints=$null;expectedSigningFingerprints=$null;allowedPublicKeyAlgorithms=$null;
    allowedHashAlgorithms=$null;timePolicy=$null;actionOriginal=$root.actionOriginal;primaryFingerprintOriginal=$null;basisRowsOriginal=$null;toolLoaderFoundationsOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false}
  $root.nodePolicyOriginal=$a
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies Node policy construction.'}
    Assert-Cli39FoundationShape32 $f @('kind','provenanceOriginal','lifetimeOriginal','action','primaryFingerprint',
      'expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms',
      'notBefore','notAfter','validationTime','validationOriginal','baselineOriginal','currentRevocationOriginal','subjectPinsOriginal','toolLoaderOriginals')
    $a.primaryFingerprintOriginal=$f.primaryFingerprint
    $a.timePolicy=@{notBefore=$f.notBefore;notAfter=$f.notAfter;validationTime=$f.validationTime;validationOriginal=$null;baselineOriginal=$null;currentRevocationOriginal=$null}
    $a.toolLoaderFoundationsOriginal=$f.toolLoaderOriginals
    if($f.kind -cne 'cli39.qualified-node-verification-policy.v2' -or $null -eq $f.provenanceOriginal -or
        -not [object]::ReferenceEquals($f.lifetimeOriginal,$LifetimeOriginal) -or $f.action -cne $root.actionOriginal -or
        $a.primaryFingerprintOriginal -cne 'CC68F5A3106FF448322E48ED27F5E38D5B0A215F' -or
        $a.timePolicy.notBefore -isnot [long] -or $a.timePolicy.notAfter -isnot [long] -or $a.timePolicy.validationTime -isnot [long] -or
        $a.timePolicy.notBefore -lt 0 -or $a.timePolicy.notBefore -gt $a.timePolicy.validationTime -or $a.timePolicy.validationTime -gt $a.timePolicy.notAfter -or
        $null -eq $f.validationOriginal -or $null -eq $f.baselineOriginal -or $null -eq $f.currentRevocationOriginal -or
        $f.subjectPinsOriginal -isnot [object[]] -or $f.subjectPinsOriginal.Length -ne 4 -or
        $a.toolLoaderFoundationsOriginal -isnot [object[]]){throw 'Complete qualified selected Node policy/time/revocation subject originals required.'}
    foreach($field in @('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms')){
      $values=$f.$field
      if($values -isnot [string[]] -or $values.Length -eq 0){throw 'Exact nonempty qualified Node policy roster required.'}
      $a[$field]=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
      foreach($value in $values){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended Node policy construction.'}
        if(($field -cin @('expectedKeyFingerprints','expectedSigningFingerprints') -and $value -cnotmatch '^[A-F0-9]{40}$') -or
            ($field -cin @('allowedPublicKeyAlgorithms','allowedHashAlgorithms') -and $value -cnotmatch '^[1-9][0-9]*$') -or
            -not $a[$field].Add($value)){throw 'Invalid/duplicate qualified Node policy roster member.'}
      }
    }
    if(-not $a.expectedKeyFingerprints.Contains($a.primaryFingerprintOriginal) -or
        -not $a.expectedSigningFingerprints.IsSubsetOf($a.expectedKeyFingerprints)){throw 'Signing roster is outside complete qualified key roster.'}
    $a.basisRowsOriginal=[Collections.Generic.List[object]]::new(3)
    $a.subjectRowsOriginal=[Collections.Generic.List[object]]::new(4)
    $roles=[string[]]@('executable','keyring','signature','listing')
    for($ordinal=0;$ordinal -lt 4;$ordinal++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended Node policy subject correspondence.'}
      $role=$roles[$ordinal];$pin=$f.subjectPinsOriginal[$ordinal];$actual=$a.consumerOriginal.subjectPins[$role]
      $row=@{roleOriginal=$role;pinOriginal=$pin;consumerPinOriginal=$actual;role=$null;path=$null;bytes=$null;sha256=$null;returned=$false};$a.subjectRowsOriginal.Add($row)
      Assert-Cli39FoundationShape32 $pin @('role','path','bytes','sha256')
      $row.role=$pin.role;$row.path=$pin.path;$row.bytes=$pin.bytes;$row.sha256=$pin.sha256
      if($row.role -cne $role -or $row.path -isnot [string] -or $row.path -cne $actual.path -or
          $row.bytes -isnot [long] -or $row.bytes -ne $actual.bytes -or
          $row.sha256 -isnot [string] -or $row.sha256 -cne $actual.sha256){throw 'Qualified Node policy subject differs from exact selected consumer pin.'}
      $row.returned=$true
    }
    foreach($pair in @(@('validationOriginal','cli39.qualified-node-validation.v1'),
        @('baselineOriginal','cli39.qualified-node-baseline.v1'),@('currentRevocationOriginal','cli39.qualified-node-current-revocation.v1'))){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended Node policy basis correspondence.'}
      $basis=$f.($pair[0])
      $snapshot=@{basisOriginal=$basis;kind=$null;primaryFingerprint=$null;keyringSha256=$null;validationTime=$null;provenanceOriginal=$null;lifetimeOriginal=$null;sourceOriginal=$null}
      $a.basisRowsOriginal.Add($snapshot)
      Assert-Cli39FoundationShape32 $basis @('kind','provenanceOriginal','lifetimeOriginal','primaryFingerprint',
        'keyringSha256','validationTime','sourceOriginal')
      $snapshot.kind=$basis.kind;$snapshot.primaryFingerprint=$basis.primaryFingerprint;$snapshot.keyringSha256=$basis.keyringSha256
      $snapshot.validationTime=$basis.validationTime;$snapshot.provenanceOriginal=$basis.provenanceOriginal
      $snapshot.lifetimeOriginal=$basis.lifetimeOriginal;$snapshot.sourceOriginal=$basis.sourceOriginal
      if($snapshot.kind -cne $pair[1] -or $null -eq $snapshot.provenanceOriginal -or $null -eq $snapshot.sourceOriginal -or
          -not [object]::ReferenceEquals($snapshot.lifetimeOriginal,$LifetimeOriginal) -or
          $snapshot.primaryFingerprint -cne $a.primaryFingerprintOriginal -or
          $snapshot.keyringSha256 -cne $a.subjectRowsOriginal[1].sha256 -or
          $snapshot.validationTime -isnot [long] -or $snapshot.validationTime -ne $a.timePolicy.validationTime){
        throw 'Qualified baseline/current revocation/validation basis differs from exact held key/time.'
      }
    }
    $a.timePolicy.validationOriginal=$a.basisRowsOriginal[0].basisOriginal
    $a.timePolicy.baselineOriginal=$a.basisRowsOriginal[1].basisOriginal
    $a.timePolicy.currentRevocationOriginal=$a.basisRowsOriginal[2].basisOriginal
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies Node policy completion.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
