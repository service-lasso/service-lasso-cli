# Fixed materialize output roster. Report correspondence is not source/native
# authentication, current file identity or writer exclusion.
function Initialize-Cli39PythonTreeDescriptors32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$b=$c.pythonTreeOriginal
  if($c.slot -cne 'cli39.full-source-materialization.v2' -or $null -eq $b -or
      -not [object]::ReferenceEquals($b.preparationOriginal,$p) -or -not $b.returned -or
      $b.ContainsKey('descriptorConstructionOriginal') -or $null -ne $b.disposalOriginal -or
      $c.ContainsKey('disposalOriginal')){throw 'Actual fixed once-only tree descriptor factory required.'}
  $o=@{bookOriginal=$b;reportOriginal=$null;membersOriginal=$null;memberMapOriginal=$null;
    directoryMapOriginal=$null;fixedMetadataPrecharged=$false;returned=$false;
    exceptionOriginal=$null;pendingOriginal=$null}
  $b.descriptorConstructionOriginal=$o
  try{
    Assert-Cli39PrerequisiteUseful26 $p
    foreach($r in $c.pythonCompletionOriginal.reportsOriginal){
      if($r.sourceOriginal.role -ceq 'members.json'){
        if($null -ne $o.reportOriginal){throw 'Duplicate actual members report original.'}
        $o.reportOriginal=$r
      }
    }
    if(-not (Test-Cli39PythonReportOriginal32 $p $o.reportOriginal) -or -not $o.reportOriginal.returned){
      throw 'SAME complete held members report required.'
    }
    $o.membersOriginal=$o.reportOriginal.rootNodeOriginal.propertiesOriginal['members'].childOriginal.childrenOriginal
    if($o.membersOriginal.Count -gt $b.maximumDirectories-2){throw 'Actual member roster exceeds precharged geometry.'}
    $v=$b.budgetOriginal
    [decimal]$fixedCharge=$b.maximumDirectories*[decimal]$v.pythonTreeCapturePerDirectoryChargeBytes
    if($b.usedChargeBytes+$fixedCharge -gt $b.plannedChargeBytes){throw 'Tree metadata exceeds planned original charge.'}
    $b.usedChargeBytes+=$fixedCharge;$o.fixedMetadataPrecharged=$true
    $b.descriptorsOriginal=[Collections.Generic.List[object]]::new([int]$b.maximumDirectories)
    $o.memberMapOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $o.directoryMapOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $rootName=Read-Cli39PlanString32 $b.planOriginal.fieldsOriginal['rootName']
    # Every retained path belongs to an actual report row; no arbitrary path
    # parameter is accepted by the directory constructor.
    foreach($row in $o.membersOriginal){
      Assert-Cli39PrerequisiteUseful26 $p
      $fields=$row.propertiesOriginal;$path=$fields['path'].childOriginal.textOriginal
      $o.pendingOriginal=$row
      if([string]::IsNullOrEmpty($path) -or $path.IndexOfAny([char[]]@('\',':',[char]0)) -ge 0 -or
          $path.StartsWith('/') -or $o.memberMapOriginal.ContainsKey($path)){
        throw 'Invalid or duplicate actual archive member path.'
      }
      $parts=$path.Split('/')
      if([Text.UTF8Encoding]::new($false,$true).GetByteCount($path) -gt
          (Read-Cli39PlanInteger32 $b.planOriginal.fieldsOriginal['maxPathBytes'])){
        throw 'Actual archive path exceeds SAME held plan byte extent.'
      }
      if($parts[0] -cne $rootName){throw 'Actual archive path lies outside fixed plan root.'}
      foreach($part in $parts){
        if([string]::IsNullOrEmpty($part) -or $part -cin @('.','..') -or
            $part.EndsWith(' ') -or $part.EndsWith('.') -or
            $part.IndexOfAny([char[]]@('<','>','"','|','?','*')) -ge 0){throw 'Unsafe actual archive path component.'}
        foreach($unit in $part.ToCharArray()){if([int]$unit -lt 32){throw 'Control character in actual archive component.'}}
        $stem=$part.Split('.')[0]
        foreach($device in @('con','prn','aux','nul','clock$','com1','com2','com3','com4','com5','com6','com7','com8','com9',
            'lpt1','lpt2','lpt3','lpt4','lpt5','lpt6','lpt7','lpt8','lpt9')){
          if([StringComparer]::OrdinalIgnoreCase.Equals($stem,$device)){throw 'Reserved Windows device component in actual archive path.'}
        }
      }
      $o.memberMapOriginal.Add($path,$row)
    }
    if(-not $o.memberMapOriginal.ContainsKey($rootName) -or
        $o.memberMapOriginal[$rootName].propertiesOriginal['kind'].childOriginal.textOriginal -cne '35'){
      throw 'Actual archive root directory row required.'
    }
    # The two fixed directories precede archive directories. Their descriptor
    # strings and retained metadata are covered before path composition.
    $sources=[Collections.Generic.List[object]]::new([int]$b.maximumDirectories)
    $o.sourcesOriginal=$sources
    $sources.Add(@{kind='output-root';path=$null;row=$null})
    $sources.Add(@{kind='tree-wrapper';path=$null;row=$null})
    foreach($row in $o.membersOriginal){
      if($row.propertiesOriginal['kind'].childOriginal.textOriginal -ceq '35'){
        $sources.Add(@{kind='archive-directory';path=$row.propertiesOriginal['path'].childOriginal.textOriginal;row=$row})
      }
    }
    foreach($source in $sources){
      Assert-Cli39PrerequisiteUseful26 $p
      [decimal]$units=$c.paths.outputRoot.Length
      if($source.kind -ceq 'tree-wrapper'){$units+=5}
      elseif($source.kind -ceq 'archive-directory'){$units+=6+$source.path.Length}
      [decimal]$charge=2*($units+1)*[decimal]$v.pythonTreeCapturePerByteChargeBytes
      if($b.usedChargeBytes+$charge -gt $b.plannedChargeBytes){throw 'Descriptor paths exceed SAME planned backing.'}
      $b.usedChargeBytes+=$charge
      $s=@{bookOriginal=$b;sourceOriginal=$source;pathOriginal=$null;expectedNamesOriginal=$null;
        returned=$false;exceptionOriginal=$null}
      $b.descriptorsOriginal.Add($s)
      try{
        $s.pathOriginal=$c.paths.outputRoot
        if($source.kind -ceq 'tree-wrapper'){$s.pathOriginal+='\tree'}
        elseif($source.kind -ceq 'archive-directory'){$s.pathOriginal+='\tree\'+$source.path.Replace('/','\')}
        $s.expectedNamesOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
        if($source.kind -ceq 'output-root'){
          foreach($name in @('plan-original.json','decompression.json','members.json','complete.json','original.tar','tree')){
            $s.expectedNamesOriginal.Add($name,@{kind='fixed-output';nameOriginal=$name})
          }
        }elseif($source.kind -ceq 'tree-wrapper'){
          $s.expectedNamesOriginal.Add($rootName,$o.memberMapOriginal[$rootName])
        }else{$o.directoryMapOriginal.Add($source.path,$s)}
        $s.returned=$true
      }catch{$s.exceptionOriginal=$_;throw}
    }
    foreach($row in $o.membersOriginal){
      Assert-Cli39PrerequisiteUseful26 $p
      $path=$row.propertiesOriginal['path'].childOriginal.textOriginal
      if($path -ceq $rootName){continue}
      $slash=$path.LastIndexOf('/');$parent=$path.Substring(0,$slash);$name=$path.Substring($slash+1)
      if(-not $o.directoryMapOriginal.ContainsKey($parent)){throw 'Actual member parent directory row missing.'}
      $o.directoryMapOriginal[$parent].expectedNamesOriginal.Add($name,$row)
    }
    [decimal]$calls=0
    foreach($s in $b.descriptorsOriginal){$calls+=[decimal]$s.expectedNamesOriginal.Count+3}
    if($calls -gt $b.maximumCalls){throw 'Actual fixed directory call roster exceeds complete source bound.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Observe-Cli39PythonTreeDirectories32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$b=$c.pythonTreeOriginal
  if($null -eq $b -or $b.ContainsKey('enumerationOriginal')){throw 'Tree enumeration is once-only.'}
  $o=@{bookOriginal=$b;descriptorOriginal=$null;returned=$false;exceptionOriginal=$null;
    state='ENUMERATION_NOT_HELD_FILE_TAR_LINK_OR_SOURCE_AUTHENTICATION'}
  $b.enumerationOriginal=$o
  try{
    $o.descriptorOriginal=Initialize-Cli39PythonTreeDescriptors32 $p
    # Preown every directory and its complete call/close roster before first
    # acquisition. Constructor failure retains the entire partial book.
    foreach($s in $b.descriptorsOriginal){$null=New-Cli39PythonDirectory32 $s}
    $b.holdsOriginal=[Collections.Generic.List[object]]::new([int]$b.maximumHolds)
    foreach($d in $b.directoriesOriginal){$null=New-Cli39PythonOutputHold32 -BookOriginal $b -DirectoryOriginal $d}
    foreach($row in $b.descriptorConstructionOriginal.membersOriginal){
      if($row.propertiesOriginal['kind'].childOriginal.textOriginal -cne '35'){
        $null=New-Cli39PythonOutputHold32 -BookOriginal $b -MemberOriginal $row
      }
    }
    foreach($name in @('plan-original.json','decompression.json','members.json','complete.json','original.tar')){
      $null=New-Cli39PythonOutputHold32 -BookOriginal $b -FixedOutputOriginal $b.descriptorsOriginal[0].expectedNamesOriginal[$name]
    }
    $o.holdObservationOriginal=Observe-Cli39PythonOutputHolds32 $b
    foreach($h in $b.holdsOriginal){if($h.kindOriginal -ceq '32'){$null=Invoke-Cli39PythonReparse32 $h}}
    $o.linkAssociationOriginal=Join-Cli39PythonOutputLinks32 $b
    foreach($d in $b.directoriesOriginal){
      $d.observedNamesOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
      $d.matchedNamesOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
      foreach($w in $d.workersOriginal){
        Assert-Cli39PrerequisiteUseful26 $p
        $null=Invoke-Cli39PythonDirectoryWorker32 $w
        if($w.endObserved){break}
        $row=Read-Cli39PythonDirectoryRow32 $w
        $name=$row.nameOriginal
        if($d.observedNamesOriginal.ContainsKey($name)){throw 'Duplicate actual enumeration component retained.'}
        $d.observedNamesOriginal.Add($name,$row)
        if($name -cin @('.','..')){continue}
        if(-not $d.expectedNamesOriginal.ContainsKey($name)){throw 'Unexpected actual enumeration component retained.'}
        $d.matchedNamesOriginal.Add($name,$row)
      }
      if(-not $d.endObserved -or $d.matchedNamesOriginal.Count -ne $d.expectedNamesOriginal.Count){
        throw 'Actual complete immediate-child roster and EOF required.'
      }
      $d.enumerationReturned=$true
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Dispose-Cli39PythonTree32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$b=$c.pythonTreeOriginal
  if($null -eq $b -or -not [object]::ReferenceEquals($b.preparationOriginal,$p) -or
      $null -ne $b.disposalOriginal){throw 'Actual tree disposal is once-only.'}
  $o=@{bookOriginal=$b;recordsOriginal=$null;returned=$false;exceptionOriginal=$null}
  $b.disposalOriginal=$o
  try{
    $o.recordsOriginal=[Collections.Generic.List[object]]::new($b.directoriesOriginal.Count)
    foreach($d in $b.directoriesOriginal){
      $r=@{directoryOriginal=$d;disposalOriginal=$null;exceptionOriginal=$null}
      $o.recordsOriginal.Add($r)
      try{
        if($null -eq $d.disposalOriginal){$r.disposalOriginal=Dispose-Cli39PythonDirectory32 $d}
        else{$r.disposalOriginal=$d.disposalOriginal}
      }catch{$r.exceptionOriginal=$_;$r.disposalOriginal=$d.disposalOriginal}
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Test-Cli39PythonTreeSearchClosure32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal
  if(-not $c.ContainsKey('pythonTreeOriginal')){return $true}
  $b=$c.pythonTreeOriginal
  if($null -eq $b -or -not [object]::ReferenceEquals($b.preparationOriginal,$p) -or
      $null -eq $b.disposalOriginal -or -not $b.disposalOriginal.returned){return $false}
  foreach($d in $b.directoriesOriginal){if(-not (Test-Cli39PythonDirectoryClosure32 $d)){return $false}}
  return $true
}

function Test-Cli39PythonTreeClosure32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  if(-not (Test-Cli39PythonTreeSearchClosure32 $OriginalPreparation)){return $false}
  $c=$OriginalPreparation.consumerOriginal
  if(-not $c.ContainsKey('pythonTreeOriginal')){return $true}
  foreach($h in $c.pythonTreeOriginal.holdsOriginal){if(-not (Test-Cli39PythonOutputHoldClosure32 $h)){return $false}}
  return $true
}

function Join-Cli39PythonOutputLinks32 {
  param([Parameter(Mandatory)]$BookOriginal)
  $b=$BookOriginal;$p=$b.preparationOriginal
  if(-not [object]::ReferenceEquals($p.consumerOriginal.pythonTreeOriginal,$b) -or
      $null -eq $b.holdObservationOriginal -or -not $b.holdObservationOriginal.returned -or
      $b.ContainsKey('linkAssociationOriginal')){throw 'Actual fixed once-only held output link association required.'}
  $o=@{bookOriginal=$b;recordsOriginal=$null;returned=$false;exceptionOriginal=$null;
    state='REPORT_NATIVE_LINK_CORRESPONDENCE_NOT_TAR_OR_SOURCE_AUTHENTICATION'}
  $b.linkAssociationOriginal=$o
  try{
    $o.recordsOriginal=[Collections.Generic.List[object]]::new($b.holdsOriginal.Count)
    foreach($h in $b.holdsOriginal){
      if($h.kindOriginal -cnotin @('31','32')){continue}
      Assert-Cli39PrerequisiteUseful26 $p
      $r=@{holdOriginal=$h;memberOriginal=$h.memberOriginal;targetMemberOriginal=$null;targetHoldOriginal=$null;
        expectedTargetOriginal=$null;observedTargetOriginal=$null;partsOriginal=$null;componentsOriginal=$null;
        comparedBytes=0;returned=$false;exceptionOriginal=$null}
      $o.recordsOriginal.Add($r)
      try{
        $fields=$r.memberOriginal.propertiesOriginal
        $r.expectedTargetOriginal=$fields['target'].childOriginal.textOriginal
        if(-not $b.descriptorConstructionOriginal.memberMapOriginal.ContainsKey($r.expectedTargetOriginal)){
          throw 'Reported link target lacks actual member original.'
        }
        $r.targetMemberOriginal=$b.descriptorConstructionOriginal.memberMapOriginal[$r.expectedTargetOriginal]
        foreach($candidate in $b.holdsOriginal){
          $candidateRow=$candidate.memberOriginal
          if($null -ne $candidate.directoryOriginal){$candidateRow=$candidate.directoryOriginal.descriptorOriginal.sourceOriginal.row}
          if([object]::ReferenceEquals($candidateRow,$r.targetMemberOriginal)){
            if($null -ne $r.targetHoldOriginal){throw 'Duplicate actual target hold association.'}
            $r.targetHoldOriginal=$candidate
          }
        }
        if($null -eq $r.targetHoldOriginal -or -not $r.targetHoldOriginal.metadataReturned){throw 'Actual held target metadata original required.'}
        if($h.kindOriginal -ceq '31'){
          if($r.targetHoldOriginal.kindOriginal -cnotin @('30','00')){throw 'Hardlink target must be an original regular member.'}
          for($i=0;$i -lt 24;$i++){
            Assert-Cli39PrerequisiteUseful26 $p
            if($h.buffersOriginal[1][$i] -ne $r.targetHoldOriginal.buffersOriginal[1][$i]){throw 'Actual hardlink volume/FileId differs from target.'}
            $r.comparedBytes=$i+1
          }
          $r.observedTargetOriginal=$r.expectedTargetOriginal
        }else{
          $decoded=$h.reparseOriginal.decodedOriginal
          if($null -eq $decoded -or -not $decoded.returned -or $decoded.flagsOriginal -ne 1 -or
              $r.targetHoldOriginal.kindOriginal -cnotin @('30','00','35')){
            throw 'Actual relative symbolic-link body and held regular/directory target required.'
          }
          $link=$decoded.substituteOriginal
          if([string]::IsNullOrEmpty($link) -or $link.IndexOfAny([char[]]@(':',[char]0)) -ge 0 -or
              $link.StartsWith('/') -or $link.StartsWith('\')){throw 'Unsupported actual substitute target form retained.'}
          $path=$fields['path'].childOriginal.textOriginal
          $r.partsOriginal=$path.Split('/')
          $r.componentsOriginal=[Collections.Generic.List[string]]::new($r.partsOriginal.Length+$link.Length)
          for($i=0;$i -lt $r.partsOriginal.Length-1;$i++){$r.componentsOriginal.Add($r.partsOriginal[$i])}
          foreach($part in $link.Replace('\','/').Split('/')){
            Assert-Cli39PrerequisiteUseful26 $p
            if($part -ceq '..'){
              if($r.componentsOriginal.Count -le 1){throw 'Actual symbolic substitute escapes archive root.'}
              $r.componentsOriginal.RemoveAt($r.componentsOriginal.Count-1)
            }elseif($part -cnotin @('','.')){$r.componentsOriginal.Add($part)}
          }
          $r.observedTargetOriginal=[string]::Join('/',$r.componentsOriginal)
          if($r.observedTargetOriginal -cne $r.expectedTargetOriginal){throw 'Actual symbolic substitute resolves to a different reported target.'}
          if((($h.attributesOriginal -band 0x10) -ne 0) -ne ($r.targetHoldOriginal.kindOriginal -ceq '35')){
            throw 'Actual symbolic-link file/directory attribute differs from target kind.'
          }
        }
        $r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}
