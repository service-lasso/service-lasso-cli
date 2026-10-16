# Closed actual materialize output reads. These observations are not tar/source
# authentication and do not confer namespace exclusion or policy authority.
function Read-Cli39TreeExtent32 {
  param($ElementOriginal)
  if($ElementOriginal.ValueKind -ne [System.Text.Json.JsonValueKind]::Number){throw 'Actual integral file extent required.'}
  $raw=$ElementOriginal.GetRawText();$value=0L
  if($raw -cnotmatch '^(0|[1-9][0-9]*)$' -or
      -not [long]::TryParse($raw,[Globalization.NumberStyles]::None,[Globalization.CultureInfo]::InvariantCulture,[ref]$value)){
    throw 'Canonical nonnegative file extent required.'
  }
  return $value
}

function Read-Cli39PythonTreeFiles32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$c=$p.consumerOriginal;$b=$c.pythonTreeOriginal
  if($c.slot -cne 'cli39.full-source-materialization.v2' -or $null -eq $b -or
      -not [object]::ReferenceEquals($b.preparationOriginal,$p) -or
      $b.ContainsKey('fileReadOriginal') -or $c.ContainsKey('disposalOriginal')){
    throw 'Once-only actual materialize tree read required.'
  }
  $run=@{bookOriginal=$b;returned=$false;exceptionOriginal=$null}
  $b.fileReadOriginal=$run
  try{
    foreach($h in $b.holdsOriginal){
      Assert-Cli39PrerequisiteUseful26 $p
      if($h.kindOriginal -cnotin @('30','00')){continue}
      $subject=$null;$extent=0L;$readRequired=$false
      if($null -ne $h.memberOriginal){
        $extent=Read-Cli39TreeExtent32 $h.memberOriginal.propertiesOriginal['bytes'].childOriginal.elementOriginal
        $readRequired=$true
      }elseif($null -ne $h.fixedOutputOriginal){
        $name=$h.fixedOutputOriginal.nameOriginal
        if($name -ceq 'original.tar'){
          $decompression=$null
          foreach($report in $c.pythonCompletionOriginal.reportsOriginal){
            if($report.sourceOriginal.role -ceq 'decompression.json'){$decompression=$report;break}
          }
          if($null -eq $decompression -or -not $decompression.returned){throw 'Actual held decompression report required.'}
          $extent=Read-Cli39TreeExtent32 $decompression.rootNodeOriginal.propertiesOriginal['tarBytes'].childOriginal.elementOriginal
          $readRequired=$true
        }else{
          foreach($output in $c.outputOriginals){if($output.role -ceq $name){$subject=$output;break}}
          if($null -eq $subject -or -not $subject.eof){throw 'SAME retained fixed output stream required.'}
          $extent=$subject.used
        }
      }else{throw 'No arbitrary regular output read source exists.'}
      if($extent -lt 0 -or $extent -ge [int]::MaxValue -or
          $b.readsOriginal.Count -ge $b.maximumFileQueries){throw 'Actual file extent/roster exceeds fixed backing.'}
      $r=@{bookOriginal=$b;holdOriginal=$h;subjectOriginal=$subject;bytesExpected=$extent;
        readRequired=$readRequired;fileQueryOriginal=$null;identityReturned=$false;
        reportedDigestCompared=0;returned=$false;exceptionOriginal=$null}
      $b.readsOriginal.Add($r)
      try{
        if(-not $h.metadataReturned -or ($h.attributesOriginal -band 0x410) -ne 0){throw 'Actual regular nonreparse output hold required.'}
        if($readRequired){
          $s=@{role='tree-held-file';path=$h.pathOriginal;stream=$null;handleOriginal=$null;
            bytes=$null;used=0;eof=$false;hashOriginal=$null;digestOriginal=$null;
            openAttempted=$false;openReturned=$false;hashAttempted=$false;hashReturned=$false;exception=$null}
          $r.subjectOriginal=$s
          [decimal]$charge=([decimal]$extent+1)*[decimal]$b.budgetOriginal.pythonTreeCapturePerByteChargeBytes
          if($b.usedChargeBytes+$charge -gt $b.plannedChargeBytes){throw 'Actual file backing exceeds SAME tree reservation.'}
          $b.usedChargeBytes+=$charge
          $s.bytes=[byte[]]::new([int]($extent+1))
          $worker=New-Cli39ManagedPhase26 $p 'tree-read-hash' $s
          $null=Start-Cli39ManagedPhase26 $worker;$null=Wait-Cli39ManagedPhase26 $worker
          if(-not $s.eof -or $s.used -ne $extent -or -not $s.hashReturned -or $null -eq $s.digestOriginal){
            throw 'Exact EOF/extent and actual digest required, including empty files.'
          }
          if($null -ne $h.memberOriginal){
            $reportedDigest=$h.memberOriginal.propertiesOriginal['sha256'].childOriginal.textOriginal
            if($s.digestOriginal.Length -ne 32 -or $reportedDigest.Length -ne 64){throw 'Actual SHA256 digest geometry differs.'}
            $digits='0123456789abcdef'
            for($i=0;$i -lt 32;$i++){
              Assert-Cli39PrerequisiteUseful26 $p
              if($reportedDigest[$i*2] -cne $digits[$s.digestOriginal[$i] -shr 4] -or
                  $reportedDigest[$i*2+1] -cne $digits[$s.digestOriginal[$i] -band 15]){
                throw 'Actual held regular content digest differs from reported member digest.'
              }
              $r.reportedDigestCompared++
            }
          }
        }
        $query=New-Cli39RootMemberFileQueryWorker32 $p $r
        $null=Invoke-Cli39RootMemberFileQuery32 $query
        $info=Complete-Cli39RootMemberFileInformation32 $query
        if(-not $info.heldExtentEqual -or $info.directoryRawOriginal -ne 0 -or
            ($info.tagAttributesOriginal -band 0x410) -ne 0){throw 'Actual held stream is not the expected regular file.'}
        for($i=0;$i -lt 24;$i++){
          Assert-Cli39PrerequisiteUseful26 $p
          if($info.fileIdBufferOriginal[$i] -ne $h.buffersOriginal[1][$i]){throw 'Actual native hold and managed stream full identity differ.'}
        }
        $r.identityReturned=$true;$r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}
    }
    $run.returned=$true;return $run
  }catch{$run.exceptionOriginal=$_;throw}
}

function Dispose-Cli39PythonTreeFiles32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$b=$p.consumerOriginal.pythonTreeOriginal
  if($b.ContainsKey('managedDisposalOriginal')){throw 'Actual tree resource disposal is once-only.'}
  $d=@{returned=$false;unknownAcquisition=$false;exceptionOriginal=$null}
  $b.managedDisposalOriginal=$d
  foreach($read in $b.readsOriginal){
    if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $read)){
      $d.unknownAcquisition=$true;continue
    }
    if(-not $read.readRequired){continue}
    $s=$read.subjectOriginal
    if(($s.openAttempted -and -not $s.openReturned) -or
        ($s.hashAttempted -and -not $s.hashReturned)){$d.unknownAcquisition=$true}
    foreach($kind in @('stream','hash')){
      $obj=if($kind -ceq 'stream'){$s.stream}else{$s.hashOriginal}
      if($null -eq $obj){continue}
      if($b.managedResourcesOriginal.Count -ge $b.managedResourcesOriginal.Capacity){throw 'Actual tree resource roster exhausted.'}
      $resource=@{sourceOriginal=$s;objectOriginal=$obj;handleOriginal=$s.handleOriginal;
        disposalAttempted=$false;disposalReturned=$false;disposalOriginal=$null;disposalWorkerOriginal=$null}
      $b.managedResourcesOriginal.Add($resource)
    }
  }
  foreach($hash in $b.hashesOriginal){
    if($hash.hashAttempted -and -not $hash.hashReturned){$d.unknownAcquisition=$true}
    if($null -ne $hash.hashOriginal){
      if($b.managedResourcesOriginal.Count -ge $b.managedResourcesOriginal.Capacity){throw 'Actual tree hash resource roster exhausted.'}
      $b.managedResourcesOriginal.Add(@{sourceOriginal=$hash;objectOriginal=$hash.hashOriginal;handleOriginal=$null;
        disposalAttempted=$false;disposalReturned=$false;disposalOriginal=$null;disposalWorkerOriginal=$null})
    }
  }
  # All actual resource originals are enrolled before the first Dispose.
  foreach($resource in $b.managedResourcesOriginal){
    try{
      $worker=New-Cli39ManagedPhase26 $p 'tree-resource-dispose' $resource
      $resource.disposalWorkerOriginal=$worker
      $null=Start-Cli39ManagedPhase26 $worker;$null=Wait-Cli39ManagedPhase26 $worker
    }catch{$resource.disposalOriginal=$_}
  }
  $d.returned=$true;return $d
}

function Test-Cli39PythonTreeFilesClosure32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$b=$p.consumerOriginal.pythonTreeOriginal
  if($null -eq $b -or $null -eq $b.readsOriginal){return $true}
  if($b.readsOriginal.Count -eq 0){return $true}
  if(-not $b.ContainsKey('managedDisposalOriginal') -or
      -not $b.managedDisposalOriginal.returned -or $b.managedDisposalOriginal.unknownAcquisition){return $false}
  foreach($read in $b.readsOriginal){if(-not (Test-Cli39RootMemberFileQueryClosure32 $p $read)){return $false}}
  foreach($w in $b.managedPhasesOriginal){
    if(-not $w.startAttempted){if($null -ne $w.thread -and $w.thread.IsAlive){return $false};continue}
    if($null -eq $w.thread -or -not $w.terminalJoined -or $w.thread.IsAlive -or
        $w.pendingActive -or $w.multiplicityUnknown -or $w.joinException.present){return $false}
  }
  foreach($r in $b.managedResourcesOriginal){
    if(-not $r.disposalReturned -or $null -ne $r.disposalOriginal){return $false}
    if($r.objectOriginal -is [IO.FileStream] -and
        ($null -eq $r.handleOriginal -or -not $r.handleOriginal.IsClosed -or $r.objectOriginal.CanRead)){return $false}
  }
  return $true
}

function Read-Cli39TarNumber32 {
  param($OriginalPreparation,[byte[]]$Bytes,[int]$Offset,[int]$Length)
  if($Offset -lt 0 -or $Length -le 0 -or [long]$Offset+$Length -gt $Bytes.Length){throw 'Actual bounded tar number required.'}
  [decimal]$value=0
  if(($Bytes[$Offset] -band 128) -ne 0){
    if(($Bytes[$Offset] -band 64) -ne 0){throw 'Negative tar number unsupported by selected reader.'}
    $value=$Bytes[$Offset] -band 127
    for($i=1;$i -lt $Length;$i++){
      Assert-Cli39PrerequisiteUseful26 $OriginalPreparation
      $value=$value*256+$Bytes[$Offset+$i]
      if($value -gt [long]::MaxValue){throw 'Actual tar number exceeds selected integral extent.'}
    }
  }else{
    $end=$Offset+$Length;$start=$Offset
    while($end -gt $start -and $Bytes[$end-1] -in @(0,32)){$end--}
    while($start -lt $end -and $Bytes[$start] -eq 32){$start++}
    for($i=$start;$i -lt $end;$i++){
      Assert-Cli39PrerequisiteUseful26 $OriginalPreparation
      if($Bytes[$i] -lt 48 -or $Bytes[$i] -gt 55){throw 'Non-octal actual tar number.'}
      $value=$value*8+($Bytes[$i]-48)
      if($value -gt [long]::MaxValue){throw 'Actual tar number exceeds selected integral extent.'}
    }
  }
  return [long]$value
}

function Observe-Cli39PythonTarMembers32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$b=$p.consumerOriginal.pythonTreeOriginal
  if($b.ContainsKey('tarAssociationOriginal') -or $null -eq $b.fileReadOriginal -or
      -not $b.fileReadOriginal.returned){throw 'Actual once-only completed file observations required.'}
  $o=@{bookOriginal=$b;tarReadOriginal=$null;recordsOriginal=$null;returned=$false;
    streamOriginal=$null;
    sourceAuthenticated=$false;extendedHeadersComplete=$false;digestsComplete=$false;exceptionOriginal=$null}
  $b.tarAssociationOriginal=$o
  try{
    foreach($read in $b.readsOriginal){
      if($null -ne $read.holdOriginal.fixedOutputOriginal -and
          $read.holdOriginal.fixedOutputOriginal.nameOriginal -ceq 'original.tar'){$o.tarReadOriginal=$read;break}
    }
    if($null -eq $o.tarReadOriginal -or -not $o.tarReadOriginal.returned){throw 'Actual held original.tar required.'}
    $tar=$o.tarReadOriginal.subjectOriginal
    $o.recordsOriginal=[Collections.Generic.List[object]]::new($b.descriptorConstructionOriginal.membersOriginal.Count)
    foreach($row in $b.descriptorConstructionOriginal.membersOriginal){
      Assert-Cli39PrerequisiteUseful26 $p
      $r=@{rowOriginal=$row;headerOffset=0L;bodyOffset=0L;bytesOriginal=0L;
        checksumOriginal=0L;checksumComputed=0L;kindByteOriginal=0;
        outputReadOriginal=$null;payloadCompared=0L;paddingCompared=0L;returned=$false;exceptionOriginal=$null}
      $o.recordsOriginal.Add($r)
      try{
        $r.headerOffset=Read-Cli39TreeExtent32 $row.propertiesOriginal['headerOffset'].childOriginal.elementOriginal
        $r.bodyOffset=Read-Cli39TreeExtent32 $row.propertiesOriginal['bodyOffset'].childOriginal.elementOriginal
        $r.bytesOriginal=Read-Cli39TreeExtent32 $row.propertiesOriginal['bytes'].childOriginal.elementOriginal
        if($r.headerOffset%512 -ne 0 -or $r.bodyOffset -ne $r.headerOffset+512 -or
            [decimal]$r.bodyOffset+$r.bytesOriginal -gt $tar.used){throw 'Actual member header/body range differs.'}
        $offset=[int]$r.headerOffset
        $r.checksumOriginal=Read-Cli39TarNumber32 $p $tar.bytes ($offset+148) 8
        for($i=0;$i -lt 512;$i++){
          Assert-Cli39PrerequisiteUseful26 $p
          if($i -ge 148 -and $i -lt 156){$r.checksumComputed+=32}
          else{$r.checksumComputed+=$tar.bytes[$offset+$i]}
        }
        if($r.checksumOriginal -ne $r.checksumComputed -or
            (Read-Cli39TarNumber32 $p $tar.bytes ($offset+124) 12) -ne $r.bytesOriginal){throw 'Actual member checksum/size differs.'}
        $r.kindByteOriginal=$tar.bytes[$offset+156]
        if($r.kindByteOriginal.ToString('x2') -cne $row.propertiesOriginal['kind'].childOriginal.textOriginal){throw 'Actual member kind differs.'}
        if($r.kindByteOriginal -in @(0,48)){
          foreach($read in $b.readsOriginal){if([object]::ReferenceEquals($read.holdOriginal.memberOriginal,$row)){$r.outputReadOriginal=$read;break}}
          if($null -eq $r.outputReadOriginal -or -not $r.outputReadOriginal.returned){throw 'Actual corresponding regular output read required.'}
          for($i=0L;$i -lt $r.bytesOriginal;$i++){
            Assert-Cli39PrerequisiteUseful26 $p
            if($tar.bytes[[int]($r.bodyOffset+$i)] -ne $r.outputReadOriginal.subjectOriginal.bytes[[int]$i]){throw 'Actual tar payload and held output bytes differ.'}
            $r.payloadCompared++
          }
        }
        $padding=(512-($r.bytesOriginal%512))%512
        if([decimal]$r.bodyOffset+$r.bytesOriginal+$padding -gt $tar.used){throw 'Actual member padding range exceeds tar.'}
        for($i=0L;$i -lt $padding;$i++){
          Assert-Cli39PrerequisiteUseful26 $p
          if($tar.bytes[[int]($r.bodyOffset+$r.bytesOriginal+$i)] -ne 0){throw 'Actual member padding is nonzero.'}
          $r.paddingCompared++
        }
        $r.returned=$true
      }catch{$r.exceptionOriginal=$_;throw}
    }
    $o.streamOriginal=Observe-Cli39PythonTarStream32 $p $o
    $o.digestJoinOriginal=Join-Cli39PythonTarDigests32 $p $o
    # This is a local bounded correspondence observation. Control/PAX/GNU
    # sequencing, names/metadata, raw digests and termination are joined here.
    # These local correspondences do not authenticate archive/tool/source bytes.
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Observe-Cli39PythonTarStream32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AssociationOriginal)
  $p=$OriginalPreparation;$b=$p.consumerOriginal.pythonTreeOriginal;$a=$AssociationOriginal
  if(-not [object]::ReferenceEquals($b.tarAssociationOriginal,$a) -or $null -ne $a.streamOriginal){throw 'SAME once-only actual tar association required.'}
  $o=@{associationOriginal=$a;rowsOriginal=$null;controlsOriginal=$null;visitedOriginal=$null;
    globalPaxOriginal=$null;localPaxOriginal=$null;longNameOriginal=$null;longLinkOriginal=$null;
    longNamePresent=$false;longLinkPresent=$false;utf8Original=$null;
    progressOriginal=0L;terminatorOffsetOriginal=0L;trailingCompared=0L;payloadOriginal=0L;returned=$false;exceptionOriginal=$null}
  $a.streamOriginal=$o
  try{
    $tar=$a.tarReadOriginal.subjectOriginal
    $report=$b.descriptorConstructionOriginal.reportOriginal
    $controls=$report.rootNodeOriginal.propertiesOriginal['controlHeaders'].childOriginal.childrenOriginal
    if([decimal]$controls.Count+$a.recordsOriginal.Count -gt $b.maximumMembers){throw 'Actual total header roster exceeds source-selected plan.'}
    $o.rowsOriginal=[Collections.Generic.Dictionary[long,object]]::new()
    $o.controlsOriginal=[Collections.Generic.Dictionary[long,object]]::new()
    $o.visitedOriginal=[Collections.Generic.List[object]]::new([int]$b.maximumMembers)
    $o.globalPaxOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
    $o.localPaxOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
    $o.utf8Original=[Text.UTF8Encoding]::new($false,$true)
    foreach($row in $a.recordsOriginal){$o.rowsOriginal.Add($row.headerOffset,$row)}
    foreach($control in $controls){
      $offset=Read-Cli39TreeExtent32 $control.propertiesOriginal['headerOffset'].childOriginal.elementOriginal
      $o.controlsOriginal.Add($offset,$control)
    }
    while($true){
      Assert-Cli39PrerequisiteUseful26 $p
      $offset=$o.progressOriginal
      if([decimal]$offset+512 -gt $tar.used){throw 'Actual tar header is truncated.'}
      $zero=$true
      for($i=0;$i -lt 512;$i++){
        Assert-Cli39PrerequisiteUseful26 $p
        if($tar.bytes[[int]($offset+$i)] -ne 0){$zero=$false;break}
      }
      if($zero){
        if($o.localPaxOriginal.Count -ne 0 -or $o.longNamePresent -or $o.longLinkPresent){throw 'Actual tar terminator has pending extended header state.'}
        $o.terminatorOffsetOriginal=$offset
        if([decimal]$offset+1024 -gt $tar.used -or ($tar.used-$offset)%512 -ne 0){throw 'Actual two-block aligned tar terminator required.'}
        for($i=$offset;$i -lt $tar.used;$i++){
          Assert-Cli39PrerequisiteUseful26 $p
          if($tar.bytes[[int]$i] -ne 0){throw 'Actual tar trailing bytes are nonzero.'}
          $o.trailingCompared++
        }
        break
      }
      $entry=@{offsetOriginal=$offset;rowOriginal=$null;controlOriginal=$null;sizeOriginal=0L;
        checksumOriginal=0L;checksumComputed=0L;kindByteOriginal=0;bodyOffsetOriginal=$offset+512;
        paddingCompared=0L;returned=$false;exceptionOriginal=$null}
      if($o.visitedOriginal.Count -ge $b.maximumMembers){throw 'Actual tar stream header roster exhausted.'}
      $o.visitedOriginal.Add($entry)
      try{
        $entry.sizeOriginal=Read-Cli39TarNumber32 $p $tar.bytes ([int]($offset+124)) 12
        $entry.checksumOriginal=Read-Cli39TarNumber32 $p $tar.bytes ([int]($offset+148)) 8
        $entry.kindByteOriginal=$tar.bytes[[int]($offset+156)]
        for($i=0;$i -lt 512;$i++){
          Assert-Cli39PrerequisiteUseful26 $p
          if($i -ge 148 -and $i -lt 156){$entry.checksumComputed+=32}
          else{$entry.checksumComputed+=$tar.bytes[[int]($offset+$i)]}
        }
        if($entry.checksumOriginal -ne $entry.checksumComputed){throw 'Actual stream header checksum differs.'}
        $padding=(512-($entry.sizeOriginal%512))%512
        [decimal]$next=[decimal]$offset+512+$entry.sizeOriginal+$padding
        if($next -gt $tar.used){throw 'Actual stream payload/padding extent exceeds held tar.'}
        if($entry.kindByteOriginal -in @(120,103,76,75)){
          if(-not $o.controlsOriginal.ContainsKey($offset) -or $o.rowsOriginal.ContainsKey($offset)){throw 'Actual control header lacks unique report original.'}
          $entry.controlOriginal=$o.controlsOriginal[$offset]
          $reported=$entry.controlOriginal.propertiesOriginal
          if((Read-Cli39TreeExtent32 $reported['bytes'].childOriginal.elementOriginal) -ne $entry.sizeOriginal -or
              $reported['kind'].childOriginal.textOriginal -cne ([char]$entry.kindByteOriginal).ToString()){
            throw 'Actual control kind/extent differs from report.'
          }
        }else{
          if(-not $o.rowsOriginal.ContainsKey($offset) -or $o.controlsOriginal.ContainsKey($offset)){throw 'Actual member header lacks unique report original.'}
          $entry.rowOriginal=$o.rowsOriginal[$offset]
          $o.payloadOriginal+=$entry.sizeOriginal
          if($entry.rowOriginal.bytesOriginal -ne $entry.sizeOriginal -or
              $entry.rowOriginal.kindByteOriginal -ne $entry.kindByteOriginal){throw 'Actual ordered member record differs.'}
        }
        for($i=0L;$i -lt $padding;$i++){
          Assert-Cli39PrerequisiteUseful26 $p
          if($tar.bytes[[int]($offset+512+$entry.sizeOriginal+$i)] -ne 0){throw 'Actual stream padding is nonzero.'}
          $entry.paddingCompared++
        }
        $null=Join-Cli39PythonTarNames32 $p $o $entry
        $entry.returned=$true;$o.progressOriginal=[long]$next
      }catch{$entry.exceptionOriginal=$_;throw}
    }
    if($o.visitedOriginal.Count -ne $o.rowsOriginal.Count+$o.controlsOriginal.Count){throw 'Actual report has unvisited tar headers.'}
    if($o.payloadOriginal -ne (Read-Cli39TreeExtent32 $report.rootNodeOriginal.propertiesOriginal['payloadBytes'].childOriginal.elementOriginal)){
      throw 'Actual ordered payload extent differs from member report.'
    }
    $complete=$null
    foreach($candidate in $p.consumerOriginal.pythonCompletionOriginal.reportsOriginal){
      if($candidate.sourceOriginal.role -ceq 'complete.json'){$complete=$candidate;break}
    }
    if($null -eq $complete -or -not $complete.returned -or
        (Read-Cli39TreeExtent32 $complete.rootNodeOriginal.propertiesOriginal['memberCount'].childOriginal.elementOriginal) -ne $o.rowsOriginal.Count -or
        (Read-Cli39TreeExtent32 $complete.rootNodeOriginal.propertiesOriginal['payloadBytes'].childOriginal.elementOriginal) -ne $o.payloadOriginal){
      throw 'Actual member/payload counts differ from completion report.'
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}

function Read-Cli39TarText32 {
  param($OriginalPreparation,$StreamOriginal,[byte[]]$Bytes,[int]$Offset,[int]$Length)
  if($Offset -lt 0 -or $Length -lt 0 -or [long]$Offset+$Length -gt $Bytes.Length){throw 'Actual bounded tar text required.'}
  $end=$Offset+$Length;$stop=$end
  for($i=$Offset;$i -lt $end;$i++){
    Assert-Cli39PrerequisiteUseful26 $OriginalPreparation
    if($Bytes[$i] -eq 0){$stop=$i;break}
  }
  for($i=$stop;$i -lt $end;$i++){
    Assert-Cli39PrerequisiteUseful26 $OriginalPreparation
    if($Bytes[$i] -ne 0){throw 'Actual tar text has nonzero suffix after NUL.'}
  }
  return $StreamOriginal.utf8Original.GetString($Bytes,$Offset,$stop-$Offset)
}

function Read-Cli39TarPax32 {
  param($OriginalPreparation,$StreamOriginal,$EntryOriginal,[byte[]]$Bytes)
  $p=$OriginalPreparation;$e=$EntryOriginal
  $map=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
  $e.paxOriginal=$map;$e.paxProgressOriginal=0L
  $e.paxRecordsOriginal=[Collections.Generic.List[object]]::new()
  [long]$cursor=$e.bodyOffsetOriginal;[long]$end=$cursor+$e.sizeOriginal
  while($cursor -lt $end){
    Assert-Cli39PrerequisiteUseful26 $p
    [decimal]$count=0;$digits=0;$start=$cursor
    while($cursor -lt $end -and $Bytes[[int]$cursor] -ne 32){
      Assert-Cli39PrerequisiteUseful26 $p
      $ch=$Bytes[[int]$cursor]
      if($ch -lt 48 -or $ch -gt 57){throw 'Actual PAX record extent is not decimal.'}
      $count=$count*10+($ch-48);$digits++;$cursor++
      if($count -gt $e.sizeOriginal){throw 'Actual PAX record extent exceeds its body.'}
    }
    if($digits -eq 0 -or $cursor -ge $end -or $count -le $digits+2 -or
        [decimal]$start+$count -gt $end){throw 'Actual PAX record extent/separator differs.'}
    $recordEnd=[long]([decimal]$start+$count)
    $record=@{offsetOriginal=$start;extentOriginal=$count;keyOriginal=$null;valueOriginal=$null;returned=$false}
    $e.paxRecordsOriginal.Add($record)
    if($Bytes[[int]($recordEnd-1)] -ne 10){throw 'Actual PAX record lacks newline.'}
    $cursor++;$keyStart=$cursor
    while($cursor -lt $recordEnd-1 -and $Bytes[[int]$cursor] -ne 61){
      Assert-Cli39PrerequisiteUseful26 $p
      if($Bytes[[int]$cursor] -gt 127){throw 'Actual PAX key is not ASCII.'}
      $cursor++
    }
    if($cursor -ge $recordEnd-1){throw 'Actual PAX record lacks equals separator.'}
    $key=$StreamOriginal.utf8Original.GetString($Bytes,[int]$keyStart,[int]($cursor-$keyStart))
    $record.keyOriginal=$key
    $cursor++
    $value=$StreamOriginal.utf8Original.GetString($Bytes,[int]$cursor,[int]($recordEnd-1-$cursor))
    $record.valueOriginal=$value
    if($key -cnotin @('path','linkpath','size','mtime','atime','ctime','uid','gid','uname','gname') -or
        $map.ContainsKey($key)){throw 'Actual PAX key is unsupported or duplicated.'}
    $map.Add($key,$value);$record.returned=$true;$cursor=$recordEnd;$e.paxProgressOriginal=$cursor-$e.bodyOffsetOriginal
  }
  return ,$map
}

function Join-Cli39PythonTarNames32 {
  param($OriginalPreparation,$StreamOriginal,$EntryOriginal)
  $p=$OriginalPreparation;$o=$StreamOriginal;$e=$EntryOriginal
  $tar=$o.associationOriginal.tarReadOriginal.subjectOriginal
  $join=@{entryOriginal=$e;nameOriginal=$null;prefixOriginal=$null;linkOriginal=$null;
    controlTextOriginal=$null;attributesOriginal=$null;metadataCompared=0;returned=$false;exceptionOriginal=$null}
  $e.nameJoinOriginal=$join
  try{
    $offset=[int]$e.offsetOriginal
    $join.nameOriginal=Read-Cli39TarText32 $p $o $tar.bytes $offset 100
    $join.linkOriginal=Read-Cli39TarText32 $p $o $tar.bytes ($offset+157) 100
    $magic=$o.utf8Original.GetString($tar.bytes,$offset+257,6)
    if($magic -ceq "ustar`0"){
      $join.prefixOriginal=Read-Cli39TarText32 $p $o $tar.bytes ($offset+345) 155
      if($join.prefixOriginal.Length -gt 0){$join.nameOriginal=$join.prefixOriginal+'/'+$join.nameOriginal}
    }elseif($magic -cne 'ustar ' -and $magic -cne "`0`0`0`0`0`0"){throw 'Actual tar magic is unsupported.'}
    switch($e.kindByteOriginal){
      120 {
        if($o.localPaxOriginal.Count -ne 0){throw 'Actual repeated local PAX state.'}
        $o.localPaxOriginal=Read-Cli39TarPax32 $p $o $e $tar.bytes
      }
      103 {
        $map=Read-Cli39TarPax32 $p $o $e $tar.bytes
        foreach($pair in $map.GetEnumerator()){
          Assert-Cli39PrerequisiteUseful26 $p
          if($pair.Key -cin @('path','linkpath','size') -or $o.globalPaxOriginal.ContainsKey($pair.Key)){throw 'Actual ambiguous global PAX state.'}
          $o.globalPaxOriginal.Add($pair.Key,$pair.Value)
        }
      }
      76 {
        if($o.longNamePresent){throw 'Actual repeated GNU long name.'}
        $o.longNameOriginal=Read-Cli39TarText32 $p $o $tar.bytes ([int]$e.bodyOffsetOriginal) ([int]$e.sizeOriginal)
        $join.controlTextOriginal=$o.longNameOriginal
        $o.longNamePresent=$true
      }
      75 {
        if($o.longLinkPresent){throw 'Actual repeated GNU long link.'}
        $o.longLinkOriginal=Read-Cli39TarText32 $p $o $tar.bytes ([int]$e.bodyOffsetOriginal) ([int]$e.sizeOriginal)
        $join.controlTextOriginal=$o.longLinkOriginal
        $o.longLinkPresent=$true
      }
      default {
        $attrs=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
        $join.attributesOriginal=$attrs
        foreach($pair in $o.globalPaxOriginal.GetEnumerator()){$attrs.Add($pair.Key,$pair.Value)}
        foreach($pair in $o.localPaxOriginal.GetEnumerator()){$attrs[$pair.Key]=$pair.Value}
        if(($o.longNamePresent -and $attrs.ContainsKey('path')) -or
            ($o.longLinkPresent -and $attrs.ContainsKey('linkpath'))){throw 'Actual ambiguous extended name/link.'}
        if($attrs.ContainsKey('path')){$join.nameOriginal=$attrs['path']}
        elseif($o.longNamePresent){$join.nameOriginal=$o.longNameOriginal}
        if($attrs.ContainsKey('linkpath')){$join.linkOriginal=$attrs['linkpath']}
        elseif($o.longLinkPresent){$join.linkOriginal=$o.longLinkOriginal}
        if($attrs.ContainsKey('size')){
          $sizeText=$attrs['size'];[decimal]$paxSize=0
          if($sizeText.Length -eq 0){throw 'Actual PAX size is empty.'}
          for($i=0;$i -lt $sizeText.Length;$i++){
            Assert-Cli39PrerequisiteUseful26 $p
            $digit=[Globalization.CharUnicodeInfo]::GetDecimalDigitValue($sizeText,$i)
            if($digit -lt 0){throw 'Actual PAX size is not a decimal digit string.'}
            $paxSize=$paxSize*10+$digit
            if($paxSize -gt $e.sizeOriginal){throw 'Actual PAX size disagrees with header.'}
            if([char]::IsHighSurrogate($sizeText[$i])){$i++}
          }
          if($paxSize -ne $e.sizeOriginal){throw 'Actual PAX size disagrees with header.'}
        }
        if($e.kindByteOriginal -eq 53 -and $join.nameOriginal.EndsWith('/')){$join.nameOriginal=$join.nameOriginal.Substring(0,$join.nameOriginal.Length-1)}
        $row=$e.rowOriginal.rowOriginal.propertiesOriginal
        if($join.nameOriginal -cne $row['path'].childOriginal.textOriginal -or
            $join.linkOriginal -cne $row['link'].childOriginal.textOriginal){throw 'Actual effective member path/link differs.'}
        $reported=$row['pax'].childOriginal.propertiesOriginal
        if($reported.Count -ne $attrs.Count){throw 'Actual effective PAX field count differs.'}
        foreach($pair in $attrs.GetEnumerator()){
          Assert-Cli39PrerequisiteUseful26 $p
          if(-not $reported.ContainsKey($pair.Key) -or $reported[$pair.Key].childOriginal.textOriginal -cne $pair.Value){throw 'Actual effective PAX value differs.'}
        }
        foreach($field in @(@('mode',100,8),@('uid',108,8),@('gid',116,8),@('mtime',136,12))){
          $number=Read-Cli39TarNumber32 $p $tar.bytes ($offset+$field[1]) $field[2]
          if($number -ne (Read-Cli39TreeExtent32 $row[$field[0]].childOriginal.elementOriginal)){throw 'Actual raw tar numeric metadata differs.'}
          $join.metadataCompared++
        }
        $o.localPaxOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::Ordinal)
        $o.longNamePresent=$false;$o.longLinkPresent=$false;$o.longNameOriginal=$null;$o.longLinkOriginal=$null
      }
    }
    $join.returned=$true;return $join
  }catch{$join.exceptionOriginal=$_;throw}
}

function New-Cli39PythonTreeHash32 {
  param($OriginalPreparation,[string]$Role,$SourceOriginal)
  $p=$OriginalPreparation;$b=$p.consumerOriginal.pythonTreeOriginal;$a=$b.tarAssociationOriginal
  if($b.hashesOriginal.Count -ge $b.maximumHashes -or -not $a.streamOriginal.returned -or
      $Role -cnotin @('tar-header','tar-control','git-regular','git-symlink')){throw 'Actual fixed complete tar hash source required.'}
  if($Role -cin @('tar-header','tar-control')){
    if(-not $a.streamOriginal.visitedOriginal.Contains($SourceOriginal) -or -not $SourceOriginal.returned){throw 'Actual visited header/control required.'}
    if($Role -ceq 'tar-header'){
      if($null -eq $SourceOriginal.rowOriginal){throw 'Header hash belongs to actual reported member.'}
      $offset=$SourceOriginal.offsetOriginal;$count=512L
      $expected=$SourceOriginal.rowOriginal.rowOriginal.propertiesOriginal['headerSHA256'].childOriginal.textOriginal
    }else{
      if($null -eq $SourceOriginal.controlOriginal){throw 'Control hash belongs to actual reported control.'}
      $offset=$SourceOriginal.bodyOffsetOriginal;$count=$SourceOriginal.sizeOriginal
      $expected=$SourceOriginal.controlOriginal.propertiesOriginal['sha256'].childOriginal.textOriginal
    }
    $bytes=$a.tarReadOriginal.subjectOriginal.bytes;$algorithm='SHA256';$prefix=$null
  }else{
    if(-not $b.descriptorConstructionOriginal.membersOriginal.Contains($SourceOriginal)){throw 'Actual reported Git member required.'}
    $row=$SourceOriginal.propertiesOriginal;$kind=$row['kind'].childOriginal.textOriginal
    if($Role -ceq 'git-regular'){
      if($kind -cnotin @('30','00')){throw 'Git regular hash kind differs.'}
      $read=$null
      foreach($candidate in $b.readsOriginal){if([object]::ReferenceEquals($candidate.holdOriginal.memberOriginal,$SourceOriginal)){$read=$candidate;break}}
      if($null -eq $read -or -not $read.returned){throw 'Actual complete held regular Git bytes required.'}
      $bytes=$read.subjectOriginal.bytes;$offset=0L;$count=$read.subjectOriginal.used
    }else{
      if($kind -cne '32'){throw 'Git symbolic hash kind differs.'}
      $link=$row['link'].childOriginal.textOriginal
      $count=$a.streamOriginal.utf8Original.GetByteCount($link);$offset=0L;$bytes=$null
    }
    $expected=$row['gitBlobSHA1'].childOriginal.textOriginal;$algorithm='SHA1'
    $prefix='blob '+$count.ToString([Globalization.CultureInfo]::InvariantCulture)+[char]0
  }
  foreach($prior in $b.hashesOriginal){if($prior.roleOriginal -ceq $Role -and [object]::ReferenceEquals($prior.sourceOriginal,$SourceOriginal)){throw 'Actual hash cannot retry.'}}
  $r=@{bookOriginal=$b;roleOriginal=$Role;sourceOriginal=$SourceOriginal;algorithmOriginal=$algorithm;
    expectedOriginal=$expected;prefixOriginal=$prefix;bytes=$null;hashOriginal=$null;digestOriginal=$null;
    hashAttempted=$false;hashReturned=$false;effectReturned=$false;exception=$null;comparedOriginal=0;returned=$false}
  $b.hashesOriginal.Add($r)
  [decimal]$total=$count;if($null -ne $prefix){$total+=$prefix.Length}
  if($total -gt [int]::MaxValue){throw 'Actual fixed hash backing exceeds selected buffer extent.'}
  [decimal]$charge=$total*[decimal]$b.budgetOriginal.pythonTreeCapturePerByteChargeBytes
  if($b.usedChargeBytes+$charge -gt $b.plannedChargeBytes){throw 'Actual retained hash backing exceeds SAME reservation.'}
  $b.usedChargeBytes+=$charge;$r.bytes=[byte[]]::new([int]$total)
  $prefixCount=0
  if($null -ne $prefix){
    $prefixCount=$prefix.Length
    for($i=0;$i -lt $prefixCount;$i++){$r.bytes[$i]=[byte][char]$prefix[$i]}
  }
  if($Role -ceq 'git-symlink'){
    $written=$a.streamOriginal.utf8Original.GetBytes($link,0,$link.Length,$r.bytes,$prefixCount)
    if($written -ne $count){throw 'Actual Git symbolic UTF8 extent differs.'}
  }else{
    for($i=0L;$i -lt $count;$i++){
      Assert-Cli39PrerequisiteUseful26 $p
      $r.bytes[[int]($prefixCount+$i)]=$bytes[[int]($offset+$i)]
    }
  }
  $worker=New-Cli39ManagedPhase26 $p 'tree-buffer-hash' $r
  $null=Start-Cli39ManagedPhase26 $worker;$null=Wait-Cli39ManagedPhase26 $worker
  $width=if($algorithm -ceq 'SHA256'){32}else{20};$digits='0123456789abcdef'
  if($null -eq $r.digestOriginal -or $r.digestOriginal.Length -ne $width -or $expected.Length -ne $width*2){throw 'Actual fixed digest geometry differs.'}
  for($i=0;$i -lt $width;$i++){
    Assert-Cli39PrerequisiteUseful26 $p
    if($expected[$i*2] -cne $digits[$r.digestOriginal[$i] -shr 4] -or
        $expected[$i*2+1] -cne $digits[$r.digestOriginal[$i] -band 15]){throw 'Actual digest differs from SAME report.'}
    $r.comparedOriginal++
  }
  $r.returned=$true;return $r
}

function Join-Cli39PythonTarDigests32 {
  param($OriginalPreparation,$AssociationOriginal)
  $p=$OriginalPreparation;$a=$AssociationOriginal;$b=$p.consumerOriginal.pythonTreeOriginal
  if(-not [object]::ReferenceEquals($b.tarAssociationOriginal,$a) -or $a.ContainsKey('digestJoinOriginal')){throw 'Actual once-only tar digest join required.'}
  $o=@{associationOriginal=$a;returned=$false;exceptionOriginal=$null};$a.digestJoinOriginal=$o
  try{
    foreach($entry in $a.streamOriginal.visitedOriginal){
      if($null -ne $entry.rowOriginal){$null=New-Cli39PythonTreeHash32 $p 'tar-header' $entry}
      else{$null=New-Cli39PythonTreeHash32 $p 'tar-control' $entry}
    }
    foreach($row in $b.descriptorConstructionOriginal.membersOriginal){
      $kind=$row.propertiesOriginal['kind'].childOriginal.textOriginal
      if($kind -cin @('30','00')){$null=New-Cli39PythonTreeHash32 $p 'git-regular' $row}
      elseif($kind -ceq '32'){$null=New-Cli39PythonTreeHash32 $p 'git-symlink' $row}
    }
    $o.hardlinkJoinsOriginal=[Collections.Generic.List[object]]::new()
    foreach($row in $b.descriptorConstructionOriginal.membersOriginal){
      Assert-Cli39PrerequisiteUseful26 $p
      $fields=$row.propertiesOriginal;$kind=$fields['kind'].childOriginal.textOriginal
      if($kind -cnotin @('30','00') -and $fields['sha256'].childOriginal.textOriginal -cne
          'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855'){
        throw 'Actual zero-payload member report digest differs from SHA256 empty bytes.'
      }
      if($kind -cne '31'){continue}
      $join=@{rowOriginal=$row;targetOriginal=$null;hashOriginal=$null;returned=$false}
      $o.hardlinkJoinsOriginal.Add($join)
      $target=$fields['target'].childOriginal.textOriginal
      if(-not $b.descriptorConstructionOriginal.memberMapOriginal.ContainsKey($target)){throw 'Actual hardlink target missing.'}
      $join.targetOriginal=$b.descriptorConstructionOriginal.memberMapOriginal[$target]
      foreach($hash in $b.hashesOriginal){
        if($hash.roleOriginal -ceq 'git-regular' -and [object]::ReferenceEquals($hash.sourceOriginal,$join.targetOriginal)){
          $join.hashOriginal=$hash;break
        }
      }
      if($null -eq $join.hashOriginal -or -not $join.hashOriginal.returned -or
          $fields['gitBlobSHA1'].childOriginal.textOriginal -cne $join.hashOriginal.expectedOriginal){
        throw 'Actual hardlink report does not borrow actual verified target Git digest.'
      }
      $join.returned=$true
    }
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}
}
