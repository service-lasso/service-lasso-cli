# SDK26100 fixed timestamp OIDs; comparison never decodes a second OID string.
function Test-Cli39PublisherTimestampOid32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal,
    [Parameter(Mandatory)][ValidateSet('1.2.840.113549.1.9.6','1.3.6.1.4.1.311.3.3.1')][string]$Literal)
  $p=$OriginalPreparation;$a=$AttributeOriginal;$h=$a.oidOriginal
  if(-not (Test-Cli39PublisherRawOid32 $p $a)){throw 'Actual complete captured attribute OID required.'}
  Observe-OwnedPrerequisiteDeadline $p
  if($p.deadlineReported){throw 'Original deadline denies timestamp OID extent correspondence.'}
  if($h.used -ne $Literal.Length+1){return $false}
  for($index=0;$index -lt $Literal.Length;$index++){
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies timestamp OID correspondence.'}
    if($h.rawBytesOriginal[$index] -ne [byte][char]$Literal[$index]){return $false}
  }
  return ($h.rawBytesOriginal[$Literal.Length] -eq 0)
}
function Observe-Cli39PublisherTimestampResults32 {
  param([Parameter(Mandatory)]$OriginalPreparation)
  $p=$OriginalPreparation;$n=$p.consumerOriginal.publisherNativeOriginal;$capture=$n.providerCaptureOriginal
  $time=$n.timeResultOriginal;$anchors=$n.anchorResultOriginal
  $transport=$p.consumerOriginal.rootCaptureOriginal.windowsPolicyFoundationTransportOriginal
  if($n.ContainsKey('timestampResultOriginal') -or -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      -not $time.returned -or -not $time.terminal -or $null -ne $time.exceptionOriginal -or
      -not $anchors.returned -or -not $anchors.terminal -or $null -ne $anchors.exceptionOriginal -or
      -not $transport.returned -or -not [object]::ReferenceEquals($time.transportOriginal,$transport) -or
      -not [object]::ReferenceEquals($anchors.transportOriginal,$transport) -or
      -not [object]::ReferenceEquals($time.captureOriginal,$capture) -or
      -not [object]::ReferenceEquals($anchors.captureOriginal,$capture)){
    throw 'SAME once-only completed actual time/anchor capture required.'}
  $o=@{preparationOriginal=$p;captureOriginal=$capture;timeOriginal=$time;anchorOriginal=$anchors;
    transportOriginal=$transport;chargeOriginal=$n.custodyReservationOriginal;recordsOriginal=$null;
    countOriginal=0D;returned=$false;exceptionOriginal=$null;terminal=$false;qualified=$false;
    state='CAPTURED_TIMESTAMP_RELATIONS_NOT_AUTHENTIC_PROVIDER_EVALUATION'}
  $n.timestampResultOriginal=$o
  try{
    foreach($primary in $capture.signerOriginals){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported -or -not (Test-Cli39PublisherSignerRawGraph32 $p $primary) -or
          $null -ne $primary.primarySignerOriginal){throw 'Actual complete primary timestamp roster required before original D.'}
      $o.countOriginal+=1D+$primary.signerFieldOriginals['unauth-attributes'].attributesOriginal.Count+$primary.counterSignerOriginals.Count
      if($o.countOriginal -gt [int]::MaxValue){throw 'Actual timestamp relation roster unrepresentable.'}
    }
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies timestamp relation charge.'}
    $charge=$o.chargeOriginal
    if($charge.timestampObservationChargeAttempted -or -not [object]::ReferenceEquals($charge.preparationOriginal,$p)){
      throw 'SAME once-only timestamp relation charge required.'}
    $charge.timestampObservationChargeAttempted=$true;$charge.timestampObservationCountOriginal=$o.countOriginal
    $charge.timestampObservationChargeBytes=$o.countOriginal*$charge.perSignerChargeBytes
    if($charge.usedChargeBytes+$charge.timestampObservationChargeBytes -gt $charge.limitBytes){
      throw 'Actual timestamp relation graph exceeds SAME custody subdivision.'}
    $charge.usedChargeBytes+=$charge.timestampObservationChargeBytes
    $o.recordsOriginal=[Collections.Generic.List[object]]::new($capture.signerOriginals.Count)
    [decimal]$actualCount=0
    foreach($primary in $capture.signerOriginals){
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline denies primary timestamp relation.'}
      $attributes=$primary.signerFieldOriginals['unauth-attributes'].attributesOriginal
      $r=@{primaryOriginal=$primary;signerInfoOriginal=$primary.signerInfoOriginal;
        attributesOriginal=$null;counterRecordsOriginal=$null;timeRecordOriginal=$null;
        recognizedAttributeCount=0;timestampCounterCount=0;returned=$false}
      $o.recordsOriginal.Add($r);$actualCount++
      $r.attributesOriginal=[Collections.Generic.List[object]]::new($attributes.Count)
      $r.counterRecordsOriginal=[Collections.Generic.List[object]]::new($primary.counterSignerOriginals.Count)
      foreach($timeRecord in $time.signerRecordsOriginal){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies primary time association.'}
        if([object]::ReferenceEquals($timeRecord.signerOriginal,$primary)){
          if($null -ne $r.timeRecordOriginal){throw 'Duplicate actual primary time association.'};$r.timeRecordOriginal=$timeRecord
        }
      }
      if($null -eq $r.timeRecordOriginal -or -not $r.timeRecordOriginal.returned){throw 'Actual primary time association absent.'}
      foreach($attribute in $attributes){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported -or -not (Test-Cli39ProviderAttributeMember32 $capture $attribute)){
          throw 'Actual primary attribute member required before original D.'}
        $a=@{attributeOriginal=$attribute;valuesOriginal=$attribute.valuesOriginal;oidOriginal=$attribute.oidOriginal;
          rsaCounterSignatureObserved=$false;rfc3161CounterSignatureObserved=$false;bodyPresentObserved=$false;returned=$false}
        $r.attributesOriginal.Add($a);$actualCount++
        $a.rsaCounterSignatureObserved=Test-Cli39PublisherTimestampOid32 $p $attribute '1.2.840.113549.1.9.6'
        $a.rfc3161CounterSignatureObserved=Test-Cli39PublisherTimestampOid32 $p $attribute '1.3.6.1.4.1.311.3.3.1'
        foreach($value in $a.valuesOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported -or -not (Test-Cli39ProviderAttributeValueMember32 $capture $attribute $value) -or
              -not (Test-Cli39PublisherRawRange32 $capture $value.rangeOriginal)){
            throw 'Actual complete timestamp attribute value custody required before original D.'}
          if($value.rangeOriginal.bytesExpected -gt 0){$a.bodyPresentObserved=$true}
        }
        if(($a.rsaCounterSignatureObserved -or $a.rfc3161CounterSignatureObserved) -and $a.bodyPresentObserved){$r.recognizedAttributeCount++}
        $a.returned=$true
      }
      foreach($counter in $primary.counterSignerOriginals){
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported -or -not (Test-Cli39ProviderSignerMember32 $capture $counter) -or
            -not [object]::ReferenceEquals($counter.primarySignerOriginal,$primary)){
          throw 'Actual helper-bound primary/counter relation required before original D.'}
        $c=@{counterOriginal=$counter;primaryOriginal=$primary;headerOriginal=$counter.headerRangeOriginal;
          signerInfoOriginal=$counter.signerInfoOriginal;timeRecordOriginal=$null;anchorRecordOriginal=$null;
          signerTypeOriginal=$counter.headerRangeOriginal.decodedOriginal.fieldsOriginal['dwSignerType'];
          signerErrorOriginal=$counter.headerRangeOriginal.decodedOriginal.fieldsOriginal['dwError'];
          timestampTypeObserved=$false;returned=$false}
        $r.counterRecordsOriginal.Add($c);$actualCount++
        if($c.signerTypeOriginal -isnot [uint32] -or $c.signerErrorOriginal -isnot [uint32]){throw 'Actual copied counter type/error DWORDs required.'}
        foreach($timeRecord in $time.signerRecordsOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies counter time association.'}
          if([object]::ReferenceEquals($timeRecord.signerOriginal,$counter)){
            if($null -ne $c.timeRecordOriginal){throw 'Duplicate actual counter time association.'};$c.timeRecordOriginal=$timeRecord
          }
        }
        foreach($anchor in $anchors.recordsOriginal){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline denies counter chain association.'}
          if([object]::ReferenceEquals($anchor.signerOriginal,$counter)){
            if($null -ne $c.anchorRecordOriginal){throw 'Duplicate actual counter chain association.'};$c.anchorRecordOriginal=$anchor
          }
        }
        if($null -eq $c.timeRecordOriginal -or -not $c.timeRecordOriginal.returned -or
            $null -eq $c.anchorRecordOriginal -or -not $c.anchorRecordOriginal.returned){throw 'Actual counter time/chain associations absent.'}
        $c.timestampTypeObserved=($c.signerTypeOriginal -eq 0x10)
        if($c.timestampTypeObserved){$r.timestampCounterCount++};$c.returned=$true
      }
      $r.returned=$true
    }
    if($actualCount -ne $o.countOriginal){throw 'Actual timestamp roster changed after precharge.'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies timestamp relation publication.'}
    $o.returned=$true;return $o
  }catch{$o.exceptionOriginal=$_;throw}finally{$o.terminal=$true}
}