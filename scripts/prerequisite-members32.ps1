# Closed file-member custody. Paths/digests never issue source/native authority.
function New-Cli39HeldMemberBook32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$c=$p.consumerOriginal
  if(-not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or -not $root.structuralReturned -or
      $c.slot -cne 'cli39.source-input-host.v1' -or $root.ContainsKey('heldMemberBookOriginal') -or
      $c.ContainsKey('disposalOriginal') -or $c.geometry.managedPhaseCount -ne 5){throw 'SAME once-only root member enrollment required.'}
  $book=@{hostOwnerOriginal=$p;rootCaptureOriginal=$root;consumerOriginal=$c;
    originalManagedRosterOriginal=$c.managedPhases;replacementManagedRosterOriginal=$null;
    originalManagedPhaseCount=5;managedPhaseCount=5;resourceCapacity=3;memberCount=0;
    recordsOriginal=$null;constructorTerminal=$false;constructorFailure=$null;rosterPublished=$false;
    pendingMemberOriginal=$null;memberConstructorUnknown=$false;metadataReturned=$false;
    readAttempted=$false;readReturned=$false;exceptionOriginal=$null;
    state='HELD_FILE_CUSTODY_ONLY_NATIVE_PROVENANCE_ASSOCIATIONS_PENDING'}
  $root.heldMemberBookOriginal=$book
  try{
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline forbids member book construction.'}
    [long]$count=0
    foreach($domain in @('sourceClosure','startupClosure','loadedClosure')){$count+=$root.fieldMapOriginal[$domain].GetArrayLength()}
    if($count -gt (([int]::MaxValue-5)/3)){throw 'Actual checked complete member phase geometry unsupported.'}
    $book.memberCount=[int]$count;$book.managedPhaseCount=[int](5+3*$count);$book.resourceCapacity=[int](3+2*$count)
    $book.recordsOriginal=[Collections.Generic.List[object]]::new([int]$count)
    $book.replacementManagedRosterOriginal=[Collections.Generic.List[object]]::new($book.managedPhaseCount)
    foreach($worker in $book.originalManagedRosterOriginal){$book.replacementManagedRosterOriginal.Add($worker)}
    if($book.replacementManagedRosterOriginal.Count -ne $book.originalManagedRosterOriginal.Count){throw 'Original root worker custody copy incomplete.'}
    $c.managedPhases=$book.replacementManagedRosterOriginal;$c.geometry.managedPhaseCount=$book.managedPhaseCount;$book.rosterPublished=$true
    return $book
  }catch{$book.constructorFailure=$_;throw}finally{$book.constructorTerminal=$true}
}
function Read-Cli39HeldMemberMetadata32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$c=$p.consumerOriginal;$book=New-Cli39HeldMemberBook32 $p
  try{
    foreach($domain in @('sourceClosure','startupClosure','loadedClosure')){
      $kind=switch -CaseSensitive ($domain){'sourceClosure'{'source-file'}'startupClosure'{'startup-file'}'loadedClosure'{'loaded-file'}}
      $role=switch -CaseSensitive ($domain){'sourceClosure'{'root-source-member'}'startupClosure'{'root-startup-member'}'loadedClosure'{'root-loaded-member'}}
      $ordinal=0
      foreach($element in $root.fieldMapOriginal[$domain].EnumerateArray()){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline ended member metadata reader.'}
        $book.memberConstructorUnknown=$true
        $r=@{bookOriginal=$book;domainOriginal=$domain;ordinalOriginal=$ordinal;elementOriginal=$element;
          expectedKindOriginal=$kind;roleOriginal=$role;propertiesOriginal=$null;fieldsOriginal=$null;
          subjectOriginal=$null;bytesExpected=0L;sha256Expected=$null;fileQueryOriginal=$null;
          constructorTerminal=$false;constructorFailure=$null;returned=$false;exceptionOriginal=$null}
        $book.pendingMemberOriginal=$r;$book.recordsOriginal.Add($r);$book.memberConstructorUnknown=$false
        try{
          $r.propertiesOriginal=[Collections.Generic.List[object]]::new(5)
          $r.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
          if($element.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Closed held member object required.'}
          foreach($property in $element.EnumerateObject()){
            $r.propertiesOriginal.Add($property)
            if($property.Name -cnotin @('kind','path','bytes','sha256') -or $r.fieldsOriginal.ContainsKey($property.Name)){throw 'Unknown/duplicate held member field retained.'}
            $r.fieldsOriginal.Add($property.Name,$property.Value)
          }
          if($r.fieldsOriginal.Count -ne 4){throw 'Complete closed held member metadata required.'}
          foreach($key in @('kind','path','sha256')){if($r.fieldsOriginal[$key].ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Exact member string required.'}}
          $path=$r.fieldsOriginal['path'].GetString();$sha=$r.fieldsOriginal['sha256'].GetString();$extent=0L;$size=$r.fieldsOriginal['bytes']
          if($r.fieldsOriginal['kind'].GetString() -cne $kind -or [string]::IsNullOrEmpty($path) -or
              $path.Contains([char]0) -or -not [IO.Path]::IsPathFullyQualified($path) -or $sha -cnotmatch '^[0-9a-f]{64}$' -or
              $size.ValueKind -ne [System.Text.Json.JsonValueKind]::Number -or $size.GetRawText() -cnotmatch '^(0|[1-9][0-9]*)$' -or
              -not $size.TryGetInt64([ref]$extent) -or $extent -lt 0 -or $extent -ge [int]::MaxValue){throw 'Canonical exact held member custody metadata required.'}
          $r.bytesExpected=$extent;$r.sha256Expected=$sha
          $subject=@{role=$role;memberOriginal=$r;path=$path;stream=$null;handleOriginal=$null;bytes=$null;
            openAttempted=$false;openReturned=$false;used=0;eof=$false;hashAttempted=$false;hashReturned=$false;
            hashOriginal=$null;digest=$null;exception=$null}
          $r.subjectOriginal=$subject;$c.subjectOriginals.Add(($role+'-'+$ordinal),$subject)
          $subject.bytes=[byte[]]::new([int]($extent+1));$r.returned=$true
        }catch{$r.constructorFailure=$_;$r.exceptionOriginal=$_;throw}finally{$r.constructorTerminal=$true}
        $ordinal++
      }
    }
    if($book.recordsOriginal.Count -ne $book.memberCount){throw 'Complete original member roster count differs.'}
    $book.metadataReturned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}
}
function Test-Cli39HeldMemberSubject32 {
  param($HostOwnerOriginal,$SubjectOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$c=$p.consumerOriginal;$s=$SubjectOriginal
  if($null -eq $root -or -not $root.ContainsKey('heldMemberBookOriginal') -or $null -eq $s){return $false}
  $b=$root.heldMemberBookOriginal;$r=$s.memberOriginal
  if($null -eq $r -or $null -eq $b.recordsOriginal -or -not $b.recordsOriginal.Contains($r) -or
      -not $b.constructorTerminal -or $null -ne $b.constructorFailure -or -not $b.metadataReturned -or
      -not $b.rosterPublished -or -not $b.readAttempted -or $b.memberConstructorUnknown -or
      -not [object]::ReferenceEquals($c.managedPhases,$b.replacementManagedRosterOriginal) -or
      $c.geometry.managedPhaseCount -ne $b.managedPhaseCount -or
      -not [object]::ReferenceEquals($b.hostOwnerOriginal,$p) -or -not [object]::ReferenceEquals($b.consumerOriginal,$c) -or
      -not [object]::ReferenceEquals($r.bookOriginal,$b) -or -not [object]::ReferenceEquals($r.subjectOriginal,$s) -or
      -not $r.constructorTerminal -or $null -ne $r.constructorFailure -or -not $r.returned -or
      $s.role -cne $r.roleOriginal -or $s.role -cnotin @('root-source-member','root-startup-member','root-loaded-member') -or
      $s.path -cne $r.fieldsOriginal['path'].GetString() -or $null -eq $s.bytes -or
      $s.bytes.Length -ne ($r.bytesExpected+1)){return $false}
  return ($c.subjectOriginals.ContainsKey(($r.roleOriginal+'-'+$r.ordinalOriginal)) -and
    [object]::ReferenceEquals($c.subjectOriginals[($r.roleOriginal+'-'+$r.ordinalOriginal)],$s))
}
function Read-Cli39HeldMembers32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$book=Read-Cli39HeldMemberMetadata32 $p
  if($book.readAttempted){throw 'Actual member reads cannot retry.'}
  $book.readAttempted=$true
  try{
    foreach($r in $book.recordsOriginal){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline forbids next held member read.'}
      $s=$r.subjectOriginal
      if(-not (Test-Cli39HeldMemberSubject32 $p $s)){throw 'Actual SAME closed member subject required.'}
      $w=New-Cli39ManagedPhase26 $p 'subject-read-hash' $s
      $null=Start-Cli39ManagedPhase26 $w;$null=Wait-Cli39ManagedPhase26 $w
      if(-not $s.eof -or -not $s.hashReturned -or $s.used -ne $r.bytesExpected){throw 'Actual exact member EOF/extent differs.'}
      $s.digest=[BitConverter]::ToString($s.digestOriginal).Replace('-','').ToLowerInvariant()
      if($s.digest -cne $r.sha256Expected){throw 'Actual held member digest differs; bytes/originals retained.'}
    }
    $book.readReturned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}
}