# Literal held-member selector only. No guessed OS path or authenticity issuer.
function Read-Cli39PrincipalSelector32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($root.ContainsKey('principalSelectorOriginal') -or -not $root.structuralReturned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$p.receiver)){
    throw 'SAME once-only structural root required for principal API selector.'
  }
  $book=@{rootCaptureOriginal=$root;hostOwnerOriginal=$p;mapsOriginal=$null;fieldsOriginal=$null;
    loadedMemberOrdinalOriginal=$null;memberOriginal=$null;subjectOriginal=$null;namespaceOwnerOriginal=$null;
    bindingAttempted=$false;bindingReturned=$false;bindingExceptionOriginal=$null;
    constructionTerminal=$false;returned=$false;exceptionOriginal=$null;
    state='HELD_MEMBER_SELECTOR_NOT_OS_OR_PRINCIPAL_AUTHORITY'}
  $root.principalSelectorOriginal=$book
  try {
    $book.mapsOriginal=[Collections.Generic.List[object]]::new(1)
    $fields=Read-Cli39ConsumerMap32 $book $root.fieldMapOriginal['principalApiModule'] @('kind','loadedMemberOrdinal') 'principal-api-module'
    $book.fieldsOriginal=$fields
    if($fields['kind'].ValueKind -ne [System.Text.Json.JsonValueKind]::String -or
        $fields['kind'].GetString() -cne 'advapi32-principal-api.v1'){
      throw 'No generic principal API module selector exists.'
    }
    $ordinal=$fields['loadedMemberOrdinal'];$value=0L
    if($ordinal.ValueKind -ne [System.Text.Json.JsonValueKind]::Number -or
        $ordinal.GetRawText() -cnotmatch '^(0|[1-9][0-9]*)$' -or
        -not $ordinal.TryGetInt64([ref]$value) -or $value -ge $root.fieldMapOriginal['loadedClosure'].GetArrayLength()){
      throw 'Exact existing loaded-member ordinal required.'
    }
    $book.loadedMemberOrdinalOriginal=[int]$value;$book.returned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}finally{$book.constructionTerminal=$true}
}

function Join-Cli39PrincipalHeldMember32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$selector=$root.principalSelectorOriginal
  $members=$root.heldMemberBookOriginal;$namespace=$root.namespaceBookOriginal
  if($null -eq $selector -or -not $selector.returned -or -not $selector.constructionTerminal -or
      $selector.bindingAttempted -or -not [object]::ReferenceEquals($selector.hostOwnerOriginal,$p) -or
      -not $members.readReturned -or -not $namespace.observationReturned -or
      $p.consumerOriginal.ContainsKey('disposalOriginal') -or $null -ne $namespace.disposalOriginal){
    throw 'SAME once-only live held and native namespace observations required.'
  }
  $selector.bindingAttempted=$true
  try {
    Assert-Cli39ConsumerReadDeadline32 $selector
    foreach($member in $members.recordsOriginal){
      if($member.domainOriginal -ceq 'loadedClosure' -and $member.ordinalOriginal -eq $selector.loadedMemberOrdinalOriginal){
        if($null -ne $selector.memberOriginal){throw 'Duplicate actual loaded ordinal retained.'}
        $selector.memberOriginal=$member;$selector.subjectOriginal=$member.subjectOriginal
      }
    }
    $member=$selector.memberOriginal;$subject=$selector.subjectOriginal
    if($null -eq $member -or -not [object]::ReferenceEquals($member.bookOriginal,$members) -or
        -not $member.returned -or -not $subject.eof -or -not $subject.hashReturned -or
        $subject.used -ne $member.bytesExpected -or $subject.digest -cne $member.sha256Expected -or
        [IO.Path]::GetFileName($subject.path) -ine 'advapi32.dll' -or
        $null -eq $subject.handleOriginal -or $subject.handleOriginal.IsClosed -or
        -not [object]::ReferenceEquals($subject.stream.SafeFileHandle,$subject.handleOriginal) -or
        -not $member.fileQueryOriginal.rawReturned){throw 'Actual SAME held Advapi32 original required.'}
    foreach($owner in $namespace.pathOwnersOriginal){
      if([object]::ReferenceEquals($owner.memberOriginal,$member)){
        if($null -ne $selector.namespaceOwnerOriginal){throw 'Duplicate actual member namespace owner retained.'}
        $selector.namespaceOwnerOriginal=$owner
      }
    }
    $owner=$selector.namespaceOwnerOriginal
    if($null -eq $owner -or -not $owner.joinReturned -or $owner.leafDirectoryOriginal -or
        -not [object]::ReferenceEquals($owner.subjectOriginal,$subject) -or
        -not [object]::ReferenceEquals($owner.queryOriginal,$member.fileQueryOriginal)){
      throw 'Actual selected held member native identity join required.'
    }
    $leaf=$owner.holdsOriginal[$owner.holdsOriginal.Count-1]
    if(-not $leaf.leafOriginal -or -not $leaf.identityJoined -or $leaf.identityBytesCompared -ne 24){
      throw 'Actual full named-to-held volume/FileId join required.'
    }
    $selector.bindingReturned=$true;return $selector
  }catch{$selector.bindingExceptionOriginal=$_;throw}
}
