# Typed derived-root lineage structure, never an authentication constructor.
function Read-Cli39ParentLineage32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)][ValidateSet('retained-caller','operation-host')][string]$Role)
  $p=$HostOwnerOriginal;$root=$p.inputRead.value
  if($root.ContainsKey('parentLineageOriginal') -or -not $root.structuralReturned){throw 'SAME once-only derived lineage reader required.'}
  $o=@{rootOriginal=$root;hostOwnerOriginal=$p;roleOriginal=$Role;propertiesOriginal=$null;fieldsOriginal=$null;
    valuesOriginal=$null;returned=$false;exceptionOriginal=$null;nativeParentAssociationOriginal=$null;authenticated=$false}
  $root.parentLineageOriginal=$o
  try{
    $keys=[string[]]@('schema','role','action','absoluteDeadlineTicks','parentInputSha256',
      'parentProcessId','parentCreationTime','parentSidHex','parentTokenId','parentAuthenticationId')
    $o.propertiesOriginal=[Collections.Generic.List[object]]::new($keys.Count+1)
    $o.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $o.valuesOriginal=@{}
    $element=$root.fieldMapOriginal['parentLineage']
    if($element.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Fixed parent lineage must be one object.'}
    foreach($property in $element.EnumerateObject()){
      Observe-OwnedPrerequisiteDeadline $p;$o.propertiesOriginal.Add($property)
      if($property.Name -cnotin $keys -or $o.fieldsOriginal.ContainsKey($property.Name)){throw 'Unknown or duplicate decoded parent lineage field.'}
      $o.fieldsOriginal.Add($property.Name,$property.Value)
    }
    if($o.fieldsOriginal.Count -ne $keys.Count){throw 'Complete fixed parent lineage required.'}
    foreach($key in $keys){
      if($o.fieldsOriginal[$key].ValueKind -ne [System.Text.Json.JsonValueKind]::String){throw 'Exact parent lineage string field required.'}
      $value=$o.fieldsOriginal[$key].GetString()
      if([string]::IsNullOrEmpty($value) -or $value.Contains([char]0)){throw 'Nonempty NUL-free parent lineage string required.'}
      $o.valuesOriginal.Add($key,$value)
    }
    $v=$o.valuesOriginal
    if($v.schema -cne 'service-lasso.cli39-parent-lineage.v1' -or $v.role -cne $Role -or
        $v.action -cne $root.actionOriginal -or $v.absoluteDeadlineTicks -cne $p.originalDeadlineTicks.ToString([Globalization.CultureInfo]::InvariantCulture) -or
        $v.parentInputSha256 -cnotmatch '^[0-9a-f]{64}$' -or $v.parentSidHex -cnotmatch '^[0-9a-f]+$' -or
        $v.parentSidHex.Length -lt 16 -or $v.parentSidHex.Length -gt 136 -or ($v.parentSidHex.Length -band 1) -ne 0){throw 'Source-selected lineage correspondence shape differs.'}
    $pidValue=0U;$creationValue=0UL;$tokenValue=0L;$authenticationValue=0L
    if($v.parentProcessId -cnotmatch '^[1-9][0-9]*$' -or -not [uint32]::TryParse($v.parentProcessId,[ref]$pidValue) -or
        $v.parentCreationTime -cnotmatch '^[1-9][0-9]*$' -or -not [uint64]::TryParse($v.parentCreationTime,[ref]$creationValue) -or
        $v.parentTokenId -cnotmatch '^(0|-?[1-9][0-9]*)$' -or -not [long]::TryParse($v.parentTokenId,[ref]$tokenValue) -or
        $v.parentAuthenticationId -cnotmatch '^(0|-?[1-9][0-9]*)$' -or -not [long]::TryParse($v.parentAuthenticationId,[ref]$authenticationValue)){
      throw 'Canonical source-native numeric lineage strings required.'
    }
    $o.parentProcessIdObserved=$pidValue;$o.parentCreationTimeObserved=$creationValue
    $o.parentTokenIdObserved=$tokenValue;$o.parentAuthenticationIdObserved=$authenticationValue
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}