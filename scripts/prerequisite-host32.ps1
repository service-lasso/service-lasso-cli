[CmdletBinding()]
param(
  [Parameter(Mandatory)][string]$InputRootPath,
  [Parameter(Mandatory)][string]$InputRootSha256,
  [Parameter(Mandatory)][long]$InputRootBytes,
  [Parameter(Mandatory)][string]$SharedSourceRoot,
  [Parameter(Mandatory)][string]$PipeName,
  [Parameter(Mandatory)][string]$InvocationToken,
  [Parameter(Mandatory)][long]$AbsoluteDeadlineTicks,
  [Parameter(Mandatory)][ValidateSet('operation-host','retained-caller')][string]$Role,
  [Parameter(Mandatory)][ValidateSet('materialize','compare','keyring','key-policy','checksum-signature','publisher')][string]$Action
)
# Fixed CLI retained-caller -> operation-host graph, no installed provider/service. Its
# exact startup native owner/peer association is part of complete input/source
# admission. This file alone cannot qualify that outer ownership boundary.
$ErrorActionPreference = 'Stop'
# Minimal original receiver exists before either source import. Its literal
# dictionary is the same receiver representation, not authority borrowed from
# a helper which has not loaded. Parser/runtime loss before first script effects
# remains externally observed UNKNOWN, never a fabricated managed original.
$receiver = @{ frames = [Collections.Generic.Dictionary[string,object]]::new([StringComparer]::Ordinal) }
$hostOwner = @{ receiver = $receiver; responseWritten = $false; responseOriginal = $null; responseReturned = $false; responseFrame = $null; responseTask = $null; controlOriginal = $null; ended = $false; originalDeadline = $AbsoluteDeadlineTicks; server = $null; retentionGate = $null; serverAttempted = $false; gateAttempted = $false; gateDisposeOriginal = $null; disposal = $null; disposalOriginal = $null; requestFrame = $null; requestReads = $null; requestReadCount = 0 }
# Publish the enclosing host owner before pipe/event constructors can acquire
# native resources. Partial constructor originals remain in this same graph.
$hostOwner.key='host-control'; $hostOwner.consumerOriginal=$null; $hostOwner.originalDeadlineTicks=$AbsoluteDeadlineTicks; $hostOwner.deadlineReported=$false; $hostOwner.deadlineResponseOriginal=$null; $hostOwner.deadlineOriginal=@{phase='original-root-deadline';deadline=$AbsoluteDeadlineTicks}; $hostOwner.deadlineResponse=$null
$receiver.frames.Add('host-control', $hostOwner)
$server = $null
$retentionGate = $null
$token = $null
$hostOwner.inputRead = @{ attempted = $false; value = $null; returned = $false; original = $null }
$hostOwner.startupRecipes = @{ attempted = $false; returned = $false; terminal = $false; original = $null; terminalLifetime = 'host-process' }
$hostOwner.sourceLoads = @{
  input = @{ attempted = $false; returned = $false; original = $null }
  owner = @{ attempted = $false; returned = $false; original = $null }
}

function New-ProducerHostReadRoster {
  # Every positive read consumes at least one of the exact forty frame bytes.
  $records = [object[]]::new(40)
  for ($index = 0; $index -lt 40; $index++) { $records[$index] = @{ attempted = $false; task = $null; returned = $false; value = $null; original = $null } }
  return $records
}

function Write-ProducerHostResponse([uint32]$Disposition) {
  if ($hostOwner.responseWritten) { return }
  # Fixed original control frame: magic NPR1, disposition U32, invocation32,
  # original absolute deadline I64. Raw failures never enter this projection.
  [byte[]]$frame = [byte[]]::new(48)
  [Array]::Copy([Text.Encoding]::ASCII.GetBytes('NPR1'), 0, $frame, 0, 4)
  [Array]::Copy([BitConverter]::GetBytes($Disposition), 0, $frame, 4, 4)
  [Array]::Copy($token, 0, $frame, 8, 32)
  [Array]::Copy([BitConverter]::GetBytes($AbsoluteDeadlineTicks), 0, $frame, 40, 8)
  $hostOwner.responseFrame = $frame
  # Mark attempted before the write; no retry can manufacture a response.
  $hostOwner.responseWritten = $true
  try { $hostOwner.responseTask = $server.WriteAsync($frame, 0, $frame.Length) }
  catch { $hostOwner.responseOriginal = $_; throw }
}

try {
  $hostOwner.sourceLoads.input.attempted = $true
  try {
    . (Join-Path $PSScriptRoot 'prerequisite-managed-worker26.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-members32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-native-query32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-output-query32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-file-query32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-member-file-query32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-namespace-recipe32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-namespace-worker32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-namespace32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-process32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-image32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-modules32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-loaded-recipes32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-loaded-worker32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-loaded32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-loader-semantics32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-child-bindings32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-profile32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-native-root32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-input32.ps1')
    . (Join-Path $PSScriptRoot 'publisher-policy32.ps1')
    . (Join-Path $PSScriptRoot 'node-release-verification-consumer26.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-consumer-structure32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-namespace-roles32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-pipe-peer32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-routing32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-lineage32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-principal-selector32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-principal-module32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-parent-recipes32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-parent-observation32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-principal-token32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-owned-birth-worker32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-owned-birth32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-derived-root-worker32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-derived-root-disposal32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-node-projection32.ps1')
    . (Join-Path $PSScriptRoot 'publisher-projection32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-node-sequence32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-static-projection32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-derived-root32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-derived-host32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-caller32.ps1')
    . (Join-Path $PSScriptRoot 'prerequisite-action32.ps1')
    $hostOwner.sourceLoads.input.returned = $true }
  catch { $hostOwner.sourceLoads.input.original = $_; throw }
  $hostOwner.sourceLoads.owner.attempted = $true
  try { . (Join-Path $SharedSourceRoot 'windows-native-producer-owner.ps1'); $hostOwner.sourceLoads.owner.returned = $true }
  catch { $hostOwner.sourceLoads.owner.original = $_; throw }
  # BOTH roles initialize the SAME shared ABI and durable sink recipes before
  # NPH or root work. A genuine preexisting external startup owner observes
  # this original deadline and retains this host on a constructor stall/failure.
  # Parsed root/profile fields cannot fund or authenticate these partials.
  $hostOwner.startupRecipes.attempted = $true
  try {
    Initialize-CoreNativeAbiRecipes $hostOwner
    $null=Initialize-Cli39NamespaceRecipes32 $hostOwner
    $null=Initialize-Cli39PrincipalRecipes32 $hostOwner
    $null=Initialize-Cli39ParentRecipes32 $hostOwner
    $null=Initialize-Cli39LoadedRecipes32 $hostOwner
    $hostOwner.startupRecipes.returned = $true
  } catch { $hostOwner.startupRecipes.original = $_; throw }
  finally { $hostOwner.startupRecipes.terminal = $true }
  if($InvocationToken -cnotmatch '^[0-9a-f]{64}$' -or
      $PipeName -cnotmatch '^service-lasso-native-producer-[0-9a-f]{32}$' -or
      -not [IO.Path]::IsPathFullyQualified($SharedSourceRoot) -or
      -not [IO.Path]::IsPathFullyQualified($InputRootPath) -or
      $InputRootBytes -le 0 -or $InputRootBytes -ge [int]::MaxValue -or
      $InputRootSha256 -cnotmatch '^[0-9a-f]{64}$' -or $AbsoluteDeadlineTicks -le 0){throw 'Exact original fixed startup routing/control inputs required.'}
  [byte[]]$token = [Convert]::FromHexString($InvocationToken)
  $hostOwner.requestReads = New-ProducerHostReadRoster
  $hostOwner.serverAttempted = $true
  $server = [IO.Pipes.NamedPipeServerStream]::new($PipeName, [IO.Pipes.PipeDirection]::InOut, 1, [IO.Pipes.PipeTransmissionMode]::Byte, [IO.Pipes.PipeOptions]'Asynchronous,CurrentUserOnly,FirstPipeInstance', 4096, 4096)
  $hostOwner.server = $server
  $hostOwner.gateAttempted = $true
  $retentionGate = [Threading.ManualResetEvent]::new($false)
  $hostOwner.retentionGate = $retentionGate
  $connection = $server.WaitForConnectionAsync()
  $hostOwner.connection = $connection
  while (-not $connection.IsCompleted) {
    if ([Diagnostics.Stopwatch]::GetTimestamp() -ge $AbsoluteDeadlineTicks) { throw 'Original host startup deadline expired.' }
    [Threading.Thread]::Sleep(1)
  }
  $connection.GetAwaiter().GetResult()
  [byte[]]$request = [byte[]]::new(40)
  $hostOwner.requestFrame = $request
  $offset = 0
  while ($offset -lt $request.Length) {
    $record = $hostOwner.requestReads[$hostOwner.requestReadCount]
    $hostOwner.requestReadCount++
    $record.attempted = $true
    try { $record.task = $server.ReadAsync($request, $offset, $request.Length - $offset) }
    catch { $record.original = $_; throw }
    $read = $record.task
    while (-not $read.IsCompleted) {
      if ([Diagnostics.Stopwatch]::GetTimestamp() -ge $AbsoluteDeadlineTicks) { throw 'Original control request deadline expired.' }
      [Threading.Thread]::Sleep(1)
    }
    try { $count = $read.GetAwaiter().GetResult(); $record.value = $count; $record.returned = $true }
    catch { $record.original = $_; throw }
    if ($count -eq 0) { throw 'Original caller closed before complete control request.' }
    $offset += $count
  }
  $hostOwner.requestValidationOriginal=@{frameOriginal=$request;readsOriginal=$hostOwner.requestReads;
    readCountOriginal=$hostOwner.requestReadCount;serverOriginal=$server;tokenOriginal=$token;
    pipeNameOriginal=$PipeName;invocationTokenOriginal=$InvocationToken;roleOriginal=$Role;actionOriginal=$Action;
    inputRootPathOriginal=$InputRootPath;inputRootBytesOriginal=$InputRootBytes;inputRootSha256Original=$InputRootSha256;
    sharedSourceRootOriginal=$SharedSourceRoot;deadlineTicks=$AbsoluteDeadlineTicks;returned=$false}
  if ([Text.Encoding]::ASCII.GetString($request, 0, 4) -cne 'NPH1' -or [BitConverter]::ToUInt32($request, 36) -ne 1) { throw 'Malformed host request.' }
  $difference = 0
  for ($index = 0; $index -lt 32; $index++) { $difference = $difference -bor ($request[4 + $index] -bxor $token[$index]) }
  if ($difference -ne 0) { throw 'Host request invocation identity mismatch.' }
  $hostOwner.requestValidationOriginal.returned=$true
  $hostOwner.deadlineResponse={param($originalOwner) Write-ProducerHostResponse 2}.GetNewClosure()
  $hostOwner.inputRead.attempted=$true
  try {
    $rootCapture=Read-Cli39PrerequisiteRootStructure32 -HostOwnerOriginal $hostOwner -InputRootPath $InputRootPath -InputRootBytes $InputRootBytes -InputRootSha256 $InputRootSha256 -Action $Action -RootRole $Role
    $hostOwner.inputRead.returned=$true
  } catch {$hostOwner.inputRead.original=$_;throw}
  $null=Read-Cli39ParentLineage32 $hostOwner $Role
  $null=Read-Cli39StaticFoundationProjection32 $hostOwner
  $null=Read-Cli39ConsumerStructure32 $rootCapture $hostOwner
  if($Action -ceq 'checksum-signature'){$null=Read-Cli39ConsumerStructure32 $rootCapture $hostOwner -KeyPolicyPrerequisite}
  $null=Read-Cli39PrincipalSelector32 $hostOwner
  $null=Read-Cli39RootRouting32 $hostOwner $Role $PipeName $InvocationToken $SharedSourceRoot
  $null=Read-Cli39NamespaceRoles32 $hostOwner
  if($Action -ceq 'publisher'){
    $null=Read-Cli39PublisherPolicyOriginal32 $rootCapture $hostOwner
    $null=Read-Cli39PublisherFoundationSelector32 $hostOwner
  }
  $null=Observe-Cli39PrerequisiteNativeRoot32 $hostOwner
  $null=Join-Cli39ChildStaticObservations32 $hostOwner
  $null=Join-Cli39ChildDerivedTransport32 $hostOwner
  $null=Join-Cli39ChildLoaderSemantics32 $hostOwner
  $null=Join-Cli39ChildCapability32 $hostOwner
  $null=Join-Cli39ChildAllocation32 $hostOwner
  $null=Complete-Cli39ChildFoundationBindings32 $hostOwner
  $null=Join-Cli39ChildNodePolicy32 $hostOwner
  $null=Join-Cli39ChildNodeLoader32 $hostOwner
  $null=Join-Cli39ChildPublisherPolicy32 $hostOwner
  # Root structure is not native authority. Complete actual startup/source/
  # loaded/principal/capability/namespace correspondence is under the genuine
  # external whole-input foundation precondition, never JSON authority.
  # Never issue any action from a JSON boolean or this structural result alone.
  if($null -eq $rootCapture.nativeAssociationOriginals){throw 'Complete actual CLI root native correspondence absent; originals remain retained.'}
  $context = @{ receiver = $receiver; deadlineResponse = $hostOwner.deadlineResponse }
  $hostOwner.context = $context
  $hostOwner.actionOriginal=@{role=$Role;action=$Action;inputOriginal=$rootCapture;attempted=$false;returned=$false;resultOriginal=$null;exceptionOriginal=$null}
  try {
    $hostOwner.actionOriginal.attempted=$true
    if($Role -ceq 'retained-caller'){
      $hostOwner.actionOriginal.resultOriginal=Invoke-Cli39OwnedPrerequisiteHost32 -HostOwnerOriginal $hostOwner -AbiOriginal $rootCapture.abiOriginal -Receiver $receiver -AdmittedInputOriginal $rootCapture.admittedInputOriginal -ReservationOriginal $rootCapture.callerReservationOriginal -DeadlineResponse $context.deadlineResponse
    }else{
      $hostOwner.actionOriginal.resultOriginal=Invoke-Cli39FixedPrerequisiteAction32 -RootCaptureOriginal $rootCapture -HostOwnerOriginal $hostOwner -DeadlineResponse $context.deadlineResponse
    }
    $hostOwner.actionOriginal.returned=$true
  }catch{$hostOwner.actionOriginal.exceptionOriginal=$_;throw}
  if (-not $hostOwner.responseWritten) { Write-ProducerHostResponse 1 }
  $hostOwner.ended = $true
} catch {
  $hostOwner.controlOriginal = $_
  if (-not $hostOwner.responseWritten -and $null -ne $server -and $server.IsConnected) {
    try { Write-ProducerHostResponse 3 } catch { $hostOwner.secondaryResponseOriginal = $_ }
  }
}

# A request authorizes the SAME local receiver to dispose its originals; it is
# not a claim to transfer arbitrary live objects through this byte stream.
try {
  $hostOwner.disposal = @{ request = [byte[]]::new(40); requestReads = $null; requestReadCount = 0; used = 0; authenticated = $false; eligible = $false; reply = $null; replyTask = $null; replyAttempted = $false; replyReturned = $false; replyOriginal = $null; receipt = [byte[]]::new(40); receiptReads = $null; receiptReadCount = 0; receiptUsed = 0; pipeDisposeTask = $null; pipeDisposeAttempted = $false; pipeDisposeReturned = $false; pipeDisposeOriginal = $null; original = $null; shutdownGranted = $false }
  $hostOwner.disposal.requestReads = New-ProducerHostReadRoster
  $hostOwner.disposal.receiptReads = New-ProducerHostReadRoster
  if ($null -eq $hostOwner.responseTask) { throw 'No original response task exists; control disposition remains unknown.' }
  while (-not $hostOwner.responseTask.IsCompleted) { [Threading.Thread]::Sleep(1) }
  try { $hostOwner.responseTask.GetAwaiter().GetResult(); $hostOwner.responseReturned = $true }
  catch { $hostOwner.responseOriginal = $_; throw }
  $disposal = $hostOwner.disposal
  while ($disposal.used -lt $disposal.request.Length) {
    $record = $disposal.requestReads[$disposal.requestReadCount]
    $disposal.requestReadCount++
    $record.attempted = $true
    try {
      $record.task = $server.ReadAsync($disposal.request, $disposal.used, $disposal.request.Length - $disposal.used)
      while (-not $record.task.IsCompleted) { [Threading.Thread]::Sleep(1) }
      $count = $record.task.GetAwaiter().GetResult(); $record.value = $count; $record.returned = $true
    } catch { $record.original = $_; throw }
    if ($count -eq 0) { throw 'Caller disconnected without receiver disposal authorization.' }
    $disposal.used += $count
  }
  if ([Text.Encoding]::ASCII.GetString($disposal.request, 0, 4) -cne 'NPA1' -or [BitConverter]::ToUInt32($disposal.request, 36) -ne 2) { throw 'Malformed receiver disposal request.' }
  $difference = 0
  for ($index = 0; $index -lt 32; $index++) { $difference = $difference -bor ($disposal.request[4 + $index] -bxor $token[$index]) }
  if ($difference -ne 0) { throw 'Receiver disposal invocation identity mismatch.' }
  $disposal.authenticated = $true
  if($hostOwner.inputRead.value.ContainsKey('derivedRootOriginal') -and
      $null -ne $hostOwner.inputRead.value.derivedRootOriginal.writeWorkerOriginal -and
      $hostOwner.inputRead.value.derivedRootOriginal.writeWorkerOriginal.startAttempted -and
      $null -eq $hostOwner.inputRead.value.derivedRootOriginal.disposalOriginal){
    $null=Invoke-Cli39DerivedRootDisposal32 $hostOwner.inputRead.value.derivedRootOriginal
  }
  $hostOwner.rootDisposalOriginal=Invoke-Cli39RootDisposal32 -HostOwnerOriginal $hostOwner
  if(-not (Test-Cli39RootClosure32 -HostOwnerOriginal $hostOwner)){throw 'Original startup reader still has actual live/uncertain resources.'}
  $disposal.eligible = Test-NativeProducerReceiverPhysicalClosure -Receiver $receiver -HostOwner $hostOwner
  if (-not $disposal.eligible) { throw 'Actual original receiver resources remain live or uncertain.' }
  $disposal.reply = [byte[]]::new(40)
  [Array]::Copy([Text.Encoding]::ASCII.GetBytes('NPD1'), 0, $disposal.reply, 0, 4)
  [Array]::Copy($token, 0, $disposal.reply, 4, 32)
  [Array]::Copy([BitConverter]::GetBytes([uint32]1), 0, $disposal.reply, 36, 4)
  $disposal.replyAttempted = $true
  try {
    $disposal.replyTask = $server.WriteAsync($disposal.reply, 0, $disposal.reply.Length)
    while (-not $disposal.replyTask.IsCompleted) { [Threading.Thread]::Sleep(1) }
    $disposal.replyTask.GetAwaiter().GetResult()
    $disposal.replyReturned = $true
  } catch { $disposal.replyOriginal = $_; throw }
  # Wait for actual receipt of the grant before closing the server endpoint;
  # an OS-buffered completed write alone does not prove client acceptance.
  while ($disposal.receiptUsed -lt $disposal.receipt.Length) {
    $record = $disposal.receiptReads[$disposal.receiptReadCount]
    $disposal.receiptReadCount++
    $record.attempted = $true
    try {
      $record.task = $server.ReadAsync($disposal.receipt, $disposal.receiptUsed, $disposal.receipt.Length - $disposal.receiptUsed)
      while (-not $record.task.IsCompleted) { [Threading.Thread]::Sleep(1) }
      $count = $record.task.GetAwaiter().GetResult(); $record.value = $count; $record.returned = $true
    } catch { $record.original = $_; throw }
    if ($count -eq 0) { throw 'Caller disconnected before actual disposal grant receipt.' }
    $disposal.receiptUsed += $count
  }
  if ([Text.Encoding]::ASCII.GetString($disposal.receipt, 0, 4) -cne 'NPE1' -or [BitConverter]::ToUInt32($disposal.receipt, 36) -ne 3) { throw 'Malformed disposal grant receipt.' }
  $difference = 0
  for ($index = 0; $index -lt 32; $index++) { $difference = $difference -bor ($disposal.receipt[4 + $index] -bxor $token[$index]) }
  if ($difference -ne 0) { throw 'Disposal grant receipt invocation identity mismatch.' }
  $disposal.pipeDisposeAttempted = $true
  try {
    if(-not (Test-Cli39RootPipePeerClosure32 $hostOwner)){throw 'Actual control-server peer query still borrows the server endpoint.'}
    $disposal.pipeDisposeTask = $server.DisposeAsync().AsTask()
    while (-not $disposal.pipeDisposeTask.IsCompleted) { [Threading.Thread]::Sleep(1) }
    $disposal.pipeDisposeTask.GetAwaiter().GetResult()
    $disposal.pipeDisposeReturned = $true
  } catch { $disposal.pipeDisposeOriginal = $_; throw }
  if ($null -ne $retentionGate) {
    $hostOwner.gateDisposeAttempted = $true
    try { $retentionGate.Dispose(); $hostOwner.gateDisposed = $true }
    catch { $hostOwner.gateDisposeOriginal = $_; throw }
  }
  # This is the receiver's actual grant after original closure and terminal IO.
  # The separate caller/native owner must still join the host root/tree/EOF.
  $disposal.shutdownGranted = $true
} catch { $hostOwner.disposalOriginal = $_; if ($null -ne $hostOwner.disposal) { $hostOwner.disposal.original = $_ } }
if ($null -eq $hostOwner.disposal -or -not $hostOwner.disposal.shutdownGranted) {
  # Unknown lifetime stays charged. No timeout extension produces late success,
  # no handle/event constructor is retried, and caller death is not an ACK.
  if ($null -ne $retentionGate -and -not $hostOwner.Contains('gateDisposeAttempted')) { $null = $retentionGate.WaitOne() }
  else { [Threading.Thread]::Sleep([Threading.Timeout]::Infinite) }
}

