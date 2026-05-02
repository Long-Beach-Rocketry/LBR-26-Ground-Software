param(
  [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path,
  [string]$OutputDir = $(Join-Path (Resolve-Path (Join-Path $PSScriptRoot '..')).Path 'installer-input'),
  [string]$ArtifactDir = $(Join-Path (Resolve-Path (Join-Path $PSScriptRoot '..')).Path 'installer-artifacts')
)

$ErrorActionPreference = 'Stop'

function Ensure-Directory([string]$Path) {
  New-Item -ItemType Directory -Force -Path $Path | Out-Null
}

function Copy-Files([string]$SourcePattern, [string]$Destination) {
  $files = Get-ChildItem -Path $SourcePattern -File -ErrorAction SilentlyContinue
  foreach ($file in $files) {
    Copy-Item $file.FullName -Destination $Destination -Force
  }
}

function Require-Path([string]$Path, [string]$Message) {
  if ([string]::IsNullOrWhiteSpace($Path) -or -not (Test-Path -LiteralPath $Path)) {
    throw $Message
  }
}

function Require-File([string]$Path, [string]$Message) {
  if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw $Message
  }
}

function Validate-InstallerInput() {
  Require-File (Join-Path $cliBinDir 'lbr_ground.exe') 'Prepared installer input is missing windows-ucrt64-x64\bin\lbr_ground.exe.'
  Require-File (Join-Path $cliBinDir 'lbr_hil_runner.exe') 'Prepared installer input is missing windows-ucrt64-x64\bin\lbr_hil_runner.exe.'
  Require-File (Join-Path $guiBinDir 'lbr-ground-gui.exe') 'Prepared installer input is missing gui-windows-x64\bin\lbr-ground-gui.exe.'
  Require-File (Join-Path $OutputDir 'docs\html\index.html') 'Prepared installer input is missing docs\html\index.html.'
}

$artifactCliZip = Join-Path $ArtifactDir 'cli\lbr-nightly-windows-ucrt64-x64.zip'
$artifactGuiZip = Join-Path $ArtifactDir 'gui\lbr-nightly-gui-windows-x64.zip'
$artifactDocsTar = Join-Path $ArtifactDir 'docs\lbr-nightly-docs-html.tar.gz'
$cliBinDir = Join-Path $OutputDir 'windows-ucrt64-x64\bin'
$guiBinDir = Join-Path $OutputDir 'gui-windows-x64\bin'

if ((Test-Path $artifactCliZip) -and (Test-Path $artifactGuiZip) -and (Test-Path $artifactDocsTar)) {
  Ensure-Directory $OutputDir
  Ensure-Directory (Join-Path $OutputDir 'docs')

  Expand-Archive -Path $artifactCliZip -DestinationPath $OutputDir -Force
  Expand-Archive -Path $artifactGuiZip -DestinationPath $OutputDir -Force
  tar -xzf $artifactDocsTar -C (Join-Path $OutputDir 'docs')
  Validate-InstallerInput
  return
}

$docsHtmlDir = $null

foreach ($candidate in @(
  (Join-Path $RepoRoot 'build\docs\doxygen\html'),
  (Join-Path $RepoRoot 'build-linux-features\docs\doxygen\html'),
  (Join-Path $RepoRoot 'build-linux\docs\doxygen\html')
)) {
  if (Test-Path $candidate) {
    $docsHtmlDir = $candidate
    break
  }
}

Require-Path (Join-Path $RepoRoot 'build\LoRa\lbr_ground.exe') 'Missing build\LoRa\lbr_ground.exe; build the CLI target first.'
Require-Path (Join-Path $RepoRoot 'build\LoRa\lbr_hil_runner.exe') 'Missing build\LoRa\lbr_hil_runner.exe; build the HIL runner target first.'
Require-Path (Join-Path $RepoRoot 'dist\lbr-ground-gui.exe') 'Missing dist\lbr-ground-gui.exe; package the GUI with PyInstaller first.'
Require-Path $docsHtmlDir 'Missing generated Doxygen HTML; build docs first.'

Ensure-Directory $cliBinDir
Ensure-Directory $guiBinDir
Ensure-Directory (Join-Path $OutputDir 'docs')

Copy-Item (Join-Path $RepoRoot 'build\LoRa\lbr_ground.exe') -Destination $cliBinDir -Force
Copy-Item (Join-Path $RepoRoot 'build\LoRa\lbr_hil_runner.exe') -Destination $cliBinDir -Force
Copy-Files (Join-Path $RepoRoot 'build\LoRa\*.dll') $cliBinDir

Copy-Item (Join-Path $RepoRoot 'dist\lbr-ground-gui.exe') -Destination $guiBinDir -Force
Copy-Item $docsHtmlDir -Destination (Join-Path $OutputDir 'docs') -Recurse -Force

Validate-InstallerInput
