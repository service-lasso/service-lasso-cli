param([Parameter(Mandatory)][string]$SharedSourceRoot)
$ErrorActionPreference='Stop'
# Surrogate captured originals exercise pure source correspondence, not native trust.
$repo=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
. (Join-Path $SharedSourceRoot 'scripts/windows-native-producer-owner.ps1')
. (Join-Path $repo 'scripts/publisher-signer-worker32.ps1')
. (Join-Path $repo 'scripts/publisher-chain32.ps1')
. (Join-Path $repo 'scripts/publisher-anchor32.ps1')
. (Join-Path $repo 'scripts/publisher-evaluation32.ps1')
. (Join-Path $repo 'scripts/publisher-revocation32.ps1')
function Assert-Case([bool]$Value,[string]$Name){if(-not $Value){throw ('Case failed: '+$Name)}}
function Assert-Reject([scriptblock]$Body,[string]$Name){$failed=$false;try{& $Body}catch{$failed=$true};Assert-Case $failed $Name}
function New-SurrogateFixture {
  $receiver=@{frames=@{}};$n=@{};$p=@{key='fixture';receiver=$receiver;consumerOriginal=@{publisherNativeOriginal=$n};
    deadlineReported=$false;originalDeadlineTicks=[long]::MaxValue;deadlineResponse={};deadlineResponseOriginal=$null}
  $receiver.frames[$p.key]=$p
  $capture=@{returned=$true;collectionTerminal=$true;custodyCompletionOriginal=@{returned=$true};signerOriginals=[Collections.Generic.List[object]]::new();
    headerOriginal=@{decodedOriginal=@{fieldsOriginal=@{dwRegSecuritySettings=[uint32]1;dwRegPolicySettings=[uint32]2;dwTrustPubSettings=[uint32]3;dwProvFlags=[uint32]0x00040000}}};
    sequenceOriginal=@{noStateObserved=$false}}
  $n.providerCaptureOriginal=$capture
  $book=@{preparationOriginal=$p;constructionTerminal=$true;constructionFailure=$null;recordsOriginal=[Collections.Generic.List[object]]::new()}
  $n.chainReservationOriginal=$book
  $signer=@{captureOriginal=$capture;primarySignerOriginal=$null;headerRangeOriginal=@{copyReturned=$true;decodedOriginal=@{returned=$true;fieldsOriginal=@{pChainContext=[IntPtr]100}}}}
  $capture.signerOriginals.Add($signer)
  $basis=[object[]]@(@{registrySecuritySettings=[uint32]1;registryPolicySettings=[uint32]2;trustPublisherSettings=[uint32]3},@{},@{},@{},@{maximumFreshnessSeconds=[uint32]20})
  $transport=@{returned=$true;operandsOriginal=@{basisOriginals=$basis;anchorsOriginal=[string[]]@('0102')}}
  $root=@{windowsPolicyFoundationTransportOriginal=$transport};$p.consumerOriginal.rootCaptureOriginal=$root
  $n.custodyReservationOriginal=@{preparationOriginal=$p;perSignerChargeBytes=100D;usedChargeBytes=0D;limitBytes=10000D;
    anchorObservationChargeAttempted=$false;evaluationObservationChargeAttempted=$false;revocationObservationChargeAttempted=$false}
  $time=@{returned=$true;terminal=$true;exceptionOriginal=$null;captureOriginal=$capture;
    signerRecordsOriginal=[object[]]@(@{returned=$true;signerOriginal=$signer})};$n.timeResultOriginal=$time
  return @{p=$p;n=$n;capture=$capture;book=$book;signer=$signer;transport=$transport}
}
function Add-SurrogateRange($f,$parent,[string]$role,[int]$index,$fields,$items){
  $resolved=Resolve-Cli39PublisherChainRange32 $f.p $parent $role $index
  $r=@{preparationOriginal=$f.p;bookOriginal=$f.book;captureOriginal=$f.capture;parentOriginal=$parent;roleOriginal=$role;indexOriginal=$index;
    pointerOriginal=$resolved.pointerOriginal;providerPointerOriginal=$resolved.pointerOriginal;bytesExpected=$resolved.extentOriginal;kindOriginal=$resolved.kindOriginal;
    charged=$true;constructionTerminal=$true;constructionFailure=$null;copyReturned=$true;decodedReturned=$true;expansionReturned=$true;
    exceptionOriginal=$null;aliasOriginal=$null;fieldsOriginal=$fields;itemsOriginal=$items;absenceObserved=($resolved.extentOriginal -eq 0)}
  $f.book.recordsOriginal.Add($r);return $r
}
function Add-SurrogateGraph($f,[bool]$FinalCtl=$false){
  $context=Add-SurrogateRange $f $f.signer 'chain-context' -1 @{cChain=[uint32]2;rgpChain=[IntPtr]200;
    fHasRevocationFreshnessTime=[uint32]1;dwRevocationFreshnessTime=[uint32]10;dwErrorStatus=[uint32]0} $null
  $array=Add-SurrogateRange $f $context 'simple-chain-array' -1 @{} ([IntPtr[]]@([IntPtr]300,[IntPtr]400))
  $first=Add-SurrogateRange $f $array 'simple-chain' 0 @{cElement=[uint32]1;rgpElement=[IntPtr]500;pTrustListInfo=[IntPtr]900;dwErrorStatus=[uint32]0} $null
  $final=Add-SurrogateRange $f $array 'simple-chain' 1 @{cElement=[uint32]1;rgpElement=[IntPtr]600;pTrustListInfo=$(if($FinalCtl){[IntPtr]901}else{[IntPtr]::Zero});dwErrorStatus=[uint32]0} $null
  $elements=Add-SurrogateRange $f $final 'element-array' -1 @{} ([IntPtr[]]@([IntPtr]700))
  $element=Add-SurrogateRange $f $elements 'element' 0 @{pCertContext=[IntPtr]800;pRevocationInfo=[IntPtr]1100;dwErrorStatus=[uint32]0} $null
  $cert=Add-SurrogateRange $f $element 'certificate-context' -1 @{pbCertEncoded=[IntPtr]1000;cbCertEncoded=[uint32]2} $null
  $der=Add-SurrogateRange $f $cert 'certificate-der' -1 @{} $null
  $der.used=2;$der.rawBytesOriginal=[byte[]]@(1,2)
  $firstElements=Add-SurrogateRange $f $first 'element-array' -1 @{} ([IntPtr[]]@([IntPtr]1200))
  $firstElement=Add-SurrogateRange $f $firstElements 'element' 0 @{pRevocationInfo=[IntPtr]::Zero;dwErrorStatus=[uint32]0} $null
  $null=Add-SurrogateRange $f $firstElement 'revocation' -1 @{} $null
  $revocation=Add-SurrogateRange $f $element 'revocation' -1 @{dwRevocationResult=[uint32]0;fHasFreshnessTime=[uint32]1;dwFreshnessTime=[uint32]10} $null
  return @{context=$context;array=$array;first=$first;final=$final;der=$der;revocation=$revocation}
}
$f=New-SurrogateFixture;$g=Add-SurrogateGraph $f
$a=Observe-Cli39PublisherAnchorResults32 $f.p
Assert-Case ([object]::ReferenceEquals($a.recordsOriginal[0].finalChainOriginal,$g.final)) 'last CTL-linked chain selected'
Assert-Case ($a.recordsOriginal[0].terminalTrustListAbsentObserved -and $a.recordsOriginal[0].matchedCount -eq 1 -and -not $a.qualified) 'anchor correspondence is not qualification'
$e=Observe-Cli39PublisherEvaluationResults32 $f.p
Assert-Case ($e.providerSettingsEqualObserved -and $e.recordsOriginal[0].freshnessWithinBoundObserved -and -not $e.qualified) 'available valid freshness'
Assert-Case ($e.wholeChainRevocationModeObserved -and $e.providerFlagsOriginal -eq [uint32]0x00040000) 'actual provider whole-chain mode observation'
Assert-Reject {Observe-Cli39PublisherEvaluationResults32 $f.p} 'once-only evaluation'
$rev=Observe-Cli39PublisherRevocationResults32 $f.p
Assert-Case ($rev.countOriginal -eq 2 -and $rev.recordsOriginal[0].absenceObserved -and -not $rev.recordsOriginal[0].freshnessAvailableObserved) 'absent revocation not fresh or non-applicable'
Assert-Case ($rev.recordsOriginal[1].finalAnchorPositionObserved -and $rev.recordsOriginal[1].freshnessWithinBoundObserved -and $null -eq $rev.recordsOriginal[1].applicabilityOriginal -and -not $rev.qualified) 'actual available element freshness not applicability'
$f=New-SurrogateFixture;$g=Add-SurrogateGraph $f $true;$a=Observe-Cli39PublisherAnchorResults32 $f.p
Assert-Case (-not $a.recordsOriginal[0].terminalTrustListAbsentObserved) 'final CTL present remains negative'
$f=New-SurrogateFixture;$f.signer.headerRangeOriginal.decodedOriginal.fieldsOriginal.pChainContext=[IntPtr]::Zero
$null=Add-SurrogateRange $f $f.signer 'chain-context' -1 @{} $null
$a=Observe-Cli39PublisherAnchorResults32 $f.p
Assert-Case ($a.recordsOriginal[0].absentObserved -and $a.recordsOriginal[0].matchedCount -eq 0) 'absent chain retains negative'
foreach($mode in @('unavailable','stale','settings','deadline','partial')){
  $f=New-SurrogateFixture;$g=Add-SurrogateGraph $f
  if($mode -eq 'unavailable'){$g.context.fieldsOriginal.fHasRevocationFreshnessTime=[uint32]0;$g.context.fieldsOriginal.dwRevocationFreshnessTime=[uint32]0}
  if($mode -eq 'stale'){$g.context.fieldsOriginal.dwRevocationFreshnessTime=[uint32]21}
  if($mode -eq 'settings'){$f.capture.headerOriginal.decodedOriginal.fieldsOriginal.dwRegPolicySettings=[uint32]99}
  $null=Observe-Cli39PublisherAnchorResults32 $f.p
  if($mode -eq 'deadline'){$f.p.originalDeadlineTicks=0;Assert-Reject {Observe-Cli39PublisherEvaluationResults32 $f.p} 'original D';Assert-Case ($f.n.evaluationResultOriginal.terminal -and $null -ne $f.n.evaluationResultOriginal.exceptionOriginal) 'deadline failure retained';continue}
  if($mode -eq 'partial'){$g.final.constructionFailure=[Exception]::new('surrogate partial');Assert-Reject {Observe-Cli39PublisherEvaluationResults32 $f.p} 'partial range';Assert-Case ($f.n.evaluationResultOriginal.recordsOriginal.Count -eq 1 -and -not $f.n.evaluationResultOriginal.returned) 'partial journal retained';continue}
  $e=Observe-Cli39PublisherEvaluationResults32 $f.p
  if($mode -eq 'settings'){Assert-Case (-not $e.providerSettingsEqualObserved) 'OS mismatch retained'}
  else{Assert-Case (-not $e.recordsOriginal[0].freshnessWithinBoundObserved) 'unavailable/stale denies freshness'}
}
$f=New-SurrogateFixture;$g=Add-SurrogateGraph $f
# A second actual signer alias points to the same canonical chain context.
$second=@{captureOriginal=$f.capture;primarySignerOriginal=$null;headerRangeOriginal=@{copyReturned=$true;decodedOriginal=@{returned=$true;fieldsOriginal=@{pChainContext=[IntPtr]100}}}}
$f.capture.signerOriginals.Add($second)
$alias=Add-SurrogateRange $f $second 'chain-context' -1 $g.context.fieldsOriginal $null;$alias.aliasOriginal=$g.context
$child=Get-Cli39PublisherPolicyChainChild32 $f.p $alias 'simple-chain-array'
Assert-Case ([object]::ReferenceEquals($child,$g.array)) 'canonical alias child custody'
$duplicate=Add-SurrogateRange $f $g.context 'simple-chain-array' -1 @{} ([IntPtr[]]@([IntPtr]300,[IntPtr]400))
Assert-Reject {Get-Cli39PublisherPolicyChainChild32 $f.p $alias 'simple-chain-array'} 'duplicate actual canonical child'
$f=New-SurrogateFixture;$g=Add-SurrogateGraph $f
$second=@{captureOriginal=$f.capture;primarySignerOriginal=$null;headerRangeOriginal=@{copyReturned=$true;decodedOriginal=@{returned=$true;fieldsOriginal=@{pChainContext=[IntPtr]100}}}}
$f.capture.signerOriginals.Add($second)
$alias=Add-SurrogateRange $f $second 'chain-context' -1 $g.context.fieldsOriginal $null
$other=New-SurrogateFixture;$otherGraph=Add-SurrogateGraph $other;$alias.aliasOriginal=$otherGraph.context
Assert-Reject {Get-Cli39PublisherPolicyChainChild32 $f.p $alias 'simple-chain-array'} 'foreign canonical alias rejected'
. (Join-Path $repo 'scripts/prerequisite-foundation32.ps1')
. (Join-Path $repo 'scripts/prerequisite-node-policy32.ps1')
. (Join-Path $repo 'scripts/prerequisite-qualification-startup32.ps1')
function New-NodeSurrogate {
  $receiver=@{frames=@{}};$pins=@{};$rows=[object[]]::new(4);$ordinal=0
  foreach($role in @('executable','keyring','signature','listing')){
    $pin=[pscustomobject]@{role=$role;path=('D:/surrogate/'+$role);bytes=[long]1;sha256=('a'*64)}
    $rows[$ordinal]=$pin;$pins[$role]=@{path=$pin.path;bytes=$pin.bytes;sha256=$pin.sha256};$ordinal++
  }
  $root=@{actionOriginal='checksum-signature';consumerShapeOriginal=@{returned=$true;prototypeOriginal=@{subjectPins=$pins}}}
  $p=@{receiver=$receiver;key='node-fixture';inputRead=@{value=$root};consumerOriginal=@{};
    originalDeadlineTicks=[long]::MaxValue;deadlineReported=$false;deadlineResponse={}}
  $receiver.frames[$p.key]=$p;$lifetime=[object]::new();$fingerprint='CC68F5A3106FF448322E48ED27F5E38D5B0A215F'
  $f=[pscustomobject]@{kind='cli39.qualified-node-verification-policy.v2';provenanceOriginal=[object]::new();lifetimeOriginal=$lifetime;
    action='checksum-signature';primaryFingerprint=$fingerprint;expectedKeyFingerprints=[string[]]@($fingerprint);
    expectedSigningFingerprints=[string[]]@($fingerprint);allowedPublicKeyAlgorithms=[string[]]@('1');allowedHashAlgorithms=[string[]]@('8');
    notBefore=[long]1;notAfter=[long]3;validationTime=[long]2;validationOriginal=$null;baselineOriginal=$null;currentRevocationOriginal=$null;subjectPinsOriginal=$rows;toolLoaderOriginals=[object[]]@()}
  foreach($pair in @(@('validationOriginal','cli39.qualified-node-validation.v1'),@('baselineOriginal','cli39.qualified-node-baseline.v1'),@('currentRevocationOriginal','cli39.qualified-node-current-revocation.v1'))){
    $f.($pair[0])=[pscustomobject]@{kind=$pair[1];provenanceOriginal=[object]::new();lifetimeOriginal=$lifetime;
      primaryFingerprint=$fingerprint;keyringSha256=('a'*64);validationTime=[long]2;sourceOriginal=[object]::new()}
  }
  return @{p=$p;root=$root;foundation=$f;lifetime=$lifetime}
}
$x=New-NodeSurrogate;$joined=Join-Cli39NodeVerificationPolicy32 $x.p $x.foundation $x.lifetime
Assert-Case ($joined.returned -and $joined.subjectRowsOriginal.Count -eq 4 -and $joined.basisRowsOriginal.Count -eq 3) 'unchanged surrogate Node correspondence'
foreach($mode in @('subject','time','pin-getter','basis-getter')){
  $x=New-NodeSurrogate;$script:GetterCount=0
  if($mode -eq 'subject'){$x.foundation.subjectPinsOriginal[0].path='D:/surrogate/changed'}
  if($mode -eq 'time'){$x.foundation.validationOriginal.validationTime=[long]3}
  if($mode -eq 'pin-getter'){
    $bad=[pscustomobject]@{role='executable';bytes=[long]1;sha256=('a'*64)}
    Add-Member -InputObject $bad -MemberType ScriptProperty -Name path -Value {$script:GetterCount++;throw 'getter must not execute'}
    $x.foundation.subjectPinsOriginal[0]=$bad
  }
  if($mode -eq 'basis-getter'){
    $bad=[pscustomobject]@{provenanceOriginal=[object]::new();lifetimeOriginal=$x.lifetime;primaryFingerprint=$x.foundation.primaryFingerprint;
      keyringSha256=('a'*64);validationTime=[long]2;sourceOriginal=[object]::new()}
    Add-Member -InputObject $bad -MemberType ScriptProperty -Name kind -Value {$script:GetterCount++;throw 'getter must not execute'}
    $x.foundation.validationOriginal=$bad
  }
  Assert-Reject {Join-Cli39NodeVerificationPolicy32 $x.p $x.foundation $x.lifetime} ('actual Node '+$mode+' rejection')
  Assert-Case ($x.root.nodePolicyOriginal.terminal -and -not $x.root.nodePolicyOriginal.returned -and $null -ne $x.root.nodePolicyOriginal.exceptionOriginal) 'Node failure journal retained'
  if($mode -cin @('pin-getter','basis-getter')){Assert-Case ($script:GetterCount -eq 0) 'closed shape rejected before getter'}
}
function New-StartupSurrogate {
  $receiver=@{frames=@{}};$p=@{receiver=$receiver;key='startup-fixture';originalDeadlineTicks=[long]::MaxValue;
    deadlineResponse={};inputRead=@{value=$null};consumerOriginal=$null};$receiver.frames[$p.key]=$p
  $startupArguments=@{HostOwnerOriginal=$p;StartupReservationOriginal=[object]::new();StartupLifetimeOriginal=[object]::new();
    SourceFoundationOriginal=[object]::new();ToolFoundationOriginal=[object]::new();InstalledFoundationOriginal=[object]::new();
    LoadedFoundationOriginal=[object]::new();NamespaceFoundationOriginal=[object]::new();PrincipalFoundationOriginal=[object]::new();
    CapabilityFoundationOriginal=[object]::new();AllocationFoundationOriginal=[object]::new();PolicyFoundationOriginal=[object]::new();
    InputRootPath='D:/surrogate/root.json';InputRootBytes=[long]1;InputRootSha256=('a'*64);SharedSourceRoot=$SharedSourceRoot;Action='publisher'}
  $carrier=New-Cli39QualificationStartup32 @startupArguments
  return @{p=$p;carrier=$carrier;journal=$p.qualificationCarrierConstructionOriginal}
}
$x=New-StartupSurrogate;$output=@(Assert-Cli39QualificationStartupCustody32 $x.carrier)
Assert-Case ($output.Count -eq 0 -and $null -eq $x.carrier.CallerOriginal) 'unchanged startup custody has no success output/effect'
foreach($mode in @('foundation','root')){
  $x=New-StartupSurrogate;$originalSource=$x.journal.sourceOriginal;$originalRoot=$x.journal.rootPathOriginal
  if($mode -eq 'foundation'){$x.carrier.SourceFoundationOriginal=[object]::new()}else{$x.carrier.InputRootPathOriginal='D:/surrogate/changed.json'}
  Assert-Reject {Assert-Cli39QualificationStartupCustody32 $x.carrier} ('changed startup '+$mode+' rejected')
  Assert-Case ([object]::ReferenceEquals($x.journal.sourceOriginal,$originalSource) -and $x.journal.rootPathOriginal -ceq $originalRoot -and $null -eq $x.carrier.CallerOriginal) 'original startup pins retained before caller effect'
}
. (Join-Path $repo 'scripts/publisher-policy32.ps1')
foreach($rawLong in @(0,1)){
  $f=New-SurrogateFixture;$g=Add-SurrogateGraph $f
  # Preclassified surrogate capture is not the real classifier/native proof.
  $f.capture.preparationOriginal=$f.p
  $f.capture.sequenceOriginal.rawGraphClassificationOriginal=@{returned=$true;completeObserved=$true}
  $f.capture.providerFieldOriginals=@{'trust-step-errors'=@{decodedOriginal=@{itemsOriginal=[uint32[]]@(0)}}}
  $f.capture.headerOriginal.decodedOriginal.fieldsOriginal.dwError=[uint32]0
  $f.capture.headerOriginal.decodedOriginal.fieldsOriginal.dwFinalError=[uint32]0
  $f.signer.headerRangeOriginal.decodedOriginal.fieldsOriginal.dwError=[uint32]0
  $f.signer.counterSignerOriginals=[Collections.Generic.List[object]]::new()
  $f.n.verifyWorkerOriginal=@{terminalJoined=$true;thread=@{IsAlive=$false};results=[int[]]@($rawLong,1);exceptionOriginal=[object[]]@($null)}
  $f.book.recordsOriginal.Add(@{kindOriginal='chain-context';decodedReturned=$true;absenceObserved=$false;fieldsOriginal=@{dwErrorStatus=[uint32]32}})
  $result=Observe-Cli39PublisherPolicyResults32 $f.p
  Assert-Case ($result.returned -and -not $result.zeroRawErrorsObserved) 'alternative context error retained'
  Assert-Case ($result.providerOperationZeroErrorsObserved -eq ($rawLong -eq 0)) 'operation result not borrowed from alternative context'
}
foreach($mode in @('different','missing')){
  $f=New-SurrogateFixture;$g=Add-SurrogateGraph $f
  if($mode -eq 'different'){$f.capture.headerOriginal.decodedOriginal.fieldsOriginal.dwProvFlags=[uint32]0x00020000}
  else{$f.capture.headerOriginal.decodedOriginal.fieldsOriginal.Remove('dwProvFlags')}
  $null=Observe-Cli39PublisherAnchorResults32 $f.p
  if($mode -eq 'missing'){
    Assert-Reject {Observe-Cli39PublisherEvaluationResults32 $f.p} 'missing provider mode DWORD rejected'
    Assert-Case ($f.n.evaluationResultOriginal.terminal -and $null -ne $f.n.evaluationResultOriginal.exceptionOriginal -and -not $f.n.evaluationResultOriginal.returned) 'missing provider mode failure retained'
  }else{
    $e=Observe-Cli39PublisherEvaluationResults32 $f.p
    Assert-Case ($e.returned -and -not $e.wholeChainRevocationModeObserved -and -not $e.qualified) 'actual non-whole-chain mode stays negative'
  }
}
. (Join-Path $repo 'scripts/publisher-semantic32.ps1')
function New-SemanticSurrogate {
  $f=New-SurrogateFixture;$g=Add-SurrogateGraph $f
  $anchors=Observe-Cli39PublisherAnchorResults32 $f.p
  $evaluation=Observe-Cli39PublisherEvaluationResults32 $f.p
  $revocation=Observe-Cli39PublisherRevocationResults32 $f.p
  $time=$f.n.timeResultOriginal;$time.timestampModeOriginal='qualified-current-policy'
  $time.providerTimeInIntervalObserved=$true
  # These explicit surrogate completed source transcripts are not native proof.
  $leaf=@{returned=$true;signerOriginal=$f.signer;equalBytes=$true;signerErrorOriginal=[uint32]0;
    certificateOriginal=@{chainAssociationOriginal=@{returned=$true;exceptionOriginal=$null;
      chainContextMatches=$true;certificateContextMatches=$true;equalBytes=$true}}}
  $timestamp=@{returned=$true;exceptionOriginal=$null;captureOriginal=$f.capture;
    recordsOriginal=[object[]]@(@{returned=$true;primaryOriginal=$f.signer;recognizedAttributeCount=0;counterRecordsOriginal=[object[]]@()})}
  $a=@{preparationOriginal=$f.p;captureOriginal=$f.capture;foundationTransportOriginal=$f.transport;
    providerResultOriginal=@{returned=$true;captureOriginal=$f.capture;exceptionOriginal=$null;providerOperationZeroErrorsObserved=$true};
    timeResultOriginal=$time;anchorResultOriginal=$anchors;evaluationResultOriginal=$evaluation;
    revocationResultOriginal=$revocation;timestampResultOriginal=$timestamp;
    certificateComparisonOriginal=@{returned=$true;constructionTerminal=$true;exceptionOriginal=$null;
      captureOriginal=$f.capture;recordsOriginal=[object[]]@($leaf)}}
  $f.n.policyOriginal=@{associationConstructionOriginal=$a}
  return @{fixture=$f;association=$a;leaf=$leaf;graph=$g}
}
foreach($mode in @('unchanged','subject','time','os','freshness','duplicate','deadline')){
  $x=New-SemanticSurrogate;$f=$x.fixture;$a=$x.association
  if($mode -eq 'subject'){$x.leaf.equalBytes=$false}
  if($mode -eq 'time'){$a.timeResultOriginal.providerTimeInIntervalObserved=$false}
  if($mode -eq 'os'){$a.evaluationResultOriginal.providerSettingsEqualObserved=$false}
  if($mode -eq 'freshness'){$a.evaluationResultOriginal.recordsOriginal[0].freshnessWithinBoundObserved=$false}
  if($mode -eq 'duplicate'){$a.certificateComparisonOriginal.recordsOriginal=[object[]]@($x.leaf,$x.leaf)}
  if($mode -eq 'deadline'){$f.p.originalDeadlineTicks=0}
  if($mode -cin @('duplicate','deadline')){
    Assert-Reject {Join-Cli39PublisherSemanticResults32 $a} ('semantic '+$mode+' rejection')
    Assert-Case ($f.n.semanticResultOriginal.terminal -and $null -ne $f.n.semanticResultOriginal.exceptionOriginal -and -not $f.n.semanticResultOriginal.returned) 'semantic original failure retained'
  }else{
    $result=Join-Cli39PublisherSemanticResults32 $a
    Assert-Case ($result.returned -and $result.supportedRelationsObserved -eq ($mode -ceq 'unchanged')) ('semantic '+$mode+' correspondence')
    Assert-Case ([object]::ReferenceEquals($result.associationOriginal,$a)) 'semantic original association retained'
  }
}
foreach($mode in @('different-counter-time','wrong-primary','missing-counter-time','no-attribute','counter-error','counter-chain','counter-stale')){
  $x=New-SemanticSurrogate;$f=$x.fixture;$a=$x.association
  $a.timeResultOriginal.timestampModeOriginal='qualified-provider-timestamp'
  $primaryTime=$a.timeResultOriginal.signerRecordsOriginal[0]
  $primaryTime.timeOriginal=[uint64]10;$primaryTime.insideIntervalObserved=$true;$primaryTime.selectedValidationEqualObserved=$true
  # Explicit surrogate completed timestamp observation, not native timestamp evidence.
  $counter=@{primarySignerOriginal=$f.signer}
  $counterTime=@{returned=$true;signerOriginal=$counter;primaryOriginal=$f.signer;timeOriginal=[uint64]999}
  $counterEvaluation=@{returned=$true;signerOriginal=$counter;selectedChainZeroErrorsObserved=$true;
    freshnessAvailableObserved=$true;freshnessWithinBoundObserved=$true}
  $counterRecord=@{returned=$true;counterOriginal=$counter;primaryOriginal=$f.signer;
    timestampTypeObserved=$true;signerErrorOriginal=[uint32]0;timeRecordOriginal=$counterTime;
    anchorRecordOriginal=@{absentObserved=$false;terminalTrustListAbsentObserved=$true;matchedCount=1}}
  $a.timestampResultOriginal.recordsOriginal[0].recognizedAttributeCount=1
  $a.timestampResultOriginal.recordsOriginal[0].counterRecordsOriginal=[object[]]@($counterRecord)
  $a.evaluationResultOriginal.recordsOriginal=[object[]]@($a.evaluationResultOriginal.recordsOriginal[0],$counterEvaluation)
  if($mode -eq 'wrong-primary'){$counterTime.primaryOriginal=[object]::new()}
  if($mode -eq 'missing-counter-time'){$counterRecord.timeRecordOriginal=$null}
  if($mode -eq 'no-attribute'){$a.timestampResultOriginal.recordsOriginal[0].recognizedAttributeCount=0}
  if($mode -eq 'counter-error'){$counterRecord.signerErrorOriginal=[uint32]1}
  if($mode -eq 'counter-chain'){$counterEvaluation.selectedChainZeroErrorsObserved=$false}
  if($mode -eq 'counter-stale'){$counterEvaluation.freshnessWithinBoundObserved=$false}
  $result=Join-Cli39PublisherSemanticResults32 $a
  Assert-Case ($result.returned -and $result.supportedRelationsObserved -eq ($mode -ceq 'different-counter-time')) ('timestamp semantic '+$mode)
  if($mode -ceq 'different-counter-time'){
    Assert-Case ([object]::ReferenceEquals($result.recordsOriginal[0].timestampTimeOriginal,$counterTime) -and $counterTime.timeOriginal -ne $primaryTime.timeOriginal) 'distinct actual counter time retained without invented equality'
  }
}
. (Join-Path $repo 'scripts/prerequisite-member-file-query32.ps1')
. (Join-Path $repo 'scripts/publisher-prepared-identity32.ps1')
foreach($mode in @('closed','failed-pin','unknown-native','unstarted-constructor','unknown-start')){
  # Explicit surrogate worker transcript tests the actual distinct predicates.
  $receiver=@{frames=@{}};$subject=@{};$n=@{}
  $p=@{key='prepared-closure';receiver=$receiver;consumerOriginal=@{slot='cli39.windows-publisher-trust.v1';publisherNativeOriginal=$n;subjectOriginals=@{'publisher-subject'=$subject}}}
  $receiver.frames[$p.key]=$p
  $b=@{preparationOriginal=$p;terminal=$true;recordsOriginal=[Collections.Generic.List[object]]::new()};$n.preparedIdentityOriginal=$b
  $r=@{bookOriginal=$b;roleOriginal='publisher-subject';subjectOriginal=$subject;fileQueryOriginal=$null};$b.recordsOriginal.Add($r)
  $w=@{memberOriginal=$r;subjectOriginal=$subject;hostOwnerOriginal=$p;constructorTerminal=$true;
    constructorFailure=$null;startAttempted=$true;threadOriginal=@{IsAlive=$false};terminalJoined=$true;
    pendingActive=$false;multiplicityUnknown=$false;joinException=$null;borrowMarkers=[int[]]@(1,1,1,1);
    borrowOriginal=[bool[]]@($true);pinMarkers=[int[]]@(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
    callMarkers=[int[]]@(1,1,1,1,1,1,1,1);exceptionsOriginal=[object[]]::new(6)}
  $r.fileQueryOriginal=$w
  if($mode -eq 'failed-pin'){$w.pinMarkers[3]=0;$w.exceptionsOriginal[1]=[Exception]::new('surrogate failed pin Free')}
  if($mode -eq 'unknown-native'){$w.callMarkers[1]=0;$w.borrowMarkers[2]=0;$w.borrowMarkers[3]=0}
  if($mode -eq 'unstarted-constructor'){$w.startAttempted=$false;$w.threadOriginal=$null;$w.constructorFailure=[Exception]::new('surrogate constructor failure');$w.borrowMarkers=[int[]]::new(4);$w.borrowOriginal=[bool[]]@($false);$w.pinMarkers=[int[]]::new(16);$w.callMarkers=[int[]]::new(8)}
  if($mode -eq 'unknown-start'){$w.threadOriginal=$null;$w.terminalJoined=$false}
  $borrow=Test-Cli39PublisherPreparedIdentityBorrowClosed32 $p
  $closure=Test-Cli39PublisherPreparedIdentityClosure32 $p
  Assert-Case ($borrow -eq ($mode -cin @('closed','failed-pin','unstarted-constructor'))) ('prepared native borrow '+$mode)
  Assert-Case ($closure -eq ($mode -cin @('closed','unstarted-constructor'))) ('prepared full resource closure '+$mode)
  if($mode -eq 'failed-pin'){Assert-Case ($null -ne $w.exceptionsOriginal[1]) 'failed pin original retained independently of known file borrow closure'}
}
. (Join-Path $repo 'scripts/publisher-invoke32.ps1')
foreach($mode in @('unchanged','policy-negative','stage-failure','closure-failure','deadline','foreign-preparation')){
  # Explicit completed-stage surrogates exercise the owning final result only.
  $f=New-SurrogateFixture;$p=$f.p;$c=$p.consumerOriginal
  $a=@{preparationOriginal=$p;returned=$true;constructionTerminal=$true;exceptionOriginal=$null;
    localCorrespondenceComplete=$true;relationOriginals=@{surrogateOnly=$true}}
  $o=@{preparationOriginal=$p;policyAssociationOriginal=$a;returned=$true;closureReturned=$true;
    firstFailureOriginal=$null;usefulCompletionOriginal=$null;usefulReturned=$false;stagesOriginal=@{}}
  $c.publisherInvocationOriginal=$o;$f.n.usefulReturned=$false
  foreach($name in @('prepare','storage','module','initialize','verify','capture','close','dispose','release','closure')){
    $o.stagesOriginal[$name]=@{attempted=$true;returned=$true;exceptionOriginal=$null}
  }
  if($mode -eq 'policy-negative'){$a.localCorrespondenceComplete=$false}
  if($mode -eq 'stage-failure'){$o.stagesOriginal.close.exceptionOriginal=[Exception]::new('surrogate close diagnostic failure')}
  if($mode -eq 'closure-failure'){$o.closureReturned=$false}
  if($mode -eq 'deadline'){$p.originalDeadlineTicks=0}
  if($mode -eq 'foreign-preparation'){$a.preparationOriginal=@{}}
  $r=Complete-Cli39PublisherInvocationResult32 $o
  Assert-Case ([object]::ReferenceEquals($o.usefulCompletionOriginal,$r) -and $r.terminal) ('final retained completion '+$mode)
  Assert-Case ($o.usefulReturned -eq ($mode -ceq 'unchanged') -and $f.n.usefulReturned -eq $o.usefulReturned) ('conditional final result '+$mode)
  if($mode -eq 'deadline'){
    Assert-Case (-not $r.returned -and $null -ne $r.exceptionOriginal -and $null -ne $o.firstFailureOriginal) 'final original deadline failure retained'
  }else{Assert-Case $r.returned ('final known result '+$mode)}
  if($mode -eq 'stage-failure'){Assert-Case ($null -ne $o.stagesOriginal.close.exceptionOriginal) 'final close failure original retained'}
  Assert-Reject {Complete-Cli39PublisherInvocationResult32 $o} ('final once-only '+$mode)
  Assert-Case ([object]::ReferenceEquals($o.usefulCompletionOriginal,$r)) 'repeat cannot overwrite first completion'
}
foreach($mode in @('unchanged','module-handle','process','reservation','source','duplicate-row','deadline')){
  # Source-owned correspondence surrogates; these are not qualified OS inputs.
  $f=New-SurrogateFixture;$p=$f.p;$c=$p.consumerOriginal;$n=$f.n;$root=$c.rootCaptureOriginal
  $budget=@{};$p.reservation=@{budget=$budget}
  $profile=@{returned=$true;role='consumerBudget';actionOriginal='publisher';valuesOriginal=$budget}
  $root.resultOriginal=@{consumerProfileOriginal=$profile};$root.rootProcessOriginal=@{}
  $root.admittedConsumerOriginal=$c;$root.admittedInputOriginal=@{}
  $member=@{};$root.publisherFoundationSelectorOriginal=@{wintrustMemberOriginal=$member}
  $c.subjectOriginals=@{wintrust=@{}}
  $actualModule=@{returned=$true;decodedOriginal=@{moduleHandleOriginal=[IntPtr]123}}
  $loaded=@{returned=$true;installedMemberOriginal=$member;actualModuleOriginal=$actualModule;actualProcessOriginal=$root.rootProcessOriginal}
  $row=@{returned=$true;operandOriginal=@{role='wintrust'};loadedRowOriginal=$loaded}
  $root.childDerivedTransportOriginal=@{}
  $transport=$root.windowsPolicyFoundationTransportOriginal
  $transport.rootOriginal=$root;$transport.parentTransportOriginal=$root.childDerivedTransportOriginal
  $transport.exceptionOriginal=$null;$transport.subjectRowsOriginal=[object[]]@($row)
  $n.preparedIdentityOriginal=@{returned=$true;terminal=$true;exceptionOriginal=$null;preparationOriginal=$p}
  $n.wintrustModuleOriginal=@{constructionTerminal=$true;constructorFailure=$null;preparation=$p;subjectOriginal=$c.subjectOriginals.wintrust;module=[IntPtr]123}
  $n.custodyReservationOriginal.profileOriginal=$profile
  $root.nativeAssociationOriginals=@{consumerReservationOriginal=$p.reservation;allocationAssociationOriginal=@{returned=$true};
    loadedAssociationOriginal=@{returned=$true};sourceAssociationOriginal=@{};toolAssociationOriginal=@{};installedAssociationOriginal=@{}}
  if($mode -eq 'module-handle'){$actualModule.decodedOriginal.moduleHandleOriginal=[IntPtr]124}
  if($mode -eq 'process'){$loaded.actualProcessOriginal=@{}}
  if($mode -eq 'reservation'){$root.nativeAssociationOriginals.consumerReservationOriginal=@{}}
  if($mode -eq 'source'){$root.nativeAssociationOriginals.sourceAssociationOriginal=$null}
  if($mode -eq 'duplicate-row'){$transport.subjectRowsOriginal=[object[]]@($row,$row)}
  if($mode -eq 'deadline'){$p.originalDeadlineTicks=0}
  if($mode -eq 'unchanged'){
    $r=Join-Cli39PublisherExecutionBindings32 $p
    Assert-Case ($r.returned -and $r.terminal -and [object]::ReferenceEquals($n.namespaceAssociationOriginal,$n.preparedIdentityOriginal) -and [object]::ReferenceEquals($n.loadedAssociationOriginal,$r)) 'execution exact reference graph retained'
    Assert-Case ([object]::ReferenceEquals($r.sourceProfileOriginal.reservationOriginal,$p.reservation)) 'execution original reservation retained'
  }else{
    Assert-Reject {Join-Cli39PublisherExecutionBindings32 $p} ('execution reject '+$mode)
    if($mode -ne 'reservation'){Assert-Case ($n.executionBindingOriginal.terminal -and $null -ne $n.executionBindingOriginal.exceptionOriginal -and -not $n.executionBindingOriginal.returned) ('execution failure journal '+$mode)}
    Assert-Case ($null -eq $n.completeSourceProfileOriginal) ('execution no publication '+$mode)
  }
}
. (Join-Path $repo 'scripts/prerequisite-node-identity32.ps1')
foreach($mode in @('pending-one','known-return','foreign-worker')){
  # Surrogate native-query transcript; no real worker or handle is created.
  $receiver=@{frames=@{}};$first=@{stream=@{};handleOriginal=@{}};$other=@{stream=@{};handleOriginal=@{}}
  $c=@{slot='cli39.node-key-policy.v1';subjectOriginals=@{executable=$first;keyring=$other}}
  $p=@{key='node-partial';receiver=$receiver;consumerOriginal=$c};$receiver.frames[$p.key]=$p
  $b=@{preparationOriginal=$p;terminal=$true;recordsOriginal=[Collections.Generic.List[object]]::new()};$c.nodePreparedIdentityOriginal=$b
  $record=@{bookOriginal=$b;roleOriginal='executable';subjectOriginal=$first;fileQueryOriginal=$null};$b.recordsOriginal.Add($record)
  $b.recordsOriginal.Add(@{bookOriginal=$b;roleOriginal='keyring';subjectOriginal=$other;fileQueryOriginal=$null})
  $w=@{memberOriginal=$record;subjectOriginal=$first;hostOwnerOriginal=$p;constructorTerminal=$true;
    startAttempted=$true;threadOriginal=@{IsAlive=$false};terminalJoined=$true;pendingActive=$false;
    multiplicityUnknown=$false;joinException=$null;callMarkers=[int[]]@(1,0,0,0,0,0,0,0);
    borrowMarkers=[int[]]@(1,1,0,0);borrowOriginal=[bool[]]@($true)};$record.fileQueryOriginal=$w
  if($mode -eq 'known-return'){$w.callMarkers[1]=1;$w.borrowMarkers[2]=1;$w.borrowMarkers[3]=1}
  if($mode -eq 'foreign-worker'){$w.memberOriginal=@{}}
  $firstResource=@{sourceOriginal=$first;objectOriginal=$first.stream;handleOriginal=$first.handleOriginal}
  $otherResource=@{sourceOriginal=$other;objectOriginal=$other.stream;handleOriginal=$other.handleOriginal}
  $hashResource=@{sourceOriginal=$first;objectOriginal=@{};handleOriginal=$null}
  $outputResource=@{sourceOriginal=@{};objectOriginal=@{};handleOriginal=@{}}
  Assert-Case ((Test-Cli39NodeResourceDisposalEligible32 $p $firstResource) -eq ($mode -ceq 'known-return')) ('Node pending subject stream '+$mode)
  Assert-Case (Test-Cli39NodeResourceDisposalEligible32 $p $otherResource) ('Node independent other stream '+$mode)
  Assert-Case (Test-Cli39NodeResourceDisposalEligible32 $p $hashResource) ('Node independent hash '+$mode)
  Assert-Case (Test-Cli39NodeResourceDisposalEligible32 $p $outputResource) ('Node independent output '+$mode)
  Assert-Case ($w.callMarkers[1] -eq $(if($mode -ceq 'known-return'){1}else{0})) 'eligibility leaves original native-return observation unchanged'
}
. (Join-Path $repo 'scripts/node-release-verification-consumer26.ps1')
# Fixed CLR-only throwing hash surrogate. No compiler or foreign PS callback.
$hashAssembly=[Reflection.Emit.AssemblyBuilder]::DefineDynamicAssembly([Reflection.AssemblyName]::new('Cli32DisposeCase'+[Guid]::NewGuid().ToString('N')),[Reflection.Emit.AssemblyBuilderAccess]::Run)
$hashModule=$hashAssembly.DefineDynamicModule('fixed-dispose-case')
$hashType=$hashModule.DefineType('Cli32ThrowingHash',[Reflection.TypeAttributes]::Public,[Security.Cryptography.HashAlgorithm])
$null=$hashType.DefineDefaultConstructor([Reflection.MethodAttributes]::Public)
foreach($name in @('Initialize','HashCore','HashFinal','Dispose')){
  $baseMethod=[Security.Cryptography.HashAlgorithm].GetMethods([Reflection.BindingFlags]'Instance,Public,NonPublic') | Where-Object {$_.Name -ceq $name -and ($name -cne 'Dispose' -or ($_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType -eq [bool]))} | Select-Object -First 1
  $parameterTypes=[Type[]]@($baseMethod.GetParameters() | ForEach-Object {$_.ParameterType})
  $access=if($name -ceq 'Initialize'){[Reflection.MethodAttributes]::Public}else{[Reflection.MethodAttributes]::Family}
  $method=$hashType.DefineMethod($name,($access -bor [Reflection.MethodAttributes]::Virtual -bor [Reflection.MethodAttributes]::HideBySig),$baseMethod.ReturnType,$parameterTypes)
  $il=$method.GetILGenerator()
  if($name -ceq 'Dispose'){
    $il.Emit([Reflection.Emit.OpCodes]::Ldstr,'explicit surrogate secondary Dispose failure')
    $il.Emit([Reflection.Emit.OpCodes]::Newobj,[InvalidOperationException].GetConstructor([Type[]]@([string])))
    $il.Emit([Reflection.Emit.OpCodes]::Throw)
  }else{if($name -ceq 'HashFinal'){$il.Emit([Reflection.Emit.OpCodes]::Ldnull)};$il.Emit([Reflection.Emit.OpCodes]::Ret)}
  $hashType.DefineMethodOverride($method,$baseMethod)
}
$throwingHashType=$hashType.CreateType()
$script:cli32DisposalFixtureOriginals=[Collections.Generic.List[object]]::new()
foreach($mode in @('pending-query','failed-pin-known-return')){
  $directory=Join-Path ([IO.Path]::GetTempPath()) ('cli32-dispose-surrogate-'+[Guid]::NewGuid().ToString('N'))
  $null=[IO.Directory]::CreateDirectory($directory)
  $streams=[Collections.Generic.List[object]]::new()
  $case=@{directoryOriginal=$directory;streamsOriginal=$streams;preparationOriginal=$null;failureOriginal=$null;cleanupExceptionOriginal=$null;cleanupPending=$false};$script:cli32DisposalFixtureOriginals.Add($case)
  try{
    foreach($name in @('first','other','output')){$streams.Add([IO.FileStream]::new((Join-Path $directory $name),[IO.FileMode]::CreateNew,[IO.FileAccess]::ReadWrite,[IO.FileShare]::Read))}
    $first=@{role='executable';stream=$streams[0];handleOriginal=$streams[0].SafeFileHandle;hashOriginal=[Activator]::CreateInstance($throwingHashType);openAttempted=$true;openReturned=$true;hashAttempted=$true;hashReturned=$true}
    $other=@{role='keyring';stream=$streams[1];handleOriginal=$streams[1].SafeFileHandle;hashOriginal=$null;openAttempted=$true;openReturned=$true;hashAttempted=$false;hashReturned=$false}
    $output=@{role='diagnostics';stream=$streams[2];handleOriginal=$streams[2].SafeFileHandle;openAttempted=$true;openReturned=$true}
    $receiver=@{frames=@{}};$c=@{slot='cli39.node-key-policy.v1';subjectOriginals=@{executable=$first;keyring=$other};outputOriginals=[object[]]@($output);
      managedPhases=[Collections.Generic.List[object]]::new(16);geometry=@{managedPhaseCount=16;workerStackBytes=1048576}}
    $p=@{key='actual-disposal-surrogate';receiver=$receiver;consumerOriginal=$c;originalDeadlineTicks=[long]::MaxValue;deadlineReported=$false;deadlineResponse={};deadlineResponseOriginal=$null};$receiver.frames[$p.key]=$p;$case.preparationOriginal=$p
    $b=@{preparationOriginal=$p;terminal=$true;recordsOriginal=[Collections.Generic.List[object]]::new()};$c.nodePreparedIdentityOriginal=$b
    $record=@{bookOriginal=$b;roleOriginal='executable';subjectOriginal=$first;fileQueryOriginal=$null};$b.recordsOriginal.Add($record)
    $b.recordsOriginal.Add(@{bookOriginal=$b;roleOriginal='keyring';subjectOriginal=$other;fileQueryOriginal=$null})
    # No real query worker exists. Only disposal workers below are real effects.
    $w=@{memberOriginal=$record;subjectOriginal=$first;hostOwnerOriginal=$p;constructorTerminal=$true;
      startAttempted=$true;threadOriginal=@{IsAlive=$false};terminalJoined=$true;pendingActive=$false;multiplicityUnknown=$false;joinException=$null;
      callMarkers=[int[]]@(1,0,0,0,0,0,0,0);borrowMarkers=[int[]]@(1,1,0,0);borrowOriginal=[bool[]]@($true);
      pinMarkers=[int[]]@(1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0);exceptionsOriginal=[object[]]::new(6)};$record.fileQueryOriginal=$w
    if($mode -ceq 'failed-pin-known-return'){$w.callMarkers[1]=1;$w.borrowMarkers[2]=1;$w.borrowMarkers[3]=1;$w.exceptionsOriginal[1]=[Exception]::new('surrogate pin Free failed')}
    $d=Invoke-CliPrerequisiteDisposal26 $c.slot $p
    $firstStream=@($d.resources | Where-Object {$_.role -ceq 'executable-stream'})[0]
    $hash=@($d.resources | Where-Object {$_.role -ceq 'executable-hash'})[0]
    Assert-Case ($d.returned -and $streams[1].SafeFileHandle.IsClosed -and $streams[2].SafeFileHandle.IsClosed) ('actual independent eligible disposals '+$mode)
    Assert-Case ($hash.disposalAttempted -and -not $hash.disposalReturned -and $null -ne $hash.disposalOriginal -and $hash.disposalWorkerOriginal.terminalJoined) ('actual secondary hash failure retained '+$mode)
    Assert-Case ($firstStream.disposalAttempted -eq ($mode -ceq 'failed-pin-known-return')) ('actual subject-specific stream attempt '+$mode)
    if($mode -ceq 'pending-query'){Assert-Case ($d.providerPending -and -not $streams[0].SafeFileHandle.IsClosed -and $firstStream.borrowPending) 'pending query retains only same borrowed stream'}
    else{Assert-Case ($streams[0].SafeFileHandle.IsClosed -and -not (Test-Cli39NodePreparedIdentityClosure32 $p) -and $null -ne $w.exceptionsOriginal[1]) 'known borrow return permits stream cleanup while failed pin full closure remains false'}
  }catch{$case.failureOriginal=$_;throw}finally{
    # Query markers above are fictional. Real dispose-worker uncertainty still
    # retains the actual fixture streams/directory through host exit.
    if($null -ne $case.preparationOriginal){
      foreach($phase in $case.preparationOriginal.consumerOriginal.managedPhases){
        if($phase.startAttempted -and ($null -eq $phase.thread -or -not $phase.terminalJoined -or $phase.thread.IsAlive -or $phase.pendingActive -or $phase.multiplicityUnknown -or $phase.joinException.present)){$case.cleanupPending=$true}
      }
    }
    if(-not $case.cleanupPending){
      foreach($stream in $streams){try{if(-not $stream.SafeFileHandle.IsClosed){$stream.Dispose()}}catch{if($null -eq $case.cleanupExceptionOriginal){$case.cleanupExceptionOriginal=$_}}}
    }
    if($null -eq $case.failureOriginal -and ($case.cleanupPending -or $null -ne $case.cleanupExceptionOriginal)){throw 'Actual fixture cleanup remains incomplete; originals retained.'}
  }
}
# Conditional composed Node result cases use surrogate captured originals.
# They call the actual final constructor/physical predicate, never tools or a
# replacement result predicate. Synthetic facts do not qualify genuine input.
function New-NodeUsefulSurrogate32 {
  param([string]$Slot='cli39.node-key-policy.v1')
  $root=@{};$receiver=@{frames=@{}}
  $c=@{slot=$Slot;rootCaptureOriginal=$root;subjectPins=@{};subjectOriginals=@{};
    outputOriginals=[Collections.Generic.List[object]]::new();managedPhases=[Collections.Generic.List[object]]::new();
    disposalOriginal=@{returned=$true;unknownAcquisition=$false;providerPending=$false;
      expectedOriginals=[Collections.Generic.HashSet[object]]::new();resources=[Collections.Generic.List[object]]::new()}}
  $p=@{key=[Guid]::NewGuid().ToString('N');receiver=$receiver;consumerOriginal=$c;
    originalDeadlineTicks=[long]::MaxValue;deadlineReported=$false;deadlineResponse={};reservation=@{}}
  $receiver.frames[$p.key]=$p;$c.originalPreparation=$p
  $identity=@{preparationOriginal=$p;reservationOriginal=$p.reservation;returned=$true;terminal=$true;
    exceptionOriginal=$null;recordsOriginal=[Collections.Generic.List[object]]::new()}
  $c.nodePreparedIdentityOriginal=$identity
  foreach($role in @('executable','keyring','signature','listing')){
    $subject=@{used=1L;eof=$true;hashReturned=$true;digest=('aa'*32);
      openAttempted=$false;hashAttempted=$false;stream=$null;hashOriginal=$null}
    $c.subjectOriginals.Add($role,$subject);$c.subjectPins.Add($role,@{bytes=1L;sha256=('aa'*32)})
    $identity.recordsOriginal.Add(@{bookOriginal=$identity;roleOriginal=$role;subjectOriginal=$subject;
      fileQueryOriginal=$null;returned=$true;exceptionOriginal=$null})
  }
  $policy=@{rootOriginal=$root;returned=$true;terminal=$true;exceptionOriginal=$null;
    basisRowsOriginal=[object[]]@(@{},@{},@{});subjectRowsOriginal=[object[]]@(@{},@{},@{},@{});
    timePolicy=@{validationOriginal=@{};baselineOriginal=@{};currentRevocationOriginal=@{}}}
  $c.nodePolicyOriginal=$policy
  $loader=@{preparationOriginal=$p;identityOriginal=$identity;returned=$true;terminal=$true;
    exceptionOriginal=$null;loaderBookOriginal=@{returned=$true;policyOriginal=$policy};commandOriginal=@{}}
  $c.nodePreparedLoaderOriginal=$loader
  $parse=@{availableKeyChecks=$true;exception=$null}
  if($Slot -ceq 'cli39.node-key-policy.v1'){$c.keyParseOriginal=$parse}else{
    $parse=@{cryptographicResult=$true;exception=$null};$c.signatureParseOriginal=$parse
  }
  $n=@{originalPreparation=$p;consumerOriginal=$c;consumerCompletionResult=$parse;
    primaryPresent=$false;unknown=$false;released=$true;exitCode=0;
    consumerCompletionOriginal=$null;consumerDisposalOriginal=$null;consumerClosureOriginal=$null}
  $p.nativeState=$n
  $birth=@{preparationOriginal=$p;nativeOwnerOriginal=$n;loaderAssociationOriginal=$loader;
    identityOriginal=$identity;returned=$true;terminal=$true;exceptionOriginal=$null}
  $c.nodeProducerBirthOriginal=$birth
  return @{preparation=$p;owner=$n;consumer=$c;root=$root;policy=$policy;parse=$parse;birth=$birth;loader=$loader}
}
foreach($mutation in @('none','foreign-loader','foreign-birth-owner','foreign-parse','original-deadline')){
  $f=New-NodeUsefulSurrogate32
  switch -CaseSensitive ($mutation){
    'foreign-loader'{$f.birth.loaderAssociationOriginal=@{}}
    'foreign-birth-owner'{$f.birth.nativeOwnerOriginal=@{}}
    'foreign-parse'{$f.owner.consumerCompletionResult=@{availableKeyChecks=$true}}
    'original-deadline'{$f.preparation.originalDeadlineTicks=[long]0}
  }
  if($mutation -ceq 'none'){
    $result=Complete-Cli39NodeUsefulResult32 $f.preparation $f.owner
    Assert-Case ($result.usefulReturned -and $result.terminal -and
      [object]::ReferenceEquals($result,$f.owner.usefulConsumerResultOriginal)) 'conditional Node key result same graph'
    Assert-Reject {Complete-Cli39NodeUsefulResult32 $f.preparation $f.owner} 'Node result constructor once only'
  }else{
    Assert-Reject {Complete-Cli39NodeUsefulResult32 $f.preparation $f.owner} ('Node result '+$mutation)
    Assert-Case (-not $f.consumer.nodeUsefulResultOriginal.usefulReturned -and
      $f.consumer.nodeUsefulResultOriginal.terminal -and
      $null -ne $f.consumer.nodeUsefulResultOriginal.exceptionOriginal) 'Node final failure retained'
  }
}
foreach($closurePending in @($false,$true)){
  $key=New-NodeUsefulSurrogate32
  $keyUseful=Complete-Cli39NodeUsefulResult32 $key.preparation $key.owner
  $signature=New-NodeUsefulSurrogate32 'cli39.node-checksum-signature.v1'
  $signature.policy=$key.policy;$signature.root=$key.root
  $signature.consumer.rootCaptureOriginal=$key.root;$signature.consumer.nodePolicyOriginal=$key.policy
  $signature.loader.loaderBookOriginal.policyOriginal=$key.policy
  $relation=@{usefulOriginal=$keyUseful;consumerOriginal=$key.consumer;policyOriginal=$key.policy;
    sequenceOriginal=@{keyringComparedBytes=1L}}
  $signature.consumer.keyPolicyResultOriginal=$relation
  $signature.parse.checkedKeyAssociationOriginal=@{relationOriginal=$relation;returned=$true}
  $signature.consumer.disposalOriginal.providerPending=$closurePending
  if($closurePending){
    Assert-Reject {Complete-Cli39NodeUsefulResult32 $signature.preparation $signature.owner} 'signature failure after completed key before physical closure'
    Assert-Case ($keyUseful.usefulReturned -and -not $signature.consumer.nodeUsefulResultOriginal.usefulReturned -and
      [object]::ReferenceEquals($signature.consumer.keyPolicyResultOriginal.usefulOriginal,$keyUseful)) 'original key result retained across signature failure'
  }else{
    $result=Complete-Cli39NodeUsefulResult32 $signature.preparation $signature.owner
    Assert-Case ($result.usefulReturned -and [object]::ReferenceEquals($result.keyResultOriginal,$relation)) 'conditional signature same key useful result'
  }
}
. (Join-Path $repo 'scripts/prerequisite-node-projection32.ps1')
foreach($mutation in @('none','missing-row','duplicate-row','foreign-selector','child-identity','child-environment')){
  $x=New-NodeSurrogate;$x.root.actionOriginal='key-policy';$x.foundation.action='key-policy'
  $x.root.rootRoleOriginal='qualification-coordinator';$x.root.namespaceBookOriginal=@{disposalOriginal=$null}
  $consumer=$x.root.consumerShapeOriginal.prototypeOriginal
  $consumer.rootCaptureOriginal=$x.root
  $consumer.slot='cli39.node-key-policy.v1';$consumer.paths=@{home='D:/surrogate/home'}
  $consumer.workingDirectoryOriginal='D:/surrogate/cwd'
  $consumer.environmentOriginal=[Collections.Generic.Dictionary[string,string]]::new([StringComparer]::OrdinalIgnoreCase)
  $consumer.environmentOriginal.Add('PATH','D:/surrogate/bin')
  $nativeIdentity=[byte[]]::new(24);$queryBuffers=[object[]]@($null,$null,$null,$nativeIdentity)
  $member=@{domainOriginal='startupClosure';ordinalOriginal=0;bytesExpected=1L;sha256Expected=('a'*64);
    subjectOriginal=@{path=$consumer.subjectPins.executable.path;eof=$true;hashReturned=$true;used=1L};
    fileQueryOriginal=@{buffersOriginal=$queryBuffers}}
  $namespaceFoundation=[object]::new();$precharge=@{}
  $leaf=@{buffersOriginal=[object[]]@($null,[byte[]]::new(24));closePhysicalReturned=$false}
  $named=@{returned=$true;ownerOriginal=@{pathOriginal=$member.subjectOriginal.path};leafOriginal=$leaf}
  $held=@{returned=$true;memberOriginal=$member}
  $j=@{returned=$true;constructionTerminal=$true;exceptionOriginal=$null;prechargeOriginal=$precharge;
    startupOriginal=@{StartupLifetimeOriginal=$x.lifetime;StartupReservationOriginal=@{cliFoundationPrechargeOriginal=$precharge}};
    heldDomainsOriginal=[object[]]@(@{rowsOriginal=[object[]]@($held)});
    namespaceOriginals=@{foundationOriginal=$namespaceFoundation;rowsOriginal=[object[]]@($named)}}
  $x.root.foundationCorrespondenceOriginal=$j
  $tool=[pscustomobject]@{kind='cli39.qualified-node-tool-loader.v1';role='gpg';provenanceOriginal=[object]::new();
    sourceOriginal=[object]::new();loaderSemanticsOriginal=[object]::new();lifetimeOriginal=$x.lifetime;
    namespaceOriginal=$namespaceFoundation;slot=$consumer.slot;workingDirectory=$consumer.workingDirectoryOriginal;
    home=$consumer.paths.home;executableDomain='startupClosure';executableOrdinal=[int]0;
    dependenciesOriginal=[object[]]@([pscustomobject]@{domain='startupClosure';ordinal=[int]0;
      path=$member.subjectOriginal.path;bytes=[long]1;sha256=('a'*64);fileIdentityOriginal=$nativeIdentity});
    environmentOriginal=[object[]]@([pscustomobject]@{name='PATH';value='D:/surrogate/bin'})}
  $x.foundation.toolLoaderOriginals=[object[]]@($tool)
  $policy=Join-Cli39NodeVerificationPolicy32 $x.p $x.foundation $x.lifetime
  $outer=Join-Cli39OuterNodeLoader32 $x.p $policy $policy.toolLoaderFoundationsOriginal
  $book=@{hostOwnerOriginal=$x.p;rootOriginal=$x.root;projectionOriginal=@{};projectionPreflightCharge=1D;
    profileOriginal=@{cliDerivedRootReservationBytes=[long]::MaxValue}}
  $measurement=Measure-Cli39NodePolicyProjection32 $book
  $projection=New-Cli39NodePolicyProjection32 $book $measurement
  Assert-Case ($outer.returned -and $projection.rowsOriginal.Count -eq $measurement.rows) 'actual loader constructor and serializer geometry'
  $static=@{nodePolicyOriginal=[Collections.Generic.List[object]]::new()}
  foreach($row in $projection.rowsOriginal){$static.nodePolicyOriginal.Add([string[]]$row.Clone())}
  if($mutation -ceq 'missing-row'){$static.nodePolicyOriginal.RemoveAt($static.nodePolicyOriginal.Count-1)}
  if($mutation -ceq 'duplicate-row'){$static.nodePolicyOriginal.Add($static.nodePolicyOriginal[0])}
  if($mutation -ceq 'foreign-selector'){
    foreach($row in $static.nodePolicyOriginal){if($row[0] -ceq 'loader' -and $row[2] -ceq 'executableOrdinal'){$row[3]='999'}}
  }
  $memberRows=[Collections.Generic.List[object]]::new()
  foreach($role in @('executable','keyring','signature','listing')){
    $pin=$consumer.subjectPins[$role]
    $m=if($role -ceq 'executable'){$member}else{@{domainOriginal='sourceClosure';ordinalOriginal=$memberRows.Count;
      bytesExpected=$pin.bytes;sha256Expected=$pin.sha256;subjectOriginal=@{path=$pin.path;eof=$true;hashReturned=$true;used=$pin.bytes}}}
    $memberRows.Add(@{memberOriginal=$m;returned=$true})
  }
  $child=@{actionOriginal='key-policy';rootRoleOriginal='operation-host';sha256Expected=('b'*64);
    staticFoundationOriginal=$static;childDerivedTransportOriginal=@{observationsReturned=$true};
    childStaticCorrespondenceOriginal=@{bindingsOriginal=@{returned=$true};memberRowsOriginal=$memberRows;
      namespaceRowsOriginal=[object[]]@($named);qualificationRootChainOriginal=@{}};
    admittedConsumerOriginal=$consumer;consumerShapeOriginal=@{prototypeOriginal=$consumer};
    namespaceBookOriginal=@{disposalOriginal=$null};admittedInputOriginal=@{parentReservationOriginal=@{
      inputRoot=('b'*64);budget=@{cliNodeLoaderBookChargeBytes=1L;cliNodeLoaderPerMemberChargeBytes=1L;
        cliNodeLoaderPerEnvironmentChargeBytes=1L;cliNodeLoaderPerByteChargeBytes=1L;cliNodeLoaderReservationBytes=[long]::MaxValue}}}}
  $x.p.inputRead.value=$child
  $consumer.rootCaptureOriginal=$child
  if($mutation -cin @('missing-row','duplicate-row')){
    Assert-Reject {Read-Cli39NodePolicyOperands32 $x.p $static} ('owning loader parser '+$mutation)
    Assert-Case ($static.nodePolicyValidationOriginal.terminal -and -not $static.nodePolicyValidationOriginal.returned) 'loader parser failure retained'
    continue
  }
  $null=Read-Cli39NodePolicyOperands32 $x.p $static
  $null=Join-Cli39ChildNodePolicy32 $x.p
  if($mutation -ceq 'child-identity'){$nativeIdentity[0]=1}
  if($mutation -ceq 'child-environment'){$consumer.environmentOriginal['PATH']='D:/surrogate/changed'}
  if($mutation -ceq 'none'){
    $joined=Join-Cli39ChildNodeLoader32 $x.p
    Assert-Case ($joined.returned -and $joined.rowsOriginal[0].membersOriginal[0].identityCompared -eq 24) 'actual loader static child counterpart'
  }else{
    Assert-Reject {Join-Cli39ChildNodeLoader32 $x.p} ('owning child loader '+$mutation)
    Assert-Case ($child.nodeLoaderOriginal.terminal -and -not $child.nodeLoaderOriginal.returned -and
      $null -ne $child.nodeLoaderOriginal.exceptionOriginal) 'child loader failure retained'
  }
}
Write-Output 'publisher-source-cases-passed-surrogate-only'
