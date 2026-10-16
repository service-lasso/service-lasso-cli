# Fixed local correspondence under the externally qualified official-host input
# precondition. Closed transport shape is not authentication. No native effects,
# callback, issuer, or provider setting is accepted through these records.
function Assert-Cli39FoundationShape32 {
  param($Original,[string[]]$Names)
  if($null -eq $Original -or $Original -is [Collections.IDictionary] -or
      $Original.GetType() -ne [Management.Automation.PSCustomObject]){
    throw 'Fixed supplied foundation transport original required, never an authority map.'
  }
  $seen=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
  foreach($property in $Original.PSObject.Properties){
    if($property.MemberType -ne [Management.Automation.PSMemberTypes]::NoteProperty -or
        $property.Name -cnotin $Names -or -not $seen.Add($property.Name)){
      throw 'Unknown/duplicate/non-data foundation property retained.'
    }
  }
  if($seen.Count -ne $Names.Length){throw 'Complete fixed foundation transport fields required.'}
}

function Join-Cli39FoundationHeldDomain32 {
  param([Parameter(Mandatory)]$CollectorOriginal,[Parameter(Mandatory)]$FoundationOriginal,
    [Parameter(Mandatory)][ValidateSet('sourceClosure','startupClosure','loadedClosure')][string]$Domain)
  $j=$CollectorOriginal;$p=$j.hostOwnerOriginal;$root=$j.rootOriginal
  Assert-Cli39FoundationShape32 $FoundationOriginal @('kind','provenanceOriginal','lifetimeOriginal','membersOriginal')
  if($FoundationOriginal.kind -cne ('cli39.qualified-held-'+$Domain+'.v1') -or
      $null -eq $FoundationOriginal.provenanceOriginal -or
      -not [object]::ReferenceEquals($FoundationOriginal.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
      $FoundationOriginal.membersOriginal -isnot [object[]]){throw 'Exact supplied held-domain transport/lifetime required.'}
  $association=@{foundationOriginal=$FoundationOriginal;domainOriginal=$Domain;rowsOriginal=$null;
    matched=0;returned=$false;exceptionOriginal=$null}
  $j.heldDomainsOriginal.Add($association)
  try{
    $association.rowsOriginal=[Collections.Generic.List[object]]::new($FoundationOriginal.membersOriginal.Length)
    foreach($member in $root.heldMemberBookOriginal.recordsOriginal){
      if($member.domainOriginal -cne $Domain){continue}
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended foundation correspondence.'}
      if($association.matched -ge $FoundationOriginal.membersOriginal.Length){throw 'Supplied foundation omits actual held member.'}
      $pin=$FoundationOriginal.membersOriginal[$association.matched]
      $row=@{pinOriginal=$pin;memberOriginal=$member;subjectOriginal=$member.subjectOriginal;
        nativeQueryOriginal=$member.fileQueryOriginal;identityBytesCompared=0;returned=$false}
      $association.rowsOriginal.Add($row)
      Assert-Cli39FoundationShape32 $pin @('domain','ordinal','path','bytes','sha256','fileIdentityOriginal')
      $subject=$row.subjectOriginal;$query=$row.nativeQueryOriginal
      if($pin.domain -cne $Domain -or $pin.ordinal -isnot [int] -or $pin.ordinal -ne $member.ordinalOriginal -or
          $pin.path -isnot [string] -or $pin.path -cne $subject.path -or
          $pin.bytes -isnot [long] -or $pin.bytes -ne $member.bytesExpected -or
          $pin.sha256 -isnot [string] -or $pin.sha256 -cne $member.sha256Expected -or
          $pin.fileIdentityOriginal -isnot [byte[]] -or $pin.fileIdentityOriginal.Length -ne 24 -or
          -not $subject.openReturned -or -not $subject.eof -or $subject.used -ne $pin.bytes -or
          -not $subject.hashReturned -or $null -eq $subject.stream -or $subject.stream.SafeFileHandle.IsClosed -or
          -not $query.rawReturned -or -not $query.decodeOriginal.returned -or -not $query.decodeOriginal.heldExtentEqual){
        throw 'Supplied complete held-domain pin differs from actual held/native member.'
      }
      # The digest was computed by the actual held reader. Keep its raw bytes
      # and full native volume/FileId backing, including zero-byte members.
      if($subject.digestOriginal -isnot [byte[]] -or $subject.digestOriginal.Length -ne 32){throw 'Actual held SHA256 result absent.'}
      for($i=0;$i -lt 32;$i++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended held digest join.'}
        $hex=$pin.sha256.Substring($i*2,2)
        if($subject.digestOriginal[$i] -ne [Convert]::ToByte($hex,16)){throw 'Actual held digest differs from qualified supplied pin.'}
      }
      for($i=0;$i -lt 24;$i++){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended held native identity join.'}
        if($pin.fileIdentityOriginal[$i] -ne $query.buffersOriginal[3][$i]){throw 'Actual held volume/FileId differs from supplied original.'}
        $row.identityBytesCompared=$i+1
      }
      $row.returned=$true;$association.matched++
    }
    if($association.matched -ne $FoundationOriginal.membersOriginal.Length){throw 'Supplied foundation contains unmatched/duplicate held member rows.'}
    $association.returned=$true;return $association
  }catch{$association.exceptionOriginal=$_;throw}
}

function Join-Cli39FoundationStartup32 {
  param($CollectorOriginal)
  $j=$CollectorOriginal;$s=$j.startupOriginal;$p=$j.hostOwnerOriginal;$root=$j.rootOriginal
  $f=$s.StartupLifetimeOriginal
  Assert-Cli39FoundationShape32 $f @('kind','hostOwnerOriginal','receiverOriginal','hostKey','absoluteDeadlineTicks',
    'rootPath','rootBytes','rootSha256','sharedSourceRoot','action','startupReservationOriginal','provenanceOriginal','hostLifetimeOriginal')
  $a=@{foundationOriginal=$f;rootOriginal=$root;startupOriginal=$s;returned=$false}
  $j.startupOriginals=$a
  if($f.kind -cne 'cli39.qualified-containing-startup.v1' -or $null -eq $f.provenanceOriginal -or
      $null -eq $f.hostLifetimeOriginal -or -not [object]::ReferenceEquals($f.hostOwnerOriginal,$p) -or
      -not [object]::ReferenceEquals($f.receiverOriginal,$p.receiver) -or
      -not [object]::ReferenceEquals($f.startupReservationOriginal,$s.StartupReservationOriginal) -or
      $f.hostKey -isnot [string] -or $f.hostKey -cne $p.key -or
      $f.absoluteDeadlineTicks -isnot [long] -or $f.absoluteDeadlineTicks -ne $p.originalDeadlineTicks -or
      $f.rootPath -isnot [string] -or $f.rootPath -cne $root.path -or
      $f.rootBytes -isnot [long] -or $f.rootBytes -ne $root.bytesExpected -or
      $f.rootSha256 -isnot [string] -or $f.rootSha256 -cne $root.sha256Expected -or
      $f.sharedSourceRoot -isnot [string] -or $f.sharedSourceRoot -cne $s.SharedSourceRootOriginal -or
      $f.action -isnot [string] -or $f.action -cne $root.actionOriginal -or
      -not $root.rootSubjectOriginal.eof -or -not $root.rootSubjectOriginal.hashReturned -or
      $root.rootSubjectOriginal.used -ne $f.rootBytes -or -not $root.rootFileQueryOriginal.rawReturned){
    throw 'Qualified supplied startup pins differ from actual containing root/owner.'
  }
  $a.returned=$true;return $a
}

function Join-Cli39FoundationNamespace32 {
  param($CollectorOriginal)
  $j=$CollectorOriginal;$p=$j.hostOwnerOriginal;$root=$j.rootOriginal;$f=$j.startupOriginal.NamespaceFoundationOriginal
  Assert-Cli39FoundationShape32 $f @('kind','provenanceOriginal','lifetimeOriginal','rowsOriginal')
  $a=@{foundationOriginal=$f;bookOriginal=$root.namespaceBookOriginal;rowsOriginal=$null;returned=$false}
  $j.namespaceOriginals=$a
  if($f.kind -cne 'cli39.qualified-namespace.v1' -or $null -eq $f.provenanceOriginal -or
      -not [object]::ReferenceEquals($f.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
      $f.rowsOriginal -isnot [object[]] -or -not $a.bookOriginal.observationReturned -or
      $null -ne $a.bookOriginal.disposalOriginal -or $f.rowsOriginal.Length -ne $a.bookOriginal.pathOwnersOriginal.Count){
    throw 'Complete supplied namespace/lifetime roster differs.'
  }
  $a.rowsOriginal=[Collections.Generic.List[object]]::new($f.rowsOriginal.Length)
  for($ordinal=0;$ordinal -lt $f.rowsOriginal.Length;$ordinal++){
    $pin=$f.rowsOriginal[$ordinal];$owner=$a.bookOriginal.pathOwnersOriginal[$ordinal]
    Assert-Cli39FoundationShape32 $pin @('path','directory','fileIdentityOriginal','exclusionOriginal','rightsOriginal')
    $row=@{pinOriginal=$pin;ownerOriginal=$owner;leafOriginal=$null;identityBytesCompared=0;returned=$false}
    $a.rowsOriginal.Add($row)
    if($pin.path -isnot [string] -or $pin.path -cne $owner.pathOriginal -or $pin.directory -isnot [bool] -or
        $pin.directory -ne $owner.leafDirectoryOriginal -or $null -eq $pin.exclusionOriginal -or
        $null -eq $pin.rightsOriginal -or $pin.fileIdentityOriginal -isnot [byte[]] -or
        $pin.fileIdentityOriginal.Length -ne 24 -or -not $owner.observationReturned -or
        ($owner.leafDirectoryOriginal -and -not $owner.directoryObserved) -or
        (-not $owner.leafDirectoryOriginal -and -not $owner.joinReturned)){throw 'Actual namespace owner differs from supplied exclusion/rights subject.'}
    foreach($hold in $owner.holdsOriginal){
      if(-not $hold.kindObserved -or -not $hold.metadataReturned -or $hold.closePhysicalReturned){throw 'Actual namespace ancestry no longer held/observed.'}
      if($hold.leafOriginal){if($null -ne $row.leafOriginal){throw 'Duplicate actual namespace leaf.'};$row.leafOriginal=$hold}
    }
    if($null -eq $row.leafOriginal){throw 'Actual namespace leaf absent.'}
    foreach($selection in @(@('exclusionOriginal','cli39.qualified-namespace-exclusion.v1'),
        @('rightsOriginal','cli39.qualified-namespace-rights.v1'))){
      $basis=$pin.($selection[0])
      Assert-Cli39FoundationShape32 $basis @('kind','provenanceOriginal','lifetimeOriginal','path','fileIdentityOriginal',
        'desiredAccess','shareMode','creationDisposition','flags','operationRole','policyOriginal')
      $expectedRole='held-file'
      if($owner.leafDirectoryOriginal){$expectedRole=$owner.directoryRoleOriginal.roleOriginal}
      if($basis.kind -cne $selection[1] -or $null -eq $basis.provenanceOriginal -or $null -eq $basis.policyOriginal -or
          -not [object]::ReferenceEquals($basis.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
          $basis.path -isnot [string] -or $basis.path -cne $owner.pathOriginal -or
          $basis.fileIdentityOriginal -isnot [byte[]] -or $basis.fileIdentityOriginal.Length -ne 24 -or
          $basis.desiredAccess -isnot [uint32] -or $basis.desiredAccess -ne 0x80 -or
          $basis.shareMode -isnot [uint32] -or $basis.shareMode -ne 1 -or
          $basis.creationDisposition -isnot [uint32] -or $basis.creationDisposition -ne 3 -or
          $basis.flags -isnot [uint32] -or $basis.flags -ne 0x02200000 -or
          $basis.operationRole -isnot [string] -or $basis.operationRole -cne $expectedRole){
        throw 'Qualified namespace exclusion/rights basis differs from exact held acquisition and fixed operation role.'
      }
      for($i=0;$i -lt 24;$i++){
        if($basis.fileIdentityOriginal[$i] -ne $row.leafOriginal.buffersOriginal[1][$i]){
          throw 'Namespace exclusion/rights policy refers to a different actual leaf.'
        }
      }
    }
    for($i=0;$i -lt 24;$i++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended namespace foundation join.'}
      if($pin.fileIdentityOriginal[$i] -ne $row.leafOriginal.buffersOriginal[1][$i]){throw 'Qualified namespace identity differs from actual held named leaf.'}
      $row.identityBytesCompared=$i+1
    }
    $row.returned=$true
  }
  $a.returned=$true;return $a
}

function Join-Cli39FoundationPrincipal32 {
  param($CollectorOriginal)
  $j=$CollectorOriginal;$p=$j.hostOwnerOriginal;$root=$j.rootOriginal;$f=$j.startupOriginal.PrincipalFoundationOriginal
  Assert-Cli39FoundationShape32 $f @('kind','provenanceOriginal','lifetimeOriginal','processId','creationTime',
    'imagePath','sidBytesOriginal','tokenId','authenticationId','tokenType')
  $process=$root.rootProcessOriginal;$image=$root.rootImageOriginal;$tokens=$root.principalModuleOriginal.tokenBookOriginal
  $a=@{foundationOriginal=$f;processOriginal=$process;imageOriginal=$image;tokenBookOriginal=$tokens;
    userOriginal=$null;statisticsOriginal=$null;sidBytesCompared=0;returned=$false}
  $j.principalOriginals=$a
  if($f.kind -cne 'cli39.qualified-containing-principal.v1' -or $null -eq $f.provenanceOriginal -or
      -not [object]::ReferenceEquals($f.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
      -not $process.rawReturned -or -not $image.rawReturned -or -not $image.pathBytesEqual -or -not $tokens.returned -or
      $f.processId -isnot [uint32] -or $f.processId -ne $process.pidOriginal[0] -or
      $f.creationTime -isnot [uint64] -or $f.creationTime -ne [BitConverter]::ToUInt64($process.timesOriginal,0) -or
      $f.imagePath -isnot [string] -or $f.imagePath -cne $root.fieldMapOriginal['hostPath'].GetString() -or
      $f.sidBytesOriginal -isnot [byte[]] -or $f.tokenId -isnot [long] -or
      $f.authenticationId -isnot [long] -or $f.tokenType -isnot [uint32]){throw 'Supplied principal differs from actual process/image observations.'}
  $a.userOriginal=$tokens.workersOriginal[0];$a.statisticsOriginal=$tokens.workersOriginal[1]
  $u=$a.userOriginal;$stats=$a.statisticsOriginal.decodedOriginal.fieldsOriginal
  if(-not $u.rawReturned -or -not $a.statisticsOriginal.rawReturned -or
      $f.sidBytesOriginal.Length -ne $u.decodedOriginal.sidExtentOriginal -or
      $f.tokenId -ne $stats['tokenIdObserved'] -or $f.authenticationId -ne $stats['authenticationIdObserved'] -or
      $f.tokenType -ne $stats['tokenTypeObserved']){throw 'Supplied principal token samples differ from actual captured fields.'}
  for($i=0;$i -lt $f.sidBytesOriginal.Length;$i++){
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline ended principal SID join.'}
    if($f.sidBytesOriginal[$i] -ne $u.backingOriginal[$u.decodedOriginal.sidOffsetOriginal+$i]){throw 'Supplied SID differs from actual retained output alias.'}
    $a.sidBytesCompared=$i+1
  }
  # Two actual samples are retained independently. Their correspondence to
  # qualified supplied facts does not turn them into an atomic token snapshot.
  $a.returned=$true;return $a
}

function Join-Cli39QualifiedMappedCoverage32 {
  param($CollectorOriginal,$LoadedRowOriginal)
  $j=$CollectorOriginal;$p=$j.hostOwnerOriginal;$row=$LoadedRowOriginal;$mapped=$row.pinOriginal.mappedOriginal
  $basis=$mapped.mappedBytesOriginal
  Assert-Cli39FoundationShape32 $basis @('kind','provenanceOriginal','lifetimeOriginal','fullPath','regionsOriginal','fileBackedImageOriginal')
  $a=@{basisOriginal=$basis;loadedRowOriginal=$row;regionsOriginal=$null;coveredBytes=0L;
    snapshotPathCompletenessOriginal=$row.observedOriginal.decodedOriginal.pathCompletenessQualified;
    fullPathBasisOriginal=$null;fileBackedImageOriginal=$null;returned=$false}
  $row.mappedCoverageOriginal=$a
  if($basis.kind -cne 'cli39.qualified-os-image-mapping.v1' -or $null -eq $basis.provenanceOriginal -or
      -not [object]::ReferenceEquals($basis.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
      $basis.fullPath -isnot [string] -or $basis.fullPath -cne $row.heldOriginal.pinOriginal.path -or
      $basis.regionsOriginal -isnot [object[]] -or $mapped.imageBytes -eq 0){
    throw 'Independent qualified full-path/OS-image mapping basis absent or inconsistent.'
  }
  $a.fullPathBasisOriginal=$basis.fullPath
  $a.regionsOriginal=[Collections.Generic.List[object]]::new($basis.regionsOriginal.Length)
  [decimal]$imageStart=$mapped.baseAddress.ToInt64();[decimal]$imageEnd=$imageStart+$mapped.imageBytes
  if($imageStart -le 0 -or $imageEnd -gt [long]::MaxValue){throw 'Actual source-selected x64 image range unsupported.'}
  foreach($query in $basis.regionsOriginal){
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline ended OS image-region join.'}
    Assert-Cli39FoundationShape32 $query @('provenanceOriginal','lifetimeOriginal','processId','creationTime',
      'queryAddressOriginal','resultOriginal','errorOriginal','bufferOriginal')
    $r=@{queryOriginal=$query;offsetOriginal=$a.coveredBytes;baseOriginal=0L;allocationBaseOriginal=0L;
      regionBytesOriginal=0UL;stateOriginal=0U;protectOriginal=0U;typeOriginal=0U;coveredBytes=0L;returned=$false}
    $a.regionsOriginal.Add($r)
    if($a.coveredBytes -ge $mapped.imageBytes -or $null -eq $query.provenanceOriginal -or
        -not [object]::ReferenceEquals($query.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
        $query.processId -isnot [uint32] -or $query.processId -ne $mapped.processId -or
        $query.creationTime -isnot [uint64] -or $query.creationTime -ne $mapped.creationTime -or
        $query.queryAddressOriginal -isnot [IntPtr] -or
        [decimal]$query.queryAddressOriginal.ToInt64() -ne ($imageStart+$a.coveredBytes) -or
        $query.resultOriginal -isnot [uint64] -or $query.resultOriginal -ne 48 -or
        $query.errorOriginal -isnot [int] -or $query.bufferOriginal -isnot [byte[]] -or $query.bufferOriginal.Length -ne 48){
      throw 'Qualified region metadata does not belong to exact actual process/image cursor.'
    }
    $raw=$query.bufferOriginal
    $r.baseOriginal=[BitConverter]::ToInt64($raw,0);$r.allocationBaseOriginal=[BitConverter]::ToInt64($raw,8)
    $r.regionBytesOriginal=[BitConverter]::ToUInt64($raw,24);$r.stateOriginal=[BitConverter]::ToUInt32($raw,32)
    $r.protectOriginal=[BitConverter]::ToUInt32($raw,36);$r.typeOriginal=[BitConverter]::ToUInt32($raw,40)
    [decimal]$cursor=$imageStart+$a.coveredBytes;[decimal]$regionEnd=[decimal]$r.baseOriginal+$r.regionBytesOriginal
    if($r.allocationBaseOriginal -ne $mapped.baseAddress.ToInt64() -or $r.regionBytesOriginal -eq 0 -or
        [decimal]$r.baseOriginal -gt $cursor -or $regionEnd -le $cursor -or $regionEnd -gt [long]::MaxValue -or
        $r.stateOriginal -notin @([uint32]0x1000,[uint32]0x2000) -or $r.typeOriginal -ne 0x1000000){
      throw 'Actual qualified region is not part of the SAME reserved/committed OS image allocation.'
    }
    $next=[Math]::Min($regionEnd,$imageEnd);$r.coveredBytes=[long]($next-$cursor)
    $a.coveredBytes+=$r.coveredBytes;$r.returned=$true
  }
  if($a.coveredBytes -ne $mapped.imageBytes){throw 'Qualified image metadata coverage has a gap or unmatched extent.'}
  $fileImage=$basis.fileBackedImageOriginal;$a.fileBackedImageOriginal=$fileImage
  Assert-Cli39FoundationShape32 $fileImage @('kind','provenanceOriginal','lifetimeOriginal','processId','creationTime',
    'allocationBase','imageBytes','regionsOriginal','installedPath','installedSha256','installedIdentityOriginal',
    'osLoaderOriginal','sectionOriginal','loaderDeclarationOriginal')
  if($fileImage.kind -cne 'cli39.qualified-os-loader-file-backed-image.v1' -or
      $null -eq $fileImage.provenanceOriginal -or $null -eq $fileImage.osLoaderOriginal -or $null -eq $fileImage.sectionOriginal -or
      -not [object]::ReferenceEquals($fileImage.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
      -not [object]::ReferenceEquals($fileImage.regionsOriginal,$basis.regionsOriginal) -or
      $fileImage.processId -isnot [uint32] -or $fileImage.processId -ne $mapped.processId -or
      $fileImage.creationTime -isnot [uint64] -or $fileImage.creationTime -ne $mapped.creationTime -or
      $fileImage.allocationBase -isnot [IntPtr] -or $fileImage.allocationBase -ne $mapped.baseAddress -or
      $fileImage.imageBytes -isnot [uint32] -or $fileImage.imageBytes -ne $mapped.imageBytes -or
      $fileImage.installedPath -isnot [string] -or $fileImage.installedPath -cne $row.heldOriginal.pinOriginal.path -or
      $fileImage.installedSha256 -isnot [string] -or $fileImage.installedSha256 -cne $row.heldOriginal.pinOriginal.sha256 -or
      $fileImage.installedIdentityOriginal -isnot [byte[]] -or $fileImage.installedIdentityOriginal.Length -ne 24){
    throw 'Qualified OS-loader/section basis is not bound to actual process, image mapping and held installed subject.'
  }
  for($i=0;$i -lt 24;$i++){
    if($fileImage.installedIdentityOriginal[$i] -ne $row.heldOriginal.nativeQueryOriginal.buffersOriginal[3][$i]){
      throw 'Qualified OS-image backing FileId differs from actual held installed original.'
    }
  }
  $null=Join-Cli39LoaderDeclaration32 $j $row
  # Native metadata and actual OS-loader/section-to-file backing are genuine
  # supplied input facts. This consumes their exact association, not authenticity
  # from nonnull/shape/flags. No page body read, protection change or PE loader is
  # required. Raw Toolhelp path completeness stays false; fullPath is independent.
  $a.returned=$true;return $a
}
function Join-Cli39FoundationLoaded32 {
  param($CollectorOriginal)
  $j=$CollectorOriginal;$p=$j.hostOwnerOriginal;$root=$j.rootOriginal;$f=$j.startupOriginal.LoadedFoundationOriginal
  Assert-Cli39FoundationShape32 $f @('kind','provenanceOriginal','lifetimeOriginal','modulesOriginal')
  $b=$root.rootModuleBookOriginal
  $a=@{foundationOriginal=$f;moduleBookOriginal=$b;rowsOriginal=$null;matched=0;returned=$false}
  $j.loadedOriginals=$a
  if($f.kind -cne 'cli39.qualified-loaded-modules.v1' -or $null -eq $f.provenanceOriginal -or
      -not [object]::ReferenceEquals($f.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
      $f.modulesOriginal -isnot [object[]] -or -not $b.returned -or -not $b.endObserved){throw 'Complete supplied loaded graph and actual EOF required.'}
  $a.rowsOriginal=[Collections.Generic.List[object]]::new($f.modulesOriginal.Length)
  foreach($observed in $b.rowsOriginal){
    if(-not $observed.moduleObserved){continue}
    if($a.matched -ge $f.modulesOriginal.Length){throw 'Actual loaded module has no supplied qualified original.'}
    $pin=$f.modulesOriginal[$a.matched];$d=$observed.decodedOriginal
    Assert-Cli39FoundationShape32 $pin @('path','baseAddress','imageBytes','installedOrdinal','mappedOriginal')
    $row=@{pinOriginal=$pin;observedOriginal=$observed;heldOriginal=$null;pathBytesCompared=0;returned=$false}
    $a.rowsOriginal.Add($row)
    if($pin.path -isnot [string] -or $pin.baseAddress -isnot [IntPtr] -or $pin.imageBytes -isnot [uint32] -or
        $pin.installedOrdinal -isnot [int] -or $pin.installedOrdinal -lt 0 -or
        $pin.installedOrdinal -ge $j.installedHeldOriginals.rowsOriginal.Count -or
        -not $d.returned -or $pin.baseAddress -ne $d.baseOriginal -or
        $pin.imageBytes -ne $d.imageBytesOriginal){throw 'Actual loaded module geometry differs from supplied original.'}
    $row.heldOriginal=$j.installedHeldOriginals.rowsOriginal[$pin.installedOrdinal]
    if($row.heldOriginal.pinOriginal.path -cne $pin.path){throw 'Loaded module is not the selected held installed member.'}
    $row.localLoadedOriginal=$null
    if(-not $root.loadedObservationOriginal.returned){throw 'Actual independent full-path/region collector required.'}
    foreach($local in $root.loadedObservationOriginal.modulesOriginal){
      if([object]::ReferenceEquals($local.snapshotRowOriginal,$observed)){
        if($null -ne $row.localLoadedOriginal){throw 'Duplicate actual local loaded correspondence.'};$row.localLoadedOriginal=$local
      }
    }
    $local=$row.localLoadedOriginal
    if($null -eq $local -or -not $local.returned -or
        -not [object]::ReferenceEquals($local.memberOriginal,$row.heldOriginal.memberOriginal) -or
        $local.pathWorkerOriginal.resultsOriginal[0] -ne $pin.path.Length){throw 'Actual independent complete loaded path/member differs.'}
    for($i=0;$i -lt $pin.path.Length;$i++){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline ended loaded module path join.'}
      if([uint16]$pin.path[$i] -ne [BitConverter]::ToUInt16($local.pathWorkerOriginal.backingOriginal,2*$i)){throw 'Actual loaded path bytes differ.'}
      $row.pathBytesCompared=2*($i+1)
    }
    $mapped=$pin.mappedOriginal
    Assert-Cli39FoundationShape32 $mapped @('provenanceOriginal','lifetimeOriginal','processId','creationTime',
      'baseAddress','imageBytes','heldFileIdentityOriginal','installedSha256','mappedBytesOriginal')
    if($null -eq $mapped.provenanceOriginal -or $null -eq $mapped.mappedBytesOriginal -or
        -not [object]::ReferenceEquals($mapped.lifetimeOriginal,$j.startupOriginal.StartupLifetimeOriginal) -or
        $mapped.processId -isnot [uint32] -or $mapped.processId -ne $root.rootProcessOriginal.pidOriginal[0] -or
        $mapped.creationTime -isnot [uint64] -or $mapped.creationTime -ne [BitConverter]::ToUInt64($root.rootProcessOriginal.timesOriginal,0) -or
        $mapped.baseAddress -isnot [IntPtr] -or $mapped.baseAddress -ne $d.baseOriginal -or
        $mapped.imageBytes -isnot [uint32] -or $mapped.imageBytes -ne $d.imageBytesOriginal -or
        $mapped.installedSha256 -isnot [string] -or $mapped.installedSha256 -cne $row.heldOriginal.pinOriginal.sha256 -or
        $mapped.heldFileIdentityOriginal -isnot [byte[]] -or $mapped.heldFileIdentityOriginal.Length -ne 24){
      throw 'Supplied qualified mapped-byte original is not bound to actual process/module/installed original.'
    }
    for($i=0;$i -lt 24;$i++){
      if($mapped.heldFileIdentityOriginal[$i] -ne $row.heldOriginal.nativeQueryOriginal.buffersOriginal[3][$i]){
        throw 'Mapped-byte provenance subject differs from actual installed held identity.'
      }
    }
    $null=Join-Cli39QualifiedMappedCoverage32 $j $row
    $row.returned=$true;$a.matched++
  }
  if($a.matched -ne $f.modulesOriginal.Length){throw 'Unmatched supplied loaded module remains.'}
  $a.returned=$true;return $a
}

function Join-Cli39FoundationCapability32 {
  param($CollectorOriginal)
  $j=$CollectorOriginal;$s=$j.startupOriginal;$root=$j.rootOriginal;$f=$s.CapabilityFoundationOriginal
  Assert-Cli39FoundationShape32 $f @('kind','provenanceOriginal','lifetimeOriginal','role','action','rootPath',
    'rootBytes','rootSha256','sharedSourceRoot','absoluteDeadlineTicks','routingOriginal','consumerOriginal','ingressOriginal')
  $a=@{foundationOriginal=$f;rootOriginal=$root;routingOriginal=$root.routingOriginal;
    consumerShapeOriginal=$root.consumerShapeOriginal;returned=$false}
  $j.capabilityOriginals=$a
  if($f.kind -cne 'cli39.qualified-fixed-capability.v1' -or $null -eq $f.provenanceOriginal -or
      $null -eq $f.ingressOriginal -or -not [object]::ReferenceEquals($f.lifetimeOriginal,$s.StartupLifetimeOriginal) -or
      $f.role -isnot [string] -or $f.role -cne $root.rootRoleOriginal -or
      $f.action -isnot [string] -or $f.action -cne $root.actionOriginal -or
      $f.rootPath -isnot [string] -or $f.rootPath -cne $root.path -or
      $f.rootBytes -isnot [long] -or $f.rootBytes -ne $root.bytesExpected -or
      $f.rootSha256 -isnot [string] -or $f.rootSha256 -cne $root.sha256Expected -or
      $f.sharedSourceRoot -isnot [string] -or $f.sharedSourceRoot -cne $s.SharedSourceRootOriginal -or
      $f.absoluteDeadlineTicks -isnot [long] -or $f.absoluteDeadlineTicks -ne $s.DeadlineTicksOriginal){
    throw 'Supplied fixed capability invocation pins differ from actual source-selected lane.'
  }
  Assert-Cli39FoundationShape32 $f.routingOriginal @('callerControlJson','hostControlJson','callerEnvironmentJson','hostEnvironmentJson',
    'callerWorkingDirectory','hostWorkingDirectory','callerHome','hostHome','callerOutputPrefix','hostOutputPrefix','entryPath','hostPath')
  $consumerFields=@('consumerJson','sourceDispositionOriginal')
  if($root.actionOriginal -ceq 'checksum-signature'){$consumerFields+='keyPolicyConsumerJson'}
  Assert-Cli39FoundationShape32 $f.consumerOriginal $consumerFields
  if($null -eq $f.consumerOriginal.sourceDispositionOriginal -or -not $root.consumerShapeOriginal.returned -or
      -not $root.routingOriginal.returned){throw 'Complete fixed routing/consumer originals required.'}
  foreach($key in @('callerControl','hostControl','callerEnvironment','hostEnvironment')){
    $expected=$f.routingOriginal.($key+'Json')
    if($expected -isnot [string] -or $expected -cne $root.fieldMapOriginal[$key].GetRawText()){
      throw 'Qualified supplied exact routing bytes differ from held root.'
    }
  }
  foreach($key in @('callerWorkingDirectory','hostWorkingDirectory','callerHome','hostHome','callerOutputPrefix','hostOutputPrefix','entryPath','hostPath')){
    if($f.routingOriginal.$key -isnot [string] -or $f.routingOriginal.$key -cne $root.fieldMapOriginal[$key].GetString()){
      throw 'Qualified supplied exact routing path differs from held root.'
    }
  }
  if($f.consumerOriginal.consumerJson -isnot [string] -or
      $f.consumerOriginal.consumerJson -cne $root.fieldMapOriginal['consumer'].GetRawText()){
    throw 'Qualified supplied exact six-slot consumer bytes differ from held root.'
  }
  if($root.actionOriginal -ceq 'checksum-signature'){
    if(-not $root.keyPolicyConsumerShapeOriginal.returned -or
        $f.consumerOriginal.keyPolicyConsumerJson -isnot [string] -or
        $f.consumerOriginal.keyPolicyConsumerJson -cne $root.fieldMapOriginal['keyPolicyConsumer'].GetRawText()){
      throw 'Qualified supplied exact auxiliary gpg consumer differs from held root.'
    }
  }
  $a.routingTransportOriginal=$f.routingOriginal;$a.consumerTransportOriginal=$f.consumerOriginal
  # The externally qualified capability binds these exact bytes and scope;
  # raw JSON equality by itself is only custody correspondence.
  $a.returned=$true;return $a
}

function Join-Cli39FoundationAllocation32 {
  param($CollectorOriginal)
  $j=$CollectorOriginal;$s=$j.startupOriginal;$root=$j.rootOriginal;$f=$s.AllocationFoundationOriginal
  Assert-Cli39FoundationShape32 $f @('kind','provenanceOriginal','lifetimeOriginal','startupReservationOriginal',
    'collectorPrechargeOriginal','rolesOriginal')
  $a=@{foundationOriginal=$f;rowsOriginal=$null;returned=$false}
  $j.allocationOriginals=$a
  $roleCount=if($root.actionOriginal -ceq 'checksum-signature'){4}else{3}
  if($f.kind -cne 'cli39.qualified-whole-allocation.v1' -or $null -eq $f.provenanceOriginal -or
      $null -eq $f.collectorPrechargeOriginal -or
      -not [object]::ReferenceEquals($f.lifetimeOriginal,$s.StartupLifetimeOriginal) -or
      -not [object]::ReferenceEquals($f.startupReservationOriginal,$s.StartupReservationOriginal) -or
      $f.rolesOriginal -isnot [object[]] -or $f.rolesOriginal.Length -ne $roleCount -or
      $root.profileOriginals.Count -ne $roleCount){throw 'Actual qualified complete startup/fixed-role allocation originals required.'}
  $a.rowsOriginal=[Collections.Generic.List[object]]::new($roleCount)
  for($ordinal=0;$ordinal -lt $roleCount;$ordinal++){
    $profile=$root.profileOriginals[$ordinal];$pin=$f.rolesOriginal[$ordinal]
    Assert-Cli39FoundationShape32 $pin @('role','valuesOriginal','reservationOriginal','measurementOriginal','aggregateOriginal')
    $row=@{pinOriginal=$pin;profileOriginal=$profile;fieldsCompared=0;returned=$false}
    $a.rowsOriginal.Add($row)
    if($pin.role -isnot [string] -or $pin.role -cne $profile.role -or -not $profile.returned -or
        $pin.valuesOriginal -isnot [Collections.Generic.Dictionary[string,object]] -or
        $pin.valuesOriginal.Count -ne $profile.valuesOriginal.Count -or $null -eq $pin.reservationOriginal -or
        $null -eq $pin.measurementOriginal -or $null -eq $pin.aggregateOriginal){throw 'Complete actual role allocation/profile originals required.'}
    foreach($key in $profile.valuesOriginal.Keys){
      Observe-OwnedPrerequisiteDeadline $j.hostOwnerOriginal
      if($j.hostOwnerOriginal.deadlineReported){throw 'Original deadline ended complete profile join.'}
      if(-not $pin.valuesOriginal.ContainsKey($key) -or $null -eq $pin.valuesOriginal[$key] -or
          $pin.valuesOriginal[$key].GetType() -ne $profile.valuesOriginal[$key].GetType() -or
          $pin.valuesOriginal[$key] -cne $profile.valuesOriginal[$key]){throw 'Qualified whole role profile differs from exact decoded values.'}
      $row.fieldsCompared++
    }
    $reservation=$pin.reservationOriginal
    if(-not [object]::ReferenceEquals($reservation.budget,$pin.valuesOriginal) -or
        $reservation.inputRoot -cne $root.sha256Expected -or
        $reservation.receiverCharge -ne $profile.valuesOriginal.receiverChargeBytes -or
        $reservation.attributeListBytes -ne $profile.valuesOriginal.attributeListBytes -or
        $reservation.outputLimitBytes -ne $profile.outputLimitBytes -or
        -not [object]::ReferenceEquals($reservation.sourceDisposition,$j.capabilityOriginals.consumerTransportOriginal.sourceDispositionOriginal)){
      throw 'Actual supplied reservation does not bind exact root/profile/source disposition.'
    }
    Assert-Cli39FoundationShape32 $pin.measurementOriginal @('kind','role','rootSha256','budgetOriginal',
      'provenanceOriginal','lifetimeOriginal','runtimeOriginal','allocatorOriginal','stackOriginal','overlapOriginal')
    $measurement=$pin.measurementOriginal
    if($measurement.kind -cne 'cli39.qualified-whole-role-measurement.v1' -or $measurement.role -cne $profile.role -or
        $measurement.rootSha256 -cne $root.sha256Expected -or
        -not [object]::ReferenceEquals($measurement.budgetOriginal,$pin.valuesOriginal) -or
        -not [object]::ReferenceEquals($measurement.lifetimeOriginal,$s.StartupLifetimeOriginal) -or
        $null -eq $measurement.provenanceOriginal -or $null -eq $measurement.runtimeOriginal -or
        $null -eq $measurement.allocatorOriginal -or $null -eq $measurement.stackOriginal -or
        -not [object]::ReferenceEquals($measurement.overlapOriginal,$pin.aggregateOriginal)){
      throw 'Qualified runtime/allocator/stack/retained-overlap measurement originals differ.'
    }
    $row.returned=$true
  }
  Assert-Cli39FoundationShape32 $f.collectorPrechargeOriginal @('kind','startupReservationOriginal','rootSha256',
    'chargeBytes','backingOriginal','measurementOriginal')
  $charge=$f.collectorPrechargeOriginal
  if($charge.kind -cne 'cli39.qualified-foundation-collector-precharge.v1' -or
      -not [object]::ReferenceEquals($charge.startupReservationOriginal,$s.StartupReservationOriginal) -or
      -not [object]::ReferenceEquals($s.StartupReservationOriginal.cliFoundationPrechargeOriginal,$charge) -or
      $charge.rootSha256 -cne $root.sha256Expected -or $charge.chargeBytes -isnot [long] -or
      $charge.chargeBytes -le 0 -or $null -eq $charge.backingOriginal -or $null -eq $charge.measurementOriginal){
    throw 'Actual preexisting complete collector charge/backing/measurement originals required.'
  }
  # This consumes a preexisting subdivision; no budget, quota or measured fit
  # is manufactured by value comparison or by this local association record.
  $a.returned=$true;return $a
}

function New-Cli39FoundationCorrespondence32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$s=$p.coordinatorOriginal.startupOriginal
  Assert-Cli39QualificationStartupCustody32 $s
  if(-not [object]::ReferenceEquals($s.HostOwnerOriginal,$p) -or
      -not $root.nativeObservationOriginal.returned -or $root.rootRoleOriginal -cne 'qualification-coordinator' -or
      $root.ContainsKey('foundationCorrespondenceOriginal') -or $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -ne $root.namespaceBookOriginal.disposalOriginal){throw 'SAME once-only live outer foundation correspondence required.'}
  # The outer startup reservation must already own this whole collector and
  # all helper/exception/partial/range backing before the first local book.
  # Parsed role budgets cannot fund or authenticate their own earlier reader.
  $precharge=$s.AllocationFoundationOriginal.collectorPrechargeOriginal
  if($null -eq $precharge -or
      -not [object]::ReferenceEquals($s.StartupReservationOriginal.cliFoundationPrechargeOriginal,$precharge) -or
      -not [object]::ReferenceEquals($precharge.startupReservationOriginal,$s.StartupReservationOriginal) -or
      $precharge.rootSha256 -cne $root.sha256Expected -or $precharge.chargeBytes -isnot [long] -or
      $precharge.chargeBytes -le 0 -or $null -eq $precharge.backingOriginal -or $null -eq $precharge.measurementOriginal){
    throw 'Actual preexisting outer collector subdivision/backing required before collector construction.'
  }
  $j=@{hostOwnerOriginal=$p;rootOriginal=$root;startupOriginal=$s;prechargeOriginal=$precharge;
    heldDomainsOriginal=$null;startupOriginals=$null;sourceOriginals=$null;namespaceOriginals=$null;
    loadedOriginals=$null;principalOriginals=$null;capabilityOriginals=$null;allocationOriginals=$null;
    constructionTerminal=$false;exceptionOriginal=$null;returned=$false;
    externalPremise='GENUINE_QUALIFIED_SUPPLIED_FOUNDATION_IS_REQUIRED_BY_ENTIRE_INPUT_ADMISSION'}
  $root.foundationCorrespondenceOriginal=$j
  try{
    $j.heldDomainsOriginal=[Collections.Generic.List[object]]::new(3)
    $null=Join-Cli39FoundationStartup32 $j
    $j.sourceOriginals=Join-Cli39FoundationHeldDomain32 $j $s.SourceFoundationOriginal 'sourceClosure'
    $j.toolHeldOriginals=Join-Cli39FoundationHeldDomain32 $j $s.ToolFoundationOriginal 'startupClosure'
    $j.installedHeldOriginals=Join-Cli39FoundationHeldDomain32 $j $s.InstalledFoundationOriginal 'loadedClosure'
    $null=Join-Cli39FoundationNamespace32 $j
    $null=Join-Cli39FoundationPrincipal32 $j
    $null=Join-Cli39FoundationLoaded32 $j
    $null=Join-Cli39FoundationCapability32 $j
    $null=Join-Cli39FoundationAllocation32 $j
    foreach($association in @($j.startupOriginals,$j.sourceOriginals,$j.toolHeldOriginals,$j.installedHeldOriginals,
        $j.namespaceOriginals,$j.loadedOriginals,$j.principalOriginals,$j.capabilityOriginals,$j.allocationOriginals)){
      if($null -eq $association -or -not $association.returned){throw 'A fixed foundation obligation remains incomplete.'}
    }
    $j.returned=$true
    # Completed correspondence remains distinct from later whole-input
    # qualification of the genuine supplied facts and useful consumer success.
    return $j
  }catch{$j.exceptionOriginal=$_;throw}finally{$j.constructionTerminal=$true}
}

function Complete-Cli39OuterFoundationBindings32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$j=$root.foundationCorrespondenceOriginal
  Assert-Cli39QualificationStartupCustody32 $p.coordinatorOriginal.startupOriginal
  if($null -eq $j -or -not $j.returned -or -not $j.constructionTerminal -or $null -ne $j.exceptionOriginal -or
      -not [object]::ReferenceEquals($j.hostOwnerOriginal,$p) -or -not [object]::ReferenceEquals($j.rootOriginal,$root) -or
      $j.ContainsKey('bindingsOriginal') -or $null -ne $root.nativeAssociationOriginals -or
      $null -ne $root.admittedInputOriginal -or $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      $null -ne $root.namespaceBookOriginal.disposalOriginal){throw 'SAME completed once-only outer correspondence required.'}
  $b=@{correspondenceOriginal=$j;hostOwnerOriginal=$p;rootOriginal=$root;reservationsOriginal=$null;
    nativeAssociationsOriginal=$null;inputOriginal=$null;returned=$false;exceptionOriginal=$null;terminal=$false}
  $j.bindingsOriginal=$b
  try{
    $b.reservationsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach($row in $j.allocationOriginals.rowsOriginal){
      $pin=$row.pinOriginal;$profile=$row.profileOriginal;$external=$pin.reservationOriginal
      if(-not $row.returned){throw 'Complete actual role allocation correspondence required.'}
      # A local binding preserves the supplied physical reservation and exact
      # decoded profile. It creates no allocator, reservation capacity or quota.
      $binding=@{inputRoot=$root.sha256Expected;sourceDisposition=$external.sourceDisposition;
        receiverCharge=$external.receiverCharge;attributeListBytes=$external.attributeListBytes;
        outputLimitBytes=$external.outputLimitBytes;budget=$profile.valuesOriginal;
        suppliedReservationOriginal=$external;profileOriginal=$profile;
        measurementOriginal=$pin.measurementOriginal;aggregateOriginal=$pin.aggregateOriginal;
        startupReservationOriginal=$j.startupOriginal.StartupReservationOriginal;
        correspondenceOriginal=$row}
      $b.reservationsOriginal.Add($profile.role,$binding)
    }
    $associations=@{hostOwnerOriginal=$p;rootOriginal=$root;correspondenceOriginal=$j;
      startupAssociationOriginal=$j.startupOriginals;sourceAssociationOriginal=$j.sourceOriginals;
      toolAssociationOriginal=$j.toolHeldOriginals;installedAssociationOriginal=$j.installedHeldOriginals;
      namespaceAssociationOriginal=$j.namespaceOriginals;loadedAssociationOriginal=$j.loadedOriginals;
      principalAssociationOriginal=$j.principalOriginals;capabilityAssociationOriginal=$j.capabilityOriginals;
      allocationAssociationOriginal=$j.allocationOriginals;
      consumerReservationOriginal=$b.reservationsOriginal['consumerBudget']}
    $b.nativeAssociationsOriginal=$associations
    if($root.actionOriginal -ceq 'checksum-signature'){$associations.keyPolicyReservationOriginal=$b.reservationsOriginal['keyPolicyBudget']}
    $input=@{rootCaptureOriginal=$root;correspondenceOriginal=$j;receiverOriginal=$p.receiver;
      role='qualification-coordinator';action=$root.actionOriginal;rootHash=$root.sha256Expected;
      principalApiModuleOriginal=$root.principalSelectorOriginal;
      parentReservationOriginal=$b.reservationsOriginal['callerBudget'];
      callerReservationOriginal=$b.reservationsOriginal['callerBudget'];
      roleReservationsOriginal=$b.reservationsOriginal;startupOriginal=$j.startupOriginal}
    $b.inputOriginal=$input
    if($root.actionOriginal -cin @('key-policy','checksum-signature')){
      $policy=$root.nodePolicyOriginal
      if($null -eq $policy -or -not $policy.returned -or -not $policy.terminal -or $null -ne $policy.exceptionOriginal -or
          -not [object]::ReferenceEquals($policy.hostOwnerOriginal,$p) -or
          -not [object]::ReferenceEquals($policy.rootOriginal,$root) -or
          -not [object]::ReferenceEquals($policy.consumerOriginal,$root.consumerShapeOriginal.prototypeOriginal)){
        throw 'Complete SAME selected Node verification policy correspondence required.'
      }
      $consumer=$policy.consumerOriginal
      $consumer.nodePolicyOriginal=$policy
      foreach($field in @('expectedKeyFingerprints','expectedSigningFingerprints','allowedPublicKeyAlgorithms','allowedHashAlgorithms','timePolicy')){
        $consumer[$field]=$policy[$field]
      }
    }
    # Publish only complete local correspondence under the selected external
    # genuine-input precondition. This is not a self-authenticating receipt.
    $root.consumerReservationOriginal=$associations.consumerReservationOriginal
    $root.admittedConsumerOriginal=$root.consumerShapeOriginal.prototypeOriginal
    $root.nativeAssociationOriginals=$associations;$root.admittedInputOriginal=$input
    $b.returned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}finally{$b.terminal=$true}
}
