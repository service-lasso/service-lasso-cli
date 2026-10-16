# Actual local full-path and region metadata. Neither MEM_IMAGE nor this raw
# collector authenticates installed bytes or the selected OS-loader foundation.
function Observe-Cli39Loaded32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal)
  $p=$HostOwnerOriginal;$b=New-Cli39LoadedObservationBook32 $p;$root=$p.inputRead.value
  try{
    $bindings=@{path=$null;system=$null};$b.bindingsOriginal=$bindings
    foreach($pair in @(@('GetModuleFileNameW','path'),@('GetSystemInfo','system'))){
      $export=New-Cli39LoadedWorker32 $b ($pair[0]+':export') 0
      $null=Invoke-Cli39LoadedWorker32 $export
      $delegate=New-Cli39LoadedWorker32 $b ($pair[0]+':delegate') 0 -BindingOriginal $export
      $null=Invoke-Cli39LoadedWorker32 $delegate;$bindings[$pair[1]]=$delegate
    }
    $system=New-Cli39LoadedWorker32 $b 'system' 48 -BindingOriginal $bindings.system
    $b.systemOriginal=$system;$null=Invoke-Cli39LoadedWorker32 $system
    $b.systemDecodedOriginal=@{workerOriginal=$system;architectureOriginal=0;pageBytesOriginal=0;
      allocationGranularityOriginal=0;returned=$false;exceptionOriginal=$null}
    $sd=$b.systemDecodedOriginal
    try{
      $sd.architectureOriginal=[BitConverter]::ToUInt16($system.backingOriginal,0)
      $sd.pageBytesOriginal=[BitConverter]::ToUInt32($system.backingOriginal,4)
      $sd.allocationGranularityOriginal=[BitConverter]::ToUInt32($system.backingOriginal,40)
      if($sd.architectureOriginal -ne 9 -or $sd.pageBytesOriginal -eq 0 -or
          ($sd.pageBytesOriginal -band ($sd.pageBytesOriginal-1)) -ne 0 -or
          $sd.allocationGranularityOriginal -lt $sd.pageBytesOriginal -or
          $sd.allocationGranularityOriginal % $sd.pageBytesOriginal -ne 0){throw 'Actual x64 system/page geometry inconsistent.'}
      $b.pageBytesOriginal=$sd.pageBytesOriginal;$sd.returned=$true
    }catch{$sd.exceptionOriginal=$_;throw}
    # The entire pinned installed loaded roster supplies path capacity, including
    # a rejection witness. Toolhelp's truncated path is never the selector.
    $b.loadedMembersOriginal=[Collections.Generic.List[object]]::new()
    $b.maximumPathOriginal=$null;$b.maximumPathCharacters=0
    foreach($member in $root.heldMemberBookOriginal.recordsOriginal){
      if($member.domainOriginal -cne 'loadedClosure'){continue}
      $b.loadedMembersOriginal.Add($member)
      if($member.subjectOriginal.path.Length -gt $b.maximumPathCharacters){
        $b.maximumPathCharacters=$member.subjectOriginal.path.Length;$b.maximumPathOriginal=$member.subjectOriginal.path
      }
    }
    if($b.loadedMembersOriginal.Count -ne $b.snapshotOriginal.expectedRows -or $null -eq $b.maximumPathOriginal){throw 'Actual complete held loaded roster differs.'}
    $b.pathGeometryOriginal=Get-Cli39LoadedPathGeometry32 $b.maximumPathOriginal
    foreach($snapshotRow in $b.snapshotOriginal.rowsOriginal){
      if(-not $snapshotRow.moduleObserved){continue}
      Observe-OwnedPrerequisiteDeadline $p
      if($p.deadlineReported){throw 'Original deadline forbids next loaded module.'}
      $decoded=$snapshotRow.decodedOriginal
      if(-not $decoded.returned -or $decoded.baseOriginal -eq [IntPtr]::Zero -or
          $decoded.moduleHandleOriginal -eq [IntPtr]::Zero){throw 'Actual completed module base/handle required.'}
      $geometry=Get-Cli39LoadedRegionGeometry32 $decoded.baseOriginal.ToInt64() $decoded.imageBytesOriginal $b.pageBytesOriginal
      $v=$b.profileOriginal.valuesOriginal
      [decimal]$charge=(1D+[decimal]$geometry.maximumRegionRows)*[decimal]$v.cliLoadedPerWorkerChargeBytes+
        ([decimal]$b.pathGeometryOriginal.backingBytes+48D*[decimal]$geometry.maximumRegionRows)*[decimal]$v.cliLoadedPerByteChargeBytes
      if($b.chargeBytes+$charge -ge $v.cliLoadedReservationBytes){throw 'Complete actual module path/region roster exceeds SAME loaded subdivision.'}
      # Reserve the complete maximum roster before row/list/thread creation.
      # Per-worker genuine charge includes its retained module/region record,
      # list element/backing/context/pin/exception/stack and allocator overlap.
      $b.chargeBytes+=$charge;$b.moduleConstructionUnknown=$true
      $module=@{bookOriginal=$b;snapshotRowOriginal=$snapshotRow;decodedOriginal=$decoded;
        geometryOriginal=$geometry;pathGeometryOriginal=$b.pathGeometryOriginal;
        chargeBytes=$charge;remainingChargeBytes=$charge;pathEnrollmentAttempted=$false;regionWorkersEnrolled=0;
        pathWorkerOriginal=$null;memberOriginal=$null;memberQueryOriginal=$null;pathComparedBytes=0;
        regionRowsOriginal=$null;cursorOriginal=$geometry.moduleBaseOriginal;
        processOriginal=$root.rootProcessOriginal;processCreationOriginal=$root.rootProcessOriginal.timesOriginal;
        constructorTerminal=$false;constructorFailure=$null;returned=$false;exceptionOriginal=$null}
      $b.pendingModuleOriginal=$module;$b.modulesOriginal.Add($module)
      try{$module.regionRowsOriginal=[Collections.Generic.List[object]]::new($geometry.maximumRegionRows)}
      catch{$module.constructorFailure=$_;throw}
      finally{$module.constructorTerminal=$true;$b.moduleConstructionUnknown=$false}
      try{
        $path=New-Cli39LoadedWorker32 $b 'path' $b.pathGeometryOriginal.backingBytes `
          -Argument $decoded.moduleHandleOriginal -CharacterCapacity $b.pathGeometryOriginal.capacityCharacters `
          -BindingOriginal $bindings.path -ModuleOriginal $module
        $module.pathWorkerOriginal=$path;$null=Invoke-Cli39LoadedWorker32 $path
        [uint64]$characters=$path.resultsOriginal[0]
        if($characters -eq 0 -or $characters -ge $b.pathGeometryOriginal.capacityCharacters){throw 'Actual loaded path failed/truncated; raw spelling retained.'}
        if($path.backingOriginal[2*[int]$characters] -ne 0 -or $path.backingOriginal[2*[int]$characters+1] -ne 0){throw 'Actual loaded full path terminator missing.'}
        foreach($member in $b.loadedMembersOriginal){
          if($member.subjectOriginal.path.Length -ne $characters){continue}
          $equal=$true
          for($i=0;$i -lt $member.subjectOriginal.path.Length;$i++){
            Observe-OwnedPrerequisiteDeadline $p
            if($p.deadlineReported){throw 'Original deadline ended exact loaded path comparison.'}
            [int]$code=$member.subjectOriginal.path[$i]
            if($path.backingOriginal[2*$i] -ne ($code -band 255) -or
                $path.backingOriginal[2*$i+1] -ne ($code -shr 8)){$equal=$false;break}
          }
          if($equal){if($null -ne $module.memberOriginal){throw 'Ambiguous duplicate installed loaded path.'};$module.memberOriginal=$member}
        }
        if($null -eq $module.memberOriginal){throw 'Actual loaded spelling has no exact held installed member.'}
        foreach($prior in $b.modulesOriginal){if(-not [object]::ReferenceEquals($prior,$module) -and
            [object]::ReferenceEquals($prior.memberOriginal,$module.memberOriginal)){throw 'Duplicate actual loaded member retained.'}}
        $module.memberQueryOriginal=$module.memberOriginal.fileQueryOriginal
        if(-not $module.memberQueryOriginal.rawReturned -or -not $module.memberQueryOriginal.decodeOriginal.returned -or
            -not $module.memberOriginal.subjectOriginal.hashReturned -or -not $module.memberOriginal.subjectOriginal.eof){throw 'Actual complete held installed member/query required.'}
        $module.pathComparedBytes=2*[long]$characters
        while($module.cursorOriginal -lt $geometry.imageEnd){
          Observe-OwnedPrerequisiteDeadline $p
          if($p.deadlineReported){throw 'Original deadline forbids next loaded region.'}
          if($module.regionRowsOriginal.Count -ge $geometry.maximumRegionRows){throw 'Actual region progress exceeded precharged complete roster.'}
          $worker=New-Cli39LoadedWorker32 $b 'region' 48 -Argument ([IntPtr]::new($module.cursorOriginal)) -ModuleOriginal $module
          $row=@{moduleOriginal=$module;workerOriginal=$worker;cursorOriginal=$module.cursorOriginal;
            baseOriginal=0L;allocationBaseOriginal=0L;regionBytesOriginal=0UL;stateOriginal=0U;typeOriginal=0U;
            regionEndOriginal=0L;nextOriginal=0L;returned=$false;exceptionOriginal=$null}
          $module.regionRowsOriginal.Add($row)
          try{
            $null=Invoke-Cli39LoadedWorker32 $worker
            if($worker.resultsOriginal[0] -ne 48){throw 'Actual full x64 MBI48 result required.'}
            $bytes=$worker.backingOriginal
            $row.baseOriginal=[BitConverter]::ToInt64($bytes,0);$row.allocationBaseOriginal=[BitConverter]::ToInt64($bytes,8)
            $row.regionBytesOriginal=[BitConverter]::ToUInt64($bytes,24);$row.stateOriginal=[BitConverter]::ToUInt32($bytes,32)
            $row.typeOriginal=[BitConverter]::ToUInt32($bytes,40)
            [decimal]$end=[decimal]$row.baseOriginal+[decimal]$row.regionBytesOriginal
            if($row.baseOriginal -lt 0 -or $row.baseOriginal % [long]$b.pageBytesOriginal -ne 0 -or
                $row.regionBytesOriginal -eq 0 -or $row.regionBytesOriginal % [uint64]$b.pageBytesOriginal -ne 0 -or
                $end -gt [long]::MaxValue -or $row.baseOriginal -gt $module.cursorOriginal -or
                $end -le $module.cursorOriginal -or $row.allocationBaseOriginal -ne $geometry.moduleBaseOriginal -or
                $row.stateOriginal -notin @(0x1000,0x2000) -or $row.typeOriginal -ne 0x1000000){throw 'Actual same-allocation MEM_IMAGE region geometry incomplete.'}
            $row.regionEndOriginal=[long]$end
            $row.nextOriginal=if($end -gt $geometry.imageEnd){$geometry.imageEnd}else{[long]$end}
            $module.cursorOriginal=$row.nextOriginal;$row.returned=$true
          }catch{$row.exceptionOriginal=$_;throw}
        }
        Observe-OwnedPrerequisiteDeadline $p
        if($p.deadlineReported){throw 'Original deadline denies completed loaded module progression.'}
        $module.returned=$true
      }catch{$module.exceptionOriginal=$_;throw}
    }
    if($b.modulesOriginal.Count -ne $b.loadedMembersOriginal.Count -or -not (Test-Cli39LoadedObservationClosure32 $p)){
      throw 'Actual complete loaded roster/physical workers required.'}
    Observe-OwnedPrerequisiteDeadline $p
    if($p.deadlineReported){throw 'Original deadline denies loaded collector completion.'}
    $b.returned=$true;return $b
  }catch{$b.exceptionOriginal=$_;throw}
}
