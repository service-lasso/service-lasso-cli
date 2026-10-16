# Fixed directory selectors. A structural row never grants native rights,
# output creation, principal identity, exclusion, or control-pipe authority.
function Read-Cli39NamespaceRoles32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value;$shape=$root.consumerShapeOriginal
  if($root.ContainsKey('namespaceRolesOriginal') -or $null -eq $shape -or
      -not $shape.returned -or -not $shape.constructionTerminal -or
      -not [object]::ReferenceEquals($shape.hostOwnerOriginal,$p) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME live structural consumer required for fixed directory roster.'}
  $book=@{rootCaptureOriginal=$root;hostOwnerOriginal=$p;mapsOriginal=$null;
    expectedOriginal=$null;rowsOriginal=$null;byRoleOriginal=$null;
    constructionTerminal=$false;returned=$false;exceptionOriginal=$null;
    state='STRUCTURAL_DIRECTORY_ROLES_NOT_NATIVE_AUTHORITY'}
  $root.namespaceRolesOriginal=$book
  try {
    $book.mapsOriginal=[Collections.Generic.List[object]]::new()
    $book.expectedOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
    foreach($pair in @(
        @('caller-cwd','callerWorkingDirectory'),@('host-cwd','hostWorkingDirectory'),
        @('caller-home','callerHome'),@('host-home','hostHome'),@('shared-source-root','sharedSourceRoot'))){
      Assert-Cli39ConsumerReadDeadline32 $book
      $book.expectedOriginal.Add($pair[0],(Read-Cli39ConsumerPath32 $root.fieldMapOriginal[$pair[1]]))
    }
    foreach($pair in @(@('caller-output-parent','callerOutputPrefix'),@('host-output-parent','hostOutputPrefix'))){
      $prefix=Read-Cli39ConsumerPath32 $root.fieldMapOriginal[$pair[1]]
      $parent=[IO.Path]::GetDirectoryName($prefix)
      if([string]::IsNullOrEmpty($parent) -or -not [IO.Path]::IsPathFullyQualified($parent)){throw 'Actual output prefix must select an absolute parent directory.'}
      $book.expectedOriginal.Add($pair[0],$parent)
    }
    if($root.actionOriginal -cne 'publisher'){
      $consumer=$shape.prototypeOriginal
      $book.expectedOriginal.Add('consumer-cwd',$consumer.workingDirectoryOriginal)
      $book.expectedOriginal.Add('consumer-home',$consumer.paths.home)
      $parent=[IO.Path]::GetDirectoryName($consumer.outputPrefix)
      if([string]::IsNullOrEmpty($parent) -or -not [IO.Path]::IsPathFullyQualified($parent)){throw 'Actual child output prefix requires absolute parent.'}
      $book.expectedOriginal.Add('consumer-output-parent',$parent)
      if($root.actionOriginal -cin @('materialize','compare','keyring')){
        $book.expectedOriginal.Add('consumer-output-root',$consumer.paths.outputRoot)
      }
    }
    if($root.actionOriginal -ceq 'checksum-signature'){
      $keyShape=$root.keyPolicyConsumerShapeOriginal
      if(-not $keyShape.returned -or -not $keyShape.constructionTerminal){throw 'Actual auxiliary key-policy shape required for fixed directories.'}
      $consumer=$keyShape.prototypeOriginal
      $book.expectedOriginal.Add('key-policy-cwd',$consumer.workingDirectoryOriginal)
      $book.expectedOriginal.Add('key-policy-home',$consumer.paths.home)
      $parent=[IO.Path]::GetDirectoryName($consumer.outputPrefix)
      if([string]::IsNullOrEmpty($parent) -or -not [IO.Path]::IsPathFullyQualified($parent)){throw 'Actual key-policy output parent required.'}
      $book.expectedOriginal.Add('key-policy-output-parent',$parent)
    }
    $book.rowsOriginal=[Collections.Generic.List[object]]::new($book.expectedOriginal.Count+1)
    $book.byRoleOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach($element in $root.fieldMapOriginal['namespaceClosure'].EnumerateArray()){
      $row=@{elementOriginal=$element;fieldsOriginal=$null;roleOriginal=$null;pathOriginal=$null;returned=$false}
      $book.rowsOriginal.Add($row)
      Assert-Cli39ConsumerReadDeadline32 $book
      if($book.rowsOriginal.Count -gt $book.expectedOriginal.Count){throw 'Additional namespace row retained; no generic native directory role exists.'}
      $fields=Read-Cli39ConsumerMap32 $book $element @('role','path') 'namespace-directory-row'
      $row.fieldsOriginal=$fields
      if($fields['role'].ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Directory role must be a string.'}
      $row.roleOriginal=$fields['role'].GetString()
      $row.pathOriginal=Read-Cli39ConsumerPath32 $fields['path']
      if(-not $book.expectedOriginal.ContainsKey($row.roleOriginal) -or
          $book.byRoleOriginal.ContainsKey($row.roleOriginal) -or
          $row.pathOriginal -cne $book.expectedOriginal[$row.roleOriginal]){throw 'Unknown, duplicate or different source-selected directory role/path.'}
      $book.byRoleOriginal.Add($row.roleOriginal,$row);$row.returned=$true
    }
    if($book.byRoleOriginal.Count -ne $book.expectedOriginal.Count){throw 'Full source-selected directory roster is required.'}
    $book.returned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}finally{$book.constructionTerminal=$true}
}
