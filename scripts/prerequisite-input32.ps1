# Source-only fixed startup reader. Complete raw custody/closed root structure
# is distinct from actual native source/startup/loaded/namespace admission.
function Read-Cli39PrerequisiteRootStructure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)][string]$InputRootPath,
    [Parameter(Mandatory)][long]$InputRootBytes,
    [Parameter(Mandatory)][string]$InputRootSha256,
    [Parameter(Mandatory)][string]$Action,
    [Parameter(Mandatory)][ValidateSet('qualification-coordinator','retained-caller','operation-host')][string]$RootRole)
  $p=$HostOwnerOriginal
  if (-not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $null -ne $p.consumerOriginal -or $InputRootBytes -le 0 -or
      $InputRootBytes -ge [int]::MaxValue -or
      -not [IO.Path]::IsPathFullyQualified($InputRootPath) -or
      $InputRootSha256 -cnotmatch '^[0-9a-f]{64}$' -or
      $Action -cnotin @('materialize','compare','keyring','key-policy','checksum-signature','publisher')) {
    throw 'Original fixed startup root association required before root effects.'
  }
  $capture=@{path=$InputRootPath;bytesExpected=$InputRootBytes;sha256Expected=$InputRootSha256;
    actionOriginal=$Action;rootRoleOriginal=$RootRole;rootSubjectOriginal=$null;parseOriginal=$null;
    fieldMapOriginal=$null;fieldOriginals=$null;resultOriginal=$null;
    profileOriginals=[Collections.Generic.List[object]]::new($(if($Action -ceq 'checksum-signature'){4}else{3}));
    structuralReturned=$false;nativeAssociationOriginals=$null;
    state='STRUCTURE_ONLY_NATIVE_ASSOCIATION_PENDING';exceptionOriginal=$null}
  $p.inputRead.value=$capture
  $c=@{slot='cli39.source-input-host.v1';subjectOriginals=@{};outputOriginals=[Collections.Generic.List[object]]::new();
    geometry=@{stackMode='clr-default';managedPhaseCount=5};
    managedPhases=[Collections.Generic.List[object]]::new(5);rootParseOriginal=$null}
  $p.consumerOriginal=$c
  # Default CLR worker semantics are source-selected, not a guessed stack
  # size. Its full actual stack/guard/TLS/runtime/allocator/startup charge must
  # belong to the external original startup reservation before execution.
  try {
    $subject=@{role='input-root';path=$InputRootPath;stream=$null;handleOriginal=$null;
      bytes=$null;openAttempted=$false;openReturned=$false;used=0;eof=$false;
      hashAttempted=$false;hashReturned=$false;hashOriginal=$null;digest=$null;exception=$null}
    $capture.rootSubjectOriginal=$subject;$c.subjectOriginals.Add('input-root',$subject)
    $subject.bytes=[byte[]]::new([int]($InputRootBytes+1))
    $worker=New-Cli39ManagedPhase26 $p 'subject-read-hash' $subject
    $null=Start-Cli39ManagedPhase26 $worker
    $null=Wait-Cli39ManagedPhase26 $worker
    if(-not $subject.eof -or $subject.used -ne $InputRootBytes){throw 'Exact original root EOF/extent differs.'}
    $subject.digest=[BitConverter]::ToString($subject.digestOriginal).Replace('-','').ToLowerInvariant()
    if($subject.digest -cne $InputRootSha256){throw 'Exact original root digest differs.'}
    $parse=@{sourceOriginal=$subject;documentOriginal=$null;memoryOriginal=$null;
      optionsOriginal=$null;effectReturned=$false;exception=$null;
      parserCallAttempted=$false;parserCallReturned=$false;parserPartialUnknown=$false;
      parserPartialDisposition='NOT_ENTERED'}
    $capture.parseOriginal=$parse;$c.rootParseOriginal=$parse
    $worker=New-Cli39ManagedPhase26 $p 'root-json-parse' $parse
    $null=Start-Cli39ManagedPhase26 $worker
    $null=Wait-Cli39ManagedPhase26 $worker
    $root=$parse.documentOriginal.RootElement
    if($root.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Fixed root must be one object.'}
    $expected=[string[]]@('schema','action','absoluteDeadlineTicks','clockFrequency',
      'entryPath','sharedSourceRoot','hostPath','kernel32Path','callerControl','hostControl',
      'callerEnvironment','hostEnvironment','callerWorkingDirectory','hostWorkingDirectory',
      'callerHome','hostHome','callerOutputPrefix','hostOutputPrefix','callerBudget','hostBudget',
      'consumerBudget','rawOutputLimitBytes','callerRawOutputLimitBytes','sourceClosure',
      'startupClosure','loadedClosure','namespaceClosure','consumer','principalApiModule')
    if($RootRole -cne 'qualification-coordinator'){$expected+='parentLineage';$expected+='foundationProjection'}
    if($Action -ceq 'publisher'){$expected+='publisherFoundation'}
    if($Action -ceq 'checksum-signature'){$expected+=@('keyPolicyConsumer','keyPolicyBudget','keyPolicyRawOutputLimitBytes')}
    $capture.expectedKeysOriginal=$expected
    $fields=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $capture.fieldMapOriginal=$fields
    # Retain every actual JsonProperty before validation, including the first
    # rejected duplicate/unknown field:29 common fields, publisher adds one,
    # plus one first-rejection witness. Source roster does not imply byte fit.
    $capture.fieldOriginals=[Collections.Generic.List[object]]::new($expected.Count+1)
    foreach($field in $root.EnumerateObject()){
      Observe-OwnedPrerequisiteDeadline $p
      $capture.fieldOriginals.Add($field)
      if($field.Name -cnotin $expected -or $fields.ContainsKey($field.Name)){throw 'Unknown or decoded duplicate root field.'}
      $fields.Add($field.Name,$field.Value)
    }
    if($fields.Count -ne $expected.Count){throw 'Incomplete fixed root field set.'}
    foreach($key in @('schema','action','absoluteDeadlineTicks','entryPath','sharedSourceRoot','hostPath',
        'kernel32Path','callerWorkingDirectory','hostWorkingDirectory','callerHome','hostHome',
        'callerOutputPrefix','hostOutputPrefix')){
      if($fields[$key].ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Exact root string field required.'}
    }
    $expectedSchema=if($RootRole -ceq 'qualification-coordinator'){'service-lasso.cli39-prerequisite-input.v2'}else{'service-lasso.cli39-prerequisite-input.v3'}
    if($fields['schema'].GetString() -cne $expectedSchema -or
        $fields['action'].GetString() -cne $Action){throw 'Fixed root schema/action differs from original invocation.'}
    $deadlineText=$fields['absoluteDeadlineTicks'].GetString()
    $deadline=0L
    if($deadlineText -cnotmatch '^[1-9][0-9]*$' -or
        -not [long]::TryParse($deadlineText,[Globalization.NumberStyles]::None,[Globalization.CultureInfo]::InvariantCulture,[ref]$deadline) -or
        $deadline -ne $p.originalDeadlineTicks){throw 'Root must retain SAME original monotonic deadline.'}
    $integerKeys=@('clockFrequency','rawOutputLimitBytes','callerRawOutputLimitBytes')
    if($Action -ceq 'checksum-signature'){$integerKeys+='keyPolicyRawOutputLimitBytes'}
    foreach($key in $integerKeys){
      $value=0L
      if($fields[$key].ValueKind -ne [System.Text.Json.JsonValueKind]::Number -or
          $fields[$key].GetRawText() -cnotmatch '^[1-9][0-9]*$' -or
          -not $fields[$key].TryGetInt64([ref]$value) -or $value -le 0){throw 'Canonical positive fixed root integer required.'}
    }
    if($fields['clockFrequency'].GetInt64() -ne [Diagnostics.Stopwatch]::Frequency){throw 'Root clock domain differs from actual source runtime.'}
    foreach($key in @('entryPath','sharedSourceRoot','hostPath','kernel32Path','callerWorkingDirectory',
        'hostWorkingDirectory','callerHome','hostHome','callerOutputPrefix','hostOutputPrefix')){
      if(-not [IO.Path]::IsPathFullyQualified($fields[$key].GetString())){throw 'Exact absolute root routing path required.'}
    }
    foreach($key in @('callerControl','hostControl','callerEnvironment','hostEnvironment',
        'callerBudget','hostBudget','consumerBudget','consumer','principalApiModule')){
      if($fields[$key].ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Exact fixed root object required.'}
    }
    foreach($key in @('sourceClosure','startupClosure','loadedClosure','namespaceClosure')){
      if($fields[$key].ValueKind -ne [System.Text.Json.JsonValueKind]::Array){throw 'Complete original association roster required.'}
    }
    $null=Read-Cli39PrerequisiteProfileStructure32 $p 'callerBudget' $fields['callerBudget'] $fields['callerRawOutputLimitBytes'].GetInt64() $Action
    $null=Read-Cli39PrerequisiteProfileStructure32 $p 'hostBudget' $fields['hostBudget'] $fields['rawOutputLimitBytes'].GetInt64() $Action
    $consumerProfile=Read-Cli39PrerequisiteProfileStructure32 $p 'consumerBudget' $fields['consumerBudget'] $fields['rawOutputLimitBytes'].GetInt64() $Action
    if($Action -ceq 'checksum-signature'){
      foreach($key in @('keyPolicyConsumer','keyPolicyBudget')){
        if($fields[$key].ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Fixed checksum key-policy object required.'}
      }
      $null=Read-Cli39PrerequisiteProfileStructure32 $p 'keyPolicyBudget' $fields['keyPolicyBudget'] $fields['keyPolicyRawOutputLimitBytes'].GetInt64() $Action
    }
    $capture.resultOriginal=@{fieldsOriginal=$fields;rootPath=$InputRootPath;rootBytes=$InputRootBytes;
      rootHash=$subject.digest;rootSubjectOriginal=$subject;rootDocumentOriginal=$parse.documentOriginal;
      action=$Action;absoluteDeadlineTicks=$deadline;receiverOriginal=$p.receiver;
      consumerProfileOriginal=$consumerProfile}
    $capture.structuralReturned=$true
    return $capture
  } catch {$capture.exceptionOriginal=$_;throw}
}

function Invoke-Cli39RootDisposal32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$c=$p.consumerOriginal
  if($null -eq $c){return $null}
  if($c.slot -cne 'cli39.source-input-host.v1' -or
      $c.ContainsKey('disposalOriginal') -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)){throw 'SAME once-only root disposal required.'}
  $capacity=3;$root=$p.inputRead.value
  if($null -ne $root -and $root.ContainsKey('heldMemberBookOriginal')){$capacity=$root.heldMemberBookOriginal.resourceCapacity}
  $d=@{resources=[Collections.Generic.List[object]]::new($capacity);returned=$false;
    unknownAcquisition=$false;pendingBorrow=$false;exceptionOriginal=$null;
    moduleDisposalExceptionOriginal=$null;namespaceDisposalExceptionOriginal=$null;
    moduleClosureObserved=$false;namespaceClosureObserved=$false}
  $c.disposalOriginal=$d
  if(-not (Test-Cli39DerivedRootClosure32 $p) -or
      -not (Test-Cli39ParentObservationClosure32 $p) -or
      -not (Test-Cli39AllOwnedHostBirthClosure32 $p) -or
      -not (Test-Cli39NativeQueryBorrowClosure32 $p) -or
      -not (Test-Cli39NativeOutputQueryBorrowClosure32 $p) -or
      -not (Test-Cli39RootFileQueryClosure32 $p) -or
      -not (Test-Cli39AllRootMemberFileQueriesClosure32 $p)){
    $d.pendingBorrow=$true;throw 'Actual fixed query worker still borrows resident root associations.'
  }
  # Never dispose beneath a pending/uncertain reader or parser. A thrown Start
  # is not a no-effect witness; its actual terminal disposition remains owed.
  foreach($worker in $c.managedPhases){
    if($worker.startAttempted -and
        (-not $worker.terminalJoined -or $null -eq $worker.thread -or $worker.thread.IsAlive)){
      $d.pendingBorrow=$true;throw 'Actual root worker still owns its original resources.'
    }
  }
  foreach($subject in $c.subjectOriginals.Values){
  if($null -ne $subject){
    if(($subject.openAttempted -and -not $subject.openReturned) -or
        ($subject.hashAttempted -and -not $subject.hashReturned)){$d.unknownAcquisition=$true}
    if($null -ne $subject.stream){$d.resources.Add(@{role='root-stream';sourceOriginal=$subject;objectOriginal=$subject.stream;handleOriginal=$subject.handleOriginal;disposalAttempted=$false;disposalReturned=$false;disposalOriginal=$null})}
    if($null -ne $subject.hashOriginal){$d.resources.Add(@{role='root-hash';sourceOriginal=$subject;objectOriginal=$subject.hashOriginal;handleOriginal=$null;disposalAttempted=$false;disposalReturned=$false;disposalOriginal=$null})}
  }
  }
  if(-not (Test-Cli39RootProcessClosure32 $p)){$d.pendingBorrow=$true;throw 'Actual process worker/pin still borrows root owner resources.'}
  if(-not (Test-Cli39RootPipePeerClosure32 $p)){$d.pendingBorrow=$true;throw 'Actual control-server peer worker/pin/handle borrow remains pending.'}
  if(-not (Test-Cli39RootPrincipalModuleClosure32 $p)){$d.pendingBorrow=$true;throw 'Actual principal module phase still borrows held originals.'}
  if(-not (Test-Cli39RootImageClosure32 $p)){$d.pendingBorrow=$true;throw 'Actual image worker/pin still borrows root owner resources.'}
  $root=$p.inputRead.value
  if($null -ne $root -and $root.ContainsKey('rootModuleBookOriginal') -and
      $null -eq $root.rootModuleBookOriginal.disposalOriginal){
    try{$null=Invoke-Cli39RootModuleDisposal32 $p}catch{$d.moduleDisposalExceptionOriginal=$_}
  }
  if($null -ne $root.namespaceBookOriginal -and $null -eq $root.namespaceBookOriginal.disposalOriginal){
    try{$null=Invoke-Cli39RootNamespaceDisposal32 $p}catch{$d.namespaceDisposalExceptionOriginal=$_}
  }
  $d.moduleClosureObserved=Test-Cli39RootModuleClosure32 $p
  $d.namespaceClosureObserved=Test-Cli39RootNamespaceClosure32 $p
  # Snapshot/namespace HANDLE close failure is an outstanding obligation, but
  # does not itself borrow the managed root stream. Actual worker borrowers do.
  if(-not (Test-Cli39RootModuleBorrowClosed32 $p)){$d.pendingBorrow=$true;throw 'Actual snapshot worker borrower remains pending.'}
  if($null -ne $root.namespaceBookOriginal){
    if(-not $root.namespaceBookOriginal.constructorTerminal){$d.pendingBorrow=$true;throw 'Actual namespace constructor remains active.'}
    foreach($hold in $root.namespaceBookOriginal.holdsOriginal){
      foreach($phaseName in @('open','metadata','close')){
        if(-not (Test-Cli39NamespaceWorkerBorrowClosure32 $hold.workersOriginal[$phaseName])){
          $d.pendingBorrow=$true;throw 'Actual namespace worker borrower remains pending.'
        }
      }
    }
  }
  $parse=$c.rootParseOriginal
  if($null -ne $parse -and $parse.parserPartialUnknown){
    # Exact CLR/runtime source must establish whether a throwing Parse leaves
    # only managed/pool originals chargeable to host exit or another external
    # obligation. No source fact currently proves that classification. Null
    # document alone never supplies it; retain unknown alongside raw exception.
    $d.unknownAcquisition=$true
  }
  if($null -ne $parse -and $null -ne $parse.documentOriginal){$d.resources.Add(@{role='root-document';sourceOriginal=$parse;objectOriginal=$parse.documentOriginal;handleOriginal=$null;disposalAttempted=$false;disposalReturned=$false;disposalOriginal=$null})}
  foreach($resource in $d.resources){
    $resource.disposalAttempted=$true
    try{
      $worker=New-Cli39ManagedPhase26 $p 'resource-dispose' $resource
      $resource.disposalWorkerOriginal=$worker
      $null=Start-Cli39ManagedPhase26 $worker
      $null=Wait-Cli39ManagedPhase26 $worker
    }catch{$resource.disposalOriginal=$_}
  }
  $d.returned=$true;return $d
}

function Test-Cli39RootClosure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$c=$p.consumerOriginal
  if(-not (Test-Cli39DerivedRootClosure32 $p)){return $false}
  if(-not (Test-Cli39ParentObservationClosure32 $p)){return $false}
  if(-not (Test-Cli39NamespaceRecipesTerminal32 $p)){return $false}
  if(-not (Test-Cli39PrincipalRecipesTerminal32 $p)){return $false}
  if(-not (Test-Cli39ParentRecipesTerminal32 $p)){return $false}
  if(-not (Test-Cli39LoadedRecipesTerminal32 $p)){return $false}
  if(-not (Test-Cli39LoadedObservationClosure32 $p)){return $false}
  if($null -eq $c){return (-not $p.inputRead.attempted)}
  $d=$c.disposalOriginal
  if($null -eq $d -or -not $d.returned -or $d.pendingBorrow -or $d.unknownAcquisition){return $false}
  if(-not (Test-Cli39NativeQueryBorrowClosure32 $p)){return $false}
  if(-not (Test-Cli39NativeOutputQueryBorrowClosure32 $p)){return $false}
  if(-not (Test-Cli39RootFileQueryClosure32 $p) -or
      -not (Test-Cli39AllRootMemberFileQueriesClosure32 $p)){return $false}
  if(-not (Test-Cli39RootProcessClosure32 $p)){return $false}
  if(-not (Test-Cli39RootPipePeerClosure32 $p)){return $false}
  if(-not (Test-Cli39RootPrincipalModuleClosure32 $p)){return $false}
  if(-not (Test-Cli39RootImageClosure32 $p)){return $false}
  if(-not (Test-Cli39RootModuleClosure32 $p)){return $false}
  if(-not (Test-Cli39RootNamespaceClosure32 $p)){return $false}
  foreach($worker in $c.managedPhases){
    if($worker.deadlineRefused -and -not $worker.startAttempted -and
        $worker.results[0] -eq 0 -and $null -ne $worker.thread -and -not $worker.thread.IsAlive){continue}
    if(-not $worker.startReturned -or -not $worker.terminalJoined -or $worker.thread.IsAlive -or
        -not $worker.completionAttempted -or $worker.pendingActive -or
        $worker.multiplicityUnknown -or $worker.joinException.present){return $false}
  }
  $capacity=3;$root=$p.inputRead.value
  if($null -ne $root -and $root.ContainsKey('heldMemberBookOriginal')){$capacity=$root.heldMemberBookOriginal.resourceCapacity}
  $expected=[Collections.Generic.List[object]]::new($capacity)
  $parse=$c.rootParseOriginal
  foreach($subject in $c.subjectOriginals.Values){
    if($null -ne $subject.stream){$expected.Add($subject.stream)}
    if($null -ne $subject.hashOriginal){$expected.Add($subject.hashOriginal)}
  }
  if($null -ne $parse -and $null -ne $parse.documentOriginal){$expected.Add($parse.documentOriginal)}
  if($expected.Count -ne $d.resources.Count){return $false}
  foreach($object in $expected){
    $matches=0
    foreach($resource in $d.resources){
      if(-not [object]::ReferenceEquals($resource.objectOriginal,$object)){continue}
      $matches++
      if(-not $resource.disposalAttempted -or -not $resource.disposalReturned -or
          $null -ne $resource.disposalOriginal){return $false}
      if($object -is [IO.FileStream] -and
          ($null -eq $resource.handleOriginal -or -not $resource.handleOriginal.IsClosed -or $object.CanRead)){return $false}
    }
    if($matches -ne 1){return $false}
  }
  # All raw bytes/maps/worker/errors and CLR-private ended-thread references
  # remain retained and charged through independently observed actual host exit.
  return $true
}
