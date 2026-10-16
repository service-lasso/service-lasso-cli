# Outer journal only: actual supplied facts remain subject to genuine complete
# input qualification. This local correspondence is neither a trust issuer nor
# child policy transport/useful acceptance.
function Join-Cli39PublisherWindowsFoundation32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$s=$p.coordinatorOriginal.startupOriginal
  Assert-Cli39QualificationStartupCustody32 $s
  $j=$root.foundationCorrespondenceOriginal;$f=$s.PolicyFoundationOriginal
  if($root.actionOriginal -cne 'publisher' -or $root.rootRoleOriginal -cne 'qualification-coordinator' -or
      $root.ContainsKey('publisherWindowsFoundationOriginal') -or -not $j.returned -or
      -not $j.constructionTerminal -or $null -ne $j.exceptionOriginal -or
      -not [object]::ReferenceEquals($j.startupOriginal,$s) -or
      -not [object]::ReferenceEquals($j.prechargeOriginal,$s.StartupReservationOriginal.cliFoundationPrechargeOriginal) -or
      -not $root.publisherFoundationSelectorOriginal.bindingReturned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME live precharged outer publisher foundation required.'}
  $o=@{hostOwnerOriginal=$p;rootOriginal=$root;startupOriginal=$s;foundationOriginal=$f;
    lifetimeOriginal=$s.StartupLifetimeOriginal;prechargeOriginal=$j.prechargeOriginal;
    correspondenceOriginal=$j;selectorOriginal=$root.publisherFoundationSelectorOriginal;
    policyOriginal=$root.publisherPolicyOriginal;declarationOriginal=$null;subjectRowsOriginal=$null;
    osPolicyOriginal=$null;storeBaselineOriginal=$null;expectedPublisherOriginal=$null;
    validationOriginal=$null;revocationOriginal=$null;sourceOriginal=$null;provenanceOriginal=$null;
    returned=$false;constructionTerminal=$false;exceptionOriginal=$null;
    externalPremise='GENUINE_QUALIFIED_SUPPLIED_WINDOWS_FOUNDATION_REQUIRED_NOT_ESTABLISHED_BY_THIS_JOURNAL';
    state='OUTER_DECLARATION_CORRESPONDENCE_CHILD_TRANSPORT_AND_CAPTURE_SEMANTICS_PENDING'}
  $root.publisherWindowsFoundationOriginal=$o
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies publisher foundation construction.'}
    Assert-Cli39FoundationShape32 $f @('kind','provenanceOriginal','lifetimeOriginal','sourceOriginal',
      'policyId','timestampMode','expectedPublisherIdentity','actionGuid','providerFlags','revocationChecks','uiChoice','unionChoice',
      'notBeforeFileTime','notAfterFileTime','validationFileTime','subjectPinsOriginal',
      'osPolicyOriginal','storeBaselineOriginal','expectedPublisherOriginal','validationOriginal','revocationOriginal')
    if($f.kind -cne 'cli39.qualified-windows-publisher-foundation.v1' -or
        -not [object]::ReferenceEquals($f.lifetimeOriginal,$o.lifetimeOriginal) -or
        $null -eq $f.provenanceOriginal -or $null -eq $f.sourceOriginal -or
        $f.subjectPinsOriginal -isnot [object[]] -or $f.subjectPinsOriginal.Length -ne 3){throw 'Complete fixed supplied Windows foundation/lifetime required.'}
    $o.provenanceOriginal=$f.provenanceOriginal;$o.sourceOriginal=$f.sourceOriginal
    # Copy all mutable scalar declarations once. Native layout/selected policy
    # correspondence below does not authenticate the supplied OS/store facts.
    $o.declarationOriginal=@{policyId=$f.policyId;timestampMode=$f.timestampMode;
      expectedPublisherIdentity=$f.expectedPublisherIdentity;actionGuid=$f.actionGuid;
      providerFlags=$f.providerFlags;revocationChecks=$f.revocationChecks;uiChoice=$f.uiChoice;unionChoice=$f.unionChoice;
      notBeforeFileTime=$f.notBeforeFileTime;notAfterFileTime=$f.notAfterFileTime;validationFileTime=$f.validationFileTime}
    $d=$o.declarationOriginal;$policy=$o.policyOriginal.valuesOriginal
    foreach($field in @('policyId','timestampMode','expectedPublisherIdentity','actionGuid')){
      if($d[$field] -isnot [string] -or [string]::IsNullOrEmpty($d[$field]) -or $d[$field].Contains([char]0)){throw 'Exact publisher declaration string required.'}
    }
    if($d.policyId -cne $policy['policyId'] -or $d.timestampMode -cne $policy['timestampMode'] -or
        $d.expectedPublisherIdentity -cne $policy['expectedPublisherIdentity'] -or
        $d.actionGuid -cne '00aac56b-cd44-11d0-8cc2-00c04fc295ee' -or
        $d.providerFlags -isnot [uint32] -or $d.providerFlags -ne 0x3040 -or
        $d.revocationChecks -isnot [uint32] -or $d.revocationChecks -ne 1 -or
        $d.uiChoice -isnot [uint32] -or $d.uiChoice -ne 2 -or
        $d.unionChoice -isnot [uint32] -or $d.unionChoice -ne 1 -or
        $d.notBeforeFileTime -isnot [long] -or $d.notAfterFileTime -isnot [long] -or
        $d.validationFileTime -isnot [long] -or $d.notBeforeFileTime -lt 0 -or
        $d.notBeforeFileTime -gt $d.validationFileTime -or $d.validationFileTime -gt $d.notAfterFileTime){throw 'Exact existing default Authenticode layout/policy/time declaration required.'}
    foreach($field in @('osPolicyOriginal','storeBaselineOriginal','expectedPublisherOriginal','validationOriginal','revocationOriginal')){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended publisher original enrollment.'}
      if($null -eq $f.$field){throw 'Actual supplied policy/store/publisher/validation/revocation original absent.'}
      $o[$field]=$f.$field
    }
    $o.subjectRowsOriginal=[Collections.Generic.List[object]]::new(3)
    $roles=[string[]]@('publisher-subject','wintrust','expected-publisher-cert')
    for($ordinal=0;$ordinal -lt 3;$ordinal++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended publisher held-subject join.'}
      $role=$roles[$ordinal];$pin=$f.subjectPinsOriginal[$ordinal];$actual=$root.consumerShapeOriginal.prototypeOriginal.subjectPins[$role]
      $r=@{roleOriginal=$role;suppliedPinOriginal=$pin;pathOriginal=$null;bytesOriginal=0L;sha256Original=$null;
        consumerPinOriginal=$actual;memberBookOriginal=$root.heldMemberBookOriginal;
        matches=0;selectorMemberOriginal=$null;returned=$false}
      $o.subjectRowsOriginal.Add($r)
      Assert-Cli39FoundationShape32 $pin @('role','path','bytes','sha256')
      $r.pathOriginal=$pin.path;$r.bytesOriginal=$pin.bytes;$r.sha256Original=$pin.sha256
      if($pin.role -cne $role -or $r.pathOriginal -isnot [string] -or $r.pathOriginal -cne $actual.path -or
          $pin.bytes -isnot [long] -or $r.bytesOriginal -ne $actual.bytes -or
          $r.sha256Original -isnot [string] -or $r.sha256Original -ine $actual.sha256){throw 'Supplied publisher pin differs from actual selected consumer.'}
      foreach($domain in $j.heldDomainsOriginal){foreach($held in $domain.rowsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended publisher held alias join.'}
        if($held.subjectOriginal.path -cne $r.pathOriginal){continue}
        if(-not $held.returned -or $held.identityBytesCompared -ne 24 -or
            $held.subjectOriginal.used -ne $r.bytesOriginal -or
            $held.memberOriginal.sha256Expected -ine $r.sha256Original -or
            -not $held.nativeQueryOriginal.decodeOriginal.returned){throw 'Actual complete publisher held/native alias differs.'}
        $r.matches++
        if(($role -ceq 'wintrust' -and [object]::ReferenceEquals($held.memberOriginal,$o.selectorOriginal.wintrustMemberOriginal)) -or
            ($role -ceq 'expected-publisher-cert' -and [object]::ReferenceEquals($held.memberOriginal,$o.selectorOriginal.expectedCertificateMemberOriginal))){$r.selectorMemberOriginal=$held.memberOriginal}
      }}
      if($r.matches -le 0 -or ($role -cne 'publisher-subject' -and $null -eq $r.selectorMemberOriginal)){throw 'Actual held publisher/Wintrust/certificate selector association absent.'}
      $r.returned=$true
    }
    $null=Join-Cli39PublisherBasisSnapshots32 $o
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline ended outer publisher journal publication.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.constructionTerminal=$true}
}
