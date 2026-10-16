# SDK24 CRYPT_ATTRIBUTE and SDK16 CRYPT_ATTR_BLOB from SAME actual copies.
function Test-Cli39ProviderAttributeMember32 {
  param($CaptureOriginal,$AttributeOriginal)
  $capture=$CaptureOriginal;$a=$AttributeOriginal
  if($null -eq $a -or -not [object]::ReferenceEquals($a.captureOriginal,$capture)){return $false}
  $s=$a.signerOriginal;$h=$a.arrayOriginal
  return ((Test-Cli39ProviderSignerMember32 $capture $s) -and
    $a.partOriginal -cin @('auth-attributes','unauth-attributes') -and
    [object]::ReferenceEquals($s.signerFieldOriginals[$a.partOriginal],$h) -and
    $null -ne $h.attributesOriginal -and $h.attributesOriginal.Contains($a))
}

function Test-Cli39ProviderAttributeValueMember32 {
  param($CaptureOriginal,$AttributeOriginal,$ValueOriginal)
  $a=$AttributeOriginal;$v=$ValueOriginal
  return ((Test-Cli39ProviderAttributeMember32 $CaptureOriginal $a) -and $null -ne $v -and
    [object]::ReferenceEquals($v.attributeOriginal,$a) -and $null -ne $a.valuesOriginal -and
    $a.valuesOriginal.Contains($v))
}

function Decode-Cli39ProviderAttributes32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal,
    [Parameter(Mandatory)][ValidateSet('auth-attributes','unauth-attributes')][string]$Part)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$s=$SignerOriginal
  $h=Get-Cli39ProviderHeaderRange32 $p $s $null '' $Part
  if(-not (Test-Cli39ProviderSignerMember32 $capture $s) -or
      (-not $h.copyReturned -and -not $h.absenceObserved) -or
      $h.rawBytesOriginal.Length -ne $h.bytesExpected -or $null -ne $h.attributesConstructorOriginal){throw 'Actual SAME once-only attribute array decode required.'}
  $ctor=@{arrayOriginal=$h;attempted=$true;returned=$false;recordsOriginal=$null;exceptionOriginal=$null}
  $h.attributesConstructorOriginal=$ctor
  try{
    $h.attributesOriginal=[Collections.Generic.List[object]]::new([int]$h.countOriginal)
    $ctor.recordsOriginal=$h.attributesOriginal
    for([int]$index=0;$index -lt $h.countOriginal;$index++){
      $a=@{captureOriginal=$capture;signerOriginal=$s;arrayOriginal=$h;partOriginal=$Part;
        indexOriginal=$index;wordsOriginal=$null;oidPointerOriginal=[IntPtr]::Zero;
        countOriginal=([uint32]0);valuesPointerOriginal=[IntPtr]::Zero;valuesRangeOriginal=$null;
        valuesOriginal=$null;valuesConstructorOriginal=$null;oidOriginal=$null;oidConstructorAttempted=$false;
        decodedReturned=$false;exceptionOriginal=$null}
      $h.attributesOriginal.Add($a)
      try{
        $a.wordsOriginal=[uint32[]]::new(6);$offset=24*$index
        for($word=0;$word -lt 6;$word++){$a.wordsOriginal[$word]=[BitConverter]::ToUInt32($h.rawBytesOriginal,$offset+4*$word)}
        $a.oidPointerOriginal=[IntPtr]::new([BitConverter]::ToInt64($h.rawBytesOriginal,$offset))
        $a.countOriginal=[BitConverter]::ToUInt32($h.rawBytesOriginal,$offset+8)
        $a.valuesPointerOriginal=[IntPtr]::new([BitConverter]::ToInt64($h.rawBytesOriginal,$offset+16))
        $a.decodedReturned=$true
      }catch{$a.exceptionOriginal=$_;throw}
    }
    $ctor.returned=$true;return $h
  }catch{$ctor.exceptionOriginal=$_;throw}
}

function Decode-Cli39ProviderAttributeValues32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$a=$AttributeOriginal
  $h=Get-Cli39ProviderHeaderRange32 $p $null $null '' '' $a
  if(-not (Test-Cli39ProviderAttributeMember32 $capture $a) -or
      (-not $h.copyReturned -and -not $h.absenceObserved) -or
      $h.rawBytesOriginal.Length -ne $h.bytesExpected -or $null -ne $a.valuesConstructorOriginal){throw 'Actual SAME once-only attribute values array required.'}
  $ctor=@{attributeOriginal=$a;arrayOriginal=$h;attempted=$true;returned=$false;recordsOriginal=$null;exceptionOriginal=$null}
  $a.valuesConstructorOriginal=$ctor
  try{
    $a.valuesOriginal=[Collections.Generic.List[object]]::new([int]$a.countOriginal)
    $ctor.recordsOriginal=$a.valuesOriginal
    for([int]$index=0;$index -lt $a.countOriginal;$index++){
      $v=@{attributeOriginal=$a;arrayOriginal=$h;indexOriginal=$index;wordsOriginal=$null;
        countOriginal=([uint32]0);pointerOriginal=[IntPtr]::Zero;rangeOriginal=$null;
        decodedReturned=$false;exceptionOriginal=$null}
      $a.valuesOriginal.Add($v)
      try{
        $v.wordsOriginal=[uint32[]]::new(4);$offset=16*$index
        for($word=0;$word -lt 4;$word++){$v.wordsOriginal[$word]=[BitConverter]::ToUInt32($h.rawBytesOriginal,$offset+4*$word)}
        $v.countOriginal=[BitConverter]::ToUInt32($h.rawBytesOriginal,$offset)
        $v.pointerOriginal=[IntPtr]::new([BitConverter]::ToInt64($h.rawBytesOriginal,$offset+8))
        $v.decodedReturned=$true
      }catch{$v.exceptionOriginal=$_;throw}
    }
    $ctor.returned=$true;return $a
  }catch{$ctor.exceptionOriginal=$_;throw}
}

function Capture-Cli39ProviderAttributeRange32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$AttributeOriginal,$ValueOriginal=$null)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$capture=$p.consumerOriginal.publisherNativeOriginal.providerCaptureOriginal;$a=$AttributeOriginal;$v=$ValueOriginal
  if(-not (Test-Cli39ProviderAttributeMember32 $capture $a) -or -not $a.decodedReturned){throw 'Actual SAME decoded attribute required.'}
  if($null -eq $v){
    if($null -ne $a.valuesRangeOriginal){throw 'Actual attribute values array cannot reset/retry.'}
    $pointer=$a.valuesPointerOriginal;[long]$extent=16L*[long]$a.countOriginal
  }else{
    if(-not (Test-Cli39ProviderAttributeValueMember32 $capture $a $v) -or -not $v.decodedReturned -or
        $null -ne $v.rangeOriginal){throw 'Actual SAME once-only decoded attribute value required.'}
    $pointer=$v.pointerOriginal;[long]$extent=$v.countOriginal
  }
  $h=@{captureOriginal=$capture;providerPointerOriginal=$pointer;stateWorkerOriginal=$capture.stateWorkerOriginal;
    attributeOriginal=$a;attributeValueOriginal=$v;bytesExpected=$extent;used=0;
    pagesOriginal=$null;rangeReturned=$false;rawBytesOriginal=$null;copyWorkerOriginal=$null;
    copyReturned=$false;absenceObserved=$false;exceptionOriginal=$null}
  if($null -eq $v){$a.valuesRangeOriginal=$h}else{$v.rangeOriginal=$h}
  try{
    $null=Get-Cli39ProviderHeaderRange32 $p $null $null '' '' $a $v
    $h.pagesOriginal=[Collections.Generic.List[object]]::new([int]$extent)
    $h.rawBytesOriginal=[byte[]]::new([int]$extent)
    if($extent -eq 0){
      $h.absenceObserved=$true;$h.rangeReturned=$true
      return $h
    }
    while(-not $h.rangeReturned){
      $w=New-Cli39ProviderPageWorker32 $p $null $null '' '' $a $v
      $null=Start-Cli39PublisherWorker26 $w
      while(-not $w.terminalJoined){
        $null=Observe-Cli39PublisherWorker26 $w
        if(-not $w.terminalJoined){$null=[Threading.Thread]::Yield()}
      }
      $null=Complete-Cli39ProviderPage32 $w
    }
    $null=Copy-Cli39ProviderHeader32 $p $null $null '' '' $a $v
    return $h
  }catch{$h.exceptionOriginal=$_;throw}
}

function Capture-Cli39ProviderAttributes32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  Assert-Cli39PublisherCollectionOpen32 $OriginalPreparation
  $p=$OriginalPreparation;$s=$SignerOriginal
  foreach($part in @('auth-attributes','unauth-attributes')){
    $h=Decode-Cli39ProviderAttributes32 $p $s $part
    foreach($a in $h.attributesOriginal){
      $null=Capture-Cli39ProviderAttributeRange32 $p $a
      $null=Decode-Cli39ProviderAttributeValues32 $p $a
      foreach($v in $a.valuesOriginal){$null=Capture-Cli39ProviderAttributeRange32 $p $a $v}
      $null=Capture-Cli39ProviderAttributeOid32 $p $a
    }
  }
  # Every encoded attribute value is retained independently of its OID. Full
  # OID strings/timestamp semantics/source joins remain required, not empty.
  return $s
}

function Test-Cli39ProviderAttributesBorrowClosed32 {
  param([Parameter(Mandatory)]$OriginalPreparation,[Parameter(Mandatory)]$SignerOriginal)
  $p=$OriginalPreparation;$s=$SignerOriginal
  foreach($part in @('auth-attributes','unauth-attributes')){
    $h=$s.signerFieldOriginals[$part]
    if($null -eq $h){continue}
    foreach($a in $h.attributesOriginal){
      if(-not (Test-Cli39ProviderAttributeOidBorrowClosed32 $p $a)){return $false}
      if($null -ne $a.valuesRangeOriginal -and
          -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p $null $null '' '' $a)){return $false}
      foreach($v in $a.valuesOriginal){
        if($null -ne $v.rangeOriginal -and
            -not (Test-Cli39ProviderHeaderRangeBorrowClosed32 $p $null $null '' '' $a $v)){return $false}
      }
    }
  }
  return $true
}
