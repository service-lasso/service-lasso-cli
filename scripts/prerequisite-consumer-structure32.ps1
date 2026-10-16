# Structural decoding only. These retained values are never native admission,
# an issuer, a measured reservation, or an executable command original.
function Assert-Cli39ConsumerReadDeadline32 {
  param($Book)
  Observe-OwnedPrerequisiteDeadline $Book.hostOwnerOriginal
  if($Book.hostOwnerOriginal.deadlineReported){throw 'Original deadline denies further consumer structure work.'}
}

function Read-Cli39ConsumerMap32 {
  param($Book,$Element,[string[]]$Keys,[string]$Role)
  $record=@{role=$Role;elementOriginal=$Element;keysOriginal=$Keys;propertiesOriginal=$null;
    valuesOriginal=$null;returned=$false;exceptionOriginal=$null}
  $Book.mapsOriginal.Add($record)
  try {
    Assert-Cli39ConsumerReadDeadline32 $Book
    $record.propertiesOriginal=[Collections.Generic.List[object]]::new($Keys.Count+1)
    $record.valuesOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    if($Element.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Consumer field must be an object.'}
    foreach($property in $Element.EnumerateObject()){
      $record.propertiesOriginal.Add($property)
      Assert-Cli39ConsumerReadDeadline32 $Book
      if($property.Name -cnotin $Keys -or $record.valuesOriginal.ContainsKey($property.Name)){
        throw 'Unknown or duplicate consumer property.'
      }
      $record.valuesOriginal.Add($property.Name,$property.Value)
    }
    if($record.valuesOriginal.Count -ne $Keys.Count){throw 'Missing fixed consumer property.'}
    $record.returned=$true;return ,$record.valuesOriginal
  }catch{$record.exceptionOriginal=$_;throw}
}

function Read-Cli39ConsumerPositive32 {
  param($Element)
  if($Element.ValueKind -ne [System.Text.Json.JsonValueKind]::Number -or
      $Element.GetRawText() -cnotmatch '^[1-9][0-9]*$'){throw 'Canonical positive consumer extent required.'}
  $value=$Element.GetInt64()
  if($value -ge [int]::MaxValue){throw 'Fixed consumer backing exceeds its source representation.'}
  return $value
}

function Read-Cli39ConsumerPath32 {
  param($Element)
  if($Element.ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Consumer path must be a string.'}
  $value=$Element.GetString()
  if([string]::IsNullOrEmpty($value) -or $value.IndexOf([char]0) -ge 0 -or
      -not [IO.Path]::IsPathFullyQualified($value)){throw 'Absolute NUL-free consumer path required.'}
  return $value
}

function Read-Cli39ConsumerStructure32 {
  param([Parameter(Mandatory)]$RootCaptureOriginal,[Parameter(Mandatory)]$HostOwnerOriginal,[switch]$KeyPolicyPrerequisite)
  $root=$RootCaptureOriginal;$p=$HostOwnerOriginal
  $shapeKey=if($KeyPolicyPrerequisite){'keyPolicyConsumerShapeOriginal'}else{'consumerShapeOriginal'}
  $elementKey=if($KeyPolicyPrerequisite){'keyPolicyConsumer'}else{'consumer'}
  $action=if($KeyPolicyPrerequisite){'key-policy'}else{$root.actionOriginal}
  if($KeyPolicyPrerequisite -and $root.actionOriginal -cne 'checksum-signature'){throw 'Only fixed checksum action owns auxiliary key-policy consumer.'}
  if(-not [object]::ReferenceEquals($p.inputRead.value,$root) -or -not $root.structuralReturned -or
      $root.ContainsKey($shapeKey) -or $p.consumerOriginal.ContainsKey('disposalOriginal') -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$p.receiver)){
    throw 'Actual once-only live structural root required.'
  }
  $book=@{rootCaptureOriginal=$root;hostOwnerOriginal=$p;mapsOriginal=$null;
    environmentPropertiesOriginal=$null;environmentOriginal=$null;prototypeOriginal=$null;
    constructionTerminal=$false;returned=$false;exceptionOriginal=$null;
    state='STRUCTURE_ONLY_NATIVE_ADMISSION_PENDING'}
  $root[$shapeKey]=$book
  try {
    $book.mapsOriginal=[Collections.Generic.List[object]]::new()
    $publisher=$action -ceq 'publisher'
    $keys=[string[]]@('schema','subjectPins','paths','geometry','environment','workingDirectory','outputPrefix')
    if($publisher){$keys=[string[]]@('schema','subjectPins','geometry','publisherPolicy')}
    $fields=Read-Cli39ConsumerMap32 $book $root.fieldMapOriginal[$elementKey] $keys $elementKey
    if($fields['schema'].ValueKind -ne [System.Text.Json.JsonValueKind]::String -or
        $fields['schema'].GetString() -cne 'service-lasso.cli39-prerequisite-consumer.v1'){
      throw 'Fixed consumer schema differs.'
    }
    $slot=switch -CaseSensitive ($action){
      'materialize' {'cli39.full-source-materialization.v2'}
      'compare' {'cli39.full-source-git-comparison.v2'}
      'keyring' {'cli39.node-keyring-preparation.v1'}
      'key-policy' {'cli39.node-key-policy.v1'}
      'checksum-signature' {'cli39.node-checksum-signature.v1'}
      'publisher' {'cli39.windows-publisher-trust.v1'}
      default {throw 'No generic consumer shape exists.'}
    }
    $python=$action -cin @('materialize','compare','keyring')
    $roles=[string[]]@('executable','keyring','signature','listing');$phaseCount=16
    if($action -ceq 'checksum-signature'){$phaseCount=18}
    if($python){$shape=Get-Cli39PythonShape32 $slot;$roles=$shape.subjects;$phaseCount=3*$roles.Count+2*(2+$shape.artifacts.Count)+2+2*$shape.reports.Count}
    if($publisher){$roles=[string[]]@('publisher-subject','wintrust','expected-publisher-cert');$phaseCount=9}
    $prototype=@{slot=$slot;rootCaptureOriginal=$root;subjectPins=@{};paths=@{};geometry=@{};
      environmentOriginal=$null;workingDirectoryOriginal=$null;outputPrefix=$null;publisherPolicyOriginal=$null}
    $book.prototypeOriginal=$prototype
    $pins=Read-Cli39ConsumerMap32 $book $fields['subjectPins'] $roles 'subject-pins'
    foreach($role in $roles){
      Assert-Cli39ConsumerReadDeadline32 $book
      $pinFields=Read-Cli39ConsumerMap32 $book $pins[$role] @('path','bytes','sha256') $role
      $pin=@{path=$null;bytes=$null;sha256=$null;fieldsOriginal=$pinFields}
      $prototype.subjectPins.Add($role,$pin)
      $pin.path=Read-Cli39ConsumerPath32 $pinFields['path']
      $pin.bytes=Read-Cli39ConsumerPositive32 $pinFields['bytes']
      if($pinFields['sha256'].ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Subject digest must be a string.'}
      $pin.sha256=$pinFields['sha256'].GetString()
      if($pin.sha256 -cnotmatch '^[A-Fa-f0-9]{64}$'){throw 'Exact subject SHA256 required.'}
    }
    $geometryKeys=[string[]]@('managedPhaseCount','workerStackBytes')
    if($python){$geometryKeys+= 'pythonOutputBytes'}
    elseif(-not $publisher){$geometryKeys+= @('statusBytes','stderrBytes','statusLines','statusLineBytes');if($phaseCount -eq 18){$geometryKeys+='payloadBytes'}}
    $geometry=Read-Cli39ConsumerMap32 $book $fields['geometry'] $geometryKeys 'geometry'
    foreach($name in $geometryKeys){
      if($name -ceq 'pythonOutputBytes'){continue}
      $prototype.geometry.Add($name,(Read-Cli39ConsumerPositive32 $geometry[$name]))
    }
    if($prototype.geometry.managedPhaseCount -ne $phaseCount){throw 'Whole fixed consumer phase roster differs.'}
    if($python){
      $outputRoles=[string[]]@('python-stdout','python-stderr')+$shape.artifacts
      $outputFields=Read-Cli39ConsumerMap32 $book $geometry['pythonOutputBytes'] $outputRoles 'python-output-extents'
      $prototype.geometry.pythonOutputBytes=@{}
      foreach($role in $outputRoles){$prototype.geometry.pythonOutputBytes.Add($role,(Read-Cli39ConsumerPositive32 $outputFields[$role]))}
    }
    if($publisher){$prototype.publisherPolicyOriginal=$fields['publisherPolicy']}
    else {
      $pathKeys=[string[]]@('home','keyring','signature','listing')
      if($python){$pathKeys=[string[]]@('home','outputRoot')}
      elseif($phaseCount -eq 18){$pathKeys+='payload'}
      $paths=Read-Cli39ConsumerMap32 $book $fields['paths'] $pathKeys 'paths'
      foreach($name in $pathKeys){$prototype.paths.Add($name,(Read-Cli39ConsumerPath32 $paths[$name]))}
      $prototype.workingDirectoryOriginal=Read-Cli39ConsumerPath32 $fields['workingDirectory']
      $prototype.outputPrefix=Read-Cli39ConsumerPath32 $fields['outputPrefix']
      $book.environmentPropertiesOriginal=[Collections.Generic.List[object]]::new()
      $book.environmentOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::OrdinalIgnoreCase)
      $environment=$fields['environment']
      if($environment.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Fixed Windows environment must be an object.'}
      foreach($property in $environment.EnumerateObject()){
        $book.environmentPropertiesOriginal.Add($property)
        Assert-Cli39ConsumerReadDeadline32 $book
        if([string]::IsNullOrEmpty($property.Name) -or $property.Name.IndexOfAny([char[]]@('=',[char]0)) -ge 0 -or
            $book.environmentOriginal.ContainsKey($property.Name) -or $property.Value.ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Invalid or duplicate Windows environment name/value.'}
        $value=$property.Value.GetString()
        if($value.IndexOf([char]0) -ge 0){throw 'Windows environment value contains NUL.'}
        $book.environmentOriginal.Add($property.Name,$value)
      }
      if($book.environmentOriginal.Count -eq 0){throw 'Existing fixed command environment must be nonempty.'}
      $prototype.environmentOriginal=$book.environmentOriginal
    }
    $book.returned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}finally{$book.constructionTerminal=$true}
}
