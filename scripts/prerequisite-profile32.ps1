# Fixed decoded profile shape only. Neither numbers nor arithmetic establish
# measured fit, external startup reservation, runtime authenticity or authority.
function Read-Cli39PrerequisiteProfileStructure32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,
    [Parameter(Mandatory)][string]$Role,
    [Parameter(Mandatory)]$ElementOriginal,
    [Parameter(Mandatory)][long]$OutputLimitBytes,
    [Parameter(Mandatory)][ValidateSet('materialize','compare','keyring','key-policy','checksum-signature','publisher')][string]$Action)
  $p=$HostOwnerOriginal;$capture=$p.inputRead.value
  $roles=if($Action -ceq 'checksum-signature'){@('callerBudget','hostBudget','consumerBudget','keyPolicyBudget')}else{@('callerBudget','hostBudget','consumerBudget')}
  if($Role -cnotin $roles -or
      $OutputLimitBytes -le 0 -or $null -eq $capture -or
      -not [object]::ReferenceEquals($p.receiver.frames[$p.key],$p) -or
      $capture.profileOriginals.Count -ge $roles.Count){throw 'SAME complete fixed role profile required.'}
  foreach($prior in $capture.profileOriginals){if($prior.role -ceq $Role){throw 'Profile role already enrolled.'}}
  $o=@{role=$Role;actionOriginal=$Action;elementOriginal=$ElementOriginal;outputLimitBytes=$OutputLimitBytes;
    propertiesOriginal=$null;fieldsOriginal=$null;valuesOriginal=$null;
    returned=$false;exceptionOriginal=$null;genuineQualified=$false;
    startupReservationOriginal=$null;measurementOriginal=$null;aggregateAssociationOriginal=$null}
  $capture.profileOriginals.Add($o)
  try{
    $keys=[string[]]@('domain','runtimeB0Bytes','runtimeB2Bytes','operationBytes',
      'receiverChargeBytes','attributeListBytes','domainLimitBytes','observationFrameBytes',
      'observationReservationBytes','sinkWorkerStackBytes','sinkWorkerReservationBytes',
      'managedOperationReservationBytes','nativeAllocationOverheadReservationBytes',
      'rawWriteRecordChargeBytes','rawWriteRosterBaseChargeBytes','rawTranscriptChargeBytes')
    if($Role -cin @('callerBudget','hostBudget')){
      $keys+=[string[]]@('cliBirthReservationBytes','cliBirthBookChargeBytes',
        'cliBirthPerWorkerChargeBytes','cliBirthPerByteChargeBytes','cliBirthWorkerStackBytes',
        'cliDerivedRootReservationBytes','cliDerivedRootBookChargeBytes','cliDerivedRootPerByteChargeBytes',
        'cliDerivedRootPerWorkerChargeBytes','cliDerivedRootWorkerStackBytes',
        'cliParentReservationBytes','cliParentBookChargeBytes','cliParentPerByteChargeBytes',
        'cliParentPerWorkerChargeBytes','cliParentWorkerStackBytes',
        'cliLoadedReservationBytes','cliLoadedBookChargeBytes','cliLoadedPerByteChargeBytes',
        'cliLoadedPerWorkerChargeBytes','cliLoadedWorkerStackBytes')
    }
    if(($Role -ceq 'consumerBudget' -and $Action -cin @('key-policy','checksum-signature')) -or
        ($Role -ceq 'keyPolicyBudget' -and $Action -ceq 'checksum-signature')){
      $keys+=[string[]]@('nodePreparedIdentityReservationBytes','nodePreparedIdentityBookChargeBytes')
    }
    if($Role -cin @('callerBudget','hostBudget') -and $Action -cin @('key-policy','checksum-signature')){
      $keys+=[string[]]@('cliNodeLoaderReservationBytes','cliNodeLoaderBookChargeBytes',
        'cliNodeLoaderPerMemberChargeBytes','cliNodeLoaderPerEnvironmentChargeBytes','cliNodeLoaderPerByteChargeBytes')
    }
    if($Role -ceq 'consumerBudget' -and $Action -ceq 'publisher'){
      $keys+=[string[]]@('publisherOidCaptureCapacityBytes','publisherOidCaptureFixedChargeBytes',
        'publisherOidCapturePerByteChargeBytes','publisherOidCaptureReservationBytes','publisherOidCaptureBookChargeBytes',
        'publisherChainCaptureReservationBytes','publisherChainCaptureBookChargeBytes',
        'publisherChainCaptureFixedChargeBytes','publisherChainCapturePerByteChargeBytes',
        'publisherCustodyReservationBytes','publisherCustodyBookChargeBytes','publisherCustodyPerSignerChargeBytes')
    }
    if($Role -ceq 'consumerBudget' -and $Action -cin @('materialize','compare','keyring')){
      $keys+=[string[]]@('pythonReportCaptureReservationBytes','pythonReportCaptureBookChargeBytes',
        'pythonReportCapturePerReportChargeBytes','pythonReportCapturePerByteChargeBytes')
    }
    if($Role -ceq 'consumerBudget' -and $Action -ceq 'materialize'){
      $keys+=[string[]]@('pythonTreeCaptureReservationBytes','pythonTreeCaptureBookChargeBytes',
        'pythonTreeCapturePerDirectoryChargeBytes','pythonTreeCapturePerEntryChargeBytes',
        'pythonTreeCapturePerMemberChargeBytes','pythonTreeCapturePerByteChargeBytes')
    }
    $o.expectedKeysOriginal=$keys
    $o.propertiesOriginal=[Collections.Generic.List[object]]::new($keys.Count+1)
    $o.fieldsOriginal=[Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal)
    $o.valuesOriginal=@{}
    if($ElementOriginal.ValueKind -ne [System.Text.Json.JsonValueKind]::Object){throw 'Fixed profile must be an object.'}
    foreach($property in $ElementOriginal.EnumerateObject()){
      Observe-OwnedPrerequisiteDeadline $p
      $o.propertiesOriginal.Add($property)
      if($property.Name -cnotin $keys -or $o.fieldsOriginal.ContainsKey($property.Name)){
        throw 'Unknown or decoded duplicate fixed profile field.'
      }
      $o.fieldsOriginal.Add($property.Name,$property.Value)
    }
    if($o.fieldsOriginal.Count -ne $keys.Count){throw 'Incomplete fixed profile field set.'}
    $expectedDomain=switch -CaseSensitive ($Role){
      'callerBudget' {'cli39-prerequisite-caller-qualification'}
      'hostBudget' {'cli39-prerequisite-host-qualification'}
      'consumerBudget' {'cli39-prerequisite-consumer-qualification'}
      'keyPolicyBudget' {'cli39-prerequisite-key-policy-qualification'}
    }
    $domain=$o.fieldsOriginal['domain']
    if($domain.ValueKind -ne [System.Text.Json.JsonValueKind]::String -or
        $domain.GetString() -cne $expectedDomain){throw 'Exact dedicated CLI qualification domain required.'}
    $o.valuesOriginal.domain=$expectedDomain
    foreach($key in $keys){
      if($key -ceq 'domain'){continue}
      $field=$o.fieldsOriginal[$key];$value=0L
      if($field.ValueKind -ne [System.Text.Json.JsonValueKind]::Number -or
          $field.GetRawText() -cnotmatch '^[1-9][0-9]*$' -or
          -not $field.TryGetInt64([ref]$value) -or $value -le 0){
        throw 'Canonical positive Int64 profile field required.'
      }
      $o.valuesOriginal.Add($key,$value)
    }
    $v=$o.valuesOriginal
    [decimal]$roleTotal=[decimal]$v.runtimeB0Bytes+[decimal]$v.runtimeB2Bytes+
      [decimal]$v.operationBytes+[decimal]$v.receiverChargeBytes
    [decimal]$fixedReservations=[decimal]$v.observationReservationBytes+
      [decimal]$v.sinkWorkerReservationBytes+[decimal]$v.managedOperationReservationBytes+
      [decimal]$v.nativeAllocationOverheadReservationBytes
    if($roleTotal -gt $v.domainLimitBytes -or $fixedReservations -ge $v.operationBytes -or
        $v.sinkWorkerStackBytes -gt [int]::MaxValue -or
        [decimal]$v.sinkWorkerReservationBytes -lt 2*[decimal]$v.sinkWorkerStackBytes -or
        $v.sinkWorkerReservationBytes -gt $v.operationBytes){
      throw 'Decoded fixed reservations cannot contain the source-owned operation.'
    }
    # Decimal multiplication either returns an exact integer or throws on a
    # vastly over-budget product. No floating conversion can round a fit in.
    [decimal]$rawCharge=[decimal]$OutputLimitBytes*[decimal]$v.rawWriteRecordChargeBytes+
      [decimal]$v.rawWriteRosterBaseChargeBytes+[decimal]$v.rawTranscriptChargeBytes
    if($v.rawTranscriptChargeBytes -lt $OutputLimitBytes -or
        $rawCharge -ge $v.managedOperationReservationBytes){
      throw 'Decoded raw write roster/transcript leaves no managed operation capacity.'
    }
    if(($Role -ceq 'consumerBudget' -and $Action -cin @('key-policy','checksum-signature')) -or
        ($Role -ceq 'keyPolicyBudget' -and $Action -ceq 'checksum-signature')){
      if($v.nodePreparedIdentityBookChargeBytes -ge $v.nodePreparedIdentityReservationBytes -or
          [decimal]$v.nodePreparedIdentityReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Fixed Node prepared identity/raw subdivisions must fit disjointly in SAME managed domain.'
      }
      # Full fixed four-worker graph, explicit default selected stacks, backing,
      # alias-list/partial/error and result retention require genuine qualification.
      $o.nodePreparedIdentityState='DECODED_DISJOINT_GEOMETRY_NOT_GENUINE_PROFILE'
    }
    if($Role -cin @('callerBudget','hostBudget')){
      if($v.cliBirthWorkerStackBytes -gt [int]::MaxValue -or
          $v.cliBirthPerWorkerChargeBytes -le $v.cliBirthWorkerStackBytes -or
          [decimal]$v.cliBirthBookChargeBytes+6*[decimal]$v.cliBirthPerWorkerChargeBytes -ge $v.cliBirthReservationBytes -or
          [decimal]$v.cliBirthReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded fixed six-worker birth subdivision leaves no backing capacity in SAME managed reservation.'
      }
      if($v.cliDerivedRootWorkerStackBytes -gt [int]::MaxValue -or
          $v.cliDerivedRootPerWorkerChargeBytes -le $v.cliDerivedRootWorkerStackBytes -or
          [decimal]$v.cliDerivedRootBookChargeBytes+3*[decimal]$v.cliDerivedRootPerWorkerChargeBytes -ge $v.cliDerivedRootReservationBytes -or
          [decimal]$v.cliDerivedRootReservationBytes+[decimal]$v.cliBirthReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded derived-root/birth/raw subdivisions cannot fit disjointly in SAME managed reservation.'
      }
      $o.cliDerivedRootReservationState='DECODED_GEOMETRY_NOT_GENUINE_PROFILE'
      $o.cliBirthReservationState='DECODED_GEOMETRY_NOT_GENUINE_PROFILE'
      if($v.cliParentWorkerStackBytes -gt [int]::MaxValue -or
          $v.cliParentPerWorkerChargeBytes -le $v.cliParentWorkerStackBytes -or
          [decimal]$v.cliParentBookChargeBytes+10*[decimal]$v.cliParentPerWorkerChargeBytes -ge $v.cliParentReservationBytes -or
          [decimal]$v.cliParentReservationBytes+[decimal]$v.cliDerivedRootReservationBytes+
            [decimal]$v.cliBirthReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded ten-worker parent/birth/derived/raw subdivisions cannot fit disjointly in SAME managed reservation.'
      }
      $o.cliParentReservationState='DECODED_GEOMETRY_NOT_GENUINE_PROFILE'
      if($v.cliLoadedWorkerStackBytes -gt [int]::MaxValue -or
          $v.cliLoadedPerWorkerChargeBytes -le $v.cliLoadedWorkerStackBytes -or
          [decimal]$v.cliLoadedBookChargeBytes+5*[decimal]$v.cliLoadedPerWorkerChargeBytes -ge $v.cliLoadedReservationBytes -or
          [decimal]$v.cliLoadedReservationBytes+[decimal]$v.cliParentReservationBytes+
            [decimal]$v.cliDerivedRootReservationBytes+[decimal]$v.cliBirthReservationBytes+
            $rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded loaded/parent/derived/birth/raw subdivisions cannot fit disjointly in SAME managed reservation.'
      }
      # Four export/delegate workers plus system is only the minimum. Module path and
      # region workers/backing are charged from observed geometry before birth.
      $o.cliLoadedReservationState='DECODED_GEOMETRY_NOT_GENUINE_PROFILE'
      if($Action -cin @('key-policy','checksum-signature')){
        if($v.cliNodeLoaderBookChargeBytes -ge $v.cliNodeLoaderReservationBytes -or
            [decimal]$v.cliNodeLoaderReservationBytes+[decimal]$v.cliLoadedReservationBytes+
            [decimal]$v.cliParentReservationBytes+[decimal]$v.cliDerivedRootReservationBytes+
            [decimal]$v.cliBirthReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
          throw 'Fixed tool-loader graph must fit disjointly in SAME managed reservation.'
        }
        $o.cliNodeLoaderReservationState='DECODED_DISJOINT_GEOMETRY_NOT_GENUINE_PROFILE'
      }
    }
    $o.roleTotalBytes=$roleTotal;$o.fixedReservationBytes=$fixedReservations;$o.rawChargeBytes=$rawCharge
    if($Role -ceq 'consumerBudget' -and $Action -ceq 'publisher'){
      [decimal]$oidCharge=[decimal]$v.publisherOidCaptureFixedChargeBytes+
        [decimal]$v.publisherOidCaptureCapacityBytes*[decimal]$v.publisherOidCapturePerByteChargeBytes
      if($v.publisherOidCaptureCapacityBytes -gt [int]::MaxValue -or
          $oidCharge+[decimal]$v.publisherOidCaptureBookChargeBytes -gt $v.publisherOidCaptureReservationBytes -or
          [decimal]$v.publisherOidCaptureReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded OID subreservation cannot fit disjointly inside SAME managed operation reservation.'
      }
      if([decimal]$v.publisherChainCaptureBookChargeBytes+[decimal]$v.publisherChainCaptureFixedChargeBytes -gt $v.publisherChainCaptureReservationBytes -or
          [decimal]$v.publisherChainCaptureReservationBytes+[decimal]$v.publisherOidCaptureReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded chain/OID/raw reservations cannot fit disjointly inside SAME managed reservation.'
      }
      if([decimal]$v.publisherCustodyBookChargeBytes+[decimal]$v.publisherCustodyPerSignerChargeBytes -gt $v.publisherCustodyReservationBytes -or
          [decimal]$v.publisherCustodyReservationBytes+[decimal]$v.publisherChainCaptureReservationBytes+
            [decimal]$v.publisherOidCaptureReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded custody/chain/OID/raw subdivisions cannot fit disjointly inside SAME managed reservation.'
      }
      $o.publisherCustodyReservationState='DECODED_GEOMETRY_NOT_GENUINE_PROFILE'
      # Book charge includes retained policy/time/anchor/evaluation/revocation/
        # timestamp constructor journals, source-owned fixed lookup temporaries and
      # failure graphs. Per-signer charge backs each actual expected-certificate
      # comparison, separately charged primary/counter time, anchor-selection,
        # evaluation, per-element revocation and primary/attribute/counter timestamp
      # record, and every actual accepted-anchor candidate comparison record.
      # Their actual count sum is checked before allocation; minima prove no fit.
      $o.publisherChainReservationState='DECODED_DISJOINT_GEOMETRY_NOT_GENUINE_PROFILE'
      $o.publisherOidChargeBytes=$oidCharge
      $o.publisherOidReservationState='DECODED_DISJOINT_GEOMETRY_NOT_GENUINE_PROFILE'
    }
    if($Role -ceq 'consumerBudget' -and $Action -cin @('materialize','compare','keyring')){
      if([decimal]$v.pythonReportCaptureBookChargeBytes+[decimal]$v.pythonReportCapturePerReportChargeBytes -gt $v.pythonReportCaptureReservationBytes -or
          [decimal]$v.pythonReportCaptureReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded Python report subdivision cannot fit disjointly inside SAME managed reservation.'
      }
      $o.pythonReportReservationState='DECODED_GEOMETRY_NOT_GENUINE_PROFILE'
    }
    if($Role -ceq 'consumerBudget' -and $Action -ceq 'materialize'){
      if([decimal]$v.pythonTreeCaptureBookChargeBytes+[decimal]$v.pythonTreeCapturePerDirectoryChargeBytes+
          [decimal]$v.pythonTreeCapturePerEntryChargeBytes+[decimal]$v.pythonTreeCapturePerMemberChargeBytes -gt $v.pythonTreeCaptureReservationBytes -or
          [decimal]$v.pythonTreeCaptureReservationBytes+[decimal]$v.pythonReportCaptureReservationBytes+$rawCharge -ge $v.managedOperationReservationBytes){
        throw 'Decoded tree/report/raw subdivisions cannot fit disjointly inside SAME managed reservation.'
      }
      $o.pythonTreeReservationState='DECODED_GEOMETRY_NOT_GENUINE_PROFILE'
    }
    $o.returned=$true
    # The actual external reservation, whole overlap, default CLR worker and
    # loader/runtime/allocator evidence remain distinct unresolved originals.
    return $o
  }catch{$o.exceptionOriginal=$_;throw}
}


# Full fixed semantic-primary lookup/record/failure graph and transient five-role lookup roster share the existing perSigner charge; complete rounded backing/stack/overlap remains genuine-input qualification, never a new allowance.
