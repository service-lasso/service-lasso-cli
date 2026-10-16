# UNRUN fixed managed effects under the original enrolled preparation.
# Context: [path, preowned buffer, actual resource, outputs[4], results[8]].
# Outputs: stream, hash, digest, original Exception. Results: entered,
# openAttempted, openReturned, used, EOF, hashAttempted, hashReturned,
# effectReturned. No PowerShell code executes on the foreign worker.
function Test-Cli39ManagedPhaseRoster26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal;$c=$w.preparation.consumerOriginal
  if($w.phase -cin @('tree-read-hash','tree-resource-dispose','tree-buffer-hash')){
    if($c.slot -cne 'cli39.full-source-materialization.v2' -or
        -not $c.ContainsKey('pythonTreeOriginal') -or $null -eq $c.pythonTreeOriginal){return $false}
    $b=$c.pythonTreeOriginal
    return ([object]::ReferenceEquals($b.preparationOriginal,$w.preparation) -and
      [object]::ReferenceEquals($w.rosterOriginal,$b.managedPhasesOriginal) -and
      $null -ne $b.managedPhasesOriginal -and $b.managedPhasesOriginal.Contains($w))
  }
  return ([object]::ReferenceEquals($w.rosterOriginal,$c.managedPhases) -and
    $null -ne $c.managedPhases -and $c.managedPhases.Contains($w))
}

function Test-Cli39PythonTreeManagedEffect32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)][string]$Phase,
    [Parameter(Mandatory)]$EffectOriginal)
  $p=$OriginalPreparation;$c=$p.consumerOriginal
  if($c.slot -cne 'cli39.full-source-materialization.v2' -or
      -not $c.ContainsKey('pythonTreeOriginal') -or $null -eq $c.pythonTreeOriginal){return $false}
  $b=$c.pythonTreeOriginal
  if(-not [object]::ReferenceEquals($b.preparationOriginal,$p)){return $false}
  if($Phase -ceq 'tree-read-hash'){
    foreach($r in $b.readsOriginal){
      if([object]::ReferenceEquals($r.bookOriginal,$b) -and
          [object]::ReferenceEquals($r.subjectOriginal,$EffectOriginal) -and
          $b.holdsOriginal.Contains($r.holdOriginal)){
        return ($r.readRequired -and -not $b.ContainsKey('disposalOriginal'))
      }
    }
  }elseif($Phase -ceq 'tree-buffer-hash'){
    return ($null -ne $b.hashesOriginal -and $b.hashesOriginal.Contains($EffectOriginal) -and
      [object]::ReferenceEquals($EffectOriginal.bookOriginal,$b) -and
      $EffectOriginal.roleOriginal -cin @('tar-header','tar-control','git-regular','git-symlink') -and
      (($EffectOriginal.roleOriginal -cin @('tar-header','tar-control') -and $EffectOriginal.algorithmOriginal -ceq 'SHA256') -or
       ($EffectOriginal.roleOriginal -cin @('git-regular','git-symlink') -and $EffectOriginal.algorithmOriginal -ceq 'SHA1')) -and
      -not $b.ContainsKey('disposalOriginal'))
  }elseif($Phase -ceq 'tree-resource-dispose'){
    foreach($r in $b.managedResourcesOriginal){
      if([object]::ReferenceEquals($r,$EffectOriginal)){
        foreach($hash in $b.hashesOriginal){
          if([object]::ReferenceEquals($r.sourceOriginal,$hash) -and
              $r.objectOriginal -is [Security.Cryptography.HashAlgorithm] -and
              [object]::ReferenceEquals($r.objectOriginal,$hash.hashOriginal)){return $true}
        }
        foreach($read in $b.readsOriginal){
          if([object]::ReferenceEquals($read.subjectOriginal,$r.sourceOriginal)){
            return (($r.objectOriginal -is [IO.FileStream] -and
                [object]::ReferenceEquals($r.objectOriginal,$read.subjectOriginal.stream)) -or
              ($r.objectOriginal -is [Security.Cryptography.HashAlgorithm] -and
                [object]::ReferenceEquals($r.objectOriginal,$read.subjectOriginal.hashOriginal)))
          }
        }
      }
    }
  }
  return $false
}

function New-Cli39ManagedPhase26 {
  param([Parameter(Mandatory)]$OriginalPreparation,
    [Parameter(Mandatory)][ValidateSet('subject-read-hash','output-read','resource-dispose','plan-json-parse','root-json-parse','report-json-parse','tree-read-hash','tree-resource-dispose','tree-buffer-hash')][string]$Phase,
    [Parameter(Mandatory)]$EffectOriginal)
  $p=$OriginalPreparation; $c=$p.consumerOriginal
  if (-not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p)) { throw 'Same enrolled preparation required.' }
  if ($c.slot -cnotin @('cli39.node-key-policy.v1','cli39.node-checksum-signature.v1',
      'cli39.full-source-materialization.v2','cli39.full-source-git-comparison.v2',
      'cli39.node-keyring-preparation.v1','cli39.windows-publisher-trust.v1','cli39.source-input-host.v1')) { throw 'No generic managed effect slot exists.' }
  $rootPhase=($c.slot -ceq 'cli39.source-input-host.v1')
  $treePhase=($Phase -cin @('tree-read-hash','tree-resource-dispose','tree-buffer-hash'))
  if($treePhase -and $c.slot -cne 'cli39.full-source-materialization.v2'){throw 'Tree phases belong only to fixed materialize consumer.'}
  if ($rootPhase -and $Phase -cnotin @('subject-read-hash','resource-dispose','root-json-parse')) {
    throw 'Fixed startup root has only its original read/hash/parse/disposal effects.'
  }
  if ($c.slot -ceq 'cli39.windows-publisher-trust.v1' -and
      $Phase -cnotin @('subject-read-hash','resource-dispose')) {
    throw 'Publisher managed effects are fixed held subjects and their independent disposal only.'
  }
  $associated=$false
  if($treePhase){
    $associated=Test-Cli39PythonTreeManagedEffect32 $p $Phase $EffectOriginal
  } elseif ($Phase -ceq 'subject-read-hash') {
    foreach ($subject in $c.subjectOriginals.Values) {
      if ([object]::ReferenceEquals($subject,$EffectOriginal)) { $associated=$true; break }
    }
  } elseif ($Phase -ceq 'root-json-parse') {
    $associated=($rootPhase -and [object]::ReferenceEquals($c.rootParseOriginal,$EffectOriginal) -and
      [object]::ReferenceEquals($EffectOriginal.sourceOriginal,$c.subjectOriginals['input-root']))
  } elseif ($Phase -ceq 'plan-json-parse') {
    $associated=($c.slot -cin @('cli39.full-source-materialization.v2','cli39.full-source-git-comparison.v2',
      'cli39.node-keyring-preparation.v1') -and
      [object]::ReferenceEquals($c.pythonPlanParseOriginal,$EffectOriginal) -and
      [object]::ReferenceEquals($EffectOriginal.sourceOriginal,$c.subjectOriginals['plan']))
  } elseif ($Phase -ceq 'report-json-parse') {
    $associated=Test-Cli39PythonReportOriginal32 $p $EffectOriginal
  } elseif ($Phase -ceq 'output-read') {
    foreach ($output in $c.outputOriginals) {
      if ([object]::ReferenceEquals($output,$EffectOriginal)) { $associated=$true; break }
    }
  } else {
    if (-not $c.ContainsKey('disposalOriginal') -or $null -eq $c.disposalOriginal) { throw 'Preowned actual disposal roster required.' }
    foreach ($resourceOriginal in $c.disposalOriginal.resources) {
      if ([object]::ReferenceEquals($resourceOriginal,$EffectOriginal)) {
        $associated=$true; break
      }
    }
    if ($associated -and $EffectOriginal.objectOriginal -isnot [IO.FileStream] -and
        $EffectOriginal.objectOriginal -isnot [Security.Cryptography.HashAlgorithm] -and
        $EffectOriginal.objectOriginal -isnot [System.Text.Json.JsonDocument]) { throw 'Fixed stream/hash/plan-document disposal only.' }
    if ($associated -and $EffectOriginal.objectOriginal -is [System.Text.Json.JsonDocument]) {
      $documentSource=$null
      if($rootPhase){$documentSource=$c.rootParseOriginal}
      elseif($c.ContainsKey('pythonPlanParseOriginal')){$documentSource=$c.pythonPlanParseOriginal}
      if($c.ContainsKey('pythonCompletionOriginal') -and $null -ne $c.pythonCompletionOriginal){
        foreach($report in $c.pythonCompletionOriginal.reportsOriginal){
          if([object]::ReferenceEquals($EffectOriginal.sourceOriginal,$report)){$documentSource=$report;break}
        }
      }
      if($null -eq $documentSource -or
          -not [object]::ReferenceEquals($EffectOriginal.sourceOriginal,$documentSource) -or
          -not [object]::ReferenceEquals($EffectOriginal.objectOriginal,$documentSource.documentOriginal)) {
        throw 'Only the SAME actual held-plan/root document may enter fixed document disposal.'
      }
    }
  }
  if (-not $associated) { throw 'Fixed effect must be the SAME enrolled actual subject/output/resource original.' }
  if ($rootPhase -and $Phase -ceq 'subject-read-hash' -and $EffectOriginal.role -ceq 'input-root' -and
      -not [object]::ReferenceEquals($EffectOriginal,$c.subjectOriginals['input-root'])) {throw 'SAME actual original input root required.'}
  if ($rootPhase -and $Phase -ceq 'subject-read-hash' -and $EffectOriginal.role -cne 'input-root' -and
      -not (Test-Cli39HeldMemberSubject32 $p $EffectOriginal)) {
    throw 'Only SAME input root or actual closed held member may enter this fixed read.'
  }
  if ($c.slot -ceq 'cli39.windows-publisher-trust.v1' -and $Phase -ceq 'subject-read-hash' -and
      $EffectOriginal.role -cnotin @('publisher-subject','wintrust','expected-publisher-cert')) {
    throw 'Only the three fixed publisher subjects may enter managed acquisition.'
  }
  if ($EffectOriginal.ContainsKey('phaseWorkerOriginal')) { throw 'Each source-owned effect has exactly one original worker, including failed construction.' }
  if (($rootPhase -and $c.geometry.stackMode -cne 'clr-default') -or
      (-not $rootPhase -and ($c.geometry.workerStackBytes -le 0 -or $c.geometry.workerStackBytes -gt [int]::MaxValue))) { throw 'Admitted source worker stack semantics required.' }
  $phaseRoster=$c.managedPhases;$phaseLimit=$c.geometry.managedPhaseCount
  if($treePhase){$phaseRoster=$c.pythonTreeOriginal.managedPhasesOriginal;$phaseLimit=$c.pythonTreeOriginal.maximumManagedPhases}
  if($null -eq $phaseRoster){throw 'Preowned finite phase roster required before worker construction.'}
  if($phaseRoster.Count -ge $phaseLimit){throw 'Original finite phase roster exhausted.'}
  if ($Phase -cnotin @('resource-dispose','tree-resource-dispose','plan-json-parse','root-json-parse','report-json-parse') -and
      ($null -eq $EffectOriginal.bytes -or
        ($EffectOriginal.bytes.Length -le 0 -and $Phase -cne 'tree-buffer-hash'))) { throw 'Actual preowned read buffer required.' }
  $w=@{ preparation=$p; effectOriginal=$EffectOriginal; phase=$Phase;rosterOriginal=$phaseRoster;
    outputs=[object[]]::new(4); results=[int[]]::new(8); context=$null;
    method=$null; il=$null; body=$null; thread=$null;
    startAttempted=$false; startReturned=$false; startException=$null; deadlineRefused=$false;
    terminalJoined=$false; completionAttempted=$false; completionException=$null;
    pendingActive=$false; multiplicityUnknown=$false;
    pending=@{ attempted=$false; returned=$false; value=$null; exception=$null };
    repeatedFalse=@{ returned=$false; repeats=0L }; terminal=@{ returned=$false };
    joinException=@{ present=$false; original=$null } }
  $phaseRoster.Add($w)
  $EffectOriginal.phaseWorkerOriginal=$w
  $path=$null; $buffer=$null; $resource=$null
  if ($Phase -cin @('resource-dispose','tree-resource-dispose')) { $resource=$EffectOriginal.objectOriginal }
  elseif ($Phase -cnotin @('plan-json-parse','root-json-parse','report-json-parse')) { $path=$EffectOriginal.path; $buffer=$EffectOriginal.bytes }
  $w.context=[object[]]@($path,$buffer,$resource,$w.outputs,$w.results)
  if ($Phase -cin @('plan-json-parse','root-json-parse','report-json-parse')) {
    $r=$EffectOriginal.sourceOriginal
    if (-not $r.eof -or $r.used -le 0 -or $null -eq $r.bytes) { throw 'Complete SAME held plan required before parser construction.' }
    $EffectOriginal.memoryOriginal=[ReadOnlyMemory[byte]]::new($r.bytes,0,$r.used)
    $EffectOriginal.optionsOriginal=[System.Text.Json.JsonDocumentOptions]::new()
    # Fixed plans contain an object and at most one nested input-pin object.
    # Fixed report writers reach object/array/row/PAX-object depth four.
    # Root grammar has at most ten source-declared nesting levels. This is a
    # grammar limit, not a native parser/allocator/stack fit assertion.
    $EffectOriginal.optionsOriginal.MaxDepth=if($rootPhase){10}elseif($Phase -ceq 'report-json-parse'){4}else{3}
    $w.context[0]=$EffectOriginal.memoryOriginal; $w.context[1]=$EffectOriginal.optionsOriginal
  }
  $w.method=[Reflection.Emit.DynamicMethod]::new('Cli39FixedManagedPhase',[void],
    [Type[]]@([object]),[Threading.Thread].Module,$true)
  $il=$w.method.GetILGenerator(); $w.il=$il
  $stream=$il.DeclareLocal([IO.FileStream]); $hash=$il.DeclareLocal([Security.Cryptography.HashAlgorithm])
  $bufferLocal=$il.DeclareLocal([byte[]]); $used=$il.DeclareLocal([int]); $count=$il.DeclareLocal([int])
  $exception=$il.DeclareLocal([Exception])
  # These emitter helpers execute only on the coordinator while constructing
  # the fixed body; no delegate/scriptblock from a caller becomes an effect.
  function Emit-Cli39Context26([int]$Index,[Type]$Type) {
    $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0)
    $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
    $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
    $il.Emit([Reflection.Emit.OpCodes]::Castclass,$Type)
  }
  function Emit-Cli39Scalar26([int]$Index,[int]$Value) {
    Emit-Cli39Context26 4 ([int[]])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Index)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,$Value)
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
  }
  $null=$il.BeginExceptionBlock()
  Emit-Cli39Scalar26 0 1
  if ($Phase -cin @('resource-dispose','tree-resource-dispose')) {
    Emit-Cli39Context26 2 ([IDisposable])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[IDisposable].GetMethod('Dispose'))
  } elseif ($Phase -cin @('plan-json-parse','root-json-parse','report-json-parse')) {
    Emit-Cli39Context26 3 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
    foreach ($pair in @(@(0,[ReadOnlyMemory[byte]]),@(1,[System.Text.Json.JsonDocumentOptions]))) {
      $il.Emit([Reflection.Emit.OpCodes]::Ldarg_0); $il.Emit([Reflection.Emit.OpCodes]::Castclass,[object[]])
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int]$pair[0]); $il.Emit([Reflection.Emit.OpCodes]::Ldelem_Ref)
      $il.Emit([Reflection.Emit.OpCodes]::Unbox_Any,[Type]$pair[1])
    }
    # Distinct original entry is published immediately before Parse, after
    # actual argument unboxing. A null document after a throw is not no effect.
    Emit-Cli39Scalar26 5 1
    $il.Emit([Reflection.Emit.OpCodes]::Call,[System.Text.Json.JsonDocument].GetMethod('Parse',
      [Type[]]@([ReadOnlyMemory[byte]],[System.Text.Json.JsonDocumentOptions])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    Emit-Cli39Scalar26 6 1
  } elseif($Phase -ceq 'tree-buffer-hash') {
    Emit-Cli39Scalar26 5 1
    $factory=if($EffectOriginal.algorithmOriginal -ceq 'SHA256'){[Security.Cryptography.SHA256]}else{[Security.Cryptography.SHA1]}
    $il.Emit([Reflection.Emit.OpCodes]::Call,$factory.GetMethod('Create',[Type[]]@()))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$hash)
    Emit-Cli39Context26 3 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$hash);$il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    Emit-Cli39Scalar26 6 1
    Emit-Cli39Context26 3 ([object[]]);$il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$hash);Emit-Cli39Context26 1 ([byte[]])
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Security.Cryptography.HashAlgorithm].GetMethod('ComputeHash',[Type[]]@([byte[]])))
    $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
  } else {
    Emit-Cli39Context26 1 ([byte[]]); $il.Emit([Reflection.Emit.OpCodes]::Stloc,$bufferLocal)
    Emit-Cli39Scalar26 1 1
    Emit-Cli39Context26 0 ([string])
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][IO.FileMode]::Open)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][IO.FileAccess]::Read)
    $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4,[int][IO.FileShare]::Read)
    $il.Emit([Reflection.Emit.OpCodes]::Newobj,[IO.FileStream].GetConstructor([Type[]]@([string],[IO.FileMode],[IO.FileAccess],[IO.FileShare])))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$stream)
    Emit-Cli39Context26 3 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$stream); $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    Emit-Cli39Scalar26 2 1
    $loop=$il.DefineLabel(); $after=$il.DefineLabel(); $notEof=$il.DefineLabel()
    $il.MarkLabel($loop)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$used)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$bufferLocal); $il.Emit([Reflection.Emit.OpCodes]::Ldlen)
    $il.Emit([Reflection.Emit.OpCodes]::Conv_I4); $il.Emit([Reflection.Emit.OpCodes]::Bge,$after)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$stream)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$bufferLocal); $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$used)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$bufferLocal); $il.Emit([Reflection.Emit.OpCodes]::Ldlen)
    $il.Emit([Reflection.Emit.OpCodes]::Conv_I4); $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$used)
    $il.Emit([Reflection.Emit.OpCodes]::Sub)
    $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[IO.FileStream].GetMethod('Read',[Type[]]@([byte[]],[int],[int])))
    $il.Emit([Reflection.Emit.OpCodes]::Stloc,$count)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$count); $il.Emit([Reflection.Emit.OpCodes]::Brtrue,$notEof)
    Emit-Cli39Scalar26 4 1; $il.Emit([Reflection.Emit.OpCodes]::Br,$after)
    $il.MarkLabel($notEof)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$used); $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$count)
    $il.Emit([Reflection.Emit.OpCodes]::Add); $il.Emit([Reflection.Emit.OpCodes]::Stloc,$used)
    Emit-Cli39Context26 4 ([int[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3)
    $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$used); $il.Emit([Reflection.Emit.OpCodes]::Stelem_I4)
    $il.Emit([Reflection.Emit.OpCodes]::Br,$loop); $il.MarkLabel($after)
    if ($Phase -cin @('subject-read-hash','tree-read-hash')) {
      Emit-Cli39Scalar26 5 1
      $il.Emit([Reflection.Emit.OpCodes]::Call,[Security.Cryptography.SHA256].GetMethod('Create',[Type[]]@()))
      $il.Emit([Reflection.Emit.OpCodes]::Stloc,$hash)
      Emit-Cli39Context26 3 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_1)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$hash); $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
      Emit-Cli39Scalar26 6 1
      Emit-Cli39Context26 3 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_2)
      $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$hash); $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$bufferLocal)
      $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_0); $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$used)
      $il.Emit([Reflection.Emit.OpCodes]::Callvirt,[Security.Cryptography.HashAlgorithm].GetMethod('ComputeHash',[Type[]]@([byte[]],[int],[int])))
      $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
    }
  }
  Emit-Cli39Scalar26 7 1
  $il.BeginCatchBlock([Exception]); $il.Emit([Reflection.Emit.OpCodes]::Stloc,$exception)
  Emit-Cli39Context26 3 ([object[]]); $il.Emit([Reflection.Emit.OpCodes]::Ldc_I4_3)
  $il.Emit([Reflection.Emit.OpCodes]::Ldloc,$exception); $il.Emit([Reflection.Emit.OpCodes]::Stelem_Ref)
  $il.EndExceptionBlock(); $il.Emit([Reflection.Emit.OpCodes]::Ret)
  $w.body=$w.method.CreateDelegate([Threading.ParameterizedThreadStart])
  if($rootPhase){$w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body)}
  else{$w.thread=[Threading.Thread]::new([Threading.ParameterizedThreadStart]$w.body,[int]$c.geometry.workerStackBytes)}
  $w.thread.IsBackground=$false
  return $w
}

function Start-Cli39ManagedPhase26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal
  if ($w.startAttempted -or -not (Test-Cli39ManagedPhaseRoster26 $w)) { throw 'Original once-only phase required.' }
  if($w.phase -cnotin @('resource-dispose','tree-resource-dispose') -and
      $w.preparation.consumerOriginal.ContainsKey('disposalOriginal') -and
      $null -ne $w.preparation.consumerOriginal.disposalOriginal){throw 'Actual disposal enrollment forbids future managed acquisition/parser starts.'}
  Observe-OwnedPrerequisiteDeadline $w.preparation
  if ($w.preparation.deadlineReported -and $w.phase -cnotin @('resource-dispose','tree-resource-dispose')) {
    $w.deadlineRefused=$true
    throw 'Original deadline forbids a new acquisition; existing originals remain retained.'
  }
  $w.startAttempted=$true
  try { $w.thread.Start($w.context); $w.startReturned=$true }
  catch { $w.startException=$_; $_.Exception.Data['originalPreparation']=$w.preparation; throw }
  return $w
}

function Observe-Cli39ManagedPhase26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal; $p=$w.preparation; $o=$w.pending
  if (-not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not (Test-Cli39ManagedPhaseRoster26 $w) -or -not $w.startAttempted -or
      $w.terminalJoined -or $w.pendingActive -or $w.multiplicityUnknown -or $w.joinException.present) { throw 'Same live original phase observation required.' }
  Observe-OwnedPrerequisiteDeadline $p
  $w.pendingActive=$true
  $o.attempted=$true; $o.returned=$false; $o.value=$null; $o.exception=$null
  try { $o.value=$w.thread.Join(0); $o.returned=$true }
  catch { $o.exception=$_; $w.joinException.present=$true; $w.joinException.original=$_; throw }
  if ($o.value) { $w.terminal.returned=$true; $w.terminalJoined=$true; $w.pendingActive=$false }
  else {
    $w.repeatedFalse.returned=$true
    if ($w.repeatedFalse.repeats -eq [long]::MaxValue) { $w.multiplicityUnknown=$true }
    else { $w.repeatedFalse.repeats++; $w.pendingActive=$false }
  }
  # No effect outputs are consumed or disposed until actual terminal join.
  # Deadline notification/continued custody belongs to the shared coordinator;
  # this observer cannot cancel, free a pending original or authorize success.
  return $o
}

function Complete-Cli39ManagedPhase26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  $w=$WorkerOriginal; $p=$w.preparation; $r=$w.effectOriginal
  if (-not $p.receiver.frames.ContainsKey($p.key) -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not (Test-Cli39ManagedPhaseRoster26 $w) -or
      -not $w.terminalJoined -or $w.thread.IsAlive -or $w.completionAttempted) { throw 'Actual original terminal phase completion required.' }
  $w.completionAttempted=$true
  # Publish originals before interpretation. Failed constructors and partial
  # IO/hash outcomes stay reachable in BOTH actual worker and effect records.
  if ($w.phase -cin @('resource-dispose','tree-resource-dispose')) {
    $r.disposalAttempted=($w.results[0] -eq 1)
    $r.disposalReturned=($w.results[7] -eq 1)
    $r.disposalOriginal=$w.outputs[3]
    $r.disposalWorkerOriginal=$w
  } elseif ($w.phase -cin @('plan-json-parse','root-json-parse','report-json-parse')) {
    $r.documentOriginal=$w.outputs[2]; $r.exception=$w.outputs[3]
    $r.parserCallAttempted=($w.results[5] -eq 1)
    $r.parserCallReturned=($w.results[6] -eq 1)
    $r.parserPartialUnknown=($r.parserCallAttempted -and -not $r.parserCallReturned)
    $r.parserPartialDisposition=if($r.parserPartialUnknown){'UNRESOLVED_CLR_PARSE_PARTIAL_RETAINED_HOST_EXIT'}else{'ACTUAL_RETURN_OR_NO_PARSE_ENTRY'}
    $r.effectReturned=($w.results[7] -eq 1)
  } elseif($w.phase -ceq 'tree-buffer-hash') {
    $r.hashOriginal=$w.outputs[1];$r.digestOriginal=$w.outputs[2];$r.exception=$w.outputs[3]
    $r.hashAttempted=($w.results[5] -eq 1);$r.hashReturned=($w.results[6] -eq 1)
    $r.effectReturned=($w.results[7] -eq 1)
  } else {
    $r.stream=$w.outputs[0]; $r.hashOriginal=$w.outputs[1]
    $r.digestOriginal=$w.outputs[2]; $r.exception=$w.outputs[3]
    $r.openAttempted=($w.results[1] -eq 1); $r.openReturned=($w.results[2] -eq 1)
    $r.used=$w.results[3]; $r.eof=($w.results[4] -eq 1)
    $r.hashAttempted=($w.results[5] -eq 1); $r.hashReturned=($w.results[6] -eq 1)
    $r.readWorkerOriginal=$w
    if ($null -ne $r.stream) { $r.handleOriginal=$r.stream.SafeFileHandle }
  }
  Observe-OwnedPrerequisiteDeadline $p
  if ($null -ne $w.outputs[3]) {
    $w.completionException=$w.outputs[3]
    throw $w.outputs[3]
  }
  if ($w.results[7] -ne 1) { throw 'Terminal worker has no complete original effect result.' }
  # This exposes real completion only; deadlineReported still denies useful
  # command success and no native/process/input admission follows this return.
  return $r
}

function Wait-Cli39ManagedPhase26 {
  param([Parameter(Mandatory)]$WorkerOriginal)
  # Coordinator-only fixed control: every observation uses the SAME original
  # deadline. Yield does not wait for the effect or abandon pending originals.
  while (-not $WorkerOriginal.terminalJoined) {
    $null=Observe-Cli39ManagedPhase26 $WorkerOriginal
    if (-not $WorkerOriginal.terminalJoined) { $null=[Threading.Thread]::Yield() }
  }
  return Complete-Cli39ManagedPhase26 $WorkerOriginal
}

