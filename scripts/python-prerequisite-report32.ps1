# Fixed output-document capture, not authentication or native tree admission.
function Test-Cli39PythonReportOriginal32 {
  param($OriginalPreparation,$ReportOriginal)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$r=$ReportOriginal
  if($c.slot -cnotin @('cli39.full-source-materialization.v2','cli39.full-source-git-comparison.v2','cli39.node-keyring-preparation.v1') -or
      $null -eq $r -or $null -eq $c.pythonCompletionOriginal -or
      -not [object]::ReferenceEquals($r.completionOriginal,$c.pythonCompletionOriginal) -or
      -not $c.pythonCompletionOriginal.reportsOriginal.Contains($r) -or
      -not $c.outputOriginals.Contains($r.sourceOriginal) -or
      $r.sourceOriginal.role -cnotin (Get-Cli39PythonShape32 $c.slot).reports){return $false}
  return [object]::ReferenceEquals($r.preparationOriginal,$p)
}

function Read-Cli39PythonReportNode32 {
  param($ReportOriginal,$ElementOriginal,$ParentOriginal,[int]$Depth)
  $r=$ReportOriginal;$p=$r.preparationOriginal
  Assert-Cli39PrerequisiteUseful26 $p
  if($Depth -gt 4 -or $r.nodesOriginal.Count -ge $r.maximumNodesOriginal){throw 'Actual fixed report grammar/backing exhausted.'}
  $node=@{reportOriginal=$r;parentOriginal=$ParentOriginal;elementOriginal=$ElementOriginal;
    kindOriginal=$ElementOriginal.ValueKind;propertiesOriginal=$null;childrenOriginal=$null;
    textOriginal=$null;returned=$false;exceptionOriginal=$null}
  $r.nodesOriginal.Add($node)
  try{
    switch($ElementOriginal.ValueKind){
      ([System.Text.Json.JsonValueKind]::Object){
        $node.propertiesOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
        $node.childrenOriginal=[Collections.Generic.List[object]]::new()
        foreach($property in $ElementOriginal.EnumerateObject()){
          # Retain the rejected duplicate property before refusing its shape.
          $entry=@{propertyOriginal=$property;childOriginal=$null;returned=$false}
          $node.childrenOriginal.Add($entry)
          if($node.propertiesOriginal.ContainsKey($property.Name)){throw 'Duplicate actual output report property.'}
          $node.propertiesOriginal.Add($property.Name,$entry)
          $entry.childOriginal=Read-Cli39PythonReportNode32 $r $property.Value $node ($Depth+1)
          $entry.returned=$true
        }
      }
      ([System.Text.Json.JsonValueKind]::Array){
        $node.childrenOriginal=[Collections.Generic.List[object]]::new()
        foreach($element in $ElementOriginal.EnumerateArray()){
          $child=Read-Cli39PythonReportNode32 $r $element $node ($Depth+1)
          $node.childrenOriginal.Add($child)
        }
      }
      ([System.Text.Json.JsonValueKind]::String){$node.textOriginal=$ElementOriginal.GetString()}
      ([System.Text.Json.JsonValueKind]::Number){$node.textOriginal=$ElementOriginal.GetRawText()}
      ([System.Text.Json.JsonValueKind]::True){}
      ([System.Text.Json.JsonValueKind]::False){}
      ([System.Text.Json.JsonValueKind]::Null){}
      default{throw 'No undefined report JSON value exists.'}
    }
    $node.returned=$true;return $node
  }catch{$node.exceptionOriginal=$_;throw}
}

function Assert-Cli39ReportObjectFields32 {
  param($NodeOriginal,[string[]]$Keys)
  if($NodeOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::Object -or
      -not $NodeOriginal.returned -or $NodeOriginal.propertiesOriginal.Count -ne $Keys.Count){throw 'Complete fixed report row required.'}
  foreach($key in $NodeOriginal.propertiesOriginal.Keys){if($key -cnotin $Keys){throw 'Unknown fixed report row field.'}}
}

function Read-Cli39ReportNonnegative32 {
  param($NodeOriginal,[switch]$Bounded)
  if($NodeOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::Number -or
      $NodeOriginal.textOriginal -cnotmatch '^(0|[1-9][0-9]*)$'){throw 'Canonical nonnegative actual report number required.'}
  if($Bounded){return $NodeOriginal.elementOriginal.GetInt64()}
}

function Assert-Cli39MaterializerMembersReport32 {
  param($ReportOriginal)
  $r=$ReportOriginal;$p=$r.preparationOriginal;$c=$p.consumerOriginal
  $map=$r.rootNodeOriginal.propertiesOriginal;$plan=$c.pythonPlanParseOriginal.fieldsOriginal
  $members=$map['members'].childOriginal.childrenOriginal
  $controls=$map['controlHeaders'].childOriginal.childrenOriginal
  if([decimal]$members.Count+[decimal]$controls.Count -gt $plan['maxMembers'].GetInt64()){throw 'Actual report header roster exceeds SAME plan.'}
  [decimal]$payload=0
  foreach($row in $members){
    Assert-Cli39PrerequisiteUseful26 $p
    $fields=$row.propertiesOriginal
    $kind=$fields['kind'].childOriginal
    if($kind.kindOriginal -ne [System.Text.Json.JsonValueKind]::String -or
        $kind.textOriginal -cnotin @('30','00','35','31','32')){throw 'Unsupported actual materializer member kind.'}
    $keys=[string[]]@('headerOffset','bodyOffset','path','kind','bytes','sha256','gitBlobSHA1','headerSHA256','link','mode','uid','gid','mtime','pax')
    if($kind.textOriginal -cin @('31','32')){$keys+='target'}
    Assert-Cli39ReportObjectFields32 $row $keys
    $offset=Read-Cli39ReportNonnegative32 $fields['headerOffset'].childOriginal -Bounded
    $body=Read-Cli39ReportNonnegative32 $fields['bodyOffset'].childOriginal -Bounded
    $extent=Read-Cli39ReportNonnegative32 $fields['bytes'].childOriginal -Bounded
    if($offset%512 -ne 0 -or [decimal]$body -ne [decimal]$offset+512 -or
        [decimal]$body+[decimal]$extent -gt $plan['maxTarBytes'].GetInt64() -or
        $extent -gt $plan['maxMemberBytes'].GetInt64() -or
        ($kind.textOriginal -cnotin @('30','00') -and $extent -ne 0)){throw 'Actual report tar extent contradicts SAME plan/source geometry.'}
    $payload+=$extent
    foreach($key in @('mode','uid','gid','mtime')){$null=Read-Cli39ReportNonnegative32 $fields[$key].childOriginal}
    foreach($key in @('path','link','sha256','headerSHA256')){
      if($fields[$key].childOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::String){throw 'Fixed actual report member string required.'}
    }
    foreach($key in @('sha256','headerSHA256')){
      if($fields[$key].childOriginal.textOriginal -cnotmatch '^[a-f0-9]{64}$'){throw 'Actual report member digest shape differs.'}
    }
    $blob=$fields['gitBlobSHA1'].childOriginal
    if($kind.textOriginal -ceq '35'){
      if($blob.kindOriginal -ne [System.Text.Json.JsonValueKind]::Null){throw 'Directory report blob must be null.'}
    }elseif($blob.kindOriginal -ne [System.Text.Json.JsonValueKind]::String -or $blob.textOriginal -cnotmatch '^[a-f0-9]{40}$'){throw 'Actual member report Git blob shape differs.'}
    if($kind.textOriginal -cin @('31','32')){
      if($fields['target'].childOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::String){throw 'Actual link report target required.'}
    }elseif($fields['link'].childOriginal.textOriginal -cne ''){throw 'Nonlink report cannot carry link metadata.'}
    $pax=$fields['pax'].childOriginal
    if($pax.kindOriginal -ne [System.Text.Json.JsonValueKind]::Object){throw 'Actual PAX report object required.'}
    foreach($name in $pax.propertiesOriginal.Keys){
      if($name -cnotin @('path','linkpath','size','mtime','atime','ctime','uid','gid','uname','gname') -or
          $pax.propertiesOriginal[$name].childOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::String){throw 'Actual selected PAX report field differs.'}
    }
  }
  foreach($row in $controls){
    Assert-Cli39PrerequisiteUseful26 $p
    Assert-Cli39ReportObjectFields32 $row @('headerOffset','kind','bytes','sha256')
    $fields=$row.propertiesOriginal
    $offset=Read-Cli39ReportNonnegative32 $fields['headerOffset'].childOriginal -Bounded
    $extent=Read-Cli39ReportNonnegative32 $fields['bytes'].childOriginal -Bounded
    if($offset%512 -ne 0 -or [decimal]$offset+512+[decimal]$extent -gt $plan['maxTarBytes'].GetInt64() -or
        $extent -gt $plan['maxMemberBytes'].GetInt64() -or
        $fields['kind'].childOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::String -or
        $fields['kind'].childOriginal.textOriginal -cnotin @('x','g','L','K') -or
        $fields['sha256'].childOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::String -or
        $fields['sha256'].childOriginal.textOriginal -cnotmatch '^[a-f0-9]{64}$'){throw 'Actual control report shape/extent differs.'}
  }
  if($payload -gt $plan['maxPayloadBytes'].GetInt64() -or
      $payload -ne $map['payloadBytes'].childOriginal.elementOriginal.GetInt64()){throw 'Actual report payload sum differs.'}
}

function Read-Cli39PythonReport32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$OutputOriginal)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$completion=$c.pythonCompletionOriginal
  if($null -eq $completion -or -not $c.outputOriginals.Contains($OutputOriginal) -or
      -not $completion.outputs.Contains($OutputOriginal) -or -not $OutputOriginal.eof -or $OutputOriginal.used -le 0 -or
      $c.ContainsKey('disposalOriginal') -or $OutputOriginal.role -cnotin (Get-Cli39PythonShape32 $c.slot).reports){throw 'SAME fixed held report before disposal required.'}
  foreach($prior in $completion.reportsOriginal){if([object]::ReferenceEquals($prior.sourceOriginal,$OutputOriginal)){throw 'Actual report parser cannot retry.'}}
  $r=@{preparationOriginal=$p;completionOriginal=$completion;sourceOriginal=$OutputOriginal;
    memoryOriginal=$null;optionsOriginal=$null;documentOriginal=$null;effectReturned=$false;exception=$null;
    parserCallAttempted=$false;parserCallReturned=$false;parserPartialUnknown=$false;parserPartialDisposition='NOT_ENTERED';
    nodesOriginal=$null;maximumNodesOriginal=0;rootNodeOriginal=$null;keysOriginal=$null;stateOriginal=$null;
    returned=$false;exceptionOriginal=$null;state='STRUCTURAL_REPORT_ONLY_NOT_SOURCE_OR_TREE_ACCEPTANCE'}
  $completion.reportsOriginal.Add($r)
  try{
    $book=$c.pythonReportReservationOriginal
    if($null -eq $book -or -not $book.returned -or
        -not [object]::ReferenceEquals($book.preparationOriginal,$p) -or
        -not [object]::ReferenceEquals($book.reservationOriginal,$p.reservation) -or
        -not [object]::ReferenceEquals($book.budgetOriginal,$p.reservation.budget) -or
        $book.recordsOriginal.Count -ge $book.shapeOriginal.reports.Count -or
        $OutputOriginal.used -gt $c.geometry.pythonOutputBytes[$OutputOriginal.role]){throw 'SAME precharged fixed report backing required.'}
    $charge=@{reportOriginal=$r;bookOriginal=$book;bytesOriginal=$OutputOriginal.used;chargeBytes=[decimal]0;returned=$false}
    $book.recordsOriginal.Add($charge);$r.chargeOriginal=$charge
    $charge.chargeBytes=[decimal]$book.budgetOriginal.pythonReportCapturePerReportChargeBytes+
      [decimal]$OutputOriginal.used*[decimal]$book.budgetOriginal.pythonReportCapturePerByteChargeBytes
    $book.usedChargeBytes+=$charge.chargeBytes
    if($book.usedChargeBytes -gt $book.plannedChargeBytes -or
        $book.usedChargeBytes -gt $book.budgetOriginal.pythonReportCaptureReservationBytes){throw 'Actual retained report charge exceeds SAME reserved overlap.'}
    $charge.returned=$true
    # Every JSON value requires at least one input byte. This finite source
    # geometry does not certify allocator/default-stack/retained overlap fit.
    $r.maximumNodesOriginal=$OutputOriginal.used
    $r.nodesOriginal=[Collections.Generic.List[object]]::new($r.maximumNodesOriginal)
    $w=New-Cli39ManagedPhase26 $p 'report-json-parse' $r
    $null=Start-Cli39ManagedPhase26 $w;$null=Wait-Cli39ManagedPhase26 $w
    $r.rootNodeOriginal=Read-Cli39PythonReportNode32 $r $r.documentOriginal.RootElement $null 0
    $keys=$null;$expectedState=$null
    switch -CaseSensitive($OutputOriginal.role){
      'decompression.json'{$keys=[string[]]@('compression','compressedBytes','tarBytes','sha256')}
      'members.json'{$keys=[string[]]@('members','controlHeaders','payloadBytes')}
      'complete.json'{$keys=[string[]]@('state','memberCount','payloadBytes');$expectedState='MATERIALIZED_NOT_SOURCE_OR_BUILD_ADMISSION'}
      'complete-comparison.json'{$keys=[string[]]@('state','originalEntries','releaseEntries','matched','differences','rawReferenceBlobsVerified','referenceEntries','referenceMatched','referenceDifferences','archiveDifferences','releaseTransformationsVerified','localSignatureVerified');$expectedState='STRUCTURAL_COMPARISON_NOT_AUTHENTICATION_OR_SOURCE_ADMISSION'}
      'packets.json'{$keys=[string[]]@('state','armorHeaders','keys','packets','selfSignaturesVerified','subkeyPolicyVerified','revocationVerified');$expectedState='CANDIDATE_KEYRING_STRUCTURAL_ONLY'}
      default{throw 'No generic output report parser exists.'}
    }
    $r.keysOriginal=$keys;$map=$r.rootNodeOriginal.propertiesOriginal
    if($r.rootNodeOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::Object -or $map.Count -ne $keys.Count){throw 'Complete fixed report object required.'}
    foreach($name in $map.Keys){if($name -cnotin $keys){throw 'Unknown fixed report property.'}}
    foreach($name in @('members','controlHeaders','differences','referenceDifferences','archiveDifferences','armorHeaders','keys','packets')){
      if($map.ContainsKey($name) -and $map[$name].childOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::Array){throw 'Fixed report array differs.'}
    }
    foreach($name in @('compressedBytes','tarBytes','payloadBytes','memberCount','originalEntries','releaseEntries','matched','referenceEntries','referenceMatched')){
      if($map.ContainsKey($name)){
        $value=$map[$name].childOriginal
        if($value.kindOriginal -ne [System.Text.Json.JsonValueKind]::Number -or
            $value.textOriginal -cnotmatch '^(0|[1-9][0-9]*)$'){throw 'Canonical nonnegative report count required.'}
        $null=$value.elementOriginal.GetInt64()
      }
    }
    if($OutputOriginal.role -ceq 'decompression.json'){
      $compression=$map['compression'].childOriginal;$digest=$map['sha256'].childOriginal
      if($compression.kindOriginal -ne [System.Text.Json.JsonValueKind]::String -or
          $compression.textOriginal -cne $c.pythonPlanParseOriginal.fieldsOriginal['compression'].GetString() -or
          $digest.kindOriginal -ne [System.Text.Json.JsonValueKind]::String -or
          $digest.textOriginal -cne $c.subjectOriginals['archive'].digest -or
          $map['compressedBytes'].childOriginal.elementOriginal.GetInt64() -ne $c.subjectOriginals['archive'].used){throw 'Actual decompression report differs from SAME held archive/plan.'}
    }
    if($null -ne $expectedState){
      $state=$map['state'].childOriginal;$r.stateOriginal=$state
      if($state.kindOriginal -ne [System.Text.Json.JsonValueKind]::String -or $state.textOriginal -cne $expectedState){throw 'Report admission nonclaim state differs.'}
    }
    foreach($name in @('rawReferenceBlobsVerified','releaseTransformationsVerified','localSignatureVerified','selfSignaturesVerified','subkeyPolicyVerified','revocationVerified')){
      if($map.ContainsKey($name) -and $map[$name].childOriginal.kindOriginal -ne [System.Text.Json.JsonValueKind]::False){throw 'Structural report cannot assert authentication.'}
    }
    if($OutputOriginal.role -ceq 'members.json'){Assert-Cli39MaterializerMembersReport32 $r}
    $r.returned=$true;return $r
  }catch{$r.exceptionOriginal=$_;throw}
}
