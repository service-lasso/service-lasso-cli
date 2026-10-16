[CmdletBinding()]
param(
  [Parameter(Mandatory)]$StartupOriginal,
  [Parameter(Mandatory)][string]$InputRootPath,
  [Parameter(Mandatory)][long]$InputRootBytes,
  [Parameter(Mandatory)][string]$InputRootSha256,
  [Parameter(Mandatory)][string]$SharedSourceRoot,
  [Parameter(Mandatory)][long]$AbsoluteDeadlineTicks,
  [Parameter(Mandatory)][ValidateSet('materialize','compare','keyring','key-policy','checksum-signature','publisher')][string]$Action
)
# The genuine official host/exact command/complete startup reservation and
# controlling native owner precede parameter binding and the first statement.
# Neither this import nor any parsed root supplies that external admission.
$ErrorActionPreference='Stop'
$script:Cli39QualificationEntryOriginal=@{pathOriginal=$PSCommandPath;
  coordinatorSourceOriginal=(Join-Path $PSScriptRoot 'prerequisite-coordinator32.ps1');
  importAttempted=$false;importReturned=$false;importExceptionOriginal=$null;
  invocationAttempted=$false;invocationReturned=$false;invocationExceptionOriginal=$null;
  hostOwnerOriginal=$null;terminal=$false}
$entry=$script:Cli39QualificationEntryOriginal
try{
  $entry.importAttempted=$true
  try{. $entry.coordinatorSourceOriginal;$entry.importReturned=$true}
  catch{$entry.importExceptionOriginal=$_;throw}
  $entry.invocationAttempted=$true
  try{
    if($StartupOriginal.GetType() -ne [Cli39QualificationStartup32] -or
        $InputRootPath -cne $StartupOriginal.InputRootPathOriginal -or $InputRootBytes -ne $StartupOriginal.InputRootBytesOriginal -or
        $InputRootSha256 -cne $StartupOriginal.InputRootSha256Original -or $SharedSourceRoot -cne $StartupOriginal.SharedSourceRootOriginal -or
        $AbsoluteDeadlineTicks -ne $StartupOriginal.DeadlineTicksOriginal -or $Action -cne $StartupOriginal.ActionOriginal){
      throw 'SAME local startup carrier and exact entry metadata required; process argument strings cannot transport it.'
    }
    $entry.hostOwnerOriginal=Invoke-Cli39QualificationFromStartup32 $StartupOriginal
    $entry.invocationReturned=$true
  }catch{
    $entry.invocationExceptionOriginal=$_
    $entry.hostOwnerOriginal=$script:Cli39QualificationCoordinatorOriginal
    throw
  }
}finally{$entry.terminal=$true}
# No receipt/boolean publication turns the retained original into admission.
