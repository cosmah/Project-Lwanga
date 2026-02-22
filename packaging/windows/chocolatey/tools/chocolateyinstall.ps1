# Chocolatey install script for Lwanga
$ErrorActionPreference = 'Stop'

$packageName = 'lwanga'
$toolsDir = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"
$url64 = 'https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.0/lwanga-1.0.0-windows-x64-installer.exe'

$packageArgs = @{
  packageName   = $packageName
  fileType      = 'exe'
  url64bit      = $url64
  silentArgs    = '/S'
  validExitCodes= @(0)
  softwareName  = 'Lwanga*'
  checksum64    = ''  # Will be filled during packaging
  checksumType64= 'sha256'
}

Install-ChocolateyPackage @packageArgs
