# Source-owned snapshots of the already enrolled genuine-input obligations.
# Shape/reference/value checks preserve custody; they do not authenticate facts.
function Join-Cli39PublisherBasisSnapshots32 {
  param([Parameter(Mandatory)]$FoundationOriginal)
  $o=$FoundationOriginal;$p=$o.hostOwnerOriginal;$root=$o.rootOriginal
  if(-not [object]::ReferenceEquals($root.publisherWindowsFoundationOriginal,$o) -or
      $o.ContainsKey('basisConstructionOriginal') -or $o.subjectRowsOriginal.Count -ne 3 -or
      $null -eq $o.prechargeOriginal -or $p.consumerOriginal.ContainsKey('disposalOriginal')){
    throw 'SAME once-only live precharged outer publisher basis snapshots required.'}
  $a=@{foundationOriginal=$o;prechargeOriginal=$o.prechargeOriginal;recordsOriginal=$null;
    anchorRecordsOriginal=$null;anchorBytesOriginal=0D;minimumRepresentationBytes=0D;
    returned=$false;exceptionOriginal=$null;terminal=$false;
    state='SUPPLIED_BASIS_SNAPSHOTS_NOT_GENUINE_QUALIFICATION'}
  $o.basisConstructionOriginal=$a
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies publisher basis snapshot allocation.'}
    $a.recordsOriginal=[Collections.Generic.List[object]]::new(5)
    $a.anchorRecordsOriginal=[Collections.Generic.List[object]]::new()
    $common=[string[]]@('kind','provenanceOriginal','lifetimeOriginal','sourceOriginal','policyId',
      'subjectSha256','expectedCertificateSha256','wintrustSha256')
    foreach($role in @('osPolicyOriginal','storeBaselineOriginal','expectedPublisherOriginal','validationOriginal','revocationOriginal')){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies publisher basis snapshot.'}
      $basis=$o[$role]
      $r=@{roleOriginal=$role;basisOriginal=$basis;valuesOriginal=$null;provenanceOriginal=$null;
        lifetimeOriginal=$null;sourceOriginal=$null;storeOriginal=$null;anchorsOriginal=$null;returned=$false}
      $a.recordsOriginal.Add($r)
      $extra=switch -CaseSensitive ($role){
        'osPolicyOriginal'{[string[]]@('registrySecuritySettings','registryPolicySettings','trustPublisherSettings')}
        'storeBaselineOriginal'{[string[]]@('storeIdentity','storeOriginal','anchorsOriginal')}
        'expectedPublisherOriginal'{[string[]]@('expectedPublisherIdentity')}
        'validationOriginal'{[string[]]@('timestampMode','providerEvaluationPolicy','notBeforeFileTime','notAfterFileTime','validationFileTime')}
        'revocationOriginal'{[string[]]@('disposition','notBeforeFileTime','notAfterFileTime','validationFileTime','maximumFreshnessSeconds')}
      }
      Assert-Cli39FoundationShape32 $basis ($common+$extra)
      # No supplied property is read before its complete closed NoteProperty guard.
      $r.valuesOriginal=@{}
      foreach($field in $common){
        if($field -cin @('provenanceOriginal','lifetimeOriginal','sourceOriginal')){$r[$field]=$basis.$field}
        else{$r.valuesOriginal[$field]=$basis.$field}
      }
      foreach($field in $extra){
        if($field -ceq 'storeOriginal'){$r.storeOriginal=$basis.storeOriginal}
        elseif($field -ceq 'anchorsOriginal'){$r.anchorsOriginal=$basis.anchorsOriginal}
        else{$r.valuesOriginal[$field]=$basis.$field}
      }
      $v=$r.valuesOriginal
      foreach($field in @('kind','policyId','subjectSha256','expectedCertificateSha256','wintrustSha256')){
        if($v[$field] -isnot [string]){throw 'Exact supplied basis string type required.'}
      }
      $kind=switch -CaseSensitive ($role){
        'osPolicyOriginal'{'cli39.qualified-windows-os-policy.v1'}
        'storeBaselineOriginal'{'cli39.qualified-windows-store-baseline.v1'}
        'expectedPublisherOriginal'{'cli39.qualified-windows-expected-publisher.v1'}
        'validationOriginal'{'cli39.qualified-windows-validation.v1'}
        'revocationOriginal'{'cli39.qualified-windows-revocation.v1'}
      }
      if($v.kind -cne $kind -or $null -eq $r.provenanceOriginal -or $null -eq $r.sourceOriginal -or
          -not [object]::ReferenceEquals($r.lifetimeOriginal,$o.lifetimeOriginal) -or
          $v.policyId -cne $o.declarationOriginal.policyId -or
          $v.subjectSha256 -ine $o.subjectRowsOriginal[0].sha256Original -or
          $v.wintrustSha256 -ine $o.subjectRowsOriginal[1].sha256Original -or
          $v.expectedCertificateSha256 -ine $o.subjectRowsOriginal[2].sha256Original){throw 'Supplied basis differs from SAME publisher subject/policy/lifetime.'}
      foreach($field in $v.Keys){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies publisher basis field sizing.'}
        $value=$v[$field]
        if($value -is [string]){
          if([string]::IsNullOrEmpty($value) -or $value.Contains([char]0)){throw 'Exact nonempty NUL-free basis string required.'}
          $a.minimumRepresentationBytes+=2D*$value.Length
        }
      }
      switch -CaseSensitive ($role){
        'osPolicyOriginal'{
          foreach($field in @('registrySecuritySettings','registryPolicySettings','trustPublisherSettings')){
            if($v[$field] -isnot [uint32]){throw 'Actual DWORD OS-policy declaration required.'}
          }
        }
        'storeBaselineOriginal'{
          if($v.storeIdentity -isnot [string] -or $null -eq $r.storeOriginal -or
              $r.anchorsOriginal -isnot [object[]] -or $r.anchorsOriginal.Length -eq 0){throw 'Actual qualified store identity/original/accepted-anchor set required.'}
          foreach($anchor in $r.anchorsOriginal){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline denies accepted-anchor sizing.'}
            if($anchor -isnot [byte[]] -or $anchor.Length -eq 0){throw 'Actual accepted anchor certificate byte original required.'}
            $record=@{basisSnapshotOriginal=$r;bytesOriginal=$anchor;snapshotOriginal=$null;copiedBytes=0;returned=$false}
            $a.anchorRecordsOriginal.Add($record);$a.anchorBytesOriginal+=$anchor.Length
          }
        }
        'expectedPublisherOriginal'{
          if($v.expectedPublisherIdentity -isnot [string] -or
              $v.expectedPublisherIdentity -cne $o.declarationOriginal.expectedPublisherIdentity){throw 'Independent expected publisher operand differs.'}
        }
        'validationOriginal'{
          if($v.timestampMode -isnot [string] -or $v.providerEvaluationPolicy -isnot [string] -or
              $v.timestampMode -cne $o.declarationOriginal.timestampMode -or
              $v.providerEvaluationPolicy -cne 'default-provider-authenticode-evaluation.v1'){throw 'Selected qualified default-provider validation semantics differ.'}
        }
        'revocationOriginal'{
          if($v.disposition -isnot [string] -or $v.disposition -cne 'qualified-applicable-cached-evidence.v1' -or
              $v.maximumFreshnessSeconds -isnot [uint32]){throw 'Applicable qualified cached revocation/freshness declaration required.'}
        }
      }
      if($role -cin @('validationOriginal','revocationOriginal')){
        foreach($field in @('notBeforeFileTime','notAfterFileTime','validationFileTime')){
          if($v[$field] -isnot [long] -or $v[$field] -ne $o.declarationOriginal[$field]){throw 'Supplied basis validation-time interval differs.'}
        }
      }
      $r.returned=$true
    }
    # Exact raw/clone/hex-string backing lower bound only. Complete rounded
    # object/list/exception overlap belongs to the genuine supplied measurement.
    $a.minimumRepresentationBytes+=5D*$a.anchorBytesOriginal
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported -or $a.minimumRepresentationBytes -ge $a.prechargeOriginal.chargeBytes){throw 'Basis snapshot backing cannot fit preexisting whole collector charge.'}
    foreach($record in $a.anchorRecordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies accepted-anchor snapshot allocation.'}
      $record.snapshotOriginal=[byte[]]::new($record.bytesOriginal.Length)
      for($index=0;$index -lt $record.bytesOriginal.Length;$index++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies accepted-anchor byte snapshot.'}
        $record.snapshotOriginal[$index]=$record.bytesOriginal[$index];$record.copiedBytes=$index+1
      }
      $record.returned=$true
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies basis snapshot publication.'}
    $a.returned=$true;return $a
  }catch{$a.exceptionOriginal=$_;throw}finally{$a.terminal=$true}
}
