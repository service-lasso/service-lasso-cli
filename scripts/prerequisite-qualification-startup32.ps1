# Fixed local carrier. Its references are actual input obligations, never trust
# synthesized by this constructor. No JSON/string conversion creates this type.
class Cli39QualificationStartup32 {
  [object]$HostOwnerOriginal
  [object]$StartupReservationOriginal
  [object]$StartupLifetimeOriginal
  [object]$SourceFoundationOriginal
  [object]$ToolFoundationOriginal
  [object]$InstalledFoundationOriginal
  [object]$LoadedFoundationOriginal
  [object]$NamespaceFoundationOriginal
  [object]$PrincipalFoundationOriginal
  [object]$CapabilityFoundationOriginal
  [object]$AllocationFoundationOriginal
  [object]$PolicyFoundationOriginal
  [string]$InputRootPathOriginal
  [long]$InputRootBytesOriginal
  [string]$InputRootSha256Original
  [string]$SharedSourceRootOriginal
  [string]$ActionOriginal
  [long]$DeadlineTicksOriginal
  [object]$ReceiverOriginal
  [string]$HostKeyOriginal
  [object]$DeadlineResponseOriginal
  [bool]$ConstructorTerminal
  [object]$ConstructorFailure
  [object]$CallerOriginal

  Cli39QualificationStartup32([object]$HostOwner,[object]$Reservation,[object]$Lifetime,
      [object]$Source,[object]$Tool,[object]$Installed,[object]$Loaded,[object]$Namespace,
      [object]$Principal,[object]$Capability,[object]$Allocation,[object]$Policy,
      [string]$RootPath,[long]$RootBytes,[string]$RootSha256,[string]$SharedRoot,[string]$Action){
    $this.HostOwnerOriginal=$HostOwner
    if($null -eq $HostOwner -or $HostOwner.ContainsKey('qualificationStartupOriginal')){throw 'Actual dedicated once-only containing host required.'}
    # Publication precedes subsequent carrier field validation; a failed
    # constructor remains the actual containing host's retained original.
    $HostOwner.qualificationStartupOriginal=$this
    try{
      $this.StartupReservationOriginal=$Reservation;$this.StartupLifetimeOriginal=$Lifetime
      $this.SourceFoundationOriginal=$Source;$this.ToolFoundationOriginal=$Tool
      $this.InstalledFoundationOriginal=$Installed;$this.LoadedFoundationOriginal=$Loaded
      $this.NamespaceFoundationOriginal=$Namespace;$this.PrincipalFoundationOriginal=$Principal
      $this.CapabilityFoundationOriginal=$Capability;$this.AllocationFoundationOriginal=$Allocation
      $this.PolicyFoundationOriginal=$Policy
      $this.InputRootPathOriginal=$RootPath;$this.InputRootBytesOriginal=$RootBytes
      $this.InputRootSha256Original=$RootSha256;$this.SharedSourceRootOriginal=$SharedRoot;$this.ActionOriginal=$Action
      $this.ReceiverOriginal=$HostOwner.receiver;$this.HostKeyOriginal=$HostOwner.key
      $this.DeadlineTicksOriginal=$HostOwner.originalDeadlineTicks;$this.DeadlineResponseOriginal=$HostOwner.deadlineResponse
      if($null -eq $Reservation -or $null -eq $Lifetime -or $null -eq $Source -or $null -eq $Tool -or
          $null -eq $Installed -or $null -eq $Loaded -or $null -eq $Namespace -or $null -eq $Principal -or
          $null -eq $Capability -or $null -eq $Allocation -or ($Action -ceq 'publisher' -and $null -eq $Policy)){
        throw 'Actual containing startup/reservation/foundation originals are absent.'
      }
      if($RootBytes -le 0 -or $RootSha256 -cnotmatch '^[0-9a-f]{64}$' -or
          -not [IO.Path]::IsPathFullyQualified($RootPath) -or -not [IO.Path]::IsPathFullyQualified($SharedRoot) -or
          $Action -cnotin @('materialize','compare','keyring','key-policy','checksum-signature','publisher') -or
          $null -eq $this.ReceiverOriginal -or [string]::IsNullOrEmpty($this.HostKeyOriginal) -or
          -not [object]::ReferenceEquals($this.ReceiverOriginal.frames[$this.HostKeyOriginal],$HostOwner) -or
          $this.DeadlineTicksOriginal -le 0 -or $null -eq $this.DeadlineResponseOriginal -or
          $null -eq $HostOwner.inputRead -or $null -ne $HostOwner.consumerOriginal -or $null -ne $HostOwner.inputRead.value){
        throw 'Actual dedicated containing host/receiver/deadline and exact CLI invocation pins required.'
      }
      # No genuine/qualified/admitted flag is set. External entire input review
      # must establish what these actual references represent and their lifetime.
    }catch{$this.ConstructorFailure=$_;throw}finally{$this.ConstructorTerminal=$true}
  }
}

function New-Cli39QualificationStartup32 {
  param([Parameter(Mandatory)]$HostOwnerOriginal,[Parameter(Mandatory)]$StartupReservationOriginal,
    [Parameter(Mandatory)]$StartupLifetimeOriginal,[Parameter(Mandatory)]$SourceFoundationOriginal,
    [Parameter(Mandatory)]$ToolFoundationOriginal,[Parameter(Mandatory)]$InstalledFoundationOriginal,
    [Parameter(Mandatory)]$LoadedFoundationOriginal,[Parameter(Mandatory)]$NamespaceFoundationOriginal,
    [Parameter(Mandatory)]$PrincipalFoundationOriginal,[Parameter(Mandatory)]$CapabilityFoundationOriginal,
    [Parameter(Mandatory)]$AllocationFoundationOriginal,$PolicyFoundationOriginal,
    [Parameter(Mandatory)][string]$InputRootPath,[Parameter(Mandatory)][long]$InputRootBytes,
    [Parameter(Mandatory)][string]$InputRootSha256,[Parameter(Mandatory)][string]$SharedSourceRoot,
    [Parameter(Mandatory)][ValidateSet('materialize','compare','keyring','key-policy','checksum-signature','publisher')][string]$Action)
  $p=$HostOwnerOriginal
  if($p.ContainsKey('qualificationCarrierConstructionOriginal')){throw 'Fixed startup carrier constructor cannot retry.'}
  $construction=@{hostOwnerOriginal=$p;reservationOriginal=$StartupReservationOriginal;
    lifetimeOriginal=$StartupLifetimeOriginal;sourceOriginal=$SourceFoundationOriginal;toolOriginal=$ToolFoundationOriginal;
    installedOriginal=$InstalledFoundationOriginal;loadedOriginal=$LoadedFoundationOriginal;namespaceOriginal=$NamespaceFoundationOriginal;
    principalOriginal=$PrincipalFoundationOriginal;capabilityOriginal=$CapabilityFoundationOriginal;allocationOriginal=$AllocationFoundationOriginal;
    policyOriginal=$PolicyFoundationOriginal;
    rootPathOriginal=$InputRootPath;rootBytesOriginal=$InputRootBytes;rootSha256Original=$InputRootSha256;
    sharedRootOriginal=$SharedSourceRoot;actionOriginal=$Action;receiverOriginal=$p.receiver;
    hostKeyOriginal=$p.key;deadlineTicksOriginal=$p.originalDeadlineTicks;deadlineResponseOriginal=$p.deadlineResponse;
    attempted=$false;returned=$false;exceptionOriginal=$null;terminal=$false;resultOriginal=$null}
  $p.qualificationCarrierConstructionOriginal=$construction
  try{
    $construction.attempted=$true
    $construction.resultOriginal=[Cli39QualificationStartup32]::new($p,$StartupReservationOriginal,$StartupLifetimeOriginal,
      $SourceFoundationOriginal,$ToolFoundationOriginal,$InstalledFoundationOriginal,$LoadedFoundationOriginal,
      $NamespaceFoundationOriginal,$PrincipalFoundationOriginal,$CapabilityFoundationOriginal,$AllocationFoundationOriginal,
      $PolicyFoundationOriginal,$InputRootPath,$InputRootBytes,$InputRootSha256,$SharedSourceRoot,$Action)
    $construction.returned=$true;return $construction.resultOriginal
  }catch{$construction.exceptionOriginal=$_;throw}finally{$construction.terminal=$true}
}

function Assert-Cli39QualificationStartupCustody32 {
  param([Parameter(Mandatory)]$StartupOriginal)
  if($StartupOriginal.GetType() -ne [Cli39QualificationStartup32]){throw 'Actual fixed startup carrier required.'}
  $s=$StartupOriginal;$p=$s.HostOwnerOriginal;$j=$p.qualificationCarrierConstructionOriginal
  if($null -eq $j -or -not $j.attempted -or -not $j.returned -or -not $j.terminal -or
      $null -ne $j.exceptionOriginal -or -not $s.ConstructorTerminal -or $null -ne $s.ConstructorFailure){
    throw 'Completed original startup construction required.'
  }
  # This preserves supplied custody only. It does not qualify the supplied facts.
  foreach($pair in @(@('hostOwnerOriginal','HostOwnerOriginal'),@('reservationOriginal','StartupReservationOriginal'),
      @('lifetimeOriginal','StartupLifetimeOriginal'),@('sourceOriginal','SourceFoundationOriginal'),
      @('toolOriginal','ToolFoundationOriginal'),@('installedOriginal','InstalledFoundationOriginal'),
      @('loadedOriginal','LoadedFoundationOriginal'),@('namespaceOriginal','NamespaceFoundationOriginal'),
      @('principalOriginal','PrincipalFoundationOriginal'),@('capabilityOriginal','CapabilityFoundationOriginal'),
      @('allocationOriginal','AllocationFoundationOriginal'),@('policyOriginal','PolicyFoundationOriginal'),
      @('receiverOriginal','ReceiverOriginal'),@('deadlineResponseOriginal','DeadlineResponseOriginal'))){
    if(-not [object]::ReferenceEquals($j[$pair[0]],$s.($pair[1]))){throw 'Startup original reference changed after construction.'}
  }
  if(-not [object]::ReferenceEquals($j.resultOriginal,$s) -or
      -not [object]::ReferenceEquals($p.qualificationStartupOriginal,$s) -or
      -not [object]::ReferenceEquals($p.receiver,$j.receiverOriginal) -or
      -not [object]::ReferenceEquals($p.deadlineResponse,$j.deadlineResponseOriginal) -or
      $j.rootPathOriginal -cne $s.InputRootPathOriginal -or $j.rootBytesOriginal -ne $s.InputRootBytesOriginal -or
      $j.rootSha256Original -cne $s.InputRootSha256Original -or $j.sharedRootOriginal -cne $s.SharedSourceRootOriginal -or
      $j.actionOriginal -cne $s.ActionOriginal -or $j.hostKeyOriginal -cne $s.HostKeyOriginal -or
      $j.hostKeyOriginal -cne $p.key -or $j.deadlineTicksOriginal -ne $s.DeadlineTicksOriginal -or
      $j.deadlineTicksOriginal -ne $p.originalDeadlineTicks){throw 'Startup invocation pins changed after construction.'}
}

function Invoke-Cli39QualificationFromStartup32 {
  param([Parameter(Mandatory)]$StartupOriginal)
  if($StartupOriginal.GetType() -ne [Cli39QualificationStartup32]){throw 'Actual fixed local startup carrier required; no reconstructed map.'}
  $s=$StartupOriginal;$p=$s.HostOwnerOriginal
  Assert-Cli39QualificationStartupCustody32 -StartupOriginal $s
  if(-not $s.ConstructorTerminal -or $null -ne $s.ConstructorFailure -or $null -ne $s.CallerOriginal -or
      $null -eq $p.qualificationCarrierConstructionOriginal -or -not $p.qualificationCarrierConstructionOriginal.returned -or
      -not $p.qualificationCarrierConstructionOriginal.terminal -or
      -not [object]::ReferenceEquals($p.qualificationCarrierConstructionOriginal.resultOriginal,$s) -or
      -not [object]::ReferenceEquals($p.qualificationCarrierConstructionOriginal.reservationOriginal,$s.StartupReservationOriginal) -or
      -not [object]::ReferenceEquals($p.qualificationStartupOriginal,$s) -or
      -not [object]::ReferenceEquals($p.receiver,$s.ReceiverOriginal) -or $p.key -cne $s.HostKeyOriginal -or
      $p.originalDeadlineTicks -ne $s.DeadlineTicksOriginal -or
      -not [object]::ReferenceEquals($p.deadlineResponse,$s.DeadlineResponseOriginal)){
    throw 'SAME once-only actual containing startup reference joins required.'
  }
  $call=@{startupOriginal=$s;hostOwnerOriginal=$p;reservationOriginal=$s.StartupReservationOriginal;
    attempted=$false;returned=$false;exceptionOriginal=$null;terminal=$false;resultOriginal=$null}
  $s.CallerOriginal=$call
  try{
    $call.attempted=$true
    $call.resultOriginal=New-Cli39QualificationCoordinator32 -StartupOriginal $s `
      -InputRootPath $s.InputRootPathOriginal -InputRootBytes $s.InputRootBytesOriginal `
      -InputRootSha256 $s.InputRootSha256Original -SharedSourceRoot $s.SharedSourceRootOriginal `
      -AbsoluteDeadlineTicks $s.DeadlineTicksOriginal -Action $s.ActionOriginal
    $call.returned=$true;return $call.resultOriginal
  }catch{$call.exceptionOriginal=$_;throw}finally{$call.terminal=$true}
}
