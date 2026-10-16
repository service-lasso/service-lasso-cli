# UNRUN fixed Python consumers under the SAME original Core preparation.
# Held reports are capture evidence, never authenticated materialized trees.
function Get-Cli39PythonShape32 {
  param([Parameter(Mandatory)][string]$Slot)
  switch -CaseSensitive ($Slot) {
    'cli39.full-source-materialization.v2' {
      return @{ script='materialize-full-source26.py';
        reports=[string[]]@('decompression.json','members.json','complete.json');
        subjects=[string[]]@('executable','script','plan','archive');
        artifacts=[string[]]@('plan-original.json','decompression.json','members.json','complete.json') }
    }
    'cli39.full-source-git-comparison.v2' {
      return @{ script='compare-full-source26.py';
        reports=[string[]]@('complete-comparison.json');
        subjects=[string[]]@('executable','script','plan','commit','tree','members','reference-members');
        artifacts=[string[]]@('plan-original.json','complete-comparison.json') }
    }
    'cli39.node-keyring-preparation.v1' {
      return @{ script='prepare-node-keyring26.py';
        reports=[string[]]@('packets.json');
        subjects=[string[]]@('executable','script','plan','key');
        artifacts=[string[]]@('plan-original.json','candidate-keyring.gpg','packets.json') }
    }
    default { throw 'No generic Python prerequisite command exists.' }
  }
}

function Prepare-Cli39PythonPrerequisite32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal
  $shape=Get-Cli39PythonShape32 $c.slot
  if ($p.nativeIssued -or $null -ne $p.preparationOriginal -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)) {
    throw 'SAME enrolled original before native issue required.'
  }
  $profile=$c.rootCaptureOriginal.resultOriginal.consumerProfileOriginal
  $book=@{preparationOriginal=$p;reservationOriginal=$p.reservation;profileOriginal=$profile;
    budgetOriginal=$p.reservation.budget;shapeOriginal=$shape;plannedChargeBytes=[decimal]0;
    usedChargeBytes=[decimal]0;recordsOriginal=$null;returned=$false;exceptionOriginal=$null;
    terminalLifetime='host-process'}
  if($c.ContainsKey('pythonReportReservationOriginal')){throw 'Python report reservation cannot retry.'}
  $c.pythonReportReservationOriginal=$book
  try{
    if($null -eq $profile -or -not $profile.returned -or $profile.role -cne 'consumerBudget' -or
        -not $c.rootCaptureOriginal.profileOriginals.Contains($profile) -or
        -not [object]::ReferenceEquals($profile.valuesOriginal,$book.budgetOriginal)){
      throw 'SAME typed Python report profile/reservation required.'
    }
    $v=$book.budgetOriginal
    $book.plannedChargeBytes=[decimal]$v.pythonReportCaptureBookChargeBytes+
      [decimal]$shape.reports.Count*[decimal]$v.pythonReportCapturePerReportChargeBytes
    foreach($role in $shape.reports){
      if(-not $c.geometry.pythonOutputBytes.ContainsKey($role)){throw 'Fixed report output extent missing.'}
      $book.plannedChargeBytes+=[decimal]$c.geometry.pythonOutputBytes[$role]*[decimal]$v.pythonReportCapturePerByteChargeBytes
    }
    if($book.plannedChargeBytes -gt $v.pythonReportCaptureReservationBytes){throw 'Complete configured report overlap exceeds SAME subdivision.'}
    $book.usedChargeBytes=[decimal]$v.pythonReportCaptureBookChargeBytes
    $book.recordsOriginal=[Collections.Generic.List[object]]::new($shape.reports.Count)
    $book.returned=$true
  }catch{$book.exceptionOriginal=$_;throw}
  $capture=@{ subjects=@{}; outputs=[Collections.Generic.List[object]]::new();
    shapeOriginal=$shape; started=$true; returned=$false; exception=$null;
    commandOriginal=$null; arguments=$null }
  $p.preparationOriginal=$capture; $c.originalPreparation=$p
  $c.subjectOriginals=$capture.subjects; $c.outputOriginals=$capture.outputs
  $outputCount=2+$shape.artifacts.Count
  $phaseCount=3*$shape.subjects.Count+2*$outputCount+2+2*$shape.reports.Count
  if ($c.geometry.managedPhaseCount -ne $phaseCount -or
      $c.subjectPins.Count -ne $shape.subjects.Count) { throw 'Whole fixed Python phase/subject geometry differs.' }
  $c.managedPhases=[Collections.Generic.List[object]]::new($phaseCount)
  $capture.outputs.Capacity=$outputCount
  $c.outputRoleOriginals=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
  try {
    foreach ($role in $shape.subjects) {
      Assert-Cli39PrerequisiteUseful26 $p
      $pin=$c.subjectPins[$role]
      if ($null -eq $pin -or -not ($pin.path -is [string]) -or
          -not [IO.Path]::IsPathFullyQualified($pin.path) -or $pin.path.Contains([char]0) -or
          $pin.bytes -le 0 -or $pin.bytes -ge [int]::MaxValue -or
          $pin.sha256 -cnotmatch '^[a-f0-9]{64}$') { throw 'Exact finite held Python subject required.' }
      $record=@{ role=$role; path=$pin.path; stream=$null; handleOriginal=$null; bytes=$null;
        openAttempted=$false; openReturned=$false; hashAttempted=$false; hashReturned=$false;
        used=0; eof=$false; digest=$null; hashOriginal=$null; exception=$null }
      $capture.subjects.Add($role,$record)
      try {
        $record.bytes=[byte[]]::new([int]($pin.bytes+1))
        $worker=New-Cli39ManagedPhase26 $p 'subject-read-hash' $record
        $null=Start-Cli39ManagedPhase26 $worker
        $null=Wait-Cli39ManagedPhase26 $worker
        if (-not $record.eof -or $record.used -ne $pin.bytes) { throw 'Complete Python subject extent differs.' }
        $record.digest=[BitConverter]::ToString($record.digestOriginal).Replace('-','').ToLowerInvariant()
        if ($record.digest -cne $pin.sha256) { throw 'Complete held Python subject differs.' }
      } catch { $record.exception=$_; throw }
    }
    $script=$capture.subjects['script']; $plan=$capture.subjects['plan']
    $mapCount=1; $propertyCount=18
    if ($c.slot -ceq 'cli39.full-source-git-comparison.v2') { $mapCount=5; $propertyCount=27 }
    if ($c.slot -ceq 'cli39.node-keyring-preparation.v1') { $propertyCount=11 }
    # Every selected field plus one first rejected unknown/duplicate witness.
    $parse=@{ sourceOriginal=$plan; memoryOriginal=$null; optionsOriginal=$null;
      documentOriginal=$null; effectReturned=$false; exception=$null; fieldsOriginal=$null;
      parserCallAttempted=$false;parserCallReturned=$false;parserPartialUnknown=$false;
      parserPartialDisposition='NOT_ENTERED';
      fieldMapsOriginal=[Collections.Generic.List[object]]::new($mapCount);
      propertiesOriginal=[Collections.Generic.List[object]]::new($propertyCount) }
    $c.pythonPlanParseOriginal=$parse
    $parser=New-Cli39ManagedPhase26 $p 'plan-json-parse' $parse
    $null=Start-Cli39ManagedPhase26 $parser
    $null=Wait-Cli39ManagedPhase26 $parser
    Assert-Cli39PythonPlanAssociation32 $p
    if($c.slot -ceq 'cli39.full-source-materialization.v2'){
      $null=Reserve-Cli39PythonTree32 $p
    }
    if ([IO.Path]::GetFileName($script.path) -cne $shape.script) { throw 'Fixed selected script differs.' }
    foreach ($path in @($c.workingDirectoryOriginal,$c.outputPrefix,$c.paths.home,$c.paths.outputRoot)) {
      if (-not ($path -is [string]) -or -not [IO.Path]::IsPathFullyQualified($path) -or
          $path.Contains([char]0)) { throw 'Exact absolute original command namespace required.' }
    }
    # Environment, namespace exclusion, loaded modules and plan-to-data/output
    # association must be actual admitted native originals, not this lexical check.
    $capture.arguments=[string[]]@('-I','-S',$script.path,$plan.path,
      $plan.used.ToString([Globalization.CultureInfo]::InvariantCulture),$plan.digest)
    $capture.commandOriginal=@{ file=$capture.subjects['executable'].path;
      arguments=$capture.arguments; environment=$c.environmentOriginal;
      workingDirectory=$c.workingDirectoryOriginal; outputPrefix=$c.outputPrefix; home=$c.paths.home }
    Assert-Cli39PrerequisiteUseful26 $p
    $capture.returned=$true
    return $capture
  } catch { $capture.exception=$_; $_.Exception.Data['originalPreparation']=$p; throw }
}

function Reserve-Cli39PythonTree32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal
  if($c.slot -cne 'cli39.full-source-materialization.v2' -or $p.nativeIssued -or
      $c.ContainsKey('pythonTreeOriginal')){throw 'Fixed materialize tree reservation is once-only before native issue.'}
  $profile=$c.rootCaptureOriginal.resultOriginal.consumerProfileOriginal
  $book=@{preparationOriginal=$p;reservationOriginal=$p.reservation;
    profileOriginal=$profile;budgetOriginal=$p.reservation.budget;
    planOriginal=$c.pythonPlanParseOriginal;queryOriginal=$c.rootCaptureOriginal.nativeOutputQueryOriginal;
    maximumMembers=[decimal]0;maximumHolds=[decimal]0;maximumDirectories=[decimal]0;maximumCalls=[decimal]0;
    maximumCloseWorkers=[decimal]0;maximumDirectoryWorkers=[decimal]0;
    maximumHoldWorkers=[decimal]0;maximumByteBacking=[decimal]0;
    maximumManagedPhases=[decimal]0;maximumFileQueries=[decimal]0;
    maximumHashes=[decimal]0;hashesOriginal=$null;
    managedPhasesOriginal=$null;readsOriginal=$null;managedResourcesOriginal=$null;
    plannedChargeBytes=[decimal]0;usedChargeBytes=[decimal]0;
    directoriesOriginal=$null;returned=$false;exceptionOriginal=$null;
    terminalLifetime='host-process';state='PLANNED_CHARGE_NOT_NATIVE_TREE_CAPTURE'}
  $c.pythonTreeOriginal=$book
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    if($null -eq $profile -or -not $profile.returned -or $profile.role -cne 'consumerBudget' -or
        -not $c.rootCaptureOriginal.profileOriginals.Contains($profile) -or
        -not [object]::ReferenceEquals($profile.valuesOriginal,$book.budgetOriginal) -or
        -not $book.planOriginal.effectReturned -or $null -eq $book.planOriginal.fieldsOriginal -or
        $null -eq $book.queryOriginal -or -not $book.queryOriginal.complete -or
        -not [object]::ReferenceEquals($book.queryOriginal.hostOwnerOriginal.inputRead.value,$c.rootCaptureOriginal) -or
        -not ($c.paths.outputRoot -is [string]) -or -not [IO.Path]::IsPathFullyQualified($c.paths.outputRoot)){
      throw 'SAME complete typed profile and held parsed plan required.'
    }
    $f=$book.planOriginal.fieldsOriginal;$v=$book.budgetOriginal
    [decimal]$members=Read-Cli39PlanInteger32 $f['maxMembers']
    [decimal]$pathBytes=Read-Cli39PlanInteger32 $f['maxPathBytes']
    $book.maximumMembers=$members
    # Archive members + outputRoot/tree wrapper + five fixed output files.
    $book.maximumHolds=$members+7
    $book.maximumDirectories=$members+2
    $book.maximumCalls=4*$members+12
    $book.maximumCloseWorkers=$book.maximumDirectories
    $book.maximumDirectoryWorkers=$book.maximumCalls+$book.maximumCloseWorkers
    $book.maximumHoldWorkers=3*$book.maximumHolds
    $book.maximumManagedPhases=7*$members+3
    $book.maximumHashes=2*$members
    $book.maximumFileQueries=$members+5
    # outputRoot + /tree/ + archive path + /* + NUL; UTF8 byte bound
    # also bounds UTF16 code units. Bodies remain charged with their paths.
    $book.maximumByteBacking=2*($book.maximumDirectories+$book.maximumCalls)*([decimal]$c.paths.outputRoot.Length+9+$pathBytes)+
      [decimal](Read-Cli39PlanInteger32 $f['maxTarBytes'])+
      [decimal](Read-Cli39PlanInteger32 $f['maxPayloadBytes'])+$members*16385+
      [decimal](Read-Cli39PlanInteger32 $f['maxTarBytes'])+
      [decimal](Read-Cli39PlanInteger32 $f['maxPayloadBytes'])+$members*($pathBytes+512+25)+
      [decimal](Read-Cli39PlanInteger32 $f['maxTarBytes'])+100*$members
    # Per-directory charge includes the full independent close worker and
    # factory/directory metadata; calls count only first/next worker graphs.
    $book.plannedChargeBytes=[decimal]$v.pythonTreeCaptureBookChargeBytes+
      $book.maximumDirectories*[decimal]$v.pythonTreeCapturePerDirectoryChargeBytes+
      $book.maximumCalls*[decimal]$v.pythonTreeCapturePerEntryChargeBytes+
      $book.maximumHolds*[decimal]$v.pythonTreeCapturePerMemberChargeBytes+
      $book.maximumByteBacking*[decimal]$v.pythonTreeCapturePerByteChargeBytes
    if($book.maximumDirectories -gt [int]::MaxValue -or $book.maximumDirectoryWorkers -gt [int]::MaxValue -or
        $book.maximumHoldWorkers -gt [int]::MaxValue -or
        $book.maximumManagedPhases -gt [int]::MaxValue -or $book.maximumFileQueries -gt [int]::MaxValue -or
        $book.plannedChargeBytes -gt $v.pythonTreeCaptureReservationBytes){
      throw 'Complete tree capture geometry exceeds SAME finite subdivision.'
    }
    $book.usedChargeBytes=[decimal]$v.pythonTreeCaptureBookChargeBytes
    $book.directoriesOriginal=[Collections.Generic.List[object]]::new([int]$book.maximumDirectories)
    $book.managedPhasesOriginal=[Collections.Generic.List[object]]::new([int]$book.maximumManagedPhases)
    $book.readsOriginal=[Collections.Generic.List[object]]::new([int]$book.maximumFileQueries)
    $book.hashesOriginal=[Collections.Generic.List[object]]::new([int]$book.maximumHashes)
    $book.managedResourcesOriginal=[Collections.Generic.List[object]]::new([int](4*$members+2))
    $book.returned=$true
    return $book
  }catch{$book.exceptionOriginal=$_;throw}
}

function Read-Cli39PlanObject32 {
  param($OriginalPreparation,$Element,[string[]]$Keys)
  if ($Element.ValueKind -ne [System.Text.Json.JsonValueKind]::Object) { throw 'Fixed plan object required.' }
  $parse=$OriginalPreparation.consumerOriginal.pythonPlanParseOriginal
  if ($parse.fieldMapsOriginal.Count -ge $parse.fieldMapsOriginal.Capacity) { throw 'Fixed original plan map roster exhausted.' }
  $fields=[Collections.Generic.Dictionary[string,object]]::new($Keys.Count,[StringComparer]::Ordinal)
  $parse.fieldMapsOriginal.Add($fields)
  foreach ($property in $Element.EnumerateObject()) {
    if ($parse.propertiesOriginal.Count -ge $parse.propertiesOriginal.Capacity) { throw 'Fixed original field/witness roster exhausted.' }
    $parse.propertiesOriginal.Add($property)
    Assert-Cli39PrerequisiteUseful26 $OriginalPreparation
    if ($property.Name -cnotin $Keys -or $fields.ContainsKey($property.Name)) { throw 'Unknown or duplicate fixed plan field.' }
    $fields.Add($property.Name,$property.Value)
  }
  if ($fields.Count -ne $Keys.Count) { throw 'Incomplete fixed plan object.' }
  return ,$fields
}

function Read-Cli39PlanString32 {
  param($Element)
  if ($Element.ValueKind -ne [System.Text.Json.JsonValueKind]::String) { throw 'Fixed plan string required.' }
  return $Element.GetString()
}

function Read-Cli39PlanInteger32 {
  param($Element)
  if ($Element.ValueKind -ne [System.Text.Json.JsonValueKind]::Number) { throw 'Fixed positive integral geometry required.' }
  $raw=$Element.GetRawText()
  if ($raw -cnotmatch '^[1-9][0-9]*$') { throw 'Canonical positive integral geometry required.' }
  $value=0L
  if (-not [long]::TryParse($raw,[Globalization.NumberStyles]::None,
      [Globalization.CultureInfo]::InvariantCulture,[ref]$value) -or $value -le 0) { throw 'Finite signed-native geometry required.' }
  return $value
}

function Assert-Cli39PythonPlanAssociation32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal; $parse=$c.pythonPlanParseOriginal
  if ($null -eq $parse.documentOriginal -or -not $parse.effectReturned -or
      -not [object]::ReferenceEquals($parse.sourceOriginal,$c.subjectOriginals['plan'])) {
    throw 'SAME held original plan and actual terminal parser result required.'
  }
  switch -CaseSensitive ($c.slot) {
    'cli39.full-source-materialization.v2' {
      $keys=[string[]]@('schema','compression','archive','archiveBytes','archiveSHA256','rootName',
        'outputParent','outputLeaf','chunkBytes','xzMemoryBytes','maxTarBytes','maxMembers',
        'maxMemberBytes','maxPayloadBytes','maxPathBytes','maxArtifactBytes','milliseconds')
      $integers=[string[]]@('archiveBytes','chunkBytes','xzMemoryBytes','maxTarBytes','maxMembers',
        'maxMemberBytes','maxPayloadBytes','maxPathBytes','maxArtifactBytes','milliseconds')
    }
    'cli39.full-source-git-comparison.v2' {
      $keys=[string[]]@('schema','commit','tree','members','rootName','expectedEntries','referenceMembers',
        'referenceRootName','maxReferenceEntries','outputParent','outputLeaf','milliseconds','maxReportBytes','maxReleaseEntries')
      $integers=[string[]]@('expectedEntries','maxReferenceEntries','milliseconds','maxReportBytes','maxReleaseEntries')
    }
    'cli39.node-keyring-preparation.v1' {
      $keys=[string[]]@('schema','key','keyBytes','keySHA256','outputParent','outputLeaf',
        'maxPackets','maxPacketBytes','maxReportBytes','milliseconds')
      $integers=[string[]]@('keyBytes','maxPackets','maxPacketBytes','maxReportBytes','milliseconds')
    }
    default { throw 'No generic Python plan association exists.' }
  }
  $fields=Read-Cli39PlanObject32 $p $parse.documentOriginal.RootElement $keys
  $parse.fieldsOriginal=$fields
  foreach ($name in $integers) { Assert-Cli39PrerequisiteUseful26 $p; $null=Read-Cli39PlanInteger32 $fields[$name] }
  if ((Read-Cli39PlanString32 $fields['schema']) -cne $c.slot) { throw 'Plan schema differs from SAME fixed slot.' }
  $parent=Read-Cli39PlanString32 $fields['outputParent']; $leaf=Read-Cli39PlanString32 $fields['outputLeaf']
  if (-not [IO.Path]::IsPathFullyQualified($parent) -or $parent.Contains([char]0) -or
      [string]::IsNullOrEmpty($leaf) -or $leaf -cin @('.','..') -or $leaf.IndexOfAny([char[]]@('/','\',':',[char]0)) -ge 0 -or
      [IO.Path]::Combine($parent,$leaf) -cne $c.paths.outputRoot) { throw 'Plan output differs from SAME original output namespace.' }
  if ($c.slot -ceq 'cli39.full-source-git-comparison.v2') {
    $parse.inputPinFieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach ($pair in @(@('commit','commit'),@('tree','tree'),@('members','members'),@('referenceMembers','reference-members'))) {
      Assert-Cli39PrerequisiteUseful26 $p
      $pin=Read-Cli39PlanObject32 $p $fields[$pair[0]] ([string[]]@('path','bytes','sha256'))
      $parse.inputPinFieldsOriginal.Add($pair[0],$pin)
      $held=$c.subjectOriginals[$pair[1]]
      if ((Read-Cli39PlanString32 $pin['path']) -cne $held.path -or
          (Read-Cli39PlanInteger32 $pin['bytes']) -ne $held.used -or
          (Read-Cli39PlanString32 $pin['sha256']) -cne $held.digest) { throw 'Complete comparison plan pin differs from SAME held original.' }
    }
    if ((Read-Cli39PlanString32 $fields['rootName']) -cne 'node-v22.23.2' -or
        (Read-Cli39PlanString32 $fields['referenceRootName']) -cne 'node-aa4c77582be995286fc6e00aaf530dc7ade102a9') { throw 'Fixed complete comparison roots differ.' }
  } else {
    $role='key'; if ($c.slot -ceq 'cli39.full-source-materialization.v2') { $role='archive' }
    $held=$c.subjectOriginals[$role]
    if ((Read-Cli39PlanString32 $fields[$role]) -cne $held.path -or
        (Read-Cli39PlanInteger32 $fields[$role+'Bytes']) -ne $held.used -or
        (Read-Cli39PlanString32 $fields[$role+'SHA256']) -cne $held.digest) { throw 'Plan data pin differs from SAME held original.' }
    if ($role -ceq 'archive') {
      if ((Read-Cli39PlanString32 $fields['compression']) -cnotin @('xz','gzip')) { throw 'Fixed explicit archive route required.' }
      $root=Read-Cli39PlanString32 $fields['rootName']
      if ($root -cnotin @('node-v22.23.2','node-aa4c77582be995286fc6e00aaf530dc7ade102a9')) { throw 'Fixed complete materializer root required.' }
    }
  }
  Assert-Cli39PrerequisiteUseful26 $p
}

function Complete-Cli39PythonPrerequisite32 {
  param([Parameter(Mandatory)]$Owner)
  $c=$Owner.consumerOriginal; $p=$c.originalPreparation
  $shape=Get-Cli39PythonShape32 $c.slot
  if ($null -ne $c.pythonCompletionOriginal) { throw 'Python completion is once-only, including failures.' }
  $result=@{ started=$true; returned=$false; exception=$null;
    outputs=[Collections.Generic.List[object]]::new(2+$shape.artifacts.Count);
    reportsOriginal=$null;
    state='HELD_REPORTS_NOT_AUTHENTICATION_OR_COMPLETE_TREE_ACCEPTANCE';
    sourceAuthenticated=$false; completeTreeAccepted=$false }
  $c.pythonCompletionOriginal=$result
  try {
    $result.reportsOriginal=[Collections.Generic.List[object]]::new($shape.reports.Count)
    if ($Owner.exitCode -ne 0) { throw 'Original Python native result was not successful; partial outputs stay retained.' }
    foreach ($role in (@('python-stdout','python-stderr')+$shape.artifacts)) {
      Assert-Cli39PrerequisiteUseful26 $p
      $path=Get-Cli39PythonOutputPath32 $c $role
      $bound=$c.geometry.pythonOutputBytes[$role]
      $record=Read-Cli39VerificationOutput26 $c $role $path $bound
      $result.outputs.Add($record)
      if($role -cin $shape.reports){$null=Read-Cli39PythonReport32 $p $record}
      if ($role -ceq 'plan-original.json') {
        $original=$c.subjectOriginals['plan']
        if ($record.used -ne $original.used) { throw 'Retained original plan extent differs.' }
        for ($i=0;$i -lt $original.used;$i++) {
          Assert-Cli39PrerequisiteUseful26 $p
          if ($record.bytes[$i] -ne $original.bytes[$i]) { throw 'Retained original plan bytes differ.' }
        }
      }
    }
    Assert-Cli39PrerequisiteUseful26 $p
    if($result.reportsOriginal.Count -ne $shape.reports.Count){throw 'Complete fixed report parser roster required.'}
    foreach($report in $result.reportsOriginal){
      if(-not (Test-Cli39PythonReportOriginal32 $p $report) -or -not $report.returned){throw 'Actual complete structural report originals required.'}
    }
    if($c.slot -ceq 'cli39.full-source-materialization.v2'){
      $result.treeEnumerationOriginal=Observe-Cli39PythonTreeDirectories32 $p
      $result.treeFilesOriginal=Read-Cli39PythonTreeFiles32 $p
      $result.tarMembersOriginal=Observe-Cli39PythonTarMembers32 $p
    }
    $result.returned=$true
    return $result
  } catch { $result.exception=$_; $_.Exception.Data['originalPreparation']=$p; throw }
}

function Get-Cli39PythonOutputPath32 {
  param([Parameter(Mandatory)]$ConsumerOriginal,[Parameter(Mandatory)][string]$Role)
  $c=$ConsumerOriginal; $shape=Get-Cli39PythonShape32 $c.slot
  switch -CaseSensitive ($Role) {
    'python-stdout' { return $c.outputPrefix+'.stdout.raw' }
    'python-stderr' { return $c.outputPrefix+'.stderr.raw' }
  }
  if ($Role -cnotin $shape.artifacts) { throw 'No caller-selected Python output exists.' }
  return [IO.Path]::Combine($c.paths.outputRoot,$Role)
}
