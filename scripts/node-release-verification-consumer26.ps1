# UNRUN source-owned phases for the shared retained native producer owner.
# The shared owner must enroll ConsumerOriginal BEFORE preparation and invoke
# completion BEFORE receiver ACK. This file creates no competing process owner.
. (Join-Path $PSScriptRoot 'prerequisite-managed-worker26.ps1')
. (Join-Path $PSScriptRoot 'python-prerequisite-consumer32.ps1')
. (Join-Path $PSScriptRoot 'python-prerequisite-report32.ps1')
. (Join-Path $PSScriptRoot 'python-prerequisite-directory-worker32.ps1')
. (Join-Path $PSScriptRoot 'python-prerequisite-reparse32.ps1')
. (Join-Path $PSScriptRoot 'python-prerequisite-hold32.ps1')
. (Join-Path $PSScriptRoot 'python-prerequisite-tree32.ps1')
. (Join-Path $PSScriptRoot 'python-prerequisite-file32.ps1')
. (Join-Path $PSScriptRoot 'windows-publisher-worker26.ps1')
. (Join-Path $PSScriptRoot 'prerequisite-node-identity32.ps1')
. (Join-Path $PSScriptRoot 'prerequisite-node-loader32.ps1')

function Assert-Cli39PrerequisiteUseful26 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  Observe-OwnedPrerequisiteDeadline $OriginalPreparation
  if ($OriginalPreparation.deadlineReported) { throw 'Original deadline denies useful completion; all original resource custody remains owed.' }
}

function Invoke-CliPrerequisitePreparation {
  param([Parameter(Mandatory)][string]$Slot,[Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal
  if ($Slot -cin @('cli39.full-source-materialization.v2','cli39.full-source-git-comparison.v2',
      'cli39.node-keyring-preparation.v1')) {
    if ($null -eq $c -or $c.slot -cne $Slot) { throw 'Fixed Python slot differs from enrollment.' }
    return Prepare-Cli39PythonPrerequisite32 $p
  }
  if ($null -eq $c -or $c.slot -cne $Slot -or $p.nativeIssued -or $null -ne $p.preparationOriginal -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)) { throw 'Original consumer enrollment required before preparation.' }
  if ($Slot -cnotin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1')) { throw 'Unsupported fixed CLI verification slot.' }
  $capture=@{ subjects=@{}; outputs=[Collections.Generic.List[object]]::new(); arguments=$null;
    started=$true; returned=$false; exception=$null;
    state='HELD_RAW_SUBJECTS_NOT_NAMESPACE_OR_LOADED_IMAGE_AUTHORITY' }
  $p.preparationOriginal=$capture
  $c.originalPreparation=$p
  $c.subjectOriginals=$capture.subjects; $c.outputOriginals=$capture.outputs
  # Four subject read/hash effects, at most three signature outputs (two key
  # policy outputs), and one independent disposal for each stream/hash. The
  # source-derived whole phase roster is 4*3 + outputCount*2, not a retry cap.
  $outputCount=2
  if ($Slot -ceq 'cli39.node-checksum-signature.v1') { $outputCount=3 }
  $phaseCount=12+2*$outputCount
  if ($c.geometry.managedPhaseCount -ne $phaseCount) { throw 'Complete fixed phase geometry differs.' }
  $c.managedPhases=[Collections.Generic.List[object]]::new($phaseCount)
  $capture.outputs.Capacity=$outputCount
  $c.outputRoleOriginals=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal)
  try {
    foreach ($role in @('executable','keyring','signature','listing')) {
      $pin=$c.subjectPins[$role]
      if ($null -eq $pin -or -not [IO.Path]::IsPathFullyQualified($pin.path) -or
          $pin.bytes -le 0 -or $pin.bytes -ge [int]::MaxValue -or
          $pin.sha256 -cnotmatch '^[A-Fa-f0-9]{64}$') { throw 'Exact finite subject pin required.' }
      $record=@{ role=$role; path=$pin.path; stream=$null; handleOriginal=$null; bytes=$null;
        openAttempted=$false; openReturned=$false; hashAttempted=$false; hashReturned=$false;
        used=0; eof=$false; digest=$null; hashOriginal=$null; exception=$null }
      $capture.subjects.Add($role,$record)
      try {
        $record.bytes=[byte[]]::new([int]($pin.bytes+1))
        $worker=New-Cli39ManagedPhase26 $p 'subject-read-hash' $record
        $null=Start-Cli39ManagedPhase26 $worker
        $null=Wait-Cli39ManagedPhase26 $worker
        if (-not $record.eof -or $record.used -ne $pin.bytes) { throw 'Complete held subject extent differs.' }
        $record.digest=[BitConverter]::ToString($record.digestOriginal).Replace('-','')
        if ($record.digest -ine $pin.sha256) { throw 'Complete held subject bytes differ.' }
      } catch { $record.exception=$_; throw }
    }
    if ($capture.subjects.keyring.path -cne $c.paths.keyring -or
        $capture.subjects.signature.path -cne $c.paths.signature -or
        $capture.subjects.listing.path -cne $c.paths.listing) { throw 'Subject paths differ from the fixed command originals.' }
    $null=Join-Cli39NodePreparedIdentity32 $p
    $capture.arguments=Get-Cli39NodeVerificationArguments26 $c
    # The shared owner must issue this SAME enrolled command, not independent
    # File/Arguments/env/cwd/outputPrefix values selected after preparation.
    $capture.commandOriginal=@{ file=$capture.subjects.executable.path; arguments=$capture.arguments;
      environment=$c.environmentOriginal; workingDirectory=$c.workingDirectoryOriginal;
      outputPrefix=$c.outputPrefix; home=$c.paths.home }
    $null=Join-Cli39NodePreparedLoader32 $p
    Assert-Cli39PrerequisiteUseful26 $p
    $capture.returned=$true
    return $capture
  } catch { $capture.exception=$_; $_.Exception.Data['originalPreparation']=$p; throw }
}

function Invoke-CliPrerequisiteCompletion {
  param([Parameter(Mandatory)][string]$Slot,[Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $owner=$p.nativeState
  if ($null -eq $owner -or $p.consumerOriginal.slot -cne $Slot -or
      -not [object]::ReferenceEquals($owner.consumerOriginal,$p.consumerOriginal) -or
      -not [object]::ReferenceEquals($owner.originalPreparation,$p) -or $owner.released) {
    throw 'Fixed completion must retain the SAME enrolled live owner before receiver release.'
  }
  switch -CaseSensitive ($Slot) {
    'cli39.full-source-materialization.v2' { return Complete-Cli39PythonPrerequisite32 $owner }
    'cli39.full-source-git-comparison.v2' { return Complete-Cli39PythonPrerequisite32 $owner }
    'cli39.node-keyring-preparation.v1' { return Complete-Cli39PythonPrerequisite32 $owner }
    'cli39.node-checksum-signature.v1' { return Complete-Cli39NodeChecksumSignature26 $owner }
    'cli39.node-key-policy.v1' { return Complete-Cli39NodeKeyPolicy26 $owner }
    default { throw 'Unsupported fixed CLI completion slot.' }
  }
}

function Get-Cli39NodeVerificationArguments26 {
  param([Parameter(Mandatory)]$ConsumerOriginal)
  $c = $ConsumerOriginal
  $pathNames=@('home','keyring','signature','listing')
  if($c.slot -ceq 'cli39.node-checksum-signature.v1'){$pathNames+='payload'}
  foreach ($name in $pathNames) {
    $value = $c.paths[$name]
    if (-not ($value -is [string]) -or -not [IO.Path]::IsPathFullyQualified($value) -or $value.Contains([char]0)) {
      throw 'Exact absolute source-owned verification subject is required.'
    }
  }
  switch -CaseSensitive ($c.slot) {
    'cli39.node-key-policy.v1' {
      # Complete available self-signature/subkey/revocation checks, no ambient
      # keyring/config/keyserver/agent. Current authenticated revocation-input
      # freshness is a SEPARATE policy relation, never inferred from this exit.
      return [string[]]@('--no-options','--batch','--no-tty','--no-auto-key-retrieve',
        '--no-auto-key-import','--no-autostart','--no-auto-check-trustdb','--no-sig-cache',
        '--homedir',$c.paths.home,'--no-default-keyring','--keyring',$c.paths.keyring,
        '--with-colons','--fixed-list-mode','--with-fingerprint','--with-fingerprint',
        '--status-fd','2','--check-sigs','CC68F5A3106FF448322E48ED27F5E38D5B0A215F')
    }
    'cli39.node-checksum-signature.v1' {
      # Explicit keyring suppresses gpgv's default trusted keyring. Output is a
      # fresh private source-owned file; no stdout/status/payload multiplexing.
      return [string[]]@('--homedir',$c.paths.home,'--keyring',$c.paths.keyring,
        '--weak-digest','SHA1','--status-fd','1','--output',$c.paths.payload,
        '--',$c.paths.signature)
    }
    default { throw 'No generic verification command slot exists.' }
  }
}

function Read-Cli39VerificationOutput26 {
  param([Parameter(Mandatory)]$ConsumerOriginal,[Parameter(Mandatory)][string]$Role,
    [Parameter(Mandatory)][string]$Path,[Parameter(Mandatory)][long]$MaximumBytes)
  $c = $ConsumerOriginal
  if ($MaximumBytes -le 0 -or $MaximumBytes -ge [int]::MaxValue) { throw 'Finite admitted output geometry is required.' }
  $expectedPath=$null
  if ($c.slot -ceq 'cli39.node-checksum-signature.v1') {
    switch -CaseSensitive ($Role) {
      'status' { $expectedPath=$c.outputPrefix+'.stdout.raw' }
      'diagnostics' { $expectedPath=$c.outputPrefix+'.stderr.raw' }
      'cleartext' { $expectedPath=$c.paths.payload }
    }
  } elseif ($c.slot -ceq 'cli39.node-key-policy.v1') {
    switch -CaseSensitive ($Role) {
      'key-colons' { $expectedPath=$c.outputPrefix+'.stdout.raw' }
      'key-status-and-diagnostics' { $expectedPath=$c.outputPrefix+'.stderr.raw' }
    }
  } elseif ($c.slot -cin @('cli39.full-source-materialization.v2','cli39.full-source-git-comparison.v2',
      'cli39.node-keyring-preparation.v1')) {
    $expectedPath=Get-Cli39PythonOutputPath32 $c $Role
  }
  if ($null -eq $expectedPath -or $Path -cne $expectedPath) { throw 'Fixed output subject association required.' }
  # The actual Stream/exception/partial buffer remains in the SAME enrolled
  # consumer graph. Neither a safe report nor a re-opened path replaces it.
  $record = @{ role=$Role; path=$Path; stream=$null; handleOriginal=$null;
    openAttempted=$false; openReturned=$false; bytes=$null; used=0; eof=$false; exception=$null }
  if (-not $c.outputRoleOriginals.Add($Role)) { throw 'Fixed original output role cannot be read twice.' }
  $c.outputOriginals.Add($record)
  try {
    $record.bytes = [byte[]]::new([int]($MaximumBytes + 1))
    $worker=New-Cli39ManagedPhase26 $c.originalPreparation 'output-read' $record
    $null=Start-Cli39ManagedPhase26 $worker
    $null=Wait-Cli39ManagedPhase26 $worker
    if (-not $record.eof -or $record.used -gt $MaximumBytes) { throw 'Complete original output exceeds admitted geometry.' }
    return $record
  } catch { $record.exception=$_; throw }
  # Disposition of these real holds belongs to the shared original owner ACK,
  # including parse failures; no child-exit Dispose substitutes for closure.
}

function Invoke-CliPrerequisiteDisposal26 {
  param([Parameter(Mandatory)][string]$Slot,[Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal
  if ($null -eq $c -or $c.slot -cne $Slot -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or ($c.ContainsKey('disposalOriginal') -and $null -ne $c.disposalOriginal)) {
    throw 'Actual original once-only consumer disposal required.'
  }
  if ($Slot -cnotin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1',
      'cli39.full-source-materialization.v2','cli39.full-source-git-comparison.v2',
      'cli39.node-keyring-preparation.v1','cli39.windows-publisher-trust.v1')) { throw 'Unsupported fixed disposal slot.' }
  $d=@{ resources=[Collections.Generic.List[object]]::new(); unknownAcquisition=$false;
    started=$true; returned=$false; providerPending=$false }
  $c.disposalOriginal=$d
  if($c.ContainsKey('pythonTreeOriginal')){
    try{$null=Dispose-Cli39PythonTree32 $p}catch{$d.treeDisposalExceptionOriginal=$_}
    if(-not (Test-Cli39PythonTreeSearchClosure32 $p)){$d.providerPending=$true;throw 'Actual output tree native search borrowers remain pending.'}
    foreach($read in $c.pythonTreeOriginal.readsOriginal){
      if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $read)){
        $d.providerPending=$true
        throw 'Actual output held-stream native query borrower remains pending before managed disposal.'
      }
    }
    foreach($phase in $c.pythonTreeOriginal.managedPhasesOriginal){
      if($phase.startAttempted -and ($null -eq $phase.thread -or -not $phase.terminalJoined -or
          $phase.thread.IsAlive -or -not $phase.completionAttempted -or $phase.pendingActive -or
          $phase.multiplicityUnknown -or $phase.joinException.present)){
        $d.providerPending=$true
        throw 'Actual tree read worker outputs/borrowers remain unresolved before resource disposition.'
      }
    }
  }
  if ($Slot -ceq 'cli39.windows-publisher-trust.v1') {
    if (-not (Test-Cli39PublisherSubjectBorrowClosed26 $p)) {
      $d.providerPending=$true
      throw 'Original native provider state still owns the held subject; no early disposal.'
    }
  }
  if($Slot -cin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1')){
    if(-not (Test-Cli39NodePreparedIdentityBorrowClosed32 $p)){
      $d.providerPending=$true
      # Retain whole pending state; visit each independently eligible resource below.
    }
  }
  # Prepare the complete actual object roster before any Dispose. Each resource
  # is independently attempted even if an earlier disposal fails; all original
  # exceptions remain charged in their actual records, never safe-text copies.
  foreach ($s in $c.subjectOriginals.Values) {
    if ($s.openAttempted -and -not $s.openReturned) { $d.unknownAcquisition=$true }
    if ($s.hashAttempted -and -not $s.hashReturned) { $d.unknownAcquisition=$true }
    if ($null -ne $s.stream) { $d.resources.Add(@{ role=($s.role+'-stream'); sourceOriginal=$s; objectOriginal=$s.stream; handleOriginal=$s.handleOriginal; disposalAttempted=$false; disposalReturned=$false; disposalOriginal=$null }) }
    if ($null -ne $s.hashOriginal) { $d.resources.Add(@{ role=($s.role+'-hash'); sourceOriginal=$s; objectOriginal=$s.hashOriginal; handleOriginal=$null; disposalAttempted=$false; disposalReturned=$false; disposalOriginal=$null }) }
  }
  foreach ($s in $c.outputOriginals) {
    if ($s.openAttempted -and -not $s.openReturned) { $d.unknownAcquisition=$true }
    if ($null -ne $s.stream) { $d.resources.Add(@{ role=($s.role+'-stream'); sourceOriginal=$s; objectOriginal=$s.stream; handleOriginal=$s.handleOriginal; disposalAttempted=$false; disposalReturned=$false; disposalOriginal=$null }) }
  }
  if ($c.ContainsKey('pythonPlanParseOriginal') -and $null -ne $c.pythonPlanParseOriginal) {
    $parse=$c.pythonPlanParseOriginal
    if ($parse.parserPartialUnknown) { $d.unknownAcquisition=$true }
    if ($null -ne $parse.documentOriginal) {
      $d.resources.Add(@{ role='held-plan-document'; sourceOriginal=$parse; objectOriginal=$parse.documentOriginal;
        handleOriginal=$null; disposalAttempted=$false; disposalReturned=$false; disposalOriginal=$null })
    }
  }
  if($c.ContainsKey('pythonCompletionOriginal') -and $null -ne $c.pythonCompletionOriginal){
    foreach($parse in $c.pythonCompletionOriginal.reportsOriginal){
      if($parse.parserPartialUnknown){$d.unknownAcquisition=$true}
      if($null -ne $parse.documentOriginal){
        $d.resources.Add(@{role=($parse.sourceOriginal.role+'-document');sourceOriginal=$parse;objectOriginal=$parse.documentOriginal;
          handleOriginal=$null;disposalAttempted=$false;disposalReturned=$false;disposalOriginal=$null})
      }
    }
  }
  $d.expectedOriginals=[Collections.Generic.List[object]]::new($d.resources.Count)
  foreach ($r in $d.resources) { $d.expectedOriginals.Add($r.objectOriginal) }
  foreach ($r in $d.resources) {
    if($Slot -cin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1')){
      $r.borrowEligibilityAttempted=$true;$r.borrowEligibilityReturned=$false
      $r.borrowEligibilityOriginal=$null;$r.borrowEligibilityExceptionOriginal=$null
      try{
        $r.borrowEligibilityOriginal=Test-Cli39NodeResourceDisposalEligible32 $p $r
        $r.borrowEligibilityReturned=$true
      }catch{$r.borrowEligibilityExceptionOriginal=$_;$d.providerPending=$true;continue}
      if(-not $r.borrowEligibilityOriginal){$r.borrowPending=$true;$d.providerPending=$true;continue}
    }
    $r.disposalAttempted=$true
    try {
      $worker=New-Cli39ManagedPhase26 $p 'resource-dispose' $r
      $r.disposalWorkerOriginal=$worker
      $null=Start-Cli39ManagedPhase26 $worker
      $null=Wait-Cli39ManagedPhase26 $worker
    }
    catch { $r.disposalOriginal=$_ }
  }
  if($c.ContainsKey('pythonTreeOriginal')){
    try{$d.treeFilesDisposalOriginal=Dispose-Cli39PythonTreeFiles32 $p}
    catch{$d.treeFilesDisposalExceptionOriginal=$_}
    $d.treeHoldDisposalsOriginal=[Collections.Generic.List[object]]::new($c.pythonTreeOriginal.holdsOriginal.Count)
    foreach($hold in $c.pythonTreeOriginal.holdsOriginal){
      $record=@{holdOriginal=$hold;disposalOriginal=$null;exceptionOriginal=$null}
      $d.treeHoldDisposalsOriginal.Add($record)
      try{$record.disposalOriginal=Dispose-Cli39PythonOutputHold32 $hold}
      catch{$record.exceptionOriginal=$_;$record.disposalOriginal=$hold.disposalOriginal}
    }
    if(-not (Test-Cli39PythonTreeClosure32 $p)){$d.providerPending=$true}
  }
  $d.returned=$true
  return $d
}

function Test-Cli39PrerequisiteClosure26 {
  param([Parameter(Mandatory)][string]$Slot,[Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation; $c=$p.consumerOriginal
  if ($null -eq $c -or $c.slot -cne $Slot -or
      -not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)) { return $false }
  if (-not $c.ContainsKey('disposalOriginal')) { return $false }
  if(-not (Test-Cli39PythonTreeClosure32 $p)){return $false}
  $d=$c.disposalOriginal
  if ($null -eq $d -or -not $d.returned -or $d.unknownAcquisition -or $d.providerPending) { return $false }
  $expected=$d.expectedOriginals; $expectedCount=0
  foreach ($s in $c.subjectOriginals.Values) {
    if ($s.openAttempted -and -not $s.openReturned) { return $false }
    if ($s.hashAttempted -and -not $s.hashReturned) { return $false }
    if ($null -ne $s.stream) { if (-not $expected.Contains($s.stream)) { return $false }; $expectedCount++ }
    if ($null -ne $s.hashOriginal) { if (-not $expected.Contains($s.hashOriginal)) { return $false }; $expectedCount++ }
  }
  foreach ($s in $c.outputOriginals) {
    if ($s.openAttempted -and -not $s.openReturned) { return $false }
    if ($null -ne $s.stream) { if (-not $expected.Contains($s.stream)) { return $false }; $expectedCount++ }
  }
  if ($c.ContainsKey('pythonPlanParseOriginal') -and $null -ne $c.pythonPlanParseOriginal -and
      $null -ne $c.pythonPlanParseOriginal.documentOriginal) {
    if (-not $expected.Contains($c.pythonPlanParseOriginal.documentOriginal)) { return $false }
    $expectedCount++
  }
  if($c.ContainsKey('pythonCompletionOriginal') -and $null -ne $c.pythonCompletionOriginal){
    foreach($parse in $c.pythonCompletionOriginal.reportsOriginal){
      if($parse.parserPartialUnknown -or -not (Test-Cli39PythonReportOriginal32 $p $parse)){return $false}
      if($null -ne $parse.documentOriginal){
        if(-not $expected.Contains($parse.documentOriginal)){return $false}
        $expectedCount++
      }
    }
  }
  if ($expected.Count -ne $expectedCount -or $expected.Count -ne $d.resources.Count) { return $false }
  foreach ($worker in $c.managedPhases) {
    if ($worker.deadlineRefused -and -not $worker.startAttempted -and
        $worker.results[0] -eq 0 -and $null -ne $worker.thread -and -not $worker.thread.IsAlive) { continue }
    if (-not $worker.startReturned -or -not $worker.terminalJoined -or $worker.thread.IsAlive -or
        -not $worker.completionAttempted -or $worker.pendingActive -or $worker.joinException.present -or
        $worker.multiplicityUnknown) { return $false }
  }
  foreach ($object in $expected) {
    $matches=0
    foreach ($r in $d.resources) {
      if (-not [object]::ReferenceEquals($r.objectOriginal,$object)) { continue }
      $matches++
      if (-not $r.disposalAttempted -or -not $r.disposalReturned -or $null -ne $r.disposalOriginal) { return $false }
      if ($object -is [IO.FileStream] -and ($null -eq $r.handleOriginal -or -not $r.handleOriginal.IsClosed -or $object.CanRead)) { return $false }
    }
    if ($matches -ne 1) { return $false }
  }
  if ($Slot -ceq 'cli39.windows-publisher-trust.v1') {
    if (-not (Test-Cli39PublisherNativeClosure32 $p)) { return $false }
  }
  # This checks actual consumer resource records only. Core separately proves
  # native child/job/IO/control terminal and receiver ACK. Captured metadata
  # objects remain paid in that receiver; no logical retirement is inferred.
  if($c.slot -cin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1')){
    if(-not (Test-Cli39NodePreparedIdentityClosure32 $p)){return $false}
  }
  return $true
}

function Complete-Cli39NodeChecksumSignature26 {
  param([Parameter(Mandatory)]$Owner)
  $c = $Owner.consumerOriginal
  if ($null -eq $c -or $c.slot -cne 'cli39.node-checksum-signature.v1') { throw 'Exact enrolled signature consumer required.' }
  $parse = @{ stdout=$null; stderr=$null; payload=$null; lines=$null; valid=$null;
    newsig=0; goodsig=0; goodKey=$null; cryptographicResult=$false;
    fullKeyPolicyVerified=$false; originalSubjectAuthenticated=$false; exception=$null }
  $c.signatureParseOriginal=$parse
  try {
    if ($Owner.exitCode -ne 0) { throw 'Original gpgv native result was not successful.' }
    $parse.stdout=Read-Cli39VerificationOutput26 $c 'status' ($c.outputPrefix+'.stdout.raw') $c.geometry.statusBytes
    $parse.stderr=Read-Cli39VerificationOutput26 $c 'diagnostics' ($c.outputPrefix+'.stderr.raw') $c.geometry.stderrBytes
    $parse.payload=Read-Cli39VerificationOutput26 $c 'cleartext' $c.paths.payload $c.geometry.payloadBytes
    $listing=$c.subjectOriginals['listing']
    if ($null -eq $listing -or $parse.payload.used -ne $listing.used) { throw 'Cleartext subject byte extent differs.' }
    for ($i=0;$i -lt $listing.used;$i++) {
      Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
      if ($parse.payload.bytes[$i] -ne $listing.bytes[$i]) { throw 'Signed extracted checksum payload differs from the exact held listing.' }
    }
    for ($i=0;$i -lt $parse.stdout.used;$i++) {
      Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
      $b=$parse.stdout.bytes[$i]
      if (($b -lt 32 -and $b -ne 10 -and $b -ne 13) -or $b -gt 126) { throw 'Unsupported original status encoding.' }
    }
    $text=[Text.Encoding]::ASCII.GetString($parse.stdout.bytes,0,$parse.stdout.used)
    $parse.lines=$text.Split([char]10)
    if ($parse.lines.Length -gt $c.geometry.statusLines) { throw 'Original status line geometry exceeded.' }
    foreach ($raw in $parse.lines) {
      Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
      $line=$raw.TrimEnd([char]13)
      if ($line.Length -eq 0) { continue }
      if ($line.Length -gt $c.geometry.statusLineBytes -or -not $line.StartsWith('[GNUPG:] ')) { throw 'Original status framing differs.' }
      $fields=$line.Substring(9).Split([char]32)
      switch -CaseSensitive ($fields[0]) {
        'NEWSIG' { $parse.newsig++; if ($parse.newsig -ne 1) { throw 'Multiple signature contexts refused.' } }
        'GOODSIG' {
          if ($parse.newsig -ne 1 -or $fields.Length -lt 3) { throw 'GOODSIG without original signature context.' }
          $parse.goodsig++; $parse.goodKey=$fields[1]
        }
        'VALIDSIG' {
          if ($parse.newsig -ne 1 -or $null -ne $parse.valid -or $fields.Length -ne 11) { throw 'Ambiguous VALIDSIG grammar.' }
          if ($fields[1] -cnotmatch '^[A-F0-9]{40}$' -or $fields[10] -cne 'CC68F5A3106FF448322E48ED27F5E38D5B0A215F') { throw 'Signature primary/subkey identity differs.' }
          if (-not $c.expectedSigningFingerprints.Contains($fields[1])) { throw 'Actual signing key lacks the selected qualified binding policy.' }
          foreach ($index in @(3,4,5,6,7,8)) { if ($fields[$index] -cnotmatch '^[0-9]+$') { throw 'Unsupported signature scalar.' } }
          if ($fields[5] -cne '4' -or $fields[6] -cne '0' -or $fields[9] -cne '01' -or
              -not $c.allowedPublicKeyAlgorithms.Contains($fields[7]) -or
              -not $c.allowedHashAlgorithms.Contains($fields[8])) { throw 'Signature algorithm/class policy differs.' }
          $created=[long]::Parse($fields[3]); $expires=[long]::Parse($fields[4])
          if ($created -lt $c.timePolicy.notBefore -or $created -gt $c.timePolicy.notAfter -or
              $created -gt $c.timePolicy.validationTime -or
              ($expires -ne 0 -and $expires -le $c.timePolicy.validationTime)) { throw 'Original signature time policy differs.' }
          $parse.valid=$fields
        }
        'KEY_CONSIDERED' {
          if ($fields.Length -ne 3 -or $fields[1] -cne 'CC68F5A3106FF448322E48ED27F5E38D5B0A215F' -or
              $fields[2] -cnotin @('0','1','2','3')) { throw 'Unexpected considered primary key or original flag grammar.' }
          # This lookup diagnostic is retained, not promoted into signing-key
          # usability/current revocation proof. Those require the selected
          # complete key-policy relation and actual signature outcome.
        }
        'SIG_ID' { if ($fields.Length -ne 4) { throw 'Unsupported original signature ID grammar.' } }
        default { throw 'Unsupported or unsuccessful signature status; full raw output retained.' }
      }
    }
    if ($parse.newsig -ne 1 -or $parse.goodsig -ne 1 -or $null -eq $parse.valid -or
        ($parse.goodKey -cne $parse.valid[1] -and $parse.goodKey -cne $parse.valid[1].Substring(24))) { throw 'Original signature outcomes do not agree.' }
    Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
    $null=Join-Cli39SignatureCheckedKey32 $Owner $parse
    $parse.cryptographicResult=$true
    # Full key binding/current revocation and authenticated held subject/native
    # loaded image policy remain separate genuine results, never booleans
    # copied from this parser. This cryptographic classification grants no GO.
    return $parse
  } catch {
    $parse.exception=$_
    $_.Exception.Data['originalOwner']=$Owner
    throw
  }
}

function Complete-Cli39NodeKeyPolicy26 {
  param([Parameter(Mandatory)]$Owner)
  $c=$Owner.consumerOriginal
  if ($null -eq $c -or $c.slot -cne 'cli39.node-key-policy.v1') { throw 'Exact enrolled key-policy consumer required.' }
  $parse=@{ stdout=$null; stderr=$null; rows=$null; keys=[Collections.Generic.List[object]]::new();
    identities=[Collections.Generic.List[object]]::new();
    signatures=[Collections.Generic.List[object]]::new(); availableKeyChecks=$false;
    actualFingerprints=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal);
    expectedFingerprints=[Collections.Generic.HashSet[string]]::new([StringComparer]::Ordinal);
    currentAuthenticatedRevocationVerified=$false; fullKeyPolicyVerified=$false; exception=$null }
  $c.keyParseOriginal=$parse
  try {
    foreach ($fingerprint in $c.expectedKeyFingerprints) {
      Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
      if ($fingerprint -cnotmatch '^[A-F0-9]{40}$' -or -not $parse.expectedFingerprints.Add($fingerprint)) { throw 'Duplicate or unsupported expected key fingerprint; original roster retained.' }
    }
    if (-not $parse.expectedFingerprints.Contains('CC68F5A3106FF448322E48ED27F5E38D5B0A215F')) { throw 'Expected complete key roster lacks selected primary.' }
    if ($Owner.exitCode -ne 0) { throw 'Original gpg key-policy native result unsuccessful.' }
    $parse.stdout=Read-Cli39VerificationOutput26 $c 'key-colons' ($c.outputPrefix+'.stdout.raw') $c.geometry.statusBytes
    $parse.stderr=Read-Cli39VerificationOutput26 $c 'key-status-and-diagnostics' ($c.outputPrefix+'.stderr.raw') $c.geometry.stderrBytes
    # UTF-8 UID bytes remain raw originals. Only ASCII machine fields below
    # confer structural/check classifications, never unescaped display names.
    $text=[Text.UTF8Encoding]::new($false,$true).GetString($parse.stdout.bytes,0,$parse.stdout.used)
    $parse.rows=$text.Split([char]10)
    if ($parse.rows.Length -gt $c.geometry.statusLines) { throw 'Complete key row geometry exceeded.' }
    $current=$null; $identity=$null; $primaryCount=0; $certifications=0
    foreach ($raw in $parse.rows) {
      Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
      $line=$raw.TrimEnd([char]13)
      if ($line.Length -eq 0) { continue }
      if ([Text.Encoding]::UTF8.GetByteCount($line) -gt $c.geometry.statusLineBytes) { throw 'Complete key row extent exceeded.' }
      $f=$line.Split([char]58)
      switch -CaseSensitive ($f[0]) {
        { $_ -ceq 'pub' -or $_ -ceq 'sub' } {
          if ($f.Length -lt 12 -or $f[1] -cmatch '[ried]' -or $f[11].Contains('D') -or
              $f[5] -cnotmatch '^[0-9]+$' -or ($f[6] -ne '' -and $f[6] -cnotmatch '^[0-9]+$')) { throw 'Invalid/revoked/expired/disabled key record.' }
          $created=[long]::Parse($f[5]); $expires=0L
          if ($f[6] -ne '') { $expires=[long]::Parse($f[6]) }
          if ($created -gt $c.timePolicy.validationTime -or ($expires -ne 0 -and $expires -le $c.timePolicy.validationTime)) { throw 'Key lifetime policy differs.' }
          if ($f[0] -ceq 'pub') { $primaryCount++ }
          $identity=$null
          $current=@{ type=$f[0]; fields=$f; fingerprint=$null; bindingChecks=0 }
          $parse.keys.Add($current)
        }
        'fpr' {
          if ($null -eq $current -or $null -ne $current.fingerprint -or $f.Length -lt 10 -or $f[9] -cnotmatch '^[A-F0-9]{40}$') { throw 'Ambiguous complete key fingerprint record.' }
          $current.fingerprint=$f[9]
          if (-not $parse.actualFingerprints.Add($f[9])) { throw 'Duplicate actual key fingerprint; complete original rows retained.' }
        }
        'sig' {
          if ($null -eq $current -or $f.Length -lt 13 -or $f[1] -cne '!' -or
              $f[12] -cne 'CC68F5A3106FF448322E48ED27F5E38D5B0A215F') { throw 'Unverified or foreign key certification/binding.' }
          $parse.signatures.Add($f)
          if ($f[10] -cmatch '^1[0-3][xl]$') {
            if ($current.type -cne 'pub' -or $null -eq $identity) { throw 'Certification lacks its exact original primary UID/attribute association.' }
            $identity.certifications++; $certifications++
          }
          elseif ($f[10] -cmatch '^18[xl]$' -and $current.type -ceq 'sub') { $current.bindingChecks++ }
          else { throw 'Unsupported complete key signature class.' }
        }
        'rev' { throw 'Original key revocation present.' }
        'rvs' { throw 'Original standalone key revocation present.' }
        { $_ -ceq 'uid' -or $_ -ceq 'uat' } {
          if ($null -eq $current -or $current.type -cne 'pub' -or
              $f.Length -lt 10 -or $f[1] -cmatch '[ried]' -or
              $f[5] -cnotmatch '^[0-9]+$' -or ($f[6] -ne '' -and $f[6] -cnotmatch '^[0-9]+$')) { throw 'Invalid original UID/attribute association or lifetime.' }
          $created=[long]::Parse($f[5]); $expires=0L
          if ($f[6] -ne '') { $expires=[long]::Parse($f[6]) }
          if ($created -gt $c.timePolicy.validationTime -or
              ($expires -ne 0 -and $expires -le $c.timePolicy.validationTime)) { throw 'Original UID/attribute lifetime differs.' }
          $identity=@{ type=$f[0]; fields=$f; primaryOriginal=$current; certifications=0 }
          $parse.identities.Add($identity)
        }
        'tru' { if ($f.Length -lt 2) { throw 'Unsupported trust diagnostic.' } }
        default { throw 'Unsupported complete key row; original output retained.' }
      }
    }
    if ($primaryCount -ne 1 -or $certifications -lt 1 -or $parse.keys.Count -ne $c.expectedKeyFingerprints.Count) { throw 'Complete original key/binding roster differs.' }
    foreach ($key in $parse.keys) {
      Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
      if ($null -eq $key.fingerprint -or -not $c.expectedKeyFingerprints.Contains($key.fingerprint) -or
          ($key.type -ceq 'pub' -and $key.fingerprint -cne 'CC68F5A3106FF448322E48ED27F5E38D5B0A215F') -or
          ($key.type -ceq 'sub' -and $key.bindingChecks -ne 1)) { throw 'Original primary/subkey binding roster differs.' }
      if($c.expectedSigningFingerprints.Contains($key.fingerprint)){
        # DETAILS field4 is this key's public algorithm; lowercase field12 s
        # belongs to this key. Uppercase S on primary is only aggregate usage.
        if($key.fields[3] -cnotmatch '^[1-9][0-9]*$' -or
            -not $c.allowedPublicKeyAlgorithms.Contains($key.fields[3]) -or
            -not $key.fields[11].Contains('s')){
          throw 'Selected signing fingerprint lacks its actual permitted local signing capability/algorithm.'
        }
      }
    }
    foreach ($identityOriginal in $parse.identities) {
      Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
      if ($identityOriginal.certifications -lt 1 -or
          $identityOriginal.primaryOriginal.fingerprint -cne 'CC68F5A3106FF448322E48ED27F5E38D5B0A215F') { throw 'Every original UID/attribute requires its own checked selected-primary certification.' }
    }
    if (-not $parse.actualFingerprints.SetEquals($parse.expectedFingerprints)) { throw 'Complete expected/actual key fingerprint sets differ; original rows retained.' }
    Assert-Cli39PrerequisiteUseful26 $c.originalPreparation
    $parse.availableKeyChecks=$true
    # These checks cover only the exact supplied available key material. They
    # cannot authenticate current revocation-input freshness or signing trust.
    return $parse
  } catch { $parse.exception=$_; $_.Exception.Data['originalOwner']=$Owner; throw }
}
