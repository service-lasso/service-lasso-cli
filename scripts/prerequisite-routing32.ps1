# Closed routing observations only. These do not authenticate startup or peers.
function Read-Cli39RootRouting32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)][ValidateSet('qualification-coordinator','operation-host','retained-caller')][string]$Role,
    [Parameter(Mandatory)][AllowEmptyString()][string]$PipeName,[Parameter(Mandatory)][AllowEmptyString()][string]$InvocationToken,
    [Parameter(Mandatory)][string]$SharedSourceRoot)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($null -eq $root -or -not $root.structuralReturned -or $root.ContainsKey('routingOriginal') -or
      -not [object]::ReferenceEquals($root.resultOriginal.receiverOriginal,$p.receiver) -or
      $p.consumerOriginal.ContainsKey('disposalOriginal')){throw 'SAME once-only live root routing original required.'}
  $book=@{rootCaptureOriginal=$root;hostOwnerOriginal=$p;roleOriginal=$Role;
    pipeNameOriginal=$PipeName;invocationTokenOriginal=$InvocationToken;sharedSourceRootOriginal=$SharedSourceRoot;
    mapsOriginal=$null;controlsOriginal=$null;environmentsOriginal=$null;
    roleValuesEqual=$false;sharedSourceRootEqual=$false;returned=$false;
    constructionTerminal=$false;exceptionOriginal=$null;state='STRUCTURAL_ROUTING_NOT_AUTHENTICATION'}
  $root.routingOriginal=$book
  try {
    $book.mapsOriginal=[Collections.Generic.List[object]]::new()
    $book.controlsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $book.environmentsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    foreach($name in @('callerControl','hostControl')){
      $record=@{fieldsOriginal=$null;pipeName=$null;invocationToken=$null;returned=$false}
      $book.controlsOriginal.Add($name,$record)
      $fields=Read-Cli39ConsumerMap32 $book $root.fieldMapOriginal[$name] @('pipeName','invocationToken') $name
      $record.fieldsOriginal=$fields
      foreach($fieldName in @('pipeName','invocationToken')){
        if($fields[$fieldName].ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Fixed control values must be strings.'}
        $record[$fieldName]=$fields[$fieldName].GetString()
      }
      if($record.pipeName -cnotmatch '^service-lasso-native-producer-[0-9a-f]{32}$' -or
          $record.invocationToken -cnotmatch '^[0-9a-f]{64}$'){throw 'Fixed control routing grammar differs.'}
      $record.returned=$true
    }
    $caller=$book.controlsOriginal['callerControl'];$hostControl=$book.controlsOriginal['hostControl']
    if($caller.pipeName -ceq $hostControl.pipeName -or $caller.invocationToken -ceq $hostControl.invocationToken){throw 'Fixed roles require distinct routing originals.'}
    $selected=$hostControl;if($Role -ceq 'retained-caller'){$selected=$caller}
    $book.roleValuesEqual=$selected.pipeName -ceq $PipeName -and $selected.invocationToken -ceq $InvocationToken
    $book.sharedSourceRootEqual=(Read-Cli39ConsumerPath32 $root.fieldMapOriginal['sharedSourceRoot']) -ceq $SharedSourceRoot
    $book.ownControlApplicable=($Role -cne 'qualification-coordinator')
    if(($book.ownControlApplicable -and -not $book.roleValuesEqual) -or -not $book.sharedSourceRootEqual){throw 'Actual startup routing strings differ from SAME root structure.'}
    if(-not $book.ownControlApplicable -and ($PipeName.Length -ne 0 -or $InvocationToken.Length -ne 0)){throw 'Outer coordinator has no invented CLI ingress control.'}
    foreach($name in @('callerEnvironment','hostEnvironment')){
      $record=@{elementOriginal=$root.fieldMapOriginal[$name];propertiesOriginal=$null;valuesOriginal=$null;returned=$false;exceptionOriginal=$null}
      $book.environmentsOriginal.Add($name,$record)
      try {
        $record.propertiesOriginal=[Collections.Generic.List[object]]::new()
        $record.valuesOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::OrdinalIgnoreCase)
        if($record.elementOriginal.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Fixed Windows environment object required.'}
        foreach($property in $record.elementOriginal.EnumerateObject()){
          $record.propertiesOriginal.Add($property)
          Assert-Cli39ConsumerReadDeadline32 $book
          if([string]::IsNullOrEmpty($property.Name) -or $property.Name.IndexOfAny([char[]]@('=',[char]0)) -ge 0 -or
              $record.valuesOriginal.ContainsKey($property.Name) -or $property.Value.ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Invalid or duplicate Windows environment original.'}
          $value=$property.Value.GetString()
          if($value.IndexOf([char]0) -ge 0){throw 'Windows environment value contains NUL.'}
          $record.valuesOriginal.Add($property.Name,$value)
        }
        if($record.valuesOriginal.Count -eq 0){throw 'Existing fixed native command environment must be nonempty.'}
        $record.returned=$true
      }catch{$record.exceptionOriginal=$_;throw}
    }
    $book.returned=$true;return $book
  }catch{$book.exceptionOriginal=$_;throw}finally{$book.constructionTerminal=$true}
}
