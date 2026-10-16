# Fixed Node tool birth correspondence. Retained native output bytes are
# observations, never a loader, principal, source or capability trust issuer.
function Join-Cli39OuterNodeLoader32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)]$PolicyOriginal,
    [Parameter(Mandatory)][object[]]$LoaderFoundationsOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$policy=$PolicyOriginal
  $j=$root.foundationCorrespondenceOriginal
  if($root.rootRoleOriginal -cne 'qualification-coordinator' -or
      $root.actionOriginal -cnotin @('key-policy','checksum-signature') -or
      $root.ContainsKey('nodeLoaderOriginal') -or -not $j.returned -or
      -not $j.constructionTerminal -or $null -ne $j.exceptionOriginal -or
      $null -ne $root.namespaceBookOriginal.disposalOriginal -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      -not $policy.returned -or -not $policy.terminal -or
      -not [object]::ReferenceEquals($policy.rootOriginal,$root) -or
      -not [object]::ReferenceEquals($j.prechargeOriginal,
        $j.startupOriginal.StartupReservationOriginal.cliFoundationPrechargeOriginal)){
    throw 'SAME completed outer policy/foundation and preexisting collector charge required.'
  }
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline denies outer Node loader construction.'}
  # Complete collection/snapshot/identity/exception overlap is part of this
  # existing genuinely qualified collector charge, never a new quota.
  $b=@{hostOwnerOriginal=$p;rootOriginal=$root;policyOriginal=$policy;
    correspondenceOriginal=$j;prechargeOriginal=$j.prechargeOriginal;
    suppliedOriginal=$LoaderFoundationsOriginal;rowsOriginal=$null;
    returned=$false;exceptionOriginal=$null;terminal=$false;
    externalPremise='GENUINE_COMPLETE_TOOL_LOADER_AND_NAMESPACE_FACTS_QUALIFIED_BY_WHOLE_INPUT'}
  $root.nodeLoaderOriginal=$b
  try{
    $roles=if($root.actionOriginal -ceq 'checksum-signature'){@('gpg','gpgv')}else{@('gpg')}
    if($LoaderFoundationsOriginal.Length -ne $roles.Count){throw 'Exact selected tool loader roles required.'}
    $b.rowsOriginal=[Collections.Generic.List[object]]::new($roles.Count)
    for($ordinal=0;$ordinal -lt $roles.Count;$ordinal++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies tool loader snapshot.'}
      $f=$LoaderFoundationsOriginal[$ordinal]
      $o=@{suppliedOriginal=$f;roleOriginal=$roles[$ordinal];consumerOriginal=$null;
        provenanceOriginal=$null;sourceOriginal=$null;loaderSemanticsOriginal=$null;
        lifetimeOriginal=$null;namespaceOriginal=$null;dependencyPinsOriginal=$null;environmentOriginal=$null;
        workingDirectoryOriginal=$null;homeOriginal=$null;slotOriginal=$null;
        executableDomainOriginal=$null;executableOrdinalOriginal=-1;
        membersOriginal=$null;executableOriginal=$null;returned=$false}
      $b.rowsOriginal.Add($o)
      Assert-Cli39FoundationShape32 $f @('kind','role','provenanceOriginal','sourceOriginal',
        'loaderSemanticsOriginal','lifetimeOriginal','namespaceOriginal','slot','workingDirectory','home',
        'executableDomain','executableOrdinal','dependenciesOriginal','environmentOriginal')
      $o.provenanceOriginal=$f.provenanceOriginal;$o.sourceOriginal=$f.sourceOriginal
      $o.loaderSemanticsOriginal=$f.loaderSemanticsOriginal;$o.lifetimeOriginal=$f.lifetimeOriginal
      $o.namespaceOriginal=$f.namespaceOriginal;$o.slotOriginal=$f.slot
      $o.workingDirectoryOriginal=$f.workingDirectory;$o.homeOriginal=$f.home
      $o.executableDomainOriginal=$f.executableDomain;$o.executableOrdinalOriginal=$f.executableOrdinal
      $o.dependencyPinsOriginal=$f.dependenciesOriginal
      $environmentPins=$f.environmentOriginal
      $consumer=if($root.actionOriginal -ceq 'checksum-signature' -and $ordinal -eq 0){
        $root.keyPolicyConsumerShapeOriginal.prototypeOriginal
      }else{$root.consumerShapeOriginal.prototypeOriginal}
      $o.consumerOriginal=$consumer
      if($f.kind -cne 'cli39.qualified-node-tool-loader.v1' -or $f.role -cne $o.roleOriginal -or
          -not [object]::ReferenceEquals($consumer.rootCaptureOriginal,$root) -or
          $null -eq $o.provenanceOriginal -or $null -eq $o.sourceOriginal -or
          $null -eq $o.loaderSemanticsOriginal -or
          -not [object]::ReferenceEquals($o.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
          -not [object]::ReferenceEquals($o.namespaceOriginal,$j.namespaceOriginals.foundationOriginal) -or
          $o.slotOriginal -isnot [string] -or $o.slotOriginal -cne $consumer.slot -or
          $o.workingDirectoryOriginal -isnot [string] -or $o.workingDirectoryOriginal -cne $consumer.workingDirectoryOriginal -or
          $o.homeOriginal -isnot [string] -or $o.homeOriginal -cne $consumer.paths.home -or
          $o.executableDomainOriginal -cnotin @('startupClosure','loadedClosure') -or
          $o.executableOrdinalOriginal -isnot [int] -or $o.executableOrdinalOriginal -lt 0 -or
          $o.dependencyPinsOriginal -isnot [object[]] -or $o.dependencyPinsOriginal.Length -eq 0 -or
          $environmentPins -isnot [object[]] -or $environmentPins.Length -ne $consumer.environmentOriginal.Count){
        throw 'Exact qualified loader/namespace/lifetime/operation operands required.'
      }
      $o.environmentOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::OrdinalIgnoreCase)
      foreach($environmentPin in $environmentPins){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies tool environment snapshot.'}
        Assert-Cli39FoundationShape32 $environmentPin @('name','value')
        $name=$environmentPin.name;$value=$environmentPin.value
        if($name -isnot [string] -or [string]::IsNullOrEmpty($name) -or
            $name.IndexOfAny([char[]]@('=',[char]0)) -ge 0 -or $value -isnot [string] -or
            $value.Contains([char]0) -or $o.environmentOriginal.ContainsKey($name) -or
            -not $consumer.environmentOriginal.ContainsKey($name) -or
            $consumer.environmentOriginal[$name] -cne $value){throw 'Exact complete tool environment differs.'}
        $o.environmentOriginal.Add($name,$value)
      }
      $o.membersOriginal=[Collections.Generic.List[object]]::new($o.dependencyPinsOriginal.Length)
      foreach($pin in $o.dependencyPinsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies tool dependency snapshot.'}
        $r=@{pinOriginal=$pin;domainOriginal=$null;ordinalOriginal=-1;pathOriginal=$null;
          bytesOriginal=0L;sha256Original=$null;identityOriginal=$null;memberOriginal=$null;
          heldAssociationOriginal=$null;namespaceAssociationsOriginal=$null;identityCompared=0;returned=$false}
        $o.membersOriginal.Add($r)
        Assert-Cli39FoundationShape32 $pin @('domain','ordinal','path','bytes','sha256','fileIdentityOriginal')
        $r.domainOriginal=$pin.domain;$r.ordinalOriginal=$pin.ordinal;$r.pathOriginal=$pin.path
        $r.bytesOriginal=$pin.bytes;$r.sha256Original=$pin.sha256
        # Retain a source-owned copy below; subsequent projection never reads
        # mutable supplied identity arrays or declaration properties again.
        if($r.domainOriginal -cnotin @('startupClosure','loadedClosure') -or
            $r.ordinalOriginal -isnot [int] -or $r.ordinalOriginal -lt 0 -or
            $r.pathOriginal -isnot [string] -or $r.bytesOriginal -isnot [long] -or $r.bytesOriginal -lt 0 -or
            $r.sha256Original -isnot [string] -or $r.sha256Original -cnotmatch '^[A-Fa-f0-9]{64}$' -or
            $pin.fileIdentityOriginal -isnot [byte[]] -or $pin.fileIdentityOriginal.Length -ne 24){
          throw 'Fixed complete installed tool dependency pin required.'
        }
        foreach($prior in $o.membersOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies tool duplicate selector traversal.'}
          if([object]::ReferenceEquals($prior,$r)){break}
          if($prior.domainOriginal -ceq $r.domainOriginal -and $prior.ordinalOriginal -eq $r.ordinalOriginal){
            throw 'Duplicate tool dependency member selector.'
          }
        }
        $r.identityOriginal=[byte[]]::new(24)
        foreach($domain in $j.heldDomainsOriginal){foreach($held in $domain.rowsOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies held dependency traversal.'}
          $member=$held.memberOriginal
          if($member.domainOriginal -ceq $r.domainOriginal -and $member.ordinalOriginal -eq $r.ordinalOriginal){
            if($null -ne $r.memberOriginal -or -not $held.returned -or
                $member.subjectOriginal.path -cne $r.pathOriginal -or $member.bytesExpected -ne $r.bytesOriginal -or
                $member.sha256Expected -ine $r.sha256Original){throw 'Tool dependency selector/held identity differs.'}
            $r.memberOriginal=$member;$r.heldAssociationOriginal=$held
          }
        }}
        if($null -eq $r.memberOriginal){throw 'Tool dependency absent from complete actual held closure.'}
        $r.namespaceAssociationsOriginal=[Collections.Generic.List[object]]::new()
        foreach($named in $j.namespaceOriginals.rowsOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies dependency namespace traversal.'}
          if($named.ownerOriginal.pathOriginal -ceq $r.pathOriginal){
            if(-not $named.returned){throw 'Incomplete tool namespace relation.'}
            $r.namespaceAssociationsOriginal.Add($named)
          }
        }
        if($r.namespaceAssociationsOriginal.Count -eq 0){throw 'Actual held tool namespace exclusion/rights relation absent.'}
        for($i=0;$i -lt 24;$i++){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies tool native identity join.'}
          $value=$pin.fileIdentityOriginal[$i];$r.identityOriginal[$i]=$value
          if($value -ne $r.memberOriginal.fileQueryOriginal.buffersOriginal[3][$i]){
            throw 'Tool dependency full native identity differs.'
          }
          foreach($named in $r.namespaceAssociationsOriginal){
            if($value -ne $named.leafOriginal.buffersOriginal[1][$i]){throw 'Tool dependency namespace alias differs.'}
          }
          $r.identityCompared=$i+1
        }
        if($r.domainOriginal -ceq $o.executableDomainOriginal -and $r.ordinalOriginal -eq $o.executableOrdinalOriginal){
          if($null -ne $o.executableOriginal -or $r.pathOriginal -cne $consumer.subjectPins.executable.path -or
              $r.bytesOriginal -ne $consumer.subjectPins.executable.bytes -or
              $r.sha256Original -ine $consumer.subjectPins.executable.sha256){throw 'Actual selected tool executable differs.'}
          $o.executableOriginal=$r
        }
        $r.returned=$true
      }
      if($null -eq $o.executableOriginal){throw 'Executable absent from qualified complete tool dependency roster.'}
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies tool loader correspondence publication.'}
      $o.returned=$true
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies complete outer tool roster.'}
    $policy.toolLoaderCorrespondenceOriginal=$b
    $b.returned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}finally{$b.terminal=$true}
}

function Join-Cli39ChildNodeLoader32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($root.actionOriginal -cnotin @('key-policy','checksum-signature')){return}
  $s=$root.childStaticCorrespondenceOriginal;$t=$root.childDerivedTransportOriginal
  $policy=$root.nodePolicyOriginal;$operands=$root.staticFoundationOriginal.nodePolicyValidationOriginal
  if($root.ContainsKey('nodeLoaderOriginal') -or -not $s.bindingsOriginal.returned -or
      -not $t.observationsReturned -or -not $policy.returned -or -not $operands.returned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $root.namespaceBookOriginal.disposalOriginal){
    throw 'SAME live child policy/static/parent transport required for tool loader join.'
  }
  $reservation=$root.admittedInputOriginal.parentReservationOriginal;$v=$reservation.budget
  $members=0L;$environments=0L;[decimal]$backing=0
  foreach($row in $root.staticFoundationOriginal.nodePolicyOriginal){
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child loader precharge.'}
    if($row[0] -ceq 'loader' -or $row[0] -cin @('loader-gpg-member','loader-gpgv-member','loader-gpg-environment','loader-gpgv-environment')){
      foreach($value in $row){$backing+=2*[decimal]$value.Length}
      if($row[0] -cin @('loader-gpg-member','loader-gpgv-member') -and $row[2] -ceq 'domain'){$members++;$backing+=24}
      if($row[0] -cin @('loader-gpg-environment','loader-gpgv-environment') -and $row[2] -ceq 'name'){$environments++}
    }
  }
  [decimal]$charge=[decimal]$v.cliNodeLoaderBookChargeBytes+
    [decimal]$members*$v.cliNodeLoaderPerMemberChargeBytes+
    [decimal]$environments*$v.cliNodeLoaderPerEnvironmentChargeBytes+$backing*$v.cliNodeLoaderPerByteChargeBytes
  if($reservation.inputRoot -cne $root.sha256Expected -or $v.cliNodeLoaderBookChargeBytes -le 0 -or
      $v.cliNodeLoaderPerMemberChargeBytes -le 0 -or $v.cliNodeLoaderPerEnvironmentChargeBytes -le 0 -or
      $v.cliNodeLoaderPerByteChargeBytes -le 0 -or $charge -ge $v.cliNodeLoaderReservationBytes){
    throw 'Actual preexisting complete child loader subdivision required before construction.'
  }
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline denies charged child loader construction.'}
  $b=@{hostOwnerOriginal=$p;rootOriginal=$root;policyOriginal=$policy;operandsOriginal=$operands;
    reservationOriginal=$reservation;chargeBytes=$charge;backingBytes=$backing;
    memberCountOriginal=$members;environmentCountOriginal=$environments;
    parentTransportOriginal=$t;qualificationChainOriginal=$s.qualificationRootChainOriginal;
    rowsOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false;
    externalPremise='GENUINE_PARENT_TOOL_LOADER_SEMANTICS_AND_IMMUTABLE_TRANSPORT_QUALIFIED_EXTERNALLY'}
  $root.nodeLoaderOriginal=$b
  try{
    $roles=if($root.actionOriginal -ceq 'checksum-signature'){@('gpg','gpgv')}else{@('gpg')}
    $b.rowsOriginal=[Collections.Generic.List[object]]::new($roles.Count)
    $map=$operands.mapOriginal;$culture=[Globalization.CultureInfo]::InvariantCulture
    for($ordinal=0;$ordinal -lt $roles.Count;$ordinal++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies child tool loader construction.'}
      $prefix='loader:'+$ordinal.ToString($culture)+':'
      $consumer=if($root.actionOriginal -ceq 'checksum-signature' -and $ordinal -eq 0){
        $root.keyPolicyConsumerShapeOriginal.prototypeOriginal
      }else{$root.consumerShapeOriginal.prototypeOriginal}
      $o=@{roleOriginal=$roles[$ordinal];consumerOriginal=$consumer;operandsOriginal=$operands;
        parentTransportOriginal=$t;workingDirectoryOriginal=$map[$prefix+'workingDirectory'];
        homeOriginal=$map[$prefix+'home'];slotOriginal=$map[$prefix+'slot'];
        executableDomainOriginal=$map[$prefix+'executableDomain'];
        executableOrdinalOriginal=[int]::Parse($map[$prefix+'executableOrdinal'],$culture);
        membersOriginal=$null;environmentOriginal=$null;executableOriginal=$null;returned=$false}
      $b.rowsOriginal.Add($o)
      if($map[$prefix+'role'] -cne $o.roleOriginal -or $o.slotOriginal -cne $consumer.slot -or
          -not [object]::ReferenceEquals($consumer.rootCaptureOriginal,$root) -or
          $o.workingDirectoryOriginal -cne $consumer.workingDirectoryOriginal -or $o.homeOriginal -cne $consumer.paths.home){
        throw 'Actual child tool operation differs from qualified static operands.'
      }
      $dependencyCount=[int]::Parse($map[$prefix+'dependencyCount'],$culture)
      $environmentCount=[int]::Parse($map[$prefix+'environmentCount'],$culture)
      if($environmentCount -ne $consumer.environmentOriginal.Count){throw 'Actual child tool environment roster differs.'}
      $o.environmentOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::OrdinalIgnoreCase)
      for($environmentOrdinal=0;$environmentOrdinal -lt $environmentCount;$environmentOrdinal++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies child tool environment join.'}
        $environmentPrefix='loader-'+$o.roleOriginal+'-environment:'+$environmentOrdinal.ToString($culture)+':'
        $name=$map[$environmentPrefix+'name'];$value=$map[$environmentPrefix+'value']
        if($o.environmentOriginal.ContainsKey($name) -or -not $consumer.environmentOriginal.ContainsKey($name) -or
            $consumer.environmentOriginal[$name] -cne $value){throw 'Actual child tool environment differs.'}
        $o.environmentOriginal.Add($name,$value)
      }
      $o.membersOriginal=[Collections.Generic.List[object]]::new($dependencyCount)
      for($memberOrdinal=0;$memberOrdinal -lt $dependencyCount;$memberOrdinal++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies child tool member join.'}
        $memberPrefix='loader-'+$o.roleOriginal+'-member:'+$memberOrdinal.ToString($culture)+':'
        $r=@{domainOriginal=$map[$memberPrefix+'domain'];ordinalOriginal=[int]::Parse($map[$memberPrefix+'ordinal'],$culture);
          pathOriginal=$map[$memberPrefix+'path'];bytesOriginal=[long]::Parse($map[$memberPrefix+'bytes'],$culture);
          sha256Original=$map[$memberPrefix+'sha256'];identityHexOriginal=$map[$memberPrefix+'fileIdentity'];
          identityOriginal=$null;memberOriginal=$null;heldAssociationOriginal=$null;
          namespaceAssociationsOriginal=$null;identityCompared=0;returned=$false}
        $o.membersOriginal.Add($r)
        foreach($prior in $o.membersOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies child dependency duplicate traversal.'}
          if([object]::ReferenceEquals($prior,$r)){break}
          if($prior.domainOriginal -ceq $r.domainOriginal -and $prior.ordinalOriginal -eq $r.ordinalOriginal){
            throw 'Duplicate actual child tool member selector.'
          }
        }
        foreach($held in $s.memberRowsOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies child held dependency traversal.'}
          $member=$held.memberOriginal
          if($member.domainOriginal -ceq $r.domainOriginal -and $member.ordinalOriginal -eq $r.ordinalOriginal){
            if($null -ne $r.memberOriginal -or -not $held.returned -or
                $member.subjectOriginal.path -cne $r.pathOriginal -or $member.bytesExpected -ne $r.bytesOriginal -or
                $member.sha256Expected -ine $r.sha256Original){throw 'Actual child held tool dependency differs.'}
            $r.memberOriginal=$member;$r.heldAssociationOriginal=$held
          }
        }
        if($null -eq $r.memberOriginal){throw 'Actual child tool dependency absent.'}
        $r.namespaceAssociationsOriginal=[Collections.Generic.List[object]]::new()
        foreach($named in $s.namespaceRowsOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies child dependency namespace traversal.'}
          if($named.ownerOriginal.pathOriginal -ceq $r.pathOriginal){
            if(-not $named.returned){throw 'Actual child namespace alias incomplete.'}
            $r.namespaceAssociationsOriginal.Add($named)
          }
        }
        if($r.namespaceAssociationsOriginal.Count -eq 0){throw 'Actual child tool namespace relation absent.'}
        $r.identityOriginal=[byte[]]::new(24)
        for($i=0;$i -lt 24;$i++){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies child tool native identity comparison.'}
          $value=[Convert]::ToByte($r.identityHexOriginal.Substring(2*$i,2),16);$r.identityOriginal[$i]=$value
          if($value -ne $r.memberOriginal.fileQueryOriginal.buffersOriginal[3][$i]){throw 'Actual child tool full identity differs.'}
          foreach($named in $r.namespaceAssociationsOriginal){
            if($value -ne $named.leafOriginal.buffersOriginal[1][$i]){throw 'Actual child tool namespace alias identity differs.'}
          }
          $r.identityCompared=$i+1
        }
        if($r.domainOriginal -ceq $o.executableDomainOriginal -and $r.ordinalOriginal -eq $o.executableOrdinalOriginal){
          if($null -ne $o.executableOriginal -or $r.pathOriginal -cne $consumer.subjectPins.executable.path -or
              $r.bytesOriginal -ne $consumer.subjectPins.executable.bytes -or
              $r.sha256Original -ine $consumer.subjectPins.executable.sha256){throw 'Actual child executable selection differs.'}
          $o.executableOriginal=$r
        }
        $r.returned=$true
      }
      if($null -eq $o.executableOriginal){throw 'Actual child executable absent from complete tool roster.'}
      $o.returned=$true
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies child loader correspondence publication.'}
    $policy.toolLoaderCorrespondenceOriginal=$b;$b.returned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}finally{$b.terminal=$true}
}

function Join-Cli39NodePreparedLoader32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$root=$c.rootCaptureOriginal
  $loaders=$root.nodeLoaderOriginal;$identity=$c.nodePreparedIdentityOriginal;$prepared=$p.preparationOriginal
  if($c.ContainsKey('nodePreparedLoaderOriginal') -or $c.ContainsKey('disposalOriginal') -or
      $c.slot -cnotin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1') -or
      $null -eq $loaders -or -not $loaders.returned -or -not $loaders.terminal -or
      -not [object]::ReferenceEquals($loaders.policyOriginal,$c.nodePolicyOriginal) -or
      -not $identity.returned -or -not $identity.terminal -or $null -eq $prepared.commandOriginal -or
      $null -ne $root.namespaceBookOriginal.disposalOriginal){throw 'SAME complete live tool loader/identity/prepared command required.'}
  Assert-Cli39PrerequisiteUseful26 $p
  $o=@{preparationOriginal=$p;consumerOriginal=$c;rootOriginal=$root;loaderBookOriginal=$loaders;
    loaderOriginal=$null;identityOriginal=$identity;commandOriginal=$prepared.commandOriginal;
    reservationOriginal=$p.reservation;identityCompared=0;membersVisited=0;
    returned=$false;exceptionOriginal=$null;terminal=$false}
  $c.nodePreparedLoaderOriginal=$o
  try{
    foreach($loader in $loaders.rowsOriginal){
      Assert-Cli39PrerequisiteUseful26 $p
      if([object]::ReferenceEquals($loader.consumerOriginal,$c)){
        if($null -ne $o.loaderOriginal -or -not $loader.returned){throw 'Ambiguous/incomplete selected tool loader.'}
        $o.loaderOriginal=$loader
      }
    }
    $loader=$o.loaderOriginal;$command=$o.commandOriginal
    if($null -eq $loader -or $loader.slotOriginal -cne $c.slot -or
        $command.file -cne $loader.executableOriginal.pathOriginal -or
        $command.workingDirectory -cne $loader.workingDirectoryOriginal -or $command.home -cne $loader.homeOriginal -or
        -not [object]::ReferenceEquals($command.arguments,$prepared.arguments) -or
        -not [object]::ReferenceEquals($command.environment,$c.environmentOriginal) -or
        $command.outputPrefix -cne $c.outputPrefix -or $command.environment.Count -ne $loader.environmentOriginal.Count){
      throw 'Actual fixed prepared tool command differs from loader operands.'
    }
    foreach($entry in $loader.environmentOriginal.GetEnumerator()){
      Assert-Cli39PrerequisiteUseful26 $p
      if(-not $command.environment.ContainsKey($entry.Key) -or $command.environment[$entry.Key] -cne $entry.Value){
        throw 'Actual prepared tool environment differs from qualified operands.'
      }
    }
    foreach($memberRow in $loader.membersOriginal){
      Assert-Cli39PrerequisiteUseful26 $p
      $member=$memberRow.memberOriginal
      if(-not $memberRow.returned -or -not $member.returned -or $null -eq $member.subjectOriginal.stream -or
          $member.subjectOriginal.stream.SafeFileHandle.IsClosed -or -not $member.fileQueryOriginal.rawReturned -or
          -not $member.fileQueryOriginal.decodeOriginal.returned){throw 'Actual held installed dependency is no longer available.'}
      foreach($named in $memberRow.namespaceAssociationsOriginal){
        Assert-Cli39PrerequisiteUseful26 $p
        if(-not $named.returned -or $named.leafOriginal.closePhysicalReturned){throw 'Actual immutable tool namespace no longer held.'}
      }
      $o.membersVisited++
    }
    $executable=$null
    foreach($row in $identity.recordsOriginal){if($row.roleOriginal -ceq 'executable'){
      if($null -ne $executable -or -not $row.returned){throw 'Ambiguous actual prepared executable identity.'};$executable=$row
    }}
    if($null -eq $executable){throw 'Actual prepared executable identity absent.'}
    for($i=0;$i -lt 24;$i++){
      Assert-Cli39PrerequisiteUseful26 $p
      if($executable.fileQueryOriginal.buffersOriginal[3][$i] -ne $loader.executableOriginal.identityOriginal[$i]){
        throw 'Actual prepared executable differs from full installed loader identity.'
      }
      $o.identityCompared=$i+1
    }
    Assert-Cli39PrerequisiteUseful26 $p
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}

function Join-Cli39NodeProducerBirth32 {
  param([Parameter(Mandatory)]$OriginalPreparation,
    [Parameter(Mandatory)]$NativeOwnerOriginal)
  $p=$OriginalPreparation;$n=$NativeOwnerOriginal;$c=$p.consumerOriginal
  if($c.slot -cnotin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1') -or
      $c.ContainsKey('nodeProducerBirthOriginal') -or
      -not [object]::ReferenceEquals($p.nativeState,$n) -or
      -not [object]::ReferenceEquals($n.originalPreparation,$p) -or
      -not [object]::ReferenceEquals($n.consumerOriginal,$c) -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)){
    throw 'SAME once-only fixed Node producer birth relation required.'
  }
  Assert-Cli39PrerequisiteUseful26 $p
  $identity=$c.nodePreparedIdentityOriginal
  if($null -eq $identity -or -not $identity.returned -or -not $identity.terminal -or
      -not [object]::ReferenceEquals($identity.preparationOriginal,$p) -or
      -not [object]::ReferenceEquals($identity.reservationOriginal,$p.reservation)){
    throw 'SAME complete precharged Node identity/result graph required.'
  }
  $o=@{preparationOriginal=$p;nativeOwnerOriginal=$n;consumerOriginal=$c;
    identityOriginal=$identity;reservationOriginal=$p.reservation;
    commandOriginal=$p.preparationOriginal.commandOriginal;createOriginal=$null;
    createCallOriginal=$null;assignOriginal=$null;resumeOriginal=$null;
    outputOriginal=$null;processHandleObserved=[IntPtr]::Zero;threadHandleObserved=[IntPtr]::Zero;
    processIdObserved=0U;threadIdObserved=0U;returned=$false;
    exceptionOriginal=$null;terminal=$false;loaderAssociationOriginal=$c.nodePreparedLoaderOriginal}
  $c.nodeProducerBirthOriginal=$o
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    $loader=$o.loaderAssociationOriginal
    if($null -eq $loader -or -not $loader.returned -or -not $loader.terminal -or
        -not [object]::ReferenceEquals($loader.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($loader.commandOriginal,$o.commandOriginal) -or
        -not [object]::ReferenceEquals($loader.identityOriginal,$identity)){
      throw 'Actual completed SAME prepared loader/command/identity relation required.'
    }
    $o.createOriginal=$n.processCreateOriginal
    if(-not $n.calls.ContainsKey('process-create-suspended') -or
        -not $n.calls.ContainsKey('job-assign') -or -not $n.calls.ContainsKey('root-resume')){
      throw 'Actual fixed native birth call roster absent.'
    }
    $o.createCallOriginal=$n.calls['process-create-suspended']
    $o.assignOriginal=$n.calls['job-assign'].last;$o.resumeOriginal=$n.calls['root-resume'].last
    $r=$o.createOriginal
    if($null -eq $r -or -not [object]::ReferenceEquals($o.createCallOriginal.first,$r) -or
        -not [object]::ReferenceEquals($o.createCallOriginal.last,$r) -or
        $o.createCallOriginal.count -ne 1 -or $null -ne $o.createCallOriginal.failed -or
        $r.name -cne 'CreateProcessW' -or $r.role -cne 'process-create-suspended' -or
        -not $r.attempted -or -not $r.returned -or $r.value -eq 0 -or
        $null -ne $r.exception -or $r.outputLength -ne 24 -or
        $r.output -isnot [byte[]] -or $r.output.Length -lt 24){
      throw 'Actual successful retained PROCESS_INFORMATION bytes required.'
    }
    # Call-Producer copies these 24 bytes into the preowned observation frame
    # before publishing processCreateOriginal. Do not dereference its old
    # native output pointer or read mutable post-close handle slots.
    $o.outputOriginal=$r.output
    $o.processHandleObserved=[IntPtr]::new([BitConverter]::ToInt64($r.output,0))
    $o.threadHandleObserved=[IntPtr]::new([BitConverter]::ToInt64($r.output,8))
    $o.processIdObserved=[BitConverter]::ToUInt32($r.output,16)
    $o.threadIdObserved=[BitConverter]::ToUInt32($r.output,20)
    if($o.processHandleObserved -eq [IntPtr]::Zero -or $o.threadHandleObserved -eq [IntPtr]::Zero -or
        $o.processHandleObserved -eq [IntPtr]::new(-1) -or $o.threadHandleObserved -eq [IntPtr]::new(-1) -or
        $o.processIdObserved -eq 0 -or $o.threadIdObserved -eq 0 -or
        $o.processIdObserved -ne $n.pid -or -not $n.assigned -or -not $n.resumed -or
        -not $n.resumeChronologyValid){throw 'Actual successful birth/assignment/resume chronology differs.'}
    $assign=$o.assignOriginal;$resume=$o.resumeOriginal
    if($assign.name -cne 'AssignProcessToJobObject' -or $assign.role -cne 'job-assign' -or
        -not $assign.attempted -or -not $assign.returned -or $assign.value -eq 0 -or
        $null -ne $assign.exception -or $n.calls['job-assign'].count -ne 1 -or
        $null -ne $n.calls['job-assign'].failed -or
        $resume.name -cne 'ResumeThread' -or $resume.role -cne 'root-resume' -or
        -not $resume.attempted -or -not $resume.returned -or $resume.value -ne 1 -or
        $null -ne $resume.exception -or $n.calls['root-resume'].count -ne 1 -or
        $null -ne $n.calls['root-resume'].failed){throw 'Actual once-only native chronology call outcomes required.'}
    if($n.primaryPresent -or $n.unknown -or -not $n.released -or $n.exitCode -ne 0 -or
        $null -ne $n.consumerCompletionOriginal -or $null -ne $n.consumerDisposalOriginal -or
        $null -ne $n.consumerClosureOriginal -or
        -not (Test-Cli39PrerequisiteClosure26 $c.slot $p)){
      throw 'Actual complete producer/consumer physical outcomes required.'
    }
    Assert-Cli39PrerequisiteUseful26 $p
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}

function Complete-Cli39NodeUsefulResult32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$NativeOwnerOriginal)
  $p=$OriginalPreparation;$n=$NativeOwnerOriginal;$c=$p.consumerOriginal;$root=$c.rootCaptureOriginal
  if($c.ContainsKey('nodeUsefulResultOriginal') -or
      $c.slot -cnotin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1') -or
      -not [object]::ReferenceEquals($p.nativeState,$n) -or
      -not [object]::ReferenceEquals($n.originalPreparation,$p)){
    throw 'SAME once-only fixed Node useful result required.'
  }
  $o=@{preparationOriginal=$p;nativeOwnerOriginal=$n;consumerOriginal=$c;rootOriginal=$root;
    birthOriginal=$c.nodeProducerBirthOriginal;loaderOriginal=$c.nodePreparedLoaderOriginal;
    identityOriginal=$c.nodePreparedIdentityOriginal;policyOriginal=$c.nodePolicyOriginal;
    parseOriginal=$n.consumerCompletionResult;basisOriginals=$null;subjectsVisited=0;
    keyResultOriginal=$null;usefulReturned=$false;exceptionOriginal=$null;terminal=$false;
    externalPremise='GENUINE_QUALIFIED_POLICY_BASELINE_CURRENT_REVOCATION_COMPLETE_LOADER_AND_WHOLE_INPUT_REQUIRED'}
  $c.nodeUsefulResultOriginal=$o;$n.usefulConsumerResultOriginal=$o
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    $policy=$o.policyOriginal;$birth=$o.birthOriginal;$loader=$o.loaderOriginal;$identity=$o.identityOriginal
    if($null -eq $policy -or -not $policy.returned -or -not $policy.terminal -or
        -not [object]::ReferenceEquals($policy.rootOriginal,$root) -or
        $null -ne $policy.exceptionOriginal -or $policy.basisRowsOriginal.Count -ne 3 -or
        $policy.subjectRowsOriginal.Count -ne 4 -or $null -eq $policy.timePolicy.validationOriginal -or
        $null -eq $policy.timePolicy.baselineOriginal -or $null -eq $policy.timePolicy.currentRevocationOriginal -or
        $null -eq $birth -or -not $birth.returned -or -not $birth.terminal -or $null -ne $birth.exceptionOriginal -or
        -not [object]::ReferenceEquals($birth.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($birth.nativeOwnerOriginal,$n) -or
        -not [object]::ReferenceEquals($birth.loaderAssociationOriginal,$loader) -or
        -not [object]::ReferenceEquals($birth.identityOriginal,$identity) -or
        $null -eq $loader -or -not $loader.returned -or -not $loader.terminal -or
        $null -ne $loader.exceptionOriginal -or -not $loader.loaderBookOriginal.returned -or
        -not [object]::ReferenceEquals($loader.loaderBookOriginal.policyOriginal,$policy) -or
        -not $identity.returned -or -not $identity.terminal -or $null -ne $identity.exceptionOriginal){
      throw 'Complete actual policy/loader/birth/identity counterpart required.'
    }
    $o.basisOriginals=$policy.basisRowsOriginal
    foreach($subject in $identity.recordsOriginal){
      Assert-Cli39PrerequisiteUseful26 $p
      if(-not $subject.returned -or $null -ne $subject.exceptionOriginal -or
          -not $subject.subjectOriginal.eof -or -not $subject.subjectOriginal.hashReturned -or
          $subject.subjectOriginal.used -ne $c.subjectPins[$subject.roleOriginal].bytes -or
          $subject.subjectOriginal.digest -ine $c.subjectPins[$subject.roleOriginal].sha256){
        throw 'Complete actual Node subject/hash/native counterpart required.'
      }
      $o.subjectsVisited++
    }
    if($o.subjectsVisited -ne 4 -or $n.primaryPresent -or $n.unknown -or -not $n.released -or
        $n.exitCode -ne 0 -or $null -ne $n.consumerCompletionOriginal -or
        $null -ne $n.consumerDisposalOriginal -or $null -ne $n.consumerClosureOriginal -or
        -not (Test-Cli39PrerequisiteClosure26 $c.slot $p)){
      throw 'Actual whole fixed Node producer/consumer completion and physical closure required.'
    }
    $parse=$o.parseOriginal
    if($c.slot -ceq 'cli39.node-key-policy.v1'){
      if(-not [object]::ReferenceEquals($parse,$c.keyParseOriginal) -or
          $null -eq $parse -or -not $parse.availableKeyChecks -or $null -ne $parse.exception){
        throw 'Actual complete key binding/check graph required with qualified current policy basis.'
      }
    }else{
      $relation=$c.keyPolicyResultOriginal;$o.keyResultOriginal=$relation
      if(-not [object]::ReferenceEquals($parse,$c.signatureParseOriginal) -or $null -eq $parse -or
          -not $parse.cryptographicResult -or $null -ne $parse.exception -or
          $null -eq $relation -or -not $relation.usefulOriginal.usefulReturned -or
          -not [object]::ReferenceEquals($relation.usefulOriginal,$relation.consumerOriginal.nodeUsefulResultOriginal) -or
          -not [object]::ReferenceEquals($relation.policyOriginal,$policy) -or
          -not [object]::ReferenceEquals($parse.checkedKeyAssociationOriginal.relationOriginal,$relation) -or
          -not $parse.checkedKeyAssociationOriginal.returned -or
          $relation.sequenceOriginal.keyringComparedBytes -ne $c.subjectOriginals['keyring'].used){
        throw 'Actual signature/full checked key useful result and same subject relation required.'
      }
    }
    Assert-Cli39PrerequisiteUseful26 $p
    $o.usefulReturned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}
